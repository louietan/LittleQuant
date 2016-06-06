using LittleQuant.Core;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Strategies
{
    /// <summary>
    /// 示例策略
    /// </summary>
    public class DemoStrategy : IStrategy
    {
        public string Name
        {
            get
            {
                return this.GetType().Name;
            }
        }

        public bool Running { get; set; }

        public event Action<LogType, string> Log;

        public void Start()
        {
            this.Running = true;
            var exchange = ExchangeManager.GetExchange<IOptionExchange>();
            var instrument = exchange.Instruments.First();
            var market = exchange.Market(instrument);
            var order = new OptionOrder
            {
                InstrumentID = instrument.ID,
                Qty = 100,
                Price = market.Asks.First().Price,
                Side = OrderSide.Buy,
                OpenClose = OpenClose.Open
            };
            exchange.SubmitOrder(order);
            this.Log?.Invoke(LogType.Trade, order.ToString());
        }

        public void Stop()
        {
            this.Running = false;
        }
    }
}
