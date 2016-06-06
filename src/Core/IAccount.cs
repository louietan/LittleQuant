using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Core
{
    /// <summary>
    /// 抽象账户。包括资金、委托、持仓等信息
    /// </summary>
    public interface IAccount<TPosition, TOrder>
        where TPosition : IPosition
        where TOrder : IOrder
    {
        /// <summary>
        /// 总资产
        /// </summary>
        double Total { get; set; }

        /// <summary>
        /// 可用资金
        /// </summary>
        double Available { get; set; }

        /// <summary>
        /// 持仓
        /// </summary>
        ICollection<TPosition> Positions { get; set; }

        /// <summary>
        /// 未成交委托
        /// </summary>
        ICollection<TOrder> PendingOrders { get; set; }
    }
}
