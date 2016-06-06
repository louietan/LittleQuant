//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///@system 期货交易测试示例
///@company SunGard China
///@fileTraderApi_Sample
///@brief 交易Api Sample
///@history 
///20140801	Kingstar IRDG		kevin 创建该文件
///运行此程序前请确保 IP端口 BrokerID 用户名密码及授权文件正确
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdio.h"
#include "KSTradeAPI.h"
#include <iostream>
#ifdef WIN32
#include "windows.h"
#endif

using namespace std;
//使用KingstarAPI命名空间
using namespace KingstarAPI;

class CTraderApiSample : public CThostFtdcTraderSpi ,public CKSVocSpi
{
public: 
	CTraderApiSample(CThostFtdcTraderApi *pUserApi, TThostFtdcBrokerIDType chBrokerID, TThostFtdcUserIDType chUserID, TThostFtdcPasswordType chPassword, TThostFtdcContractCodeType chContract)
	{
		m_nRequestID = 0;
		m_pUserApi = pUserApi;
		strncpy(m_chBrokerID, chBrokerID, sizeof(m_chBrokerID)-1);
		strncpy(m_chUserID, chUserID, sizeof(m_chUserID)-1);
		strncpy(m_chPassword, chPassword, sizeof(m_chPassword)-1);
		strncpy(m_chContract, chContract, sizeof(m_chContract)-1);
	}


	~CTraderApiSample() {}

	// After making a succeed connection with the server, the client should send the login request to the server.
	virtual void OnFrontConnected()
	{
		printf("OnFrontConnected:\n");

		CThostFtdcReqUserLoginField reqUserLogin;
		memset(&reqUserLogin, 0, sizeof(reqUserLogin));
		// set BrokerID
		printf("BrokerID:%s\n", m_chBrokerID);
		strcpy(reqUserLogin. BrokerID, m_chBrokerID);

		// set user id
		printf("userid:%s\n", m_chUserID);
		strcpy(reqUserLogin.UserID, m_chUserID);

		// set password
		printf("password:%s\n",m_chPassword);
		strcpy(reqUserLogin.Password, m_chPassword);

		// send the login request
		m_pUserApi->ReqUserLogin(&reqUserLogin, m_nRequestID++ );
	}

	//When the connection between client and the server disconnected,the follwing function will be called.
	virtual void OnFrontDisconnected(int nReason)
	{ 
		//  In this  case,  API  will reconnect，the client application can ignore this.
		printf("OnFrontDisconnected.\n");
	} 

	virtual void OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus)
	{
		printf("OnRtnInstrumentStatus:");
		if (NULL != pInstrumentStatus)
		{
			printf("%s-%c-%c",pInstrumentStatus->ExchangeID, pInstrumentStatus->InstrumentStatus, pInstrumentStatus->EnterReason);
		}
		printf("\n");
	}

	///报单录入错误回报
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
	{
		printf("OnErrRtnOrderInsert:\n");

	}

	///报单操作错误回报
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
	{
		printf("OnErrRtnOrderAction:\n");
	}

	// After receiving the login request from the client，the server will send the following response to notify the client whether the login success or not.
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspUserLogin:");
		if (pRspUserLogin != NULL)
		{
			printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|%d|%d|%s|", 
				pRspUserLogin->BrokerID,					// 经纪公司代码
				pRspUserLogin->UserID,						// 用户代码
				pRspUserLogin->TradingDay,					// 交易日
				pRspUserLogin->SystemName,					// 交易系统名称
				pRspUserLogin->LoginTime,					// 登陆成功时间
				pRspUserLogin->SHFETime,					// 上期所时间
				pRspUserLogin->DCETime,						// 大商所时间
				pRspUserLogin->CZCETime,					// 郑商所时间
				pRspUserLogin->FFEXTime,					// 中金所时间
				pRspUserLogin->FrontID,						// frond id
				pRspUserLogin->SessionID,					// session id
				pRspUserLogin->MaxOrderRef					// 最大报单引用
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
		if (pRspInfo->ErrorID != 0)
		{
			// in case any login failure, the client should handle this error.
			printf("Failed to login, errorcode=%d errormsg=%s requestid=%d chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
			return;
		}

		// get trading day
		printf("%s\n",m_pUserApi->GetTradingDay());
		// qryInvestor request.
		CThostFtdcQryInvestorField Investor;
		memset(&Investor, 0, sizeof(Investor));
		// broker id 
		strcpy(Investor.BrokerID, m_chBrokerID);
		// investor ID 
		strcpy(Investor.InvestorID, m_chUserID);

		m_pUserApi->ReqQryInvestor(&Investor, m_nRequestID++ );
	}

	// investor response
	virtual void OnRspQryInvestor(CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryInvestor:");
		if (NULL != pInvestor)
		{
			printf("%s|%s|%s|%s|%s|%s|%c|%s|%s|",
				pInvestor->InvestorID,						// 投资者代码
				pInvestor->InvestorName,					// 投资者名称
				pInvestor->IdentifiedCardNo,				// 证件号码
				pInvestor->Telephone,						// 联系电话
				pInvestor->Address,							// 通讯地址
				pInvestor->InvestorGroupID,					// 投资者分组代码
				pInvestor->IdentifiedCardType,				// 证件类型
				pInvestor->Mobile,							// 手机
				pInvestor->OpenDate);						// 开户日期
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg); 
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// tradeaccount request.
			CThostFtdcQryTradingAccountField TradingAccount;
			memset(&TradingAccount, 0, sizeof(TradingAccount));
			// broker id 
			strcpy(TradingAccount.BrokerID, m_chBrokerID);
			// investor ID 
			strcpy(TradingAccount.InvestorID, m_chUserID);

			m_pUserApi->ReqQryTradingAccount(&TradingAccount, m_nRequestID++ );
		}
	}

	// tradeaccount response
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryTradingAccount:");
		if (NULL != pTradingAccount)
		{
			printf(":%s|%.04f|%.04f|%.04f|%.04f|%.04f|%.04f|%.04f|%.04f|%.04f|%.04f|%.04f|",
				pTradingAccount->AccountID,					// 账号
				pTradingAccount->PreBalance,				// 上次结算准备金
				pTradingAccount->Available,					// 可用资金
				pTradingAccount->Commission,				// 手续费
				pTradingAccount->PositionProfit,			// 持仓盈亏
				pTradingAccount->CloseProfit,				// 平仓盈亏
				pTradingAccount->FrozenCommission,			// 冻结的手续费
				pTradingAccount->FrozenCash,				// 冻结的资金
				pTradingAccount->CurrMargin,				// 当前保证金总额
				pTradingAccount->ExchangeMargin,			// 交易所保证金
				pTradingAccount->Mortgage,					// 质押金额
				pTradingAccount->Credit);					// 信用额度
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		// QryExchange
		CThostFtdcQryExchangeField QryExchange;
		memset(&QryExchange, 0, sizeof(QryExchange));
		// exchange id
		//交易所代码 上期SHFE  大商DCE  郑商CZCE  中金CFFEX  原油INE  上证SSE  深证SZSE
		strcpy(QryExchange.ExchangeID, "SHFE");

		m_pUserApi->ReqQryExchange(&QryExchange, m_nRequestID++ );
	}

	// RspQryExchange
	virtual void OnRspQryExchange(CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryExchange:");
		if (NULL != pExchange)
		{
			printf("%s|%s", 
				pExchange->ExchangeID,					// 交易所代码
				pExchange->ExchangeName);				// 交易所名称
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// RspQryInstrument
			CThostFtdcQryInstrumentField QryInstrument;
			memset(&QryInstrument, 0, sizeof(QryInstrument));
			// exchange id
			strcpy(QryInstrument.ExchangeID, "SHFE");
			strcpy(QryInstrument.InstrumentID, m_chContract);

			m_pUserApi->ReqQryInstrument(&QryInstrument, m_nRequestID++ );
		}
	}

	// RspQryInstrument
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryInstrument:");
		if (NULL != pInstrument)
		{
			printf("%s|%s|%s|%d|%s|%s|%.04f|%d|", 
				pInstrument->ExchangeID,							// 交易所代码
				pInstrument->InstrumentID,							// 合约代码
				pInstrument->InstrumentName,						// 合约名称
				pInstrument->VolumeMultiple,						// 合约数量乘数
				pInstrument->ExpireDate,							// 到期日
				pInstrument->ProductID,								// 产品代码
				pInstrument->PriceTick,								// 最小变动价位
				nRequestID);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// QryInvestorPositionDetail
			CThostFtdcQryInvestorPositionDetailField InvestorPositionDetail;
			memset(&InvestorPositionDetail, 0, sizeof(InvestorPositionDetail));
			// broker id 
			strcpy(InvestorPositionDetail.BrokerID, m_chBrokerID);
			// investor id
			strcpy(InvestorPositionDetail.InvestorID, m_chUserID);

			m_pUserApi->ReqQryInvestorPositionDetail(&InvestorPositionDetail, m_nRequestID++ );
		}
	}

	// QryInvestorPositionDetail response
	virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryInvestorPositionDetail:");
		if (NULL != pInvestorPositionDetail)
		{
			printf("%s|%s|%s|%s|%s|%c|%c|%d|%.04f|%.04f|%.04f|",
				pInvestorPositionDetail->TradingDay,			// 交易日
				pInvestorPositionDetail->OpenDate,				// 开仓日期
				pInvestorPositionDetail->TradeID,				// 成交编号
				pInvestorPositionDetail->InvestorID,			// 投资者代码
				pInvestorPositionDetail->ExchangeID,			// 交易所代码
				pInvestorPositionDetail->Direction,				// 买卖标志
				pInvestorPositionDetail->HedgeFlag,				// 投保标志
				pInvestorPositionDetail->Volume,				// 数量
				pInvestorPositionDetail->OpenPrice,				// 开仓价格
				pInvestorPositionDetail->Margin,				// 投资者保证金
				pInvestorPositionDetail->ExchMargin				// 交易所保证金
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// QryInstrumentMarginRate
			CThostFtdcQryInstrumentMarginRateField InstrumentMarginRate;
			memset(&InstrumentMarginRate, 0, sizeof(InstrumentMarginRate));
			// broker id 
			strcpy(InstrumentMarginRate.BrokerID, m_chBrokerID);
			// investor id
			strcpy(InstrumentMarginRate.InvestorID, m_chUserID);
			// instrument id
			strcpy(InstrumentMarginRate.InstrumentID, m_chContract);
			//strcpy(InstrumentMarginRate.InstrumentID, "");
			m_pUserApi->ReqQryInstrumentMarginRate(&InstrumentMarginRate, m_nRequestID++ );
		}
	}

	// QryInstrumentMarginRate response
	virtual void OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryInstrumentMarginRate:");
		if (NULL != pInstrumentMarginRate)
		{
			printf("%s|%s|%.04f|%.04f|%.04f|%.04f|",
				pInstrumentMarginRate->InvestorID,						// 投资者代码
				pInstrumentMarginRate->InstrumentID,					// 合约代码
				pInstrumentMarginRate->LongMarginRatioByMoney,			// 多头保证金率
				pInstrumentMarginRate->LongMarginRatioByVolume,			// 多头保证金费
				pInstrumentMarginRate->ShortMarginRatioByMoney,			// 空头保证金率
				pInstrumentMarginRate->ShortMarginRatioByVolume);		// 空头保证金费
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);


		if (bIsLast == true)
		{
			// QryInstrumentCommissionRate
			CThostFtdcQryInstrumentCommissionRateField InstrumentCommissionRate;
			memset(&InstrumentCommissionRate, 0, sizeof(InstrumentCommissionRate));
			// broker id 
			strcpy(InstrumentCommissionRate.BrokerID, m_chBrokerID);
			// investor id
			strcpy(InstrumentCommissionRate.InvestorID, m_chUserID);
			// instrument id
			strcpy(InstrumentCommissionRate.InstrumentID, m_chContract);
			m_pUserApi->ReqQryInstrumentCommissionRate(&InstrumentCommissionRate, m_nRequestID++ );
		}
	}

	// QryInstrumentCommissionRate response
	virtual void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryInstrumentCommissionRate:");
