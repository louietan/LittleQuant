using NLog;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace LittleQuant.Exchanges.CTP
{
    /// <summary>
    /// CTP对查询请求频率有限制，此类功能是对查询请求进行缓冲
    /// </summary>
    public class CTPQryReqScheduler
    {
        private static ILogger Logger = LogManager.GetCurrentClassLogger();

        private AutoResetEvent _notEmpty = new AutoResetEvent(false);
        private ConcurrentQueue<Func<int>> _tasks = new ConcurrentQueue<Func<int>>();
        private TimeSpan _frequency = TimeSpan.FromMilliseconds(500);

        public CTPQryReqScheduler()
        {
            Task.Run(() =>
            {
                while (true)
                {
                    if (this._tasks.IsEmpty)
                        this._notEmpty.WaitOne();
                    Func<int> first = null;
                    while (this._tasks.TryDequeue(out first))
                    {
                        Debug.WriteLine($"{DateTime.Now.ToString("mm:ss.ffff")} dequeue a task and run it...");
                        Predicate<int> needRetry = _ => _ == -2 || _ == -3;
                        while (needRetry(first()))
                        {
                            Thread.Sleep(this._frequency);
                            Debug.WriteLine($"{DateTime.Now.ToString("mm:ss.ffff")} retry...");
                        }
                        Debug.WriteLine($"{DateTime.Now.ToString("mm:ss.ffff")} waiting tasks count: {this._tasks.Count()}");
                        Thread.Sleep(this._frequency);
                    }
                }
            });
        }

        public void Schedule(Func<int> fn)
        {
            Debug.WriteLine($"{DateTime.Now.ToString("mm:ss.ffff")} put a task in the queue, waiting tasks count: {this._tasks.Count()}");
            this._tasks.Enqueue(fn);
            if (!this._tasks.IsEmpty)
                this._notEmpty.Set();
        }
    }
}
