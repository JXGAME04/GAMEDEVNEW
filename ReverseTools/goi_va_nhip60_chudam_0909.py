# -*- coding: utf-8 -*-
"""goi_va_nhip60_chudam_0909.py - [NHIP60] + [CHUDAM] + [TENNEN] 09/09

Goc loi (chot sau ca ngay do): nhoe BAM MAT tren man hinh giu mau - moi khung anh deu DUNG (chu chup
anh luon binh thuong; 9 khung lien tiep do duoc cung mau, cung 113 diem net), nhung mat bam theo vat
truot muot thi anh bi keo tren vong mac; net chu 1 diem anh tron voi vien den va nen dat nau do => "am do".
PaintInterp=0 het han vi 18 buoc/giay mat khong bam noi. Ba can keo duoc:

  [NHIP60]  (CoreShell.cpp, GOI_PROCFRAME_POSSHIFT) chu yeu cau: "van ve theo Hz man hinh (giao dien,
            hieu ung muot) nhung vi tri the gioi chi cap nhat o nhip 60 Hz". POSSHIFT chi chay o nhip
            [Client] NhipTheGioi (mac dinh 60; 0 = moi khung ve nhu cu; 72 hoac 48 cho buoc DEU o 144 Hz).
            Khung bi bo qua giu nguyen vi tri ve cu. Giao dien / hieu ung van ve moi khung.
  [CHUDAM]  (KFont3 + KRepresentShell3::SetOption + KNpc::PaintInfo) chu dam: moi ky tu phat HAI o (x, x+1)
            o CA luot vien LAN luot ruot => net 2 diem anh, vien den van bao ngoai. Net 2 px bi keo +-1 van
            con loi mau kem; net 1 px thi bi tron 100 %. Bat/tat qua SetOption(TEXTBOLD) - them gia tri enum,
            KHONG doi vtable. [Client] ChuDam = 1 (mac dinh) | 0 = tat.
  [TENNEN]  (KNpc::PaintInfo) nen mo toi sau ten / danh hieu / ten bang / ten quai: vet keo tron kem voi nen
            toi thanh xam thay vi tron voi dat nau do thanh AM DO. Dung RU_T_SHADOW (hinh chu nhat dac co alpha).
            [Client] TenNen = 0..255 do dac (mac dinh 128) | 0 = tat.

Ban [VIENCHU] truoc lam NGUOC (vien xam = giam tuong phan) - chu thay "mo di" la dung.
Tat ca mac dinh trong ma, KHONG dung config.ini cua chu.
"""
import io
import sys

NL = "\r\n"
T = "\t"
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources"
F_ENUM = ROOT + r"\Represent\iRepresent\iRepresentShell.h"
F_FH = ROOT + r"\Represent\iRepresent\Font\KFont3.h"
F_FC = ROOT + r"\Represent\iRepresent\Font\KFont3.cpp"
F_R3 = ROOT + r"\Represent\Represent3\KRepresentShell3.cpp"
F_NPC = ROOT + r"\Core\Src\KNpc.cpp"
F_CS = ROOT + r"\Core\Src\CoreShell.cpp"


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, s, h0, ten):
    if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    if s.count("\n") != s.count("\r\n"):
        print("FAIL LF don " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten, cho=1):
    n = s.count(old)
    if n != cho:
        print("FAIL neo %s: thay %d lan, can %d" % (ten, n, cho)); sys.exit(1)
    return s.replace(old, new)


# ---------------------------------------------------------------- 1. enum
s = doc(F_ENUM); h0 = sum(1 for c in s if ord(c) >= 0x80)
if "TEXTBOLD" not in s:
    s = rep(s, T + "PERSPECTIVE," + NL + "};",
            T + "PERSPECTIVE," + NL + T + "TEXTBOLD," + T + "// [CHUDAM 09/09] chu dam (them cuoi enum, khong doi vtable)" + NL + "};",
            "enum")
    ghi(F_ENUM, s, h0, "iRepresentShell.h")
else:
    print("enum da co")

# ---------------------------------------------------------------- 2. KFont3.h
s = doc(F_FH); h0 = sum(1 for c in s if ord(c) >= 0x80)
if "ms_bDam" not in s:
    s = rep(s, T + "void" + T + "SetBorderColor(unsigned int uColor);",
            T + "void" + T + "SetBorderColor(unsigned int uColor);" + NL +
            T + "static void" + T + "SetBold(bool bDam) { ms_bDam = bDam; }" + T + "// [CHUDAM 09/09]",
            "Fh SetBold")
    s = rep(s, T + "static unsigned int" + T + "ms_uBorderColor;",
            T + "static unsigned int" + T + "ms_uBorderColor;" + NL +
            T + "static bool" + T + T + "ms_bDam;" + T + "// [CHUDAM 09/09] 1 = moi ky tu ve hai lan (x, x+1) o ca hai luot",
            "Fh ms_bDam")
    ghi(F_FH, s, h0, "KFont3.h")
