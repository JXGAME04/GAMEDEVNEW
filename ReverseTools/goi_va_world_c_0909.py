# -*- coding: utf-8 -*-
"""goi_va_world_c_0909.py - [WORLD 09/09 c] Tach phan "khac" cua vong vung: OBJECT / DAN (missile) + so luong.

Ket qua [WORLD b] trong tran TK 11:40: npc 84-180/tick nhung tong NPC chi 0,51-0,93 ms/tick = 4-6 us/NPC
(khong phai 45 us), 4 pha deu nho; quet_vung 2-6,5 ms (max 10,2) => 1,5-5,7 ms nam NGOAI Npc[].Activate():
vong OBJECT, vong DAN (Missle[].Activate(), be dan da nang 500 -> 3000), hay chi phi moi vung.
Do them (client, PaintLog=1): thoi gian + so phan tu vong object va vong dan trong KRegion::Activate;
in vao [WORLD b] (moi 10 s) va [WORLD-TICK] (tick >= 20 ms).
"""
import io
import sys

NL = "\r\n"
T = "\t"
S4 = "    "
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src"
F_REG = ROOT + r"\KRegion.cpp"
F_SW = ROOT + r"\KSubWorld.cpp"
F_SWS = ROOT + r"\KSubWorldSet.cpp"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, s.count("\r\n"), s.count("\n") - s.count("\r\n"), sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, crlf0, lf0, h0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "�" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


# ---------------------------------------------------------------- KRegion.cpp: dem thoi gian 2 vong
s, c0, l0, h0 = doc(F_REG)
if "[WORLD 09/09 c]" not in s:
    old = S4 + "pObjNode = (KIndexNode *)m_ObjList.GetHead();"
    new = NL.join([
        "#ifndef _SERVER",
        S4 + "LARGE_INTEGER liK0, liK1;" + T + "// [WORLD 09/09 c] thoi gian vong OBJECT va vong DAN (client, PaintLog=1)",
        S4 + "extern int g_nCorePaintLog; extern void WorldKhacXong(int nLoai, const LARGE_INTEGER& a, const LARGE_INTEGER& b, unsigned uSo);",
        S4 + "const bool bKhacDo = (g_nCorePaintLog > 0);",
        S4 + "if (bKhacDo) QueryPerformanceCounter(&liK0);",
        "#endif",
        old,
    ])
    s = rep(s, old, new, "REG obj bat dau")
    old = S4 + "if (m_nObjSyncCounter > m_ObjList.GetNodeCount() * 2)"
    new = NL.join([
        "#ifndef _SERVER",
        S4 + "if (bKhacDo) { QueryPerformanceCounter(&liK1); WorldKhacXong(0, liK0, liK1, (unsigned)m_ObjList.GetNodeCount()); }",
        "#endif",
        old,
    ])
    s = rep(s, old, new, "REG obj ket thuc")
    old = S4 + "pNode = (KIndexNode *)m_MissleList.GetHead();"
    new = NL.join([
        "#ifndef _SERVER",
        S4 + "if (bKhacDo) QueryPerformanceCounter(&liK0);" + T + "// [WORLD 09/09 c] vong DAN",
        "#endif",
        old,
    ])
    s = rep(s, old, new, "REG dan bat dau")
    old = "#ifdef _SERVER" + NL + S4 + "pNode = (KIndexNode *)m_PlayerList.GetHead();"
    new = NL.join([
        "#ifndef _SERVER",
        S4 + "if (bKhacDo) { QueryPerformanceCounter(&liK1); WorldKhacXong(1, liK0, liK1, (unsigned)m_MissleList.GetNodeCount()); }",
        "#endif",
        old,
    ])
    s = rep(s, old, new, "REG dan ket thuc")
    # Player[CLIENT_PLAYER_INDEX].Active() o duoi ham (chi vung nguoi choi dung) - nam trong quet_vung, ngoai 3 vong
    old = T*2 + "Player[CLIENT_PLAYER_INDEX].Active();"
    new = NL.join([
        T*2 + "if (bKhacDo) QueryPerformanceCounter(&liK0);" + T + "// [WORLD 09/09 c] nguoi choi (auto nhat / quet muc tieu / tim duong ...)",
        T*2 + "Player[CLIENT_PLAYER_INDEX].Active();",
        T*2 + "if (bKhacDo) { QueryPerformanceCounter(&liK1); WorldKhacXong(2, liK0, liK1, 1u); }",
    ])
    s = rep(s, old, new, "REG nguoi choi")
    ghi(F_REG, s, c0, l0, h0, "KRegion.cpp")
else:
    print("KRegion.cpp da co")

