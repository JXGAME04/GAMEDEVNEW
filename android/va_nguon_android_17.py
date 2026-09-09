# -*- coding: utf-8 -*-
# [ANDROID 09/09] Dot va 17: CHAM VAO THANH THONG TIN MUC TIEU -> RA DANH SACH TUY CHON.
#
# Chu ta duong di theo nguon tham khao: "kich vao doi tuong se hien ra thong tin doi tuong tren man
# hinh - roi kich vao thong tin do se hien ra danh sach cac dong thong tin tuy chon".
#
# Doc ky roi thi ra: HAI phan dau DA CO SAN trong ban nay, chi thieu mat xich thu ba.
#   1. Cham vao doi tuong  -> thanh thong tin muc tieu hien len: KUiTargetInfo (da chay, da chup anh).
#   2. Danh sach tuy chon  -> PopUpContextPeopleMenu() trong UiGame.cpp (giao dich / to doi / ket ban /
#      xem trang bi). Tren ban PC no mo bang Ctrl + chuot phai (ui\autoexec.lua goi Mouse_Menu).
#   3. THIEU: cham vao chinh thanh thong tin do thi khong co gi xay ra.
#
# Dot nay noi mat xich 3: bam vao nen hoac anh dau cua thanh thong tin muc tieu -> mo dung danh sach
# tuy chon do. Cach dung ten muc tieu de dung KUiPlayerItem lay y het UiMsgCentrePad.cpp (cho bam vao
# ten nguoi trong khung chat): FindSpecialNPC(ten) roi lui ve dien tay neu khong tim thay.
#
# Nho vay KHONG phai dong vao Core (ban USVOLAM them han mot thong bao GDCNI_SHOW_NPC_BAR vao Core de
# lam viec nay - ban nay khong can vi da co san thanh thong tin muc tieu).
#
# Ban Windows khong doi hanh vi: moi khoi deu trong #ifdef JX_ANDROID.
import io, os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def doc(p):
    return io.open(os.path.join(ROOT, p), encoding="latin-1", newline="").read()

def ghi(p, s):
    io.open(os.path.join(ROOT, p), "w", encoding="latin-1", newline="").write(s)

def nl(s, crlf):
    return s.replace("\n", "\r\n") if crlf else s

def va(duong, dau_da_co, cu, moi, ten):
    s = doc(duong)
    crlf = ("\r\n" in s)
    if nl(dau_da_co, crlf) in s:
        print("  bo qua (da co): %s" % ten)
        return
    cu2, moi2 = nl(cu, crlf), nl(moi, crlf)
    if s.count(cu2) != 1:
        print("  !! KHONG VA DUOC %s: tim thay %d cho (can dung 1)" % (ten, s.count(cu2)))
        sys.exit(1)
    ghi(duong, s.replace(cu2, moi2))
    print("  va xong: %s" % ten)


T = "Sources/S3Client/Ui/UiCase/UiTargetInfo.cpp"
TH = "Sources/S3Client/Ui/UiCase/UiTargetInfo.h"

print("1. UiTargetInfo.h: khai bao ham mo menu")
va(TH, "MoMenuMucTieu",
"""	void	PaintWindow();""",
"""	void	PaintWindow();
#ifdef JX_ANDROID
	void	MoMenuMucTieu();	// [ANDROID 09/09 MENU] cham vao thanh thong tin -> danh sach tuy chon
#endif""", "UiTargetInfo.h khai bao")

print("2. UiTargetInfo.cpp: them include va than ham")
va(T, '#include "UiGame.h"',
"""#include "UiLoginBg.h\"""",
"""#include "UiLoginBg.h"
#ifdef JX_ANDROID
#include "UiGame.h"		// [ANDROID 09/09 MENU] PopUpContextPeopleMenu
#endif""", "UiTargetInfo.cpp include UiGame.h")

