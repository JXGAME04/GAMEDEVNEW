# -*- coding: utf-8 -*-
"""[REP3 03/09 HIENTHI] Giao dien WAuto (tab Co ban) cho 2 tuy chon chong dong cua client:
   - NpcTheSame : nguoi choi KHAC mac chung 1 bo (KProtocolProcess.cpp REP3_NpcTheSame)
   - MissleIndex: moi chieu dung chung 1 hieu ung dan (KMissle.cpp REP3_MissleIndex)
 Duong di: WAuto (checkbox/edit -> autoData.bWANpcTheSame/bWAMissle/nWAMissleIndex, luu APdata\<ID>.dat nhu moi o khac)
   -> moi luot GAMELOOPINTV gui goi IPCHienThi (PRT_HIENTHI) cho MOI game dang ket noi (khong can tick auto)
   -> S3Client.cpp dispatch PRT_HIENTHI -> CoreShell ATYPE_HIENTHI ghi g_nWAOpt* + moc thoi gian
   -> REP3_NpcTheSame / REP3_MissleIndex dung gia tri WAuto khi con moi (< 5 s), het han thi ve [Client] config.ini.
 Tep sua: Core/Src/ipc_shared.h + E:\Src_Auto_Ngoai\WAuto\WAuto\ipc_shared.h (GIONG NHAU), CoreShell.h/.cpp, S3Client.cpp,
   KProtocolProcess.cpp, KMissle.cpp (worktree rep3-0309); WAuto: resource.h, WAuto.rc (UTF-16), WAuto.cpp (UTF-16).
 Luat: TCVN3/ASCII doc-ghi latin-1 newline=''; UTF-16 doc-ghi utf-16 newline=''; moi neo kiem dem == 1."""
import io, os, re, shutil, sys

MARK = "[REP3 03/09]"
SRC = r"D:\GAMEDEVNEW_wt_rep3\Sources"
CORE = SRC + r"\Core\Src"
WA = r"E:\Src_Auto_Ngoai\WAuto\WAuto"

def rd(p, enc="latin-1"):
    with io.open(p, "r", encoding=enc, newline="") as f: return f.read()
def wr(p, s, enc="latin-1"):
    with io.open(p, "w", encoding=enc, newline="") as f: f.write(s)
def nl_of(s):
    return "\r\n" if s.count("\r\n") >= s.count("\n") // 2 else "\n"
def rep1(s, old, new, name):
    n = s.count(old)
    assert n == 1, "%s: found %d (expected 1)" % (name, n)
    return s.replace(old, new)
def resub1(s, pat, repl, name):
    n = len(re.findall(pat, s, re.S))
    assert n == 1, "%s: found %d (expected 1)" % (name, n)
    return re.sub(pat, repl, s, count=1, flags=re.S)

# ---------------------------------------------------------------- 1. ipc_shared.h (2 ban)
def va_ipc(p):
    s = rd(p); N = nl_of(s)
    if "bWANpcTheSame" in s:
        print("  da va:", p); return
    s = resub1(s, r"(\tPRT_QUITGAME,[ \t]*\r?\n)(};)",
               lambda m: m.group(1) + "\tPRT_HIENTHI,\t// %s tuy chon HIEN THI (NpcTheSame / MissleIndex) tu tab Co ban - gui moi luot, khong can tick auto" % MARK + N + m.group(2), "enum PRT")
    s = resub1(s, r"(\tint\t\tnCTCapBinh;[^\r\n]*\r?\n)",
               lambda m: m.group(1) +
               "\t// == HIEN THI khi dong nguoi (03/09/2026) - PHAI o cuoi struct, truoc constructor (APdata .dat ghi NGUYEN struct) ==" + N +
               "\tint\t\tbWANpcTheSame;\t// nguoi choi KHAC mac chung 1 bo giap/non, khong phi phong (client: NpcTheSame)" + N +
               "\tint\t\tbWAMissle;\t\t// bat: moi chieu dung chung 1 hieu ung dan (client: MissleIndex)" + N +
               "\tint\t\tnWAMissleIndex;\t// dong trong bang missles lam hieu ung chung (mac dinh 1)" + N, "autoData fields")
    s = resub1(s, r"(\t\tnCTCapBinh = 5;\r?\n)",
               lambda m: m.group(1) + "\t\tbWANpcTheSame = 0;" + N + "\t\tbWAMissle = 0;" + N + "\t\tnWAMissleIndex = 1;" + N, "autoData ctor")
    s = resub1(s, r"(struct IPCGameLoop : public SharedState\r?\n\{\r?\n\tautoData setting;\r?\n\};\r?\n)",
               lambda m: m.group(1) + N +
               "// %s tuy chon hien thi tu WAuto - goi nho gui moi luot GAMELOOPINTV cho moi game dang ket noi" % MARK + N +
               "struct IPCHienThi : public SharedState" + N + "{" + N +
               "\tint bNpcTheSame;\t// 1 = nguoi choi khac mac chung 1 bo" + N +
               "\tint nMissleIndex;\t// >0 = dong missles dung chung cho moi chieu, 0 = tat" + N + "};" + N, "IPCHienThi")
    wr(p, s); print("  OK:", p)

