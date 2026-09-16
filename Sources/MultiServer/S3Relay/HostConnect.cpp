// HostConnect.cpp: implementation of the CHostConnect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "HostConnect.h"
#include "RelayRpc.h"	// [RELAYHT 06/09]
#include "HostServer.h"
#include "DealRelay.h"
#include "S3Relay.h"
#include "../../sword3paysys/s3accserver/AccountLoginDef.h"

CHostConnect::stdHostServer	CHostConnect::m_sHostServer;

// [MAYID 16/09] Bo dem thu tu ghi so cho MOI GameServer (toan relay). Dung de tra loi cau hoi 'bao nhieu phien
// cung ma may da vao TRUOC toi' => hai nguoi vao cung luc thi nguoi sau moi bi tinh la vuot nguong.
static DWORD gs_dwMayIdSeq = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHostConnect::CHostConnect(CHostServer* pHostServer, unsigned long id)
	: CNetConnect(pHostServer, id)
{
	nGsNetIdx = id;
	nServerIndex = 1;
}

CHostConnect::~CHostConnect()
{
	m_serMaps.clear();
	nServerIndex = 1;
}


void CHostConnect::RecvPackage(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolFamily == pf_normal)
	{
		Proc0_Normal(pData, size);
	}
	else if (pHeader->ProtocolFamily == pf_relay)
	{
		Proc0_Relay(pData, size);
	}
	else if (pHeader->ProtocolFamily == pf_playercommunity)
	{
		Proc0_PlayerCommunity(pData, size);
	}
	else if (pHeader->ProtocolFamily == pf_udataserveridx)
	{
		Proc0_ServerCommunity(pData, size);
	}
	else if (pHeader->ProtocolFamily == pf_udatasservermaps)
	{
		Proc0_ServerMaps(pData, size);
	}
}

void CHostConnect::Proc0_Normal(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolID == c2s_entergame)
	{
		Proc1_Normal_EnterGame(pData, size);
	}
	else if (pHeader->ProtocolID == c2s_leavegame)
	{
		Proc1_Normal_LeaveGame(pData, size);
	}
}

