# -*- coding: utf-8 -*-
"""goi_va_sangtron_0909.py - [SANGTRON 09/09] Lay do sang NOI SUY TRON thay vi lay O GAN NHAT.

GOC LOI da do duoc, khong doan:
  [SANGDO] mot diem DUNG YEN co do sang nhay tu 0x10 den 0xA2 (gap 10 lan), doi 6-24 lan moi giay,
           va nhay Y HET NHAU o ca PaintInterp=1 lan PaintInterp=0 (khung=1440 moi 10 s ca hai luot)
           => ban do sang KHONG phai cho hai che do khac nhau.
  [INTERP] PaintInterp=1: hinh duoc VE - va duoc LAY SANG - o vi tri noi suy, lech toi 45 don vi so
           voi vi tri tick (nut cay canh khong duoc doi, chi SetDrawPos). PaintInterp=0: 0 dong.
  O luoi sang rong 32 don vi, GetPoint3dLighting lay O GAN NHAT khong noi suy.
  => 45 don vi = vuot 1-2 bien o NGAY TRONG MOT NHIP TICK, nen vat di chuyen NHAY BAC do sang vai lan
     moi tick theo nhip khung ve. PaintInterp=0 chi lay mau mot lan moi tick nen khong lo ra.

SUA: noi suy song tuyen 4 o lan can -> do sang bien thien TRON theo vi tri, het bac de nhay.
Khong tat den, khong dung toi bang mau / atlas / gop lenh ve.
Cong tac lui: [Client] Rep3SangTron = 0 (ve dung nhu cu). Mac dinh 1.
Kem dong log [SANGTRON] do bien do bac nhay ma ban nay xoa di.

Engine co san GetPoint3dLightingLerp NHUNG khong duong ve nao goi, VA no bi loi trong so
(vColor[0]*fOffX + vColor[1]*(1-fOffX) la nguoc chieu) nen KHONG dung lai, viet ban rieng bang so nguyen.
"""
import io
import sys

NL = "\r\n"
H = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3\KRepresentShell3.h"
C = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3\KRepresentShell3.cpp"


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: thay %d lan, can 1" % (ten, n))
        sys.exit(1)
    return s.replace(old, new)


# ------------------------------------------------------------------ header
s = doc(H)
h0 = sum(1 for c in s if ord(c) >= 0x80)
if "GetPoint3dLightingTron" in s:
    print("header da va roi")
else:
    old = "    unsigned int GetPoint3dLightingLerp(D3DXVECTOR3& v);"
    new = old + NL + "    inline unsigned int GetPoint3dLightingTron(D3DXVECTOR3& v);\t// [SANGTRON 09/09] noi suy tron 4 o lan can"
    s = rep(s, old, new, "H1")
    if sum(1 for c in s if ord(c) >= 0x80) != h0:
        print("FAIL byte cao header"); sys.exit(1)
    io.open(H, "w", encoding="latin-1", newline="").write(s)
    print("OK KRepresentShell3.h")

# ------------------------------------------------------------------ cpp
s = doc(C)
h0 = sum(1 for c in s if ord(c) >= 0x80)
if "[SANGTRON 09/09]" in s:
    print("cpp da va roi")
    sys.exit(0)

# C1 - bien toan cuc
old = "int  g_nRep3Pal       = 1;"
new = NL.join([
    "// [SANGTRON 09/09] 1 = lay do sang NOI SUY TRON 4 o lan can (mac dinh), 0 = lay O GAN NHAT nhu cu.",
    "int  g_nRep3SangTron  = 1;",
    "unsigned g_uRep3SangMax = 0, g_uRep3SangLay = 0;\t// do bien do bac nhay ma ban nay xoa di",
    "double   g_dRep3SangTong = 0.0;",
    old,
])
s = rep(s, old, new, "C1 bien toan cuc")

