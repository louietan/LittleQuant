using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LittleQuant.Core
{
    /// <summary>
    /// 期权账户
    /// </summary>
    public class OptionAccount : IAccount<OptionPosition, OptionOrder>
    {
        private ICollection<OptionPosition> _positions;
        private ICollection<OptionOrder> _pendingOrders;

        public double Total { get; set; }

        /// <summary>
        /// 可用资金
        /// </summary>
        public double Available { get; set; }

        public ICollection<OptionPosition> Positions
        {
            get
            {
                return this._positions ?? (this._positions = Enumerable.Empty<OptionPosition>().ToList());
            }
            set
            {
                this._positions = value;
            }
        }

        public ICollection<OptionOrder> PendingOrders
        {
            get
            {
                return this._pendingOrders ?? (this._pendingOrders = Enumerable.Empty<OptionOrder>().ToList());
            }
            set
            {
                this._pendingOrders = value;
            }
        }

        public override string ToString()
        {
            var builder = new StringBuilder();
            builder.AppendFormat("总资产: {0}, 可用资金: {1}" + Environment.NewLine, this.Total, this.Available).AppendLine("未完成的委托:");
            foreach (var o in this.PendingOrders)
            {
                builder.AppendLine(o.ToString());
            }

            builder.AppendLine("持仓:");
            foreach (var p in this.Positions)
            {
                builder.AppendFormat("合约代码: {0} 量: {1}" + Environment.NewLine, p.InstrumentID, p.Qty);
            }
            return builder.ToString();
        }
    }
}
