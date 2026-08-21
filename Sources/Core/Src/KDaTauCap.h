// KDaTauCap.h - bo dem chup goi hoi thoai/cua so cho auto Da Tau (auto ngoai WAuto).
// Cac hook GHI: KPlayer.cpp (OnScriptAction: UI_SELECTDIALOG + UI_TALKDIALOG),
// KProtocolProcess.cpp (s2cOpenQuestFinishDlg / OpenAffairBox / s2cExtendChat).
// May trang thai ATYPE_DATAU (CoreShell.cpp) DOC, so sanh seq de biet du lieu moi.
// Bien the hien dinh nghia trong KPlayer.cpp (compile ca client lan server, vo hai).
#ifndef KDATAUCAP_H
#define KDATAUCAP_H

// (r5f) id DAT CHO goi xin DANH BA SAP (di nho goi c2s_playerneedcount cu).
// PHAI dung chung o 3 noi: KNpcSet::SetID (bo qua khi cap id - khong de npc
// that mang id nay), KProtocolProcess::c2sNeedCount (server), CoreShell
// DTP_MUASAP (client hoi).
#define DATAU_SAPMAP_ID 0x0DA75AB1u
// tran do dai chuoi danh ba: sentlen tren duong day la BYTE va
// KPlayerChat::SendSystemInfo kep = MAX_SENTENCE_LENGTH (256) -> 256 TRAN VE 0
// lam mat trang ca goi. Giu duoi 200 cho chac.
#define DATAU_SAPMAP_MAXLEN 200

struct KDaTauCapture
{
	unsigned int uDlgSeq;    // tang moi UI_SELECTDIALOG (Say)
	char szDlg[2048];        // "cau hoi|dap an 1|dap an 2|..." (TCVN3, CHUA ma hoa/tach)
	unsigned int uTalkSeq;   // tang moi UI_TALKDIALOG (Talk)
	char szTalk[1024];       // "cau 1|cau 2|..."
	unsigned int uFinSeq;    // tang moi s2c_openquestfinishdlg (cua so 3 ruong)
	int nFinType;            // m_bType: <=4 KUiDaTau (exp/money/random), >4 KUiDaTau1
	unsigned int uBoxSeq;    // tang moi khi give-box MO (S2C_GIVE_BOX nType==1)
	int nBoxOpen;            // 1 dang mo, 0 da dong
	char szBoxFunc[64];      // ten ham nop (S2C_GIVE_BOX.Value2)
	unsigned int uMsgSeq;    // tang moi thong diep "He Thong" (tien do nhat cuon...)
	// (20/08 r3) VONG 4 KHE: 2 tin den trong 1 tick engine (~250ms) truoc day
	// de mat tin dau; tin CHOT "du manh" bi de la bot khong bao gio ve tra.
	// Khe cua tin thu N = aMsg[N & 3].
	char aMsg[4][320];
	// (r4 - PB) kenh RIENG cho tin TIEN DO ("tong cong N tam"): loat thong bao
	// toan server (Top Cao Thu phat 10+ tin/khung) co the quet sach vong khe
	// chung - tin tien do luon co cho o day, khong ai de duoc.
	unsigned int uTienSeq;
	char szTien[320];
	// (r5c) tra loi tham do SO MON cua sap (s2c_playergetcount): server chi tra
	// loi khi FindAroundPlayer thay PLAYER that (nguoi choi / bot PB) - sap
	// trang tri SimCity (KNpc kind_player, m_nPlayerIdx=0) IM LANG -> day la
	// cach duy nhat phan biet sap that/gia tu client.
	unsigned int uCntSeq;
	unsigned int dwCntId;
	int nCnt;
	// (r5e) DANH BA SAP: hoi bang needcount dwId=0x0DA75AB1, server tra
	// "[SapMap] id:x:y ..." (toa do CELL) qua tin He Thong - chi sap co PLAYER
	// that (nguoi + bot PB), dan SimCity tu bi loai.
	unsigned int uSapMapSeq;
	char szSapMap[400];
};

extern KDaTauCapture g_sDTCap;

#endif // KDATAUCAP_H
