using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LittleQuant.Core
{
    public enum LogType
    {
        Trade,
        Debug,
        Info,
        Error
    }

    /// <summary>
    /// 抽象交易策略
    /// </summary>
    public interface IStrategy
    {
        /// <summary>
        /// 报告状态
        /// </summary>
        event Action<LogType, String> Log;

        void Start();
        void Stop();
        bool Running { get; set; }

        string Name { get; }
    }
}
