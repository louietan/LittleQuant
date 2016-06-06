using System;
using CTP;
using System.Diagnostics;

namespace CSTraderTest
{
    class Program
    {
        static void Main(string[] args)
        {
            new testTraderApi().Run();
        }
    }

    class testTraderApi
    {
        CTPTraderAdapter api = null;
        string FRONT_ADDR = "tcp://asp-sim2-front1.financial-trading-platform.com:26205";  // 前置地址
        string BROKER_ID = "2030";                       // 经纪公司代码
        string INVESTOR_ID = "888888";                    // 投资者代码
        string PASSWORD = "888888";                     // 用户密码
        string INSTRUMENT_ID = "m1301";
        EnumDirectionType DIRECTION = EnumDirectionType.Sell;
        double LIMIT_PRICE = 3400;
        int iRequestID = 0;

        // 会话参数
        int FRONT_ID;	//前置编号
        int SESSION_ID;	//会话编号
        string ORDER_REF;	//报单引用

        bool ORDER_ACTION_SENT = false;		//是否发送了报单

        public void Run()
        {
            api = new CTPTraderAdapter();
            api.OnFrontConnected += new FrontConnected(OnFrontConnected);
            api.OnFrontDisconnected += new FrontDisconnected(OnFrontDisconnected);
            api.OnHeartBeatWarning += new HeartBeatWarning(OnHeartBeatWarning);
            api.OnRspError += new RspError(OnRspError);
            api.OnRspUserLogin += new RspUserLogin(OnRspUserLogin);
            api.OnRspOrderAction += new RspOrderAction(OnRspOrderAction);
            api.OnRspOrderInsert += new RspOrderInsert(OnRspOrderInsert);
            api.OnRspQryInstrument += new RspQryInstrument(OnRspQryInstrument);
            api.OnRspQryInvestorPosition += new RspQryInvestorPosition(OnRspQryInvestorPosition);
            api.OnRspQryTradingAccount += new RspQryTradingAccount(OnRspQryTradingAccount);
            api.OnRspSettlementInfoConfirm += new RspSettlementInfoConfirm(OnRspSettlementInfoConfirm);
            api.OnRtnOrder += new RtnOrder(OnRtnOrder);
            api.OnRtnTrade += new RtnTrade(OnRtnTrade);

            api.SubscribePublicTopic(EnumTeResumeType.THOST_TERT_RESTART);					// 注册公有流
            api.SubscribePrivateTopic(EnumTeResumeType.THOST_TERT_RESTART);					// 注册私有流

            try
            {
                api.RegisterFront(FRONT_ADDR);
                api.Init();
                api.Join(); // 阻塞直到关闭或者CTRL+C
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
            finally
            {
                api.Release();
            }
        }

        void OnFrontConnected()
        {
            DebugPrintFunc(new StackTrace());
            ReqUserLogin();
        }

        void ReqUserLogin()
        {
            ThostFtdcReqUserLoginField req = new ThostFtdcReqUserLoginField();
            req.BrokerID = BROKER_ID;
            req.UserID = INVESTOR_ID;
            req.Password = PASSWORD;
            int iResult = api.ReqUserLogin(req, ++iRequestID);
            Console.WriteLine("--->>> 发送用户登录请求: " + ((iResult == 0) ? "成功" : "失败"));
        }

        void OnRspUserLogin(ThostFtdcRspUserLoginField pRspUserLogin,ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            DebugPrintFunc(new StackTrace());
            if (bIsLast && !IsErrorRspInfo(pRspInfo))
            {
                // 保存会话参数
                FRONT_ID = pRspUserLogin.FrontID;
                SESSION_ID = pRspUserLogin.SessionID;
                int iNextOrderRef = 0;
                if(!string.IsNullOrEmpty(pRspUserLogin.MaxOrderRef))
                    iNextOrderRef = Convert.ToInt32(pRspUserLogin.MaxOrderRef);
                iNextOrderRef++;
                ORDER_REF = Convert.ToString(iNextOrderRef);
                ///获取当前交易日
                Console.WriteLine("--->>> 获取当前交易日 = " + api.GetTradingDay());
                ///投资者结算结果确认
                ReqSettlementInfoConfirm();
            }
        }

        void ReqSettlementInfoConfirm()
        {
            ThostFtdcSettlementInfoConfirmField req = new ThostFtdcSettlementInfoConfirmField();
            req.BrokerID = BROKER_ID;
            req.InvestorID = INVESTOR_ID;
            int iResult = api.ReqSettlementInfoConfirm(req, ++iRequestID);
            Console.WriteLine("--->>> 投资者结算结果确认: " + ((iResult == 0) ? "成功" : "失败"));
        }

        void OnRspSettlementInfoConfirm(ThostFtdcSettlementInfoConfirmField pSettlementInfoConfirm, ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            DebugPrintFunc(new StackTrace());
            if (bIsLast && !IsErrorRspInfo(pRspInfo))
            {
                ///请求查询合约
                ReqQryInstrument();
            }
        }

        void ReqQryInstrument()
        {
            ThostFtdcQryInstrumentField req = new ThostFtdcQryInstrumentField();
            req.InstrumentID = INSTRUMENT_ID;
            int iResult = api.ReqQryInstrument(req, ++iRequestID);
            Console.WriteLine("--->>> 请求查询合约: " + ((iResult == 0) ? "成功" : "失败"));
        }


        void OnRspQryInstrument(ThostFtdcInstrumentField pInstrument, ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            DebugPrintFunc(new StackTrace());
            if (bIsLast && !IsErrorRspInfo(pRspInfo))
            {
                //请求查询合约
                ReqQryTradingAccount();
            }
        }

        void ReqQryTradingAccount()
        {
            ThostFtdcQryTradingAccountField req = new ThostFtdcQryTradingAccountField();
            req.BrokerID = BROKER_ID;
            req.InvestorID = INVESTOR_ID;
            int iResult = api.ReqQryTradingAccount(req, ++iRequestID);
            Console.WriteLine("--->>> 请求查询资金账户: " + ((iResult == 0) ? "成功" : "失败"));
        }

        void OnRspQryTradingAccount(ThostFtdcTradingAccountField pTradingAccount, ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            DebugPrintFunc(new StackTrace());

            if (bIsLast && !IsErrorRspInfo(pRspInfo))
            {
                //请求查询投资者持仓
                ReqQryInvestorPosition();
            }
        }

        void ReqQryInvestorPosition()
        {
            ThostFtdcQryInvestorPositionField req = new ThostFtdcQryInvestorPositionField();
            req.BrokerID = BROKER_ID;
            req.InvestorID = INVESTOR_ID;
            req.InstrumentID = INSTRUMENT_ID;
            int iResult = api.ReqQryInvestorPosition(req, ++iRequestID);
            Console.WriteLine("--->>> 请求查询投资者持仓: " + ((iResult == 0) ? "成功" : "失败"));
        }

        void OnRspQryInvestorPosition(ThostFtdcInvestorPositionField pInvestorPosition, ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            DebugPrintFunc(new StackTrace());
            if (bIsLast && !IsErrorRspInfo(pRspInfo))
            {
                // 报单录入请求
                //ReqOrderInsert();
            }
        }

        void ReqOrderInsert()
        {
            ThostFtdcInputOrderField req = new ThostFtdcInputOrderField();
            ///经纪公司代码
            req.BrokerID = BROKER_ID;
            ///投资者代码
            req.InvestorID = INVESTOR_ID;
            ///合约代码
            req.InstrumentID = INSTRUMENT_ID;
            ///报单引用
            req.OrderRef = ORDER_REF;
            ///用户代码
            //	TThostFtdcUserIDType	UserID;
            ///报单价格条件: 限价
            req.OrderPriceType = CTP.EnumOrderPriceTypeType.LimitPrice;
            ///买卖方向: 
            req.Direction = DIRECTION;
            ///组合开平标志: 开仓
            req.CombOffsetFlag_0 = CTP.EnumOffsetFlagType.Open;
            ///组合投机套保标志
            req.CombHedgeFlag_0 = CTP.EnumHedgeFlagType.Speculation;
            ///价格
            req.LimitPrice = LIMIT_PRICE;
            ///数量: 1
            req.VolumeTotalOriginal = 1;
            ///有效期类型: 当日有效
            req.TimeCondition = CTP.EnumTimeConditionType.GFD;
            ///GTD日期
            //	TThostFtdcDateType	GTDDate;
            ///成交量类型: 任何数量
            req.VolumeCondition = CTP.EnumVolumeConditionType.AV;
            ///最小成交量: 1
            req.MinVolume = 1;
            ///触发条件: 立即
            req.ContingentCondition = CTP.EnumContingentConditionType.Immediately;
            ///止损价
            //	TThostFtdcPriceType	StopPrice;
            ///强平原因: 非强平
            req.ForceCloseReason = CTP.EnumForceCloseReasonType.NotForceClose;
            ///自动挂起标志: 否
            req.IsAutoSuspend = 0;
            ///业务单元
            //	TThostFtdcBusinessUnitType	BusinessUnit;
            ///请求编号
            //	TThostFtdcRequestIDType	RequestID;
            ///用户强评标志: 否
            req.UserForceClose = 0;

            int iResult = api.ReqOrderInsert(req, ++iRequestID);
            Console.WriteLine("--->>> 报单录入请求: " + ((iResult == 0) ? "成功" : "失败"));
        }

        void OnRspOrderInsert(ThostFtdcInputOrderField pInputOrder, ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            DebugPrintFunc(new StackTrace());
            IsErrorRspInfo(pRspInfo);
        }

        void ReqOrderAction(ThostFtdcOrderField pOrder)
        {

            if (ORDER_ACTION_SENT)
                return;

            ThostFtdcInputOrderActionField req = new ThostFtdcInputOrderActionField();
            ///经纪公司代码
            req.BrokerID = pOrder.BrokerID;
            ///投资者代码
            req.InvestorID = pOrder.InvestorID;
            ///报单操作引用
            //	TThostFtdcOrderActionRefType	OrderActionRef;
            ///报单引用
            req.OrderRef = pOrder.OrderRef;
            ///请求编号
            //	TThostFtdcRequestIDType	RequestID;
            ///前置编号
            req.FrontID = FRONT_ID;
            ///会话编号
            req.SessionID = SESSION_ID;
            ///交易所代码
            //	TThostFtdcExchangeIDType	ExchangeID;
            ///报单编号
            //	TThostFtdcOrderSysIDType	OrderSysID;
            ///操作标志
            req.ActionFlag = CTP.EnumActionFlagType.Delete;
            ///价格
            //	TThostFtdcPriceType	LimitPrice;
            ///数量变化
            //	TThostFtdcVolumeType	VolumeChange;
            ///用户代码
            //	TThostFtdcUserIDType	UserID;
            ///合约代码
            req.InstrumentID = pOrder.InstrumentID;

            int iResult = api.ReqOrderAction(req, ++iRequestID);
            Console.WriteLine("--->>> 报单操作请求: " + ((iResult == 0) ? "成功" : "失败"));

            ORDER_ACTION_SENT = true;
        }

        void OnRspOrderAction(ThostFtdcInputOrderActionField pInputOrderAction, ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            DebugPrintFunc(new StackTrace());
            IsErrorRspInfo(pRspInfo);
        }

        ///报单通知
        void OnRtnOrder(ThostFtdcOrderField pOrder)
        {
            DebugPrintFunc(new StackTrace());
            if (IsMyOrder(pOrder))
            {
                if (IsTradingOrder(pOrder))
                    ReqOrderAction(pOrder);
                else if (pOrder.OrderStatus == EnumOrderStatusType.Canceled)
                    Console.WriteLine("--->>> 撤单成功");
            }
        }

        ///成交通知
        void OnRtnTrade(ThostFtdcTradeField pTrade)
        {
            DebugPrintFunc(new StackTrace());
        }

        void OnFrontDisconnected(int nReason)
        {
            DebugPrintFunc(new StackTrace());
            Console.WriteLine("--->>> Reason = {0}", nReason);
        }

        void OnHeartBeatWarning(int nTimeLapse)
        {
            DebugPrintFunc(new StackTrace());
            Console.WriteLine("--->>> nTimerLapse = " + nTimeLapse);
        }

        void OnRspError(ThostFtdcRspInfoField pRspInfo, int nRequestID, bool bIsLast)
        {
            DebugPrintFunc(new StackTrace());
            IsErrorRspInfo(pRspInfo);
        }

        bool IsErrorRspInfo(ThostFtdcRspInfoField pRspInfo)
        {
            // 如果ErrorID != 0, 说明收到了错误的响应
            bool bResult = ((pRspInfo != null) && (pRspInfo.ErrorID != 0));
            if (bResult)
                Console.WriteLine("--->>> ErrorID={0}, ErrorMsg={1}", pRspInfo.ErrorID, pRspInfo.ErrorMsg);
            return bResult;
        }

        bool IsMyOrder(ThostFtdcOrderField pOrder)
        {
            return ((pOrder.FrontID == FRONT_ID) &&
                    (pOrder.SessionID == SESSION_ID) &&
                    (pOrder.OrderRef == ORDER_REF));
        }
    

        bool IsTradingOrder(ThostFtdcOrderField pOrder)
        {
            return ((pOrder.OrderStatus != EnumOrderStatusType.PartTradedNotQueueing) &&
                    (pOrder.OrderStatus != EnumOrderStatusType.Canceled) &&
                    (pOrder.OrderStatus != EnumOrderStatusType.AllTraded));
        }

        void DebugPrintFunc(StackTrace stkTrace)
        {
            string s = stkTrace.GetFrame(0).ToString();
            s = s.Split(new char[] { ' ' })[0];
            Debug.WriteLine("--->>> " + s);
            Console.WriteLine("--->>> " + s);
        }

    }
}
