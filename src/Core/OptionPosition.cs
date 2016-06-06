using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Core
{
    /// <summary>
    /// 期权持仓
    /// </summary>
    public class OptionPosition : IPosition
    {
        public string InstrumentID { get; set; }
        public int Qty { get; set; }
        public int UsableQty { get; set; }
    }
}