void CHostConnect::Proc1_Normal_EnterGame(const void* pData, size_t size)
{
	tagEnterGame2* pEnterGame2 = (tagEnterGame2*)pData;

	std::_tstring strAcc = (char*)pEnterGame2->szAccountName;
	assert(!strAcc.empty());
	std::_tstring strRole = (char*)pEnterGame2->szCharacterName;
	assert(!strRole.empty());

	std::_tstring strHWID= (char*)pEnterGame2->szHWID;
	assert(!strHWID.empty());

	ACCINFO infoAcc;
	infoAcc.role = strRole;
	infoAcc.nameID = pEnterGame2->dwNameID;
	infoAcc.param = pEnterGame2->lnID;

	ROLEINFO infoRole;
	infoRole.acc = strAcc;
	infoRole.nameID = pEnterGame2->dwNameID;
	infoRole.param = pEnterGame2->lnID;

	PARAMINFO infoParam;
	infoParam.acc = strAcc;
	infoParam.role = strRole;
	infoParam.hwid = strHWID;
	infoParam.nameID = pEnterGame2->dwNameID;
	infoParam.seq = ++gs_dwMayIdSeq;	// [MAYID 16/09]

	{{
	DUMMY_AUTOLOCKWRITE(m_lockPlayer);

	/*
	 * [MAYID 15/09] DON RAC TRUOC KHI GHI - chong KET BO DEM VINH VIEN.
	 *
	 * Ba bang phai luon nhat quan: m_mapAcc[acc].param tro toi m_mapParam[param], va muc do phai quay lai
	 * dung acc ay. Truoc day EnterGame CHI GHI DE m_mapAcc[acc]. Neu CUNG MOT TAI KHOAN vao lai ma chua kip
	 * co LeaveGame cu (hay gap nhat: rot mang dot ngot roi vao lai TRUOC khi het ping timeout 60 giay) thi:
	 *     lan 1:  m_mapAcc[X] = {param:100}    m_mapParam[100] = {hwid}
	 *     lan 2:  m_mapAcc[X] = {param:200}    <-- de MAT so 100
	 *     LeaveGame sau do chi xoa duoc m_mapParam[200]  =>  m_mapParam[100] MO COI VINH VIEN.
	 * Moi muc mo coi lam bo dem cua DUNG ma may do tang them mot va KHONG BAO GIO giam. Lap lai vai lan la
	 * may do khong con dang nhap duoc nua, khong ai hieu vi sao. Day dung la 'player out ra lam ket gioi han'.
	 *
	 * Chieu doi xung: khe ket noi (lnID) duoc TAI SU DUNG cho nguoi khac. Neu m_mapParam[lnID] con muc cu cua
	 * nguoi truoc thi phai don ca m_mapAcc/m_mapRole cua nguoi do; neu khong, den luot ho LeaveGame se xoa
	 * NHAM muc cua nguoi moi (bo dem tut xuong, ho thoat gioi han ma khong biet).
	 *
	 * Don ca hai chieu o day thi so sach TU NHAT QUAN voi MOI thu tu su kien, khong phu thuoc kich ban nao.
	 */
	{
		ACCMAP::iterator itAccCu = m_mapAcc.find(strAcc);

		if (itAccCu != m_mapAcc.end())
		{
			rTRACE("[MAYID] don muc cu cung tai khoan: acc=%s param cu=%08X (chua co LeaveGame)",
				strAcc.c_str(), itAccCu->second.param);
			m_mapParam.erase(itAccCu->second.param);
			m_mapRole.erase(itAccCu->second.role);
		}

		PARAMMAP::iterator itKheCu = m_mapParam.find(pEnterGame2->lnID);

		if (itKheCu != m_mapParam.end())
		{
			rTRACE("[MAYID] don muc cu cung khe: khe=%08X acc cu=%s (khe duoc tai su dung)",
				pEnterGame2->lnID, itKheCu->second.acc.c_str());
			m_mapAcc.erase(itKheCu->second.acc);
			m_mapRole.erase(itKheCu->second.role);
		}
	}

	m_mapAcc[strAcc] = infoAcc;
	m_mapRole[strRole] = infoRole;
	m_mapParam[pEnterGame2->lnID] = infoParam;
	}}

	{{
	g_FriendMgr.SomeoneLogin(strRole);
	}}

	rTRACE("Host:player login: %s [%s] (%08X, %08X)", strAcc.c_str(), strRole.c_str(), GetIP(), pEnterGame2->lnID, pEnterGame2->nSelServer, nGsNetIdx);

	int rCount = CountLoginByHWID(strHWID);
	rTRACE("strHWID: [%s] count limit: [%d] seq=%u", strHWID.c_str(), rCount, infoParam.seq);
}

void CHostConnect::Proc1_Normal_LeaveGame(const void* pData, size_t size)
{
	tagLeaveGame2* pLeaveGame = (tagLeaveGame2*)pData;

	std::_tstring strAcc = pLeaveGame->szAccountName;
	assert(!strAcc.empty());

	{{
	DUMMY_AUTOLOCKWRITE(m_lockPlayer);

	ACCMAP::iterator itAcc = m_mapAcc.find(strAcc);
	if (itAcc != m_mapAcc.end())
	{
		ACCINFO& rAccInfo = (*itAcc).second;

		{{
		//ensure
		g_ChannelMgr.B_ClearPlayer(GetIP(), rAccInfo.param);
		g_FriendMgr.SomeoneLogout(rAccInfo.role);
		}}
		

		/*
		 * [MAYID 15/09] Chi xoa muc m_mapParam neu no VAN THUOC VE tai khoan nay. Khe ket noi (param = lnID)
		 * duoc tai su dung cho nguoi khac, nen xoa mu se lam NGUOI MOI khong con bi dem - tuc la ho thoat
		 * gioi han ma khong ai biet.
		 */
		{
			PARAMMAP::iterator itParam = m_mapParam.find(rAccInfo.param);

			if (itParam != m_mapParam.end())
			{
				if (itParam->second.acc == strAcc)
				{
					m_mapParam.erase(itParam);
				}
				else
				{
					rTRACE("[MAYID] KHONG xoa khe %08X: no da thuoc ve acc=%s chu khong phai %s",
						rAccInfo.param, itParam->second.acc.c_str(), strAcc.c_str());
				}
			}
		}

		m_mapRole.erase(rAccInfo.role);

		m_mapAcc.erase(itAcc);
	}
	}}

	rTRACE("player logout: %s", strAcc.c_str());
}


