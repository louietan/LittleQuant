using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Core
{
    public static class Utilities
    {
        /// <summary>
        /// 示例：
        ///       var func = CreateLastTimeGetter(TimeSpan.FromSeconds(3));  // 假设当前时间是 13:20:33
        ///       func();  // 返回 13:20:33
        ///       // 过了2秒钟
        ///       func();  // 返回 13:20:33
        ///       // 又过了2秒钟
        ///       func();  // 返回 13:20:37
        /// </summary>
        /// <param name="span"></param>
        /// <returns></returns>
        public static Func<DateTime> CreateLastTimeGetter(TimeSpan span)
        {
            var last = DateTime.Now;
            return new Func<DateTime>(() =>
            {
                if (DateTime.Now - last >= span)
                    last = DateTime.Now;
                return last;
            });
        }
    }
}
