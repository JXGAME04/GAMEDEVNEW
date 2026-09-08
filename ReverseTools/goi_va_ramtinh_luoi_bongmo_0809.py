# -*- coding: utf-8 -*-
"""goi_va_ramtinh_luoi_bongmo_0809.py - [RAMTINH 08/09] CAT MANG TINH CoreClient (chi client, khong doi giao thuc):
 (1) Luoi tim duong: VGridNode m_GridNode[MAX_CELL] (2.400.000 x 20 B = 48 MB) + int m_pTempCover[MAX_CELL] (9,6 MB) nam trong .bss
     CoreClient -> cap phat HEAP dung co luoi ban do dang dung (CapLuoi), tha m_pTempCover ngay sau khi dung luoi, tha ca luoi khi huy.
     Do bo cuc lop (sizeof(KSubWorld) 57,6 MB -> ~0,3 MB): KSubWorld.h/.cpp.
 (2) Bong mo NPC: KNpcBlur m_cNpcBlur (23.708 B, chi dung khi luot) nam trong moi KNpc (800 x 23,7 KB = 19 MB) -> con tro cap phat lan dau
     NPC luot (CapBongMo), tha khi NPC bi go (Remove). KNpcRes.h/.cpp, CoreDrawGameObj.cpp.
 Doi bo cuc KNpc/KSubWorld -> phai build lai CA Game.exe (S3Client truy cap Npc[]/SubWorld[]). Tep latin-1 CRLF."""
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

# ======================= (1) LUOI TIM DUONG =======================
p, s = load("KSubWorld.h"); h0 = hi(s)
if "m_nGridCellCap" not in s:
    s = rep(s, "\tVGridNode\tm_GridNode[MAX_CELL];\r\n",
        "\tVGridNode*\tm_GridNode;\t\t// [RAMTINH 08/09] cap phat HEAP dung co luoi ban do dang dung (CapLuoi); truoc: mang tinh MAX_CELL = 48 MB .bss\r\n"
        "\tint\t\t\tm_nGridCellCap;\t// [RAMTINH 08/09] so o da cap cho m_GridNode\r\n")
    s = rep(s, "\tint\t\tm_pTempCover[MAX_CELL];\r\n",
        "\tint*\t\tm_pTempCover;\t\t// [RAMTINH 08/09] chi dung luc dung luoi (ProcLoadPathGrid), tha ngay sau (ThaTempCover); truoc: 9,6 MB .bss\r\n")
    s = rep(s, "\tBOOL\tm_bHavePath;\r\n\tBOOL\tm_uPaintTime;\r\n#endif\r\n};\r\n",
        "\tBOOL\tm_bHavePath;\r\n\tBOOL\tm_uPaintTime;\r\n"
        "\tBOOL\t\tCapLuoi(int nAllCell);\t// [RAMTINH 08/09] cap m_GridNode + m_pTempCover dung nAllCell o (cap lai neu khac co), FALSE = het bo nho\r\n"
        "\tvoid\t\tThaLuoi();\t\t\t// tha ca hai + m_bHavePath = FALSE\r\n"
        "\tvoid\t\tThaTempCover();\t\t// tha rieng m_pTempCover sau khi dung luoi xong\r\n"
        "#endif\r\n};\r\n")
    # inline LuoiOCoDiDuoc: bao ve con tro NULL
    old = "\t\tif (m_hLoadPathGrid && WaitForSingleObject(m_hLoadPathGrid, 0) == WAIT_TIMEOUT)\r\n\t\t\treturn -1;\r\n\t\tconst int nW = m_nGridW * REGION_GRID_WIDTH;\r\n"
    s = rep(s, old, "\t\tif (m_hLoadPathGrid && WaitForSingleObject(m_hLoadPathGrid, 0) == WAIT_TIMEOUT)\r\n\t\t\treturn -1;\r\n\t\tif (!m_GridNode)\r\n\t\t\treturn -1;\t// [RAMTINH 08/09] luoi chua cap\r\n\t\tconst int nW = m_nGridW * REGION_GRID_WIDTH;\r\n")
save(p, s, h0); print("OK KSubWorld.h")