void CHostConnect::Proc0_Relay(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolID == relay_c2c_data)
	{
		Proc1_Relay_Data(pData, size);
	}
	else if (pHeader->ProtocolID == relay_c2c_askwaydata)
	{
		Proc1_Relay_AskWay(pData, size);
	}
	else if (pHeader->ProtocolID == relay_s2c_loseway)
	{
		Proc1_Relay_LoseWay(pData, size);
	}
}

void CHostConnect::Proc1_Relay_Data(const void* pData, size_t size)
{
 	RELAY_DATA* pRelayData = (RELAY_DATA*)pData;

	assert (pRelayData->nFromIP == 0);
	pRelayData->nFromIP = g_RootClient.GetSvrIP(); //gGetHostIP(global_adapt);
	pRelayData->nFromRelayID = GetID();

	if (pRelayData->nToIP == 0)
	{
		//arrived
		// [RELAYHT 06/09] GameServer goi kich ban tren relay: than goi bat dau
		// bang s2s_script. Day la LUONG MANG nen chi xep hang, RelayRpc_Tick()
		// (luong chinh) moi chay Lua.
		if (pRelayData->routeDateLength > 1 &&
			*((BYTE*)(pRelayData + 1)) == (BYTE)s2s_script)
		{
			RelayRpc_OnPacket((BYTE*)(pRelayData + 1) + 1,
				pRelayData->routeDateLength - 1, GetID());
		}
		return;
	}
	else if (pRelayData->nToIP == g_RootClient.GetSvrIP())  //from bishop
	{
		pRelayData->nToIP = 0;
		if (g_HostServer.IsConnectReady(pRelayData->nToRelayID))
		{
			CNetConnectDup conndup = g_HostServer.FindNetConnect(pRelayData->nToRelayID);
			if (conndup.IsValid())
			{
				conndup.SendPackage(pRelayData, size);
				return;
			}
		}
	}
	else if (pRelayData->nToIP == INADDR_BROADCAST)
	{
		if (pRelayData->nToRelayID == 0 || !g_RootClient.IsReady())
		{//local
			pRelayData->nToRelayID = 0;
			g_HostServer.BroadPackage(pRelayData, size);
		}
		else
		{//global
			g_RootClient.SendPackage(pRelayData, size);
		}
		return;
	}
	else
	{
		CNetSockDupEx sockdup = dealrelay::FindRelaySockByIP(pRelayData->nToIP);
		if (sockdup.IsValid())
		{
			pRelayData->nToIP = 0;
			sockdup.SendPackage(pRelayData, size);
			return;
		}

		if (g_RootClient.IsReady())
		{
			g_RootClient.SendPackage(pRelayData, size);
			return;
		}
	}


	//fail to relay, lose data
	size_t sizeLoseData = sizeof(RELAY_DATA) + size;
	RELAY_DATA* pLoseDataDup = (RELAY_DATA*)_alloca(sizeLoseData);

	gFillLosewayPckg(0, GetID(), 
		pRelayData, size, pLoseDataDup, sizeLoseData);

	SendPackage(pLoseDataDup, sizeLoseData);
}

void CHostConnect::Proc1_Relay_AskWay(const void* pData, size_t size)
{
	RELAY_ASKWAY_DATA* pAskWayData = (RELAY_ASKWAY_DATA*)pData;

	if (pAskWayData->seekMethod == rm_account_id)
	{
		Proc2_Relay_AskWay_AccountRoleID(pData, size, TRUE);
	}
	else if (pAskWayData->seekMethod == rm_role_id)
	{
		Proc2_Relay_AskWay_AccountRoleID(pData, size, FALSE);
	}
	else if (pAskWayData->seekMethod == rm_map_id)
	{
		Proc2_Relay_AskWay_MapID(pData, size);
	}
	else if (pAskWayData->seekMethod = rm_gm)
	{
		Proc2_Relay_AskWay_GM(pData, size);
	}

}

