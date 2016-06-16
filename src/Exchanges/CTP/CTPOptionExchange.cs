using CTP;
using LittleQuant.Core;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Threading;
using YamlDotNet.Serialization;
using YamlDotNet.Serialization.NamingConventions;

namespace LittleQuant.Exchanges.CTP
{
    public class Config
    {
        public string BrokerId { get; set; }
        public string AuthCode { get; set; }
        public string UserProductInfo { get; set; }
        public bool NeedAuth { get; set; }
        public string Account { get; set; }
        public string Password { get; set; }
        public string[] TradingFrontends { get; set; }
        public string[] MarketFrontends { get; set; }
    }

    public class CTPOptionExchange : IOptionExchange
    {
        private CTPTraderAdapter _trade;
        private CTPMDAdapter _market;
        private Config _config;
        private SessionState _session = new SessionState();
        private AutoResetEvent _asyncWaiter = new AutoResetEvent(false);  // 用于初始化时等待异步请求的响应
        private bool _initialized = false;
        private IDictionary<string, OptionContract> _contracts = new Dictionary<string, OptionContract>();
        private IDictionary<string, Market> _markets = new Dictionary<string, Market>();
        private OptionAccount _account = new OptionAccount();

        private ICollection<ThostFtdcOrderField> _orders = new List<ThostFtdcOrderField>();
        private IDictionary<int, ICollection<ThostFtdcInvestorPositionField>> _positions = new Dictionary<int, ICollection<ThostFtdcInvestorPositionField>>();  // 键是reqeust id

        private CTPQryReqScheduler _ctpReqstr = new CTPQryReqScheduler();

        public void Initialize()
        {
            var configPath = Path.Combine(Path.GetDirectoryName(this.GetType().Assembly.Location), "CTP.yaml");
            if (!File.Exists(configPath))
                throw new Exception(string.Format("{0}初始化失败，配置文件{1}不存在", this.GetType().FullName, configPath));

            using (var reader = new StreamReader(configPath))
            {
                var deserializer = new Deserializer(namingConvention: new UnderscoredNamingConvention());
                this._config = deserializer.Deserialize<Config>(reader);
            }

            // ================================================================================
            // 交易初始化
            // ================================================================================
            this._trade = new CTPTraderAdapter(Path.GetDirectoryName(this.GetType().Assembly.Location) + '\\');
            this._trade.OnFrontConnected += _trade_OnFrontConnected;
            this._trade.OnRspUserLogin += _trade_OnRspUserLogin;
            this._trade.OnRspAuthenticate += _trade_OnRspAuthenticate;
            this._trade.OnRspQryInstrument += _trade_OnRspQryInstrument;
            this._trade.OnRspError += OnRspError;
            this._trade.OnRspQryTradingAccount += _trade_OnRspQryTradingAccount;
            this._trade.OnRspQryOrder += _trade_OnRspQryOrder;
            this._trade.OnRspOrderInsert += (order, rsp, _3, _4) =>
            {
                if (rsp.ErrorID != 0)
                    this.OnLog($"ERROR: OnRspOrderInsert: {nameof(rsp.ErrorID) + rsp.ErrorID}, {nameof(rsp.ErrorMsg) + rsp.ErrorMsg}");
            };
            this._trade.OnRtnOrder += _trade_OnRtnOrder;
            this._trade.OnRtnTrade += _ =>
            {
                this.QueryPositions();
                this.OnTrade(new Trade { InstrumentID = _.InstrumentID, Price = _.Price, Qty = _.Volume });
            };
            this._trade.OnRspQryInvestorPosition += (positionField, rspField, requestId, isLast) =>
            {
                if (positionField != null)
                    this._positions[requestId].Add(positionField);
                if (isLast)
                {
                    if (rspField != null && rspField.ErrorID != 0)
                        throw new Exception($"查询持仓失败，代码：{rspField.ErrorID}，描述：{rspField.ErrorMsg}");

                    this.Account.Positions = this._positions[requestId].Select(_ => _.ToOptionPosition()).Where(_ => _.Qty > 0).ToList();
                    this._positions.Remove(requestId);
                }
            };

            foreach (var f in this._config.TradingFrontends)
                this._trade.RegisterFront(f);
            this._trade.SubscribePrivateTopic(EnumTeResumeType.THOST_TERT_QUICK);
            this._trade.Init();

            this.OnLog("等待交易登录响应。。。");
            this._asyncWaiter.WaitOne();

            // ================================================================================
            // 行情初始化
            // ================================================================================
            this._market = new CTPMDAdapter();
            this._market.OnRspError += OnRspError;
            this._market.OnFrontConnected += _market_OnFrontConnected;
            this._market.OnRspUserLogin += _market_OnRspUserLogin;
            this._market.OnRtnDepthMarketData += _market_OnRtnDepthMarketData;
            foreach (var f in this._config.MarketFrontends)
                this._market.RegisterFront(f);
            this._market.Init();

            this.OnLog("等待行情登录响应。。。");
            this._asyncWaiter.WaitOne();

            this.OnLog("查询合约并订阅行情。。。");
            this._ctpReqstr.Schedule(() => this._trade.ReqQryInstrument(new ThostFtdcQryInstrumentField { ExchangeID = "SSE" }, this._session.NextRequestID()));
            this._asyncWaiter.WaitOne();
        }

