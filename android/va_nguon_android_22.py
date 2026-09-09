# -*- coding: utf-8 -*-
# [ANDROID 09/09] Dot va 22: TOI GAN NPC THI HIEN ICON "NOI CHUYEN" TREN DAU.
#
# Chu: "toi gan npc nao phai hien icon de kich vao chon doi thoai hay khong".
#
# Cham vao NPC thi thoai da mo duoc san roi (da do), nen icon nay la de NGUOI CHOI BIET cho nao cham
# duoc - va vi icon nam ngay tren dau NPC nen cham vao icon cung la cham trung NPC.
#
# Khong them ma so GDI moi (them vao giua enum se day cac ma sau no lech di). Dung lai
# NPC_OI_TARGET_INFO voi nParam == 1 = "tra ve NPC DOI THOAI gan nhat" thay vi muc tieu dang chon.
#
# Client doi toa do the gioi -> man hinh bang iRepresentShell::CoordinateTransform roi ve icon cao
# hon dau NPC mot chut; NPC nao ra ngoai khung ve thi thoi (tuc la chi hien cho NPC dang nhin thay).
#
# Anh: \spr\obj\box\YellowPoint.spr cua bo VNKU (cham tron nho).
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


print("1. Core: nParam == 1 -> NPC doi thoai gan nhat")
va("Sources/Core/Src/CoreShell.cpp", "NPC DOI THOAI gan nhat",
"""	case NPC_OI_TARGET_INFO:
	{
		int idx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx;""",
"""	case NPC_OI_TARGET_INFO:
	{
#ifdef JX_ANDROID
		// [ANDROID 09/09 ICON] nParam == 1: tra ve NPC DOI THOAI gan nhat (khong phai muc tieu dang
		// chon) de client ve icon "noi chuyen" tren dau no. Dung lai ma so nay chu khong them ma moi -
		// them vao giua enum se day moi ma so phia sau lech di.
		if (nParam == 1)
		{
			KUiTargetDetailInfo* pGan = (KUiTargetDetailInfo*)uParam;
			int nMe = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			if (pGan == NULL || nMe <= 0 || nMe >= MAX_NPC)
				break;
			int nX0 = 0, nY0 = 0;
			Npc[nMe].GetDrawPos(&nX0, &nY0);
			int nChon = 0;
			__int64 nXaNhat = 0;
			int nDuyet = 0;
			while (nDuyet = NpcSet.GetNextIdx(nDuyet))
			{
				if (nDuyet == nMe || Npc[nDuyet].m_Kind != kind_dialoger || Npc[nDuyet].m_RegionIndex < 0)
					continue;
				int x = 0, y = 0;
				Npc[nDuyet].GetDrawPos(&x, &y);
				__int64 dx = (__int64)(x - nX0), dy = (__int64)(y - nY0);
				__int64 d = dx * dx + dy * dy;
				if (!nChon || d < nXaNhat)
				{
					nChon = nDuyet;
					nXaNhat = d;
				}
			}
			if (nChon)
			{
				strcpy_s(pGan->sTargetName, sizeof(pGan->sTargetName), Npc[nChon].Name);
				Npc[nChon].GetDrawPos(&pGan->nViTriVeX, &pGan->nViTriVeY);
				pGan->nDangKhoa = 0;
				nRet = 1;
			}
			break;
		}
#endif
		int idx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx;""", "Core tim NPC doi thoai gan nhat")

