# -*- coding: ascii -*-
"""delta_patch5.py - [DELTA 07/09 e] chan doan goi 75: doi nam o NHOM truong nao (toc do / rank / chi so / trang bi /
ten-chuoi / nhieu nhom). Chi may chu, chi dem va ghi [PS-BO]. Byte-safe nhu delta_patch.py."""
import io, sys
KNPC_C = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KNpc.cpp"
edits = []
def E(old, new, count=1): edits.append((old, new, count))

E("static int   s_nPSDoiFight = 0, s_nPSDoiCoNgua = 0, s_nPSDoiKhac = 0;\n",
  "static int   s_nPSDoiFight = 0, s_nPSDoiCoNgua = 0, s_nPSDoiKhac = 0;\n"
  "// [DELTA 07/09 e] doi nam o nhom truong nao: 0 toc do (Walk/Run/Attack/CastSpeed), 1 rank/danh hieu, 2 chi so\n"
  "// (RankInWorld/Repute/FuYuan/PK/ReBorn), 3 trang bi/ngua/anh, 4 ten-chuoi-co; bam voi nhom do xoa trang.\n"
  "static DWORD s_adwPSNhom[5][MAX_NPC];\n"
  "static int   s_anPSNhom[5] = { 0, 0, 0, 0, 0 };\n"
  "static int   s_nPSNhomNhieu = 0;\n")

E("\t\t\t\ts_adwPSBamDay[m_Index] = dwBamDay;\n"
  "\t\t\t\ts_adwPSBamKhac[m_Index] = dwBamKhac;\n"
  "\t\t\t}\n",
  "\t\t\t\ts_adwPSBamDay[m_Index] = dwBamDay;\n"
  "\t\t\t\ts_adwPSBamKhac[m_Index] = dwBamKhac;\n"
  "\t\t\t}\n"
  "\t\t\t{\t// [DELTA 07/09 e] doi o NHOM nao: nhom nao bam-xoa-trang KHONG doi thi doi nam trong nhom do\n"
  "\t\t\t\tDWORD adwNhom[5];\n"
  "\t\t\t\tfor (int k = 0; k < 5; k++)\n"
  "\t\t\t\t{\n"
  "\t\t\t\t\tPLAYER_NORMAL_SYNC sN = sPSBam;\n"
  "\t\t\t\t\tswitch (k)\n"
  "\t\t\t\t\t{\n"
  "\t\t\t\t\tcase 0: sN.WalkSpeed = 0; sN.RunSpeed = 0; sN.AttackSpeed = 0; sN.CastSpeed = 0; break;\n"
  "\t\t\t\t\tcase 1: sN.RankID = 0; sN.RankBattleID = 0; sN.PlayerTitle = 0; sN.HonorID = 0; sN.MantleLevel = 0; break;\n"
  "\t\t\t\t\tcase 2: sN.RankInWorld = 0; sN.Repute = 0; sN.FuYuan = 0; sN.PKValue = 0; sN.ReBorn = 0; break;\n"
  "\t\t\t\t\tcase 3: sN.HelmType = 0; sN.ArmorType = 0; sN.WeaponType = 0; sN.MantleType = 0; sN.MaskType = 0; sN.HorseType = 0; sN.ExItemID = 0; sN.ExBoxID = 0; sN.CUnlocked = 0; sN.ImagePlayer = 0; break;\n"
  "\t\t\t\t\tdefault: memset(sN.MateName, 0, sizeof(sN.MateName)); memset(sN.TongName, 0, sizeof(sN.TongName)); memset(sN.TongTitle, 0, sizeof(sN.TongTitle)); sN.TongFigure = 0; sN.TongRecruit = 0; memset(sN.GameTitle, 0, sizeof(sN.GameTitle)); memset(sN.bMeridianLevel, 0, sizeof(sN.bMeridianLevel)); sN.nFirstFaction = 0; sN.m_bBaiTan = 0; sN.m_btSomeFlag = 0; break;\n"
  "\t\t\t\t\t}\n"
  "\t\t\t\t\tadwNhom[k] = PS_Bam(&sN, (int)sizeof(PLAYER_NORMAL_SYNC));\n"
  "\t\t\t\t}\n"
  "\t\t\t\tif (s_adwPSBam[m_Index] != 0 && dwBam != s_adwPSBam[m_Index] && s_adwPSNhom[0][m_Index] != 0)\n"
  "\t\t\t\t{\n"
  "\t\t\t\t\tint nTim = -1;\n"
  "\t\t\t\t\tfor (int k = 0; k < 5 && nTim < 0; k++)\n"
  "\t\t\t\t\t\tif (adwNhom[k] == s_adwPSNhom[k][m_Index])\n"
  "\t\t\t\t\t\t\tnTim = k;\n"
  "\t\t\t\t\tif (nTim >= 0) s_anPSNhom[nTim]++; else s_nPSNhomNhieu++;\n"
  "\t\t\t\t}\n"
  "\t\t\t\tfor (int k = 0; k < 5; k++)\n"
  "\t\t\t\t\ts_adwPSNhom[k][m_Index] = adwNhom[k];\n"
  "\t\t\t}\n")

E("\t\tAUTOLOG_EVERY(10000, \"[PS-BO] goi ngoai hinh: gui=%d bo=%d (%d%% bo) lam moi moi %d giay | doi: chi_co_chien_dau=%d co_khac_hoac_ngua=%d khac=%d\",\n"
  "\t\t\ts_nPSGui, s_nPSBo, (s_nPSGui + s_nPSBo) > 0 ? (s_nPSBo * 100 / (s_nPSGui + s_nPSBo)) : 0, s_nPSLamMoi,\n"
  "\t\t\ts_nPSDoiFight, s_nPSDoiCoNgua, s_nPSDoiKhac);\n",
  "\t\tAUTOLOG_EVERY(10000, \"[PS-BO] goi ngoai hinh: gui=%d bo=%d (%d%% bo) lam moi moi %d giay | doi: chi_co_chien_dau=%d co_khac_hoac_ngua=%d khac=%d | nhom: toc_do=%d rank=%d chi_so=%d trang_bi=%d ten=%d nhieu=%d\",\n"
  "\t\t\ts_nPSGui, s_nPSBo, (s_nPSGui + s_nPSBo) > 0 ? (s_nPSBo * 100 / (s_nPSGui + s_nPSBo)) : 0, s_nPSLamMoi,\n"
  "\t\t\ts_nPSDoiFight, s_nPSDoiCoNgua, s_nPSDoiKhac,\n"
  "\t\t\ts_anPSNhom[0], s_anPSNhom[1], s_anPSNhom[2], s_anPSNhom[3], s_anPSNhom[4], s_nPSNhomNhieu);\n")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
s = rd(KNPC_C); before = hb(s)
for old, new, cnt in edits:
    old = old.replace("\n", "\r\n"); new = new.replace("\n", "\r\n")
    n = s.count(old)
    if n != cnt: print("FAIL: found %d (expect %d): %s" % (n, cnt, old[:90].encode("ascii", "replace").decode())); sys.exit(1)
    if any(ord(c) >= 0x80 for c in old + new): print("FAIL non-ASCII"); sys.exit(1)
    s = s.replace(old, new)
if hb(s) != before: print("FAIL high-byte changed"); sys.exit(1)
wr(KNPC_C, s); print("OK KNpc.cpp high-bytes", before, "ALL APPLIED")
