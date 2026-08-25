# -*- coding: utf-8 -*-
"""C32 - YEU CAU CHU: "chua nhan nhiem vu thi kich vao chi nam nhiem vu o Sat Thu
thi TU DUNG THO DIA PHU VE roi DI CHUYEN TOI NPC Nhiep Thi Tran o map do".
Them 2 pha vao may TG_SatThu:
   10 = chua nhan nhiem vu, dang o map KHONG co NPC 769 -> DT_UsePortal (tho dia /
        hoi thanh phu - dung ham co san cua nghe Da Tau) roi doi doi map
   11 = dang o map CO NPC 769 -> di toi toa do NPC (bang s_nST3Npc* SINH TU DONG
        tu autoexec_npc_hd3.lua) -> tim NPC theo TEMPLATE 769 (chac chan hon so
        ten vi ten TCVN3 co byte cao) -> mo thoai roi tat auto.
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

P = r"D:\GAMEDEVNEW\Sources\Core\Src\CoreShell.cpp"
T = "\t"
d = io.open(P, encoding="latin-1", newline="").read()
NL = "\r\n" if "\r\n" in d else "\n"
if "TG_SatThuTimNpc" in d:
    print("da va roi"); raise SystemExit

MSG_VE   = V("[Chỉ nam] Chưa nhận nhiệm vụ - đang dùng phù về thành gặp Nhiếp Thí Trần...")
MSG_TOINPC = V("[Chỉ nam] Đang chạy tới Nhiếp Thí Trần...")
MSG_GAP  = V("[Chỉ nam] Đã tới Nhiếp Thí Trần - hãy chọn nhiệm vụ sát thủ.")
MSG_KHONGPHU = V("[Chỉ nam] Không có phù về thành trong túi - hãy tự về thành.")

# ---- 1) ham tim NPC 769 gan nhat (theo TEMPLATE, khong theo ten) ----
a = "static void TG_SatThuStop(const char* szMsg)"
assert d.count(a) == 1
ham = NL.join([
    "// [C32] tim NPC theo TEMPLATE (m_NpcSettingIdx) - chac chan hon so ten vi ten",
    "// TCVN3 co byte cao, g_StrLower co the doi byte.",
    "static int TG_SatThuTimNpc(int nTemplate, int nAtX, int nAtY, int nRadius)",
    "{",
    T + "int dX, dY;",
    T + "int nIdx = 0;",
    T + "while (nIdx = NpcSet.GetNextIdx(nIdx))",
    T + "{",
    T + T + "if (Npc[nIdx].m_NpcSettingIdx != nTemplate)",
    T + T + T + "continue;",
    T + T + "if (Npc[nIdx].m_RegionIndex < 0)",
    T + T + T + "continue;",
    T + T + "Npc[nIdx].GetMpsPos(&dX, &dY);",
    T + T + "if (nRadius > 0 && g_GetDistance(nAtX, nAtY, dX, dY) > nRadius)",
    T + T + T + "continue;",
    T + T + "return nIdx;",
    T + "}",
    T + "return 0;",
    "}",
    "",
    "// [C32] map hien tai co NPC Nhiep Thi Tran khong -> tra chi so trong bang, -1 neu khong",
    "static int TG_SatThuChiSoNpcMap(int nMap)",
    "{",
    T + "for (int i = 0; i < ST3_NPC_SO; i++)",
    T + T + "if (s_nST3NpcMap[i] == nMap)",
    T + T + T + "return i;",
    T + "return -1;",
    "}",
    "",
])
d = d.replace(a, ham + a)

# ---- 2) Start: chua nhan nhiem vu -> pha 10/11 thay vi bao loi ----
old = NL.join([
    T + "int nBoss = (int)Player[nPlayerIdx].m_cTask.GetSaveVal(1082);",
    T + "if (nBoss < 1 || nBoss > ST3_POS_MAX)",
    T + "{",
    T + T + 'DT_Msg(nPlayerIdx, "<color=Yellow>' + V("[Chỉ nam] Chưa nhận nhiệm vụ sát thủ.") + '");',
    T + T + "return 0;",
    T + "}",
])
assert old in d, "anchor Start"
new = NL.join([
    T + "int nBoss = (int)Player[nPlayerIdx].m_cTask.GetSaveVal(1082);",
    T + "if (nBoss < 1 || nBoss > ST3_POS_MAX)",
    T + "{",
    T + T + "// [C32] CHUA nhan nhiem vu -> dan duong VE GAP NPC Nhiep Thi Tran",
    T + T + "g_nTGSTBoss = 0;",
    T + T + "g_nTGSTMap = 0;",
    T + T + "g_nTGSTTry = 0;",
    T + T + "g_nTGSTDlgTry = 0;",
    T + T + "g_uTGSTNext = 0;",
    T + T + "g_nTGSTNpc = 0;",
    T + T + "g_uTGSTDlgSeen = g_sDTCap.uDlgSeq;",
    T + T + "g_nTGSTPhase = (TG_SatThuChiSoNpcMap(SubWorld[0].m_SubWorldID) >= 0) ? 11 : 10;",
    T + T + "g_nTGSTOn = 1;",
    T + T + 'DT_Msg(nPlayerIdx, "<color=Cyan>%s");' % (MSG_TOINPC if False else MSG_VE),
    T + T + "return 1;",
    T + "}",
])
d = d.replace(old, new)

# ---- 3) Tick: xu ly pha 10/11 (dat ngay sau kiem timeout) ----
a3 = T + "// --- pha 1: chay toi Xa Phu roi mo thoai ---"
assert d.count(a3) == 1
pha = NL.join([
    T + "// --- pha 10: chua nhan nhiem vu, khong o map co NPC -> dung phu ve thanh ---",
    T + "if (g_nTGSTPhase == 10)",
    T + "{",
    T + T + "if (TG_SatThuChiSoNpcMap(SubWorld[0].m_SubWorldID) >= 0)",
    T + T + "{",
    T + T + T + "g_nTGSTPhase = 11;",
    T + T + T + "g_nTGSTTry = 0;",
    T + T + T + "return;",
    T + T + "}",
    T + T + "if ((g_nTGSTTry % 12) == 1)\t// ~5s thu dung phu mot lan",
    T + T + "{",
    T + T + T + "if (!DT_UsePortal(nPlayerIdx))",
    T + T + T + "{",
    T + T + T + T + 'TG_SatThuStop("<color=Yellow>' + MSG_KHONGPHU + '");',
    T + T + T + T + "return;",
    T + T + T + "}",
    T + T + "}",
    T + T + "return;",
    T + "}",
    T + "// --- pha 11: chay toi NPC Nhiep Thi Tran roi mo thoai ---",
    T + "if (g_nTGSTPhase == 11)",
    T + "{",
    T + T + "int nI = TG_SatThuChiSoNpcMap(SubWorld[0].m_SubWorldID);",
    T + T + "if (nI < 0)",
    T + T + "{",
    T + T + T + "g_nTGSTPhase = 10;\t// bi keo sang map khac - ve thanh lai",
    T + T + T + "return;",
    T + T + "}",
    T + T + "int nDX = s_nST3NpcX[nI] * 32;",
    T + T + "int nDY = s_nST3NpcY[nI] * 32;",
    T + T + "int nNpc = TG_SatThuTimNpc(ST3_NPC_TEMPLATE, nDX, nDY, 600);",
    T + T + "if (nNpc)",
    T + T + "{",
    T + T + T + "int nX, nY, dX, dY;",
    T + T + T + "Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nX, &nY);",
    T + T + T + "Npc[nNpc].GetMpsPos(&dX, &dY);",
    T + T + T + "if (g_GetDistance(nX, nY, dX, dY) <= 160)",
    T + T + T + "{",
    T + T + T + T + "Player[nPlayerIdx].DialogNpc(nNpc);",
    T + T + T + T + 'TG_SatThuStop("<color=Cyan>' + MSG_GAP + '");',
    T + T + T + T + "return;",
    T + T + T + "}",
    T + T + T + "DT_WalkTo(nPlayerIdx, dX, dY, 128, uCur);",
    T + T + T + "return;",
    T + T + "}",
    T + T + "DT_WalkTo(nPlayerIdx, nDX, nDY, 200, uCur);",
    T + T + "return;",
    T + "}",
    "",
])
d = d.replace(a3, pha + a3)
io.open(P, "w", encoding="latin-1", newline="").write(d)
print("da them pha 10/11 (chua nhan nhiem vu -> ve thanh gap NPC)")
