using LittleQuant.Commons;
using LittleQuant.Commons.HttpClient;
using LittleQuant.Core;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using YamlDotNet.Dynamic;

namespace LittleQuant.Exchanges.XinYeWeb
{
    /// <summary>
    /// 通过向兴业证券web端交易发送http请求实现
    /// NOTE: Trade, Instruments目前不起作用
    /// </summary>
    public class WebTrader : IStockExchange
    {
        private IHttpClient _http;
        private StockAccount _account = new StockAccount();
        private string _tokenId;  // 登录时获得，每次请求都要带上它
        private dynamic _config;

        private Dictionary<StockOrder, dynamic> _orders;
        private IDictionary<string, Stock> _stocks = new Dictionary<string, Stock>();

        public WebTrader()
        {
            using (var reader = new StreamReader(Path.Combine(Path.GetDirectoryName(this.GetType().Assembly.Location), "XinYeWeb.yaml")))
            {
                this._config = new DynamicYaml(reader);
            }
        }

        public StockAccount Account
        {
            get
            {
                // ================================================================================
                // 查询资金
                // ================================================================================
                var formData = new[] {
                    "client_cmd", "cmd_zijin_query",
                    "cmd", "cmd_zijin_query",
                    "gdzh", "",
                    "mkcode", "2",
                    "table", "data_tb",
                    "token_id", this._tokenId,
                    "zjzh", (string)this._config.account
                };
                var data = this.PostEx("https://webwt.xyzq.com.cn/stock_query.php", formData, deserializer: JsonHelper.ToObject<dynamic>);
                _account.Available = data.item[0].d_2116;
                _account.Total = data.item[0].d_2191;

                // ================================================================================
                // 查询持仓
                // ================================================================================
                formData = new[] {
                    "client_cmd", "cmd_qu_gupiao",
                    "cmd", "cmd_qu_gupiao",
                    "gdzh", "",
                    "mkcode", "2",
                    "page_currency", "R",
                    "table", "data_tb",
                    "token_id", this._tokenId,
                    "zjzh", (string)this._config.account
                };
                data = this.PostEx("https://webwt.xyzq.com.cn/stock_query.php", formData, deserializer: JsonHelper.ToObject<dynamic>);
                _account.Positions = ((IEnumerable<dynamic>)data.item).Select(_ => new StockPosition { InstrumentID = _.d_2102, Qty = _.d_2117, UsableQty = _.d_2121 }).ToList();

                // ================================================================================
                // 查询未成交委托
                // ================================================================================
                formData = new[] {
                    "client_cmd", "cmd_qu_chedan",
                    "cmd", "cmd_qu_weituo",
                    "gdzh", "",
                    "mkcode", "",
                    "option", "1",
                    "table", "data_tb",
                    "token_id", this._tokenId,
                    "zjzh", (string)this._config.account
                };
                var orderMapper = new Func<dynamic, StockOrder>(_ =>
                {
                    var order = new StockOrder
                    {
                        FilledQty = _.d_2128,
                        InstrumentID = _.d_2102,
                        Qty = _.d_2126,
                        LeavesQty = (int)_.d_2126 - (int)_.d_2128,
                        OrderTime = DateTime.ParseExact((string)_.d_2140, "HH:mm:ss", CultureInfo.InvariantCulture),
                        Price = _.d_2127,
                        Side = ((string)_.d_2109) == "买入" ? OrderSide.Buy : OrderSide.Sell,
                        Status = OrderStatus.Pending
                    };
                    order.OrderID = order.InstrumentID + order.Side;
                    return order;
                });
                data = this.PostEx("https://webwt.xyzq.com.cn/stock_query.php", formData, deserializer: JsonHelper.ToObject<dynamic>);
                data = (IEnumerable<dynamic>)data.item ?? Enumerable.Empty<dynamic>();
                this._orders = Enumerable.ToDictionary(Enumerable.GroupBy<dynamic, StockOrder>(data, orderMapper),
                                                       new Func<IGrouping<StockOrder, dynamic>, StockOrder>(_ => _.Key),
                                                       new Func<IGrouping<StockOrder, dynamic>, dynamic>(_ => _.ToList().First()));
                this._account.PendingOrders = this._orders.Keys;

                return this._account;
            }
        }

        public IEnumerable<Stock> Instruments
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        public event Action<string> Log;
        public event Action<Trade> Trade;

