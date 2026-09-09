# -*- coding: utf-8 -*-
"""goi_va_ammau_0909.py - [AMMAU 09/09] m_ulAdjustColorId KHONG duoc khoi tao => khe NPC doi chu
thi thua huong mau bang/doc/chay cua nguoi cu.

CHUOI BANG CHUNG (do het, khong doan):
 1. [SANGDUNG] m_bDoLighting=1 -> 0, va 'so lan ve QUA NHANH CO CHIEU SANG: 0' o CA HAI ky
    => he chieu sang KHONG he tham gia ve SPR. Huong anh sang dong lai.
 2. KRepresentShell3::DrawSpriteAlpha3D dong 3443: neu nRenderStyle != IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST
    thi color = 0xffffffff. Nghia la renderer VUT BO moi mau truyen vao, TRU khi style la COLOR_ADJUST.
    => SPR chi co the doi mau khi bRenderStyle == COLOR_ADJUST.
 3. Chi hai cho dat style do: KNpcRes.cpp:574 va :613, va deu voi dieu kien
        if (m_ulAdjustColorId > 0 && m_ulAdjustColorId <= g_ulAdjustColorCount)
            bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;
            Color.Color_dw = g_pAdjustColorTab[m_ulAdjustColorId - 1];
    => SPR doi mau KHI VA CHI KHI m_ulAdjustColorId > 0.
 4. m_ulAdjustColorId chi duoc gan tu KNpc::ProcessState (KNpc.cpp 1938-1993: bang/doc/chay, con lai
    dat adjustcolor_physics = 0 = tat). NHUNG no KHONG duoc khoi tao o dau ca:
    - ham dung KNpcRes::KNpcRes() (dong 29) dat hang chuc thanh vien nhung KHONG dat no
    - KNpcRes::Init() cung KHONG dat no
    ma KNpc::Load goi Init khi khe NPC DOI CHU (~7,5 lan/giay - do duoc trong dot [MOCOI]).
 => Nguoi moi vao khe thua huong nguyen mau bang/doc/chay cua nguoi cu cho toi khi ProcessState cua
    chinh no chay. Khe doi chu lien tuc khi CO NGUOI DI CHUYEN (vao/ra tam dong bo) va dung yen thi
    khong doi => dung y het trieu chung "di chuyen thi doi mau, dung yen thi dung mau".

SUA: khoi tao m_ulAdjustColorId = 0 o ham dung VA o Init.
   [Client] SuaAmMau = 1 (mac dinh, co sua) | 0 = y nhu cu, de chu doi chung.
DO: dem so o ve dung COLOR_ADJUST ma NPC do KHONG he co trang thai bang/doc/chay = so lan bi am mau OAN.
"""
import io
import sys

NL = "\r\n"
P = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KNpcRes.cpp"


def rep(s, old, new, ten, cho=1):
    n = s.count(old)
    if n != cho:
        print("FAIL neo %s: thay %d lan, can %d" % (ten, n, cho)); sys.exit(1)
    return s.replace(old, new)


s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for c in s if ord(c) >= 0x80)
if "[AMMAU 09/09]" in s:
    print("da va roi"); sys.exit(0)

# 1) bien dem + co cau hinh + ham in, dat truoc ham dung
old = "KNpcRes::KNpcRes()"
new = NL.join([
    "// [AMMAU 09/09] Dem so o ve bi AM MAU OAN: dung COLOR_ADJUST trong khi NPC do khong he",
    "// co trang thai bang / doc / chay. In moi 10 giay vao jx_paint.log khi [Client] PaintLog=1.",
    "extern int g_nCorePaintLog;",
    "static unsigned s_uAmDung = 0, s_uAmOan = 0;",
    "static int s_nSuaAmMau = -1;",
    "static void AmMauInDong()",
    "{",
    "\tif (g_nCorePaintLog <= 0)",
    "\t\treturn;",
    "\tstatic DWORD s_dwLan = 0;",
    "\tconst DWORD dwNow = GetTickCount();",
    "\tif (s_dwLan == 0) { s_dwLan = dwNow; return; }",
    "\tif (dwNow - s_dwLan < 10000)",
    "\t\treturn;",
    "\ts_dwLan = dwNow;",
    "\tFILE* pLog = fopen(\"jx_paint.log\", \"a\");",
    "\tif (pLog)",
    "\t{",
    "\t\tfprintf(pLog, \"[AMMAU] t=%u | o ve dung doi mau: DUNG (co bang/doc/chay) %u | OAN (khong trang thai gi) %u | sua=%d\\n\",",
    "\t\t\tdwNow, s_uAmDung, s_uAmOan, s_nSuaAmMau);",
    "\t\tfclose(pLog);",
    "\t}",
    "\ts_uAmDung = s_uAmOan = 0;",
    "}",
    "",
    old,
])
s = rep(s, old, new, "1 khai bao")

