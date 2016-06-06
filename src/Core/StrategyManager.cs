using NLog;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;

namespace LittleQuant.Core
{
    public class StrategyManager
    {
        static ILogger Logger = LogManager.GetLogger("strategy");

        /// <summary>
        /// 
        /// </summary>
        /// <param name="exchangeService"></param>
        /// <param name="assemblyPath">策略所在dll完整路径</param>
        /// <exception cref="Exception">指定程序集中没有策略类</exception>
        public static IStrategy Load(string assemblyPath)
        {
            var asm = Assembly.LoadFile(assemblyPath);

            var strategyType = asm.GetTypes().Where(_ => _.GetInterface(typeof(IStrategy).FullName) != null).FirstOrDefault();
            if (strategyType == null)
                throw new Exception("该dll中没有策略类！");
            var strategy = (IStrategy)Activator.CreateInstance(strategyType);
            strategy.Log += (type, log) => Logger.Info(type.ToString() + ": " + log);

            return strategy;
        }
    }
}
