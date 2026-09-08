# -*- coding: utf-8 -*-
"""goi_va_ramtinh2_zero_init_0809.py - [RAMTINH 08/09 b] SAP 15:41 (jx_crash.log): KNpcBlur::SetMapPos -> KScenePlaceC::MoveObject ghi
dia chi rac. Goc: KNpcBlur truoc nam trong Npc[] TINH (bo nho .bss = 0 truoc khi ctor chay) nen m_SceneID[]/m_SceneIDNpcIdx[]/m_nMapXpos..
= 0; ctor KNpcBlur KHONG gan cac mang do -> cap bang new tren heap = RAC -> MoveObject voi id rac. Sua: cap bang calloc (0) + placement
new (ctor) = y het tinh; tha bang ~KNpcBlur + free. Luoi VGridNode cung vay (ctor chi gan w/h/connStart/connCount): calloc + placement new
tung o, m_pTempCover calloc."""
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

p, s = load("KNpcRes.cpp"); h0 = hi(s)
if "calloc(1, sizeof(KNpcBlur))" not in s:
    s = rep(s, "\tif (!m_pcNpcBlur)\r\n\t{\r\n\t\tm_pcNpcBlur = new(std::nothrow) KNpcBlur();\r\n\t\tif (m_pcNpcBlur)\r\n\t\t\tm_pcNpcBlur->Init();\r\n\t}\r\n\treturn m_pcNpcBlur;\r\n}\r\n",
        "\tif (!m_pcNpcBlur)\r\n\t{\r\n"
        "\t\t// [RAMTINH 08/09 b] PHAI bang 0 truoc khi ctor chay (nhu khi nam trong Npc[] tinh): ctor KNpcBlur khong gan m_SceneID[] /\r\n"
        "\t\t// m_SceneIDNpcIdx[] / m_nMapXpos.. -> new thuong de RAC -> SetMapPos goi MoveObject voi id rac -> SAP (15:41 08/09).\r\n"
        "\t\tvoid* pMem = calloc(1, sizeof(KNpcBlur));\r\n"
        "\t\tif (pMem)\r\n\t\t{\r\n"
        "\t\t\tm_pcNpcBlur = new (pMem) KNpcBlur();\r\n"
        "\t\t\tm_pcNpcBlur->Init();\r\n\t\t}\r\n\t}\r\n"
        "\treturn m_pcNpcBlur;\r\n}\r\n")
    s = rep(s, "\tif (m_pcNpcBlur)\r\n\t{\r\n\t\tdelete m_pcNpcBlur;\r\n\t\tm_pcNpcBlur = NULL;\r\n\t}\r\n}\r\n",
        "\tif (m_pcNpcBlur)\r\n\t{\r\n\t\tm_pcNpcBlur->~KNpcBlur();\t// [RAMTINH 08/09 b] cap bang calloc + placement new\r\n\t\tfree(m_pcNpcBlur);\r\n\t\tm_pcNpcBlur = NULL;\r\n\t}\r\n}\r\n")
    if "#include <stdlib.h>" not in s:
        s = rep(s, "#include <new>\t// [RAMTINH 08/09] std::nothrow\r\n", "#include <new>\t// [RAMTINH 08/09] std::nothrow\r\n#include <stdlib.h>\t// [RAMTINH 08/09 b] calloc/free\r\n")
save(p, s, h0); print("OK KNpcRes.cpp")

p, s = load("KSubWorld.cpp"); h0 = hi(s)
if "calloc((size_t)nAllCell, sizeof(VGridNode))" not in s:
    s = rep(s, "\t\tif (!m_pTempCover)\r\n\t\t\tm_pTempCover = new(std::nothrow) int[nAllCell];\r\n\t\treturn m_pTempCover != NULL;\r\n",
        "\t\tif (!m_pTempCover)\r\n\t\t\tm_pTempCover = (int*)calloc((size_t)nAllCell, sizeof(int));\r\n\t\treturn m_pTempCover != NULL;\r\n")
    s = rep(s, "\tThaLuoi();\r\n\tm_GridNode = new(std::nothrow) VGridNode[nAllCell];\r\n\tm_pTempCover = new(std::nothrow) int[nAllCell];\r\n\tif (!m_GridNode || !m_pTempCover)\r\n\t{\r\n\t\tThaLuoi();\r\n\t\treturn FALSE;\r\n\t}\r\n",
        "\tThaLuoi();\r\n"
        "\t// [RAMTINH 08/09 b] bang 0 roi moi chay ctor (y het mang tinh .bss): ctor VGridNode chi gan w/h/connStart/connCount\r\n"
        "\tm_GridNode = (VGridNode*)calloc((size_t)nAllCell, sizeof(VGridNode));\r\n"
        "\tm_pTempCover = (int*)calloc((size_t)nAllCell, sizeof(int));\r\n"
        "\tif (!m_GridNode || !m_pTempCover)\r\n\t{\r\n\t\tThaLuoi();\r\n\t\treturn FALSE;\r\n\t}\r\n"
        "\tfor (int i = 0; i < nAllCell; i++)\r\n\t\tnew (&m_GridNode[i]) VGridNode();\r\n")
    s = rep(s, "\tif (m_GridNode)\r\n\t{\r\n\t\tdelete [] m_GridNode;\r\n\t\tm_GridNode = NULL;\r\n\t}\r\n",
        "\tif (m_GridNode)\r\n\t{\r\n\t\tfree(m_GridNode);\t// [RAMTINH 08/09 b] calloc\r\n\t\tm_GridNode = NULL;\r\n\t}\r\n")
    s = rep(s, "\tif (m_pTempCover)\r\n\t{\r\n\t\tdelete [] m_pTempCover;\r\n\t\tm_pTempCover = NULL;\r\n\t}\r\n",
        "\tif (m_pTempCover)\r\n\t{\r\n\t\tfree(m_pTempCover);\t// [RAMTINH 08/09 b] calloc\r\n\t\tm_pTempCover = NULL;\r\n\t}\r\n")
    if "#include <stdlib.h>" not in s:
        s = rep(s, "#include <new>\t// [RAMTINH 08/09] std::nothrow\r\n", "#include <new>\t// [RAMTINH 08/09] std::nothrow\r\n#include <stdlib.h>\t// [RAMTINH 08/09 b] calloc/free\r\n")
save(p, s, h0); print("OK KSubWorld.cpp")
print("XONG RAMTINH b")
