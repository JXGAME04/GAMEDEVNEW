// KTongJX2.h - JX2 port: ban sao du lieu bang hoi tren GameServer.
// Mo hinh JX2 (BANGHOI_JX2_PHANTICH.md):
//   - Relay la CHU du lieu; GameServer chi giu ban sao day du de doc (duyet duoc).
//   - Ham Lua TONG_*/TONGM_* doc ban sao nay; ho Apply* CHI gui goi len Relay
//     (khong ghi cuc bo) - du lieu cap nhat khi Relay phat echo ve.
//   - Bang: map field {WORD -> DWORD} (field 1..48 + bien nhiem vu >= 1002),
//     ngan quy 64-bit = field 3 (thap) + field 4 (cao).
//   - Thanh vien: map field (khoa 1..16 + >= 1001) + set quyen.
//////////////////////////////////////////////////////////////////////

#ifndef __KTONGJX2_H__
#define __KTONGJX2_H__

#ifdef _SERVER

#include "KWin32.h"
#include <map>
#include <set>

struct KTongJX2Member
{
	DWORD	dwNameID;
	char	szName[32];
	BYTE	btFigure;		// 0 bang chu / 1 truong lao / 2 doi truong / 3 bang chung / 4 an si
	BYTE	btSex;
	std::map<WORD, DWORD>	mapField;
	std::set<DWORD>			setRight;
};

struct KTongJX2Tong
{
	DWORD	dwNameID;
	char	szName[32];
	BYTE	btCamp;
	std::map<WORD, DWORD>			mapField;
	std::map<DWORD, KTongJX2Member>	mapMember;	// khoa = NameID thanh vien
	DWORD	dwTaskTemp[256];					// bo dem cuc bo, KHONG dong bo (dung JX2)
	char	szAnnounce[128];					// thong bao bang (= defTONG_JX2_ANNOUNCE_LEN)
	DWORD	dwWsNpc[8];							// NPC cong trinh tac phuong (cuc bo GS, khong dong bo)
};

class KTongJX2Mgr
{
public:
	std::map<DWORD, KTongJX2Tong>	m_mapTong;	// khoa = NameID bang
	BOOL	m_bSynced;							// da nhan xong dump dau tien tu relay

	KTongJX2Mgr() : m_bSynced(FALSE) {}

	KTongJX2Tong*	FindTong(DWORD dwTongNameID);
	KTongJX2Member*	FindMember(KTongJX2Tong* pTong, DWORD dwMemberNameID);

	DWORD	GetField(DWORD dwTongNameID, WORD wKey);
	DWORD	GetMemberField(KTongJX2Member* pMember, WORD wKey);

	// Nhan goi pf_tong enumS2C_TONG_JX2_* tu relay (chuyen qua KSOServer -> CoreServerShell)
	void	OnRelayPacket(const void* pData, int nSize);
};

extern KTongJX2Mgr g_TongJX2;

#endif // _SERVER

#endif // __KTONGJX2_H__
