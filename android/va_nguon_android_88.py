# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 KHOAMT d] Nhat ky va 87 cho thay khoa DAT DUOC roi MAT ngay trong 1 giay:
#   [TTMT] cham (611,283) -> khoa muc tieu 17 (LePhong143)
#   [TTMT] thong tin doi: nguoi=0 hover=0 khoa=0 -> chon 0
# Tuc nhanh kiem "con song khong" (Npc[o].m_dwID && m_RegionIndex >= 0) that bai ngay sau do. Nguyen nhan: KHOA THEO
# O MANG (chi so Npc[]) - o do co the bi dung lai cho NPC khac, hoac NPC tam thoi ra khoi vung ma client dang giu.
#
# SUA: khoa theo MA NGUOI CHOI (m_dwID) chu khong theo o mang:
#   - Luc cham: nho ca o va ma (g_uJxMucTieuKhoaId).
#   - Moi lan hoi thong tin: o cu con dung ma -> dung ngay (duong nhanh); khong thi QUET NpcSet tim lai ma do
#     -> tim thay thi cap nhat o, khong thay thi cho them KHOAMT_CHO_MS (3 s) roi moi bo (nguoi choi chay ra
#     khoi tam nhin mot lat khong lam mat thanh thong tin).
#   - Khi bo that thi ghi ro LY DO mot lan.
# Windows: khong doi (rao JX_ANDROID).

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 KHOAMT d]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, ham):
    s = doc(p)
    if DAU in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


def va_kplayer(s):
    return thay(s, ["int g_nJxMucTieuKhoa = 0;\t// [ANDROID 11/09 KHOAMT] NPC/nguoi choi da CHAM (OnButtonDown); 0 = khong khoa. Chi client Android."],
                ["int g_nJxMucTieuKhoa = 0;\t// [ANDROID 11/09 KHOAMT] NPC/nguoi choi da CHAM (OnButtonDown); 0 = khong khoa. Chi client Android.",
                 "unsigned int g_uJxMucTieuKhoaId = 0;\t// %s MA (m_dwID) cua nguoi da cham - khoa theo ma, khong theo o mang" % DAU,
                 "unsigned int g_uJxMucTieuMatLuc = 0;\t// %s luc bat dau khong tim thay (0 = dang thay); qua 3 s moi bo han" % DAU],
                "KPlayer.cpp: bien khoa theo ma")


def va_coreshell(s):
    # luc cham: nho ca ma
    s = thay(s, ["\t\t\t\tg_nJxMucTieuKhoa = nTrung;",
                 "\t\t\t\tg_DebugLog(\"[TTMT] cham (%d,%d) -> khoa muc tieu %d (%s)\", pCham->nViTriVeX, pCham->nViTriVeY, nTrung, Npc[nTrung].Name);"],
                ["\t\t\t\textern unsigned int g_uJxMucTieuKhoaId, g_uJxMucTieuMatLuc;",
                 "\t\t\t\tg_nJxMucTieuKhoa = nTrung;",
                 "\t\t\t\tg_uJxMucTieuKhoaId = Npc[nTrung].m_dwID;\t// %s khoa theo MA, o mang co the bi dung lai cho NPC khac" % DAU,
                 "\t\t\t\tg_uJxMucTieuMatLuc = 0;",
                 "\t\t\t\tg_DebugLog(\"[TTMT] cham (%d,%d) -> khoa muc tieu %d ma %u (%s)\", pCham->nViTriVeX, pCham->nViTriVeY, nTrung, g_uJxMucTieuKhoaId, Npc[nTrung].Name);"],
                "CoreShell.cpp: nho ma khi khoa")
    # luc hoi: tim theo ma
    s = thay(s, ["\t\t\textern int g_nJxMucTieuKhoa;",
                 "\t\t\tif (g_nJxMucTieuKhoa > 0 && g_nJxMucTieuKhoa < MAX_NPC && Npc[g_nJxMucTieuKhoa].m_dwID && Npc[g_nJxMucTieuKhoa].m_RegionIndex >= 0)",
                 "\t\t\t\tidx = g_nJxMucTieuKhoa;",
                 "\t\t\telse",
                 "\t\t\t\tg_nJxMucTieuKhoa = 0;"],
                ["\t\t\textern int g_nJxMucTieuKhoa;",
                 "\t\t\textern unsigned int g_uJxMucTieuKhoaId, g_uJxMucTieuMatLuc;",
                 "\t\t\t// %s tim theo MA: o cu con dung ma thi dung ngay, khong thi quet lai; mat qua 3 s moi bo han" % DAU,
                 "\t\t\tif (g_uJxMucTieuKhoaId)",
                 "\t\t\t{",
                 "\t\t\t\tint nO = 0;",
                 "",
                 "\t\t\t\tif (g_nJxMucTieuKhoa > 0 && g_nJxMucTieuKhoa < MAX_NPC",
                 "\t\t\t\t\t&& Npc[g_nJxMucTieuKhoa].m_dwID == g_uJxMucTieuKhoaId && Npc[g_nJxMucTieuKhoa].m_RegionIndex >= 0)",
                 "\t\t\t\t\tnO = g_nJxMucTieuKhoa;\t// duong nhanh: o cu van dung nguoi do",
                 "\t\t\t\telse",
                 "\t\t\t\t{",
                 "\t\t\t\t\tint nQ = 0;",
                 "",
                 "\t\t\t\t\twhile (nQ = NpcSet.GetNextIdx(nQ))",
                 "\t\t\t\t\t{",
                 "\t\t\t\t\t\tif (Npc[nQ].m_dwID == g_uJxMucTieuKhoaId && Npc[nQ].m_RegionIndex >= 0)",
                 "\t\t\t\t\t\t{",
                 "\t\t\t\t\t\t\tnO = nQ;",
                 "\t\t\t\t\t\t\tbreak;",
                 "\t\t\t\t\t\t}",
                 "\t\t\t\t\t}",
                 "\t\t\t\t}",
                 "\t\t\t\tif (nO)",
                 "\t\t\t\t{",
                 "\t\t\t\t\tg_nJxMucTieuKhoa = nO;",
                 "\t\t\t\t\tg_uJxMucTieuMatLuc = 0;",
                 "\t\t\t\t\tidx = nO;",
                 "\t\t\t\t}",
                 "\t\t\t\telse",
                 "\t\t\t\t{\t// tam thoi khong thay (ra khoi tam nhin): giu them 3 s roi moi bo",
                 "\t\t\t\t\tunsigned int uNay = (unsigned int)GetTickCount();",
                 "",
                 "\t\t\t\t\tif (g_uJxMucTieuMatLuc == 0)",
                 "\t\t\t\t\t\tg_uJxMucTieuMatLuc = uNay;",
                 "\t\t\t\t\telse if (uNay - g_uJxMucTieuMatLuc > 3000)",
                 "\t\t\t\t\t{",
                 "\t\t\t\t\t\tg_DebugLog(\"[TTMT] bo khoa: ma %u khong con thay sau 3 s\", g_uJxMucTieuKhoaId);",
                 "\t\t\t\t\t\tg_uJxMucTieuKhoaId = 0;",
                 "\t\t\t\t\t\tg_nJxMucTieuKhoa = 0;",
                 "\t\t\t\t\t\tg_uJxMucTieuMatLuc = 0;",
                 "\t\t\t\t\t}",
                 "\t\t\t\t}",
                 "\t\t\t}"],
                "CoreShell.cpp: tim khoa theo ma")
    return s


va("Sources/Core/Src/KPlayer.cpp", va_kplayer)
va("Sources/Core/Src/CoreShell.cpp", va_coreshell)
print("xong")