# ---------------------------------------------------------------- 2. CoreShell.h / CoreShell.cpp
def va_coreshell():
    p = CORE + r"\CoreShell.h"; s = rd(p); N = nl_of(s)
    if "ATYPE_HIENTHI" not in s:
        s = resub1(s, r"(\tATYPE_CONGTHANH,[^\r\n]*\r?\n)(};)",
                   lambda m: m.group(1) + "\tATYPE_HIENTHI,\t\t// %s tuy chon hien thi tu WAuto (IPCHienThi: NpcTheSame / MissleIndex)" % MARK + N + m.group(2), "ATYPE enum")
        wr(p, s); print("  OK: CoreShell.h")
    else: print("  da va: CoreShell.h")
    p = CORE + r"\CoreShell.cpp"; s = rd(p); N = nl_of(s)
    if "g_nWAOptNpcTheSame" not in s:
        s = rep1(s, "static const autoData* s_pApDiDuong = NULL;" + N,
                 "static const autoData* s_pApDiDuong = NULL;" + N + N +
                 "// %s tuy chon HIEN THI tu WAuto (tab Co ban): WAuto gui IPCHienThi moi luot (khong can tick auto)." % MARK + N +
                 "// KProtocolProcess (NpcTheSame) va KMissle (MissleIndex) doc cac bien nay, chi tin khi con moi (< 5 s), het han ve config.ini." + N +
                 "int   g_nWAOptNpcTheSame  = 0;" + N +
                 "int   g_nWAOptMissleIndex = 0;" + N +
                 "DWORD g_dwWAOptTime       = 0;" + N, "globals")
        old = ("\t\t\t\tcase ATYPE_CONGTHANH:" + N + "\t\t\t\t{" + N +
               "\t\t\t\t\treturn CT_Process(nPlayerIdx, (const autoData*)nParam, uCurTime);" + N + "\t\t\t\t}" + N)
        new = old + N.join([
            "\t\t\t\tcase ATYPE_HIENTHI:\t// %s tuy chon hien thi tu WAuto (goi IPCHienThi)" % MARK,
            "\t\t\t\t{",
            "\t\t\t\t\tconst IPCHienThi* pHT = (const IPCHienThi*)nParam;",
            "\t\t\t\t\tif (pHT)",
            "\t\t\t\t\t{",
            "\t\t\t\t\t\tg_nWAOptNpcTheSame  = pHT->bNpcTheSame;",
            "\t\t\t\t\t\tg_nWAOptMissleIndex = pHT->nMissleIndex;",
            "\t\t\t\t\t\tg_dwWAOptTime       = GetTickCount();",
            "\t\t\t\t\t}",
            "\t\t\t\t\tnRet = 1;",
            "\t\t\t\t\tbreak;",
            "\t\t\t\t}",
            "",
        ])
        s = rep1(s, old, new, "case ATYPE_HIENTHI")
        wr(p, s); print("  OK: CoreShell.cpp")
    else: print("  da va: CoreShell.cpp")

