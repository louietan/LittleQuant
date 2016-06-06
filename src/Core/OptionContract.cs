using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LittleQuant.Core
{
    /// <summary>
    /// 期权合约
    /// </summary>
    public class OptionContract : IInstrument
    {
        public string ID { get; set; }

        public string Name { get; set; }

        /// <summary>
        /// 合约单位
        /// </summary>
        public int Size { get; set; }

        public CallPut CallPut { get; set; }

        /// <summary>
        /// 标的代码
        /// </summary>
        public String SubjectMatter { get; set; }

        /// <summary>
        /// 行权价
        /// </summary>
        public double Strike { get; set; }

        /// <summary>
        /// 到期日
        /// </summary>
        public DateTime Maturity { get; set; }
    }
}
