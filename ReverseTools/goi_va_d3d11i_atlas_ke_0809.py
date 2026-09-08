# -*- coding: ascii -*-
"""goi_va_d3d11i_atlas_ke_0809.py - [D3D11 08/09 i] atlas xep theo ke (shelf): doi khai bao CAtlasPage/CAtlasMgr trong D3D9on11i.h
va cach goi Alloc/Free trong D3D9on11.cpp cho khop D3D9on11Atlas.cpp moi."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"
def load(name):
    p = ROOT + "\\" + name
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    if name.startswith("D3D9on11"): s = s.replace("\r\n", "\n")
    return p, s
def save(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:70], c, n)); sys.exit(1)
    return s.replace(old, new)

p, s = load("D3D9on11i.h")
if "std::vector<std::vector<std::pair<UINT, UINT> > > m_free;" not in s:
    s = rep(s, "\tUINT m_binW, m_binH, m_cols, m_rows, m_used;\n\tstd::vector<UINT> m_free;\n};\n",
        "\tUINT m_binH, m_rows, m_used;\n\tstd::vector<std::vector<std::pair<UINT, UINT> > > m_free;\t// moi hang: cac doan trong (x0, x1)\n};\n")
    s = rep(s, "\tbool Alloc(UINT w, UINT h, CAtlasPage** ppPage, UINT* pSlot, UINT* pX, UINT* pY);\n\tvoid Free(CAtlasPage* pPage, UINT slot);\n",
        "\tbool Alloc(UINT w, UINT h, CAtlasPage** ppPage, UINT* pX, UINT* pY);\n\tvoid Free(CAtlasPage* pPage, UINT x, UINT y, UINT w);\n")
    s = rep(s, "\tCAtlasPage* NewPage(UINT binW, UINT binH);\n", "\tCAtlasPage* NewPage(UINT binH);\n")
save(p, s); print("OK D3D9on11i.h")

p, s = load("D3D9on11.cpp")
if "Free(m_pPage, m_ax, m_ay, m_w)" not in s:
    s = rep(s, "\t\tif (m_pPage && m_pDev->m_pAtlas) m_pDev->m_pAtlas->Free(m_pPage, m_slot);\n", "\t\tif (m_pPage && m_pDev->m_pAtlas) m_pDev->m_pAtlas->Free(m_pPage, m_ax, m_ay, m_w);\n")
    s = rep(s, "\t\tif (!m_pDev->m_pAtlas || !m_pDev->m_pAtlas->Alloc(m_w, m_h, &m_pPage, &m_slot, &m_ax, &m_ay))\n", "\t\tif (!m_pDev->m_pAtlas || !m_pDev->m_pAtlas->Alloc(m_w, m_h, &m_pPage, &m_ax, &m_ay))\n")
save(p, s); print("OK D3D9on11.cpp")
print("XONG")
