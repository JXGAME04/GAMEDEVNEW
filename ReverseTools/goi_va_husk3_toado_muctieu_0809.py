# -*- coding: utf-8 -*-
"""goi_va_husk3_toado_muctieu_0809.py - [HUSK 08/09 c] Do that tran TK 15:5x: client ve 87 % goi 224; 9,7 % (co cua so 24 %) la 'notgt' =
NPC muc tieu KHONG co trong bang NPC client (chua nap / mo coi) -> gói mang them TOA DO MPS cua muc tieu (S2C_SKILL_FIRED 16 -> 24 byte):
client khong co muc tieu thi van ve chieu bay toi diem do. Doi co goi -> hello phien ban 5, may chu chi phat khi moi client >= 5
(client 4 cu tu lui ve tu mo phong nho g_nHUSK_daNhan = 0). Bo dem moi: notgt_ve_toado."""
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

p, s = load("KProtocol.h"); h0 = hi(s)
if "nTgtMpsX" not in s:
    s = rep(s, "\tint\t\tnMpsX;\r\n\tint\t\tnMpsY;\r\n} S2C_SKILL_FIRED;\t\t\t// 16 byte (pack 1)\r\n",
        "\tint\t\tnMpsX;\r\n\tint\t\tnMpsY;\r\n"
        "\tint\t\tnTgtMpsX;\t\t// [HUSK 08/09 c] toa do MPS muc tieu (hoac = nMpsX/nMpsY khi phong theo toa do): client khong co NPC muc tieu\r\n"
        "\tint\t\tnTgtMpsY;\t\t// (chua nap / mo coi) van ve chieu bay toi diem nay. Hello phien ban 5.\r\n"
        "} S2C_SKILL_FIRED;\t\t\t// 24 byte (pack 1)\r\n")
save(p, s, h0); print("OK KProtocol.h")

p, s = load("KNpc.cpp"); h0 = hi(s)
if "nTgtMpsX" not in s:
    s = rep(s, "\t\t\tif (Player[i].m_nNetConnectIdx >= 0 && g_abyDeltaHello[i] < 4)\r\n\t\t\t\tn++;\r\n",
        "\t\t\tif (Player[i].m_nNetConnectIdx >= 0 && g_abyDeltaHello[i] < 5)\t// [HUSK 08/09 c] 5 = goi 24 byte co toa do muc tieu\r\n\t\t\t\tn++;\r\n")
    s = rep(s, "\t\tsGoi.nMpsX = -1;\r\n\t\tsGoi.nMpsY = (int)Npc[nParam2].m_dwID;\r\n\t}\r\n\telse\r\n\t{\r\n\t\tsGoi.nMpsX = nParam1;\r\n\t\tsGoi.nMpsY = nParam2;\r\n\t}\r\n",
        "\t\tsGoi.nMpsX = -1;\r\n\t\tsGoi.nMpsY = (int)Npc[nParam2].m_dwID;\r\n"
        "\t\tsGoi.nTgtMpsX = 0; sGoi.nTgtMpsY = 0;\r\n"
        "\t\tif (Npc[nParam2].m_RegionIndex >= 0 && Npc[nParam2].m_SubWorldIndex == m_SubWorldIndex)\r\n"
        "\t\t\tNpc[nParam2].GetMpsPos(&sGoi.nTgtMpsX, &sGoi.nTgtMpsY);\t// [HUSK 08/09 c] du phong khi client khong co NPC muc tieu\r\n"
        "\t}\r\n\telse\r\n\t{\r\n\t\tsGoi.nMpsX = nParam1;\r\n\t\tsGoi.nMpsY = nParam2;\r\n\t\tsGoi.nTgtMpsX = nParam1;\r\n\t\tsGoi.nTgtMpsY = nParam2;\r\n\t}\r\n")
    s = rep(s, "int g_nFX_husk_rx = 0, g_nFX_husk_noidx = 0, g_nFX_husk_minh = 0, g_nFX_husk_noskill = 0, g_nFX_husk_notgt = 0, g_nFX_husk_fire = 0, g_nFX_husk_fail = 0, g_nFX_husk_bocuc = 0;\t// [HUSK 08/09] client: goi 224\r\n",
        "int g_nFX_husk_rx = 0, g_nFX_husk_noidx = 0, g_nFX_husk_minh = 0, g_nFX_husk_noskill = 0, g_nFX_husk_notgt = 0, g_nFX_husk_fire = 0, g_nFX_husk_fail = 0, g_nFX_husk_bocuc = 0;\t// [HUSK 08/09] client: goi 224\r\n"
        "int g_nFX_husk_toado = 0;\t// [HUSK 08/09 c] client: muc tieu khong co -> ve theo toa do trong goi\r\n")
    s = rep(s, "huy_sync=%d msl_max=%d/%d | HUSK(224): rx=%d ve=%d hong=%d noidx=%d minh=%d noskill=%d notgt=%d bo_cuc=%d\",\r\n",
        "huy_sync=%d msl_max=%d/%d | HUSK(224): rx=%d ve=%d hong=%d noidx=%d minh=%d noskill=%d notgt=%d notgt_ve_toado=%d bo_cuc=%d\",\r\n")
    s = rep(s, "\t\t\t\t\tg_nFX_husk_rx, g_nFX_husk_fire, g_nFX_husk_fail, g_nFX_husk_noidx, g_nFX_husk_minh, g_nFX_husk_noskill, g_nFX_husk_notgt, g_nFX_husk_bocuc);\r\n",
        "\t\t\t\t\tg_nFX_husk_rx, g_nFX_husk_fire, g_nFX_husk_fail, g_nFX_husk_noidx, g_nFX_husk_minh, g_nFX_husk_noskill, g_nFX_husk_notgt, g_nFX_husk_toado, g_nFX_husk_bocuc);\r\n")
    s = rep(s, "g_nFX_husk_notgt = 0; g_nFX_husk_bocuc = 0;\t// [HUSK 08/09]\r\n", "g_nFX_husk_notgt = 0; g_nFX_husk_toado = 0; g_nFX_husk_bocuc = 0;\t// [HUSK 08/09]\r\n")
