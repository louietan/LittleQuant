using KSFT;
using LittleQuant.Core;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Exchanges.Kingstar
{
    public static class Extensions
    {
        public static OrderStatus GetOrderStatus(this ThostFtdcOrderField self)
        {
            switch (self.OrderStatus)
            {
                case EnumOrderStatusType.AllTraded:
                    return OrderStatus.Filled;
                case EnumOrderStatusType.Canceled:
                    return OrderStatus.Canceled;
                case EnumOrderStatusType.NoTradeNotQueueing:
                case EnumOrderStatusType.NoTradeQueueing:
                    return OrderStatus.Pending;
                case EnumOrderStatusType.PartTradedNotQueueing:
                case EnumOrderStatusType.PartTradedQueueing:
                    return OrderStatus.Pending;
                default:
                    return OrderStatus.Unknown;
            }
        }

        public static OpenClose GetOpenClose(this ThostFtdcOrderField self)
        {
            switch (self.CombOffsetFlag_0)
            {
                case EnumOffsetFlagType.Close:
                case EnumOffsetFlagType.CloseToday:
                case EnumOffsetFlagType.CloseYesterday:
                case EnumOffsetFlagType.ForceClose:
                case EnumOffsetFlagType.LocalForceClose:
                    return OpenClose.Close;
                default:
                    return OpenClose.Open;
            }
        }

        public static OptionOrder ToOptionOrder(this ThostFtdcOrderField self)
        {
            return new OptionOrder
            {
                InstrumentID = self.InstrumentID,
                FilledQty = self.VolumeTraded,
                LeavesQty = self.VolumeTotal,
                OrderID = self.OrderSysID,
                OpenClose = self.GetOpenClose(),
                Price = self.LimitPrice,
                Qty = self.VolumeTotalOriginal,
                Side = self.Direction == EnumDirectionType.Buy ? OrderSide.Buy : OrderSide.Sell,
                OrderTime = DateTime.ParseExact(self.InsertDate + " " + self.InsertTime, "yyyyMMdd HH:mm:ss", DateTimeFormatInfo.InvariantInfo),
                Status = self.GetOrderStatus()
            };
        }

        public static OptionPosition ToOptionPosition(this ThostFtdcInvestorPositionField self)
        {
            return new OptionPosition
            {
                InstrumentID = self.InstrumentID,
                Qty = self.Position,
                UsableQty = self.Position - self.LongFrozen - self.ShortFrozen
            };
        }
    }
}
