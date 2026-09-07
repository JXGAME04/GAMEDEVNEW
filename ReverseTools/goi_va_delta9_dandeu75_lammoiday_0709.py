# -*- coding: ascii -*-
"""delta_patch9.py - [DELTA 07/09 i] chi may chu (KNpc.cpp):
 (1) ky lam moi goi 75 DAN DEU: + (m_Index % 128) giay cho tung NPC -> 1.000 bot khong con lam moi cung luc
     (do 15:00: hai cua so 10 s co 778 va 1.024 goi 75 = 177 + 234 KB, chiem 38 % cua so dinh 62 KB/s)
 (2) ky lam moi goi 77 DAY DU (DongBoLamMoiDay): mac dinh 10 s -> 60 s, tran 60 s -> 600 s. Goi day du chi con la luoi
     an toan: moi truong cham da nam trong bam cham (doi -> phat day du ngay), NPC moi biet qua c2s_requestnpc -> SendSyncData.
     (do 15:00: 77 = 15,4 % byte trong tran, 410 goi/10 s ~ dung bang so NPC trong tam / 10 s)
 (3) chan doan: dem bit co nao cua m_btSomeFlag doi (XOR) va tach bot/nguoi, in trong [PS-BO] (co_khac_hoac_ngua=14.508
     trong 35 phut chua ro bit nao)
Byte-safe: latin-1, ASCII-only, CRLF, kiem so byte cao."""
import io, sys
P = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KNpc.cpp"
edits = []
def E(old, new, count=1): edits.append((old, new, count))

# (2) ky lam moi day du
E("static int   s_nNSLamMoi = -1, s_nNSLamMoiDay = 10000, s_nNSGon = 1;\n",
  "static int   s_nNSLamMoi = -1, s_nNSLamMoiDay = 60000, s_nNSGon = 1;\t// [DELTA 07/09 i] day du 10 s -> 60 s\n")
E("\ts_nNSLamMoiDay = (int)GetPrivateProfileIntA(\"Server\", \"DongBoLamMoiDay\", 10000, \".\\\\config.ini\");\n"
  "\tif (s_nNSLamMoiDay < 1000) s_nNSLamMoiDay = 1000;\n"
  "\tif (s_nNSLamMoiDay > 60000) s_nNSLamMoiDay = 60000;\n",
  "\t// [DELTA 07/09 i] 10 s -> 60 s: goi day du chi la luoi an toan (moi truong cham nam trong bam cham -> doi la phat day du\n"
  "\t// ngay; NPC moi thay -> client hoi c2s_requestnpc -> SendSyncData gui day du). Do 15:00: 77 = 15,4 % byte trong tran,\n"
  "\t// 410 goi/10 s = dung bang so NPC trong tam chia 10 s.\n"
  "\ts_nNSLamMoiDay = (int)GetPrivateProfileIntA(\"Server\", \"DongBoLamMoiDay\", 60000, \".\\\\config.ini\");\n"
  "\tif (s_nNSLamMoiDay < 1000) s_nNSLamMoiDay = 1000;\n"
  "\tif (s_nNSLamMoiDay > 600000) s_nNSLamMoiDay = 600000;\n")

# (3) chan doan bit co + bot/nguoi
E("static int   s_nPSDoiFight = 0, s_nPSDoiCoNgua = 0, s_nPSDoiKhac = 0;\n",
  "static int   s_nPSDoiFight = 0, s_nPSDoiCoNgua = 0, s_nPSDoiKhac = 0;\n"
  "// [DELTA 07/09 i] bit nao cua m_btSomeFlag doi (XOR voi lan truoc, chi dem khi goi phai phat lai) va la bot hay nguoi\n"
  "static BYTE  s_abyPSCo[MAX_NPC];\n"
  "static int   s_anPSBit[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };\n"
  "static int   s_nPSDoiBot = 0, s_nPSDoiNguoi = 0;\n")
E("\t\t\t\t\telse if (dwBam != s_adwPSBam[m_Index] && dwBamKhac == s_adwPSBamKhac[m_Index]) s_nPSDoiCoNgua++;\n"
  "\t\t\t\t\telse if (dwBam != s_adwPSBam[m_Index]) s_nPSDoiKhac++;\n"
  "\t\t\t\t}\n",
  "\t\t\t\t\telse if (dwBam != s_adwPSBam[m_Index] && dwBamKhac == s_adwPSBamKhac[m_Index]) s_nPSDoiCoNgua++;\n"
  "\t\t\t\t\telse if (dwBam != s_adwPSBam[m_Index]) s_nPSDoiKhac++;\n"
  "\t\t\t\t\tif (dwBam != s_adwPSBam[m_Index])\t// [DELTA 07/09 i] bit co nao doi, bot hay nguoi\n"
  "\t\t\t\t\t{\n"
  "\t\t\t\t\t\tconst BYTE byXor = (BYTE)(sPSBam.m_btSomeFlag ^ s_abyPSCo[m_Index]);\n"
  "\t\t\t\t\t\tfor (int b = 0; b < 8; b++)\n"
  "\t\t\t\t\t\t\tif (byXor & (1 << b))\n"
  "\t\t\t\t\t\t\t\ts_anPSBit[b]++;\n"
  "\t\t\t\t\t\tif (m_nPlayerIdx > 0) s_nPSDoiNguoi++; else s_nPSDoiBot++;\n"
  "\t\t\t\t\t}\n"
  "\t\t\t\t}\n"
  "\t\t\t\ts_abyPSCo[m_Index] = sPSBam.m_btSomeFlag;\n")
