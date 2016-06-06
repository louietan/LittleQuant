/////////////////////////////////////////////////////////////////////////
/// 金仕达 KSFT C++ => .Net Framework Adapter
/// Summary: 条件单应答接口托管转非托管
/// Author:	ksftapi@gmail.com
/// Date: 20130314
/// Update List:
///   20130318 修改
///   20130326 添加注释
/////////////////////////////////////////////////////////////////////////

#pragma once

#include "KSFTCosAdapter.h"
#include <vcclr.h>
#include "util.h"

using namespace KingstarAPI;

namespace Native
{
	///<summary>
	/// 非托管类
	///</summary>
	class CCosSpi : public CKSCosSpi
	{
	public:
		CCosSpi(KSFTCosAdapter^ pAdapter) {m_pAdapter = pAdapter;};

		///<summary>
		///条件单录入响应
		///</summary>
		virtual void OnRspInitInsertConditionalOrder(CKSConditionalOrderOperResultField *pInitInsertConditionalOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
			m_pAdapter->OnRspInitInsertConditionalOrder(MNConv<TKSConditionalOrderOperResultField^, CKSConditionalOrderOperResultField>::N2M(pInitInsertConditionalOrder), RspInfoField(pRspInfo), nRequestID, bIsLast);
		};
		
		///<summary>
		///条件单查询响应
		///</summary>
		virtual void OnRspQueryConditionalOrder(CKSConditionalOrderOperResultField *pQueryConditionalOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
			m_pAdapter->OnRspQueryConditionalOrder(MNConv<TKSConditionalOrderOperResultField^, CKSConditionalOrderOperResultField>::N2M(pQueryConditionalOrder), RspInfoField(pRspInfo), nRequestID, bIsLast);
		};
		
		///<summary>
		///条件单修改响应
		///</summary>
		virtual void OnRspModifyConditionalOrder(CKSConditionalOrderOperResultField *pModifyConditionalOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
			m_pAdapter->OnRspModifyConditionalOrder(MNConv<TKSConditionalOrderOperResultField^, CKSConditionalOrderOperResultField>::N2M(pModifyConditionalOrder), RspInfoField(pRspInfo), nRequestID, bIsLast);
		};
		
		///<summary>
		///条件单暂停激活响应
		///</summary>
		virtual void OnRspPauseConditionalOrder(CKSConditionalOrderOperResultField *pPauseConditionalOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
			TKSConditionalOrderOperResultField^ field = MNConv<TKSConditionalOrderOperResultField^, CKSConditionalOrderOperResultField>::N2M(pPauseConditionalOrder);
			m_pAdapter->OnRspPauseConditionalOrder(field, RspInfoField(pRspInfo), nRequestID, bIsLast);
		};

		///<summary>
		///条件单删除响应
		///</summary>
		virtual void OnRspRemoveConditionalOrder(CKSConditionalOrderRspResultField *pRemoveConditionalOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
			m_pAdapter->OnRspRemoveConditionalOrder(MNConv<TKSConditionalOrderRspResultField^, CKSConditionalOrderRspResultField>::N2M(pRemoveConditionalOrder), RspInfoField(pRspInfo), nRequestID, bIsLast);
		};

		///<summary>
		///条件单选择响应
		///</summary>
		virtual void OnRspSelectConditionalOrder(CKSConditionalOrderRspResultField *pSelectConditionalOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
			m_pAdapter->OnRspSelectConditionalOrder(MNConv<TKSConditionalOrderRspResultField^, CKSConditionalOrderRspResultField>::N2M(pSelectConditionalOrder), RspInfoField(pRspInfo), nRequestID, bIsLast);
		};

		///<summary>
		///止损止盈单录入响应
		///</summary>
		virtual void OnRspInsertProfitAndLossOrder(CKSProfitAndLossOrderOperResultField *pInsertProfitAndLossOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
			m_pAdapter->OnRspInsertProfitAndLossOrder(MNConv<TKSProfitAndLossOrderOperResultField^, CKSProfitAndLossOrderOperResultField>::N2M(pInsertProfitAndLossOrder), RspInfoField(pRspInfo), nRequestID, bIsLast);
		};

		///<summary>
		///止损止盈单修改响应
		///</summary>
		virtual void OnRspModifyProfitAndLossOrder(CKSProfitAndLossOrderOperResultField *pModifyProfitAndLossOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
			m_pAdapter->OnRspModifyProfitAndLossOrder(MNConv<TKSProfitAndLossOrderOperResultField^, CKSProfitAndLossOrderOperResultField>::N2M(pModifyProfitAndLossOrder), RspInfoField(pRspInfo), nRequestID, bIsLast);
		};

		///<summary>
		///止损止盈单删除响应
		///</summary>
		virtual void OnRspRemoveProfitAndLossOrder(CKSProfitAndLossOrderOperResultField *pRemoveProfitAndLossOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
			TKSProfitAndLossOrderRemoveField^ field = MNConv<TKSProfitAndLossOrderRemoveField^, CKSProfitAndLossOrderOperResultField>::N2M(pRemoveProfitAndLossOrder);
			m_pAdapter->OnRspRemoveProfitAndLossOrder(field, RspInfoField(pRspInfo), nRequestID, bIsLast);
		};

		///<summary>
		///止损止盈单查询响应
		///</summary>
		virtual void OnRspQueryProfitAndLossOrder(CKSProfitAndLossOrderOperResultField *pQueryProfitAndLossOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
			TKSProfitAndLossOrderOperResultField^ field = MNConv<TKSProfitAndLossOrderOperResultField^, CKSProfitAndLossOrderOperResultField>::N2M(pQueryProfitAndLossOrder);
			m_pAdapter->OnRspQueryProfitAndLossOrder(field, RspInfoField(pRspInfo), nRequestID, bIsLast);
		};

		///<summary>
		///条件单请求选择通知
		///</summary>
		virtual void OnRtnCOSAskSelect(CKSCOSAskSelectField *pCOSAskSelect){
			TKSCOSAskSelectField^ field = safe_cast<TKSCOSAskSelectField^>(Marshal::PtrToStructure(IntPtr(pCOSAskSelect), TKSCOSAskSelectField::typeid));
			m_pAdapter->OnRtnCOSAskSelect(field);
		};

		///<summary>
		///条件单状态通知
		///</summary>
		virtual void OnRtnCOSStatus(CKSCOSStatusField *pCOSStatus){
			TKSCOSStatusField^ field = safe_cast<TKSCOSStatusField^>(Marshal::PtrToStructure(IntPtr(pCOSStatus), TKSCOSStatusField::typeid));
			m_pAdapter->OnRtnCOSStatus(field);
		};

		///<summary>
		///止损止盈单状态通知
		///</summary>
		virtual void OnRtnPLStatus(CKSPLStatusField *pPLStatus){
			TKSPLStatusField^ field = safe_cast<TKSPLStatusField^>(Marshal::PtrToStructure(IntPtr(pPLStatus), TKSPLStatusField::typeid));
			m_pAdapter->OnRtnPLStatus(field);
		};
	private:
		gcroot<KSFTCosAdapter^> m_pAdapter;
	};
};
