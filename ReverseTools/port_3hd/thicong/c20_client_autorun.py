# -*- coding: utf-8 -*-
"""C20 - phan CLIENT cua "kich -> tu chay toi xa phu -> qua map -> toi toa do boss":
  1) CoreShell.h: op moi GOI_TASKGUIDE_GOTO_SATTHU (cuoi enum - noi bo client,
     khong dung protocol mang).
  2) CoreShell.cpp: may TG_SatThu (nhai khuon TG_XaFu, tai dung DT_WalkTo /
     DT_FindNpcName / g_MoveStation / DT_Msg):
       pha 1: chay den Xa Phu thanh hien tai -> gui uicmdscript "st3_goboss"
              (server thu tien + NewWorld toi gan boss - khuon cu denchobossST)
       pha 2: doi chuyen map (server tra loi bang Msg2Player neu thieu tien)
       pha 3: DT_WalkTo toi o boss (bang KSatThuBossPos.h SINH TU DONG)
     Neu dang o DUNG map boss thi vao thang pha 3 (khong ton tien xe).
     Bam lai dong nhiem vu khi dang chay = huy (giong Da Tau).
  3) UiTaskGuide.cpp: TryGoXaFu mo nhanh SATTHU; chu "thu %d" -> "so hieu %d"
     (nguoi choi TU CHON boss trong nhom nen so 147 la SO DONG BANG, khong phai
     lan nhan thu may); them dong huong dan bam de tu chay.
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
CU = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase"
T = "\t"


def rw(p, fn):
    d = io.open(p, encoding="latin-1", newline="").read()
    d2 = fn(d)
    if d2 != d:
        io.open(p, "w", encoding="latin-1", newline="").write(d2)
        print("  OK", p.rsplit("\\", 1)[-1])
    else:
        print("  (khong doi)", p.rsplit("\\", 1)[-1])


# ---------- 1) CoreShell.h ----------
def f_h(d):
    if "GOI_TASKGUIDE_GOTO_SATTHU" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = "\tGDCNI_CITY_INFO_UPDATE,\t\t// [CITYINFO 21/08] uParam = id thanh vua nhan tu server"
    assert d.count(a) == 1
    return d.replace(a, a + NL + "\tGOI_TASKGUIDE_GOTO_SATTHU,\t// [3HD C20] bam nhiem vu Sat Thu tren F11 -> tu chay toi boss (bam lai = huy)")
rw(CORE + r"\CoreShell.h", f_h)


# ---------- 2) CoreShell.cpp ----------
def f_cpp(d):
    if "TG_SatThuStart" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    # 2a. may TG_SatThu - dat truoc khoi DT_FindFarMob
    a = "// [DaTau] tim quai con SONG da sync NGOAI tam danh de chay toi (T4 di tim quai)."
    assert d.count(a) == 1
    MSG_HUY = V("[Chỉ nam] Đã hủy dẫn đường tới boss.")
    MSG_KHONGNV = V("[Chỉ nam] Chưa nhận nhiệm vụ sát thủ.")
    MSG_KHONGXAPHU = V("[Chỉ nam] Không thấy Xa Phu ở map này - hãy về thành rồi bấm lại.")
    MSG_BATDAU = V("[Chỉ nam] Đang dẫn đường tới boss - bấm lại vào dòng nhiệm vụ để hủy.")
    MSG_GAPXAPHU = V("[Chỉ nam] Đã gặp Xa Phu - đang thuê xe tới chỗ boss...")
    MSG_LAU = V("[Chỉ nam] Đi quá lâu - dừng dẫn đường.")
    MSG_KHONGQUA = V("[Chỉ nam] Chưa được chuyển map (thiếu tiền xe?) - dừng dẫn đường.")
    MSG_TOI = V("[Chỉ nam] Đã tới khu vực boss - cẩn thận!")
    body = NL.join([
        "//---------------------------------------------------------------------------",
        "// [3HD C20] Bam nhiem vu 'San Boss Sat Thu' tren F11 -> tu dan duong toi boss:",
        "//   pha 1: chay den Xa Phu (tai dung nghe Da Tau) roi gui 'st3_goboss' - server",
        "//          thu tien xe + NewWorld toi gan boss (khuon cu denchobossST ban Viet);",
        "//   pha 2: doi chuyen map;  pha 3: di bo toi o boss (KSatThuBossPos.h).",
        "// Dang o dung map boss thi vao thang pha 3 (khong ton tien).",
        "//---------------------------------------------------------------------------",
        "#include \"KSatThuBossPos.h\"",
        "",
        "static int  g_nTGSTOn = 0;",
        "static int  g_nTGSTPhase = 0;\t\t// 1 = di xa phu, 2 = doi chuyen map, 3 = di toi boss",
        "static int  g_nTGSTMap = 0;\t\t\t// map boss",
        "static int  g_nTGSTX = 0, g_nTGSTY = 0;\t// o boss (cell)",
        "static int  g_nTGSTTry = 0;",
        "static UINT g_uTGSTNext = 0;",
        "",
        "static void TG_SatThuStop(const char* szMsg)",
        "{",
        T + "if (g_nTGSTOn && szMsg)",
        T + T + "DT_Msg(CLIENT_PLAYER_INDEX, szMsg);",
        T + "g_nTGSTOn = 0;",
        "}",
        "",
        "static int TG_SatThuStart()",
        "{",
        T + "int nPlayerIdx = CLIENT_PLAYER_INDEX;",
        T + "if (g_nTGSTOn)",
        T + "{",
        T + T + "TG_SatThuStop(\"<color=Cyan>" + MSG_HUY + "\");",
        T + T + "return 0;",
        T + "}",
        T + "if (Player[nPlayerIdx].m_nIndex <= 0)",
        T + T + "return 0;",
        T + "int nBoss = (int)Player[nPlayerIdx].m_cTask.GetSaveVal(1082);",
        T + "if (nBoss < 1 || nBoss > ST3_POS_MAX)",
        T + "{",
        T + T + "DT_Msg(nPlayerIdx, \"<color=Yellow>" + MSG_KHONGNV + "\");",
        T + T + "return 0;",
        T + "}",
        T + "g_nTGSTMap = s_nST3BossMap[nBoss];",
        T + "g_nTGSTX = s_nST3BossX[nBoss];",
        T + "g_nTGSTY = s_nST3BossY[nBoss];",
        T + "g_nTGSTTry = 0;",
        T + "g_uTGSTNext = 0;",
        T + "if (SubWorld[0].m_SubWorldID == g_nTGSTMap)",
        T + T + "g_nTGSTPhase = 3;\t// da o map boss - di bo toi noi",
        T + "else",
        T + "{",
        T + T + "MapStation::iterator it = g_MoveStation.find(SubWorld[0].m_SubWorldID);",
        T + T + "if (it == g_MoveStation.end() || it->second.empty())",
        T + T + "{",
        T + T + T + "DT_Msg(nPlayerIdx, \"<color=Yellow>" + MSG_KHONGXAPHU + "\");",
        T + T + T + "return 0;",
        T + T + "}",
        T + T + "g_nTGSTPhase = 1;",
        T + "}",
        T + "g_nTGSTOn = 1;",
        T + "DT_Msg(nPlayerIdx, \"<color=Cyan>" + MSG_BATDAU + "\");",
        T + "return 1;",
        "}",
        "",
        "static void TG_SatThuTick()",
        "{",
        T + "if (!g_nTGSTOn)",
        T + T + "return;",
        T + "int nPlayerIdx = CLIENT_PLAYER_INDEX;",
        T + "if (Player[nPlayerIdx].m_nIndex <= 0)",
        T + "{",
        T + T + "g_nTGSTOn = 0;",
        T + T + "return;",
        T + "}",
        T + "UINT uCur = timeGetTime();",
        T + "if (uCur < g_uTGSTNext)",
        T + T + "return;",
        T + "g_uTGSTNext = uCur + 400;",
        T + "if (++g_nTGSTTry > 450)\t// ~3 phut",
        T + "{",
        T + T + "TG_SatThuStop(\"<color=Yellow>" + MSG_LAU + "\");",
        T + T + "return;",
        T + "}",
        T + "if (g_nTGSTPhase == 1)",
        T + "{",
        T + T + "MapStation::iterator it = g_MoveStation.find(SubWorld[0].m_SubWorldID);",
        T + T + "if (it == g_MoveStation.end() || it->second.empty())",
        T + T + "{",
        T + T + T + "TG_SatThuStop(\"<color=Yellow>" + MSG_KHONGXAPHU + "\");",
        T + T + T + "return;",
        T + T + "}",
        T + T + "sStation& s = it->second[0];",
        T + T + "int nIdx = DT_FindNpcName(nPlayerIdx, \"xa phu\", s.x, s.y, 400);",
        T + T + "if (nIdx)",
        T + T + "{",
        T + T + T + "int nX, nY, dX, dY;",
        T + T + T + "Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nX, &nY);",
        T + T + T + "Npc[nIdx].GetMpsPos(&dX, &dY);",
        T + T + T + "if (g_GetDistance(nX, nY, dX, dY) <= 160)",
        T + T + T + "{",
        T + T + T + T + "SendUiCmdScript(6, (char*)\"st3_goboss\");",
        T + T + T + T + "g_nTGSTPhase = 2;",
        T + T + T + T + "g_nTGSTTry = 0;",
        T + T + T + T + "DT_Msg(nPlayerIdx, \"<color=Cyan>" + MSG_GAPXAPHU + "\");",
        T + T + T + T + "return;",
        T + T + T + "}",
        T + T + T + "DT_WalkTo(nPlayerIdx, dX, dY, 128, uCur);",
        T + T + T + "return;",
        T + T + "}",
        T + T + "DT_WalkTo(nPlayerIdx, s.x, s.y, 200, uCur);",
        T + T + "return;",
        T + "}",
        T + "if (g_nTGSTPhase == 2)",
        T + "{",
        T + T + "if (SubWorld[0].m_SubWorldID == g_nTGSTMap)",
        T + T + "{",
        T + T + T + "g_nTGSTPhase = 3;",
        T + T + T + "g_nTGSTTry = 0;",
        T + T + T + "return;",
        T + T + "}",
        T + T + "if (g_nTGSTTry > 25)\t// ~10s khong duoc chuyen (thieu tien...)",
        T + T + T + "TG_SatThuStop(\"<color=Yellow>" + MSG_KHONGQUA + "\");",
        T + T + "return;",
        T + "}",
        T + "// pha 3: di toi o boss",
        T + "if (SubWorld[0].m_SubWorldID != g_nTGSTMap)",
        T + "{",
        T + T + "g_nTGSTOn = 0;\t// bi keo sang map khac - tat im lang",
        T + T + "return;",
        T + "}",
        T + "if (DT_WalkTo(nPlayerIdx, g_nTGSTX * 32, g_nTGSTY * 32, 250, uCur))",
        T + T + "TG_SatThuStop(\"<color=Cyan>" + MSG_TOI + "\");",
        "}",
        "",
    ])
    d = d.replace(a, body + a)
    # 2b. tick tai 2 site canh TG_XaFuTick
    old_t = "TG_XaFuTick();\t// [TaskGuide] dan duong den Xa Phu (chi chay khi dang bat)"
    assert d.count(old_t) == 2
    d = d.replace(old_t, old_t + NL + T + T + "TG_SatThuTick();\t// [3HD C20] dan duong toi boss Sat Thu", 1)
    # site 2 co indent khac (1 tab) - thay ca 2 bang regex don gian: lan 2
    d = d.replace(old_t, old_t + NL + T + "TG_SatThuTick();\t// [3HD C20] dan duong toi boss Sat Thu", 1)
    # 2c. case op
    a2 = T + "case GOI_TASKGUIDE_GOTO_XAFU:\t// [TaskGuide] bang F11: nhiem vu loai 4 -> tu chay den Xa Phu"
    assert d.count(a2) == 1
    ins = NL.join([
        T + "case GOI_TASKGUIDE_GOTO_SATTHU:\t// [3HD C20] bang F11: nhiem vu Sat Thu -> tu chay toi boss",
        T + T + "nRet = TG_SatThuStart();",
        T + T + "break;",
    ])
    d = d.replace(a2, ins + NL + a2)
    return d
rw(CORE + r"\CoreShell.cpp", f_cpp)


# ---------- 3) UiTaskGuideStr.h: doi chu + hint ----------
def f_str(d):
    NL = "\r\n" if "\r\n" in d else "\n"
    changed = False
    old = '#define ST3_CUR_FMT'
    # thay toan dong CUR_FMT
    lines = d.split(NL)
    for i, l in enumerate(lines):
        if l.startswith('#define ST3_CUR_FMT'):
            newl = '#define ST3_CUR_FMT    "' + V("Đang truy nã sát thủ số hiệu") + ' <color=yellow>%d <color>' + V("(nhóm cấp %d) - ngươi tự chọn trong danh sách.") + '"'
            if lines[i] != newl:
                lines[i] = newl
                changed = True
    d = NL.join(lines)
    if "ST3_HINT_GO" not in d:
        d = d.rstrip("\r\n") + NL + '#define ST3_HINT_GO    "' + V("Kích vào dòng nhiệm vụ để tự chạy tới boss (tốn tiền xe nếu khác map).") + '"' + NL
        changed = True
    return d
rw(CU + r"\UiTaskGuideStr.h", f_str)


# ---------- 4) UiTaskGuide.cpp: TryGoXaFu nhanh SATTHU + hint ----------
def f_ui(d):
    if "GOI_TASKGUIDE_GOTO_SATTHU" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    old = NL.join([
        T + "if (m_Entries[m_nCurEntry].nTaskId != TASKGUIDE_DATAU_TASKID)",
        T + T + "return;",
        T + "if (DTG_TaskVal(1021) != 4 || DTG_TaskVal(1028) != 1)",
        T + T + "return;",
        T + "if (g_pCoreShell)",
        T + T + "g_pCoreShell->OperationRequest(GOI_TASKGUIDE_GOTO_XAFU, 0, 0);",
    ])
    assert old in d, "TryGoXaFu anchor"
    new = NL.join([
        T + "// [3HD C20] tab Sat Thu: tu chay toi boss (xa phu -> qua map -> toa do)",
        T + "if (m_Entries[m_nCurEntry].nTaskId == TASKGUIDE_SATTHU_TASKID)",
        T + "{",
        T + T + "if (g_pCoreShell)",
        T + T + T + "g_pCoreShell->OperationRequest(GOI_TASKGUIDE_GOTO_SATTHU, 0, 0);",
        T + T + "return;",
        T + "}",
        T + "if (m_Entries[m_nCurEntry].nTaskId != TASKGUIDE_DATAU_TASKID)",
        T + T + "return;",
        T + "if (DTG_TaskVal(1021) != 4 || DTG_TaskVal(1028) != 1)",
        T + T + "return;",
        T + "if (g_pCoreShell)",
        T + T + "g_pCoreShell->OperationRequest(GOI_TASKGUIDE_GOTO_XAFU, 0, 0);",
    ])
    d = d.replace(old, new)
    # hint sau dong target C18
    a2 = NL.join([
        T + T + "sprintf(szLine, ST3_TARGET_FMT, s_szST3BossName[nBoss], s_szST3BossInfo[nBoss]);",
        T + T + "AddLine(szLine);",
    ])
    assert d.count(a2) == 1
    d = d.replace(a2, a2 + NL + T + T + "AddLine(ST3_HINT_GO);\t// [C20]")
    return d
rw(CU + r"\UiTaskGuide.cpp", f_ui)
print("xong C20 (client)")