p, s = load("KSubWorld.cpp"); h0 = hi(s)
if "CapLuoi" not in s:
    s = rep(s, "#ifndef _SERVER\r\n\tm_hLoadPathGrid = NULL;\r\n    m_bStopThread = FALSE;\r\n",
        "#ifndef _SERVER\r\n\tm_hLoadPathGrid = NULL;\r\n\tm_GridNode = NULL; m_pTempCover = NULL; m_nGridCellCap = 0;\t// [RAMTINH 08/09]\r\n    m_bStopThread = FALSE;\r\n")
    s = rep(s, "        CloseHandle(m_hLoadPathGrid);\r\n        m_hLoadPathGrid = NULL;\r\n\t}\r\n\tm_bHavePath = FALSE;\r\n#endif\r\n}\r\n",
        "        CloseHandle(m_hLoadPathGrid);\r\n        m_hLoadPathGrid = NULL;\r\n\t}\r\n\tm_bHavePath = FALSE;\r\n\tThaLuoi();\t// [RAMTINH 08/09]\r\n#endif\r\n}\r\n")
    fns = (
        "// [RAMTINH 08/09] Luoi tim duong client: cap phat HEAP dung co luoi cua ban do dang dung. Truoc: VGridNode[MAX_CELL] 48 MB +\r\n"
        "// int[MAX_CELL] 9,6 MB nam trong .bss CoreClient -> RAM rieng +57,6 MB du ban do nho (12x12 region = 73.728 o = 1,5 MB).\r\n"
        "// Cap lai khi doi sang ban do khac co (tha ban cu), tha han khi huy KSubWorld. m_pTempCover chi can luc dung luoi -> tha ngay sau.\r\n"
        "BOOL KSubWorld::CapLuoi(int nAllCell)\r\n{\r\n"
        "\tif (nAllCell <= 0 || nAllCell > 16000000)\r\n\t\treturn FALSE;\r\n"
        "\tif (m_GridNode && m_nGridCellCap == nAllCell)\r\n\t{\r\n"
        "\t\tif (!m_pTempCover)\r\n\t\t\tm_pTempCover = new(std::nothrow) int[nAllCell];\r\n"
        "\t\treturn m_pTempCover != NULL;\r\n\t}\r\n"
        "\tThaLuoi();\r\n"
        "\tm_GridNode = new(std::nothrow) VGridNode[nAllCell];\r\n"
        "\tm_pTempCover = new(std::nothrow) int[nAllCell];\r\n"
        "\tif (!m_GridNode || !m_pTempCover)\r\n\t{\r\n\t\tThaLuoi();\r\n\t\treturn FALSE;\r\n\t}\r\n"
        "\tm_nGridCellCap = nAllCell;\r\n"
        "\treturn TRUE;\r\n}\r\n"
        "void KSubWorld::ThaLuoi()\r\n{\r\n"
        "\tif (m_GridNode)\r\n\t{\r\n\t\tdelete [] m_GridNode;\r\n\t\tm_GridNode = NULL;\r\n\t}\r\n"
        "\tThaTempCover();\r\n"
        "\tm_nGridCellCap = 0;\r\n"
        "\tm_bHavePath = FALSE;\r\n}\r\n"
        "void KSubWorld::ThaTempCover()\r\n{\r\n"
        "\tif (m_pTempCover)\r\n\t{\r\n\t\tdelete [] m_pTempCover;\r\n\t\tm_pTempCover = NULL;\r\n\t}\r\n}\r\n\r\n"
        "DWORD WINAPI LoadPathGrid(void* pParam)\r\n")
    s = rep(s, "DWORD WINAPI LoadPathGrid(void* pParam)\r\n", fns)
    # ProcLoadPathGrid: cap luoi (client)
    s = rep(s, "\tint nAllCell  = nAllCellW * nAllCellH;\r\n\tchar\tFile[MAX_PATH];\r\n",
        "\tint nAllCell  = nAllCellW * nAllCellH;\r\n"
        "#ifndef _SERVER\r\n\tif (!CapLuoi(nAllCell))\t// [RAMTINH 08/09] cap phat dung co (server cap trong LoadPathGridSrv)\r\n\t\treturn;\r\n#endif\r\n"
        "\tchar\tFile[MAX_PATH];\r\n")
    # nap cache .fp (client): cap luoi + kiem kich thuoc truoc khi doc
    s = rep(s, "\t\t\t\tFile.Read(&uVersion, sizeof(UINT));\r\n\t\t\t\tFile.Read(m_GridNode, uVersion);\r\n",
        "\t\t\t\tFile.Read(&uVersion, sizeof(UINT));\r\n"
        "\t\t\t\tconst int nAllCellCache = m_nGridW*m_nRegionWidth * m_nGridH*m_nRegionHeight;\t// [RAMTINH 08/09] cap dung co + doi chieu kich thuoc (cache lech -> dung lai)\r\n"
        "\t\t\t\tif (uVersion == (UINT)(sizeof(VGridNode) * nAllCellCache) && CapLuoi(nAllCellCache))\r\n\t\t\t\t{\r\n"
        "\t\t\t\tFile.Read(m_GridNode, uVersion);\r\n")
    s = rep(s, "\t\t\t\t\tm_bHavePath = TRUE;\r\n\t\t\t\t}\r\n\t\t\t\tbLoadData = true;\r\n\t\t\t}\r\n",
        "\t\t\t\t\tm_bHavePath = TRUE;\r\n\t\t\t\t}\r\n\t\t\t\tbLoadData = true;\r\n\t\t\t\tThaTempCover();\t// [RAMTINH 08/09]\r\n\t\t\t\t}\r\n\t\t\t}\r\n")
    # sau khi dung luoi moi: tha m_pTempCover
    s = rep(s, "\t\t\tProcLoadPathGrid();\r\n\t\t\tg_CreatePath(\"\\\\maps\");\r\n",
        "\t\t\tProcLoadPathGrid();\r\n\t\t\tThaTempCover();\t// [RAMTINH 08/09] chi can luc dung luoi\r\n\t\t\tg_CreatePath(\"\\\\maps\");\r\n")
    if "#include <new>" not in s:
        s = rep(s, "#include \"KSubWorld.h\"\r\n", "#include \"KSubWorld.h\"\r\n#include <new>\t// [RAMTINH 08/09] std::nothrow\r\n", 1)