# C2 - doc config
old = "\tg_nRep3Pal       = Rep3Ini(\"Rep3Pal\", 1);\t// [D3D11 08/09 r]"
new = old + NL + "\tg_nRep3SangTron  = Rep3Ini(\"Rep3SangTron\", 1);\t// [SANGTRON 09/09]"
s = rep(s, old, new, "C2 doc config")

# C3 - ham noi suy tron, dat NGAY TRUOC SetPoint3dLighting (truoc moi cho goi)
old = "void __fastcall KRepresentShell3::SetPoint3dLighting(VERTEX3D& pDes, VERTEX3D& pSrc, DWORD color)"
new = NL.join([
    "// [SANGTRON 09/09] Do sang NOI SUY TRON theo 4 o lan can, neo vao TAM o. Xem dau tep goi_va_sangtron_0909.py.",
    "// O luoi 32 don vi ma ban goc lay O GAN NHAT => vat di chuyen nhay BAC do sang moi lan vuot bien o;",
    "// PaintInterp=1 lam hinh di 45 don vi trong mot nhip tick nen vuot 1-2 bien o ngay giua tick = doi mau.",
    "inline unsigned int KRepresentShell3::GetPoint3dLightingTron(D3DXVECTOR3& v)",
    "{",
    "\tif (!m_bDoLighting)",
    "\t\treturn 0xff404040;",
    "\tconst int nX = ((int)v.x) - m_nLightingAreaX;",
    "\tconst int nY = ((int)v.y) - m_nLightingAreaY;",
    "\tif ((unsigned)nX >= 1536 || (unsigned)nY >= 3072)",
    "\t\treturn pLightingArray[0];",
    "\t// neo vao TAM o: tam o thu i nam o toa do i*SIZE + SIZE/2",
    "\tconst int gx = nX - (LIGHTING_GRID_SIZEX / 2);",
    "\tconst int gy = nY - (LIGHTING_GRID_SIZEY / 2);",
    "\tint ix, iy, fx, fy;",
    "\tif (gx < 0) { ix = 0; fx = 0; } else { ix = gx / LIGHTING_GRID_SIZEX; fx = gx - ix * LIGHTING_GRID_SIZEX; }",
    "\tif (gy < 0) { iy = 0; fy = 0; } else { iy = gy / LIGHTING_GRID_SIZEY; fy = gy - iy * LIGHTING_GRID_SIZEY; }",
    "\tint ix1 = ix + 1, iy1 = iy + 1;",
    "\tif (ix  > LIGHTING_GRID_WIDTH  - 1) ix  = LIGHTING_GRID_WIDTH  - 1;",
    "\tif (ix1 > LIGHTING_GRID_WIDTH  - 1) ix1 = LIGHTING_GRID_WIDTH  - 1;",
    "\tif (iy  > LIGHTING_GRID_HEIGHT - 1) iy  = LIGHTING_GRID_HEIGHT - 1;",
    "\tif (iy1 > LIGHTING_GRID_HEIGHT - 1) iy1 = LIGHTING_GRID_HEIGHT - 1;",
    "\tconst DWORD c00 = pLightingArray[iy  * LIGHTING_GRID_WIDTH + ix ];",
    "\tconst DWORD c10 = pLightingArray[iy  * LIGHTING_GRID_WIDTH + ix1];",
    "\tconst DWORD c01 = pLightingArray[iy1 * LIGHTING_GRID_WIDTH + ix ];",
    "\tconst DWORD c11 = pLightingArray[iy1 * LIGHTING_GRID_WIDTH + ix1];",
    "\tconst int wx1 = fx, wx0 = LIGHTING_GRID_SIZEX - fx;",
    "\tconst int wy1 = fy, wy0 = LIGHTING_GRID_SIZEY - fy;",
    "\tDWORD dwOut = 0xff000000;",
    "\tint nCa;",
    "\tfor (nCa = 16; nCa >= 0; nCa -= 8)",
    "\t{",
    "\t\tconst int a = (int)((c00 >> nCa) & 0xFF), b = (int)((c10 >> nCa) & 0xFF);",
    "\t\tconst int c = (int)((c01 >> nCa) & 0xFF), d = (int)((c11 >> nCa) & 0xFF);",
    "\t\tconst int t = (a * wx0 + b * wx1) * wy0 + (c * wx0 + d * wx1) * wy1;",
    "\t\tdwOut |= ((DWORD)(t / (LIGHTING_GRID_SIZEX * LIGHTING_GRID_SIZEY))) << nCa;",
    "\t}",
    "\t{\t// do bien do BAC NHAY ma ban nay xoa di (o gan nhat so voi noi suy)",
    "\t\tconst DWORD dwGan = pLightingArray[(nY / LIGHTING_GRID_SIZEY) * LIGHTING_GRID_WIDTH + (nX / LIGHTING_GRID_SIZEX)];",
    "\t\tint nMax = 0;",
    "\t\tfor (nCa = 16; nCa >= 0; nCa -= 8)",
    "\t\t{",
    "\t\t\tint e = (int)((dwGan >> nCa) & 0xFF) - (int)((dwOut >> nCa) & 0xFF);",
    "\t\t\tif (e < 0) e = -e;",
    "\t\t\tif (e > nMax) nMax = e;",
    "\t\t}",
    "\t\tg_uRep3SangLay++;",
    "\t\tg_dRep3SangTong += (double)nMax;",
    "\t\tif ((unsigned)nMax > g_uRep3SangMax) g_uRep3SangMax = (unsigned)nMax;",
    "\t}",
    "\treturn dwOut;",
    "}",
    "",
    old,
])
s = rep(s, old, new, "C3 ham noi suy")

