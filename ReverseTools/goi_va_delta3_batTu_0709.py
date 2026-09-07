# -*- coding: ascii -*-
"""delta_patch3.py - [DELTA 07/09 c] (1) m_nProtectedTime chi tinh la 'doi' khi doi giua 0 va >0 (may chu tru moi tick
-> truoc day moi NormalSync cua NPC dang bat tu deu thanh goi day du 99 byte); (2) [NS-BO] in so trong 10 s thay vi
cong don. Chi may chu (CoreServer.dll). Byte-safe nhu delta_patch.py."""
import io, sys
KNPC_C = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KNpc.cpp"
edits = []
def E(old, new, count=1): edits.append((old, new, count))

E("\t\t\tsCham.Doing = 0; sCham.State = 0; sCham.m_CurrentLife = 0; sCham.m_CurrentMana = 0;\n",
  "\t\t\tsCham.Doing = 0; sCham.State = 0; sCham.m_CurrentLife = 0; sCham.m_CurrentMana = 0;\n"
  "\t\t\t// [DELTA 07/09 c] vong bat tu: may chu tru moi tick (KNpc.cpp:1627) nen chi coi la 'doi' khi bat/tat;\n"
  "\t\t\t// client khong dem nguoc, chi can biet dang bat tu hay khong (goi day du khi bat va khi het).\n"
  "\t\t\tsCham.m_nProtectedTime = (NpcSync.m_nProtectedTime > 0) ? 1 : 0;\n")

E("\t\tif (!bPhat) s_nNSBo++; else if (bGon) s_nNSGonDem++; else s_nNSDay++;\n"
  "\t\tAUTOLOG_EVERY(10000, \"[NS-BO] dong bo theo thay doi: bo=%d gon=%d day=%d (lam moi %d ms, day du %d ms, gon=%d, client cu=%d)\",\n"
  "\t\t\ts_nNSBo, s_nNSGonDem, s_nNSDay, s_nNSLamMoi, s_nNSLamMoiDay, s_nNSGon, NS_SoClientCu(dwLuc));\n",
  "\t\tif (!bPhat) s_nNSBo++; else if (bGon) s_nNSGonDem++; else s_nNSDay++;\n"
  "\t\t{\t// [DELTA 07/09 c] in so trong 10 giay (truoc: cong don tu luc boot, kho doc)\n"
  "\t\t\tstatic DWORD s_dwNSMoc = 0;\n"
  "\t\t\tif (s_dwNSMoc == 0)\n"
  "\t\t\t\ts_dwNSMoc = dwLuc;\n"
  "\t\t\telse if (dwLuc - s_dwNSMoc >= 10000)\n"
  "\t\t\t{\n"
  "\t\t\t\ts_dwNSMoc = dwLuc;\n"
  "\t\t\t\tAUTOLOG(\"[NS-BO] 10s dong bo theo thay doi: bo=%d gon=%d day=%d (lam moi %d ms, day du %d ms, gon=%d, client cu=%d)\",\n"
  "\t\t\t\t\ts_nNSBo, s_nNSGonDem, s_nNSDay, s_nNSLamMoi, s_nNSLamMoiDay, s_nNSGon, NS_SoClientCu(dwLuc));\n"
  "\t\t\t\ts_nNSBo = 0; s_nNSGonDem = 0; s_nNSDay = 0;\n"
  "\t\t\t}\n"
  "\t\t}\n")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
s = rd(KNPC_C); before = hb(s)
for old, new, cnt in edits:
    old = old.replace("\n", "\r\n"); new = new.replace("\n", "\r\n")
    n = s.count(old)
    if n != cnt: print("FAIL: anchor found %d (expect %d): %s" % (n, cnt, old[:90].encode("ascii", "replace").decode())); sys.exit(1)
    if any(ord(c) >= 0x80 for c in old + new): print("FAIL non-ASCII"); sys.exit(1)
    s = s.replace(old, new)
if hb(s) != before: print("FAIL high-byte changed"); sys.exit(1)
wr(KNPC_C, s); print("OK KNpc.cpp high-bytes", before, "ALL APPLIED")