save(p, s, h0); print("OK KSubWorld.cpp")

# ======================= (2) BONG MO NPC =======================
p, s = load("KNpcRes.h"); h0 = hi(s)
if "m_pcNpcBlur" not in s:
    # giu nguyen chu thich GBK cuoi dong (chi doi phan khai bao)
    s = rep(s, "\tKNpcBlur\t\tm_cNpcBlur;", "\tKNpcBlur*\t\tm_pcNpcBlur;\t// [RAMTINH 08/09] bong mo (23.708 B) cap phat lan dau NPC luot (CapBongMo), tha khi NPC bi go; truoc: co dinh trong moi KNpc = 800 x 23,7 KB.")
    s = rep(s, "\tvoid\t\t\tCreateBlur(int nNpcIdx, int nRange, int nDir);\r\n",
        "\tvoid\t\t\tCreateBlur(int nNpcIdx, int nRange, int nDir);\r\n"
        "\tKNpcBlur*\t\tCapBongMo();\t\t// [RAMTINH 08/09] cap bong mo lan dau can (NULL = het bo nho)\r\n"
        "\tvoid\t\t\tThaBongMo();\r\n")
save(p, s, h0); print("OK KNpcRes.h")

p, s = load("KNpcRes.cpp"); h0 = hi(s)
if "m_pcNpcBlur" not in s:
    if re.search(r"m_DataRes\s*=\s*", io.open(ROOT + "\\KNpc.cpp", "r", encoding="latin-1", newline="").read()):
        print("FAIL: KNpc.cpp gan m_DataRes theo gia tri -> con tro bong mo se bi chia se"); sys.exit(1)
    s = rep(s, "\tm_nBlurState = 0;\r\n\tmemset(m_szSoundName, 0, sizeof(m_szSoundName));\r\n",
        "\tm_nBlurState = 0;\r\n\tm_pcNpcBlur = NULL;\t// [RAMTINH 08/09]\r\n\tmemset(m_szSoundName, 0, sizeof(m_szSoundName));\r\n")
    s = rep(s, "KNpcRes::~KNpcRes()\r\n{\r\n", "KNpcRes::~KNpcRes()\r\n{\r\n\tThaBongMo();\t// [RAMTINH 08/09]\r\n")
    s = rep(s, "\treturn m_cNpcBlur.Init();\r\n", "\tif (m_pcNpcBlur)\t// [RAMTINH 08/09] bong mo cap khi can (CapBongMo); da co thi dat lai nhu cu\r\n\t\tm_pcNpcBlur->Init();\r\n\treturn TRUE;\r\n")
    s = rep(s, "\tm_cNpcBlur.Remove();\r\n", "\tif (m_pcNpcBlur)\t// [RAMTINH 08/09] tra 23,7 KB khi NPC bi go\r\n\t{\r\n\t\tm_pcNpcBlur->Remove();\r\n\t\tThaBongMo();\r\n\t}\r\n")
    s = rep(s, "\tm_cNpcBlur.ChangeAlpha();\r\n\tif (m_nBlurState == TRUE && m_cNpcBlur.NowGetBlur() && HaBongMoNhipNay(nNpcIdx))",
        "\tif (m_pcNpcBlur)\t// [RAMTINH 08/09] chua tung luot = chua co bong mo\r\n\t\tm_pcNpcBlur->ChangeAlpha();\r\n\tif (m_nBlurState == TRUE && CapBongMo() && m_pcNpcBlur->NowGetBlur() && HaBongMoNhipNay(nNpcIdx))")
    s = rep(s, "\t\tm_cNpcBlur.AddObj();\r\n", "\t\tif (CapBongMo())\r\n\t\t\tm_pcNpcBlur->AddObj();\r\n")
    s = rep(s, "\t\tm_cNpcBlur.RemoveObj();\r\n", "\t\tif (m_pcNpcBlur)\r\n\t\t\tm_pcNpcBlur->RemoveObj();\r\n")
    s = rep(s, "void KNpcRes::CreateBlur(int nNpcIdx, int nRange, int nDir)\r\n{\r\n\tif(nNpcIdx <= 0 || nRange <= 0)\r\n\t\treturn;\r\n",
        "// [RAMTINH 08/09] Bong mo cap phat lan dau NPC nay luot (23.708 B), tha khi NPC bi go (Remove) hoac huy.\r\n"
        "KNpcBlur* KNpcRes::CapBongMo()\r\n{\r\n"
        "\tif (!m_pcNpcBlur)\r\n\t{\r\n"
        "\t\tm_pcNpcBlur = new(std::nothrow) KNpcBlur();\r\n"
        "\t\tif (m_pcNpcBlur)\r\n\t\t\tm_pcNpcBlur->Init();\r\n\t}\r\n"
        "\treturn m_pcNpcBlur;\r\n}\r\n"
        "void KNpcRes::ThaBongMo()\r\n{\r\n"
        "\tif (m_pcNpcBlur)\r\n\t{\r\n\t\tdelete m_pcNpcBlur;\r\n\t\tm_pcNpcBlur = NULL;\r\n\t}\r\n}\r\n\r\n"
        "void KNpcRes::CreateBlur(int nNpcIdx, int nRange, int nDir)\r\n{\r\n\tif(nNpcIdx <= 0 || nRange <= 0)\r\n\t\treturn;\r\n"
        "\tif (!CapBongMo())\r\n\t\treturn;\t// [RAMTINH 08/09]\r\n")
    n_left = s.count("m_cNpcBlur.")
    s = s.replace("m_cNpcBlur.", "m_pcNpcBlur->")
    print("   KNpcRes.cpp: doi %d cho m_cNpcBlur. -> m_pcNpcBlur->" % n_left)
    if "#include <new>" not in s:
        s = rep(s, "#include \"KNpcRes.h\"\r\n", "#include \"KNpcRes.h\"\r\n#include <new>\t// [RAMTINH 08/09] std::nothrow\r\n", 1)
save(p, s, h0); print("OK KNpcRes.cpp")

p, s = load("CoreDrawGameObj.cpp"); h0 = hi(s)
if "m_pcNpcBlur" not in s:
    s2, n = re.subn(r"Npc\[uId\]\.GetNpcRes\(\)->m_cNpcBlur\.Draw\((\d)\);",
                    r"if (Npc[uId].GetNpcRes()->m_pcNpcBlur) Npc[uId].GetNpcRes()->m_pcNpcBlur->Draw(\1);\t// [RAMTINH 08/09]", s)
    if n != 8: print("FAIL CoreDrawGameObj: thay %d, can 8" % n); sys.exit(1)
    s = s2
save(p, s, h0); print("OK CoreDrawGameObj.cpp")
print("XONG RAMTINH")
