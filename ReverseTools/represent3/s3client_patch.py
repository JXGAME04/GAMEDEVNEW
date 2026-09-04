# -*- coding: utf-8 -*-
"""[REP3 03/09] S3Client.cpp (Game.exe) trong worktree D:\GAMEDEVNEW_wt_rep3:
 1. InitRepresentShell: bo hack keo cua so cao them 40 px; neu Represent3 nap/khoi tao that bai -> TU DONG lui ve Represent2.
 2. HandleInput: bo co toa do chuot y*40/808 (di kem hack tren).
 3. KNpc.h MAX_NPC client 256 -> 800 (theo client VLTK 2.0); KNpcSet.h MAX_NPC_REQUEST 20 -> 128.
Doc/ghi latin-1, giu kieu xuong dong cua tung tep."""
import io, re, os

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)
def nl(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
def sub1(text, pattern, repl, name):
    n = len(re.findall(pattern, text, re.S))
    assert n == 1, "%s: found %d" % (name, n)
    return re.sub(pattern, repl, text, count=1, flags=re.S)

MARK = "[REP3 03/09]"
ROOT = r"D:\GAMEDEVNEW_wt_rep3\Sources"

# ---------------- S3Client.cpp
p = os.path.join(ROOT, r"S3Client\S3Client.cpp")
s = rd(p); N = nl(s)
if MARK not in s:
    # 1a. bo khoi keo cua so +40 (tu 'if (g_bRepresent3 && !bFullScreen)' den truoc 'if (g_pRepresentShell->Create(')
    pat = r"\tif \(g_bRepresent3 && !bFullScreen\)" + N + r"\t\{" + N + r".*?" + N + r"\t\}" + N + r"(\s*if \(g_pRepresentShell->Create\(nWidth, nHeight, bFullScreen != 0\)\))"
    s = sub1(s, pat, lambda m: ("\t// %s bo hack keo cua so cao them 40 px (Represent3 tu dat cua so dung nWidth x nHeight, Present 1:1)" % MARK) + N + m.group(1), "window hack")
    # 1b. fallback ve Represent2 khi Represent3 that bai
    old_fail = (r"\telse" + N + r"\t\{" + N + r"\t\tError_SetErrorCode\(g_bRepresent3 \? ERR_T_REPRESENT3_INIT_FAILED : ERR_T_REPRESENT2_INIT_FAILED\);" + N + r"\t\treturn FALSE;" + N + r"\t\}")
    new_fail = ("\telse" + N + "\t{" + N +
        "\t\t// %s Represent3 khoi tao that bai (thieu d3d9/card yeu) -> lui ve Represent2, khong thoat game" % MARK + N +
        "\t\tif (g_bRepresent3)" + N + "\t\t{" + N +
        "\t\t\tAUTOLOG(\"[REP3] Represent3 Create that bai -> lui ve Represent2\");" + N +
        "\t\t\tg_pRepresentShell->Release();" + N + "\t\t\tg_pRepresentShell = NULL;" + N +
        "#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY" + N +
        "\t\t\tif (l_hRepresentModule) { FreeLibrary(l_hRepresentModule); l_hRepresentModule = NULL; }" + N +
        "#endif" + N +
        "\t\t\tg_bRepresent3 = false;" + N +
        "\t\t\treturn InitRepresentShell(bFullScreen, nWidth, nHeight);" + N +
        "\t\t}" + N +
        "\t\tError_SetErrorCode(ERR_T_REPRESENT2_INIT_FAILED);" + N + "\t\treturn FALSE;" + N + "\t}")
    s = sub1(s, old_fail, lambda m: new_fail, "create fail")
    # 1c. LoadLibrary that bai voi Represent3 -> lui ve Represent2
    old_load = (r"\t\tif \(l_hRepresentModule == NULL && \(l_hRepresentModule = LoadLibrary\(g_bRepresent3 \? REPRESENT_MODULE_3 : REPRESENT_MODULE_2\)\) == NULL\)" + N +
                r"\t\t\{" + N + r"\t\t\tError_SetErrorCode\(ERR_T_LOAD_MODULE_FAILED\);" + N + r"\t\t\treturn FALSE;" + N + r"\t\t\}")
    new_load = ("\t\tif (l_hRepresentModule == NULL && (l_hRepresentModule = LoadLibrary(g_bRepresent3 ? REPRESENT_MODULE_3 : REPRESENT_MODULE_2)) == NULL)" + N +
                "\t\t{" + N +
                "\t\t\tif (g_bRepresent3)\t// %s thieu Represent3.dll -> lui ve Represent2" % MARK + N +
                "\t\t\t{" + N + "\t\t\t\tAUTOLOG(\"[REP3] khong nap duoc Represent3.dll -> lui ve Represent2\");" + N +
                "\t\t\t\tg_bRepresent3 = false;" + N + "\t\t\t\treturn InitRepresentShell(bFullScreen, nWidth, nHeight);" + N + "\t\t\t}" + N +
                "\t\t\tError_SetErrorCode(ERR_T_LOAD_MODULE_FAILED);" + N + "\t\t\treturn FALSE;" + N + "\t\t}")
    s = sub1(s, old_load, lambda m: new_load, "load fail")
    # 2. HandleInput: bo co chuot
    old_mouse = (r"\t\tif \(g_bRepresent3 && !g_bScreen\)" + N + r"\t\t\{" + N +
                 r"\t\t\tif \(\(uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST && uMsg != WM_MOUSEWHEEL\) \|\| uMsg == WM_MOUSEHOVER\)" + N +
                 r"\t\t\t\{" + N + r".*?" + N + r"\t\t\t\}" + N + r"\t\t\}" + N)
    s = sub1(s, old_mouse, lambda m: ("\t\t// %s bo co toa do chuot y*40/808 (di kem hack cua so +40 da bo)" % MARK) + N, "mouse hack")
    assert "AUTOLOG(" in s.split("[REP3 03/09]")[0] or "AUTOLOG" in s, "AUTOLOG macro?"
    wr(p, s); print("S3Client.cpp OK")
else:
    print("S3Client.cpp da va")

# ---------------- KNpc.h : MAX_NPC client
p = os.path.join(ROOT, r"Core\Src\KNpc.h")
s = rd(p); N = nl(s)
if MARK not in s:
    s = sub1(s, r"#define\t\tMAX_NPC\t\t\t\t256\t\t\t\t//max npc tai client la 256",
             "#define\t\tMAX_NPC\t\t\t\t800\t\t\t\t// %s client 256 -> 800 (client VLTK 2.0 = 800): dong nguoi khong roi NPC" % MARK, "MAX_NPC")
    wr(p, s); print("KNpc.h OK")
else:
    print("KNpc.h da va")

# ---------------- KNpcSet.h : MAX_NPC_REQUEST
p = os.path.join(ROOT, r"Core\Src\KNpcSet.h")
s = rd(p); N = nl(s)
if MARK not in s:
    s = sub1(s, r"#define\t\tMAX_NPC_REQUEST\t20", "#define\t\tMAX_NPC_REQUEST\t128\t// %s 20 -> 128: Tong Kim can khe hoi NPC (KProtocolProcess.cpp ~2059)" % MARK, "MAX_NPC_REQUEST")
    wr(p, s); print("KNpcSet.h OK")
else:
    print("KNpcSet.h da va")
print("DONE")