        private void _trade_OnRspAuthenticate(ThostFtdcRspAuthenticateField pRspAuthenticateField, ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            if (pRspInfo.ErrorID != 0)
                throw new Exception($"认证失败：{nameof(pRspInfo.ErrorID)} {pRspInfo.ErrorID}, {nameof(pRspInfo.ErrorMsg)} {pRspInfo.ErrorMsg}");

            this.Login();
        }

        void _trade_OnRspQryOrder(ThostFtdcOrderField pOrder, ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            if (pOrder != null)
            {
                var exists = this._orders.FirstOrDefault(_ => _.OrderSysID == pOrder.OrderSysID);
                if (exists == null)
                    this._orders.Add(pOrder);
            }
            if (bIsLast)
                this.Account.PendingOrders = this._orders.Where(_ => (_.OrderSysID ?? "").Trim().Length > 0 && _.GetOrderStatus() == OrderStatus.Pending).Select(_ => _.ToOptionOrder()).ToList();
        }

        void _trade_OnRtnOrder(ThostFtdcOrderField pOrder)
        {
            this.QueryAccount();
            var orderId = (pOrder.OrderSysID ?? "").Trim();
            var old = this.Account.PendingOrders.FirstOrDefault(_ => _.OrderID == orderId);
            if (old != null)
                this.Account.PendingOrders.Remove(old);
            if (orderId.Length > 0 && pOrder.GetOrderStatus() == OrderStatus.Pending)
                this.Account.PendingOrders.Add(pOrder.ToOptionOrder());

            var oldNative = this._orders.ToList().FirstOrDefault(_ => _.OrderSysID == orderId);
            if (oldNative != null)
                this._orders.Remove(oldNative);
            if (orderId.Length > 0)
                this._orders.Add(pOrder);
        }

        void _trade_OnRspQryTradingAccount(ThostFtdcTradingAccountField pTradingAccount, ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            if (pTradingAccount != null)
            {
                this.Account.Available = pTradingAccount.Available;
                this.Account.Total = pTradingAccount.Balance;
            }
        }

        void _market_OnRtnDepthMarketData(ThostFtdcDepthMarketDataField pDepthMarketData)
        {
            if (pDepthMarketData == null)
                return;

            var market = new Market
            {
                InstrumentID = pDepthMarketData.InstrumentID,
                PrevClose = pDepthMarketData.PreClosePrice,
                LastPrice = pDepthMarketData.LastPrice
            };

            var asks = new List<Quote>
            {
                new Quote { Price = pDepthMarketData.AskPrice1, Qty = pDepthMarketData.AskVolume1 },
                new Quote { Price = pDepthMarketData.AskPrice2, Qty = pDepthMarketData.AskVolume2 },
                new Quote { Price = pDepthMarketData.AskPrice3, Qty = pDepthMarketData.AskVolume3 },
                new Quote { Price = pDepthMarketData.AskPrice4, Qty = pDepthMarketData.AskVolume4 },
                new Quote { Price = pDepthMarketData.AskPrice5, Qty = pDepthMarketData.AskVolume5 }
            };
            var bids = new List<Quote>
            {
                new Quote { Price = pDepthMarketData.BidPrice1, Qty = pDepthMarketData.BidVolume1 },
                new Quote { Price = pDepthMarketData.BidPrice2, Qty = pDepthMarketData.BidVolume2 },
                new Quote { Price = pDepthMarketData.BidPrice3, Qty = pDepthMarketData.BidVolume3 },
                new Quote { Price = pDepthMarketData.BidPrice4, Qty = pDepthMarketData.BidVolume4 },
                new Quote { Price = pDepthMarketData.BidPrice5, Qty = pDepthMarketData.BidVolume5 }
            };
            market.SetAsks(asks.Where(_ => _.Qty > 0));
            market.SetBids(bids.Where(_ => _.Qty > 0));
            this._markets[market.InstrumentID] = market;

            if (!this._initialized && this._markets.Count() == this.Instruments.Count())
            {
                this._initialized = true;
                this._asyncWaiter.Set();
            }
        }

