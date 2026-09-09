# -*- coding: utf-8 -*-
# [ANDROID 09/09] Dot va 20: VONG CHON DUOI CHAN DOI TUONG.
#
# Chu: "ben mobile kich vao doi tuong la co vong tron duoi chan doi tuong nham co dinh lai de hien
# thong tin". Ban JX1 Mobile cua chu lam bang SetInstantSpr(enumINSTANT_STATE_SELECT_NPC) trong Core.
# Ban nay KHONG di duong do vi hai le:
#   - KNpcRes chi co MOT khe anh phu (m_cSpecialSpr) va dang dung chung voi hieu ung ky nang
#     (KNpc.cpp:3383), lai tu tat sau mot luc -> vong se chop tat va cuop cho cua hieu ung ky nang;
#   - bang \settings\NpcRes\player_instant_special_file.txt cua bo du lieu nay DANG HONG (duong dan
#     cut "\spr\skill\", khong co ten tep) nen SetInstantSpr cung khong lay duoc anh.
#
# Di duong khac, gon va khong dong vao he ve cua Core:
#   1. Core: NPC_OI_TARGET_INFO tra them VI TRI VE cua muc tieu (toa do the gioi) + co "dang khoa".
#   2. Client: moi khung ve mot anh vong ngay tai vi tri do, dung DrawPrimitives voi
#      bSinglePlaneCoord = FALSE (toa do THE GIOI) nen Represent3 tu dat dung cho.
#
# Anh lay cua bo VNKU cua chu: \spr\npcres\focused_enemy_circle.spr va focused_non_enemy_circle.spr
# (da chep vao thu muc du lieu).
#
# Ban Windows khong doi hanh vi: moi khoi deu trong #ifdef JX_ANDROID (ke ca hai truong them vao
# struct, nen kich thuoc struct ben PC khong doi).
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


print("1. GameDataDef.h: them vi tri ve vao thong tin muc tieu")
va("Sources/Core/Src/GameDataDef.h", "nViTriVeX",
"""struct KUiTargetDetailInfo
{
	char			sTargetName[32];
	int				nLifePercent;
	BYTE			Series;
};""",
"""struct KUiTargetDetailInfo
{
	char			sTargetName[32];
	int				nLifePercent;
	BYTE			Series;
#ifdef JX_ANDROID
	// [ANDROID 09/09 VONG] vi tri VE cua muc tieu (toa do THE GIOI) de client ve vong chon duoi chan
	int				nViTriVeX;
	int				nViTriVeY;
	int				nDangKhoa;	// 1 = muc tieu dang KHOA (m_nPeopleIdx); 0 = chi dang tro chuot vao
#endif
};""", "GameDataDef.h them vi tri ve")

print("2. CoreShell.cpp: tra vi tri ve cua muc tieu")
va("Sources/Core/Src/CoreShell.cpp", "nViTriVeX",
"""			pTargetInfo->nLifePercent = (Npc[idx].m_CurrentLife * 100 / max(Npc[idx].m_CurrentLifeMax, 1));
			nRet = 1;""",
"""			pTargetInfo->nLifePercent = (Npc[idx].m_CurrentLife * 100 / max(Npc[idx].m_CurrentLifeMax, 1));
#ifdef JX_ANDROID
			// [ANDROID 09/09 VONG] tra them vi tri VE de client ve vong chon duoi chan muc tieu
			Npc[idx].GetDrawPos(&pTargetInfo->nViTriVeX, &pTargetInfo->nViTriVeY);
			pTargetInfo->nDangKhoa = (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx == idx) ? 1 : 0;
#endif
			nRet = 1;""", "CoreShell tra vi tri ve")

