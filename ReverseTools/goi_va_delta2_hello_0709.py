# -*- coding: ascii -*-
"""delta_patch2.py - [DELTA 07/09] bat tay 'hello': client moi bao da hieu goi vi tri gon; may chu chi phat goi gon
khi MOI client dang noi deu da bao -> client cu (chua swap) khong bao gio nhan ma 221. Byte-safe nhu delta_patch.py."""
import io, sys

ROOT = r"D:\GAMEDEVNEW_wt_delta"
S = ROOT + r"\Sources"
KDEF_H = ROOT + r"\Headers\KProtocolDef.h"
KPRO_H = S + r"\Core\Src\KProtocol.h"
KPRO_C = S + r"\Core\Src\KProtocol.cpp"
KPP_H  = S + r"\Core\Src\KProtocolProcess.h"
KPP_C  = S + r"\Core\Src\KProtocolProcess.cpp"
KPS_C  = S + r"\Core\Src\KPlayerSet.cpp"
KNPC_C = S + r"\Core\Src\KNpc.cpp"

edits = []; inserts = []
def E(f, old, new, count=1): edits.append((f, old, new, count))
def I(f, anchor, text, where): inserts.append((f, anchor, text, where))

I(KDEF_H, "\tc2s_scriptdata,\t\t// [MAIL 03/09] goi ScriptProtocol (ObjBuffer) client -> may chu",
  "\tc2s_deltahello = 175,\t// [DELTA 07/09] client bao 'hieu goi vi tri gon' (C2S_DELTA_HELLO). Ma 175 = o dau doan trong\n"
  "\t\t\t\t\t\t\t// 175-249, khong doi ma cu; g_nProtocolSize (KProtocol.cpp, phan may chu) phai co o 110 cho ma nay.\n", "after")

I(KPRO_H, "} NPC_POS_SYNC;",
  "\n// [DELTA 07/09] client -> may chu ngay sau c2s_syncend: 'toi hieu goi s2c_syncnpcpos'. May chu chi phat goi gon khi\n"
  "// MOI client dang noi deu da bao (KProtocolProcess::DeltaHello), nen client cu khong bao gio nhan ma 221.\n"
  "typedef struct\n"
  "{\n"
  "\tBYTE\tProtocolType;\t// c2s_deltahello\n"
  "\tBYTE\tbyPhienBan;\t\t// 1\n"
  "} C2S_DELTA_HELLO;\n", "after")

I(KPRO_C, "\t-1,\t\t\t\t\t\t\t// c2s_scriptdata [MAIL 03/09]",
  "\tsizeof(C2S_DELTA_HELLO),\t// c2s_deltahello = 175 [DELTA 07/09] - o 110 = ma 175 (ngay sau 174), KHONG chen gi giua\n", "after")

I(KPP_H, "\tvoid\tNpcRequestCommand(int nIndex, BYTE* pMsg);",
  "\tvoid\tDeltaHello(int nIndex, BYTE* pMsg);\t// [DELTA 07/09] client bao hieu goi vi tri gon\n", "after")

I(KPP_C, "\tProcessFunc[c2s_requestnpc] = &KProtocolProcess::NpcRequestCommand;",
  "\tProcessFunc[c2s_deltahello] = &KProtocolProcess::DeltaHello;\t// [DELTA 07/09]\n", "after")

I(KPP_C, "void KProtocolProcess::NpcRequestCommand(int nIndex, BYTE* pProtocol)",
  "// [DELTA 07/09] client moi bao da hieu goi vi tri gon s2c_syncnpcpos. KNpc::NormalSync chi phat goi gon khi MOI client\n"
  "// dang noi deu da bao (NS_SoClientCu() == 0), nen client cu (chua swap CoreClient.dll) khong bao gio nhan ma 221.\n"
  "// KPlayerSet::Add xoa co khi cap khe moi (khe dung lai cho ket noi khac).\n"
  "BYTE g_abyDeltaHello[MAX_PLAYER];\n"
  "void KProtocolProcess::DeltaHello(int nIndex, BYTE* pMsg)\n"
  "{\n"
  "\tif (nIndex > 0 && nIndex < MAX_PLAYER)\n"
  "\t\tg_abyDeltaHello[nIndex] = 1;\n"
  "}\n\n", "before")

E(KPP_C, "\tif (g_pClient)\n\t\tg_pClient->SendPackToServer(&SyncEnd, sizeof(BYTE));\n",
  "\tif (g_pClient)\n\t\tg_pClient->SendPackToServer(&SyncEnd, sizeof(BYTE));\n"
  "\t// [DELTA 07/09] bao may chu: client nay hieu goi vi tri gon s2c_syncnpcpos (may chu cu khong co handler -> bo qua)\n"
  "\t{\n"
  "\t\tC2S_DELTA_HELLO sHello;\n"
  "\t\tsHello.ProtocolType = (BYTE)c2s_deltahello;\n"
  "\t\tsHello.byPhienBan = 1;\n"
  "\t\tif (g_pClient)\n"
  "\t\t\tg_pClient->SendPackToServer((BYTE*)&sHello, sizeof(sHello));\n"
  "\t}\n")

