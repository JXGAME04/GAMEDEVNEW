# -*- coding: utf-8 -*-
"""go_cac_ban_do_sai_0809.py - [DON 08/09] Chu yeu cau: bo cac ban va SAI va cac bo do da dung xong, de khong lam roi
ma nguon va config. Go het nhung thu duoi day (deu da bi so do bac bo hoac da xong viec):
  [TENMAU]  dem phe theo khoang cach            - KNpc.cpp                 (da chung minh mau phe DUNG)
  [VETRUNG] dem NPC bi ve trung trong mot khung - CoreDrawGameObj.cpp, CoreShell.cpp (ket qua 0,00 - khong ai bi ve hai lan)
  [NHIPVE]  ghi vi tri ve tung khung            - CoreShell.cpp            (da lay du so lieu, sua xong [NHIP d])
  [CHU]     khoa VienChu                        - KFont3.cpp, KRepresentShell3.cpp (khong phai nguyen nhan; dat 1 = y het cu)
  [CHUP]    khoa ChupThem chup lien tiep        - KRepresentShell3.cpp     (khong dung toi)
  [SANG]    khoa AnhSang / AnhSangNen           - KIpoTree.cpp             (tat han van bi -> khong phai nguyen nhan)
GIU LAI (deu la sua dung, khong phai bo do): [MOCOI] nut canh mo coi, [NHIP c/d/e] dong ho noi suy + PaintFps chan
tran khi vsync, [VE c] bo o ve rong, [MAU] so sat thuong theo tick, [NAP e] tep thieu, [WORLD] do tick the gioi
(dang can cho viec tut FPS luc dong)."""
import io, re, sys

NL = "\r\n"
SRC = r"D:\GAMEDEVNEW_wt_delta\Sources"


def sua(rel, ham):
    p = SRC + "\\" + rel
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    h0 = sum(1 for c in s if ord(c) >= 0x80)
    s2 = ham(s)
    if s2 is None:
        print("   bo qua %s (khong thay neo)" % rel)
        return
    if sum(1 for c in s2 if ord(c) >= 0x80) != h0:
        print("FAIL byte cao", rel); sys.exit(1)
    if re.search(r"[^\r]\n", s2):
        print("FAIL LF", rel); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s2)
    print("OK %s" % rel)


def cat_khoi(s, dau, cuoi, gom_cuoi=True):
    """xoa tu chuoi 'dau' den het chuoi 'cuoi' (lan xuat hien dau tien sau 'dau')"""
    i = s.find(dau)
    if i < 0:
        return None
    j = s.find(cuoi, i)
    if j < 0:
        return None
    return s[:i] + (s[j + len(cuoi):] if gom_cuoi else s[j:])


# ---- KNpc.cpp: [TENMAU]
def f_knpc(s):
    s2 = cat_khoi(s, "// [TENMAU 08/09 a] Do mau ten theo khoang cach", "int KNpc::PaintInfo(", False)
    if s2 is None:
        return None
    if "int KNpc::PaintInfo(int nHeightOffset" not in s2:
        return None
    # go cho goi trong than ham
    i = s2.find("\t\tif (g_nCorePaintLog > 0)" + NL + "\t\t{\t// [TENMAU 08/09 a]")
    if i >= 0:
        j = s2.find("\t\t}" + NL, s2.find("TenMauInDong();", i))
        s2 = s2[:i] + s2[j + len("\t\t}" + NL):]
    return s2 if "TenMau" not in s2 else None


# ---- CoreDrawGameObj.cpp: [VETRUNG]
def f_draw(s):
    s2 = cat_khoi(s, "// [VETRUNG 08/09] DEM ve trung", "void\tCoreDrawGameObj(unsigned int uObjGenre")
    if s2 is None:
        return None
    s2 = "void\tCoreDrawGameObj(unsigned int uObjGenre".join([s2, ""]) if False else s2
    if "void\tCoreDrawGameObj(unsigned int uObjGenre" not in s2:
        return None
    s2 = s2.replace("\t\t\t\tif (g_nCorePaintLog > 0) { VeTrungGhi(uId); VeTrungInDong(); }\t// [VETRUNG 08/09]" + NL, "")
    return s2 if "VeTrung" not in s2 else None


# ---- CoreShell.cpp: [VETRUNG] seq + [NHIPVE]
def f_shell(s):
    s = s.replace("static int g_nDoNhipVe = -1;\t// [NHIPVE 08/09] so khung con phai ghi (-1 = chua doc cau hinh)" + NL, "")
    s = s.replace("unsigned g_uPaintFrameSeq = 0;\t// [VETRUNG 08/09] tang mot lan moi khung ve (POSSHIFT chay dung mot lan/khung)" + NL, "")
    s = s.replace("\t\tg_uPaintFrameSeq++;\t// [VETRUNG 08/09]" + NL, "")
    i = s.find("\t\tif (g_nDoNhipVe < 0)")
    if i >= 0:
        j = s.find("\t\t}" + NL, s.find("\t\t\t}" + NL, s.find("fprintf(pLog, \"[NHIPVE]", i)))
        s = s[:i] + s[j + len("\t\t}" + NL):]
    return s if ("g_nDoNhipVe" not in s and "g_uPaintFrameSeq" not in s) else None