print("3. JxCanDieuKhien.cpp: ve vong chon")
THAN = r'''
//---------------------------------------------------------------------------
// [ANDROID 09/09 VONG] VONG CHON DUOI CHAN MUC TIEU
//
// Chu: "kich vao doi tuong la co vong tron duoi chan doi tuong nham co dinh lai de hien thong tin".
//
// Ve bang DrawPrimitives voi bSinglePlaneCoord = FALSE, tuc la dua TOA DO THE GIOI - Represent3 tu
// doi sang toa do man hinh, nen vong luon nam dung duoi chan du man hinh cuon di dau.
// Vi tri do Core tra ve trong KUiTargetDetailInfo (NPC_OI_TARGET_INFO), xem dot va 20.
//
// config.ini [Cham]:
//   VongChon=1                 ; 0 = tat
//   VongChonAnh=\spr\npcres\focused_non_enemy_circle.spr
//   VongChonAnhDich=\spr\npcres\focused_enemy_circle.spr   (dung khi muc tieu dang bi khoa danh)
//---------------------------------------------------------------------------
void JxVongChon_Ve()
{
	DocCaiDat();
	if (!s_nVongBat || g_pCoreShell == NULL || g_pRepresentShell == NULL)
		return;
	KUiTargetDetailInfo tt;
	memset(&tt, 0, sizeof(tt));
	if (!g_pCoreShell->GetGameData(NPC_OI_TARGET_INFO, (KUPARAM)&tt, 0))
		return;
	if (tt.sTargetName[0] == 0)
		return;
	if (s_nVongCoAnh < 0)
	{
		s_nVongCoAnh = (CoAnh(s_szVongAnh) && CoAnh(s_szVongAnhDich)) ? 1 : 0;
		g_DebugLog("[VONG] anh vong chon: %s -> co anh=%d", s_szVongAnh, s_nVongCoAnh);
	}
	if (!s_nVongCoAnh)
		return;
	static KRUImage s_Vong, s_VongDich;
	KRUImage& a = tt.nDangKhoa ? s_VongDich : s_Vong;
	const char* pszAnh = tt.nDangKhoa ? s_szVongAnhDich : s_szVongAnh;
	if (a.szImage[0] == 0)
	{
		memset(&a, 0, sizeof(a));
		a.nType = ISI_T_SPR;
		a.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		a.Color.Color_dw = 0xffffffff;
		a.nISPosition = IMAGE_IS_POSITION_INIT;
		a.nFrame = 0;
		strncpy(a.szImage, pszAnh, sizeof(a.szImage) - 1);
	}
	// Anh .spr ve tu goc tren-trai cua no nen phai lui lai nua khung, khong thi vong nam lech
	// xuong duoi ben phai chan doi tuong (da nhin tan mat o ban dung dau tien).
	KRPosition2 oOffV = { 0, 0 }, oCoV = { 0, 0 };
	int nLuiX = 0, nLuiY = 0;
	if (g_pRepresentShell->GetImageFrameParam(a.szImage, 0, &oOffV, &oCoV, a.nType) && oCoV.nX > 0)
	{
		nLuiX = oCoV.nX / 2;
		nLuiY = oCoV.nY / 2;
	}
	a.oPosition.nX = tt.nViTriVeX - nLuiX;
	a.oPosition.nY = tt.nViTriVeY - nLuiY;
	// FALSE = toa do THE GIOI (khong phai toa do man hinh) -> Represent3 tu dat dung cho
	g_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE, false);
}

'''
s = doc("Sources/S3Client/Platform/JxCanDieuKhien.cpp")
crlf = ("\r\n" in s)
if "JxVongChon_Ve" in s:
    print("  bo qua (da co): than ve vong chon")
else:
    neo = nl("void JxCan_Ve()\n{", crlf)
    if s.count(neo) != 1:
        print("  !! khong tim thay JxCan_Ve")
        sys.exit(1)
    i = s.find(neo)
    ghi("Sources/S3Client/Platform/JxCanDieuKhien.cpp", s[:i] + nl(THAN, crlf) + s[i:])
    print("  va xong: than ve vong chon")

va("Sources/S3Client/Platform/JxCanDieuKhien.cpp", "s_nVongBat = 1;",
"""static char	s_szAnhNum[128] = "\\\\spr\\\\Ui3\\\\UiSkillControl\\\\joystick_ctrl.spr";""",
"""static char	s_szAnhNum[128] = "\\\\spr\\\\Ui3\\\\UiSkillControl\\\\joystick_ctrl.spr";
static int	s_nVongBat = 1;
static int	s_nVongCoAnh = -1;
static char	s_szVongAnh[128]    = "\\\\spr\\\\npcres\\\\focused_non_enemy_circle.spr";
static char	s_szVongAnhDich[128] = "\\\\spr\\\\npcres\\\\focused_enemy_circle.spr";""", "khai bao bien vong chon")

va("Sources/S3Client/Platform/JxCanDieuKhien.cpp", 'GameDataDef.h"',
"""#include "../../Core/src/coreshell.h\"""",
"""#include "../../Core/src/coreshell.h"
#include "../../Core/src/GameDataDef.h\"""", "include GameDataDef.h")

va("Sources/S3Client/Platform/JxCanDieuKhien.cpp", "s_nVongBat   = GetPrivateProfileInt",
"""	GetPrivateProfileString("Cham", "CanAnhNum", s_szAnhNum, s_szAnhNum, sizeof(s_szAnhNum), szCfg);""",
"""	GetPrivateProfileString("Cham", "CanAnhNum", s_szAnhNum, s_szAnhNum, sizeof(s_szAnhNum), szCfg);
	s_nVongBat   = GetPrivateProfileInt("Cham", "VongChon", 1, szCfg);
	GetPrivateProfileString("Cham", "VongChonAnh", s_szVongAnh, s_szVongAnh, sizeof(s_szVongAnh), szCfg);
	GetPrivateProfileString("Cham", "VongChonAnhDich", s_szVongAnhDich, s_szVongAnhDich, sizeof(s_szVongAnhDich), szCfg);""", "doc cai dat vong chon")

va("Sources/S3Client/Platform/JxCanDieuKhien.h", "JxVongChon_Ve",
"""// Goi cuoi moi khung ve (UiShell::UiPaint): ve can len man hinh.
void JxCan_Ve();""",
"""// Goi cuoi moi khung ve (UiShell::UiPaint): ve can len man hinh.
void JxCan_Ve();
// Goi cuoi moi khung ve: ve vong chon duoi chan muc tieu dang chon.
void JxVongChon_Ve();""", "khai bao JxVongChon_Ve")

print("4. UiShell.cpp: goi ve vong chon")
va("Sources/S3Client/Ui/UiShell.cpp", "JxVongChon_Ve();",
"""	JxCan_Ve();	// [ANDROID 09/09 CAN] ve can len tren cung, ngay truoc khi ket khung""",
"""	JxVongChon_Ve();	// [ANDROID 09/09 VONG] vong chon duoi chan muc tieu (toa do the gioi)
	JxCan_Ve();	// [ANDROID 09/09 CAN] ve can len tren cung, ngay truoc khi ket khung""", "UiShell goi ve vong chon")

print("")
print("XONG dot va 20.")
