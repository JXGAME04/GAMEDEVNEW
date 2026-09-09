# -*- coding: utf-8 -*-
"""goi_va_tenmau1_do_mau_ten_theo_khoangcach_0809.py - [TENMAU 08/09 a] DO TRUOC.
Chu xac nhan: "ai dung GAN thi mau ten binh thuong, ai dung XA thi CHU TEN va DANH HIEU bi am do" (than nguoi van dung mau).
Duong sinh mau chu duy nhat: KNpc::PaintInfo tinh dwColor tu m_CurrentCamp roi goi OutputText. Bang mau:
  camp_begin 0 = trang, justice 1 = (255,168,94) cam, evil 2 = (255,146,255) hong, balance 3 = (85,255,145) luc,
  free 4 = (255,0,0) DO DAC, animal 5 = trang, event 6 = hong, con lai (default) = 0xce14c4 tim do.
=> Neu NPC o xa co m_CurrentCamp roi vao 4 hoac ra ngoai 0..6 thi ten se DO. Ban nay dem xem co dung khong:
moi 10 giay in mot dong [TENMAU] vao jx_paint.log, chia GAN (<400 mps) / XA (>=400 mps) kem phan bo m_CurrentCamp
tung nhom va so lan roi ra ngoai pham vi. Chi bat khi [Client] PaintLog=1. Khong doi mot dong hanh vi ve nao."""
import io, re, sys

P = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KNpc.cpp"
NL = "\r\n"
s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for ch in s if ord(ch) >= 0x80)


def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n:
        print("FAIL neo %r: thay %d, can %d" % (old[:80], c, n))
        sys.exit(1)
    return s.replace(old, new)


if "g_uTenMauGan" in s:
    print("da va roi")
    sys.exit(0)

khoi = NL.join([
"// [TENMAU 08/09 a] Do mau ten theo khoang cach: chu bao nguoi choi O XA bi am do phan CHU (than nguoi van dung mau).",
"// Mau ten sinh tu m_CurrentCamp; camp_free (4) = do dac, ra ngoai 0..6 = tim do. Dem xem NPC xa co roi vao do khong.",
"extern int g_nCorePaintLog;\t// CoreShell.cpp: S3Client dat qua GOI_PROCFRAME_BREATHE khi [Client] PaintLog=1",
"static unsigned g_uTenMauGan[8] = {0,0,0,0,0,0,0,0};\t// 0..6 = camp, 7 = ngoai pham vi",
"static unsigned g_uTenMauXa[8]  = {0,0,0,0,0,0,0,0};",
"static void TenMauGhi(int nCamp, int nKhoang)",
"{",
"\tint i = (nCamp >= 0 && nCamp <= 6) ? nCamp : 7;",
"\tif (nKhoang < 400) g_uTenMauGan[i]++; else g_uTenMauXa[i]++;",
"}",
"static void TenMauInDong()",
"{",
"\tstatic DWORD s_dwLan = 0;",
"\tconst DWORD dwNow = GetTickCount();",
"\tif (s_dwLan == 0) { s_dwLan = dwNow; return; }",
"\tif (dwNow - s_dwLan < 10000) return;",
"\ts_dwLan = dwNow;",
"\tFILE* pLog = fopen(\"jx_paint.log\", \"a\");",
"\tif (pLog)",
"\t{",
"\t\tfprintf(pLog, \"[TENMAU] t=%u | GAN trang %u cam %u hong %u luc %u DO %u thu %u sukien %u NGOAI %u | XA trang %u cam %u hong %u luc %u DO %u thu %u sukien %u NGOAI %u\\n\",",
"\t\t\tdwNow,",
"\t\t\tg_uTenMauGan[0], g_uTenMauGan[1], g_uTenMauGan[2], g_uTenMauGan[3], g_uTenMauGan[4], g_uTenMauGan[5], g_uTenMauGan[6], g_uTenMauGan[7],",
"\t\t\tg_uTenMauXa[0], g_uTenMauXa[1], g_uTenMauXa[2], g_uTenMauXa[3], g_uTenMauXa[4], g_uTenMauXa[5], g_uTenMauXa[6], g_uTenMauXa[7]);",
"\t\tfclose(pLog);",
"\t}",
"\tmemset(g_uTenMauGan, 0, sizeof(g_uTenMauGan));",
"\tmemset(g_uTenMauXa, 0, sizeof(g_uTenMauXa));",
"}",
"int KNpc::PaintInfo(int nHeightOffset, bool bSelect, int nFontSize, DWORD dwBorderColor)",
"{",
""])
s = rep(s, "int KNpc::PaintInfo(int nHeightOffset, bool bSelect, int nFontSize, DWORD dwBorderColor)" + NL + "{" + NL, khoi)

do = NL.join([
"\t\tif (g_nCorePaintLog > 0)",
"\t\t{\t// [TENMAU 08/09 a] dem camp theo khoang cach toi nhan vat cua minh",
"\t\t\tint nMeX = 0, nMeY = 0;",
"\t\t\tNpc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetDrawPos(&nMeX, &nMeY);",
"\t\t\tconst int ddx = nMpsX - nMeX, ddy = (nMpsY - nMeY) * 2;\t// truc Y nen 2 lan",
"\t\t\tconst int nKc = (int)sqrt((double)ddx * ddx + (double)ddy * ddy);",
"\t\t\tTenMauGhi((int)m_CurrentCamp, nKc);",
"\t\t\tTenMauInDong();",
"\t\t}",
""])
neo = "\t\tchar\tszString[128];" + NL + "\t\tif (SubWorld[Npc[CLIENT_PLAYER_INDEX].m_SubWorldIndex].m_SubWorldID == 209 && m_nPlayerIdx != CLIENT_PLAYER_INDEX)" + NL
s = rep(s, neo, do + neo)

if sum(1 for ch in s if ord(ch) >= 0x80) != h0:
    print("FAIL byte cao")
    sys.exit(1)
if re.search(r"[^\r]\n", s):
    print("FAIL LF")
    sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KNpc.cpp")
print("XONG TENMAU a")
