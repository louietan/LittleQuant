using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Core
{
    public static class ExchangeManager
    {
        public static event Action<string> Log;

        private static void OnLog(string msg)
        {
            if (Log != null)
                Log(msg);
        }

        private static IDictionary<Type, IExchange> ExchangeSingletons = new Dictionary<Type, IExchange>();  // 交易所单例对象，键是实现类
        private static IDictionary<Type, IExchange> Cache = new Dictionary<Type, IExchange>();  // 键是接口

        /// <summary>
        /// 用类名初始化交易接口
        /// </summary>
        /// <param name="classes">item: 交易接口完整限定类名, 例：System.String, mscorlib, Version=2.0.0.0, Culture=neutral, publicKeyToken=b77a5c561934e089</param>
        public static void InitExchanges(IEnumerable<string> classes)
        {
            foreach (var i in classes)
            {
                var type = Type.GetType(i);
                if (type == null)
                    throw new Exception($"找不到类型 {i}");
                if (!ExchangeSingletons.ContainsKey(type))
                {
                    var exchange = Activator.CreateInstance(type) as IExchange;
                    if (exchange == null)
                        throw new Exception($"无法将类型\"{i}\"转换为\"{typeof(IExchange).Name}\"");
                    exchange.Log += Log;
                    exchange.Trade += _ => OnLog($"{exchange.GetType().Name}成交通知: {_.ToString()}");
                    OnLog($"正在初始化{type.Name}。。。");
                    exchange.Initialize();
                    OnLog($"{type.Name}初始化完成！");
                    ExchangeSingletons[type] = exchange;
                }
            }
        }

        /// <summary>
        /// 获取交易所，获取的对象是单例
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <returns></returns>
        public static T GetExchange<T>() where T : IExchange
        {
            return (T)GetExchange(typeof(T));
        }

        public static IExchange GetExchange(Type type)
        {
            if (!Cache.ContainsKey(type))
            {
                var exchange = ExchangeSingletons.Values.FirstOrDefault(_ => type.IsAssignableFrom(_.GetType()));
                if (exchange == null)
                    throw new Exception($"未注册{type.Name}接口的实现类");
                Cache[type] = exchange;
            }

            return Cache[type];
        }
    }
}
