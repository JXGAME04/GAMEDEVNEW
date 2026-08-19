// KDaTauCap.h - bo dem chup goi hoi thoai/cua so cho auto Da Tau (auto ngoai WAuto).
// Cac hook GHI: KPlayer.cpp (OnScriptAction: UI_SELECTDIALOG + UI_TALKDIALOG),
// KProtocolProcess.cpp (s2cOpenQuestFinishDlg / OpenAffairBox / s2cExtendChat).
// May trang thai ATYPE_DATAU (CoreShell.cpp) DOC, so sanh seq de biet du lieu moi.
// Bien the hien dinh nghia trong KPlayer.cpp (compile ca client lan server, vo hai).
#ifndef KDATAUCAP_H
#define KDATAUCAP_H

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
	char szMsg[512];         // noi dung thong diep (TCVN3)
};

extern KDaTauCapture g_sDTCap;

#endif // KDATAUCAP_H