        public void CancelOrder(StockOrder order)
        {
            var info = this.QueryStockInfo(order.InstrumentID);
            var rawOrder = this._orders[order];
            var postData = new[] {
                "chedan_type", order.Side == OrderSide.Buy ? "1" : "2",
                "client_cmd", "cmd_qu_chedan",
                "cmd", "cmd_chedan",
                "gdzh", (string)info.gdzh,
                "htbh", (string)rawOrder.d_2135,
                "mkcode", (string)info.marketcode,
                "token_id", this._tokenId,
                "wtrq", null,
                "zjzh", (string)this._config.account,
                "zqdm", order.InstrumentID
            };

            var resp = this.PostEx("https://webwt.xyzq.com.cn/stock_chedan.php", postData, deserializer: JsonHelper.ToObject<dynamic>);
            if (resp.ret_code != 0)
                this.Log?.Invoke($"{resp.ret_msg}");
        }

        public Stock GetInstrument(string id)
        {
            if (!this._stocks.ContainsKey(id))
            {
                var info = this.QueryStockInfo(id);
                this._stocks[id] = new Stock { ID = id, Name = info.st_name };
            }
            return this._stocks[id];
        }

        public void Initialize()
        {
            this._http = new SimpleHttpClient();
            this._http.Get<string>("https://webwt.xyzq.com.cn");

            var resp = this._http.Get<string>("https://webwt.xyzq.com.cn/login.php?do=login&", specialHeaders: new SpecialHeaders { Referrer = "https://webwt.xyzq.com.cn" }, enc: Encoding.GetEncoding("GBK"));
            var match = Regex.Match(resp, "(?<=name\\=token\\s+value\\=\")\\w+");
            if (!match.Success)
                throw new Exception("未获取到token");

            var formData = new[] {
                "account", (string)this._config.account,
                "account_type", "0",
                "commkey", (string)this._config.comm_pass,
                "crpttype", "1",
                "hardwarecode", "呵呵",
                "localip", "192.168.1.123",
                "token", match.Value,
                "tradekey", (string)this._config.trade_pass,
                "yyb_name", "1"
            };

            resp = this._http.Post<string>("https://webwt.xyzq.com.cn/login.php?do=do_login", formData, enc: Encoding.GetEncoding("GBK"));
            match = Regex.Match(resp, "(?<=login\\(')\\w+");
            if (!match.Success)
                throw new Exception($"登录异常，返回结果：{resp}");
            this._tokenId = match.Value;
        }

        public Market Market(Stock instrument)
        {
            return MarketUtility.QueryMarket(instrument);
        }

        public void SubmitOrder(StockOrder order)
        {
            var info = this.QueryStockInfo(order.InstrumentID);

            var postData = new[] {
                "amount", order.Qty.ToString(),
                "cmd", order.Side == OrderSide.Buy ? "cmd_wt_mairu" : "cmd_wt_maichu",
                "gdzh", (string)info.gdzh,
                "mkcode", (string)info.marketcode,
                "price", order.Price.ToString(),
                "stockcode", order.InstrumentID,
                "table", "main_tb",
                "token_id", this._tokenId,
                "type", "ptwt",
                "zjzh", (string)this._config.account
            };

            var resp = this.PostEx("https://webwt.xyzq.com.cn/stock_weituo.php", postData, deserializer: JsonHelper.ToObject<dynamic>);
            if (resp.ret_code != 0)
                this.Log?.Invoke($"{resp.ret_msg}");
        }

        private dynamic QueryStockInfo(string stockCode)
        {
            var data = new[] {
                "auto_ref", "0",
                "cmd", "cmd_stock_query",
                "force", "0",
                "g_newpwd", "",
                "kyye", "1",
                "loc_cal_buyable", "1",
                "op_type", "mairu",
                "price", "",
                "sjwt", "",
                "sjwt_type", "",
                "stockcode", stockCode,
                "table", "main_tb",
                "token_id", this._tokenId
            };

            var respData = this.PostEx("https://webwt.xyzq.com.cn/stock_hq.php", data, deserializer: JsonHelper.ToObject<dynamic>);
            if (respData.ret_code != 0)
                throw new Exception($"{respData.ret_msg}");
            return respData;
        }

        private dynamic PostEx(string uri, IList<string> form,
                               NameValueCollection headers = null,
                               SpecialHeaders specialHeaders = null,
                               Func<String, dynamic> deserializer = null,
                               Encoding enc = null)
        {
            var doPost = new Func<dynamic>(() => this._http.Post(uri, form, headers, specialHeaders, deserializer, enc));
            var needRetry = new Predicate<dynamic>(_ => _.ret_code != 0 && ((string)_.ret_msg).Contains("超时"));
            dynamic resp;
            while (needRetry(resp = doPost()))
            {
                this.Log?.Invoke($"{resp.ret_msg}，正在重连。。。");
                this.Initialize();
                this.Log?.Invoke($"完成重连");
            }
            return resp;
        }
    }
}