void CHostConnect::Proc1_Relay_LoseWay(const void* pData, size_t size)
{
 	RELAY_DATA* pLoseData = (RELAY_DATA*)pData;

	assert (pLoseData->nFromIP == 0);
	pLoseData->nFromIP = gGetHostIP(global_adapt);
	pLoseData->nFromRelayID = GetID();

	if (pLoseData->nToIP == 0)
	{
		//arrived
		return;
	}
	else if (pLoseData->nToIP == gGetHostIP(global_adapt))
	{
		pLoseData->nToIP = 0;
		if (g_HostServer.IsConnectReady(pLoseData->nToRelayID))
		{
			CNetConnectDup conndup = g_HostServer.FindNetConnect(pLoseData->nToRelayID);
			if (conndup.IsValid())
			{
				conndup.SendPackage(pLoseData, size);
				return;
			}
		}
	}
	else if (pLoseData->nToIP == INADDR_BROADCAST)
	{
		assert(FALSE);
		return;
	}
	else
	{
		CNetSockDupEx sockdup = dealrelay::FindRelaySockByIP(pLoseData->nToIP);
		if (sockdup.IsValid())
		{
			pLoseData->nToIP = 0;
			sockdup.SendPackage(pLoseData, size);
			return;
		}

		if (g_RootClient.IsReady())
		{
			g_RootClient.SendPackage(pLoseData, size);
			return;
		}
	}


	//fail to relay, lose data
	//don't gen more loseway
}

void CHostConnect::Proc2_Relay_AskWay_AccountRoleID(const void* pData, size_t size, BOOL acc)
{
	RELAY_ASKWAY_DATA* pAskWayData = (RELAY_ASKWAY_DATA*)pData;

	assert(pAskWayData->nFromIP == 0);
	pAskWayData->nFromIP = gGetHostIP(global_adapt);
	pAskWayData->nFromRelayID = GetID();
	pAskWayData->seekRelayCount ++;


	std::_tstring strName = (char*)(pAskWayData + 1);
	assert(!strName.empty());

	DWORD nameid = -1;
	unsigned long param = -1;
	CNetConnectDup conndup;


	BOOL find = FALSE;

	if (acc ? FindPlayerByAcc(strName, NULL, &nameid, &param)
			: FindPlayerByRole(strName, NULL, &nameid, &param))
	{ //the player is at this gamesvr
		conndup = *this;

		find = TRUE;
	}
	else if (acc ? g_HostServer.FindPlayerByAcc(this, strName, &conndup, NULL, &nameid, &param)
				 : g_HostServer.FindPlayerByRole(this, strName, &conndup, NULL, &nameid, &param))
	{//the player is at other gamesvr in this group
		find = TRUE;
	}

	if (find)
	{
		assert(conndup.IsValid());

		size_t pckgsize = sizeof(DWORD)*2 + size;
		RELAY_ASKWAY_DATA* pClntWayData = (RELAY_ASKWAY_DATA*)_alloca(pckgsize);

		gFillClntwayPckg(nameid, param, pAskWayData, size, pClntWayData, pckgsize);

		conndup.SendPackage(pClntWayData, pckgsize);

		return;
	}


	if (acc)
	{
		//the player is at other group, send to root
		if (g_RootClient.IsReady())
		{
			g_RootClient.SendPackage(pAskWayData, size);
			return;
		}
	}

	//lose way
	size_t sizeLoseData = sizeof(RELAY_DATA) + size;
	RELAY_DATA* pLoseDataDup = (RELAY_DATA*)_alloca(sizeLoseData);

	gFillLosewayPckg(0, GetID(), 
		pAskWayData, size, pLoseDataDup, sizeLoseData);

	SendPackage(pLoseDataDup, sizeLoseData);

}

void CHostConnect::Proc2_Relay_AskWay_MapID(const void* pData, size_t size)
{
	RELAY_ASKWAY_DATA* pAskWayData = (RELAY_ASKWAY_DATA*)pData;

	assert(pAskWayData->nFromIP == 0);
	pAskWayData->nFromIP = g_RootClient.GetSvrIP();//gGetHostIP(global_adapt);
	pAskWayData->nFromRelayID = GetID();
	pAskWayData->seekRelayCount ++;

	g_GatewayClient.PassAskWayMap(GetIP(), pAskWayData, nServerIndex);
}

void CHostConnect::Proc2_Relay_AskWay_GM(const void* pData, size_t size)
{
	RELAY_ASKWAY_DATA* pAskWayData = (RELAY_ASKWAY_DATA*)pData;

	assert (pAskWayData->nFromIP == 0);
	pAskWayData->nFromIP = gGetHostIP(global_adapt);
	pAskWayData->nFromRelayID = GetID();
	pAskWayData->seekRelayCount ++;

	if (g_RootClient.IsReady())
	{
		g_RootClient.SendPackage(pData, size);
		return;
	}

	//fail to relay, lose data

	size_t sizeLoseData = sizeof(RELAY_DATA) + size;
	RELAY_DATA* pLoseDataDup = (RELAY_DATA*)_alloca(sizeLoseData);

	gFillLosewayPckg(0, GetID(), 
		pData, size, pLoseDataDup, sizeLoseData);

	SendPackage(pLoseDataDup, sizeLoseData);
}