# ---------------------------------------------------------------- 3. S3Client.cpp
def va_s3client():
    p = SRC + r"\S3Client\S3Client.cpp"; s = rd(p); N = nl_of(s)
    if "PRT_HIENTHI" in s:
        print("  da va: S3Client.cpp"); return
    old = "\t\t\tcase PRT_HIDEGAME:" + N
    new = N.join([
        "\t\t\tcase PRT_HIENTHI:\t// %s tuy chon hien thi tu WAuto (NpcTheSame / MissleIndex), khong can tick auto" % MARK,
        "\t\t\t{",
        "\t\t\t\tIPCHienThi* pHT = (IPCHienThi*)p;",
        "\t\t\t\tif(pHT->Size >= sizeof(IPCHienThi) && g_pCoreShell)",
        "\t\t\t\t\tg_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_HIENTHI, (int)pHT);",
        "\t\t\t}",
        "\t\t\tbreak;",
        "",
    ]) + old
    s = rep1(s, old, new, "case PRT_HIENTHI")
    wr(p, s); print("  OK: S3Client.cpp")

# ---------------------------------------------------------------- 4. KProtocolProcess.cpp / KMissle.cpp
def va_core_readers():
    p = CORE + r"\KProtocolProcess.cpp"; s = rd(p); N = nl_of(s)
    if "g_nWAOptNpcTheSame" not in s:
        s = rep1(s, "static void REP3_NpcTheSame(int nIdx)" + N,
                 "extern int   g_nWAOptNpcTheSame;\t// %s tu CoreShell.cpp (WAuto tab Co ban)" % MARK + N +
                 "extern DWORD g_dwWAOptTime;" + N +
                 "static void REP3_NpcTheSame(int nIdx)" + N, "extern NTS")
        s = rep1(s, "\tif (s_nOn <= 0 || nIdx <= 0 || nIdx >= MAX_NPC)" + N + "\t\treturn;" + N,
                 "\tint nOn = s_nOn;" + N +
                 "\tif (g_dwWAOptTime && (GetTickCount() - g_dwWAOptTime) < 5000)\t// %s WAuto dang gui tuy chon -> ghi de config.ini" % MARK + N +
                 "\t\tnOn = g_nWAOptNpcTheSame;" + N +
                 "\tif (nOn <= 0 || nIdx <= 0 || nIdx >= MAX_NPC)" + N + "\t\treturn;" + N, "NTS check")
        wr(p, s); print("  OK: KProtocolProcess.cpp")
    else: print("  da va: KProtocolProcess.cpp")
    p = CORE + r"\KMissle.cpp"; s = rd(p); N = nl_of(s)
    if "g_nWAOptMissleIndex" not in s:
        old = ("static int REP3_MissleIndex()" + N + "{" + N + "\tstatic int s_nIdx = -1;" + N + "\tif (s_nIdx < 0)" + N +
               "\t\ts_nIdx = (int)GetPrivateProfileIntA(\"Client\", \"MissleIndex\", 0, \".\\\\config.ini\");" + N + "\treturn s_nIdx;" + N + "}" + N)
        new = ("extern int   g_nWAOptMissleIndex;\t// %s tu CoreShell.cpp (WAuto tab Co ban)" % MARK + N +
               "extern DWORD g_dwWAOptTime;" + N +
               "static int REP3_MissleIndex()" + N + "{" + N + "\tstatic int s_nIdx = -1;" + N + "\tif (s_nIdx < 0)" + N +
               "\t\ts_nIdx = (int)GetPrivateProfileIntA(\"Client\", \"MissleIndex\", 0, \".\\\\config.ini\");" + N +
               "\tif (g_dwWAOptTime && (GetTickCount() - g_dwWAOptTime) < 5000)\t// %s WAuto dang gui tuy chon -> ghi de config.ini" % MARK + N +
               "\t\treturn g_nWAOptMissleIndex;" + N +
               "\treturn s_nIdx;" + N + "}" + N)
        s = rep1(s, old, new, "REP3_MissleIndex")
        wr(p, s); print("  OK: KMissle.cpp")
    else: print("  da va: KMissle.cpp")

