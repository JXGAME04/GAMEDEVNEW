# -*- coding: utf-8 -*-
"""goi_va_husk2_tu_lui_0809.py - [HUSK 08/09 b] Client chi ngung tu mo phong (NPC khac) SAU KHI da nhan it nhat mot goi 224 tu may chu
dang noi (g_nHUSK_daNhan). Ly do: neu chu swap client truoc ma may chu chua restart (hoac [Server] HieuUngSuKien=0) thi khong co goi 224
-> hieu ung NPC khac se MAT SACH. Nay: chua thay 224 = chay nhu cu; dat lai 0 moi lan gui hello (dang nhap / doi may chu)."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src"
def load(name):
    p = ROOT + "\\" + name
    return p, io.open(p, "r", encoding="latin-1", newline="").read()
def save(p, s, h0):
    if sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao", p); sys.exit(1)
    if re.search(r"[^\r]\n", s): print("FAIL LF", p); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:80], c, n)); sys.exit(1)
    return s.replace(old, new)
def hi(s): return sum(1 for ch in s if ord(ch) >= 0x80)

p, s = load("KNpc.cpp"); h0 = hi(s)
if "g_nHUSK_daNhan" not in s:
    s = rep(s, "int g_nFX_sv_husk = 0, g_nFX_sv_husk_cu = 0;\t// [HUSK 08/09] may chu: goi 224 da phat / lan bo qua vi con client cu (hello < 4)\r\n",
        "int g_nFX_sv_husk = 0, g_nFX_sv_husk_cu = 0;\t// [HUSK 08/09] may chu: goi 224 da phat / lan bo qua vi con client cu (hello < 4)\r\n"
        "int g_nHUSK_daNhan = 0;\t// [HUSK 08/09 b] client: da nhan >= 1 goi 224 tu may chu dang noi -> moi ngung tu mo phong; dat lai 0 khi gui hello\r\n")
    s = rep(s, "static BOOL HUSK_BoCastCuc(KNpc* pNpc)\t// NPC khac + che do bat -> khong Cast cuc bo, cho goi 224\r\n{\r\n\treturn HUSK_ClientBat() && !S13_IsRealPlayer(pNpc);\r\n}\r\n",
        "static BOOL HUSK_BoCastCuc(KNpc* pNpc)\t// NPC khac + che do bat + may chu DA gui 224 -> khong Cast cuc bo, cho goi 224\r\n{\r\n"
        "\t// [HUSK 08/09 b] chua nhan goi 224 nao (may chu cu / [Server] HieuUngSuKien=0 / chua restart) -> tu mo phong nhu cu, khong mat hieu ung\r\n"
        "\treturn HUSK_ClientBat() && g_nHUSK_daNhan && !S13_IsRealPlayer(pNpc);\r\n}\r\n")
    s = rep(s, "// Cong lui: client [Client] HieuUngSuKien=0 (client bao hello 3 -> may chu khong phat 224 cho AI); may chu [Server] HieuUngSuKien=0\r\n// (doc lai moi 10 s; khi tat PHAI tat ca client, neu khong hieu ung NPC khac khong hien).\r\n",
        "// Cong lui: client [Client] HieuUngSuKien=0 (client bao hello 3 -> may chu khong phat 224 cho AI); may chu [Server] HieuUngSuKien=0\r\n// (doc lai moi 10 s). [b] Client chi ngung tu mo phong sau khi DA NHAN goi 224 dau tien (g_nHUSK_daNhan) -> may chu cu / tat co / chua\r\n// restart: client van tu mo phong nhu cu, khong bao gio mat hieu ung.\r\n")
save(p, s, h0); print("OK KNpc.cpp")

p, s = load("KProtocolProcess.cpp"); h0 = hi(s)
if "g_nHUSK_daNhan" not in s:
    s = rep(s, "\tif (!pMsg || !HUSK_ClientBat())\r\n\t\treturn;\r\n\tS2C_SKILL_FIRED* pGoi = (S2C_SKILL_FIRED*)pMsg;\r\n",
        "\tif (!pMsg || !HUSK_ClientBat())\r\n\t\treturn;\r\n\t{ extern int g_nHUSK_daNhan; g_nHUSK_daNhan = 1; }\t// [HUSK 08/09 b] may chu nay co phat 224 -> tu day NPC khac khong tu mo phong nua\r\n\tS2C_SKILL_FIRED* pGoi = (S2C_SKILL_FIRED*)pMsg;\r\n")
    s = rep(s, "\t\t{ extern int HUSK_ClientBat(); sHello.byPhienBan = (BYTE)(HUSK_ClientBat() ? 4 : 3); }",
        "\t\t{ extern int HUSK_ClientBat(); extern int g_nHUSK_daNhan; g_nHUSK_daNhan = 0; sHello.byPhienBan = (BYTE)(HUSK_ClientBat() ? 4 : 3); }")
save(p, s, h0); print("OK KProtocolProcess.cpp")
print("XONG HUSK b")
