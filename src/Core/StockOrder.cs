using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Core
{
    public class StockOrder : IOrder
    {
        public int FilledQty { get; set; }

        public string InstrumentID { get; set; }

        public int LeavesQty { get; set; }

        public string OrderID { get; set; }

        public DateTime OrderTime { get; set; }

        public double Price { get; set; }

        public int Qty { get; set; }

        public OrderSide Side { get; set; }

        public OrderStatus Status { get; set; }

        public override string ToString()
        {
            return string.Format(
                "instrument: {0}, orderid: {1}, side: {2}, qty: {3}, price: {4}, leaves_qty: {5}, status: {6}, time: {7}",
                this.InstrumentID, this.OrderID, this.Side.ToString(), this.Qty, this.Price, this.LeavesQty, this.Status.ToString(), this.OrderTime.ToString("yyyy-MM-dd HH:mm:ss"));
        }
    }
}