#if 0
		if (NULL != pInstrumentCommissionRate)
		{
			printf("%s|%s|%.04f|%.04f|%.04f|%.04f|%.04f|%.04f|",
				pInstrumentCommissionRate->InvestorID,						// 投资者代码
				pInstrumentCommissionRate->InstrumentID,					// 合约代码
				pInstrumentCommissionRate->OpenRatioByMoney,				// 开仓手续费率
				pInstrumentCommissionRate->OpenRatioByVolume,				// 开仓手续费
				pInstrumentCommissionRate->CloseRatioByMoney,				// 平仓手续费率
				pInstrumentCommissionRate->CloseRatioByVolume,				// 平仓手续费
				pInstrumentCommissionRate->CloseTodayRatioByMoney,			// 平今手续费率
				pInstrumentCommissionRate->CloseTodayRatioByVolume);		// 平今手续费
		}
#endif
		if (NULL != pInstrumentCommissionRate)
		{
			printf("%s|%s|%.08f|%.08f|%.08f|%.08f|%.08f|%.08f|",
				pInstrumentCommissionRate->InvestorID,						// 投资者代码
				pInstrumentCommissionRate->InstrumentID,					// 合约代码
				pInstrumentCommissionRate->OpenRatioByMoney,				// 开仓手续费率
				pInstrumentCommissionRate->OpenRatioByVolume,				// 开仓手续费
				pInstrumentCommissionRate->CloseRatioByMoney,				// 平仓手续费率
				pInstrumentCommissionRate->CloseRatioByVolume,				// 平仓手续费
				pInstrumentCommissionRate->CloseTodayRatioByMoney,			// 平今手续费率
				pInstrumentCommissionRate->CloseTodayRatioByVolume);		// 平今手续费
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// QryDepthMarketData 
			CThostFtdcQryDepthMarketDataField DepthMarketData;
			memset(&DepthMarketData, 0, sizeof(DepthMarketData));
			// instrument ID 
			strcpy(DepthMarketData.InstrumentID, m_chContract);

			m_pUserApi->ReqQryDepthMarketData(&DepthMarketData, m_nRequestID++ );
		}
	}

	// output the DepthMarketData result 
	virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryDepthMarketData:");
		if(pDepthMarketData != NULL)
		{
			printf("%s|%s|%.04f|%.04f|%.04f|%.04f|%.04f|%d|%.04f|%.04f|%.04f|%d|%d|%.04f|%.04f|%.04f|%.04f|%.04f|%s|%d|%.04f|%d|%.04f|%d|%.04f|%d|%.04f|%d|%.04f|%d|%.04f|%d|%.04f|%d|%.04f|",
				pDepthMarketData->ExchangeID,					// 交易所代码
				pDepthMarketData->InstrumentID,					// 合约代码
				pDepthMarketData->PreClosePrice,				// 昨收盘
				pDepthMarketData->OpenPrice,					// 今开盘
				pDepthMarketData->HighestPrice,					// 最高价
				pDepthMarketData->LowestPrice,					// 最低价
				pDepthMarketData->LastPrice,					// 最新价
				pDepthMarketData->Volume,						// 数量
				pDepthMarketData->Turnover,						// 成交金额
				pDepthMarketData->BidPrice1,					// 申买价一
				pDepthMarketData->AskPrice1,					// 申卖价一
				pDepthMarketData->BidVolume1,					// 申买量一
				pDepthMarketData->AskVolume1,					// 申卖量一
				pDepthMarketData->UpperLimitPrice,				// 涨停板价
				pDepthMarketData->LowerLimitPrice,				// 跌停板价
				pDepthMarketData->PreSettlementPrice,			// 上次结算价
				pDepthMarketData->SettlementPrice,				// 本次结算价
				pDepthMarketData->OpenInterest,					// 持仓量
				pDepthMarketData->TradingDay,					// 交易日
				pDepthMarketData->BidVolume2,					// 申买量二
				pDepthMarketData->BidPrice2,					// 申买价二
				pDepthMarketData->BidVolume3,					// 申买量三
				pDepthMarketData->BidPrice3,					// 申买价三
				pDepthMarketData->BidVolume4,					// 申买量四
				pDepthMarketData->BidPrice4,					// 申买价四
				pDepthMarketData->BidVolume5,					// 申买量五
				pDepthMarketData->BidPrice5,					// 申买价五
				pDepthMarketData->AskVolume2,					// 申卖量二
				pDepthMarketData->AskPrice2,					// 申卖价二
				pDepthMarketData->AskVolume3,					// 申卖量三
				pDepthMarketData->AskPrice3,					// 申卖价三
				pDepthMarketData->AskVolume4,					// 申卖量四
				pDepthMarketData->AskPrice4,					// 申卖价四
				pDepthMarketData->AskVolume5,					// 申卖量五
				pDepthMarketData->AskPrice5						// 申卖价五
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		//return;

		if (bIsLast == true)
		{
			// send order insertion request.
			CThostFtdcInputOrderField ord;
			memset(&ord, 0, sizeof(ord));
			// broker id 
			strcpy(ord.BrokerID, m_chBrokerID); 
			// investor ID 
			strcpy(ord.InvestorID, m_chUserID); 
			// instrument ID 
			strcpy(ord.InstrumentID, m_chContract);
			//下单需填交易所代码
			strcpy(ord.ExchangeID, "SHFE");
			///order reference 
			strcpy(ord.OrderRef, "9");  
			// user id 
			strcpy(ord.UserID, m_chUserID); 
			// order price type 
			ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice; 
			// direction 
			ord.Direction = THOST_FTDC_D_Buy; 
			// combination order’s offset flag 
			ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
			//strcpy(ord.CombOffsetFlag, "0"); 
			// combination or hedge flag 
			ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
			//strcpy(ord.CombHedgeFlag, "0");
			// price 
			ord.LimitPrice = 17000; 
			// volume 
			ord.VolumeTotalOriginal = 1; 
			// valid date 
			ord.TimeCondition = THOST_FTDC_TC_GFD; 
			// GTD DATE 
			strcpy(ord.GTDDate, ""); 
			// volume condition 
			ord.VolumeCondition = THOST_FTDC_VC_AV; 
			// min volume 
			ord.MinVolume = 0; 
			// trigger condition 
			ord.ContingentCondition = THOST_FTDC_CC_Immediately; 
			// stop price 
			ord.StopPrice = 0; 
			// force close reason 
			ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose; 
			// auto suspend flag 
			ord.IsAutoSuspend = 0;
			// request id
			ord.RequestID = m_nRequestID;

			m_pUserApi->ReqOrderInsert(&ord, m_nRequestID++ );
		}

		if (bIsLast == true)
		{
			// qryorder
			CThostFtdcQryOrderField QryOrder;
			memset(&QryOrder, 0, sizeof(QryOrder));
			// broker id 
			strcpy(QryOrder.BrokerID, m_chBrokerID);
			// investor ID 
			strcpy(QryOrder.InvestorID, m_chUserID);

			m_pUserApi->ReqQryOrder(&QryOrder, m_nRequestID++ );
		}

	}
	// order insertion response 
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int  nRequestID, bool bIsLast) 
	{
		printf("OnRspOrderInsert:");
		if (NULL != pInputOrder)
		{
			printf("%s", pInputOrder->OrderRef);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

	}; 

	// order insertion return 
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder) 
	{
		printf("OnRtnOrder:");
		if (NULL != pOrder)
		{
			printf("%d|%s|OrderSysID:%s|OrderLocalID:%s|%s|%s|%c|%s|%c|%s|%s|%d|%.04f|%d|%d|%s|%s|%s|%c|%c|%c|%c|%.04f|%s|%s|",
				pOrder->SequenceNo,							// 序号	
				pOrder->InvestorID,							// 客户号
				pOrder->OrderSysID,							// 委托号
				pOrder->OrderLocalID,						// 本地报单编号
				pOrder->ExchangeID,							// 交易所代码
				pOrder->InstrumentID,						// 合约号
				pOrder->OrderStatus,						// 报单状态
				pOrder->StatusMsg,							// 状态信息
				pOrder->Direction,							// 买卖标记
				pOrder->CombOffsetFlag,						// 开平仓标志
				pOrder->CombHedgeFlag,						// 投保标记
				pOrder->VolumeTotalOriginal,				// 委托数量
				pOrder->LimitPrice,							// 委托价格
				pOrder->VolumeTraded,						// 成交数量
				pOrder->VolumeTotal,						// 未成交数量
				pOrder->TradingDay,							// 交割期
				pOrder->InsertTime,							// 委托时间
				pOrder->CancelTime,							// 撤单时间
				pOrder->OrderType,							// 报单类型
				pOrder->OrderSource,						// 报单来源
				pOrder->OrderPriceType,						// 报单价格条件
				pOrder->TimeCondition,						// 有效期类型
				pOrder->StopPrice,							// 止损价
				pOrder->ActiveTime,							// 激活时间
				pOrder->OrderRef							// 报单引用
				); 
		}
		printf("\n");
		printf("RequestID=[%d]\n", pOrder->RequestID);

		// order insertion success, then send order action request.
		if (pOrder->OrderStatus == THOST_FTDC_OST_NoTradeQueueing && atoi(pOrder->OrderSysID) != 0)
		{
			// delete ord
			CThostFtdcInputOrderActionField ord;
			memset(&ord, 0, sizeof(ord));
			// broker id 
			strcpy(ord.BrokerID, m_chBrokerID); 
			// investor ID 
			strcpy(ord.InvestorID, m_chUserID);
			// order action refernce
			ord.OrderActionRef=000000000001;

			/// order reference 
			strcpy(ord.OrderRef, pOrder->OrderRef);		// *必传1
			// front id
			ord.FrontID = pOrder->FrontID;				// *必传2
			// session id
			ord.SessionID = pOrder->SessionID;			// *必传3

			// exchange ID
			strcpy(ord.ExchangeID, pOrder->ExchangeID);	// *必传4
			// 报单到交易所后可用ExchangeID+OrderSysID撤单，否则用OrderRef+FrontID+SessionID+ExchangeID撤单
			// action order ID
			strcpy(ord.OrderSysID,pOrder->OrderSysID);	

			//  action order num(unavailable yet)
			ord.VolumeChange=0;
			// instrument ID 
			strcpy(ord.InstrumentID, pOrder->InstrumentID); 
			// user id 
			strcpy(ord.UserID, m_chUserID); 

			m_pUserApi->ReqOrderAction(&ord, m_nRequestID++ ); 
		}
	}

	///trade return
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade)
	{
		static int s_nTotalBuy = 0;
		static int s_nTotalSell = 0;

		printf("OnRtnTrade:");
		if (NULL != pTrade)
		{
			if (pTrade->Direction == THOST_FTDC_D_Buy)
				s_nTotalBuy += pTrade->Volume;
			else if (pTrade->Direction == THOST_FTDC_D_Sell)
				s_nTotalSell += pTrade->Volume;
			else
				printf ("invalid direction:%c\n", pTrade->Direction);

			printf("%d|%s|%s|%s|%s|成交|%c|%c|%c|%d|%.04f|%s|%s|%s|%s|s_nTotalBuy=%d|s_nTotalSell=%d|",
				pTrade->SequenceNo,					// 序号
				pTrade->InvestorID,					// 客户号
				pTrade->ExchangeID,					// 交易所代码
				pTrade->OrderSysID,					// 委托单号
				pTrade->InstrumentID,				// 合约编码
				pTrade->Direction,					// 买卖标记
				pTrade->OffsetFlag,					// 开平仓标志
				pTrade->HedgeFlag,					// 投保标记
				pTrade->Volume,						// 成交数量
				pTrade->Price,						// 成交价格
				pTrade->TradeID,					// 成交号
				pTrade->TradingDay,					// 交割期
				pTrade->TradeTime,					// 成交时间
				pTrade->OrderRef,					// 报单引用
				s_nTotalBuy,
				s_nTotalSell
				);
		}
		printf("\n");
	}

	// output the order action result 
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspOrderAction:");
		if (NULL != pInputOrderAction)
		{
			printf("%s|OrderSysID:%s|%s|%s|%.04f|",
				pInputOrderAction->InvestorID,							// 客户号
				pInputOrderAction->OrderSysID,							// 委托号
				pInputOrderAction->ExchangeID,							// 交易所代码
				pInputOrderAction->InstrumentID,						// 合约号
				pInputOrderAction->LimitPrice							// 委托价格
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	}

	// qryorder return
	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryOrder:");
		if(pOrder != NULL)
		{
			printf("%s", pOrder->OrderSysID);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{	
			// qrytrade
			CThostFtdcQryTradeField QryTrade;
			memset(&QryTrade, 0, sizeof(QryTrade));
			// broker id 
			strcpy(QryTrade.BrokerID, m_chBrokerID); 
			// investor ID 
			strcpy(QryTrade.InvestorID, m_chUserID);
			// start time
			strcpy(QryTrade.TradeTimeStart, "20110530");
			// end time
			strcpy(QryTrade.TradeTimeEnd, "20110601");
			// exchange id
			strcpy(QryTrade.ExchangeID, "SHFE");
			// instructment id
			strcpy(QryTrade.InstrumentID, m_chContract);

			m_pUserApi->ReqQryTrade(&QryTrade, m_nRequestID++ );
		}
	}

	// qrytrade return
	virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryTrade:");
		if(pTrade != NULL)
		{
			printf("%s|%s|%s|%s|%c|%c|%c|%d|%.04f|%s|%s|%s|%s|%d",
				pTrade->InvestorID,							// 客户号
				pTrade->ExchangeID,							// 交易所代码
				pTrade->OrderSysID,							// 报单编号
				pTrade->InstrumentID,						// 合约代码
				pTrade->Direction,							// 买卖标记
				pTrade->OffsetFlag,							// 开平标志
				pTrade->HedgeFlag,							// 投保标记
				pTrade->Volume,								// 成交数量
				pTrade->Price,								// 成交价格
				pTrade->TradeID,							// 成交号
				pTrade->TradeDate,							// 日期
				pTrade->TradingDay,							// 交易日
				pTrade->TradeTime,							// 成交时间
				pTrade->SequenceNo							// 序号
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// qry position
			CThostFtdcQryInvestorPositionField QryInvestorPosition;
			memset(&QryInvestorPosition, 0, sizeof(QryInvestorPosition));
			// broker id 
			strcpy(QryInvestorPosition.BrokerID, m_chBrokerID); 
			// investor ID 
			strcpy(QryInvestorPosition.InvestorID, m_chUserID);
			// instrument id
			strcpy(QryInvestorPosition.InstrumentID, m_chContract);

			m_pUserApi->ReqQryInvestorPosition(&QryInvestorPosition, m_nRequestID++ );;
		}
	}

	// QryInvestorPosition return
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryInvestorPosition:");
		if(pInvestorPosition != NULL)
		{
			printf("%s|%d|%d|%.04f|%.04f|%s|%c|%.04f|%c|%s|%.04f|", 
				pInvestorPosition->InvestorID,					// 客户号
				pInvestorPosition->Position,					// 今日总持仓
				pInvestorPosition->TodayPosition,				// 今日现持仓
				pInvestorPosition->PositionCost,				// 持仓成本	
				pInvestorPosition->OpenCost,					// 开仓成本
				pInvestorPosition->InstrumentID,				// 合约代码
				pInvestorPosition->HedgeFlag,					// 投机套保标志
				pInvestorPosition->PositionProfit,				// 持仓盈亏
				pInvestorPosition->PosiDirection,				// 持仓多空方向
				pInvestorPosition->TradingDay,					// 交易日
				pInvestorPosition->UseMargin					// 占用的保证金
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{	
			// qry CFMMC key
			CThostFtdcQryCFMMCTradingAccountKeyField CFMMCTradingAccountKey;
			memset(&CFMMCTradingAccountKey, 0, sizeof(CFMMCTradingAccountKey));
			// broker id
			strcpy(CFMMCTradingAccountKey.BrokerID,m_chBrokerID);
			// user id
			strcpy(CFMMCTradingAccountKey.InvestorID, m_chUserID);

			m_pUserApi->ReqQryCFMMCTradingAccountKey(&CFMMCTradingAccountKey, m_nRequestID++ );
		}
	}

	// QryCFMMCTradingAccountKey return
	virtual void OnRspQryCFMMCTradingAccountKey(CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryCFMMCTradingAccountKey:");
		if (NULL != pCFMMCTradingAccountKey)
		{
			printf("%s|%s|%d|%s|",
				pCFMMCTradingAccountKey->AccountID,				   // 投资者帐号
				pCFMMCTradingAccountKey->ParticipantID,			   // 经纪公司统一编码
				pCFMMCTradingAccountKey->KeyID,					   // 密钥编号
				pCFMMCTradingAccountKey->CurrentKey				   // 动态密钥
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// password update
			CThostFtdcUserPasswordUpdateField UserPasswordUpdate;
			memset(&UserPasswordUpdate, 0, sizeof(UserPasswordUpdate));
			// broker id
			strcpy(UserPasswordUpdate.BrokerID,m_chBrokerID);
			// user id
			strcpy(UserPasswordUpdate.UserID, m_chUserID);
			// old password
			strcpy(UserPasswordUpdate.OldPassword, "123456");
			// new password
			strcpy(UserPasswordUpdate.NewPassword, "123456");

			m_pUserApi->ReqUserPasswordUpdate(&UserPasswordUpdate, m_nRequestID++ );
		}
	}

	// PasswordUpdate return
	virtual void OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspUserPasswordUpdate:");
		if (NULL != pUserPasswordUpdate)
		{
			printf("%s|%s|%s|",
				pUserPasswordUpdate->UserID,			// 客户号
				pUserPasswordUpdate->OldPassword,		// 旧口令
				pUserPasswordUpdate->NewPassword		// 新口令
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// account password update
			CThostFtdcTradingAccountPasswordUpdateField TradingAccountPasswordUpdate ;
			memset(&TradingAccountPasswordUpdate, 0, sizeof(TradingAccountPasswordUpdate));
			// broker id
			strcpy(TradingAccountPasswordUpdate.BrokerID,m_chBrokerID);
			// account id
			strcpy(TradingAccountPasswordUpdate.AccountID, m_chUserID);
			// old password
			strcpy(TradingAccountPasswordUpdate.OldPassword, "123456");
			// new password
			strcpy(TradingAccountPasswordUpdate.NewPassword, "123456");

			m_pUserApi->ReqTradingAccountPasswordUpdate(&TradingAccountPasswordUpdate, m_nRequestID++ );
		}
	}

	// TradingAccountPasswordUpdate
	virtual void OnRspTradingAccountPasswordUpdate(CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspTradingAccountPasswordUpdate:");
		if (NULL != pTradingAccountPasswordUpdate)
		{
			printf("%s|%s|%s|",
				pTradingAccountPasswordUpdate->AccountID,				// 资金账户
				pTradingAccountPasswordUpdate->OldPassword,				// 旧口令
				pTradingAccountPasswordUpdate->NewPassword				// 新口令
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// SettlementInfo Confirm
			CThostFtdcQrySettlementInfoConfirmField SettlementInfoConfirm;
			memset(&SettlementInfoConfirm, 0, sizeof(SettlementInfoConfirm));
			// broker id
			strcpy(SettlementInfoConfirm.BrokerID, m_chBrokerID);
			// investor ID
			strcpy(SettlementInfoConfirm.InvestorID, m_chUserID);

			m_pUserApi->ReqQrySettlementInfoConfirm(&SettlementInfoConfirm, m_nRequestID++ );
		}
	}

	// QrySettlementInfoConfirm return
	virtual void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQrySettlementInfoConfirm:");
		if (NULL != pSettlementInfoConfirm)
		{
			printf("%s|%s|",
				pSettlementInfoConfirm->InvestorID,					// 客户号
				pSettlementInfoConfirm->ConfirmDate					// 确认日期
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// QrySettlementInfo
			CThostFtdcQrySettlementInfoField QrySettlementInfo;
			memset(&QrySettlementInfo, 0, sizeof(QrySettlementInfo));
			// broker id 
			strcpy(QrySettlementInfo.BrokerID, m_chBrokerID);
			// investor ID 
			strcpy(QrySettlementInfo.InvestorID, m_chUserID);
			// trading day
			strcpy(QrySettlementInfo.TradingDay, "");

			m_pUserApi->ReqQrySettlementInfo(&QrySettlementInfo, m_nRequestID++ );
		}
	}

	///QrySettlementInfo return
	virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQrySettlementInfoConfirm:");
		if(pSettlementInfo != NULL)
		{
			printf("%s|%s|",
				pSettlementInfo->InvestorID,			// 客户号
				pSettlementInfo->Content				// 消息正文
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// QrySettlementInfo
			CThostFtdcQryInvestorPositionCombineDetailField InvestorPositionDetail;
			memset(&InvestorPositionDetail, 0, sizeof(InvestorPositionDetail));
			// broker id 
			strcpy(InvestorPositionDetail.BrokerID, m_chBrokerID); 
			// investor ID 
			strcpy(InvestorPositionDetail.InvestorID, m_chUserID);
			// 
			strcpy(InvestorPositionDetail.CombInstrumentID, "fu1109");

			m_pUserApi->ReqQryInvestorPositionCombineDetail(&InvestorPositionDetail, m_nRequestID++ );
		}
	}

	// QryInvestorPositionCombineDetail return
	virtual void OnRspQryInvestorPositionCombineDetail(CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryInvestorPositionCombineDetail:");
		if(pInvestorPositionCombineDetail != NULL)
		{
			printf("%s|%s|%s|%d|%.04f|",
				pInvestorPositionCombineDetail->InvestorID,					// 客户号
				pInvestorPositionCombineDetail->ExchangeID,					// 交易所代码
				pInvestorPositionCombineDetail->CombInstrumentID,			// 组合合约
				pInvestorPositionCombineDetail->TotalAmt,					// 持仓量
				pInvestorPositionCombineDetail->Margin						// 投资者保证金
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// BankToFuture
			CThostFtdcReqTransferField ReqTransfer;
			memset(&ReqTransfer, 0, sizeof(ReqTransfer));
			// broker id 
			strcpy(ReqTransfer.BrokerID, m_chBrokerID); 
			// AccountID ID 
			strcpy(ReqTransfer.AccountID, m_chUserID);
			// BankPassWord
			strcpy(ReqTransfer.BankPassWord, "123");
			//Password
			strcpy(ReqTransfer.Password, "123");
			// TradeAmount
			ReqTransfer.TradeAmount = 100;
			// BankID
			strcpy(ReqTransfer.BankID, "2");
			// BankAccount
			strcpy(ReqTransfer.BankAccount, "360000016");

			m_pUserApi->ReqFromBankToFutureByFuture(&ReqTransfer, m_nRequestID++ );
		}
	}

	///期货发起银行资金转期货应答
	virtual void OnRspFromBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
	{
		printf("OnRspFromBankToFutureByFuture:");
		if (NULL != pReqTransfer)
		{
			printf("%s|%s|%s|%s|%d|%.04f|%c|%c|",
				pReqTransfer->AccountID,						// 投资者账号
				pReqTransfer->BankID,							// 银行代码
				pReqTransfer->BankAccount,						// 银行账号
				pReqTransfer->TradeTime,						// 操作时间
				pReqTransfer->PlateSerial,						// 银期平台流水号
				pReqTransfer->TradeAmount,						// 转账金额
				pReqTransfer->TransferStatus,					// 处理状态
				pReqTransfer->BankAccType						// 银行类型
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// FutureToBan
			CThostFtdcReqTransferField ReqTransfer;
			memset(&ReqTransfer, 0, sizeof(ReqTransfer));
			// broker id 
			strcpy(ReqTransfer.BrokerID, m_chBrokerID); 
			// AccountID ID 
			strcpy(ReqTransfer.AccountID, m_chUserID);
			// BankPassWord
			strcpy(ReqTransfer.BankPassWord, "123");
			// TradeAmount
			ReqTransfer.TradeAmount = 100;
			// BankID
			strcpy(ReqTransfer.BankID, "2");
			// BankAccount
			strcpy(ReqTransfer.BankAccount, "360000016");

			m_pUserApi->ReqFromFutureToBankByFuture(&ReqTransfer, m_nRequestID++ );
		}
	}

	///期货发起期货资金转银行应答
	virtual void OnRspFromFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspFromFutureToBankByFuture:");
		if (NULL != pReqTransfer)
		{
			printf("%s|%s|%s|%s|%d|%.04f|%c|%c|",
				pReqTransfer->AccountID,						// 投资者账号
				pReqTransfer->BankID,							// 银行代码
				pReqTransfer->BankAccount,						// 银行账号
				pReqTransfer->TradeTime,						// 操作时间
				pReqTransfer->PlateSerial,						// 银期平台流水号
				pReqTransfer->TradeAmount,						// 转账金额
				pReqTransfer->TransferStatus,					// 处理状态
				pReqTransfer->BankAccType						// 银行类型
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// FutureToBank
			CThostFtdcReqQueryAccountField ReqQueryAccount;
			memset(&ReqQueryAccount, 0, sizeof(ReqQueryAccount));
			// AccountID ID 
			strcpy(ReqQueryAccount.AccountID, m_chUserID);
			// BankPassWord
			strcpy(ReqQueryAccount.BankPassWord, "1234");
			// BankID
			strcpy(ReqQueryAccount.BankID, "2");
			// BankAccount
			strcpy(ReqQueryAccount.BankAccount, "360000016");

			m_pUserApi->ReqQueryBankAccountMoneyByFuture(&ReqQueryAccount, m_nRequestID++ );
		}
	}	

	///期货发起查询银行余额应答
	virtual void OnRspQueryBankAccountMoneyByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQueryBankAccountMoneyByFuture:");
		if (NULL != pReqQueryAccount)
		{
			printf("%s|%s|%s|%s|%d|%c|",
				pReqQueryAccount->AccountID,						// 投资者账号
				pReqQueryAccount->BankID,							// 银行代码
				pReqQueryAccount->BankAccount,						// 银行账号
				pReqQueryAccount->TradeTime,						// 操作时间
				pReqQueryAccount->PlateSerial,						// 银期平台流水号
				pReqQueryAccount->BankAccType						// 银行类型
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// TransferSerial
			CThostFtdcQryTransferSerialField QryTransferSerial;
			memset(&QryTransferSerial, 0, sizeof(QryTransferSerial));
			// broker id 
			strcpy(QryTransferSerial.BrokerID, m_chBrokerID); 
			// investor ID 
			strcpy(QryTransferSerial.AccountID, m_chUserID);
			// BankID
			strcpy(QryTransferSerial.BankID, "5");

			m_pUserApi->ReqQryTransferSerial(&QryTransferSerial, m_nRequestID++ );
		}
	}

	// TransferSerial return
	virtual void OnRspQryTransferSerial(CThostFtdcTransferSerialField *pTransferSerial, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryTransferSerial:");
		if (pTransferSerial != NULL)
		{
			printf("%d|%s|%s|%s|%s|%s|%.04f|%s|%c|%c|",
				pTransferSerial->PlateSerial,				// 银期平台流水号
				pTransferSerial->AccountID,					// 投资者账号
				pTransferSerial->BankAccount,				// 银行账号
				pTransferSerial->OperatorCode,				// 操作员
				pTransferSerial->TradingDay,				// 操作日期
				pTransferSerial->TradeTime,					// 操作时间
				pTransferSerial->TradeAmount,				// 交易金额
				pTransferSerial->BankID,					// 银行代码
				pTransferSerial->BankAccType,				// 银行类型
				pTransferSerial->AvailabilityFlag			// 有效标志
				);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			CThostFtdcQryNoticeField QryNotice;
			memset(&QryNotice, 0, sizeof(QryNotice));
			// broker id 
			strcpy(QryNotice.BrokerID, m_chBrokerID);

			m_pUserApi->ReqQryNotice(&QryNotice, m_nRequestID++ );
		}
	}

	// notice return
	virtual void OnRspQryNotice(CThostFtdcNoticeField *pNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryNotice:");
		if (NULL != pNotice)
		{
			printf(":%s", pNotice->Content);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			CThostFtdcQryTradingNoticeField QryTradingNotice;
			memset(&QryTradingNotice, 0, sizeof(QryTradingNotice)-1);
			// broker id 
			strcpy(QryTradingNotice.BrokerID, m_chBrokerID);
			// investor ID
			strcpy(QryTradingNotice.InvestorID, m_chUserID);

			m_pUserApi->ReqQryTradingNotice(&QryTradingNotice, m_nRequestID++ );
		}
	}

	// trade notice return
	virtual void OnRspQryTradingNotice(CThostFtdcTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryTradingNotice:");
		if (NULL != pTradingNotice)
		{
			printf(":%s", pTradingNotice->FieldContent);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		if (bIsLast == true)
		{
			// logout
			CThostFtdcUserLogoutField UserLogout;
			memset(&UserLogout, 0, sizeof(UserLogout));
			// broker id 
			strcpy(UserLogout.BrokerID, m_chBrokerID); 
			// investor ID 
			strcpy(UserLogout.UserID, m_chUserID);
			m_pUserApi->ReqUserLogout(&UserLogout, m_nRequestID++ );
		}
	}

	// logout return
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspUserLogout:");
		if (NULL != pUserLogout)
		{
			printf("%s",pUserLogout->UserID);
		}
		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

		return;
	}

	//查询开盘前的持仓明细应答
	virtual void OnRspQryInvestorOpenPosition(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		printf("OnRspQryInvestorOpenPosition:");

		printf("\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	}
private:     
	// 实例指针
	CThostFtdcTraderApi *m_pUserApi;
	// 经济公司代码
	TThostFtdcBrokerIDType m_chBrokerID;
	// 用户名
	TThostFtdcUserIDType m_chUserID;
	// 用户密码
	TThostFtdcPasswordType m_chPassword;
	// 合约
	TThostFtdcContractCodeType m_chContract;
public:
	// 请求号
	int m_nRequestID;
};

class CCosHandler:public CKSCosSpi
{
public:
	CCosHandler ()
	{
	}

	~CCosHandler ()
	{
	}

	//条件单录入响应
	virtual void OnRspInitInsertConditionalOrder (CKSConditionalOrderOperResultField *pInitInsertConditionalOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
	{
		printf ("hello insert\n");
		if (pInitInsertConditionalOrder != NULL)
		{
			cout << "OnRspInitInsertConditionalOrder:"
				<< "经济公司代码:" << pInitInsertConditionalOrder->BrokerID
				<< "客户号:" << pInitInsertConditionalOrder->InvestorID
				<< "条件单编号:" << pInitInsertConditionalOrder->ConditionalOrderID
				<< "本地报单号:" << pInitInsertConditionalOrder->OrderLocalID
				<< "交易所代码:" << pInitInsertConditionalOrder->ExchangeID
				<< "合约编码:" << pInitInsertConditionalOrder->InstrumentID
				<< "报单状态:" << pInitInsertConditionalOrder->OrderStatus
				<< "买卖标记:" << pInitInsertConditionalOrder->Direction
				<< "开平仓标志:" << pInitInsertConditionalOrder->CombOffsetFlag
				<< "投保标记:" << pInitInsertConditionalOrder->CombHedgeFlag
				<< "委托数量:" << pInitInsertConditionalOrder->VolumeTotalOriginal
				<< "委托价格:" << pInitInsertConditionalOrder->LimitPrice
				<< "交易日:" << pInitInsertConditionalOrder->TradingDay
				<< "撤销用户:" << pInitInsertConditionalOrder->UserID
				<< "撤销时间:" << pInitInsertConditionalOrder->CancelTime
				<< "客户代码:" << pInitInsertConditionalOrder->ClientID
				<< "条件单状态:" << pInitInsertConditionalOrder->ConditionalOrderStatus 
				<< "错误信息:" << pInitInsertConditionalOrder->ErrorMsg 
				<< "价格类别:" << pInitInsertConditionalOrder->OrderPriceType 
				<< "触发次数:" << pInitInsertConditionalOrder->TriggeredTimes 
				<< "条件单类别:" << pInitInsertConditionalOrder->OrderType 
				<< "备注:" << pInitInsertConditionalOrder->Memo 
				<< "有效时间:" << pInitInsertConditionalOrder->ActiveTime 
				<< "失效时间:" << pInitInsertConditionalOrder->InActiveTime << endl;
		}
		printf ("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf ("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	}

	//条件单查询响应
	virtual void OnRspQueryConditionalOrder (CKSConditionalOrderOperResultField * pQueryConditionalOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
	{
		printf ("hello query\n");
		if (pQueryConditionalOrder != NULL)
		{
			cout << "OnRspQueryConditionalOrder:"
				<< "经济公司代码:" << pQueryConditionalOrder->BrokerID
				<< "客户号:" << pQueryConditionalOrder->InvestorID
				<< "条件单编号:" << pQueryConditionalOrder->ConditionalOrderID
				<< "本地报单号:" << pQueryConditionalOrder->OrderLocalID
				<< "交易所代码:" << pQueryConditionalOrder->ExchangeID
				<< "合约编码:" << pQueryConditionalOrder->InstrumentID
				<< "报单状态:" << pQueryConditionalOrder->OrderStatus
				<< "买卖标记:" << pQueryConditionalOrder->Direction
				<< "开平仓标志:" << pQueryConditionalOrder->CombOffsetFlag
				<< "投保标记:" << pQueryConditionalOrder->CombHedgeFlag
				<< "委托数量:" << pQueryConditionalOrder->VolumeTotalOriginal
				<< "委托价格:" << pQueryConditionalOrder->LimitPrice
				<< "交易日:" << pQueryConditionalOrder->TradingDay
				<< "撤销用户:" << pQueryConditionalOrder->UserID
				<< "撤销时间:" << pQueryConditionalOrder->CancelTime
				<< "客户代码:" << pQueryConditionalOrder->ClientID
				<< "条件单状态:" << pQueryConditionalOrder->ConditionalOrderStatus
				<< "错误信息:" << pQueryConditionalOrder->ErrorMsg
				<< "价格类别:" << pQueryConditionalOrder->OrderPriceType
				<< "触发次数:" << pQueryConditionalOrder->TriggeredTimes
				<< "条件单类别:" << pQueryConditionalOrder->OrderType
				<< "备注:" << pQueryConditionalOrder->Memo
				<< "有效时间:" << pQueryConditionalOrder->ActiveTime
				<< "失效时间:" << pQueryConditionalOrder->InActiveTime << endl;
		}
		printf ("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf ("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	}

	//条件单修改响应
	virtual void OnRspModifyConditionalOrder (CKSConditionalOrderOperResultField *pModifyConditionalOrder,	CThostFtdcRspInfoField * pRspInfo,	int nRequestID, bool bIsLast)
	{
		printf ("hello modify\n");
		if (pModifyConditionalOrder != NULL)
		{
			cout << "OnRspModifyConditionalOrder:"
				<< "经济公司代码:" << pModifyConditionalOrder->BrokerID
				<< "客户号:" << pModifyConditionalOrder->InvestorID
				<< "条件单编号:" << pModifyConditionalOrder->ConditionalOrderID
				<< "本地报单号:" << pModifyConditionalOrder->OrderLocalID
				<< "交易所代码:" << pModifyConditionalOrder->ExchangeID
				<< "合约编码:" << pModifyConditionalOrder->InstrumentID
				<< "报单状态:" << pModifyConditionalOrder->OrderStatus
				<< "买卖标记:" << pModifyConditionalOrder->Direction
				<< "开平仓标志:" << pModifyConditionalOrder->CombOffsetFlag
				<< "投保标记:" << pModifyConditionalOrder->CombHedgeFlag
				<< "委托数量:" << pModifyConditionalOrder->VolumeTotalOriginal
				<< "委托价格:" << pModifyConditionalOrder->LimitPrice
				<< "交易日:" << pModifyConditionalOrder->TradingDay
				<< "撤销用户:" << pModifyConditionalOrder->UserID
				<< "撤销时间:" << pModifyConditionalOrder->CancelTime
				<< "客户代码:" << pModifyConditionalOrder->ClientID
				<< "条件单状态:" << pModifyConditionalOrder->ConditionalOrderStatus
				<< "错误信息:" << pModifyConditionalOrder->ErrorMsg
				<< "价格类别:" << pModifyConditionalOrder->OrderPriceType
				<< "触发次数:" << pModifyConditionalOrder->TriggeredTimes
				<< "条件单类别:" << pModifyConditionalOrder->OrderType
				<< "备注:" << pModifyConditionalOrder->Memo
				<< "有效时间:" << pModifyConditionalOrder->ActiveTime
				<< "失效时间:" << pModifyConditionalOrder->InActiveTime << endl;
		}
		printf ("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf ("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	}

	//条件单暂停激活响应
	virtual void OnRspPauseConditionalOrder (CKSConditionalOrderOperResultField *pPauseConditionalOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
	{
		printf ("hello pause or active\n");
		if (pPauseConditionalOrder != NULL)
		{
			cout << "OnRspPauseConditionalOrder:"
				<< "经济公司代码:" << pPauseConditionalOrder->BrokerID
				<< "客户号:" << pPauseConditionalOrder->InvestorID
				<< "条件单编号:" << pPauseConditionalOrder->ConditionalOrderID
				<< "本地报单号:" << pPauseConditionalOrder->OrderLocalID
				<< "交易所代码:" << pPauseConditionalOrder->ExchangeID
				<< "合约编码:" << pPauseConditionalOrder->InstrumentID
				<< "报单状态:" << pPauseConditionalOrder->OrderStatus
				<< "买卖标记:" << pPauseConditionalOrder->Direction
				<< "开平仓标志:" << pPauseConditionalOrder->CombOffsetFlag
				<< "投保标记:" << pPauseConditionalOrder->CombHedgeFlag
				<< "委托数量:" << pPauseConditionalOrder->VolumeTotalOriginal
				<< "委托价格:" << pPauseConditionalOrder->LimitPrice
				<< "交易日:" << pPauseConditionalOrder->TradingDay
				<< "撤销用户:" << pPauseConditionalOrder->UserID
				<< "撤销时间:" << pPauseConditionalOrder->CancelTime
				<< "客户代码:" << pPauseConditionalOrder->ClientID
				<< "条件单状态:" << pPauseConditionalOrder->ConditionalOrderStatus
				<< "错误信息:" << pPauseConditionalOrder->ErrorMsg
				<< "价格类别:" << pPauseConditionalOrder->OrderPriceType
				<< "触发次数:" << pPauseConditionalOrder->TriggeredTimes
				<< "条件单类别:" << pPauseConditionalOrder->OrderType
				<< "备注:" << pPauseConditionalOrder->Memo
				<< "有效时间:" << pPauseConditionalOrder->ActiveTime
				<< "失效时间:" << pPauseConditionalOrder->InActiveTime << endl;
		}
		printf ("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf ("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

	}

	//条件单删除响应
	virtual void OnRspRemoveConditionalOrder (CKSConditionalOrderRspResultField *pRemoveConditionalOrder,	CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
	{
		printf ("hello remove\n");
		if (pRemoveConditionalOrder != NULL)
		{
			cout << "OnRspRemoveConditionalOrder:"
				<< "营业部代码:" << pRemoveConditionalOrder->BrokerID
				<< "投资者代码:" << pRemoveConditionalOrder->InvestorID
				<< "条件单编号:" << pRemoveConditionalOrder->ConditionalOrderID << endl;
		}
		printf ("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf ("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	}

	//条件单选择响应
	virtual void OnRspSelectConditionalOrder (CKSConditionalOrderRspResultField	*pSelectConditionalOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
	{
		printf ("hello select\n");
		if (pSelectConditionalOrder != NULL)
		{
			cout << "OnRspSelectConditionalOrder:"
				<< "营业部代码:" << pSelectConditionalOrder->BrokerID
				<< "投资者代码:" << pSelectConditionalOrder->InvestorID
				<< "条件单编号:" << pSelectConditionalOrder->ConditionalOrderID << endl;
		}
		printf ("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf ("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	}

	//盈损单录入响应
	virtual void OnRspInsertProfitAndLossOrder (CKSProfitAndLossOrderOperResultField *pInsertProfitAndLossOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
	{
		printf ("hello profit and loss insert\n");
		if (pInsertProfitAndLossOrder != NULL)
		{
			cout << "OnRspInsertProfitAndLossOrder:"
				<< "营业部代码:" << pInsertProfitAndLossOrder->BrokerID
				<< "投资者代码:" << pInsertProfitAndLossOrder->InvestorID
				<< "止损止盈单号:" << pInsertProfitAndLossOrder->ProfitAndLossOrderID 
				<< "操作员:" << pInsertProfitAndLossOrder->UserID
				<< "投资者名称:" << pInsertProfitAndLossOrder->InvestorName 
				<< "本地报单编号:" << pInsertProfitAndLossOrder->OrderLocalID 
				<< "止损价:" << pInsertProfitAndLossOrder->StopLossPrice 
				<< "止盈价:" << pInsertProfitAndLossOrder->TakeProfitPrice 
				<< "平仓方式:" << pInsertProfitAndLossOrder->CloseMode 
				<< "平仓反向加减价位数:" << pInsertProfitAndLossOrder->Figures 
				<< "行情触发时的最新价:" << pInsertProfitAndLossOrder->LastPrice 
				<< "生成止损止盈单时间:" << pInsertProfitAndLossOrder->ProfitAndLossOrderFormTime 
				<< "生成条件单时间:" <<pInsertProfitAndLossOrder->ConditionalOrderFormTime 
				<< "生成委托单时间:" <<pInsertProfitAndLossOrder->OrderFormTime 
				<< "止损止盈单状态:" << pInsertProfitAndLossOrder->ProfitAndLossOrderStatus 
				<< "条件单编号:" <<pInsertProfitAndLossOrder->ConditionalOrderID 
				<< "交易所代码:" << pInsertProfitAndLossOrder->ExchangeID 
				<< "客户代码:" << pInsertProfitAndLossOrder->ClientID 
				<< "合约代码:" << pInsertProfitAndLossOrder->InstrumentID 
				<< "开平标志:" << pInsertProfitAndLossOrder->CombOffsetFlag 
				<< "投机套保标志:" << pInsertProfitAndLossOrder->CombHedgeFlag 
				<< "买卖方向" << pInsertProfitAndLossOrder->Direction 
				<< "价格" << pInsertProfitAndLossOrder->LimitPrice 
				<< "数量" << pInsertProfitAndLossOrder->VolumeTotalOriginal 
				<< "生成盈损价的方式" <<pInsertProfitAndLossOrder->OffsetValue 
				<< "业务单元" << pInsertProfitAndLossOrder->BusinessUnit 
				<< "条件单触发价格类型" << pInsertProfitAndLossOrder->SpringType 
				<< "浮动止损价" << pInsertProfitAndLossOrder->FloatLimitPrice 
				<< "开仓成交价格" << pInsertProfitAndLossOrder->OpenTradePrice << endl;
		}
		printf ("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf ("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);

	}

	//盈损单修改响应
	virtual void OnRspModifyProfitAndLossOrder (CKSProfitAndLossOrderOperResultField *pModifyProfitAndLossOrder,	CThostFtdcRspInfoField * pRspInfo,	int nRequestID, bool bIsLast)
	{
		printf ("hello profit and loss modify\n");
		if (pModifyProfitAndLossOrder != NULL)
		{
			cout << "OnRspModifyProfitAndLossOrder:"
				<< "营业部代码:" << pModifyProfitAndLossOrder->BrokerID
				<< "投资者代码:" << pModifyProfitAndLossOrder->InvestorID
				<< "止损止盈单号:" << pModifyProfitAndLossOrder->ProfitAndLossOrderID 
				<< "操作员:" << pModifyProfitAndLossOrder->UserID 
				<< "投资者名称:" << pModifyProfitAndLossOrder->InvestorName 
				<< "本地报单编号:" << pModifyProfitAndLossOrder->OrderLocalID 
				<< "止损价:" << pModifyProfitAndLossOrder->StopLossPrice 
				<< "止盈价:" << pModifyProfitAndLossOrder->TakeProfitPrice 
				<< "平仓方式:" << pModifyProfitAndLossOrder->CloseMode 
				<< "平仓反向加减价位数:" << pModifyProfitAndLossOrder->Figures 
				<< "行情触发时的最新价:" << pModifyProfitAndLossOrder->LastPrice 
				<< "生成止损止盈单时间:" << pModifyProfitAndLossOrder->ProfitAndLossOrderFormTime 
				<< "生成条件单时间:" <<pModifyProfitAndLossOrder->ConditionalOrderFormTime 
				<< "生成委托单时间:" <<pModifyProfitAndLossOrder->OrderFormTime 
				<< "止损止盈单状态:" << pModifyProfitAndLossOrder->ProfitAndLossOrderStatus 
				<< "条件单编号:" <<pModifyProfitAndLossOrder->ConditionalOrderID 
				<< "交易所代码:" << pModifyProfitAndLossOrder->ExchangeID 
				<< "客户代码:" << pModifyProfitAndLossOrder->ClientID 
				<< "合约代码:" << pModifyProfitAndLossOrder->InstrumentID 
				<< "开平标志:" << pModifyProfitAndLossOrder->CombOffsetFlag 
				<< "投机套保标志:" << pModifyProfitAndLossOrder->CombHedgeFlag 
				<< "买卖方向" << pModifyProfitAndLossOrder->Direction 
				<< "价格" << pModifyProfitAndLossOrder->LimitPrice 
				<< "数量" << pModifyProfitAndLossOrder->VolumeTotalOriginal 
				<< "生成盈损价的方式" <<pModifyProfitAndLossOrder->OffsetValue 
				<< "业务单元" << pModifyProfitAndLossOrder->BusinessUnit 
				<< "条件单触发价格类型" << pModifyProfitAndLossOrder->SpringType 
				<< "浮动止损价" << pModifyProfitAndLossOrder->FloatLimitPrice 
				<< "开仓成交价格" << pModifyProfitAndLossOrder->OpenTradePrice << endl;
		}
		printf ("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf ("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	}

	//盈损单查询响应
	virtual void OnRspQueryProfitAndLossOrder (CKSProfitAndLossOrderOperResultField *pQueryProfitAndLossOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
	{
		printf ("hello profit and loss query\n");
		if (pQueryProfitAndLossOrder != NULL)
		{
			cout << "OnRspQueryProfitAndLossOrder:"
				<< "营业部代码:" << pQueryProfitAndLossOrder->BrokerID
				<< "投资者代码:" << pQueryProfitAndLossOrder->InvestorID
				<< "止损止盈单号:" << pQueryProfitAndLossOrder->ProfitAndLossOrderID
				<< "操作员:" << pQueryProfitAndLossOrder->UserID
				<< "投资者名称:" << pQueryProfitAndLossOrder->InvestorName
				<< "本地报单编号:" << pQueryProfitAndLossOrder->OrderLocalID
				<< "止损价:" << pQueryProfitAndLossOrder->StopLossPrice
				<< "止盈价:" << pQueryProfitAndLossOrder->TakeProfitPrice
				<< "平仓方式:" << pQueryProfitAndLossOrder->CloseMode
				<< "平仓反向加减价位数:" << pQueryProfitAndLossOrder->Figures
				<< "行情触发时的最新价:" << pQueryProfitAndLossOrder->LastPrice
				<< "生成止损止盈单时间:" << pQueryProfitAndLossOrder->ProfitAndLossOrderFormTime 
				<< "生成条件单时间:" <<pQueryProfitAndLossOrder->ConditionalOrderFormTime
				<< "生成委托单时间:" <<pQueryProfitAndLossOrder->OrderFormTime 
				<< "止损止盈单状态:" << pQueryProfitAndLossOrder->ProfitAndLossOrderStatus 
				<< "条件单编号:" <<pQueryProfitAndLossOrder->ConditionalOrderID 
				<< "交易所代码:" << pQueryProfitAndLossOrder->ExchangeID 
				<< "客户代码:" << pQueryProfitAndLossOrder->ClientID 
				<< "合约代码:" << pQueryProfitAndLossOrder->InstrumentID 
				<< "开平标志:" << pQueryProfitAndLossOrder->CombOffsetFlag 
				<< "投机套保标志:" << pQueryProfitAndLossOrder->CombHedgeFlag 
				<< "买卖方向" << pQueryProfitAndLossOrder->Direction 
				<< "价格" << pQueryProfitAndLossOrder->LimitPrice 
				<< "数量" << pQueryProfitAndLossOrder->VolumeTotalOriginal 
				<< "生成盈损价的方式" <<pQueryProfitAndLossOrder->OffsetValue 
				<< "业务单元" << pQueryProfitAndLossOrder->BusinessUnit 
				<< "条件单触发价格类型" << pQueryProfitAndLossOrder->SpringType 
				<< "浮动止损价" << pQueryProfitAndLossOrder->FloatLimitPrice 
				<< "开仓成交价格" << pQueryProfitAndLossOrder->OpenTradePrice << endl;
		}
		printf ("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf ("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	}

	///止损止盈单删除响应
	virtual void  	OnRspRemoveProfitAndLossOrder (CKSProfitAndLossOrderRemoveField *pRemoveProfitAndLossOrder, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
	{
		printf ("hello profit and loss delete\n");
		if (pRemoveProfitAndLossOrder != NULL)
		{
			cout << "OnRspRemoveProfitAndLossOrder:"
				<< "营业部代码:" << pRemoveProfitAndLossOrder->BrokerID
				<< "投资者代码" << pRemoveProfitAndLossOrder->InvestorID
				<< "止损止盈单号" << pRemoveProfitAndLossOrder->ProfitAndLossOrderID << endl;
		}
		printf ("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf ("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
	}

	///条件单请求选择通知
	virtual void OnRtnCOSAskSelect (CKSCOSAskSelectField * pCOSAskSelect)
	{
		printf ("condition order select Notice\n");
		if (pCOSAskSelect != NULL)
		{
			cout << "OnRtnCOSAskSelect:"
				<< "营业部代码:" << pCOSAskSelect->BrokerID
				<< "操作员:" << pCOSAskSelect->UserID
				<< "投资者代码:" << pCOSAskSelect->InvestorID
				<< "序号" << pCOSAskSelect->SequenceNo
				<< "条件单编号:" << pCOSAskSelect->ConditionalOrderID
				<< "备注:" << pCOSAskSelect->Memo
				<< "选择方式:" << pCOSAskSelect->SelectType << endl;
		}
	}

	///条件单状态通知
	virtual void OnRtnCOSStatus (CKSCOSStatusField * pCOSStatus)
	{
		printf ("condition order status notice\n");
		if (pCOSStatus != NULL)
		{
			cout << "OnRtnCOSStatus:"
				<< "营业部代码:" << pCOSStatus->BrokerID
				<< "操作员:" << pCOSStatus->UserID
				<< "投资者代码:" << pCOSStatus->InvestorID
				<< "序号:" << pCOSStatus->SequenceNo
				<< "条件单编号:" << pCOSStatus->ConditionalOrderID
				<< "条件单状态:" << pCOSStatus->ConditionalOrderStatus
				<< "备注:" << pCOSStatus->Memo
				<< "本地报单编号:" << pCOSStatus->OrderLocalID
				<< "交易所代码:" << pCOSStatus->ExchangeID
				<< "合约代码:" << pCOSStatus->InstrumentID
				<< "报单状态:" << pCOSStatus->OrderStatus
				<< "开平标志:" << pCOSStatus->CombOffsetFlag
				<< "投机套保标志:" << pCOSStatus->CombHedgeFlag
				<< "买卖方向:" << pCOSStatus->Direction
				<< "价格:" << pCOSStatus->LimitPrice
				<< "数量:" << pCOSStatus->VolumeTotalOriginal
				<< "交易日:" << pCOSStatus->TradingDay
				<< "撤销用户:" << pCOSStatus->CancelUserID
				<< "撤销时间:" << pCOSStatus->CancelTime
				<< "客户代码:" << pCOSStatus->ClientID
				<< "业务单元:" << pCOSStatus->BusinessUnit
				<< "报单编号:" << pCOSStatus->OrderSysID
				<< "今成交数量:" << pCOSStatus->VolumeTraded
				<< "剩余数量:" << pCOSStatus->VolumeTotal
				<< "委托时间:" << pCOSStatus->InsertTime
				<< "激活时间:" << pCOSStatus->ActiveTime
				<< "成交价格:" << pCOSStatus->TradePrice
				<< "货币代码:" << pCOSStatus->CurrencyID << endl;
		}
	}

	///止损止盈单状态通知
	virtual void OnRtnPLStatus (CKSPLStatusField * pPLStatus)
	{
		printf ("profit and loss order status notice\n");
		if (pPLStatus != NULL)
		{
			cout << "OnRtnPLStatus:"
				<< "营业部代码:" << pPLStatus->BrokerID
				<< "操作员:" << pPLStatus->UserID
				<< "投资者代码:" << pPLStatus->InvestorID
				<< "序号:" << pPLStatus->SequenceNo
				<< "止损止盈单编号:" << pPLStatus->ProfitAndLossOrderID
				<< "止损条件单编号:" << pPLStatus->StopLossOrderID
				<< "止盈条件单编号:" << pPLStatus->TakeProfitOrderID
				<< "盈损单状态:" << pPLStatus->ProfitAndLossOrderStatus
				<< "止损价:" << pPLStatus->StopLossPrice
				<< "止盈价:" << pPLStatus->TakeProfitPrice
				<< "生成盈损价的方式:" << pPLStatus->OffsetValue
				<< "开仓成交价格:" << pPLStatus->OpenTradePrice
				<< "备注:" << pPLStatus->Memo
				<< "本地报单编号:" << pPLStatus->OrderLocalID
				<< "交易所代码:" << pPLStatus->ExchangeID
				<< "合约代码:" << pPLStatus->InstrumentID
				<< "报单状态:" << pPLStatus->OrderStatus
				<< "开平标志:" << pPLStatus->CombOffsetFlag
				<< "投机套保标志:" << pPLStatus->CombHedgeFlag
				<< "买卖方向:" << pPLStatus->Direction
				<< "价格:" << pPLStatus->LimitPrice
				<< "数量:" << pPLStatus->VolumeTotalOriginal
				<< "交易日:" << pPLStatus->TradingDay
				<< "撤销用户:" << pPLStatus->CancelUserID
				<< "撤销时间:" << pPLStatus->CancelTime
				<< "客户代码:" << pPLStatus->ClientID
				<< "业务单元:" << pPLStatus->BusinessUnit
				<< "报单编号:" << pPLStatus->OrderSysID
				<< "今成交数量:" << pPLStatus->VolumeTraded
				<< "剩余数量:" << pPLStatus->VolumeTotal
				<< "委托时间:" << pPLStatus->InsertTime
				<< "激活时间:" << pPLStatus->ActiveTime
				<< "成交价格:" << pPLStatus->TradePrice
				<< "货币代码:" << pPLStatus->CurrencyID << endl;
		}
	}
};

void FormatConditionalInsertData (CKSConditionalOrderInitInsert * ConditionOrder)
{
	strcpy (ConditionOrder->BrokerID, "6A89B428");
	strcpy (ConditionOrder->ExchangeID, "CFFEX");
	strcpy (ConditionOrder->InvestorID, "80002");
	strcpy (ConditionOrder->InstrumentID, "IF1303");
	ConditionOrder->Direction = '0';
	ConditionOrder->CombOffsetFlag = '0';
	ConditionOrder->CombHedgeFlag = '1';
	ConditionOrder->VolumeTotalOriginal = 2;
	ConditionOrder->LimitPrice = 200;
	ConditionOrder->OrderPriceType = '1';
	ConditionOrder->ConditionalType = '1';	//条件类别
	ConditionOrder->ConditionalPrice = 300;
	ConditionOrder->TriggeredTimes = 2;
	ConditionOrder->OrderType = '1';
	strcpy (ConditionOrder->ActiveTime, "142010");
	strcpy (ConditionOrder->InActiveTime, "162010");
	strcpy (ConditionOrder->CurrencyID, "RMB");

	return;

}

void FormatConditionalQueryData (CKSConditionalOrderQuery * Querycondition)
{
	strcpy (Querycondition->BrokerID, "6A89B428");
	strcpy (Querycondition->InvestorID, "80002");
	///不存在的单号
	//Querycondition->ConditionalOrderID  = 90;

	//已知的单号 
	Querycondition->ConditionalOrderID  = 1;
	return;
}

void FormatConditionalPauseActiveData (CKSConditionalOrderStateAlter *Pausecondition)
{
	strcpy (Pausecondition->BrokerID, "6A89B428");
	strcpy (Pausecondition->InvestorID, "80002");
	Pausecondition->ConditionalOrderID = 18;
	Pausecondition->ConditionalOrderStateAlter = '0';

	return;
}

void FormatConditionalRemoveData (CKSConditionalOrderRemove * Removecondition)
{
	strcpy (Removecondition->BrokerID, "6A89B428");
	strcpy (Removecondition->InvestorID, "80002");
	Removecondition->ConditionalOrderID = 5;

	return;
}

void FormatConditionalModifyData (CKSConditionalOrderModify * Modifycondition)
{
	strcpy(Modifycondition->BrokerID,"6A89B428");
	strcpy(Modifycondition->InvestorID,"80002");
	Modifycondition->ConditionalOrderID = 16;
	Modifycondition->LimitPrice = 1000;
	Modifycondition->VolumeTotalOriginal = 9;
	strcpy (Modifycondition->ExchangeID, "CFFEX");
	strcpy (Modifycondition->InstrumentID, "IF1303");
	Modifycondition->Direction = '1';
	Modifycondition->CombOffsetFlag = '3';
	Modifycondition->CombHedgeFlag = '1';

	Modifycondition->OrderPriceType = '1';
	Modifycondition->ConditionalType = '1';	//条件类别
	Modifycondition->ConditionalPrice = 3000;
	Modifycondition->TriggeredTimes = 10;
	Modifycondition->OrderType = '1';
	return;
}

void FormatConditionalSelectData (CKSConditionalOrderSelect * Selectcondition)
{
	strcpy (Selectcondition->BrokerID, "6A89B428");
	strcpy (Selectcondition->InvestorID, "80002");
	Selectcondition->ConditionalOrderID = 2;
	Selectcondition->SelectResult = KSCOS_Select_AGAIN;
}

void FormatProfitAndLossInsertData (CKSProfitAndLossOrderInsert *ProfitAndLossOrder)
{
	strcpy (ProfitAndLossOrder->BrokerID, "6A89B428");
	strcpy (ProfitAndLossOrder->InvestorID, "80002");
	//strcpy (ProfitAndLossOrder->OrderLocalID, "8");
	//strcpy (ProfitAndLossOrder->OrderLocalID, "1");
	strcpy (ProfitAndLossOrder->OrderLocalID, "6");
	ProfitAndLossOrder->StopLossPrice = 2100;
	ProfitAndLossOrder->TakeProfitPrice = 2500;
	strcpy (ProfitAndLossOrder->ExchangeID, "CFFEX");
	strcpy (ProfitAndLossOrder->BusinessUnit, "cffex");
	ProfitAndLossOrder->TriggeredTimes = 2;
	ProfitAndLossOrder->CloseMode = '1';
	ProfitAndLossOrder->FiguresPrice = 0.2;
	ProfitAndLossOrder->OffsetValue = '0';
	ProfitAndLossOrder->SpringType = '1';
	strcpy (ProfitAndLossOrder->CurrencyID, "RMB");

	return;
}

void FormatProfitAndLossQueryData (CKSProfitAndLossOrderQuery *ProfitAndLossOrderQuery)
{
	///营业部代码
	strcpy (ProfitAndLossOrderQuery->BrokerID, "6A89B428");
	///投资者代码
	strcpy (ProfitAndLossOrderQuery->InvestorID, "80002");
	///止损止盈单号
	ProfitAndLossOrderQuery->ProfitAndLossOrderID = 12;
	///本地报单编号
	strcpy (ProfitAndLossOrderQuery->OrderLocalID, "8");
	///交易所代码
	strcpy (ProfitAndLossOrderQuery->ExchangeID, "CFFEX");
	///业务单元
	strcpy (ProfitAndLossOrderQuery->BusinessUnit, "cffex");
	return;
}

void FormatProfitAndLossRemoveData (CKSProfitAndLossOrderRemove *ProfitAndLossOrderRemove)
{
	///营业部代码
	strcpy (ProfitAndLossOrderRemove->BrokerID, "6A89B428");
	///投资者代码
	strcpy (ProfitAndLossOrderRemove->InvestorID, "80002");
	///止损止盈单号
	ProfitAndLossOrderRemove->ProfitAndLossOrderID = 2;
	///本地报单编号
	strcpy (ProfitAndLossOrderRemove->OrderLocalID, "8");
	///交易所代码
	strcpy (ProfitAndLossOrderRemove->ExchangeID, "CFFEX");
	///业务单元
	strcpy (ProfitAndLossOrderRemove->BusinessUnit, "cffex");

	return;
}

void FormatProfitAndLossModifyData (CKSProfitAndLossOrderModify *ProfitAndLossOrderModify)
{
	///营业部代码
	strcpy (ProfitAndLossOrderModify->BrokerID, "6A89B428");
	///投资者代码
	strcpy (ProfitAndLossOrderModify->InvestorID, "80002");
	///止损止盈单号
	ProfitAndLossOrderModify->ProfitAndLossOrderID =9 ;
	ProfitAndLossOrderModify->StopLossPrice = 2999;
	ProfitAndLossOrderModify->TakeProfitPrice = 3999;
	ProfitAndLossOrderModify->CloseMode = '1';
	ProfitAndLossOrderModify->FiguresPrice = 0.2;
	ProfitAndLossOrderModify->OffsetValue = '0';
	ProfitAndLossOrderModify->SpringType = '1';
	ProfitAndLossOrderModify->TriggeredTimes= 30;

	return;
}

//实例数量
const int MAX_CONNECTION = 1;

int main(int argc, char* argv[])
{
	CThostFtdcTraderApi* pUserApi[MAX_CONNECTION] = {0};
	CTraderApiSample* pSpi[MAX_CONNECTION] = {0}; 
	CKSCosApi *pCosAPI = NULL;

	for (int i=0; i < MAX_CONNECTION; i++ )
	{
		// 创建实例
		// 运行此程序前请将环境服务商提供的授权文件放在程序运行目录
		pUserApi[i] = CThostFtdcTraderApi::CreateFtdcTraderApi();

		// 创建spi实例
		// 运行此程序前请修改BrokerID 用户名及密码
		pSpi[i] = new CTraderApiSample(pUserApi[i], "31000853", "80002", "123456", "IF1506");

		CCosHandler CosSpiTest;			//产生一个条件单响应的实例

		// 注册spi实例
		pUserApi[i]->RegisterSpi(pSpi[i]);

		//订阅私有流
		pUserApi[i]->SubscribePrivateTopic(THOST_TERT_RESUME);

		//订阅公有流
		pUserApi[i]->SubscribePublicTopic(THOST_TERT_QUICK);

		// 注册前置机	
		// 运行此程序前请修改 IP端口
		pUserApi[i]->RegisterFront("tcp://10.253.117.107:13163");
		//pUserApi[i]->RegisterNameServer("tcp://10.253.117.110:11000");
		//CThostFtdcFensUserInfoField FensUserInfo = {0};
		//strncpy(FensUserInfo.UserID, "201301", sizeof(FensUserInfo.UserID)-1);
		//FensUserInfo.LoginMode = 'E';
		//pUserApi[i]->RegisterFensUserInfo(&FensUserInfo);
		// 初始化
		pUserApi[i]->Init();
		getchar();

		pCosAPI = (CKSCosApi *) pUserApi[i]->LoadExtApi(&CosSpiTest,KS_COS_API);	//注册条件单实例
		if( NULL == pCosAPI )
		{
			printf("register conditional order instance failed\n");
		}
		//条件单下单
		CKSConditionalOrderInitInsert ConditionOrder;
		memset (&ConditionOrder, 0, sizeof (ConditionOrder));
		FormatConditionalInsertData (&ConditionOrder);

		//查询条件单实例 
		CKSConditionalOrderQuery Querycondition;
		memset (&Querycondition, 0, sizeof (CKSConditionalOrderQuery));
		FormatConditionalQueryData (&Querycondition);

		pCosAPI->ReqInitInsertConditionalOrder(&ConditionOrder, pSpi[i]->m_nRequestID++);
		pCosAPI->ReqQueryConditionalOrder (&Querycondition, pSpi[i]->m_nRequestID++);
	}

	printf ("\npress return to quit...\n");

	getchar();
	for (int i=0; i < MAX_CONNECTION; i++ )
	{
		// release the API instance
		pUserApi[i]->Release();

		// delete pSpi
		delete pSpi[i];
	}

	return 0;
}


