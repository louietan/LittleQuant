using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;

namespace LittleQuant.Core
{
    /// <summary>
    /// 行情数据
    /// </summary>
    public class Market
    {
        public string InstrumentID { get; set; }

        /// <summary>
        /// 最新价格
        /// </summary>
        public double LastPrice { get; set; }

        /// <summary>
        /// 昨收盘价
        /// </summary>
        public double PrevClose { get; set; }

        /// <summary>
        /// 买单，按价格从大到小
        /// </summary>
        public ReadOnlyCollection<Quote> Bids { get; private set; }

        /// <summary>
        /// 卖单，按价格从小到大
        /// </summary>
        public ReadOnlyCollection<Quote> Asks { get; private set; }

        public override string ToString()
        {
            var builder = new StringBuilder();
            builder.AppendFormat("{0}{1}", this.InstrumentID, Environment.NewLine)
                .AppendFormat("最新价: {0}, 昨收盘价: {1}" + Environment.NewLine, this.LastPrice, this.PrevClose);

            builder.Append("买单:" + Environment.NewLine);
            foreach (var i in this.Bids)
            {
                builder.AppendFormat("{0} {1}{2}", i.Price, i.Qty, Environment.NewLine);
            }
            builder.Append("卖单:" + Environment.NewLine);
            foreach (var i in this.Asks)
            {
                builder.AppendFormat("{0} {1}{2}", i.Price, i.Qty, Environment.NewLine);
            }
            return builder.ToString();
        }

        public void SetAsks(IEnumerable<Quote> asks)
        {
            this.Asks = asks.OrderBy(_ => _.Price).ToList().AsReadOnly();
        }

        public void SetBids(IEnumerable<Quote> bids)
        {
            this.Bids = bids.OrderByDescending(_ => _.Price).ToList().AsReadOnly();
        }
    }
}
