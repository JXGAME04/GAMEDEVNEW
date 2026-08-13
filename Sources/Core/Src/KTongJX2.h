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
	char	szRecruit[128];						// van an chieu mo (STR kind 3)
	BYTE	btApplyCount;						// don xin vao bang (cuc bo GS, toi da 8)
	char	szApplyName[8][32];
	DWORD	dwApplyID[8];
	WORD	wApplyLevel[8];
	BYTE	btApplySex[8];						// nho gioi tinh de duyet duoc ca khi nguoi xin da thoat
	char	szEvent[16][96];					// so su kien (ring, = RECORD_NUM/LEN)
	char	szHistory[16][96];
	int		nEventHead;
	int		nHistoryHead;
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

	// Phuc vu cua so client JX2 (SGDI_TONG_JX2VIEW / SGDI_TONG_JX2OP):
	// dung goi TONG_JX2_*_SYNC vao pOut tu ban sao -> tra so byte (0 = loi)
	int		BuildClientView(int nPlayerIdx, int nPage, int nStart, void* pOut, int nOutSize);
	// thao tac tu cua so (kiem quyen theo bang ID JX2 roi push len relay) -> 0 = ok
	int		DoClientOp(int nPlayerIdx, const void* pCmd);
	int		DoClientOpBody(int nPlayerIdx, const void* pCmd);
};

extern KTongJX2Mgr g_TongJX2;

#endif // _SERVER

#endif // __KTONGJX2_H__
