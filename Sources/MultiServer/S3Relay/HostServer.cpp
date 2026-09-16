// HostServer.cpp: implementation of the CHostServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HostServer.h"
#include "HostConnect.h"
#include "S3Relay.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHostServer::CHostServer()
{

}

CHostServer::~CHostServer()
{

}


CNetConnect* CHostServer::CreateConnect(CNetServer* pNetServer, unsigned long id)
{
	return new CHostConnect((CHostServer*)pNetServer, id);
}

void CHostServer::DestroyConnect(CNetConnect* pConn)
{
	delete pConn;
}

void CHostServer::OnBuildup()
{
	AUTOLOCKWRITE(m_lockIpMap);

	rTRACE("host server startup");
}

void CHostServer::OnClearup()
{
	AUTOLOCKWRITE(m_lockIpMap);

	m_mapIp2Connect.clear();

	rTRACE("host server shutdown");
}

void CHostServer::OnClientConnectCreate(CNetConnect* pConn)
{
	AUTOLOCKWRITE(m_lockIpMap);

	m_mapIp2Connect[pConn->GetIP()] = (CHostConnect*)pConn;
}

void CHostServer::OnClientConnectClose(CNetConnect* pConn)
{
	AUTOLOCKWRITE(m_lockIpMap);

	m_mapIp2Connect.erase(pConn->GetIP());
}

CNetConnectDup CHostServer::FindHostConnectByIP(DWORD IP)
{
	AUTOLOCKREAD(m_lockIpMap);

	IP2CONNECTMAP::iterator it = m_mapIp2Connect.find(IP);
	if (it == m_mapIp2Connect.end())
		return CNetConnectDup();

	CHostConnect* pHostConn = (*it).second;
	if (!pHostConn)
		return CNetConnectDup();

	return CNetConnectDup(*pHostConn);
}

CNetConnectDup CHostServer::FindHostConnectByMapID(int nMapID)
{
	AUTOLOCKREAD(m_lockIpMap);
	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CHostConnect* pConnMe = (*it).second;
		if(pConnMe->ncheckMapIn(nMapID))
			return CNetConnectDup(*pConnMe);
	}
	return CNetConnectDup();
}

BOOL CHostServer::FindPlayerByAcc(CHostConnect* pConn, const std::_tstring& acc, CNetConnectDup* pConnDup, std::_tstring* pRole, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealAcc)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); it++)
	{
		CHostConnect* pConnMe = (*it).second;

		if (pConnMe != pConn)
		{
			if (pConnMe->FindPlayerByAcc(acc, pRole, pNameID, pParam, pRealAcc))
			{
				if (pConnDup != NULL)
					*pConnDup = *pConnMe;

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CHostServer::FindPlayerByRole(CHostConnect* pConn, const std::_tstring& role, CNetConnectDup* pConnDup, std::_tstring* pAcc, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealRole)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); it++)
	{
		CHostConnect* pConnMe = (*it).second;

		if (pConnMe != pConn)
		{
			if (pConnMe->FindPlayerByRole(role, pAcc, pNameID, pParam, pRealRole))
			{
				if (pConnDup != NULL)
					*pConnDup = *pConnMe;

				return TRUE;
			}
		}
	}

	return FALSE;
}

/*
 * [MAYID 16/09] Dem so phien dang mang ma may nay tren MOI GameServer dang noi vao relay.
 *
 * Truoc: duyet m_mapIp2Connect. Bang do khoa theo DWORD IP: GameServer thu hai cung IP GHI DE muc cua GameServer
 * thu nhat, va OnClientConnectClose erase(ip) khi MOT trong hai dong => co luc khong GameServer nao cua IP do
 * duoc dem. Nay duyet m_mapId2Connect cua CNetServer (moi ket noi dang song, giong CNetServer::Route).
 *
 * Cong don bang int va KEP o 255: num_login tren day (KTongProtocol.h) la BYTE, 256 phien cung ma se quay ve 0
 * = qua nguong; ma dung chung (KHONG-KHAI-MA-MAY, UUID mau...) hoan toan co the toi con so do.
 */
struct _MayIdDemCtx
{
	const std::_tstring* pHwid;
	int nTong;
	int nKetNoi;
};

static void _MayIdDemMotKetNoi(CNetConnect* pConn, void* pCtx)
{
	_MayIdDemCtx* p = (_MayIdDemCtx*)pCtx;

	p->nKetNoi++;
	p->nTong += ((CHostConnect*)pConn)->CountLoginByHWID(*p->pHwid);
}

BYTE CHostServer::CountLoginByHWID(CHostConnect* pConn, const std::_tstring& hwid)
{
	_MayIdDemCtx ctx;

	ctx.pHwid = &hwid;
	ctx.nTong = 0;
	ctx.nKetNoi = 0;

	ForEachConnect(_MayIdDemMotKetNoi, &ctx);

	if (ctx.nTong > 255)
	{
		rTRACE("[MAYID] ma may [%s] co %d phien tren %d GameServer, KEP ve 255", hwid.c_str(), ctx.nTong, ctx.nKetNoi);
		ctx.nTong = 255;
	}

	return (BYTE)ctx.nTong;
}