else:
    print("KFont3.h da co")

# ---------------------------------------------------------------- 3. KFont3.cpp
s = doc(F_FC); h0 = sum(1 for c in s if ord(c) >= 0x80)
if "VeLoChu" not in s:
    s = rep(s, "unsigned int" + T + T + "KFont3::ms_uBorderColor" + T + "= 0xff000000;",
            "unsigned int" + T + T + "KFont3::ms_uBorderColor" + T + "= 0xff000000;" + NL +
            "bool" + T + T + T + "KFont3::ms_bDam" + T + T + "= false;" + T + "// [CHUDAM 09/09]" + NL +
            "// [CHUDAM 09/09] Ve mot luot chu; neu dam thi ve them mot lan dich phai 1 diem anh roi tra lai." + NL +
            "// Goi trong TUNG luot (vien roi ruot) nen thu tu van dung: vien(x) vien(x+1) roi ruot(x) ruot(x+1)." + NL +
            "static void VeLoChu(LPDIRECT3DDEVICE9 pDev, KFontVertex* p, unsigned int nNumPolys, bool bDam)" + NL +
            "{" + NL +
            T + "pDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, nNumPolys, p, sizeof(KFontVertex));" + NL +
            T + "if (!bDam)" + NL +
            T + T + "return;" + NL +
            T + "const unsigned int n = nNumPolys * 3;" + NL +
            T + "unsigned int i;" + NL +
            T + "for (i = 0; i < n; i++) p[i].x += 1.0f;" + NL +
            T + "pDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, nNumPolys, p, sizeof(KFontVertex));" + NL +
            T + "for (i = 0; i < n; i++) p[i].x -= 1.0f;" + NL +
            "}",
            "Fc dinh nghia")
    # thay HAI lenh ve trong tung ham (lat cat theo ten ham de khong dung ham khac)
    goi = T + "ms_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, nNumPolys," + NL + T + T + "pPolyVertices, sizeof(KFontVertex));"
    moi = T + "VeLoChu(ms_pd3dDevice, pPolyVertices, nNumPolys, ms_bDam);" + T + "// [CHUDAM 09/09]"
    for fn in ("void KFont3::RenderTextCmpReverse(", "void KFont3::RenderTextCmp("):
        a = s.find(fn)
        if a < 0:
            print("FAIL khong thay " + fn); sys.exit(1)
        b = s.find(NL + "}" + NL, a)
        lat = s[a:b]
        if lat.count(goi) != 2:
            print("FAIL %s: %d lenh ve, can 2" % (fn, lat.count(goi))); sys.exit(1)
        s = s[:a] + lat.replace(goi, moi) + s[b:]
    ghi(F_FC, s, h0, "KFont3.cpp")
else:
    print("KFont3.cpp da co")

# ---------------------------------------------------------------- 4. KRepresentShell3.cpp SetOption
s = doc(F_R3); h0 = sum(1 for c in s if ord(c) >= 0x80)
if "case TEXTBOLD" not in s:
    s = rep(s, T + "case PERSPECTIVE:",
            T + "case TEXTBOLD:" + T + "// [CHUDAM 09/09] chu dam cho ten / danh hieu (KNpc::PaintInfo bat, ve xong tat)" + NL +
            T + T + "KFont3::SetBold(bOn);" + NL +
            T + T + "break;" + NL +
            T + "case PERSPECTIVE:",
            "R3 SetOption")
    ghi(F_R3, s, h0, "KRepresentShell3.cpp")
else:
    print("KRepresentShell3.cpp da co")

