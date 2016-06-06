/////////////////////////////////////////////////////////////////////////
/// 上期技术 CTP C++ => .Net Framework Adapter
/// Author:	shawn666.liu@gmail.com
/// Date: 20120420
/////////////////////////////////////////////////////////////////////////
#pragma once

namespace CTP
{
	// common deleagats
	public delegate void FrontConnected();
	public delegate void FrontDisconnected(int nReason);
	public delegate void HeartBeatWarning(int nTimeLapse);
	public delegate void RspUserLogin(ThostFtdcRspUserLoginField^ pRspUserLogin, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspUserLogout(ThostFtdcUserLogoutField^ pUserLogout, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspError(ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);

	// marketdata 
	public delegate void RspSubMarketData(ThostFtdcSpecificInstrumentField^ pSpecificInstrument, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspUnSubMarketData(ThostFtdcSpecificInstrumentField^ pSpecificInstrument, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RtnDepthMarketData(ThostFtdcDepthMarketDataField^ pDepthMarketData);

	// trader
	public delegate void RspAuthenticate(ThostFtdcRspAuthenticateField^ pRspAuthenticateField, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspUserPasswordUpdate(ThostFtdcUserPasswordUpdateField^ pUserPasswordUpdate, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspTradingAccountPasswordUpdate(ThostFtdcTradingAccountPasswordUpdateField^ pTradingAccountPasswordUpdate, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspOrderInsert(ThostFtdcInputOrderField^ pInputOrder, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspParkedOrderInsert(ThostFtdcParkedOrderField^ pParkedOrder, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspParkedOrderAction(ThostFtdcParkedOrderActionField^ pParkedOrderAction, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspOrderAction(ThostFtdcInputOrderActionField^ pInputOrderAction, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQueryMaxOrderVolume(ThostFtdcQueryMaxOrderVolumeField^ pQueryMaxOrderVolume, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspSettlementInfoConfirm(ThostFtdcSettlementInfoConfirmField^ pSettlementInfoConfirm, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspRemoveParkedOrder(ThostFtdcRemoveParkedOrderField^ pRemoveParkedOrder, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspRemoveParkedOrderAction(ThostFtdcRemoveParkedOrderActionField^ pRemoveParkedOrderAction, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryOrder(ThostFtdcOrderField^ pOrder, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryTrade(ThostFtdcTradeField^ pTrade, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryInvestorPosition(ThostFtdcInvestorPositionField^ pInvestorPosition, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryTradingAccount(ThostFtdcTradingAccountField^ pTradingAccount, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryInvestor(ThostFtdcInvestorField^ pInvestor, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryTradingCode(ThostFtdcTradingCodeField^ pTradingCode, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryInstrumentMarginRate(ThostFtdcInstrumentMarginRateField^ pInstrumentMarginRate, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryInstrumentCommissionRate(ThostFtdcInstrumentCommissionRateField^ pInstrumentCommissionRate, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryExchange(ThostFtdcExchangeField^ pExchange, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryInstrument(ThostFtdcInstrumentField^ pInstrument, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryDepthMarketData(ThostFtdcDepthMarketDataField^ pDepthMarketData, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQrySettlementInfo(ThostFtdcSettlementInfoField^ pSettlementInfo, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryTransferBank(ThostFtdcTransferBankField^ pTransferBank, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryInvestorPositionDetail(ThostFtdcInvestorPositionDetailField^ pInvestorPositionDetail, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryNotice(ThostFtdcNoticeField^ pNotice, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQrySettlementInfoConfirm(ThostFtdcSettlementInfoConfirmField^ pSettlementInfoConfirm, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryInvestorPositionCombineDetail(ThostFtdcInvestorPositionCombineDetailField^ pInvestorPositionCombineDetail, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryCFMMCTradingAccountKey(ThostFtdcCFMMCTradingAccountKeyField^ pCFMMCTradingAccountKey, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryEWarrantOffset(ThostFtdcEWarrantOffsetField^ pEWarrantOffset, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryTransferSerial(ThostFtdcTransferSerialField^ pTransferSerial, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryAccountregister(ThostFtdcAccountregisterField^ pAccountregister, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RtnOrder(ThostFtdcOrderField^ pOrder);
	public delegate void RtnTrade(ThostFtdcTradeField^ pTrade);
	public delegate void ErrRtnOrderInsert(ThostFtdcInputOrderField^ pInputOrder, ThostFtdcRspInfoField^ pRspInfo);
	public delegate void ErrRtnOrderAction(ThostFtdcOrderActionField^ pOrderAction, ThostFtdcRspInfoField^ pRspInfo);
	public delegate void RtnInstrumentStatus(ThostFtdcInstrumentStatusField^ pInstrumentStatus);
	public delegate void RtnTradingNotice(ThostFtdcTradingNoticeInfoField^ pTradingNoticeInfo);
	public delegate void RtnErrorConditionalOrder(ThostFtdcErrorConditionalOrderField^ pErrorConditionalOrder);
	public delegate void RspQryContractBank(ThostFtdcContractBankField^ pContractBank, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryParkedOrder(ThostFtdcParkedOrderField^ pParkedOrder, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryParkedOrderAction(ThostFtdcParkedOrderActionField^ pParkedOrderAction, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryTradingNotice(ThostFtdcTradingNoticeField^ pTradingNotice, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryBrokerTradingParams(ThostFtdcBrokerTradingParamsField^ pBrokerTradingParams, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQryBrokerTradingAlgos(ThostFtdcBrokerTradingAlgosField^ pBrokerTradingAlgos, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RtnFromBankToFutureByBank(ThostFtdcRspTransferField^ pRspTransfer);
	public delegate void RtnFromFutureToBankByBank(ThostFtdcRspTransferField^ pRspTransfer);
	public delegate void RtnRepealFromBankToFutureByBank(ThostFtdcRspRepealField^ pRspRepeal);
	public delegate void RtnRepealFromFutureToBankByBank(ThostFtdcRspRepealField^ pRspRepeal);
	public delegate void RtnFromBankToFutureByFuture(ThostFtdcRspTransferField^ pRspTransfer);
	public delegate void RtnFromFutureToBankByFuture(ThostFtdcRspTransferField^ pRspTransfer);
	public delegate void RtnRepealFromBankToFutureByFutureManual(ThostFtdcRspRepealField^ pRspRepeal);
	public delegate void RtnRepealFromFutureToBankByFutureManual(ThostFtdcRspRepealField^ pRspRepeal);
	public delegate void RtnQueryBankBalanceByFuture(ThostFtdcNotifyQueryAccountField^ pNotifyQueryAccount);
	public delegate void ErrRtnBankToFutureByFuture(ThostFtdcReqTransferField^ pReqTransfer, ThostFtdcRspInfoField^ pRspInfo);
	public delegate void ErrRtnFutureToBankByFuture(ThostFtdcReqTransferField^ pReqTransfer, ThostFtdcRspInfoField^ pRspInfo);
	public delegate void ErrRtnRepealBankToFutureByFutureManual(ThostFtdcReqRepealField^ pReqRepeal, ThostFtdcRspInfoField^ pRspInfo);
	public delegate void ErrRtnRepealFutureToBankByFutureManual(ThostFtdcReqRepealField^ pReqRepeal, ThostFtdcRspInfoField^ pRspInfo);
	public delegate void ErrRtnQueryBankBalanceByFuture(ThostFtdcReqQueryAccountField^ pReqQueryAccount, ThostFtdcRspInfoField^ pRspInfo);
	public delegate void RtnRepealFromBankToFutureByFuture(ThostFtdcRspRepealField^ pRspRepeal);
	public delegate void RtnRepealFromFutureToBankByFuture(ThostFtdcRspRepealField^ pRspRepeal);
	public delegate void RspFromBankToFutureByFuture(ThostFtdcReqTransferField^ pReqTransfer, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspFromFutureToBankByFuture(ThostFtdcReqTransferField^ pReqTransfer, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RspQueryBankAccountMoneyByFuture(ThostFtdcReqQueryAccountField^ pReqQueryAccount, ThostFtdcRspInfoField^ pRspInfo, int nRequestID, bool bIsLast);
	public delegate void RtnOpenAccountByBank(ThostFtdcOpenAccountField^ pOpenAccount);
	public delegate void RtnCancelAccountByBank(ThostFtdcCancelAccountField^ pCancelAccount);
	public delegate void RtnChangeAccountByBank(ThostFtdcChangeAccountField^ pChangeAccount);
};