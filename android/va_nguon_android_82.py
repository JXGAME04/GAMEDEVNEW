# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 ICON e] Chu: "icon giao tiep ... chi hien khi ban dung gan va hien phia goc phai man hinh - dua icon do vao
# phan chinh toa do de toi tu dieu chinh". Icon doi_thoai nay nam CO DINH tren man hinh (mac dinh goc phai: W-200, H/2+40),
# chi hien khi NPC doi thoai gan nhat cach nhan vat <= [Cham] IconNpcGan px (tren man hinh, tinh tu giua khung ve),
# dang ky voi UiToaDo (khoa "IconNpc") nen keo duoc trong che do "Sua giao dien" (vi tri luu UserData\UiToaDo.ini),
# trong che do sua thi luon ve de keo. Cham icon = bam vao than NPC gan (do diem nhu ICON c). IconNpcCao khong dung nua.

import io
import os
import re

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 ICON e]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, ham):
    s = doc(p)
    if DAU in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


VUNG_MOI = r'''//	[ANDROID 11/09 ICON e] Icon "doi thoai" nam CO DINH tren man hinh (mac dinh goc phai), chi hien khi dung GAN NPC doi thoai
//	(cach nhan vat <= IconNpcGan px tren man hinh); keo duoc trong che do "Sua giao dien" (khoa IconNpc, UiToaDo).
//	Chu: "icon giao tiep ... chi hien khi ban dung gan va hien phia goc phai man hinh ... dua icon do vao phan chinh toa do".
static void IconNpc_Tam(int* px, int* py)
{
	*px = (s_nIconHudX >= 0) ? s_nIconHudX : (SCREEN_WIDTH - 200);
	*py = (s_nIconHudY >= 0) ? s_nIconHudY : (SCREEN_HEIGHT / 2 + 40);
}

static bool IconNpc_ORiengTrung(void* pNgu, int x, int y)
{
	int nX, nY, nR = s_nIconRong / 2 + 6;

	(void)pNgu;
	IconNpc_Tam(&nX, &nY);
	return (x - nX) * (x - nX) + (y - nY) * (y - nY) <= nR * nR;
}

static void IconNpc_ORiengLay(void* pNgu, int* px, int* py)
{
	(void)pNgu;
	IconNpc_Tam(px, py);
}

static void IconNpc_ORiengDat(void* pNgu, int x, int y)
{
	(void)pNgu;
	s_nIconHudX = x;
	s_nIconHudY = y;
}

void JxIconNpc_Ve()
{
	static bool s_bDaDangKy = false;
	bool bGan = false;
	KUiTargetDetailInfo gan;

	DocCaiDat();
	if (!s_nIconBat || g_pCoreShell == NULL || g_pRepresentShell == NULL)
		return;
	if (!s_bDaDangKy)
	{
		s_bDaDangKy = true;
		UiToaDo_DangKyORieng("IconNpc", IconNpc_ORiengTrung, IconNpc_ORiengLay, IconNpc_ORiengDat, NULL);
	}
	if (s_nIconCoAnh < 0)
	{
		s_nIconCoAnh = CoAnh(s_szIconAnh) ? 1 : 0;
		g_DebugLog("[ICON] anh icon NPC: %s -> co anh=%d", s_szIconAnh, s_nIconCoAnh);
	}
	if (!s_nIconCoAnh)
		return;
	// NPC doi thoai gan nhat co GAN khong: <= IconNpcGan px tren man hinh, tinh tu giua khung ve (= nhan vat)
	memset(&gan, 0, sizeof(gan));
	if (g_pCoreShell->GetGameData(NPC_OI_TARGET_INFO, (KUPARAM)&gan, 1) && gan.sTargetName[0])
	{
		int x = gan.nViTriVeX, y = gan.nViTriVeY;
		int dx, dy;

		g_pRepresentShell->CoordinateTransform(x, y, 0);	// the gioi -> man hinh (chan NPC)
		dx = x - SCREEN_WIDTH / 2;
		dy = y - SCREEN_HEIGHT / 2;
		if (x >= 0 && y >= 0 && x < SCREEN_WIDTH && y < SCREEN_HEIGHT && dx * dx + dy * dy <= s_nIconGan * s_nIconGan)
		{
			bGan = true;
			s_nIconNpcX = x;
			s_nIconNpcY = y;
			strncpy(s_szIconTen, gan.sTargetName, sizeof(s_szIconTen) - 1);
			s_szIconTen[sizeof(s_szIconTen) - 1] = 0;
		}
	}
	s_nIconNpcCo = bGan ? 1 : 0;
	if (!bGan && !UiToaDo_DangSua())
	{
		s_uIconVeLuc = 0;
		return;
	}
	{
		int nTX, nTY;
		KRPosition2 oOffI = { 0, 0 }, oCoI = { 0, 0 };
		int nRong = s_nIconRong, nCao = s_nIconRong;
		int nKhung = bGan ? (int)(((unsigned int)GetTickCount() / 500) % (unsigned int)KyNang_SoKhung(s_szIconAnh)) : 0;

		IconNpc_Tam(&nTX, &nTY);
		if (g_pRepresentShell->GetImageFrameParam(s_szIconAnh, 0, &oOffI, &oCoI, ISI_T_SPR) && oCoI.nX > 0 && oCoI.nY > 0)
			nCao = nRong * oCoI.nY / oCoI.nX;
		VeAnhRong(s_szIconAnh, nTX - nRong / 2, nTY - nCao / 2, nRong, nCao, nKhung);	// 2 khung nhap nhay 0,5 s khi co NPC gan
		s_nIconVeX0 = nTX - nRong / 2 - 8;
		s_nIconVeX1 = nTX + nRong / 2 + 8;
		s_nIconVeY0 = nTY - nCao / 2 - 8;
		s_nIconVeY1 = nTY + nCao / 2 + 8;
		s_uIconVeLuc = (unsigned int)GetTickCount();
	}
}

//	Cham trung icon vua ve (trong 300 ms) khi dang co NPC gan -> do diem bam trung THAN NPC (Core FindSelectNPC tai
//	chan-40/55/25/70/12/85, diem khong bi giao dien che) de KSdlApp bam vao nhu cham thang NPC: di toi + mo thoai.
int JxIconNpc_Cham(int x, int y, int* pnX, int* pnY)
{
	static const int aLui[] = { 40, 55, 25, 70, 12, 85 };
	int i;

	if (!s_nIconBat || !s_nIconNpcCo || s_uIconVeLuc == 0 || (unsigned int)GetTickCount() - s_uIconVeLuc > 300)
		return 0;
	if (x < s_nIconVeX0 || x > s_nIconVeX1 || y < s_nIconVeY0 || y > s_nIconVeY1)
		return 0;
	for (i = 0; i < (int)(sizeof(aLui) / sizeof(aLui[0])); i++)
	{
		KUiPlayerItem oAi;
		int nKind = 0;
		int nX = s_nIconNpcX, nY = s_nIconNpcY - aLui[i];

		memset(&oAi, 0, sizeof(oAi));
		if (g_pCoreShell && g_pCoreShell->FindSelectNPC(nX, nY, relation_all, false, &oAi, nKind)
			&& strncmp(oAi.Name, s_szIconTen, sizeof(s_szIconTen) - 1) == 0 && !JxUi_CoGiaoDienTaiDiem(nX, nY))
		{
			*pnX = nX;
			*pnY = nY;
			return 1;
		}
	}
	g_DebugLog("[ICON] cham icon nhung khong do duoc diem bam trung NPC %s", s_szIconTen);
	return 0;
}

'''


