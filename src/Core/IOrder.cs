using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LittleQuant.Core
{
    /// <summary>
    /// 委托状态
    /// </summary>
    public enum OrderStatus
    {
        /// <summary>
        /// 未完成
        /// </summary>
        Pending,

        /// <summary>
        /// 全部成交
        /// </summary>
        Filled,

        /// <summary>
        /// 已撤销
        /// </summary>
        Canceled,

        Unknown
    }

    /// <summary>
    /// 抽象委托
    /// </summary>
    public interface IOrder
    {
        string InstrumentID { get; set; }

        /// <summary>
        /// 委托编号
        /// </summary>
        string OrderID { get; set; }

        OrderSide Side { get; set; }

        /// <summary>
        /// 价格
        /// </summary>
        double Price { get; set; }

        /// <summary>
        /// 委托总数
        /// </summary>
        int Qty { get; set; }

        /// <summary>
        /// 成交数量
        /// </summary>
        int FilledQty { get; set; }

        /// <summary>
        /// 未成交数量
        /// </summary>
        int LeavesQty { get; set; }

        /// <summary>
        /// 委托时间
        /// </summary>
        DateTime OrderTime { get; set; }

        OrderStatus Status { get; set; }
    }

    public static class IOrderExtension
    {
        /// <summary>
        /// 是否是部分成交
        /// </summary>
        /// <param name="self"></param>
        /// <returns></returns>
        public static bool IsPartialFilled(this IOrder self)
        {
            return self.Status == OrderStatus.Pending && self.LeavesQty != self.Qty;
        }
    }
}