# ---------------------------------------------------------------- 5. WAuto: resource.h / WAuto.rc / WAuto.cpp
def backup(p):
    b = p + ".cu_0309_truoc_hienthi"
    if not os.path.exists(b): shutil.copy2(p, b)

def va_resource():
    p = WA + r"\resource.h"; s = rd(p); N = nl_of(s)
    if "IDC_CHECKBOX_0_NTS" in s:
        print("  da va: resource.h"); return
    backup(p)
    s = resub1(s, r"(#define IDC_COMBO_15_CAP\s+627\r?\n)",
               lambda m: m.group(1) +
               "// == tab 0 Co ban, khoi HIEN THI khi dong nguoi (03/09/2026): ShowTab tab 0 hien them dai 628..631 ==" + N +
               "#define IDC_SEP_0C\t\t\t628" + N +
               "#define IDC_CHECKBOX_0_NTS\t629" + N +
               "#define IDC_CHECKBOX_0_MIS\t630" + N +
               "#define IDC_EDITOR_0_MIS\t631" + N, "resource ids")
    wr(p, s); print("  OK: resource.h")

def va_rc():
    p = WA + r"\WAuto.rc"; s = rd(p, "utf-16"); N = "\r\n"
    if "IDC_CHECKBOX_0_NTS" in s:
        print("  da va: WAuto.rc"); return
    backup(p)
    old = "COMBOBOX IDC_COMBO_15_CAP, 114, 257, 42, 70"
    i = s.index(old); j = s.index(N, i) + len(N)
    add = (u'\t\tCONTROL "", IDC_SEP_0C, "Static", SS_ETCHEDHORZ, 6, 334, 148, 1' + N +
           u'\t\tCONTROL "Ng\u01b0\u1eddi ch\u01a1i kh\u00e1c m\u1eb7c chung 1 b\u1ed9 (\u0111\u00f4ng ng\u01b0\u1eddi nh\u1eb9 m\u00e1y)", IDC_CHECKBOX_0_NTS, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 338, 152, 10' + N +
           u'\t\tCONTROL "M\u1ecdi chi\u00eau d\u00f9ng chung 1 hi\u1ec7u \u1ee9ng, d\u00f2ng:", IDC_CHECKBOX_0_MIS, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 350, 122, 10' + N +
           u'\t\tEDITTEXT IDC_EDITOR_0_MIS, 128, 350, 28, 10, ES_NUMBER | WS_TABSTOP' + N)
    assert s.count(old) == 1
    s = s[:j] + add + s[j:]
    wr(p, s, "utf-16"); print("  OK: WAuto.rc")

