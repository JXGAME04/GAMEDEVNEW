# -*- coding: ascii -*-
"""delta_patch11.py - [DELTA 07/09 k] chi may chu, KHONG doi hanh vi (chan doan + sua log tran so):
 (1) Goi 77 day du con 16,4 % byte trong khi ky lam moi day du da len 60 s va day/(gon+day) chi 1,9 %
     => phan lon goi day du la do BAM CHAM doi. Dem xem doi nam o NHOM truong nao, y het cach da lam
     cho goi 75 o va e: bam lai voi tung nhom xoa trang, nhom nao KHONG doi thi doi nam trong nhom do.
     Nhom: 0 toc do (Walk/Run/A/CSpeed) - 1 trang thai ky nang (StateInfo) - 2 chi so toi da (LifeMax/ManaMax)
           - 3 phe/loai (Camp/Series/NpcEnchant/MissionGroup) - 4 vong bat tu (m_nProtectedTime).
     Moc so sanh duoc luu DUNG LUC luu s_adwNSBamCham (lan phat DAY DU truoc) nen khong lech moc nhu
     bo dem bit cua va i.
 (2) Sua dong [PS-BO]: 'bo * 100' tran so nguyen 32 bit khi bo > 21 trieu (in ra -24%) -> tinh bang __int64.
Byte-safe: latin-1, ASCII-only, CRLF, kiem so byte cao."""
import io, sys
P = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KNpc.cpp"
edits = []
def E(old, new, count=1): edits.append((old, new, count))

E("static DWORD s_adwNSNgua[MAX_NPC];\t// [DELTA 07/09 f] bam ngua+toc do lan phat truoc (de gui them goi gon sau goi day du)\n",
  "static DWORD s_adwNSNgua[MAX_NPC];\t// [DELTA 07/09 f] bam ngua+toc do lan phat truoc (de gui them goi gon sau goi day du)\n"
  "// [DELTA 07/09 k] bam cham voi tung NHOM truong xoa trang, luu cung luc voi s_adwNSBamCham (lan phat DAY DU truoc):\n"
  "// 0 toc do, 1 trang thai ky nang, 2 chi so toi da, 3 phe/loai, 4 vong bat tu. Nhom nao khong doi = thu pham.\n"
  "#define NS_SO_NHOM\t5\n"
  "static DWORD s_adwNSNhom[NS_SO_NHOM][MAX_NPC];\n"
  "static int   s_anNSNhom[NS_SO_NHOM] = { 0, 0, 0, 0, 0 };\n"
  "static int   s_nNSNhomNhieu = 0;\n"
  "// bam cham voi nhom k xoa trang (k < 0: khong xoa gi)\n"
  "static DWORD NS_BamNhom(const NPC_NORMAL_SYNC* pCham, int k)\n"
  "{\n"
  "\tNPC_NORMAL_SYNC sN = *pCham;\n"
  "\tswitch (k)\n"
  "\t{\n"
  "\tcase 0: sN.m_WalkSpeed = 0; sN.m_RunSpeed = 0; sN.m_ASpeed = 0; sN.m_CSpeed = 0; break;\n"
  "\tcase 1: memset(sN.StateInfo, 0, sizeof(sN.StateInfo)); break;\n"
  "\tcase 2: sN.m_CurrentLifeMax = 0; sN.m_LifeMax = 0; sN.m_CurrentManaMax = 0; sN.m_ManaMax = 0; break;\n"
  "\tcase 3: sN.Camp = 0; sN.m_bySeries = 0; sN.NpcEnchant = 0; sN.MissionGroup = 0; break;\n"
  "\tdefault: sN.m_nProtectedTime = 0; break;\n"
  "\t}\n"
  "\treturn PS_Bam(&sN, (int)sizeof(sN));\n"
  "}\n")