        void _market_OnRspUserLogin(ThostFtdcRspUserLoginField pRspUserLogin, ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            if (pRspInfo.ErrorID != 0)
                this.OnLog(string.Format("行情登录失败: 代码: {0}, 消息: {1}", pRspInfo.ErrorID, pRspInfo.ErrorMsg));
            else
                this.OnLog("行情登录成功");
            this._asyncWaiter.Set();
        }

        void _market_OnFrontConnected()
        {
            var usr = new ThostFtdcReqUserLoginField
            {
                BrokerID = this._config.BrokerId,
                UserID = this._config.Account,
                Password = this._config.Password
            };
            var ret = this._market.ReqUserLogin(usr, this._session.NextRequestID());
            if (ret != 0)
                throw new Exception(string.Format("行情登录失败，调用ReqUserLogin返回{0}", ret));
        }

        void _trade_OnRspQryInstrument(ThostFtdcInstrumentField pInstrument, ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            if (pInstrument != null)
            {
                switch (pInstrument.ProductClass)
                {
                    case EnumProductClassType.ETFOption:
                        this._contracts[pInstrument.InstrumentID] = new OptionContract
                        {
                            ID = pInstrument.InstrumentID,
                            Name = pInstrument.InstrumentName,
                            CallPut = pInstrument.OptionsType == EnumOptionsType.Call ? CallPut.Call : CallPut.Put,
                            Maturity = DateTime.ParseExact(pInstrument.ExpireDate, "yyyyMMdd", CultureInfo.InvariantCulture),
                            Strike = pInstrument.StrikePrice,
                            SubjectMatter = pInstrument.UnderlyingInstrID,
                            Size = pInstrument.VolumeMultiple
                        };
                        break;
                }
            }
            if (bIsLast)
            {
                var ret = this._market.SubscribeMarketData(this.Instruments.Select(_ => _.ID).ToArray());
            }
        }

        void OnRspError(ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            this.OnLog(string.Format("错误：id: {0}, 描述: {1}", pRspInfo.ErrorID, pRspInfo.ErrorMsg));
            if (!this._initialized)
                this._asyncWaiter.Set();
        }

        void _trade_OnRspUserLogin(ThostFtdcRspUserLoginField pRspUserLogin, ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            if (pRspInfo.ErrorID != 0)
                this.OnLog(string.Format("交易登录失败: 代码: {0}, 消息: {1}", pRspInfo.ErrorID, pRspInfo.ErrorMsg));
            else
            {
                this.OnLog("交易登录成功");
                this._session.FrontID = pRspUserLogin.FrontID;
                this._session.SessionID = pRspUserLogin.SessionID;
                if (!string.IsNullOrEmpty(pRspUserLogin.MaxOrderRef))
                    this._session.OrderRef = Convert.ToInt32(pRspUserLogin.MaxOrderRef);
                var orderRef = this._session.NextOrderRef();
                this._trade.ReqSettlementInfoConfirm(new ThostFtdcSettlementInfoConfirmField { BrokerID = this._config.BrokerId, InvestorID = this._config.Account, ConfirmDate = DateTime.Now.ToString("yyyyMMdd"), ConfirmTime = DateTime.Now.ToString("HH:mm:ss") }, this._session.NextRequestID());

                this.QueryOrders();
                this.QueryAccount();
                this.QueryPositions();
            }
            this._asyncWaiter.Set();
        }

        void _trade_OnFrontConnected()
        {
            if (this._config.NeedAuth)
                this.Auth();
            else
                this.Login();
        }

        private void Auth()
        {
            var field = new ThostFtdcReqAuthenticateField { AuthCode = this._config.AuthCode, BrokerID = this._config.BrokerId, UserID = this._config.Account, UserProductInfo = this._config.UserProductInfo };
            var ret = this._trade.ReqAuthenticate(field, this._session.NextRequestID());
            if (ret != 0)
                throw new Exception(string.Format("请求认证失败，返回{0}", ret));
        }

