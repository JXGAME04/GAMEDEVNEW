// ChatConnect.h: interface for the CChatConnect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHATCONNECT_H__5ED21755_43E7_4141_A572_2506BF7C31BA__INCLUDED_)
#define AFX_CHATCONNECT_H__5ED21755_43E7_4141_A572_2506BF7C31BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetConnect.h"

class CChatConnect : public CNetConnect  
{
public:
	CChatConnect(class CChatServer* pHostServer, unsigned long id);
	virtual ~CChatConnect();

protected:
	virtual void OnClientConnectCreate();
	virtual void OnClientConnectClose();

	virtual void RecvPackage(const void* pData, size_t size);

private:
	void Proc0_SomeoneChat(const void* pData, size_t size);
	void Proc0_ChannelChat(const void* pData, size_t size);
	void Proc0_GroupMan(const void* pData, size_t size);
	void Proc0_ServerCommunity(const void* pData, size_t size);

	void Proc0_Normal(const void* pData, size_t size);
	void Proc1_Normal_EnterGame(const void* pData, size_t size);
	void Proc1_Normal_LeaveGame(const void* pData, size_t size);

	unsigned int nServerIndex;
	unsigned int nGsNumber;

	struct ACCINFO
	{
		std::_tstring role;
		DWORD nameID;
		unsigned long param;
	};
	typedef std::map<std::_tstring, ACCINFO, _tstring_iless>	ACCMAP;
	ACCMAP m_mapAcc;

	struct ROLEINFO
	{
		std::_tstring acc;
		DWORD nameID;
		unsigned long param;
	};
	typedef std::map<std::_tstring, ROLEINFO, _tstring_less>	ROLEMAP;
	ROLEMAP m_mapRole;

	struct PARAMINFO
	{
		std::_tstring acc;
		std::_tstring role;
		std::_tstring hwid;
		DWORD nameID;
	};
	typedef std::map<unsigned long, PARAMINFO>	PARAMMAP;
	PARAMMAP m_mapParam;

public:
	BOOL FindPlayerByParam(unsigned long param, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID);
	BOOL FindPlayerByAcc(const std::_tstring& acc, std::_tstring* pRole, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealAcc = NULL);
	BOOL FindPlayerByRole(const std::_tstring& role, std::_tstring* pAcc, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealRole = NULL);
};

#endif // !defined(AFX_CHATCONNECT_H__5ED21755_43E7_4141_A572_2506BF7C31BA__INCLUDED_)