E("\t\t\tconst DWORD dwBamCham = PS_Bam(&sCham, (int)sizeof(sCham));\n",
  "\t\t\tconst DWORD dwBamCham = PS_Bam(&sCham, (int)sizeof(sCham));\n"
  "\t\t\t// [DELTA 07/09 k] bam cham DOI thi doi nam o nhom nao (chi dem, khong doi quyet dinh phat)\n"
  "\t\t\tDWORD adwNSNhom[NS_SO_NHOM];\n"
  "\t\t\t{\n"
  "\t\t\t\tfor (int k = 0; k < NS_SO_NHOM; k++)\n"
  "\t\t\t\t\tadwNSNhom[k] = NS_BamNhom(&sCham, k);\n"
  "\t\t\t\tif (s_adwNSBamCham[m_Index] != 0 && dwBamCham != s_adwNSBamCham[m_Index] && s_adwNSNhom[0][m_Index] != 0)\n"
  "\t\t\t\t{\n"
  "\t\t\t\t\tint nTim = -1;\n"
  "\t\t\t\t\tfor (int k = 0; k < NS_SO_NHOM && nTim < 0; k++)\n"
  "\t\t\t\t\t\tif (adwNSNhom[k] == s_adwNSNhom[k][m_Index])\n"
  "\t\t\t\t\t\t\tnTim = k;\n"
  "\t\t\t\t\tif (nTim >= 0) s_anNSNhom[nTim]++; else s_nNSNhomNhieu++;\n"
  "\t\t\t\t}\n"
  "\t\t\t}\n")

E("\t\t\t\tif (!bGon)\n"
  "\t\t\t\t{\n"
  "\t\t\t\t\ts_adwNSBamCham[m_Index] = dwBamCham;\n"
  "\t\t\t\t\ts_adwNSLucDay[m_Index] = dwLuc;\n"
  "\t\t\t\t}\n",
  "\t\t\t\tif (!bGon)\n"
  "\t\t\t\t{\n"
  "\t\t\t\t\ts_adwNSBamCham[m_Index] = dwBamCham;\n"
  "\t\t\t\t\ts_adwNSLucDay[m_Index] = dwLuc;\n"
  "\t\t\t\t\tfor (int k = 0; k < NS_SO_NHOM; k++)\t// [DELTA 07/09 k] moc nhom luu CUNG LUC voi bam cham\n"
  "\t\t\t\t\t\ts_adwNSNhom[k][m_Index] = adwNSNhom[k];\n"
  "\t\t\t\t}\n")

E("\t\t\t\tAUTOLOG(\"[NS-BO] 10s dong bo theo thay doi: bo=%d gon=%d day=%d gon_them=%d (lam moi %d ms, day du %d ms, gon=%d, client cu=%d)\",\n"
  "\t\t\t\t\ts_nNSBo, s_nNSGonDem, s_nNSDay, s_nNSGonThem, s_nNSLamMoi, s_nNSLamMoiDay, s_nNSGon, NS_SoClientCu(dwLuc));\n",
  "\t\t\t\tAUTOLOG(\"[NS-BO] 10s dong bo theo thay doi: bo=%d gon=%d day=%d gon_them=%d (lam moi %d ms, day du %d ms, gon=%d, client cu=%d) | bam cham doi o nhom: toc_do=%d trang_thai=%d chi_so_max=%d phe_loai=%d bat_tu=%d nhieu=%d\",\n"
  "\t\t\t\t\ts_nNSBo, s_nNSGonDem, s_nNSDay, s_nNSGonThem, s_nNSLamMoi, s_nNSLamMoiDay, s_nNSGon, NS_SoClientCu(dwLuc),\n"
  "\t\t\t\t\ts_anNSNhom[0], s_anNSNhom[1], s_anNSNhom[2], s_anNSNhom[3], s_anNSNhom[4], s_nNSNhomNhieu);\n"
  "\t\t\t\tfor (int k = 0; k < NS_SO_NHOM; k++)\t// dem lai moi 10 giay cho de doc (y het cac so khac tren dong nay)\n"
  "\t\t\t\t\ts_anNSNhom[k] = 0;\n"
  "\t\t\t\ts_nNSNhomNhieu = 0;\n")

# sua tran so nguyen trong dong [PS-BO]
E("\t\t\ts_nPSGui, s_nPSBo, (s_nPSGui + s_nPSBo) > 0 ? (s_nPSBo * 100 / (s_nPSGui + s_nPSBo)) : 0, s_nPSLamMoi,\n",
  "\t\t\t// [DELTA 07/09 k] truoc day 's_nPSBo * 100' tran so nguyen 32 bit khi bo > 21 trieu (in ra '-24% bo')\n"
  "\t\t\ts_nPSGui, s_nPSBo, (s_nPSGui + s_nPSBo) > 0 ? (int)(((__int64)s_nPSBo * 100) / (__int64)(s_nPSGui + s_nPSBo)) : 0, s_nPSLamMoi,\n")

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
