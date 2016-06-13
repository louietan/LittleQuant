using LittleQuant.Core;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace LittleQuant.Tests
{
    class ExchangeBasicTests<TAccount, TInstrument, TOrder, TPosition>
        where TAccount : IAccount<TPosition, TOrder>
        where TInstrument : IInstrument
        where TOrder : IOrder
        where TPosition : IPosition
    {
        public IExchange<TAccount, TInstrument, TOrder, TPosition> Exchange { get; set; }

        public ExchangeBasicTests<TAccount, TInstrument, TOrder, TPosition> SetExchange(string impl)
        {
            var type = Type.GetType(impl);
            Debug.Assert(type != null);
            this.Exchange = (IExchange<TAccount, TInstrument, TOrder, TPosition>)Activator.CreateInstance(type);
            Console.WriteLine("initializing exchange: " + type.Name + "...");
            this.Exchange.Initialize();
            Console.WriteLine("exchange: " + type.Name + "  initialized!");
            this.Exchange.Log += Console.WriteLine;
            return this;
        }

        public bool Test_Instruments_NotEmpty()
        {
            return this.Exchange.Instruments.Any();
        }

        public bool Test_GetInstrument_ForAnyInstrument_NotNull()
        {
            return this.Exchange.Instruments.Any(_ => this.Exchange.GetInstrument(_.ID) != null);
        }

        public bool Test_Market_ForAnyInstruemnt_NotNull()
        {
            return this.Exchange.Instruments.Any(_ => this.Exchange.Market(_) != null);
        }

        public bool Test_SubmitOrder_NotFilled_ShouldInPendingList()
        {
            var instrument = this.Exchange.Instruments.FirstOrDefault(_ => (new Predicate<Market>(m => m.Bids.Count >= 3 && m.Asks.Count >= 3)).Invoke(this.Exchange.Market(_)));
            var market = this.Exchange.Market(instrument);
            var order = (TOrder)Activator.CreateInstance(typeof(TOrder));
            order.InstrumentID = instrument.ID;
            order.Side = OrderSide.Buy;
            order.Qty = 1;
            order.Price = market.Bids.Last().Price;
            this.Exchange.SubmitOrder(order);
            Thread.Sleep(5000);
            return this.Exchange.Account.PendingOrders.Any(_ => _.InstrumentID == instrument.ID);
        }

        public bool Test_SubmitOrder_Filled_PositionsShouldChanged()
        {
            var instrument = this.Exchange.Instruments.FirstOrDefault(_ => (new Predicate<Market>(_market => _market.Bids.Count >= 3 && _market.Asks.Count >= 3)).Invoke(this.Exchange.Market(_)));
            var market = this.Exchange.Market(instrument);
            var order = (TOrder)Activator.CreateInstance(typeof(TOrder));
            order.InstrumentID = instrument.ID;
            order.Side = OrderSide.Buy;
            order.Qty = 1;
            order.Price = market.Asks.First().Price * 1.1;
            this.Exchange.SubmitOrder(order);
            Thread.Sleep(5000);
            return this.Exchange.Account.Positions.Any(_ => _.InstrumentID == instrument.ID);
        }

        public bool Test_CancelOrder_ShouldRemovedFromPendingList()
        {
            var pending = this.Exchange.Account.PendingOrders.First();
            // this.Exchange.CancelOrder(pending.OrderID); TODO: 
            Thread.Sleep(2000);
            return this.Exchange.Account.PendingOrders.Any(_ => _.OrderID != pending.OrderID);
        }
    }
}
