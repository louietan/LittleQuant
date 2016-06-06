
#include "StdAfx.h"

#include "MdSpi.h"
#include "CTPMDAdapter.h"
#include <memory.h>



CTPMDAdapter::CTPMDAdapter(void)
{
	m_pApi = CThostFtdcMdApi::CreateFtdcMdApi();
	m_pSpi = new CMdSpi(this);
	m_pApi->RegisterSpi(m_pSpi);
}

CTPMDAdapter::CTPMDAdapter(String^ pszFlowPath, bool bIsUsingUdp)
{
	CAutoStrPtr asp(pszFlowPath);
	m_pApi = CThostFtdcMdApi::CreateFtdcMdApi(asp.m_pChar, bIsUsingUdp);
	m_pSpi = new CMdSpi(this);
	m_pApi->RegisterSpi(m_pSpi);
}

CTPMDAdapter::~CTPMDAdapter(void)
{
	Release();
}

void CTPMDAdapter::Release(void)
{
	if(m_pApi)
	{
		m_pApi->RegisterSpi(0);
		m_pApi->Release();
		m_pApi = nullptr;
		delete m_pSpi;
	}
}

void CTPMDAdapter::RegisterFront(String^  pszFrontAddress)
{
	CAutoStrPtr asp = CAutoStrPtr(pszFrontAddress);
	m_pApi->RegisterFront(asp.m_pChar);
}

void CTPMDAdapter::Init(void)
{
	m_pApi->Init();
}

void CTPMDAdapter::Join(void)
{
	m_pApi->Join();
}

String^ CTPMDAdapter::GetTradingDay()
{
	return gcnew String(m_pApi->GetTradingDay());
}

int CTPMDAdapter::ReqUserLogin(ThostFtdcReqUserLoginField^ pReqUserLoginField, int nRequestID)
{
	CThostFtdcReqUserLoginField native;
	MNConv<ThostFtdcReqUserLoginField^, CThostFtdcReqUserLoginField>::M2N(pReqUserLoginField, &native);
	return m_pApi->ReqUserLogin(&native, nRequestID);
}

int CTPMDAdapter::ReqUserLogout(ThostFtdcUserLogoutField^ pUserLogout, int nRequestID)
{
	CThostFtdcUserLogoutField native;
	MNConv<ThostFtdcUserLogoutField^, CThostFtdcUserLogoutField>::M2N(pUserLogout, &native);
	return m_pApi->ReqUserLogout(&native, nRequestID);
}

int CTPMDAdapter::SubscribeMarketData(array<String^>^ ppInstrumentID)
{
	if(ppInstrumentID == nullptr || ppInstrumentID->Length <= 0)
		return -1;

	int count = ppInstrumentID->Length;
	char** pp = new char*[count];
	CAutoStrPtr** asp = new CAutoStrPtr*[count];
	for(int i=0; i<count; i++)
	{
		CAutoStrPtr* ptr = new CAutoStrPtr(ppInstrumentID[i]);
		asp[i] = ptr;
		pp[i] = ptr->m_pChar;
	}

	int result = m_pApi->SubscribeMarketData(pp, count);
	
	// 释放所有分配的字符串内存
	for(int i=0; i<count; i++)
		delete asp[i];
	delete asp;
	delete pp;

	return result;
}

int CTPMDAdapter::UnSubscribeMarketData(array<String^>^ ppInstrumentID)
{
	if(ppInstrumentID == nullptr || ppInstrumentID->Length <= 0)
		return -1;

	int count = ppInstrumentID->Length;
	char** pp = new char*[count];
	CAutoStrPtr** asp = new CAutoStrPtr*[count];
	for(int i=0; i<count; i++)
	{
		CAutoStrPtr* ptr = new CAutoStrPtr(ppInstrumentID[i]);
		asp[i] = ptr;
		pp[i] = ptr->m_pChar;
	}

	int result = m_pApi->UnSubscribeMarketData(pp, count);
	
	// 释放所分配的字符串内存
	for(int i=0; i<count; i++)
		delete asp[i];
	delete asp;
	delete pp;

	return result;
}
