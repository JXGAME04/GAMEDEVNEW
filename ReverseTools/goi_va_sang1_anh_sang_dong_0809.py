# -*- coding: utf-8 -*-
"""goi_va_sang1_anh_sang_dong_0809.py - [SANG 08/09] Chu chi dung cho: "co he thong sang toi cua Represent3; nhan vat
dung cho nao thi xung quanh do sang, con xa thi bi toi am mau; luc tha nhieu skill thi khu vuc do sang dung mau".
DUNG VAY, va do duoc trong ma:
 - KIpoTree::Paint goi RenderLightMap() + SetLightInfo() moi khung ve khi m_bDynamicLighting && IsRep3D().
 - RenderLightMap (ban dang bien dich, KIpoTree.cpp:938; hai ban trên deu bi chu thich) dat NEN m_dwAmbient = 0xff101010,
   tuc 16 tren thang 64. Represent3 coi 0x404040 la mau TRUNG TINH (GetPoint3dLighting tra 0xff404040 khi tat chieu sang),
   nen cho KHONG co nguon sang chi con 16/64 = 25 % do sang, va khi chi mot phan anh sang toi thi cac kenh len khong deu
   => vua toi vua lech mau. Moi NPC va moi dan deu la mot nguon sang (ban kinh 320) nen quanh nguoi va quanh cho tha
   skill thi sang dung mau - dung y het chu ta.
Ban nay khong ep mot lua chon nao, chi mo hai nut trong config.ini de chu tu chinh roi chot:
 [Client] AnhSang    = 1 bat nhu hien nay (mac dinh) | 0 tat han -> moi noi sang deu nhu khong co he chieu sang
                       (va bo luon chi phi tinh ban do sang moi khung - do duoc la nang khi dong nguoi)
 [Client] AnhSangNen = muc nen 0..64, mac dinh 16 nhu hien nay; dat 64 = nen sang chuan, den chi lam SANG THEM.
Tep: Core/Src/Scene/KIpoTree.cpp (latin-1 CRLF)."""
import io, re, sys

P = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\Scene\KIpoTree.cpp"
NL = "\r\n"
s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for ch in s if ord(ch) >= 0x80)


def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n:
        print("FAIL neo %r: thay %d, can %d" % (old[:80], c, n)); sys.exit(1)
    return s.replace(old, new)


if "g_nAnhSang" in s:
    print("da va roi"); sys.exit(0)

# 1. doc cau hinh mot lan
s = rep(s, "void KIpoTree::Paint(RECT* pRepresentArea, IPOT_RENDER_LAYER eLayer)" + NL + "{" + NL,
    "// [SANG 08/09] hai nut cho he chieu sang dong (chu tu chinh roi chot; doc mot lan)" + NL +
    "static int g_nAnhSang = -1, g_nAnhSangNen = -1;" + NL +
    "static void AnhSangDocCauHinh()" + NL +
    "{" + NL +
    "\tif (g_nAnhSang >= 0) return;" + NL +
    "\tg_nAnhSang = (int)GetPrivateProfileIntA(\"Client\", \"AnhSang\", 1, \".\\\\config.ini\");" + NL +
    "\tg_nAnhSangNen = (int)GetPrivateProfileIntA(\"Client\", \"AnhSangNen\", 16, \".\\\\config.ini\");" + NL +
    "\tif (g_nAnhSangNen < 0) g_nAnhSangNen = 0;" + NL +
    "\tif (g_nAnhSangNen > 64) g_nAnhSangNen = 64;" + NL +
    "}" + NL +
    "void KIpoTree::Paint(RECT* pRepresentArea, IPOT_RENDER_LAYER eLayer)" + NL + "{" + NL)

# 2. AnhSang = 0 -> tat han (SetLightInfo(NULL) lam Represent3 tra mau trung tinh) va bo luon phan tinh toan
old = ("\tif(eLayer == IPOT_RL_COVER_GROUND && m_bDynamicLighting && g_pRepresent && g_pRepresent->IsRep3D())" + NL +
       "\t{" + NL)
if s.count(old) != 1:
    print("FAIL neo Paint: thay %d" % s.count(old)); sys.exit(1)
new = ("\tAnhSangDocCauHinh();\t// [SANG 08/09]" + NL +
       "\tif (eLayer == IPOT_RL_COVER_GROUND && g_nAnhSang == 0 && g_pRepresent && g_pRepresent->IsRep3D())" + NL +
       "\t{\t// [SANG 08/09] tat han: bao bo ve dung mau trung tinh cho moi diem, va KHONG tinh ban do sang nua" + NL +
       "\t\tstatic bool s_bDaTat = false;" + NL +
       "\t\tif (!s_bDaTat) { g_pRepresent->SetLightInfo(0, 0, NULL); s_bDaTat = true; }" + NL +
       "\t}" + NL +
       "\telse if(eLayer == IPOT_RL_COVER_GROUND && m_bDynamicLighting && g_pRepresent && g_pRepresent->IsRep3D())" + NL +
       "\t{" + NL)
s = s.replace(old, new)

# 3. muc nen lay tu cau hinh
s = rep(s, "\tm_dwAmbient = 0xff101010;" + NL,
    "\t// [SANG 08/09] nen cua ban do sang. 0x40 la muc TRUNG TINH cua Represent3 (GetPoint3dLighting tra 0xff404040" + NL +
    "\t// khi tat chieu sang), nen 0x10 = chi 25 % do sang o cho khong co den. [Client] AnhSangNen doi duoc." + NL +
    "\t{" + NL +
    "\t\tAnhSangDocCauHinh();" + NL +
    "\t\tconst DWORD dwNen = (DWORD)g_nAnhSangNen;" + NL +
    "\t\tm_dwAmbient = 0xff000000 | (dwNen << 16) | (dwNen << 8) | dwNen;" + NL +
    "\t}" + NL)

if sum(1 for ch in s if ord(ch) >= 0x80) != h0:
    print("FAIL byte cao"); sys.exit(1)
if re.search(r"[^\r]\n", s):
    print("FAIL LF"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KIpoTree.cpp"); print("XONG SANG")
