using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Core
{
    /// <summary>
    /// 抽象持仓
    /// </summary>
    public interface IPosition
    {
        string InstrumentID { get; set; }

        /// <summary>
        /// 持仓数量
        /// </summary>
        int Qty { get; set; }

        /// <summary>
        /// 可用数量
        /// </summary>
        int UsableQty { get; set; }
    }
}
