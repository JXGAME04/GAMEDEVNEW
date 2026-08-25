# -*- coding: utf-8 -*-
"""C33 - 2 YEU CAU CHU ve khung "Theo doi nhiem vu":
 (1) "chi theo doi 1 lan 1 loai nhiem vu, ban Linux theo doi duoc TOAN BO va co
     nut truot xuong" => doi co DUY NHAT s_nTracedTaskId thanh TAP HOP nhieu he
     (mang co s_abTraced[]); khung liet ke MOI he dang theo doi, moi he 2 dong
     (ten he + ban rut gon). Khung von da dung KScrollMessageListBox nen CO SAN
     thanh cuon - chi can do nhieu dong va noi capability.
 (2) "kich vao khung theo doi thi TU DI CHUYEN, khong can mo bang Chi nam" =>
     bat WND_N_LIST_ITEM_SEL trong KUiTaskTrace, tra dong -> he -> goi thang
     GOI_TASKGUIDE_GOTO_SATTHU / GOI_TASKGUIDE_GOTO_XAFU.
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")

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


# ---------- 1) UiTaskGuide.h ----------
def f_h(d):
    if "IsTracedTask" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = T + "static int				GetTracedTaskId();						// [C18]"
    assert d.count(a) == 1
    return d.replace(a, NL.join([
        a,
        T + "static bool				IsTracedTask(int nTaskId);				// [C33] he nay dang theo doi?",
        T + "static void				SetTracedTask(int nTaskId, bool bOn);	// [C33]",
        T + "static int				GetTracedList(int* pOut, int nMax);		// [C33] danh sach he dang theo doi",
        T + "static void				BuildTraceLineOf(int nTaskId, char* pOut, int nSize);	// [C33]",
        T + "static const char*		GetTaskTitle(int nTaskId);				// [C33] ten he (mau vang)",
    ]))
rw(CU + r"\UiTaskGuide.h", f_h)


# ---------- 2) UiTaskGuide.cpp ----------
def f_cpp(d):
    if "s_abTraced" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    # 2a. thay bien don bang mang co
    old = NL.join([
        "// [C18] tab dang duoc 'Theo doi' (mac dinh Da Tau de giu hanh vi cu)",
        "static int s_nTracedTaskId = TASKGUIDE_DATAU_TASKID;",
        "int KUiTaskGuide::GetTracedTaskId()",
        "{",
        T + "return s_nTracedTaskId;",
        "}",
    ])
    assert old in d, "anchor s_nTracedTaskId"
    new = NL.join([
        "// [C33] CAC he dang duoc 'Theo doi' (truoc day chi giu DUY NHAT 1 he).",
        "// Chi so = TaskId (6..15). Mac dinh khong theo doi he nao - nguoi choi tu bam.",
        "#define TRACE_MAX_TASKID 16",
        "static bool s_abTraced[TRACE_MAX_TASKID] = { false };",
        "",
        "bool KUiTaskGuide::IsTracedTask(int nTaskId)",
        "{",
        T + "if (nTaskId < 0 || nTaskId >= TRACE_MAX_TASKID)",
        T + T + "return false;",
        T + "return s_abTraced[nTaskId];",
        "}",
        "",
        "void KUiTaskGuide::SetTracedTask(int nTaskId, bool bOn)",
        "{",
        T + "if (nTaskId < 0 || nTaskId >= TRACE_MAX_TASKID)",
        T + T + "return;",
        T + "s_abTraced[nTaskId] = bOn;",
        "}",
        "",
        "int KUiTaskGuide::GetTracedList(int* pOut, int nMax)",
        "{",
        T + "int n = 0;",
        T + "for (int i = 0; i < TRACE_MAX_TASKID && n < nMax; i++)",
        T + T + "if (s_abTraced[i])",
        T + T + T + "pOut[n++] = i;",
        T + "return n;",
        "}",
        "",
        "// he dau tien dang theo doi (giu tuong thich cho ma cu)",
        "int KUiTaskGuide::GetTracedTaskId()",
        "{",
        T + "for (int i = 0; i < TRACE_MAX_TASKID; i++)",
        T + T + "if (s_abTraced[i])",
        T + T + T + "return i;",
        T + "return TASKGUIDE_DATAU_TASKID;",
        "}",
        "",
        "const char* KUiTaskGuide::GetTaskTitle(int nTaskId)",
        "{",
        T + "if (nTaskId == TASKGUIDE_SATTHU_TASKID)",
        T + T + "return ST3_TRACE_TITLE;",
        T + "return DTG_TRACE_TITLE;",
        "}",
        "",
        "// ban rut gon cua MOT he bat ky (khung theo doi liet ke nhieu he)",
        "void KUiTaskGuide::BuildTraceLineOf(int nTaskId, char* pOut, int nSize)",
        "{",
        T + "if (nTaskId == TASKGUIDE_SATTHU_TASKID)",
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
        T + "BuildBriefLine(pOut, nSize);\t// mac dinh: Da Tau",
        "}",
    ])
    d = d.replace(old, new)
    # 2b. BuildTraceLine cu -> dung he dau tien (giu nguyen API)
    old2 = T + "if (s_nTracedTaskId == TASKGUIDE_SATTHU_TASKID)"
    assert d.count(old2) == 1
    d = d.replace(old2, T + "if (GetTracedTaskId() == TASKGUIDE_SATTHU_TASKID)")
    # 2c. UpdateButtons: theo TRANG THAI TUNG HE
    oldb = NL.join([
        T + "m_BtnTrace.Enable((bDT || bST) && !KUiTaskTrace::IsTraced());",
        T + "m_BtnCancelTrace.Enable((bDT || bST) && KUiTaskTrace::IsTraced());",
    ])
    assert oldb in d, "anchor UpdateButtons"
    newb = NL.join([
        T + "// [C33] moi he theo doi doc lap: bat 'Theo doi' khi he DANG CHON chua theo doi",
        T + "int nCurTask = (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount) ? m_Entries[m_nCurEntry].nTaskId : -1;",
        T + "bool bCoBan = (bDT || bST);\t// he da co ban rut gon de hien trong khung",
        T + "m_BtnTrace.Enable(bCoBan && !IsTracedTask(nCurTask));",
        T + "m_BtnCancelTrace.Enable(bCoBan && IsTracedTask(nCurTask));",
    ])
    d = d.replace(oldb, newb)
    # 2d. nut Theo doi / Huy theo doi
    oldt = NL.join([
        T + T + T + "if (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount)",
        T + T + T + T + "s_nTracedTaskId = m_Entries[m_nCurEntry].nTaskId;	// [C18]",
        T + T + T + "KUiTaskTrace::SetTraced(true);",
    ])
    assert oldt in d, "anchor BtnTrace"
    newt = NL.join([
        T + T + T + "if (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount)",
        T + T + T + T + "SetTracedTask(m_Entries[m_nCurEntry].nTaskId, true);\t// [C33] them he",
        T + T + T + "KUiTaskTrace::SetTraced(true);",
    ])
    d = d.replace(oldt, newt)
    oldc = NL.join([
        T + T + "if (uParam == (unsigned int)(KWndWindow*)&m_BtnCancelTrace)",
        T + T + "{",
        T + T + T + "KUiTaskTrace::SetTraced(false);",
    ])
    assert oldc in d, "anchor BtnCancelTrace"
    newc = NL.join([
        T + T + "if (uParam == (unsigned int)(KWndWindow*)&m_BtnCancelTrace)",
        T + T + "{",
        T + T + T + "// [C33] chi bo HE DANG CHON; con he khac thi khung van mo",
        T + T + T + "if (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount)",
        T + T + T + T + "SetTracedTask(m_Entries[m_nCurEntry].nTaskId, false);",
        T + T + T + "{",
        T + T + T + T + "int anTmp[TRACE_MAX_TASKID];",
        T + T + T + T + "if (GetTracedList(anTmp, TRACE_MAX_TASKID) > 0)",
        T + T + T + T + T + "KUiTaskTrace::OnTaskValueChanged(0);\t// con he khac -> chi ve lai",
        T + T + T + T + "else",
        T + T + T + T + T + "KUiTaskTrace::SetTraced(false);",
        T + T + T + "}",
    ])
    d = d.replace(oldc, newc)
    return d
rw(CU + r"\UiTaskGuide.cpp", f_cpp)


# ---------- 3) UiTaskTrace.cpp: liet ke nhieu he + click de di ----------
def f_trace(d):
    if "m_anLineTask" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    old = d[d.index("void KUiTaskTrace::UpdateView()"):]
    old = old[:old.index(NL + "}") + len(NL) + 1]
    new = NL.join([
        "void KUiTaskTrace::UpdateView()",
        "{",
        T + "KWndMessageListBox* pList = m_List.GetMessageListBox();",
        T + "pList->Clear();",
        T + "m_nLineCount = 0;",
        "",
        T + "// [C33] liet ke MOI he dang theo doi (khung dung KScrollMessageListBox nen",
        T + "// co san thanh cuon); moi he 2 dong: ten he (vang) + ban rut gon.",
        T + "int anTask[16];",
        T + "int nTask = KUiTaskGuide::GetTracedList(anTask, 16);",
        T + "pList->SetCapability(nTask > 0 ? nTask * 2 + 2 : 4);",
        "",
        T + "char szLine[512];",
        T + "for (int i = 0; i < nTask; i++)",
        T + "{",
        T + T + "strncpy(szLine, KUiTaskGuide::GetTaskTitle(anTask[i]), sizeof(szLine) - 1);",
        T + T + "szLine[sizeof(szLine) - 1] = 0;",
        T + T + "int nLen = TEncodeText(szLine, strlen(szLine));",
        T + T + "pList->AddOneMessage(szLine, nLen);",
        T + T + "if (m_nLineCount < 32) m_anLineTask[m_nLineCount++] = anTask[i];",
        "",
        T + T + "KUiTaskGuide::BuildTraceLineOf(anTask[i], szLine, sizeof(szLine));",
        T + T + "if (szLine[0])",
        T + T + "{",
        T + T + T + "nLen = TEncodeText(szLine, strlen(szLine));",
        T + T + T + "pList->AddOneMessage(szLine, nLen);",
        T + T + T + "if (m_nLineCount < 32) m_anLineTask[m_nLineCount++] = anTask[i];",
        T + T + "}",
        T + "}",
        "}",
        "",
    ])
    d = d.replace(old, new)
    # WndProc: bat click dong -> tu di chuyen
    if "WND_N_LIST_ITEM_SEL" not in d:
        i = d.index("void KUiTaskTrace::Initialize()")
        blk = NL.join([
            "// [C33] bam vao mot dong trong khung theo doi = TU DI CHUYEN toi muc tieu",
            "// cua he do (khong can mo bang Chi nam nhiem vu nua).",
            "int KUiTaskTrace::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)",
            "{",
            T + "if (uMsg == WND_N_LIST_ITEM_SEL && uParam == (unsigned int)(KWndWindow*)&m_List)",
            T + "{",
            T + T + "int nSel = m_List.GetMessageListBox()->GetCurSel();",
            T + T + "if (nSel >= 0 && nSel < m_nLineCount && g_pCoreShell)",
            T + T + "{",
            T + T + T + "int nTask = m_anLineTask[nSel];",
            T + T + T + "if (nTask == 10 /*SATTHU*/)",
            T + T + T + T + "g_pCoreShell->OperationRequest(GOI_TASKGUIDE_GOTO_SATTHU, 0, 0);",
            T + T + T + "else if (nTask == 6 /*DA TAU*/)",
            T + T + T + T + "g_pCoreShell->OperationRequest(GOI_TASKGUIDE_GOTO_XAFU, 0, 0);",
            T + T + "}",
            T + T + "return true;",
            T + "}",
            T + "return KWndShowAnimate::WndProc(uMsg, uParam, nParam);",
            "}",
            "",
        ])
        d = d[:i] + blk + d[i:]
    return d
rw(CU + r"\UiTaskTrace.cpp", f_trace)


# ---------- 4) UiTaskTrace.h: bien moi + khai bao WndProc ----------
def f_th(d):
    if "m_anLineTask" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = T + "KScrollMessageListBox	m_List;		// [TaskInfo] + _List + _Scroll"
    assert d.count(a) == 1
    return d.replace(a, NL.join([
        a,
        T + "// [C33] dong thu i thuoc he nao (de bam vao dong la tu di chuyen)",
        T + "int						m_anLineTask[32];",
        T + "int						m_nLineCount;",
    ]))
rw(CU + r"\UiTaskTrace.h", f_th)
print("xong C33 (nguon)")