E(KPS_C, "\t\tPlayer[i].m_nNetConnectIdx = -1;\n\t\tPlayer[i].m_dwLoginTime = g_SubWorldSet.GetGameTime();\n",
  "\t\tPlayer[i].m_nNetConnectIdx = -1;\n"
  "\t\t{\t// [DELTA 07/09] khe moi cap: chua biet client co hieu goi vi tri gon khong\n"
  "\t\t\textern BYTE g_abyDeltaHello[MAX_PLAYER];\n"
  "\t\t\tg_abyDeltaHello[i] = 0;\n"
  "\t\t}\n"
  "\t\tPlayer[i].m_dwLoginTime = g_SubWorldSet.GetGameTime();\n")

# KNpc.cpp: NS_SoClientCu + dieu kien goi gon + log
E(KNPC_C, "// co nguoi vua vao vung nay hoac 8 vung ke trong ky lam moi -> phai phat de ho biet NPC (client chi biet NPC la qua goi 77)\n",
  "// so client that dang noi ma CHUA bao hieu goi gon (client cu) - dem lai moi giay; > 0 thi khong phat goi gon cho ai\n"
  "extern BYTE g_abyDeltaHello[MAX_PLAYER];\n"
  "static int NS_SoClientCu(DWORD dwLuc)\n"
  "{\n"
  "\tstatic DWORD s_dwMoc = 0;\n"
  "\tstatic int   s_nCu = 0;\n"
  "\tif (s_dwMoc == 0 || (dwLuc - s_dwMoc) >= 1000)\n"
  "\t{\n"
  "\t\ts_dwMoc = dwLuc;\n"
  "\t\tint n = 0;\n"
  "\t\tfor (int i = 1; i < MAX_PLAYER; i++)\n"
  "\t\t\tif (Player[i].m_nNetConnectIdx >= 0 && !g_abyDeltaHello[i])\n"
  "\t\t\t\tn++;\n"
  "\t\ts_nCu = n;\n"
  "\t}\n"
  "\treturn s_nCu;\n"
  "}\n"
  "// co nguoi vua vao vung nay hoac 8 vung ke trong ky lam moi -> phai phat de ho biet NPC (client chi biet NPC la qua goi 77)\n")
E(KNPC_C, "\t\t\telse if (s_nNSGon && dwBamCham == s_adwNSBamCham[m_Index] && (dwLuc - s_adwNSLucDay[m_Index]) < (DWORD)s_nNSLamMoiDay)\n",
  "\t\t\telse if (s_nNSGon && NS_SoClientCu(dwLuc) == 0 && dwBamCham == s_adwNSBamCham[m_Index] && (dwLuc - s_adwNSLucDay[m_Index]) < (DWORD)s_nNSLamMoiDay)\n")
E(KNPC_C, "\t\tAUTOLOG_EVERY(10000, \"[NS-BO] dong bo theo thay doi: bo=%d gon=%d day=%d (lam moi %d ms, day du %d ms, gon=%d)\",\n"
  "\t\t\ts_nNSBo, s_nNSGonDem, s_nNSDay, s_nNSLamMoi, s_nNSLamMoiDay, s_nNSGon);\n",
  "\t\tAUTOLOG_EVERY(10000, \"[NS-BO] dong bo theo thay doi: bo=%d gon=%d day=%d (lam moi %d ms, day du %d ms, gon=%d, client cu=%d)\",\n"
  "\t\t\ts_nNSBo, s_nNSGonDem, s_nNSDay, s_nNSLamMoi, s_nNSLamMoiDay, s_nNSGon, NS_SoClientCu(dwLuc));\n")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
files = {}
for f, old, new, cnt in edits: files.setdefault(f, rd(f))
for f, anc, txt, where in inserts: files.setdefault(f, rd(f))
before = {f: hb(s) for f, s in files.items()}
edits = [(f, old.replace("\n", "\r\n"), new.replace("\n", "\r\n"), cnt) for f, old, new, cnt in edits]
inserts = [(f, anc, txt.replace("\n", "\r\n"), where) for f, anc, txt, where in inserts]
for f, old, new, cnt in edits:
    s = files[f]; n = s.count(old)
    if n != cnt:
        print("FAIL edit %s: found %d (expect %d): %s" % (f, n, cnt, old[:100].encode("ascii", "replace").decode())); sys.exit(1)
    if any(ord(c) >= 0x80 for c in old + new): print("FAIL non-ASCII", f); sys.exit(1)
    files[f] = s.replace(old, new)
for f, anc, txt, where in inserts:
    s = files[f]; lines = s.split("\n")
    hits = [i for i, l in enumerate(lines) if anc in l]
    if len(hits) != 1: print("FAIL insert %s: found %d: %s" % (f, len(hits), anc)); sys.exit(1)
    if any(ord(c) >= 0x80 for c in txt): print("FAIL non-ASCII insert", f); sys.exit(1)
    i = hits[0]
    lines[i] = (lines[i] + "\n" + txt.rstrip("\n")) if where == "after" else (txt.rstrip("\n") + "\n" + lines[i])
    files[f] = "\n".join(lines)
for f, s in files.items():
    if hb(s) != before[f]: print("FAIL high-byte changed", f); sys.exit(1)
for f, s in files.items():
    wr(f, s); print("OK", f.split("\\")[-1], "high-bytes", before[f])
print("ALL APPLIED")