BOOL CHostConnect::ncheckMapIn(int nMapID)
{
	for(SERMAP::iterator it = m_serMaps.begin(); it!=m_serMaps.end(); ++it)
	{
		if(nMapID == (*it).second)
		{
			rTRACE("---ncheckMapIn gs nMapID:%d TRUE---", nMapID);
			return TRUE;
		}
	}
	rTRACE("---ncheckMapIn gs nMapID:%d FALSE---", nMapID);
	return FALSE;
}

void CHostConnect::Proc0_ServerMaps(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;
	if (pHeader->ProtocolID == relay_s2c_updatemaps)
	{
		tagUpMapGame2* pUMI = (tagUpMapGame2*)pData;
		int nMapCount = 0;
		m_serMaps.clear();

		g_SetRootPath(NULL);
		g_SetFilePath("\\");
		KIniFile _mapList;
		char szMapListPath[128];
		ZeroMemory(szMapListPath, sizeof(szMapListPath));
		sprintf(szMapListPath, "\\maps\\WorldSet_%s.ini", pUMI->szParamName);
		if(_mapList.Load(szMapListPath))
		{
			_mapList.GetInteger("Init", "Count", 0, &nMapCount);
			rTRACE("---Proc0_ServerMaps Start---");
			while(--nMapCount >= 0)
			{
				char szKeyName[32]={0};
				sprintf(szKeyName, "World%02d", nMapCount);
				int nMapIdx = 0;
				_mapList.GetInteger("World", szKeyName, 0, &nMapIdx);
				m_serMaps[nMapCount+1] = nMapIdx;
				rTRACE("---Proc0_ServerMaps nMapCount:%d nMapIdx:%d okay---", nMapCount, nMapIdx);
			}
			rTRACE("---Proc0_ServerMaps End---");
			_mapList.Clear();
		}
		else
		{
			rTRACE("---Proc0_ServerMaps not load: %s---", szMapListPath);
		}
	}
}

void CHostConnect::Proc0_ServerCommunity(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;
	if (pHeader->ProtocolID == relay_s2c_updateserver)
	{
		tagLeaveGame2* pServerData = (tagLeaveGame2*)pData;
		nServerIndex = (short)HIWORD(pServerData->nSelServer);
		nGsNumber = LOWORD(pServerData->nSelServer);
		m_sHostServer[nServerIndex] = GetID();
		setSerRegIndex(nServerIndex);
		setGsNumber(nGsNumber);
		rTRACE("---Host start Okay nServerIndex:%d, nServerIndex:%08X ---", nServerIndex, nGsNumber);
	}
}

void CHostConnect::Proc0_PlayerCommunity(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolID == playercomm_c2s_querychannelid)
	{
		Proc1_PlayerCommunity_QueryChannelID(pData, size);
	}
	else if (pHeader->ProtocolID == playercomm_c2s_freechannelid)
	{
		Proc1_PlayerCommunity_FreeChannelID(pData, size);
	}
	else if (pHeader->ProtocolID == playercomm_c2s_subscribe)
	{
		Proc1_PlayerCommunity_Subscribe(pData, size);
	}
}