va(T, "void KUiTargetInfo::MoMenuMucTieu()",
"""int KUiTargetInfo::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)""",
"""#ifdef JX_ANDROID
//--------------------------------------------------------------------------
// [ANDROID 09/09 MENU] Cham vao thanh thong tin muc tieu -> ra danh sach tuy chon.
//
// Duong di tren dien thoai: cham doi tuong -> thanh nay hien len (da co san) -> cham vao thanh
// -> danh sach giao dich / to doi / ket ban / xem trang bi. Tren ban PC danh sach do mo bang
// Ctrl + chuot phai; ngon tay khong co phim Ctrl nen di duong nay.
//
// Cach dung KUiPlayerItem tu TEN muc tieu lay y het UiMsgCentrePad.cpp (bam vao ten nguoi trong
// khung chat): hoi Core bang FindSpecialNPC, khong thay thi dien tay ten va de nIndex = -1.
// Chi nguoi choi moi co danh sach nay; muc tieu la quai / NPC thuong thi khong lam gi.
//--------------------------------------------------------------------------
void KUiTargetInfo::MoMenuMucTieu()
{
	if (g_pCoreShell == NULL || m_Info.sTargetName[0] == 0)
		return;
	KUiPlayerItem SelectPlayer;
	memset(&SelectPlayer, 0, sizeof(SelectPlayer));
	int nKind = -1;
	if (!(g_pCoreShell->FindSpecialNPC(m_Info.sTargetName, &SelectPlayer, nKind) && nKind == kind_player))
		return;		// khong phai nguoi choi thi khong co danh sach tuy chon
	// Dat danh sach NGAY DUOI than thanh, khong de len thanh: o [Main] chi la mot o 27x23 con than
	// thanh la cac o CON (BackGround0, Head0...), nen phai lay khung bao ca dam con moi ra dung day.
	int x = 0, y = 0;
	RECT rc;
	GetAllChildLayoutRect(&rc);
	if (rc.right > rc.left && rc.bottom > rc.top)
	{
		x = rc.left;
		y = rc.bottom + 4;
	}
	else
	{
		GetAbsolutePos(&x, &y);
		y += m_Height;
	}
	PopUpContextPeopleMenu(SelectPlayer, x, y);
}
#endif

int KUiTargetInfo::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)""", "than ham MoMenuMucTieu")

print("3. Noi vao su kien bam nut")
va(T, "MoMenuMucTieu();",
"""		case WND_N_BUTTON_CLICK:
		{
			if (uParam == (KUPARAM)(KWndWindow*)&m_btnSwitch)""",
"""		case WND_N_BUTTON_CLICK:
		{
#ifdef JX_ANDROID
			// [ANDROID 09/09 MENU] bam vao nen hoac anh dau cua thanh = mo danh sach tuy chon
			if (uParam == (KUPARAM)(KWndWindow*)&a_btnBackGround ||
				uParam == (KUPARAM)(KWndWindow*)&a_IconHead)
			{
				MoMenuMucTieu();
				break;
			}
#endif
			if (uParam == (KUPARAM)(KWndWindow*)&m_btnSwitch)""", "noi vao WND_N_BUTTON_CLICK")

print("4. Cho ca than thanh nhan duoc cham")
va(T, "WND_S_SIZE_WITH_ALL_CHILD",
"""        KWndShowAnimate::Init(&Ini, "Main");""",
"""        KWndShowAnimate::Init(&Ini, "Main");
#ifdef JX_ANDROID
        // [ANDROID 09/09 MENU] O [Main] cua kuitargetinfo.ini chi la mot o 27x23, con than thanh
        // (BackGround0, Head0...) la cac o CON nam ngoai o do. Nen cham vao than thanh se ROI RA
        // NGOAI cua so, xuong ban do -> bo chon muc tieu thay vi mo menu (da do tan mat).
        // Bat co "do trung theo ca dam con" de ca than thanh nhan duoc cham.
        m_Style |= WND_S_SIZE_WITH_ALL_CHILD;
#endif""", "thanh nhan cham theo ca vung con")

print("")
print("XONG dot va 17.")
