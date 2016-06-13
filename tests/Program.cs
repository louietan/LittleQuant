using LittleQuant.Core;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace LittleQuant.Tests
{
    class Program
    {
        static void Main(string[] args)
        {
            var suite = new ExchangeBasicTests<StockAccount, Stock, StockOrder, StockPosition>().SetExchange("LittleQuant.Exchanges.XinYeWeb.WebTrader, XinYeWebHack");
            var counter = 0;
            while (true)
            {
                var exchange = suite.Exchange;
                Console.WriteLine($"============================第{++counter}次测试==================================");
                Console.WriteLine(exchange.Account.ToString());
                exchange.SubmitOrder(new StockOrder { InstrumentID = "600300", Qty = 100, Price = 5.21, Side = OrderSide.Buy });
                //exchange.SubmitOrder(new StockOrder { InstrumentID = "603398", Qty = 100, Price = 39.48, Side = OrderSide.Sell });
                exchange.SubmitOrder(new StockOrder { InstrumentID = "000725", Qty = 100, Price = 2.05, Side = OrderSide.Buy });
                Console.WriteLine(exchange.Account.ToString());
                exchange.Account.PendingOrders.ToList().ForEach(exchange.CancelOrder);
                Thread.Sleep(2000);
            }

            Console.WriteLine("Done!");
            Console.ReadKey();
        }

        /// <summary>
        /// 测试方法以Test_开头
        /// </summary>
        /// <param name="suite"></param>
        static void RunTests(Object suite)
        {
            var tests = suite.GetType().GetMethods().Where(_ => _.Name.StartsWith("Test_"));
            foreach (var i in tests)
            {
                if ((bool)i.Invoke(suite, null))
                {
                    Console.ForegroundColor = ConsoleColor.Green;
                    Console.WriteLine($"{i.Name} passed!");
                }
                else
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine($"{i.Name} failed!");
                }
                Console.ResetColor();
            }
        }

        static object DebugTest(Object suite, string test)
        {
            var method = suite.GetType().GetMethods().FirstOrDefault(_ => _.Name == test);
            return method.Invoke(suite, null);
        }
    }
}