save(p, s, h0); print("OK KNpc.cpp")

p, s = load("KProtocolProcess.cpp"); h0 = hi(s)
if "nTgtMpsX" not in s:
    s = rep(s, "\t\t{ extern int HUSK_ClientBat(); extern int g_nHUSK_daNhan; g_nHUSK_daNhan = 0; sHello.byPhienBan = (BYTE)(HUSK_ClientBat() ? 4 : 3); }",
        "\t\t{ extern int HUSK_ClientBat(); extern int g_nHUSK_daNhan; g_nHUSK_daNhan = 0; sHello.byPhienBan = (BYTE)(HUSK_ClientBat() ? 5 : 3); }\t// [HUSK 08/09 c] 5 = goi 224 24 byte co toa do muc tieu")
    s = rep(s, "\textern int g_nFX_husk_rx, g_nFX_husk_noidx, g_nFX_husk_minh, g_nFX_husk_noskill, g_nFX_husk_notgt, g_nFX_husk_fire, g_nFX_husk_fail;\r\n",
        "\textern int g_nFX_husk_rx, g_nFX_husk_noidx, g_nFX_husk_minh, g_nFX_husk_noskill, g_nFX_husk_notgt, g_nFX_husk_fire, g_nFX_husk_fail, g_nFX_husk_toado;\r\n")
    s = rep(s, "\t\tint nTgt = (pGoi->nMpsY > 0) ? NpcSet.SearchID((DWORD)pGoi->nMpsY) : 0;\r\n"
               "\t\tif (nTgt <= 0 || nTgt >= MAX_NPC || Npc[nTgt].m_RegionIndex < 0) { g_nFX_husk_notgt++; return; }\r\n"
               "\t\tbVe = pSkill->Cast(nIdx, -1, nTgt);\r\n",
        "\t\tint nTgt = (pGoi->nMpsY > 0) ? NpcSet.SearchID((DWORD)pGoi->nMpsY) : 0;\r\n"
        "\t\tif (nTgt > 0 && nTgt < MAX_NPC && Npc[nTgt].m_RegionIndex >= 0)\r\n"
        "\t\t\tbVe = pSkill->Cast(nIdx, -1, nTgt);\r\n"
        "\t\telse if (pGoi->nTgtMpsX > 0 && pGoi->nTgtMpsY > 0)\r\n"
        "\t\t{\t// [HUSK 08/09 c] khong co NPC muc tieu tren client (chua nap / mo coi): ve chieu bay toi toa do muc tieu\r\n"
        "\t\t\tbVe = pSkill->Cast(nIdx, pGoi->nTgtMpsX, pGoi->nTgtMpsY);\r\n"
        "\t\t\tg_nFX_husk_toado++;\r\n\t\t}\r\n"
        "\t\telse { g_nFX_husk_notgt++; return; }\r\n")
save(p, s, h0); print("OK KProtocolProcess.cpp")
print("XONG HUSK c")