# ---------------------------------------------------------------- 5. KNpc.cpp
s = doc(F_NPC); h0 = sum(1 for c in s if ord(c) >= 0x80)
if "VeNenChu" not in s:
    sig = "int KNpc::PaintInfo(int nHeightOffset, bool bSelect, int nFontSize, DWORD dwBorderColor)"
    helper = NL.join([
        "#ifndef _SERVER",
        "// [CHUDAM/TENNEN 09/09] Nhoe bam mat: moi khung deu dung nhung mat bam vat truot lam net 1 diem anh tron voi",
        "// vien den va nen dat nau do => am do. Hai cach lam chu chiu duoc vet: (1) CHU DAM - net 2 diem anh giu duoc",
        "// loi mau; (2) NEN MO TOI sau chu - vet tron voi nen toi thanh xam thay vi tron voi dat thanh do.",
        "// [Client] ChuDam = 1 (mac dinh) | 0 tat.   [Client] TenNen = do dac 0..255 (mac dinh 128) | 0 tat.",
        "static int s_nChuDam = -1, s_nTenNen = -1;",
        "static void DocCauHinhChu()",
        "{",
        T + "if (s_nChuDam >= 0)",
        T + T + "return;",
        T + "s_nChuDam = (int)GetPrivateProfileIntA(\"Client\", \"ChuDam\", 1, \".\\\\config.ini\");",
        T + "s_nTenNen = (int)GetPrivateProfileIntA(\"Client\", \"TenNen\", 128, \".\\\\config.ini\");",
        T + "if (s_nTenNen < 0) s_nTenNen = 0;",
        T + "if (s_nTenNen > 255) s_nTenNen = 255;",
        "}",
        "// Ve nen mo toi phia sau mot dong chu. (nX, nY, nZ) la dung toa do dua cho OutputText; nRong = rong chu",
        "// (nFontSize * so byte / 2), nCao = nFontSize. Toa do y the gioi bi chia 2 khi ra man hinh nen cao nhan 2.",
        "static void VeNenChu(int nX, int nY, int nRong, int nCao, int nZ)",
        "{",
        T + "if (s_nTenNen <= 0 || !g_pRepresent)",
        T + T + "return;",
        T + "KRUShadow nen;",
        T + "memset(&nen, 0, sizeof(nen));",
        T + "nen.oPosition.nX = nX - 2;",
        T + "nen.oPosition.nY = nY - 2;",
        T + "nen.oPosition.nZ = nZ;",
        T + "nen.oEndPos.nX = nX + nRong + 2;",
        T + "nen.oEndPos.nY = nY + (nCao + 1) * 2;",
        T + "nen.oEndPos.nZ = nZ;",
        T + "int nA = (255 - s_nTenNen) >> 3;" + T + "// RU_T_SHADOW ve voi alpha = 255 - (a << 3)",
        T + "if (nA > 31) nA = 31;",
        T + "nen.Color.Color_b.r = 0; nen.Color.Color_b.g = 0; nen.Color.Color_b.b = 0;",
        T + "nen.Color.Color_b.a = (unsigned char)nA;",
        T + "g_pRepresent->DrawPrimitives(1, &nen, RU_T_SHADOW, FALSE);",
        "}",
        "#endif",
        "",
        sig,
    ])
    s = rep(s, sig, helper, "NPC helper")

    s = rep(s, T + "nFontSize = 13;",
            T + "nFontSize = 13;" + NL +
            T + "DocCauHinhChu();" + T + "// [CHUDAM/TENNEN 09/09]" + NL +
            T + "if (g_pRepresent)" + NL +
            T + T + "g_pRepresent->SetOption(TEXTBOLD, s_nChuDam != 0);",
            "NPC bat dam")

    s = rep(s, "#endif" + NL + NL + T + "return nHeightOffset;" + NL + "}",
            "#endif" + NL + NL +
            T + "if (g_pRepresent)" + NL +
            T + T + "g_pRepresent->SetOption(TEXTBOLD, false);" + T + "// [CHUDAM 09/09] tra lai, khong de chu khac bi dam" + NL +
            T + "return nHeightOffset;" + NL + "}",
            "NPC tat dam")

    # nen sau tung dong chu
    a = T*3 + "g_pRepresent->OutputText(smallerFontSize, szUpperLine, KRF_ZERO_END, nMpsX - smallerFontSize * g_StrLen(szUpperLine) / 4, yOffset,"
    s = rep(s, a, T*3 + "VeNenChu(nMpsX - smallerFontSize * g_StrLen(szUpperLine) / 4, yOffset, smallerFontSize * g_StrLen(szUpperLine) / 2, smallerFontSize, nHeightOff);" + T + "// [TENNEN 09/09]" + NL + a, "NPC nen upper")

    a = T*3 + "g_pRepresent->OutputText(nFontSize, szTong, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(szTong) / 4, nMpsY -24,"
    s = rep(s, a, T*3 + "VeNenChu(nMpsX - nFontSize * g_StrLen(szTong) / 4, nMpsY - 24, nFontSize * g_StrLen(szTong) / 2, nFontSize, nHeightOff);" + T + "// [TENNEN 09/09]" + NL + a, "NPC nen tong")

    a = T*4 + "g_pRepresent->OutputText(nFontSize, m_szGameTitle, KRF_ZERO_END," + NL + T*5 + "nMpsX - nFontSize * g_StrLen(m_szGameTitle) / 4, nYY,"
    s = rep(s, a, T*4 + "VeNenChu(nMpsX - nFontSize * g_StrLen(m_szGameTitle) / 4, nYY, nFontSize * g_StrLen(m_szGameTitle) / 2, nFontSize, nHeightOff);" + T + "// [TENNEN 09/09]" + NL + a, "NPC nen title")

    a = T*3 + "g_pRepresent->OutputText(nFontSize, szString, KRF_ZERO_END, nX, nY, dwColor, 0, nHeightOff, dwBorderColor);"
    s = rep(s, a, T*3 + "VeNenChu(nX, nY, nFontSize * g_StrLen(szString) / 2, nFontSize, nHeightOff);" + T + "// [TENNEN 09/09]" + NL + a, "NPC nen quai")

    a = T*2 + "dwColor = 0xffffffff;" + NL + T*2 + "g_pRepresent->OutputText(nFontSize, Name, KRF_ZERO_END,"
    s = rep(s, a, T*2 + "dwColor = 0xffffffff;" + NL + T*2 + "VeNenChu(nMpsX - nFontSize * g_StrLen(Name) / 4, nMpsY, nFontSize * g_StrLen(Name) / 2, nFontSize, nHeightOff);" + T + "// [TENNEN 09/09]" + NL + T*2 + "g_pRepresent->OutputText(nFontSize, Name, KRF_ZERO_END,", "NPC nen name")

    a = T*2 + "g_pRepresent->OutputText(nFontSize, szString, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(Name) / 4, nMpsY,"
    s = rep(s, a, T*2 + "VeNenChu(nMpsX - nFontSize * g_StrLen(Name) / 4, nMpsY, nFontSize * g_StrLen(szString) / 2, nFontSize, nHeightOff);" + T + "// [TENNEN 09/09]" + NL + a, "NPC nen name2")
    ghi(F_NPC, s, h0, "KNpc.cpp")