void CHostConnect::Proc1_PlayerCommunity_QueryChannelID(const void* pData, size_t size)
{
	if (size < sizeof(PLAYERCOMM_QUERYCHANNELID) + sizeof(tagPlusSrcInfo))
		return;

	PLAYERCOMM_QUERYCHANNELID* pPlayerCommQID = (PLAYERCOMM_QUERYCHANNELID*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	//std::_tstring rolename;
	//if (!FindPlayerByParam(pSrcInfo->lnID, NULL, &rolename, NULL))
	//	return;

	CNetConnectDup chatconndup;
	if(!g_ChatServer.FindPlayerBySerNoAndGsNumber(nServerIndex, nGsNumber, &chatconndup))
	{
		dTRACE("Proc1_PlayerCommunity_QueryChannelID error (nServerIndex:%08X, nGsNumber:%08X)", nServerIndex, nGsNumber);
		return;
	}

	DWORD ip = chatconndup.GetIP();
	
	g_ChannelMgr.B_QueryChannelID(pPlayerCommQID->channel, GetIP(), pSrcInfo->lnID, pSrcInfo->nameid, ip);
}

void CHostConnect::Proc1_PlayerCommunity_FreeChannelID(const void* pData, size_t size)
{
	if (size < sizeof(PLAYERCOMM_FREECHANNELID) + sizeof(tagPlusSrcInfo))
		return;

	PLAYERCOMM_FREECHANNELID* pPlayerCommFID = (PLAYERCOMM_FREECHANNELID*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	g_ChannelMgr.B_FreeChannID(pPlayerCommFID->channelid, GetIP(), pSrcInfo->lnID);
}

void CHostConnect::Proc1_PlayerCommunity_Subscribe(const void* pData, size_t size)
{
	if (size < sizeof(PLAYERCOMM_SUBSCRIBE) + sizeof(tagPlusSrcInfo))
		return;

	PLAYERCOMM_SUBSCRIBE* pPlayerCommSub = (PLAYERCOMM_SUBSCRIBE*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	CNetConnectDup chatconndup;
	if(!g_ChatServer.FindPlayerBySerNoAndGsNumber(nServerIndex, nGsNumber, &chatconndup))
	{
		dTRACE("Proc1_PlayerCommunity_Subscribe error (nServerIndex:%08X, nGsNumber:%08X)", nServerIndex, nGsNumber);
		return;
	}

	DWORD ip = chatconndup.GetIP();
	if (pPlayerCommSub->subscribe)
		g_ChannelMgr.B_Subscribe(ip, pSrcInfo->lnID, pPlayerCommSub->channelid);
	else
		g_ChannelMgr.B_Unsubscribe(ip, pSrcInfo->lnID, pPlayerCommSub->channelid);

}

void CHostConnect::OnClientConnectCreate()
{
	rTRACE("host connect create: %s", _ip2a(GetIP()));
}

void CHostConnect::OnClientConnectClose()
{
	rTRACE("host connect close: %s", _ip2a(GetIP()));

	if (!g_RootClient.IsReady())
		return;

	size_t pckgsize = sizeof(RELAY_ASKWAY_DATA) + sizeof(EXTEND_HEADER) + sizeof(KServerInfo);

	RELAY_ASKWAY_DATA* pAskWayData = (RELAY_ASKWAY_DATA*)_alloca(pckgsize);
	pAskWayData->ProtocolFamily = pf_relay;
	pAskWayData->ProtocolID = relay_c2c_askwaydata;
	pAskWayData->nFromIP = 0;
	pAskWayData->nFromRelayID = 0;
	pAskWayData->seekRelayCount = 0;
	pAskWayData->seekMethod = rm_gm;
	pAskWayData->wMethodDataLength = 0;
	pAskWayData->routeDateLength = sizeof(EXTEND_HEADER) + sizeof(KServerInfo);

	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)(pAskWayData + 1);
	pHeader->ProtocolFamily = pf_normal;
	pHeader->ProtocolID = s2c_gatewayinfo;
	KServerInfo* pSvrInfo = (KServerInfo*)(pHeader + 1);
	pSvrInfo->Size = sizeof(KServerInfo);
	pSvrInfo->Type = ServerInfo;
	pSvrInfo->Operate = 0;
	strcpy(pSvrInfo->Version, ACCOUNT_CURRENT_VERSION);
	pSvrInfo->nValue = 0;
	pSvrInfo->Account[0] = 0;
	pSvrInfo->nServerType = server_Logout;
	pSvrInfo->nValue = GetIP();

	g_RootClient.SendPackage(pAskWayData, pckgsize);
}


BOOL CHostConnect::FindPlayerByAcc(const std::_tstring& acc, std::_tstring* pRole, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealAcc)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	ACCMAP::iterator it = m_mapAcc.find(acc);
	if (it == m_mapAcc.end())
		return FALSE;

	const ACCINFO& infoAcc = (*it).second;

	if (pRole != NULL)
		*pRole = infoAcc.role;
	if (pNameID != NULL)
		*pNameID = infoAcc.nameID;
	if (pParam != NULL)
		*pParam = infoAcc.param;

	if (pRealAcc != NULL)
		*pRealAcc = (*it).first;

	return TRUE;
}

BOOL CHostConnect::FindPlayerByRole(const std::_tstring& role, std::_tstring* pAcc, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealRole)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	ROLEMAP::iterator it = m_mapRole.find(role);
	if (it == m_mapRole.end())
		return FALSE;

	const ROLEINFO& infoRole = (*it).second;

	if (pAcc != NULL)
		*pAcc = infoRole.acc;
	if (pNameID != NULL)
		*pNameID = infoRole.nameID;
	if (pParam != NULL)
		*pParam = infoRole.param;

	if (pRealRole != NULL)
		*pRealRole = (*it).first;

	return TRUE;
}