print("2. Client: ve icon tren dau NPC")
THAN = r'''
//---------------------------------------------------------------------------
// [ANDROID 09/09 ICON] ICON "NOI CHUYEN" TREN DAU NPC DOI THOAI GAN NHAT
//
// Chu: "toi gan npc nao phai hien icon de kich vao chon doi thoai hay khong". Cham vao NPC thi thoai
// da mo duoc san, nen icon nay de nguoi choi BIET cho nao cham duoc - va vi no nam ngay tren dau NPC
// nen cham vao icon cung la cham trung NPC.
//
// Core tra vi tri NPC theo toa do THE GIOI (NPC_OI_TARGET_INFO voi nParam = 1); doi sang toa do man
// hinh bang CoordinateTransform roi ve cao hon dau mot chut. NPC ra ngoai khung ve thi khong ve -
// tuc la chi hien cho NPC dang nhin thay.
//
// config.ini [Cham]: IconNpc=1 / IconNpcAnh / IconNpcCao (cao hon chan NPC bao nhieu diem anh)
//---------------------------------------------------------------------------
void JxIconNpc_Ve()
{
	DocCaiDat();
	if (!s_nIconBat || g_pCoreShell == NULL || g_pRepresentShell == NULL)
		return;
	KUiTargetDetailInfo gan;
	memset(&gan, 0, sizeof(gan));
	if (!g_pCoreShell->GetGameData(NPC_OI_TARGET_INFO, (KUPARAM)&gan, 1))
		return;
	if (gan.sTargetName[0] == 0)
		return;
	if (s_nIconCoAnh < 0)
	{
		s_nIconCoAnh = CoAnh(s_szIconAnh) ? 1 : 0;
		g_DebugLog("[ICON] anh icon NPC: %s -> co anh=%d", s_szIconAnh, s_nIconCoAnh);
	}
	if (!s_nIconCoAnh)
		return;
	int x = gan.nViTriVeX, y = gan.nViTriVeY;
	g_pRepresentShell->CoordinateTransform(x, y, 0);	// the gioi -> man hinh
	y -= s_nIconCao;
	if (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)
		return;		// NPC ra ngoai khung ve
	static KRUImage s_Icon;
	if (s_Icon.szImage[0] == 0)
	{
		memset(&s_Icon, 0, sizeof(s_Icon));
		s_Icon.nType = ISI_T_SPR;
		s_Icon.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		s_Icon.Color.Color_dw = 0xffffffff;
		s_Icon.nISPosition = IMAGE_IS_POSITION_INIT;
		s_Icon.nFrame = 0;
		strncpy(s_Icon.szImage, s_szIconAnh, sizeof(s_Icon.szImage) - 1);
	}
	KRPosition2 oOffI = { 0, 0 }, oCoI = { 0, 0 };
	int nLui = 0;
	if (g_pRepresentShell->GetImageFrameParam(s_Icon.szImage, 0, &oOffI, &oCoI, s_Icon.nType) && oCoI.nX > 0)
		nLui = oCoI.nX / 2;
	s_Icon.oPosition.nX = x - nLui;
	s_Icon.oPosition.nY = y;
	g_pRepresentShell->DrawPrimitives(1, &s_Icon, RU_T_IMAGE, true);	// true = toa do MAN HINH
}

'''
s = doc("Sources/S3Client/Platform/JxCanDieuKhien.cpp")
crlf = ("\r\n" in s)
if "JxIconNpc_Ve" in s:
    print("  bo qua (da co): than ve icon NPC")
else:
    neo = nl("void JxCan_Ve()\n{", crlf)
    if s.count(neo) != 1:
        print("  !! khong tim thay JxCan_Ve")
        sys.exit(1)
    i = s.find(neo)
    ghi("Sources/S3Client/Platform/JxCanDieuKhien.cpp", s[:i] + nl(THAN, crlf) + s[i:])
    print("  va xong: than ve icon NPC")

va("Sources/S3Client/Platform/JxCanDieuKhien.cpp", "s_nIconBat = 1;",
"""static char	s_szVongAnhDich[128] = "\\\\spr\\\\npcres\\\\focused_enemy_circle.spr";""",
"""static char	s_szVongAnhDich[128] = "\\\\spr\\\\npcres\\\\focused_enemy_circle.spr";
static int	s_nIconBat = 1;
static int	s_nIconCoAnh = -1;
static int	s_nIconCao = 62;	// icon cao hon chan NPC bao nhieu diem anh
static char	s_szIconAnh[128] = "\\\\spr\\\\Ui3\\\\UiGui\\\\NpcTalk.spr";""", "khai bao bien icon NPC")

va("Sources/S3Client/Platform/JxCanDieuKhien.cpp", "s_nIconBat   = GetPrivateProfileInt",
"""	GetPrivateProfileString("Cham", "VongChonAnhDich", s_szVongAnhDich, s_szVongAnhDich, sizeof(s_szVongAnhDich), szCfg);""",
"""	GetPrivateProfileString("Cham", "VongChonAnhDich", s_szVongAnhDich, s_szVongAnhDich, sizeof(s_szVongAnhDich), szCfg);
	s_nIconBat   = GetPrivateProfileInt("Cham", "IconNpc", 1, szCfg);
	s_nIconCao   = GetPrivateProfileInt("Cham", "IconNpcCao", 62, szCfg);
	GetPrivateProfileString("Cham", "IconNpcAnh", s_szIconAnh, s_szIconAnh, sizeof(s_szIconAnh), szCfg);""", "doc cai dat icon NPC")

va("Sources/S3Client/Platform/JxCanDieuKhien.h", "JxIconNpc_Ve",
"""// Goi cuoi moi khung ve: ve vong chon duoi chan muc tieu dang chon.
void JxVongChon_Ve();""",
"""// Goi cuoi moi khung ve: ve vong chon duoi chan muc tieu dang chon.
void JxVongChon_Ve();
// Goi cuoi moi khung ve: ve icon "noi chuyen" tren dau NPC doi thoai gan nhat.
void JxIconNpc_Ve();""", "khai bao JxIconNpc_Ve")

print("3. UiShell.cpp: goi ve icon NPC")
va("Sources/S3Client/Ui/UiShell.cpp", "JxIconNpc_Ve();",
"""	JxVongChon_Ve();	// [ANDROID 09/09 VONG] vong chon duoi chan muc tieu (toa do the gioi)""",
"""	JxVongChon_Ve();	// [ANDROID 09/09 VONG] vong chon duoi chan muc tieu (toa do the gioi)
	JxIconNpc_Ve();	// [ANDROID 09/09 ICON] icon "noi chuyen" tren dau NPC doi thoai gan nhat""", "UiShell goi ve icon NPC")

print("")
print("XONG dot va 22.")
