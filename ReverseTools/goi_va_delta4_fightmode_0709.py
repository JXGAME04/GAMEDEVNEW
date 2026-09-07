# -*- coding: ascii -*-
"""delta_patch4.py - [DELTA 07/09 d] (1) co chien dau nguoi choi di theo STATE_FIGHTMODE (0x40) trong goi 77/221 va
bo bit 0x02 khoi bam goi 75 (75 = 231 B phat lai moi lan bot doi che do chien dau: 15 % byte tran Tong Kim);
(2) [PS-BO] dem nguyen nhan doi (chi co chien dau / ngua+co / khac); (3) [PERF] ghi them pid.
Byte-safe nhu delta_patch.py (CRLF, ASCII, dem high-byte)."""
import io, sys
S = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src"
KNPC_H = S + r"\KNpc.h"; KNPC_C = S + r"\KNpc.cpp"; KPP_C = S + r"\KProtocolProcess.cpp"; KPERF_C = S + r"\KPerfTick.cpp"
edits = []
def E(f, old, new, count=1): edits.append((f, old, new, count))

E(KNPC_H, "#define\t\tSTATE_WALKRUN\t0x0020\n",
  "#define\t\tSTATE_WALKRUN\t0x0020\n"
  "#define\t\tSTATE_FIGHTMODE\t0x0040\t// [DELTA 07/09 d] co chien dau nguoi choi trong goi 77/221 (thay bit 0x02 cua goi 75)\n")

# server: dung goi
E(KNPC_C, "\tif (m_WalkRun.nTime > 0)\n\t\tNpcSync.State |= STATE_WALKRUN;\n",
  "\tif (m_WalkRun.nTime > 0)\n\t\tNpcSync.State |= STATE_WALKRUN;\n"
  "\tif (m_FightMode)\n\t\tNpcSync.State |= STATE_FIGHTMODE;\t// [DELTA 07/09 d] di theo goi vi tri, khong phat lai goi 75 234 byte\n")

# server: PS statics + dem nguyen nhan
E(KNPC_C, "static int   s_nPSBo = 0, s_nPSGui = 0;\n",
  "static int   s_nPSBo = 0, s_nPSGui = 0;\n"
  "// [DELTA 07/09 d] doi chieu nguyen nhan goi 75 phai phat lai: bam DAY DU (co ca bit 0x02) va bam KHONG co/ngua\n"
  "static DWORD s_adwPSBamDay[MAX_NPC];\n"
  "static DWORD s_adwPSBamKhac[MAX_NPC];\n"
  "static int   s_nPSDoiFight = 0, s_nPSDoiCoNgua = 0, s_nPSDoiKhac = 0;\n")

E(KNPC_C, "\t\t\tconst DWORD dwBam = PS_Bam(&PlayerSync, (int)sizeof(PLAYER_NORMAL_SYNC));\n",
  "\t\t\t// [DELTA 07/09 d] bit 0x02 (co chien dau) doi lien tuc trong Tong Kim (bot danh/nghi/hoi sinh) -> bo khoi bam;\n"
  "\t\t\t// co nay da di theo STATE_FIGHTMODE cua goi 77/221. Dem nguyen nhan doi de doi chieu trong [PS-BO].\n"
  "\t\t\tPLAYER_NORMAL_SYNC sPSBam = PlayerSync;\n"
  "\t\t\tsPSBam.m_btSomeFlag &= ~0x02;\n"
  "\t\t\tconst DWORD dwBam = PS_Bam(&sPSBam, (int)sizeof(PLAYER_NORMAL_SYNC));\n"
  "\t\t\t{\n"
  "\t\t\t\tconst DWORD dwBamDay = PS_Bam(&PlayerSync, (int)sizeof(PLAYER_NORMAL_SYNC));\n"
  "\t\t\t\tPLAYER_NORMAL_SYNC sPSKhac = sPSBam;\n"
  "\t\t\t\tsPSKhac.m_btSomeFlag = 0; sPSKhac.HorseType = 0;\n"
  "\t\t\t\tconst DWORD dwBamKhac = PS_Bam(&sPSKhac, (int)sizeof(PLAYER_NORMAL_SYNC));\n"
  "\t\t\t\tif (s_adwPSBamDay[m_Index] != 0)\n"
  "\t\t\t\t{\n"
  "\t\t\t\t\tif (dwBamDay != s_adwPSBamDay[m_Index] && dwBam == s_adwPSBam[m_Index]) s_nPSDoiFight++;\n"
  "\t\t\t\t\telse if (dwBam != s_adwPSBam[m_Index] && dwBamKhac == s_adwPSBamKhac[m_Index]) s_nPSDoiCoNgua++;\n"
  "\t\t\t\t\telse if (dwBam != s_adwPSBam[m_Index]) s_nPSDoiKhac++;\n"
  "\t\t\t\t}\n"
  "\t\t\t\ts_adwPSBamDay[m_Index] = dwBamDay;\n"
  "\t\t\t\ts_adwPSBamKhac[m_Index] = dwBamKhac;\n"
  "\t\t\t}\n")