int CHostConnect::CountLoginByHWID(const std::_tstring& hwid)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);
	
	int mCount = 0;
	for (PARAMMAP::iterator it = m_mapParam.begin(); it != m_mapParam.end(); it++)
	{
		const PARAMINFO& infoParam = (*it).second;
		if(infoParam.hwid == hwid)
			mCount++;
	}

	return mCount;
}

BOOL CHostConnect::FindPlayerByParam(unsigned long param, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	PARAMMAP::iterator it = m_mapParam.find(param);

	if (it == m_mapParam.end())
		return FALSE;

	const PARAMINFO& infoParam = (*it).second;

	if (pAcc != NULL)
		*pAcc = infoParam.acc;
	if (pRole != NULL)
		*pRole = infoParam.role;
	if (pNameID != NULL)
		*pNameID = infoParam.nameID;

	return TRUE;
}

/*
BOOL CHostConnect::BroadOnPlayer(BOOL byAcc, DWORD fromIP, unsigned long fromRelayID, const void* pData, size_t size)
{
	assert(pData != NULL && size > 0);

	size_t methodsize = _NAME_LEN + sizeof(DWORD) * 2;
	size_t pckgsize = sizeof(RELAY_ASKWAY_DATA) + methodsize + size;

	RELAY_ASKWAY_DATA* pAskwayData = (RELAY_ASKWAY_DATA*)_alloca(pckgsize);
	pAskwayData->ProtocolFamily = pf_relay;
	pAskwayData->ProtocolID = relay_c2c_askwaydata;
	pAskwayData->nFromIP = fromIP;
	pAskwayData->nFromRelayID = fromRelayID;
	pAskwayData->seekRelayCount = 0;
	pAskwayData->seekMethod = byAcc ? rm_account_id : rm_role_id;
	pAskwayData->wMethodDataLength = methodsize;
	pAskwayData->routeDateLength = size;

	char* pTheName = (char*)(pAskwayData + 1);
	DWORD* pTheInfo = (DWORD*)(pTheName + _NAME_LEN);

	void* pRouteData = pTheInfo + 2;
	memcpy(pRouteData, pData, size);


	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	for (PARAMMAP::const_iterator it = m_mapParam.begin(); it != m_mapParam.end(); it++)
	{
		const PARAMINFO& infoParam = (*it).second;
		strcpy(pTheName, byAcc ? infoParam.acc.c_str() : infoParam.role.c_str());
		pTheInfo[0] = infoParam.nameID;
		pTheInfo[1] = (*it).first;

		SendPackage(pAskwayData, pckgsize);
	}

	return TRUE;
}
*/

void CHostConnect::PrepareRecvs()
{
}

void CHostConnect::UnprepareRecvs()
{
	g_ChannelMgr.DoBlockOp(-1);
}


// [MAYID 16/09] xem HostConnect.h
DWORD CHostConnect::TimSeqChinhMinh(const std::_tstring& hwid, unsigned long lnID)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	PARAMMAP::iterator it = m_mapParam.find(lnID);

	if (it == m_mapParam.end())
		return 0;

	if ((*it).second.hwid != hwid)
		return 0;

	return (*it).second.seq;
}

int CHostConnect::DemHwidTruoc(const std::_tstring& hwid, DWORD seqChinhMinh)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	int nCount = 0;

	for (PARAMMAP::iterator it = m_mapParam.begin(); it != m_mapParam.end(); it++)
	{
		const PARAMINFO& infoParam = (*it).second;

		if (infoParam.hwid == hwid && (seqChinhMinh == 0 || infoParam.seq < seqChinhMinh))
			nCount++;
	}

	return nCount;
}


size_t CHostConnect::GetPlayerCount()
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	return m_mapRole.size();
}