# C4 - cho goi 1: SetPoint3dLighting
old = "\tpDes.color = GetPoint3dLighting(pDes.position);"
new = "\tpDes.color = g_nRep3SangTron ? GetPoint3dLightingTron(pDes.position) : GetPoint3dLighting(pDes.position);\t// [SANGTRON 09/09]"
s = rep(s, old, new, "C4 SetPoint3dLighting")

# C5 - cho goi 2: duong ve anh (RU_T_IMAGE)
old = "\t\t\t\tcolor = (GetPoint3dLighting(v) & 0x00ffffff) | alpha;"
new = "\t\t\t\tcolor = ((g_nRep3SangTron ? GetPoint3dLightingTron(v) : GetPoint3dLighting(v)) & 0x00ffffff) | alpha;\t// [SANGTRON 09/09]"
s = rep(s, old, new, "C5 duong ve anh")

# C6 - dong log
old = "\t\t\tg_dRep3PresentMs = 0.0; g_uRep3Presents = 0; g_uRep3PresentSkip = 0;"
new = NL.join([
    "\t\t\tRep3Log(\"[SANGTRON] tron=%d | lay mau %u lan | chenh lech O GAN NHAT so voi NOI SUY: TB %.1f max %u (thang 0..255)\",",
    "\t\t\t\tg_nRep3SangTron, g_uRep3SangLay, g_uRep3SangLay ? g_dRep3SangTong / g_uRep3SangLay : 0.0, g_uRep3SangMax);",
    "\t\t\tg_uRep3SangLay = 0; g_dRep3SangTong = 0.0; g_uRep3SangMax = 0;",
    old,
])
s = rep(s, old, new, "C6 dong log")

if sum(1 for c in s if ord(c) >= 0x80) != h0:
    print("FAIL byte cao cpp"); sys.exit(1)
if "\ufffd" in s:
    print("FAIL FFFD cpp"); sys.exit(1)
io.open(C, "w", encoding="latin-1", newline="").write(s)
print("OK KRepresentShell3.cpp (byte cao %d giu nguyen)" % h0)

for f in (H, C):
    t = doc(f)
    print("  %-28s CRLF %5d | LF don %d | byte cao %d"
          % (f.rsplit("\\", 1)[-1], t.count("\r\n"), t.count("\n") - t.count("\r\n"), sum(1 for c in t if ord(c) >= 0x80)))
print("XONG SANGTRON")
