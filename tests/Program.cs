using LittleQuant.Core;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Tests
{
    class Program
    {
        static void Main(string[] args)
        {
            var suite = new ExchangeBasicTests<OptionAccount, OptionContract, OptionOrder, OptionPosition>().SetExchange("LittleQuant.Exchanges.CTP.CTPOptionExchange, CTP");
            RunTests(suite);
            //suite = new ExchangeBasicTests<OptionAccount, OptionContract, OptionOrder, OptionPosition>().SetExchange("LittleQuant.Exchanges.Kingstar.KingstarOptionExchange, Kingstar");
            //RunTests(suite);

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
