using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Core
{
    /// <summary>
    /// 期权委托
    /// </summary>
    public class OptionOrder : IOrder
    {
        public OptionOrder()
        {
            this.OrderTime = DateTime.Now;
        }

        public string InstrumentID { get; set; }

        public string OrderID { get; set; }

        public OrderSide Side { get; set; }

        public int Qty { get; set; }

        public double Price { get; set; }

        public int FilledQty { get; set; }

        public int LeavesQty { get; set; }

        public OpenClose OpenClose { get; set; }

        public OrderStatus Status { get; set; }

        public DateTime OrderTime { get; set; }

        public override string ToString()
        {
            return string.Format(
                "instrument: {0}, orderid: {1}, side: {2}, qty: {3}, price: {4}, leaves_qty: {5}, open_close: {6}, status: {7}, time: {8}",
                this.InstrumentID, this.OrderID, this.Side.ToString(), this.Qty, this.Price, this.LeavesQty, this.OpenClose.ToString(), this.Status.ToString(), this.OrderTime.ToString("yyyy-MM-dd HH:mm:ss"));
        }
    }
}