def va_can(s):
    nl = nl_cua(s)
    s = thay(s, ["static unsigned int\ts_uIconVeLuc = 0;\t// [ANDROID 11/09 ICON b] luc ve icon gan nhat (0 = chua ve)"],
             ["static unsigned int\ts_uIconVeLuc = 0;\t// [ANDROID 11/09 ICON b] luc ve icon gan nhat (0 = chua ve)",
              "static int\ts_nIconHudX = -1, s_nIconHudY = -1;\t// %s tam icon co dinh tren man hinh (-1 = mac dinh goc phai); UiToaDo khoa IconNpc" % DAU,
              "static int\ts_nIconGan = 220;\t// %s NPC cach nhan vat <= bao nhieu px (man hinh) thi hien icon. [Cham] IconNpcGan" % DAU,
              "static int\ts_nIconNpcCo = 0;\t// %s 1 = dang co NPC gan (icon dang hien that, khong phai chi de keo)" % DAU], "khai bao")
    s = thay(s, ['\ts_nIconRong  = GetPrivateProfileInt("Cham", "IconNpcRong", 44, szCfg);\t// [ANDROID 11/09 ICON b] [ANDROID 11/09 ICON d]'],
             ['\ts_nIconRong  = GetPrivateProfileInt("Cham", "IconNpcRong", 44, szCfg);\t// [ANDROID 11/09 ICON b] [ANDROID 11/09 ICON d]',
              '\ts_nIconGan   = GetPrivateProfileInt("Cham", "IconNpcGan", 220, szCfg);\t// %s' % DAU,
              '\tif (s_nIconGan < 40) s_nIconGan = 40;'], "DocCaiDat IconNpcGan")
    # thay ca vung tu 'void JxIconNpc_Ve()' toi truoc 'void JxCan_Ve()'
    a = s.find("void JxIconNpc_Ve()" + nl + "{")
    b = s.find("void JxCan_Ve()" + nl + "{")
    if a < 0 or b < 0 or b <= a:
        raise SystemExit("khong tim thay vung JxIconNpc_Ve..JxCan_Ve")
    moi = VUNG_MOI.replace("\n", nl)
    s = s[:a] + moi + s[b:]
    return s


def va_config(s):
    s = s.replace("IconNpcCao=100", "IconNpcGan=220")
    s = s.replace("; [ANDROID 11/09 ICON d] doi_thoai.spr (86x86, 2 khung) - chu chon thay nut \"Giao tiep\"",
                  "; [ANDROID 11/09 ICON e] icon nam CO DINH tren man hinh (mac dinh goc phai), chi hien khi NPC doi thoai gan nhat cach nhan vat <= IconNpcGan px;\n;   keo cho khac trong che do \"Sua giao dien\" (khoa IconNpc, UserData\\UiToaDo.ini). Cham icon = di toi NPC + mo thoai.".replace("\n", nl_cua(s)))
    s = s.replace(";   IconNpcRong = be rong ve (px, cao theo ti le anh); IconNpcCao = mep tren icon cao hon chan NPC bao nhieu px; IconNpc=0 de tat.",
                  ";   IconNpcRong = co ve (px); IconNpcGan = khoang cach hien icon (px man hinh); IconNpc=0 de tat.")
    if DAU not in s:
        raise SystemExit("config: khong doi duoc gi")
    return s


va("Sources/S3Client/Platform/JxCanDieuKhien.cpp", va_can)
va("android/du_lieu_ghi_de/config.ini", va_config)
p = "D:/jx1_android_data/config.ini"
if os.path.isfile(p):
    s = doc(p)
    if DAU not in s:
        ghi(p, va_config(s))
        print("da va: config song")
print("xong")