# ---------------------------------------------------------------- KSubWorld.cpp: bien + ham + in tick
s, c0, l0, h0 = doc(F_SW)
if "[WORLD 09/09 c]" not in s:
    old = "static int      t_nNpcMaxIdx = 0;"
    new = NL.join([
        old,
        "// [WORLD 09/09 c] vong OBJECT (0), vong DAN (1), Player.Active() (2): thoi gian + so phan tu, ky 10 s (g_) va moi tick (t_)",
        "double   g_dKhacMs[3] = { 0.0, 0.0, 0.0 }; unsigned g_uKhacSo[3] = { 0, 0, 0 };",
        "static double t_dKhacMs[3] = { 0.0, 0.0, 0.0 }; static unsigned t_uKhacSo[3] = { 0, 0, 0 };",
        "void WorldKhacXong(int nLoai, const LARGE_INTEGER& a, const LARGE_INTEGER& b, unsigned uSo)",
        "{",
        T + "if (nLoai < 0 || nLoai > 2) return;",
        T + "const double d = WorldMs(a, b);",
        T + "g_dKhacMs[nLoai] += d; g_uKhacSo[nLoai] += uSo; t_dKhacMs[nLoai] += d; t_uKhacSo[nLoai] += uSo;",
        "}",
    ])
    s = rep(s, old, new, "SW bien")
    old = T*4 + "\" | nang nhat %.2f ms idx %d kind %d doing %d\\n\","
    new = T*4 + "\" | nang nhat %.2f ms idx %d kind %d doing %d | object %u %.1f ms | dan %u %.1f ms | nguoi choi %.1f ms\\n\","
    s = rep(s, old, new, "SW tick fmt")
    old = T*4 + "t_dNpcMax, i, bCo ? (int)Npc[i].m_Kind : -1, bCo ? (int)Npc[i].m_Doing : -1);"
    new = T*4 + "t_dNpcMax, i, bCo ? (int)Npc[i].m_Kind : -1, bCo ? (int)Npc[i].m_Doing : -1," + NL + \
          T*4 + "t_uKhacSo[0], t_dKhacMs[0], t_uKhacSo[1], t_dKhacMs[1], t_dKhacMs[2]);"
    s = rep(s, old, new, "SW tick args")
    old = T + "t_dPha[0] = t_dPha[1] = t_dPha[2] = t_dPha[3] = 0.0;"
    new = old + NL + T + "t_dKhacMs[0] = t_dKhacMs[1] = t_dKhacMs[2] = 0.0; t_uKhacSo[0] = t_uKhacSo[1] = t_uKhacSo[2] = 0;" + T + "// [WORLD 09/09 c]"
    s = rep(s, old, new, "SW tick reset")
    ghi(F_SW, s, c0, l0, h0, "KSubWorld.cpp")
else:
    print("KSubWorld.cpp da co")

# ---------------------------------------------------------------- KSubWorldSet.cpp: in [WORLD b]
s, c0, l0, h0 = doc(F_SWS)
if "[WORLD 09/09 c]" not in s:
    old = T*2 + "extern double g_dWorldNhac, g_dNpcPha[4], g_dNpcTong, g_dNpcMax; extern unsigned g_uNpcLan; extern int g_nNpcMaxIdx;"
    new = old + NL + T*2 + "extern double g_dKhacMs[3]; extern unsigned g_uKhacSo[3];" + T + "// [WORLD 09/09 c]"
    s = rep(s, old, new, "SWS extern")
    old = T*4 + "\" | pha PS %.2f AI %.2f PC %.2f ST %.2f ms/tick | nang nhat %.2f ms idx %d\\n\","
    new = T*4 + "\" | pha PS %.2f AI %.2f PC %.2f ST %.2f ms/tick | nang nhat %.2f ms idx %d | object %.1f/tick %.2f ms | dan %.1f/tick %.2f ms | nguoi choi %.2f ms/tick\\n\","
    s = rep(s, old, new, "SWS fmt")
    old = T*4 + "g_dNpcMax, g_nNpcMaxIdx);"
    new = T*4 + "g_dNpcMax, g_nNpcMaxIdx," + NL + \
          T*4 + "(double)g_uKhacSo[0] / g_uWorldTick, g_dKhacMs[0] / g_uWorldTick, (double)g_uKhacSo[1] / g_uWorldTick, g_dKhacMs[1] / g_uWorldTick, g_dKhacMs[2] / g_uWorldTick);"
    s = rep(s, old, new, "SWS args")
    old = T*2 + "g_dNpcTong = 0.0; g_dNpcMax = 0.0; g_uNpcLan = 0; g_nNpcMaxIdx = 0;"
    new = old + NL + T*2 + "g_dKhacMs[0] = g_dKhacMs[1] = g_dKhacMs[2] = 0.0; g_uKhacSo[0] = g_uKhacSo[1] = g_uKhacSo[2] = 0;" + T + "// [WORLD 09/09 c]"
    s = rep(s, old, new, "SWS reset")
    ghi(F_SWS, s, c0, l0, h0, "KSubWorldSet.cpp")
else:
    print("KSubWorldSet.cpp da co")
print("XONG WORLD c")