        private void Login()
        {
            var usr = new ThostFtdcReqUserLoginField
            {
                BrokerID = this._config.BrokerId,
                UserID = this._config.Account,
                Password = this._config.Password
            };
            var ret = this._trade.ReqUserLogin(usr, this._session.NextRequestID());
            if (ret != 0)
                throw new Exception(string.Format("交易登录失败，调用ReqUserLogin返回{0}", ret));
        }

        private void QueryOrders()
        {
            this._orders.Clear();
            this._ctpReqstr.Schedule(() => this._trade.ReqQryOrder(new ThostFtdcQryOrderField { InvestorID = this._config.Account }, this._session.NextRequestID()));
        }

        private void QueryAccount()
        {
            this._ctpReqstr.Schedule(() => this._trade.ReqQryTradingAccount(new ThostFtdcQryTradingAccountField { BrokerID = this._config.BrokerId, InvestorID = this._config.Account, CurrencyID = "" }, this._session.NextRequestID()));
        }

        private void QueryPositions()
        {
            var requestId = this._session.NextRequestID();
            this._positions[requestId] = new List<ThostFtdcInvestorPositionField>();
            this._ctpReqstr.Schedule(() => this._trade.ReqQryInvestorPosition(new ThostFtdcQryInvestorPositionField { BrokerID = this._config.BrokerId, InvestorID = this._config.Account }, requestId));
        }

        public OptionAccount Account
        {
            get { return this._account; }
        }

        public IEnumerable<OptionContract> Instruments
        {
            get { return this._contracts.Values; }
        }

        public OptionContract GetInstrument(string code)
        {
            if (!this._contracts.ContainsKey(code))
                return null;
            return this._contracts[code];
        }

        public void SubmitOrder(OptionOrder order)
        {
            var input = new ThostFtdcInputOrderField
            {
                BrokerID = this._config.BrokerId,
                InvestorID = this._config.Account,
                InstrumentID = order.InstrumentID,
                OrderRef = this._session.NextOrderRef().ToString(),
                UserID = this._config.Account,
                OrderPriceType = EnumOrderPriceTypeType.LimitPrice,
                Direction = order.Side == OrderSide.Buy ? EnumDirectionType.Buy : EnumDirectionType.Sell,
                CombOffsetFlag_0 = order.OpenClose == OpenClose.Open ? EnumOffsetFlagType.Open : EnumOffsetFlagType.Close,
                CombHedgeFlag_0 = EnumHedgeFlagType.Speculation,
                LimitPrice = Math.Round(order.Price, 4),
                VolumeTotalOriginal = order.Qty,
                TimeCondition = EnumTimeConditionType.GFD,
                VolumeCondition = EnumVolumeConditionType.AV,
                ContingentCondition = EnumContingentConditionType.Immediately,
                ForceCloseReason = EnumForceCloseReasonType.NotForceClose,
                IsAutoSuspend = 0,
            };
            var ret = this._trade.ReqOrderInsert(input, this._session.NextRequestID());
            if (ret != 0)
                this.OnLog(string.Format("下单失败，返回{0}", ret));
        }

        public void CancelOrder(OptionOrder order)
        {
            var native = new List<ThostFtdcOrderField>(this._orders).First(_ => _.OrderSysID == order.OrderID);
            var action = new ThostFtdcInputOrderActionField
            {
                BrokerID = this._config.BrokerId,
                InvestorID = this._config.Account,
                OrderSysID = native.OrderSysID,
                ActionFlag = EnumActionFlagType.Delete,
                UserID = this._config.Account,
                ExchangeID = native.ExchangeID,
                OrderRef = native.OrderRef,
                InstrumentID = order.InstrumentID
            };
            var ret = this._trade.ReqOrderAction(action, this._session.NextRequestID());
            if (ret != 0)
                this.OnLog(string.Format("发送撤单请求失败，返回{0}", ret));
        }

        public Market Market(OptionContract instrument)
        {
            if (this._markets.ContainsKey(instrument.ID))
                return this._markets[instrument.ID];
            else
                return null;
        }

        public event Action<Trade> Trade;

        public event Action<string> Log;

        void OnTrade(Trade trade)
        {
            this.Trade?.Invoke(trade);
        }

        void OnLog(string evnt)
        {
            this.Log?.Invoke(evnt);
        }
    }
}
