using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Core
{
    public class StockAccount : IAccount<StockPosition, StockOrder>
    {
        public double Available { get; set; }

        public ICollection<StockOrder> PendingOrders { get; set; }

        public ICollection<StockPosition> Positions { get; set; }

        public double Total { get; set; }

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
