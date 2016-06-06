using NLog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;

namespace LittleQuant.Core
{
    public static class MarketUtility
    {
        static Logger Logger = LogManager.GetCurrentClassLogger();

        /// <summary>
        /// 查询行情的备用方法。
        /// </summary>
        public static Market QueryMarket(IInstrument instrument)
        {
            using (var http = new HttpClient())
            {
                var prefix = new Func<string, string>(code =>
                {
                    // 这里判断不够准确
                    if (code.StartsWith("600") || code.StartsWith("601") || code.StartsWith("603") /*沪市A股*/
                          || (code.Length == 6 && (code.StartsWith("50") || code.StartsWith("51") || code.StartsWith("52")) /*沪市基金*/))
                        return "sh";
                    else
                        return "sz";
                });

                var url = $"http://hq.sinajs.cn/list={prefix(instrument.ID)}{instrument.ID}";
                var resp = http.GetStringAsync(url).Result;
                var tokens = resp.Split(new[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                if (tokens.Length < 30)
                {
                    var msg = string.Format($"http获取{instrument.ID}行情失败，url：{url}，结果{resp}");
                    Logger.Error(msg);
                    return null;
                }
                var market = new Market
                {
                    InstrumentID = instrument.ID,
                    PrevClose = double.Parse(tokens[2]),
                    LastPrice = double.Parse(tokens[3])
                };
                var buys = new Quote[] {
                    new Quote{ Price = double.Parse(tokens[11]), Qty = int.Parse(tokens[10]) },
                    new Quote{ Price = double.Parse(tokens[13]), Qty = int.Parse(tokens[12]) },
                    new Quote{ Price = double.Parse(tokens[15]), Qty = int.Parse(tokens[14]) },
                    new Quote{ Price = double.Parse(tokens[17]), Qty = int.Parse(tokens[16]) },
                    new Quote{ Price = double.Parse(tokens[19]), Qty = int.Parse(tokens[18]) }
                };
                var sells = new Quote[] {
                    new Quote { Price = double.Parse(tokens[21]), Qty =  int.Parse(tokens[20]) } ,
                    new Quote { Price = double.Parse(tokens[23]), Qty =  int.Parse(tokens[22]) } ,
                    new Quote { Price = double.Parse(tokens[25]), Qty =  int.Parse(tokens[24]) } ,
                    new Quote { Price = double.Parse(tokens[27]), Qty =  int.Parse(tokens[26]) } ,
                    new Quote { Price = double.Parse(tokens[29]), Qty =  int.Parse(tokens[28]) }
                };
                market.SetAsks(buys);
                market.SetBids(sells);
                return market;
            }
        }
    }
}