/* [MAYID 16/09] xem HostServer.h */
struct _MayIdTimCtx
{
	const std::_tstring* pHwid;
	unsigned long lnID;
	unsigned int nSerIdx;
	unsigned int nGsNum;
	DWORD ip;
	int nMucKhop;		// 3 = cung (serRegIndex, gsNumber), 2 = cung IP, 1 = bat ky
	DWORD dwSeq;
};

static void _MayIdTimChinhMinh(CNetConnect* pConn, void* pCtx)
{
	_MayIdTimCtx* p = (_MayIdTimCtx*)pCtx;
	CHostConnect* pHost = (CHostConnect*)pConn;
	DWORD dwSeq = pHost->TimSeqChinhMinh(*p->pHwid, p->lnID);
	int nMuc;

	if (dwSeq == 0)
		return;

	if (pHost->getSerRegIndex() == p->nSerIdx && pHost->getGsNumber() == p->nGsNum)
		nMuc = 3;
	else if (pHost->GetIP() == p->ip)
		nMuc = 2;
	else
		nMuc = 1;

	/* cung muc khop thi lay muc ghi so MOI NHAT: cau hoi den ngay sau khi ghi so */
	if (nMuc > p->nMucKhop || (nMuc == p->nMucKhop && dwSeq > p->dwSeq))
	{
		p->nMucKhop = nMuc;
		p->dwSeq = dwSeq;
	}
}

struct _MayIdDemTruocCtx
{
	const std::_tstring* pHwid;
	DWORD dwSeq;
	int nTong;
};

static void _MayIdDemTruocMotKetNoi(CNetConnect* pConn, void* pCtx)
{
	_MayIdDemTruocCtx* p = (_MayIdDemTruocCtx*)pCtx;

	p->nTong += ((CHostConnect*)pConn)->DemHwidTruoc(*p->pHwid, p->dwSeq);
}

int CHostServer::DemHwidTruocNguoiHoi(const std::_tstring& hwid, unsigned long lnID, unsigned int nSerRegIndex, unsigned int nGsNumber, DWORD ipHoi, BOOL* pbThayChinhMinh)
{
	_MayIdTimCtx tim;
	_MayIdDemTruocCtx dem;

	tim.pHwid = &hwid;
	tim.lnID = lnID;
	tim.nSerIdx = nSerRegIndex;
	tim.nGsNum = nGsNumber;
	tim.ip = ipHoi;
	tim.nMucKhop = 0;
	tim.dwSeq = 0;

	ForEachConnect(_MayIdTimChinhMinh, &tim);

	if (pbThayChinhMinh)
		*pbThayChinhMinh = (tim.dwSeq != 0);

	dem.pHwid = &hwid;
	dem.dwSeq = tim.dwSeq;
	dem.nTong = 0;

	ForEachConnect(_MayIdDemTruocMotKetNoi, &dem);

	return dem.nTong;
}

BOOL CHostServer::FindPlayerByIpParam(CHostConnect* pConn, DWORD ip, unsigned long param, CNetConnectDup* pConnDup, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID)
{
	AUTOLOCKREAD(m_lockIpMap);

	IP2CONNECTMAP::const_iterator it = m_mapIp2Connect.find(ip);
	if (it == m_mapIp2Connect.end())
		return FALSE;

	CHostConnect* pConnMe = (*it).second;

	if (pConnMe != pConn)
	{
		if (pConnMe->FindPlayerByParam(param, pAcc, pRole, pNameID))
		{
			if (pConnDup != NULL)
				*pConnDup = *pConnMe;

			return TRUE;
		}
	}

	return FALSE;
}

/*
BOOL CHostServer::BroadOnPlayer(BOOL byAcc, DWORD fromIP, unsigned long fromRelayID, const void* pData, size_t size)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::const_iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); it++)
	{
		CHostConnect* pConnMe = (*it).second;

		pConnMe->BroadOnPlayer(byAcc, fromIP, fromRelayID, pData, size);
	}

	return FALSE;
}
*/


size_t CHostServer::GetPlayerCount()
{
	AUTOLOCKREAD(m_lockIpMap);

	size_t total = 0;

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); it++)
	{
		CHostConnect* pConn = (*it).second;

		assert(pConn);
		if (pConn)
			total += pConn->GetPlayerCount();
	}

	return total;
}


BOOL CHostServer::TraceInfo()
{
	AUTOLOCKREAD(m_lockIpMap);

	std::_tstring info("message: [HostServer] ");
	char buffer[_MAX_PATH];

	sprintf(buffer, "<total: %d> : ", m_mapIp2Connect.size());
	info.append(buffer);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); it++)
	{
		if (it != m_mapIp2Connect.begin())
			info.append(", ");

		sprintf(buffer, "%08X", (*it).first);
		info.append(buffer);
	}

	rTRACE(info.c_str());

	return TRUE;
}

BOOL CHostServer::TracePlayer()
{
	AUTOLOCKREAD(m_lockIpMap);

	std::_tstring info("message: [Player] : ");
	char buffer[_MAX_PATH];

	size_t total = 0;

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); it++)
	{
		CHostConnect* pConn = (*it).second;

		assert(pConn);
		if (pConn)
		{
			if (it != m_mapIp2Connect.begin())
				info.append(", ");

			size_t count = pConn->GetPlayerCount();
			total += count;

			sprintf(buffer, "%08X: %d", (*it).first, count);
			info.append(buffer);
		}
	}

	sprintf(buffer, " <total: %d>", total);
	info.append(buffer);

	rTRACE(info.c_str());

	return TRUE;
}

