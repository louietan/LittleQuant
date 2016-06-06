using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Core
{
    /// <summary>
    /// 表示一次成交
    /// </summary>
    public class Trade
    {
        public string InstrumentID { get; set; }

        /// <summary>
        /// 成交价
        /// </summary>
        public double Price { get; set; }

        /// <summary>
        /// 成交量
        /// </summary>
        public int Qty { get; set; }

        public override string ToString()
        {
            return string.Format("instrument: {0}, price: {1}, vol: {2}", InstrumentID, Price, Qty);
        }
    }
}