def va_wauto_cpp():
    p = WA + r"\WAuto.cpp"; s = rd(p, "utf-16"); N = "\r\n"
    if "bWANpcTheSame" in s:
        print("  da va: WAuto.cpp"); return
    backup(p)
    # (a) SaveRoleData: doc 3 o
    s = rep1(s, "\tgnode.apdata.nCTSoBinh = atoi(szRootPath);" + N,
             "\tgnode.apdata.nCTSoBinh = atoi(szRootPath);" + N +
             "\t// %s tab Co ban - hien thi khi dong nguoi" % MARK + N +
             "\tgnode.apdata.bWANpcTheSame = (IsDlgButtonChecked(hDlg, IDC_CHECKBOX_0_NTS) == BST_CHECKED);" + N +
             "\tgnode.apdata.bWAMissle = (IsDlgButtonChecked(hDlg, IDC_CHECKBOX_0_MIS) == BST_CHECKED);" + N +
             "\tGetDlgItemTextA(hDlg, IDC_EDITOR_0_MIS, szRootPath, MAX_PATH);" + N +
             "\tgnode.apdata.nWAMissleIndex = atoi(szRootPath);" + N +
             "\tif(gnode.apdata.nWAMissleIndex < 0) gnode.apdata.nWAMissleIndex = 0;" + N, "SaveRoleData")
    # (b) LoadRoleData -> UI
    old = "\t{ wchar_t wct[16]; _itow_s(gnode.apdata.nCTSoBinh, wct, 10); SetDlgItemText(hDlg, IDC_EDITOR_15_SB, wct); }" + N
    s = rep1(s, old, old +
             "\tCheckDlgButton(hDlg, IDC_CHECKBOX_0_NTS, gnode.apdata.bWANpcTheSame?BST_CHECKED:BST_UNCHECKED);\t// %s" % MARK + N +
             "\tCheckDlgButton(hDlg, IDC_CHECKBOX_0_MIS, gnode.apdata.bWAMissle?BST_CHECKED:BST_UNCHECKED);" + N +
             "\t{ wchar_t wct[16]; _itow_s(gnode.apdata.nWAMissleIndex, wct, 10); SetDlgItemText(hDlg, IDC_EDITOR_0_MIS, wct); }" + N, "LoadRoleData UI")
    # (c) mac dinh khi chua co tep (truoc khoi 'tab 10 Lien dau')
    old = "\t\t// tab 10 Lien dau: tinh nang mac dinh TAT;"
    s = rep1(s, old,
             "\t\tgnode.apdata.bWANpcTheSame = 0;\t// %s hien thi: mac dinh tat, dong missles 1" % MARK + N +
             "\t\tgnode.apdata.bWAMissle = 0;" + N +
             "\t\tgnode.apdata.nWAMissleIndex = 1;" + N + old, "defaults")
    # (d) kich hoat luu
    s = rep1(s, "\t\t\tcase IDC_CHECKBOX_15_MUA:" + N,
             "\t\t\tcase IDC_CHECKBOX_15_MUA:" + N + "\t\t\tcase IDC_CHECKBOX_0_NTS:\t// %s" % MARK + N + "\t\t\tcase IDC_CHECKBOX_0_MIS:" + N, "BN_CLICKED list")
    s = rep1(s, "\t\t\tcase IDC_EDITOR_15_SB:" + N,
             "\t\t\tcase IDC_EDITOR_15_SB:" + N + "\t\t\tcase IDC_EDITOR_0_MIS:\t// %s" % MARK + N, "EN_KILLFOCUS list")
    # (e) ShowTab tab 0: them dai 628..631
    old = ("\tif(nTabBtn == 0)" + N + "\t{" + N + "\t\tfor(i=IDC_STRING_0_L;i<=IDC_STRING_0_NE;++i)" + N + "\t\t{" + N +
           "\t\t\tShowWindow(::GetDlgItem( hDlg, i ), SW_SHOW);" + N + "\t\t}" + N + "\t}" + N)
    new = ("\tif(nTabBtn == 0)" + N + "\t{" + N + "\t\tfor(i=IDC_STRING_0_L;i<=IDC_STRING_0_NE;++i)" + N + "\t\t{" + N +
           "\t\t\tShowWindow(::GetDlgItem( hDlg, i ), SW_SHOW);" + N + "\t\t}" + N +
           "\t\tfor(i=IDC_SEP_0C;i<=IDC_EDITOR_0_MIS;++i)\t// %s khoi hien thi khi dong nguoi (dai 628..631)" % MARK + N + "\t\t{" + N +
           "\t\t\tShowWindow(::GetDlgItem( hDlg, i ), SW_SHOW);" + N + "\t\t}" + N + "\t}" + N)
    s = rep1(s, old, new, "ShowTab tab0")
    # (f) day tab 0: 330 -> 362
    s = rep1(s, "\t330, 279, 279, 304, 282, 260, 266, 303, 294, 268, 326, 328, 225, 335, 270",
             "\t362, 279, 279, 304, 282, 260, 266, 303, 294, 268, 326, 328, 225, 335, 270\t// %s tab 0: 330 -> 362 (them 2 hang hien thi)" % MARK, "s_aTabDay")
    # (g) tooltip
    old = u'\t{ IDC_COMBO_15_CAP, L"'
    i = s.index(old); j = s.index(N, i) + len(N)
    assert s.count(old) == 1
    tips = (u'\t{ IDC_CHECKBOX_0_NTS, L"M\u1ecdi ng\u01b0\u1eddi ch\u01a1i KH\u00c1C hi\u1ec7n c\u00f9ng m\u1ed9t b\u1ed9 gi\u00e1p/n\u00f3n, kh\u00f4ng phi phong \u2192 ch\u1ed7 \u0111\u00f4ng \u00edt ph\u1ea3i n\u1ea1p \u1ea3nh, \u0111\u1ee1 gi\u1eadt. \u00c1p cho ng\u01b0\u1eddi v\u1eeba xu\u1ea5t hi\u1ec7n; t\u1eaft th\u00ec ng\u01b0\u1eddi \u0111ang th\u1ea5y gi\u1eef nguy\u00ean t\u1edbi khi xu\u1ea5t hi\u1ec7n l\u1ea1i. Ch\u00ednh m\u00ecnh kh\u00f4ng \u0111\u1ed5i." },' + N +
            u'\t{ IDC_CHECKBOX_0_MIS, L"M\u1ecdi chi\u00eau th\u1ee9c d\u00f9ng chung m\u1ed9t hi\u1ec7u \u1ee9ng \u0111\u1ea1n (d\u00f2ng trong b\u1ea3ng missles) \u2192 ch\u1ed7 \u0111\u00f4ng nh\u1eb9 m\u00e1y. T\u1eaft l\u00e0 hi\u1ec7u \u1ee9ng b\u00ecnh th\u01b0\u1eddng ngay chi\u00eau k\u1ebf ti\u1ebfp." },' + N +
            u'\t{ IDC_EDITOR_0_MIS, L"S\u1ed1 d\u00f2ng trong b\u1ea3ng missles l\u1ea5y l\u00e0m hi\u1ec7u \u1ee9ng chung (d\u00f2ng ph\u1ea3i c\u00f3 AnimFile1; m\u1eb7c \u0111\u1ecbnh 1)." },' + N)
    s = s[:j] + tips + s[j:]
    # (h) gui IPCHienThi moi luot (sau khoi gui GAMELOOP, ngoai dieu kien tick)
    old = "\t\t\t\t\tSendCmdToGame(gnode, &gl, sizeof(IPCGameLoop));" + N + "\t\t\t\t}" + N
    new = old + N.join([
        "\t\t\t\t{\t// %s tuy chon HIEN THI (tab Co ban): gui moi luot cho moi game dang ket noi, KHONG can tick auto" % MARK,
        "\t\t\t\t\tIPCHienThi ht;",
        "\t\t\t\t\tht.CmdID = PRT_HIENTHI;",
        "\t\t\t\t\tht.Size = sizeof(IPCHienThi);",
        "\t\t\t\t\tht.bNpcTheSame = gnode.apdata.bWANpcTheSame;",
        "\t\t\t\t\tht.nMissleIndex = gnode.apdata.bWAMissle ? gnode.apdata.nWAMissleIndex : 0;",
        "\t\t\t\t\tSendCmdToGame(gnode, &ht, sizeof(IPCHienThi));",
        "\t\t\t\t}",
        "",
    ])
    s = rep1(s, old, new, "send IPCHienThi")
    wr(p, s, "utf-16"); print("  OK: WAuto.cpp")

if __name__ == "__main__":
    print("== ipc_shared.h"); va_ipc(CORE + r"\ipc_shared.h"); va_ipc(WA + r"\ipc_shared.h")
    print("== CoreShell"); va_coreshell()
    print("== S3Client"); va_s3client()
    print("== readers"); va_core_readers()
    print("== WAuto"); va_resource(); va_rc(); va_wauto_cpp()
    # kiem 2 ban ipc_shared.h giong nhau (bo qua CR)
    a = rd(CORE + r"\ipc_shared.h").replace("\r", ""); b = rd(WA + r"\ipc_shared.h").replace("\r", "")
    print("ipc_shared.h 2 ban giong nhau (bo CR):", a == b)
    print("DONE")
