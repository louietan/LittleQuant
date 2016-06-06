using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Core
{
    /// <summary>
    /// 报价
    /// </summary>
    public struct Quote
    {
        public double Price { get; set; }
        public int Qty { get; set; }
    }
}