# 2) khoi tao trong ham dung
old = "\tm_pcNpcBlur = NULL;\t// [RAMTINH 08/09]"
new = old + NL + "\tm_ulAdjustColorId = 0;\t// [AMMAU 09/09] truoc day KHONG duoc khoi tao"
s = rep(s, old, new, "2 ham dung")

# 3) dat lai trong Init (duong khe NPC DOI CHU)
old = NL.join([
    "\tfor (i = 0; i < MAX_NPC_IMAGE_NUM; i++)",
    "\t{",
    "\t\tm_cDrawFile[i].nType = ISI_T_SPR;",
])
new = NL.join([
    "\t// [AMMAU 09/09] Khe NPC doi chu thi phai xoa mau bang/doc/chay cua NGUOI CU, neu khong nguoi",
    "\t// moi bi ve dung mau do cho toi khi ProcessState cua chinh no chay. [Client] SuaAmMau = 0 de lui.",
    "\tif (s_nSuaAmMau < 0)",
    "\t\ts_nSuaAmMau = (int)GetPrivateProfileIntA(\"Client\", \"SuaAmMau\", 1, \".\\\\config.ini\");",
    "\tif (s_nSuaAmMau)",
    "\t\tm_ulAdjustColorId = 0;",
    old,
])
s = rep(s, old, new, "3 Init")

# 4) dem o hai cho dung COLOR_ADJUST
old = NL.join([
    "\t\t\t\tif (m_ulAdjustColorId > 0 && m_ulAdjustColorId <= g_ulAdjustColorCount)",
    "\t\t\t\t{",
    "\t\t\t\t\tm_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;",
])
new = NL.join([
    "\t\t\t\tif (m_ulAdjustColorId > 0 && m_ulAdjustColorId <= g_ulAdjustColorCount)",
    "\t\t\t\t{",
    "\t\t\t\t\tif (g_nCorePaintLog > 0)",
    "\t\t\t\t\t{\t// [AMMAU 09/09] co that su dang bang/doc/chay khong?",
    "\t\t\t\t\t\tif (Npc[nNpcIdx].m_FreezeState.nTime > 0 || Npc[nNpcIdx].m_PoisonState.nTime > 0 || Npc[nNpcIdx].m_BurnState.nTime > 0)",
    "\t\t\t\t\t\t\ts_uAmDung++;",
    "\t\t\t\t\t\telse",
    "\t\t\t\t\t\t\ts_uAmOan++;",
    "\t\t\t\t\t}",
    "\t\t\t\t\tm_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;",
])
s = rep(s, old, new, "4 dem", 2)

# 5) goi ham in o cuoi Draw - neo vao cho tra ve cua Draw
old = "\tif (m_pcNpcBlur)\t// [RAMTINH 08/09] bong mo cap khi can (CapBongMo); da co thi dat lai nhu cu"
new = "\tAmMauInDong();\t// [AMMAU 09/09]" + NL + old
s = rep(s, old, new, "5 goi in")

if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
    print("FAIL ma hoa"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KNpcRes.cpp (byte cao %d giu nguyen)" % h0)
t = io.open(P, "r", encoding="latin-1", newline="").read()
print("  CRLF %d | LF don %d | byte cao %d" % (t.count("\r\n"), t.count("\n") - t.count("\r\n"), sum(1 for c in t if ord(c) >= 0x80)))
print("XONG AMMAU")
