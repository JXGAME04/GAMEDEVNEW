# -*- coding: utf-8 -*-
"""C18 - F11 cho San Boss Sat Thu (3 bao cao cua chu 13:2x):
  a) Hien MUC TIEU: ten boss + noi o (bang nuong tu killer.txt - co toa do).
  b) 3 nut (Bo nhiem vu / Theo doi / Huy theo doi) hoat dong cho tab Sat Thu
     (truoc chi Da Tau): Quit gui "st3_quit" qua kenh uicmdscript type 6 ->
     server ExecuteScript nieshichen.lua cancel() (dung ham /cancel cua thoai);
     Trace dung khung KUiTaskTrace chung, nho tab nao dang theo doi.
  c) Khung theo doi hien dong rut gon dung he dang theo doi.
Chua lam: tu chay toi xa phu / qua map / toi toa do boss (tinh nang auto moi,
can chot tuyen xe + co che di lien map - de dot rieng, xem BANGIAO).
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

CU = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase"
CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
T = "\t"


def rw(p, fn):
    d = io.open(p, encoding="latin-1", newline="").read()
    d2 = fn(d)
    if d2 != d:
        io.open(p, "w", encoding="latin-1", newline="").write(d2)
        print("  OK", p.rsplit("\\", 1)[-1])
    else:
        print("  (khong doi)", p.rsplit("\\", 1)[-1])


# ---------- 1) UiTaskGuideStr.h: chuoi moi ----------
def f_str(d):
    if "ST3_TARGET_FMT" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    blk = NL.join([
        "// [3HD 25/08 C18] muc tieu boss + khung theo doi cua San Boss Sat Thu",
        '#define ST3_TARGET_FMT "' + V("Mục tiêu:") + ' <color=yellow>%s <color>%s."',
        '#define ST3_TRACE_TITLE "<color=yellow>' + V("Săn Boss Sát Thủ") + ' <color>"',
        '#define ST3_BRIEF_FMT "%s - %s. ' + V("Giết:") + ' %d/8."',
        '#define ST3_BRIEF_NONE "' + V("Chưa nhận nhiệm vụ sát thủ.") + '"',
        "",
    ])
    return d.rstrip("\r\n") + NL + blk
rw(CU + r"\UiTaskGuideStr.h", f_str)


# ---------- 2) UiTaskGuide.h: khai bao ----------
def f_h(d):
    if "GetTracedTaskId" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = T + "static void				BuildBriefLine(char* pOut, int nSize);"
    assert d.count(a) == 1
    return d.replace(a, a + NL + NL.join([
        T + "static void				BuildTraceLine(char* pOut, int nSize);	// [C18] theo tab dang theo doi",
        T + "static int				GetTracedTaskId();						// [C18]",
    ]))
rw(CU + r"\UiTaskGuide.h", f_h)


# ---------- 3) UiTaskGuide.cpp ----------
def f_cpp(d):
    if "st3_quit" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    # 3a. include bang boss
    a = '#include "UiTaskGuideStr.h"'
    assert d.count(a) == 1
    d = d.replace(a, a + NL + '#include "UiTaskGuideSatThuBoss.h"	// [C18] SINH TU DONG tu killer.txt')
    # 3b. bien nho tab dang theo doi + accessor (dat truoc UpdateButtons)
    a2 = "void KUiTaskGuide::UpdateButtons()"
    assert d.count(a2) == 1
    d = d.replace(a2, NL.join([
        "// [C18] tab dang duoc 'Theo doi' (mac dinh Da Tau de giu hanh vi cu)",
        "static int s_nTracedTaskId = TASKGUIDE_DATAU_TASKID;",
        "int KUiTaskGuide::GetTracedTaskId()",
        "{",
        T + "return s_nTracedTaskId;",
        "}",
        "",
        a2,
    ]))
    # 3c. UpdateButtons: mo cho SATTHU
    old_btn = NL.join([
        T + "bool bDT = (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount &&",
        T + T + "m_Entries[m_nCurEntry].nTaskId == TASKGUIDE_DATAU_TASKID);",
        T + "int nCourse = bDT ? DTG_TaskVal(1028) : 0;",
        T + "m_BtnQuit.Enable(bDT && nCourse == 1);",
        T + "m_BtnTrace.Enable(bDT && !KUiTaskTrace::IsTraced());",
        T + "m_BtnCancelTrace.Enable(bDT && KUiTaskTrace::IsTraced());",
    ])
    assert old_btn in d, "khong thay UpdateButtons cu"
    new_btn = NL.join([
        T + "bool bDT = (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount &&",
        T + T + "m_Entries[m_nCurEntry].nTaskId == TASKGUIDE_DATAU_TASKID);",
        T + "// [C18] tab San Boss Sat Thu cung dung duoc 3 nut nhu Da Tau",
        T + "bool bST = (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount &&",
        T + T + "m_Entries[m_nCurEntry].nTaskId == TASKGUIDE_SATTHU_TASKID);",
        T + "int nCourse = bDT ? DTG_TaskVal(1028) : 0;",
        T + "int nBossST = bST ? DTG_TaskVal(1082) : 0;",
        T + "m_BtnQuit.Enable((bDT && nCourse == 1) || (bST && nBossST >= 1 && nBossST <= ST3_BOSS_MAX));",
        T + "m_BtnTrace.Enable((bDT || bST) && !KUiTaskTrace::IsTraced());",
        T + "m_BtnCancelTrace.Enable((bDT || bST) && KUiTaskTrace::IsTraced());",
    ])
    d = d.replace(old_btn, new_btn)
    # 3d. BtnQuit: phan nhanh st3_quit
    old_q = NL.join([
        T + T + T + "// server mo hop xac nhan huy chuan (Task_CancelConfirm) - du luat",
        T + T + T + "if (g_pCoreShell)",
        T + T + T + T + "g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 6, (int)\"tg_quit\");",
    ])
    assert old_q in d, "khong thay BtnQuit cu"
    new_q = NL.join([
        T + T + T + "// server mo hop xac nhan huy chuan (Da Tau) / huy truc tiep (Sat Thu)",
        T + T + T + "if (g_pCoreShell)",
        T + T + T + "{",
        T + T + T + T + "const char* pCmd = \"tg_quit\";",
        T + T + T + T + "if (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount &&",
        T + T + T + T + T + "m_Entries[m_nCurEntry].nTaskId == TASKGUIDE_SATTHU_TASKID)",
        T + T + T + T + T + "pCmd = \"st3_quit\";	// [C18] -> nieshichen.lua cancel()",
        T + T + T + T + "g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 6, (int)pCmd);",
        T + T + T + "}",
    ])
    d = d.replace(old_q, new_q)
    # 3e. BtnTrace: ghi nho tab
    old_t = NL.join([
        T + T + "if (uParam == (unsigned int)(KWndWindow*)&m_BtnTrace)",
        T + T + "{",
        T + T + T + "KUiTaskTrace::SetTraced(true);",
    ])
    assert old_t in d
    new_t = NL.join([
        T + T + "if (uParam == (unsigned int)(KWndWindow*)&m_BtnTrace)",
        T + T + "{",
        T + T + T + "if (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount)",
        T + T + T + T + "s_nTracedTaskId = m_Entries[m_nCurEntry].nTaskId;	// [C18]",
        T + T + T + "KUiTaskTrace::SetTraced(true);",
    ])
    d = d.replace(old_t, new_t)
    # 3f. BuildSatThuText: them dong muc tieu sau dong ST3_CUR_FMT
    old_c = NL.join([
        T + T + "sprintf(szLine, ST3_CUR_FMT, nBoss, nCapNhom);",
        T + T + "AddLine(szLine);",
    ])
    assert d.count(old_c) == 1, "khong thay ST3_CUR_FMT"
    new_c = NL.join([
        T + T + "sprintf(szLine, ST3_CUR_FMT, nBoss, nCapNhom);",
        T + T + "AddLine(szLine);",
        T + T + "// [C18] muc tieu: ten + noi o (co toa do) tu bang nuong killer.txt",
        T + T + "sprintf(szLine, ST3_TARGET_FMT, s_szST3BossName[nBoss], s_szST3BossInfo[nBoss]);",
        T + T + "AddLine(szLine);",
    ])
    d = d.replace(old_c, new_c)
    # 3g. BuildTraceLine dispatch (dat sau BuildBriefLine - truoc ham DTG_FixTagAfterVn)
    a3 = "// [FIX 24/08 v2] TEncodeText (Engine\\Src\\Text.cpp:468)"
    i = d.index(a3)
    blk = NL.join([
        "// [C18] dong rut gon cho khung Theo doi - theo tab da bam 'Theo doi'.",
        "void KUiTaskGuide::BuildTraceLine(char* pOut, int nSize)",
        "{",
        T + "if (s_nTracedTaskId == TASKGUIDE_SATTHU_TASKID)",
        T + "{",
        T + T + "int nBoss = DTG_TaskVal(1082);",
        T + T + "if (nBoss >= 1 && nBoss <= ST3_BOSS_MAX)",
        T + T + "{",
        T + T + T + "int nUsed = DTG_TaskVal(1193);",
        T + T + T + "if (nUsed < 0) nUsed = 0;",
        T + T + T + "_snprintf(pOut, nSize - 1, ST3_BRIEF_FMT, s_szST3BossName[nBoss], s_szST3BossInfo[nBoss], nUsed);",
        T + T + T + "pOut[nSize - 1] = 0;",
        T + T + "}",
        T + T + "else",
        T + T + "{",
        T + T + T + "strncpy(pOut, ST3_BRIEF_NONE, nSize - 1);",
        T + T + T + "pOut[nSize - 1] = 0;",
        T + T + "}",
        T + T + "return;",
        T + "}",
        T + "BuildBriefLine(pOut, nSize);",
        "}",
        "",
    ])
    d = d[:i] + blk + d[i:]
    return d
rw(CU + r"\UiTaskGuide.cpp", f_cpp)


# ---------- 4) UiTaskTrace.cpp: tieu de + noi dung theo tab ----------
def f_trace(d):
    if "GetTracedTaskId" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    old = NL.join([
        T + "// dong 1: ten nhiem vu (mau vang)",
        T + "strncpy(szLine, DTG_TRACE_TITLE, sizeof(szLine) - 1);",
    ])
    assert old in d
    new = NL.join([
        T + "// dong 1: ten nhiem vu (mau vang) - theo tab dang theo doi [C18]",
        T + "const char* pTitle = (KUiTaskGuide::GetTracedTaskId() == 10 /*SATTHU*/)",
        T + T + "? ST3_TRACE_TITLE : DTG_TRACE_TITLE;",
        T + "strncpy(szLine, pTitle, sizeof(szLine) - 1);",
    ])
    d = d.replace(old, new)
    old2 = T + "KUiTaskGuide::BuildBriefLine(szLine, sizeof(szLine));"
    assert d.count(old2) == 1
    d = d.replace(old2, T + "KUiTaskGuide::BuildTraceLine(szLine, sizeof(szLine));	// [C18]")
    return d
rw(CU + r"\UiTaskTrace.cpp", f_trace)


# ---------- 5) KProtocolProcess.cpp: st3_quit ----------
def f_proto(d):
    if "st3_quit" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    old = NL.join([
        T + T + T + "if (!strcmp(pUiCmd->szFunc, \"tg_quit\"))",
        T + T + T + "{",
        T + T + T + T + "Player[nIndex].ExecuteScript(\"\\\\script\\\\global\\\\seasonnpc.lua\", \"tg_quit\", 0);",
        T + T + T + "}",
    ])
    assert old in d, "khong thay tg_quit"
    new = old + NL + NL.join([
        T + T + T + "else if (!strcmp(pUiCmd->szFunc, \"st3_quit\"))",
        T + T + T + "{",
        T + T + T + T + "// [3HD C18] nut Bo nhiem vu tab San Boss Sat Thu: dung ham /cancel",
        T + T + T + T + "// cua chinh thoai nieshichen (huy nhiem vu dang nhan, giu luat goc)",
        T + T + T + T + "Player[nIndex].ExecuteScript(\"\\\\script\\\\task\\\\tollgate\\\\killer\\\\nieshichen.lua\", \"cancel\", 0);",
        T + T + T + "}",
    ])
    d = d.replace(old, new)
    return d
rw(CORE + r"\KProtocolProcess.cpp", f_proto)
print("xong C18 (nguon) - build Game.exe + CoreServer + CoreClient")