E(KNPC_C, "\t\tAUTOLOG_EVERY(10000, \"[PS-BO] goi ngoai hinh: gui=%d bo=%d (%d%% bo) lam moi moi %d giay\",\n"
  "\t\t\ts_nPSGui, s_nPSBo, (s_nPSGui + s_nPSBo) > 0 ? (s_nPSBo * 100 / (s_nPSGui + s_nPSBo)) : 0, s_nPSLamMoi);\n",
  "\t\tAUTOLOG_EVERY(10000, \"[PS-BO] goi ngoai hinh: gui=%d bo=%d (%d%% bo) lam moi moi %d giay | doi: chi_co_chien_dau=%d co_khac_hoac_ngua=%d khac=%d\",\n"
  "\t\t\ts_nPSGui, s_nPSBo, (s_nPSGui + s_nPSBo) > 0 ? (s_nPSBo * 100 / (s_nPSGui + s_nPSBo)) : 0, s_nPSLamMoi,\n"
  "\t\t\ts_nPSDoiFight, s_nPSDoiCoNgua, s_nPSDoiKhac);\n")

# client: nhan co chien dau tu State
E(KPP_C, "\t\tNpc[nIdx].m_WalkRun.nTime\t\t= NpcSync->State & STATE_WALKRUN;\n",
  "\t\tNpc[nIdx].m_WalkRun.nTime\t\t= NpcSync->State & STATE_WALKRUN;\n"
  "\t\t// [DELTA 07/09 d] co chien dau cua nguoi choi khac di theo goi 77/221 (truoc chi qua goi 75 -> 75 phat lai lien tuc)\n"
  "\t\tif (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex && Npc[nIdx].m_Kind == kind_player)\n"
  "\t\t\tNpc[nIdx].m_FightMode = (NpcSync->State & STATE_FIGHTMODE) ? enumPKWar : enumPKNormal;\n")

# perf: pid
E(KPERF_C, "\t\t\"[PERF] %02d:%02d:%02d tick=%d tre=%d (%.1f%%) online=%d khoang=%.1fs\",\n"
  "\t\tst.wHour, st.wMinute, st.wSecond,\n"
  "\t\ts_nFrame, s_nLate,\n"
  "\t\ts_nFrame > 0 ? (s_nLate * 100.0 / s_nFrame) : 0.0,\n"
  "\t\tnOnlinePlayer, dWindow / 1000.0);\n",
  "\t\t\"[PERF] %02d:%02d:%02d tick=%d tre=%d (%.1f%%) online=%d khoang=%.1fs pid=%u\",\n"
  "\t\tst.wHour, st.wMinute, st.wSecond,\n"
  "\t\ts_nFrame, s_nLate,\n"
  "\t\ts_nFrame > 0 ? (s_nLate * 100.0 / s_nFrame) : 0.0,\n"
  "\t\tnOnlinePlayer, dWindow / 1000.0, (unsigned)GetCurrentProcessId());\t// [DELTA 07/09 d] pid: nhieu tien trinh cung ghi tep nay\n")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
files = {}
for f, old, new, cnt in edits: files.setdefault(f, rd(f))
before = {f: hb(s) for f, s in files.items()}
for f, old, new, cnt in edits:
    old = old.replace("\n", "\r\n"); new = new.replace("\n", "\r\n")
    s = files[f]; n = s.count(old)
    if n != cnt: print("FAIL %s: found %d (expect %d): %s" % (f, n, cnt, old[:90].encode("ascii", "replace").decode())); sys.exit(1)
    if any(ord(c) >= 0x80 for c in old + new): print("FAIL non-ASCII", f); sys.exit(1)
    files[f] = s.replace(old, new)
for f, s in files.items():
    if hb(s) != before[f]: print("FAIL high-byte changed", f); sys.exit(1)
for f, s in files.items(): wr(f, s); print("OK", f.split("\\")[-1], "high-bytes", before[f])
print("ALL APPLIED")
