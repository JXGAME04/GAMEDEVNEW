// KTongJX2Relay.h - JX2 port: xu ly du lieu KV + quyen bang hoi phia Relay.
// Mo hinh theo BANGHOI_JX2_PHANTICH.md: Relay la chu du lieu, GameServer chi giu ban sao.
//////////////////////////////////////////////////////////////////////

#ifndef __KTONGJX2RELAY_H__
#define __KTONGJX2RELAY_H__

// 1 muc thanh vien rut gon dung khi dump ban sao xuong GameServer
struct JX2MemberBrief
{
	DWORD	dwNameID;
	char	szName[32];
	BYTE	btFigure;		// 0 bang chu / 1 truong lao / 2 doi truong / 3 bang chung
	BYTE	btSex;
};

class CTongConnect;

// Cac ham xu ly goi enumC2S_TONG_JX2_* (goi tu CTongConnect::Proc0_Tong)
void JX2_ProcTongField(CTongConnect* pConn, const void* pData);
void JX2_ProcTongMoney(CTongConnect* pConn, const void* pData);
void JX2_ProcMemberField(CTongConnect* pConn, const void* pData);
void JX2_ProcRight(CTongConnect* pConn, const void* pData);
void JX2_ProcGetFull(CTongConnect* pConn);
void JX2_ProcString(CTongConnect* pConn, const void* pData);
void JX2_ProcTongOp(CTongConnect* pConn, const void* pData);
void JX2_ProcZhaoMu(CTongConnect* pConn, const void* pData);

// Bo hen gio bang hoi (goi tu WM_TIMER cua so chinh relay, 30s/lan):
// bao tri ngay 00:00 + quy -> kinh nghiem moi 750s ([tongjx2] relay_config.ini)
void JX2_TimerTick();

#endif // __KTONGJX2RELAY_H__