# ---- KFont3.cpp: [CHU]
def f_font(s):
    i = s.find("\t// [CHU 08/09] Chu thich cua ham noi")
    if i < 0:
        return None
    j = s.find("\t\ta = 0xFF;" + NL, i)
    if j < 0:
        return None
    return s[:i] + "\tif (a == 0) // " + NL + "\t\ta = 0xFF; //" + NL + s[j + len("\t\ta = 0xFF;" + NL):]


# ---- KRepresentShell3.cpp: [CHU] + [CHUP]
def f_rep3(s):
    s = s.replace("int  g_nRep3VienChu   = 1;\t// [CHU 08/09] 1 = ep vien den cho moi chuoi chu (nhu tu truoc); 0 = ton trong ben goi (alpha 0 = khong vien)" + NL, "")
    s = s.replace("\tg_nRep3VienChu   = Rep3Ini(\"VienChu\", 1);\t// [CHU 08/09]" + NL, "")
    s = s.replace("int  g_nRep3ChupThem  = 0;\t// [CHUP 08/09] so khung chup THEM sau moi lan bam phim chup (0 = nhu cu)" + NL, "")
    s = s.replace("// [CHUP 08/09] chup lien tiep: giu ten goc + so khung con phai chup + kieu/chat luong" + NL +
                  "static char  s_szChupGoc[260] = {0};" + NL +
                  "static int   s_nChupConLai = 0, s_nChupKieu = 0, s_nChupChatLuong = 0, s_nChupSo = 0;" + NL, "")
    for a in ("\tg_nRep3ChupThem  = Rep3Ini(\"ChupThem\", 0);\t// [CHUP 08/09] kep 0..30" + NL,
              "\tif (g_nRep3ChupThem < 0) g_nRep3ChupThem = 0;" + NL,
              "\tif (g_nRep3ChupThem > 30) g_nRep3ChupThem = 30;" + NL):
        s = s.replace(a, "")
    i = s.find("\tif (g_nRep3ChupThem > 0 && s_nChupConLai == 0)")
    if i >= 0:
        j = s.find("\t}" + NL, i)
        s = s[:i] + s[j + len("\t}" + NL):]
    i = s.find("\tif (s_nChupConLai > 0 && s_szChupGoc[0])")
    if i >= 0:
        j = s.find("\t}" + NL + NL, i)
        s = s[:i] + s[j + len("\t}" + NL + NL):]
    return s if ("g_nRep3VienChu" not in s and "ChupThem" not in s and "s_nChupConLai" not in s) else None


# ---- KIpoTree.cpp: [SANG]
def f_ipo(s):
    s2 = cat_khoi(s, "// [SANG 08/09] hai nut cho he chieu sang dong", "void KIpoTree::Paint(RECT* pRepresentArea", False)
    if s2 is None:
        return None
    if "void KIpoTree::Paint(RECT* pRepresentArea" not in s2:
        return None
    s2 = s2.replace("\tAnhSangDocCauHinh();\t// [SANG 08/09]" + NL, "")
    i = s2.find("\tif (eLayer == IPOT_RL_COVER_GROUND && g_nAnhSang == 0")
    if i >= 0:
        j = s2.find("\telse if(eLayer == IPOT_RL_COVER_GROUND && m_bDynamicLighting", i)
        s2 = s2[:i] + "\tif(eLayer == IPOT_RL_COVER_GROUND && m_bDynamicLighting" + s2[j + len("\telse if(eLayer == IPOT_RL_COVER_GROUND && m_bDynamicLighting"):]
    i = s2.find("\t// [SANG 08/09] nen cua ban do sang.")
    if i >= 0:
        j = s2.find("\t}" + NL, s2.find("m_dwAmbient = 0xff000000", i))
        s2 = s2[:i] + "\tm_dwAmbient = 0xff101010;" + NL + s2[j + len("\t}" + NL):]
    return s2 if "AnhSang" not in s2 else None


sua(r"Core\Src\KNpc.cpp", f_knpc)
sua(r"Core\Src\CoreDrawGameObj.cpp", f_draw)
sua(r"Core\Src\CoreShell.cpp", f_shell)
sua(r"Represent\iRepresent\Font\KFont3.cpp", f_font)
sua(r"Represent\Represent3\KRepresentShell3.cpp", f_rep3)
sua(r"Core\Src\Scene\KIpoTree.cpp", f_ipo)
print("XONG DON")
