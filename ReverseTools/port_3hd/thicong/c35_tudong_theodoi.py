# -*- coding: utf-8 -*-
"""C35 - 2 yeu cau chu:
 (1) "Sat Thu Bi Bao dung thi KHONG XOA item": engine chi TU TRU item cho script
     nam trong danh sach sIsJx2ItemScript (KItemList.cpp) - quy uoc JX2 "main()
     tra != 1 thi engine tru 1 vat pham". shashou_mibao.lua CHUA co trong danh
     sach nen roi vao nhanh ExecuteScript thuong => khong tru.
     (chuangguanbaoxiang.lua TU ConsumeItem(6,1,3360) nen KHONG them - tranh tru 2 lan.)
 (2) "moi lan nhan nhiem vu thi khung theo doi TU HIEN, khoi bam nut Theo doi":
     them KUiTaskGuide::AutoTraceOnTask(nTaskId) chay o GameSpaceChangedNotify
     (chay CA KHI bang F11 dang dong): task 1082 (Sat Thu) / 1021 (Da Tau) chuyen
     tu KHONG CO -> CO nhiem vu thi tu bat theo doi + mo khung. Nguoi choi van
     tat duoc bang nut "Huy theo doi" (co s_abAutoDone chan bat lai lien tuc).
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
CU = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase"
UI = r"D:\GAMEDEVNEW\Sources\S3Client\Ui"
T = "\t"


def rw(p, fn):
    d = io.open(p, encoding="latin-1", newline="").read()
    d2 = fn(d)
    if d2 != d:
        io.open(p, "w", encoding="latin-1", newline="").write(d2)
        print("  OK", p.rsplit("\\", 1)[-1])
    else:
        print("  (khong doi)", p.rsplit("\\", 1)[-1])


# ---------- 1) KItemList.cpp: them shashou_mibao vao danh sach ----------
def f_item(d):
    if "shashou_mibao" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = T*2 + '"\\\\script\\\\missions\\\\tongcastle\\\\shenmuling.lua",'
    assert d.count(a) == 1, d.count(a)
    i = d.index(a)
    eol = d.index(NL, i) + len(NL)
    ins = (T*2 + '"\\\\script\\\\task\\\\tollgate\\\\killer\\\\shashou_mibao.lua",'
           + T + "// [3HD C35] Sat Thu Bi Bao: thanh cong tra nil -> engine tru 1 cai" + NL)
    return d[:eol] + ins + d[eol:]
rw(CORE + r"\KItemList.cpp", f_item)


# ---------- 2) UiTaskGuide.h: khai bao ----------
def f_h(d):
    if "AutoTraceOnTask" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = T + "static void				OnTaskValueChanged(int nTaskId);"
    assert d.count(a) == 1
    return d.replace(a, a + NL + T + "static void				AutoTraceOnTask(int nTaskId);			// [C35] nhan nhiem vu -> tu theo doi")
rw(CU + r"\UiTaskGuide.h", f_h)


# ---------- 3) UiTaskGuide.cpp: than ham ----------
def f_cpp(d):
    if "AutoTraceOnTask" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = "void KUiTaskGuide::RefreshButtons()"
    assert d.count(a) == 1
    body = NL.join([
        "// [C35] Tu BAT theo doi khi nguoi choi VUA NHAN nhiem vu (khoi phai bam nut).",
        "// Chay ca khi bang F11 dang DONG (goi tu GameSpaceChangedNotify). Chi tu bat",
        "// dung MOT LAN cho moi lan nhan: nguoi choi tat di thi khong bi bat lai, den",
        "// khi ho nhan nhiem vu MOI (task ve 0 roi co lai) thi mo lai.",
        "static bool s_abAutoDone[TRACE_MAX_TASKID] = { false };",
        "",
        "void KUiTaskGuide::AutoTraceOnTask(int nTaskId)",
        "{",
        T + "int nHe = -1;",
        T + "bool bCo = false;",
        T + "if (nTaskId == 1082)",
        T + "{",
        T + T + "nHe = TASKGUIDE_SATTHU_TASKID;",
        T + T + "int nB = DTG_TaskVal(1082);",
        T + T + "bCo = (nB >= 1 && nB <= ST3_BOSS_MAX);",
        T + "}",
        T + "else if (nTaskId == 1021 || nTaskId == 1028)",
        T + "{",
        T + T + "nHe = TASKGUIDE_DATAU_TASKID;",
        T + T + "bCo = (DTG_TaskVal(1021) != 0);",
        T + "}",
        T + "if (nHe < 0 || nHe >= TRACE_MAX_TASKID)",
        T + T + "return;",
        T + "if (!bCo)",
        T + "{",
        T + T + "s_abAutoDone[nHe] = false;\t// het nhiem vu -> cho phep tu bat lan sau",
        T + T + "return;",
        T + "}",
        T + "if (s_abAutoDone[nHe] || IsTracedTask(nHe))",
        T + T + "return;",
        T + "s_abAutoDone[nHe] = true;",
        T + "SetTracedTask(nHe, true);",
        T + "KUiTaskTrace::SetTraced(true);\t// mo khung neu dang dong",
        T + "RefreshButtons();",
        "}",
        "",
    ])
    return d.replace(a, body + a)
rw(CU + r"\UiTaskGuide.cpp", f_cpp)


# ---------- 4) GameSpaceChangedNotify.cpp: goi truoc 2 ham cu ----------
def f_no(d):
    if "AutoTraceOnTask" in d:
        return d
    NL = "\r\n" if "\r\n" in d else "\n"
    a = T*2 + "KUiTaskGuide::OnTaskValueChanged((int)uParam);"
    assert d.count(a) == 1
    return d.replace(a, T*2 + "KUiTaskGuide::AutoTraceOnTask((int)uParam);\t// [C35] nhan nhiem vu -> tu theo doi" + NL + a)
rw(UI + r"\GameSpaceChangedNotify.cpp", f_no)
print("xong C35 (nguon) - can build CoreServer + CoreClient + Game.exe")
