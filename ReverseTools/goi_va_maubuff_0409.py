# -*- coding: utf-8 -*-
"""[MAUBUFF 04/09] Hieu ung buff doi mau moi lan buff - sua tan goc.

 Chu game: "khi buff ky nang len hien thi hinh anh thi moi lan buff len no se thay doi 1 mau
 (toi muon fix lai co dinh mau cua hien thi goc)".

 GOC: KNpcRes::Draw (KNpcRes.cpp) ve bang mang THANH VIEN m_cDrawFile[MAX_NPC_IMAGE_NUM].
 Init() dat mac dinh cho ca mang: bRenderStyle = IMAGE_RENDER_STYLE_ALPHA, Color.a = 255.
 Nhung trong Draw, khoi ve THAN NGUOI va khoi ve HIEU UNG TRANG BI doi cac o do sang
 IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST + mau doc/bong/dong bang (g_pAdjustColorTab), hoac ha alpha
 khi tang hinh - va KHONG AI TRA LAI. Cac khoi ve HIEU UNG TRANG THAI (buff) thi chi gan
 szImage/uImage/nFrame/oPosition, KHONG he gan bRenderStyle/Color => chung an theo gia tri CON SOT
 trong dung o do cua mang. Vi vay moi lan buff (so o thay doi, hoac nhan vat dang trung doc/bong/
 dong bang) hieu ung lai ra mot mau khac.

 VA 1: moi cho ve hieu ung trang thai deu dat lai dung mac dinh cua Init:
       bRenderStyle = IMAGE_RENDER_STYLE_ALPHA; Color.a = 255 (giu START_BLUR_ALPHA khi tang hinh
       de tang hinh van mo). Nghia la hieu ung buff luon ve bang mau GOC cua chinh spr do.
 VA 2: khoi ve THAN NGUOI gan kieu ve/mau vao m_cDrawFile[i] trong khi anh gan vao m_cDrawFile[nPos].
       Khi bang thu tu co "lo" (i != nPos) thi than nguoi nhan mau cua o KHAC, con mau dinh cho no
       lai roi vao o dang giu hieu ung buff. Day dung la loi ma chu thich o khoi TRANG BI ngay tren
       (dong 518-520) noi da sua roi - nhung khoi than nguoi thi bi bo sot. Nay dung nPos cho ca hai.

 Doc/ghi latin-1 newline=''. Neo kiem dem == 1."""
import io
import re

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KNpcRes.cpp"
MARK = "[MAUBUFF 04/09]"


def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f:
        return f.read()


def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f:
        f.write(s)


s = rd(P)
N = "\r\n" if "\r\n" in s else "\n"
if MARK in s:
    print("da va")
    raise SystemExit

# ---------- VA 2: khoi ve than nguoi dung nPos thay vi i ----------
old_than = N.join([
    "\t\t\t\tif (m_ulAdjustColorId > 0 && m_ulAdjustColorId <= g_ulAdjustColorCount)",
    "\t\t\t\t{",
    "\t\t\t\t\tm_cDrawFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;",
    "\t\t\t\t\tm_cDrawFile[i].Color.Color_dw = g_pAdjustColorTab[m_ulAdjustColorId - 1];",
    "\t\t\t\t}",
    "\t\t\t\telse",
    "\t\t\t\t{",
    "\t\t\t\t\tm_cDrawFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;",
    "\t\t\t\t\tif (Npc[nNpcIdx].m_HideState.nTime)",
    "\t\t\t\t\t\tm_cDrawFile[i].Color.Color_b.a = START_BLUR_ALPHA;",
    "\t\t\t\t\telse",
    "\t\t\t\t\t\tm_cDrawFile[i].Color.Color_b.a = 255;",
    "\t\t\t\t}",
])
assert s.count(old_than) == 1, "khoi than nguoi: tim thay %d cho" % s.count(old_than)
new_than = N.join([
    "\t\t\t\t// %s dung nPos, KHONG dung i: anh gan vao m_cDrawFile[nPos] nen kieu ve/mau" % MARK,
    "\t\t\t\t// cung phai vao dung o do. Bang thu tu co 'lo' (i != nPos) thi than nguoi nhan mau",
    "\t\t\t\t// cua o khac, con mau dinh cho no roi vao o dang giu hieu ung buff - dung loi ma",
    "\t\t\t\t// chu thich o khoi TRANG BI ngay tren da sua, khoi nay bi bo sot.",
    "\t\t\t\tif (m_ulAdjustColorId > 0 && m_ulAdjustColorId <= g_ulAdjustColorCount)",
    "\t\t\t\t{",
    "\t\t\t\t\tm_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;",
    "\t\t\t\t\tm_cDrawFile[nPos].Color.Color_dw = g_pAdjustColorTab[m_ulAdjustColorId - 1];",
    "\t\t\t\t}",
    "\t\t\t\telse",
    "\t\t\t\t{",
    "\t\t\t\t\tm_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;",
    "\t\t\t\t\tif (Npc[nNpcIdx].m_HideState.nTime)",
    "\t\t\t\t\t\tm_cDrawFile[nPos].Color.Color_b.a = START_BLUR_ALPHA;",
    "\t\t\t\t\telse",
    "\t\t\t\t\t\tm_cDrawFile[nPos].Color.Color_b.a = 255;",
    "\t\t\t\t}",
])
s = s.replace(old_than, new_than, 1)

# ---------- VA 1: tra kieu ve GOC cho moi cho ve hieu ung trang thai (va ca bong) ----------
re_state = re.compile(r"^(\s*)strcpy\(m_cDrawFile\[nPos\]\.szImage, (m_cStateSpr\[|this->m_cNpcShadow)")
lines = s.split(N)
out = []
dem = 0
for ln in lines:
    m = re_state.match(ln)
    if m and not ln.lstrip().startswith("//"):
        tab = m.group(1)
        dem += 1
        if dem == 1:
            out.append(tab + "// %s m_cDrawFile la mang THANH VIEN: khoi ve than nguoi / hieu ung trang bi" % MARK)
            out.append(tab + "// doi o nay sang ALPHA_COLOR_ADJUST + mau doc/bong/dong bang roi khong tra lai,")
            out.append(tab + "// nen hieu ung buff an theo mau con sot => moi lan buff ra mot mau khac.")
            out.append(tab + "// Tra ve dung mac dinh cua Init: ALPHA + alpha 255 (tang hinh van mo).")
        else:
            out.append(tab + "// %s tra kieu ve goc (xem ghi chu o cho dau tien)" % MARK)
        out.append(tab + "m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;")
        out.append(tab + "m_cDrawFile[nPos].Color.Color_b.a = Npc[nNpcIdx].m_HideState.nTime ? START_BLUR_ALPHA : 255;")
    out.append(ln)

assert dem >= 9, "chi tim thay %d cho ve hieu ung trang thai (cho >= 9)" % dem
wr(P, N.join(out))
print("KNpcRes.cpp OK: tra kieu ve goc cho %d cho ve hieu ung trang thai + sua khoi than nguoi dung nPos" % dem)
