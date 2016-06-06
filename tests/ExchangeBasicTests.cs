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
        private IExchange<TAccount, TInstrument, TOrder, TPosition> _exchange;

        public ExchangeBasicTests<TAccount, TInstrument, TOrder, TPosition> SetExchange(string impl)
        {
            var type = Type.GetType(impl);
            Debug.Assert(type != null);
            this._exchange = Activator.CreateInstance(type) as IExchange<TAccount, TInstrument, TOrder, TPosition>;
            Debug.Assert(this._exchange != null);
            Console.WriteLine("initializing exchange: " + type.Name + "...");
            this._exchange.Initialize();
            Console.WriteLine("exchange: " + type.Name + "  initialized!");
            this._exchange.Log += Console.WriteLine;
            return this;
        }

        public bool Test_Instruments_NotEmpty()
        {
            return this._exchange.Instruments.Any();
        }

        public bool Test_GetInstrument_ForAnyInstrument_NotNull()
        {
            return this._exchange.Instruments.Any(_ => this._exchange.GetInstrument(_.ID) != null);
        }

        public bool Test_Market_ForAnyInstruemnt_NotNull()
        {
            return this._exchange.Instruments.Any(_ => this._exchange.Market(_) != null);
        }

        public bool Test_SubmitOrder_NotFilled_ShouldInPendingList()
        {
            var instrument = this._exchange.Instruments.FirstOrDefault(_ => (new Predicate<Market>(m => m.Bids.Count >= 3 && m.Asks.Count >= 3)).Invoke(this._exchange.Market(_)));
            var market = this._exchange.Market(instrument);
            var order = (TOrder)Activator.CreateInstance(typeof(TOrder));
            order.InstrumentID = instrument.ID;
            order.Side = OrderSide.Buy;
            order.Qty = 1;
            order.Price = market.Bids.Last().Price;
            this._exchange.SubmitOrder(order);
            Thread.Sleep(5000);
            return this._exchange.Account.PendingOrders.Any(_ => _.InstrumentID == instrument.ID);
        }

        public bool Test_SubmitOrder_Filled_PositionsShouldChanged()
        {
            var instrument = this._exchange.Instruments.FirstOrDefault(_ => (new Predicate<Market>(_market => _market.Bids.Count >= 3 && _market.Asks.Count >= 3)).Invoke(this._exchange.Market(_)));
            var market = this._exchange.Market(instrument);
            var order = (TOrder)Activator.CreateInstance(typeof(TOrder));
            order.InstrumentID = instrument.ID;
            order.Side = OrderSide.Buy;
            order.Qty = 1;
            order.Price = market.Asks.First().Price * 1.1;
            this._exchange.SubmitOrder(order);
            Thread.Sleep(5000);
            return this._exchange.Account.Positions.Any(_ => _.InstrumentID == instrument.ID);
        }

        public bool Test_CancelOrder_ShouldRemovedFromPendingList()
        {
            var pending = this._exchange.Account.PendingOrders.First();
            this._exchange.CancelOrder(pending.OrderID);
            Thread.Sleep(2000);
            return this._exchange.Account.PendingOrders.Any(_ => _.OrderID != pending.OrderID);
        }
    }
}
