using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Core
{
    /// <summary>
    /// 抽象合约
    /// </summary>
    public interface IInstrument
    {
        string ID { get; set; }
        string Name { get; set; }
    }
}
