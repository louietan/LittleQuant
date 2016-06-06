using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Core
{
    /// <summary>
    /// 股票
    /// </summary>
    public class Stock : IInstrument
    {
        public string ID { get; set; }

        public string Name { get; set; }
    }
}