else:
    print("KNpc.cpp da co")

# ---------------------------------------------------------------- 6. CoreShell.cpp POSSHIFT
s = doc(F_CS); h0 = sum(1 for c in s if ord(c) >= 0x80)
if "[NHIP60 09/09]" not in s:
    a = T*2 + "int" + T + "nAlpha = (int)uParam;"
    gate = NL.join([
        T*2 + "// [NHIP60 09/09] Chu: \"van ve theo Hz man hinh (giao dien, hieu ung muot) nhung vi tri the gioi chi cap nhat",
        T*2 + "// o nhip 60 Hz\". Nhoe bam mat = toc do truot x thoi gian giu khung; PaintInterp=0 (18 buoc/giay) het han vi",
        T*2 + "// mat khong bam noi buoc nhay. Muc trung gian: POSSHIFT chi chay o nhip [Client] NhipTheGioi (mac dinh 60;",
        T*2 + "// 0 = moi khung ve nhu cu; 72 hoac 48 cho buoc DEU o 144 Hz). Khung bo qua giu nguyen vi tri ve cu.",
        T*2 + "{",
        T*3 + "static int    s_nNhipTG = -1;",
        T*3 + "static double s_dKeTiep = 0.0;",
        T*3 + "if (s_nNhipTG < 0)",
        T*3 + "{",
        T*4 + "s_nNhipTG = (int)GetPrivateProfileIntA(\"Client\", \"NhipTheGioi\", 60, \".\\\\config.ini\");",
        T*4 + "if (s_nNhipTG < 0) s_nNhipTG = 0;",
        T*4 + "if (s_nNhipTG > 1000) s_nNhipTG = 1000;",
        T*3 + "}",
        T*3 + "if (s_nNhipTG > 0)",
        T*3 + "{",
        T*4 + "const double dNow = (double)timeGetTime();",
        T*4 + "if (s_dKeTiep == 0.0 || dNow - s_dKeTiep > 250.0 || dNow < s_dKeTiep - 250.0)",
        T*5 + "s_dKeTiep = dNow;" + T + "// lan dau / treo lau / dong ho quay vong: neo lai",
        T*4 + "if (dNow < s_dKeTiep)",
        T*5 + "break;" + T + "// chua toi luot: giu vi tri ve cua khung truoc (nRet giu 1)",
        T*4 + "s_dKeTiep += 1000.0 / (double)s_nNhipTG;" + T + "// tich luy, khong neo vao khung => trung binh dung nhip",
        T*3 + "}",
        T*2 + "}",
        a,
    ])
    s = rep(s, a, gate, "CS gate")
    ghi(F_CS, s, h0, "CoreShell.cpp")
else:
    print("CoreShell.cpp da co")

for f in (F_ENUM, F_FH, F_FC, F_R3, F_NPC, F_CS):
    t = doc(f)
    print("  %-22s CRLF %5d | LF don %d | byte cao %d" % (f.rsplit("\\", 1)[-1], t.count("\r\n"), t.count("\n") - t.count("\r\n"), sum(1 for c in t if ord(c) >= 0x80)))
print("XONG NHIP60 + CHUDAM + TENNEN")