E("\t\tAUTOLOG_EVERY(10000, \"[PS-BO] goi ngoai hinh: gui=%d bo=%d (%d%% bo) lam moi moi %d giay | doi: chi_co_chien_dau=%d co_khac_hoac_ngua=%d khac=%d | nhom: toc_do=%d rank=%d chi_so=%d trang_bi=%d ten=%d nhieu=%d\",\n"
  "\t\t\ts_nPSGui, s_nPSBo, (s_nPSGui + s_nPSBo) > 0 ? (s_nPSBo * 100 / (s_nPSGui + s_nPSBo)) : 0, s_nPSLamMoi,\n"
  "\t\t\ts_nPSDoiFight, s_nPSDoiCoNgua, s_nPSDoiKhac,\n"
  "\t\t\ts_anPSNhom[0], s_anPSNhom[1], s_anPSNhom[2], s_anPSNhom[3], s_anPSNhom[4], s_nPSNhomNhieu);\n",
  "\t\tAUTOLOG_EVERY(10000, \"[PS-BO] goi ngoai hinh: gui=%d bo=%d (%d%% bo) lam moi moi %d giay | doi: chi_co_chien_dau=%d co_khac_hoac_ngua=%d khac=%d | nhom: toc_do=%d rank=%d chi_so=%d trang_bi=%d ten=%d nhieu=%d | bit: 01=%d 02=%d 04=%d 08=%d 10=%d 20=%d 40=%d 80=%d | bot=%d nguoi=%d\",\n"
  "\t\t\ts_nPSGui, s_nPSBo, (s_nPSGui + s_nPSBo) > 0 ? (s_nPSBo * 100 / (s_nPSGui + s_nPSBo)) : 0, s_nPSLamMoi,\n"
  "\t\t\ts_nPSDoiFight, s_nPSDoiCoNgua, s_nPSDoiKhac,\n"
  "\t\t\ts_anPSNhom[0], s_anPSNhom[1], s_anPSNhom[2], s_anPSNhom[3], s_anPSNhom[4], s_nPSNhomNhieu,\n"
  "\t\t\ts_anPSBit[0], s_anPSBit[1], s_anPSBit[2], s_anPSBit[3], s_anPSBit[4], s_anPSBit[5], s_anPSBit[6], s_anPSBit[7], s_nPSDoiBot, s_nPSDoiNguoi);\n")

# (1) dan deu ky lam moi 75
E("\t\t\tif (s_adwPSBam[m_Index] == dwBam &&\n"
  "\t\t\t\t(dwLuc - s_adwPSLuc[m_Index]) < (DWORD)(s_nPSLamMoi * 1000))\t// GetTickCount tinh bang mili giay\n",
  "\t\t\t// [DELTA 07/09 i] DAN DEU ky lam moi: + (m_Index % 128) giay cho tung NPC. Bot vao tam cung luc thi lam moi\n"
  "\t\t\t// cung luc (do 15:00: mot cua so 10 s co 1.024 goi 75 = 234 KB, 38 % cua so dinh 62 KB/s); dan ra 128 s.\n"
  "\t\t\tif (s_adwPSBam[m_Index] == dwBam &&\n"
  "\t\t\t\t(dwLuc - s_adwPSLuc[m_Index]) < (DWORD)(s_nPSLamMoi * 1000 + (m_Index % 128) * 1000))\t// GetTickCount tinh bang mili giay\n")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
s = rd(P); before = hb(s)
for old, new, cnt in edits:
    old = old.replace("\n", "\r\n"); new = new.replace("\n", "\r\n")
    if any(ord(c) >= 0x80 for c in old + new): print("FAIL non-ASCII"); sys.exit(1)
    n = s.count(old)
    if n != cnt: print("FAIL: found %d (expect %d): %s" % (n, cnt, old[:110].encode("ascii", "replace").decode())); sys.exit(1)
    s = s.replace(old, new)
if hb(s) != before: print("FAIL high-byte changed"); sys.exit(1)
wr(P, s); print("OK KNpc.cpp high-bytes", before, "ALL APPLIED")
