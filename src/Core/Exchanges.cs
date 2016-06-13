using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Text;
using System.Threading.Tasks;

namespace LittleQuant.Core
{
    public interface IExchange
    {
        event Action<string> Log;

        event Action<Trade> Trade;

        /// <summary>
        /// 为本接口的使用作准备工作，诸如：连接、登录、更新账户、查合约之类的操作
        /// </summary>
        void Initialize();
    }

    /// <summary>
    /// 抽象交易所。提供行情获取、交易请求、账号查询等操作
    /// </summary>
    public interface IExchange<TAccount, TInstrument, TOrder, TPosition> : IExchange
        where TAccount : IAccount<TPosition, TOrder>
        where TInstrument : IInstrument
        where TOrder : IOrder
        where TPosition : IPosition
    {
        /// <summary>
        /// 获取交易账户
        /// </summary>
        TAccount Account { get; }

        /// <summary>
        /// 所有合约
        /// </summary>
        IEnumerable<TInstrument> Instruments { get; }

        /// <summary>
        /// 根据标识获取单个合约
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        TInstrument GetInstrument(string id);

        /// <summary>
        /// 下单
        /// </summary>
        /// <param name="order"></param>
        void SubmitOrder(TOrder order);

        /// <summary>
        /// 撤单
        /// </summary>
        /// <param name="order"></param>
        void CancelOrder(TOrder order);

        /// <summary>
        /// 获取某个合约当前的行情
        /// </summary>
        /// <param name="instrument"></param>
        /// <returns></returns>
        Market Market(TInstrument instrument);
    }

    /// <summary>
    /// 期权交易所
    /// </summary>
    public interface IOptionExchange : IExchange<OptionAccount, OptionContract, OptionOrder, OptionPosition>
    {
    }

    /// <summary>
    /// 股票交易所
    /// </summary>
    public interface IStockExchange : IExchange<StockAccount, Stock, StockOrder, StockPosition>
    { }
}
