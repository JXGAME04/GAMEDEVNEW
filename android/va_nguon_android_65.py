# -*- coding: utf-8 -*-
#
# [ANDROID 10/09 BANGCHON] + [ANDROID 10/09 BUFF]  -  chu (10/09 14:xx), CHI ban mobile:
#   1. "o ky nang chinh khong doi ky nang duoc"            -> o chinh cung gan duoc (dat lam ky nang danh
#      TRAI cua Core + nho "Chinh=" trong KyNangMobile.ini, mo game ap lai).
#   2. "mo bang ky nang, bam ky nang nao thi hien thong tin va kem 3 nut gan chinh / gan phu / go"
#      -> bang 3 nut ve tay (JxKyNang_MoBangChon / ChamBangChon), mo tu OnSkillPickDrop.
#   3. "khong cho lay ky nang len tay khi bam vao xem thong tin" -> OnSkillPickDrop (Android) khong bao
#      gio nhac len nua.
#   5. "o phu bo ky nang buff thi chi can kich vao se tu su dung" -> Core tra them nTuDung (khong nham ke
#      dich, len minh/dong doi); cham la UseSkill ngay tai cho nhan vat (giua khung ve).
#   (4. "phai Nga My ... tu luan chuyen": chua ro y, hoi lai chu - chua lam.)
# Ban PC: GameDataDef/CoreShell chi them truong + tinh them, khong doi hanh vi; UI ky nang guard JX_ANDROID.

import io
import os
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes  # noqa: E402

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
DAU = "[ANDROID 10/09 BANGCHON]"


def vn(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay(s, cu, moi, ten):
    """cu/moi: danh sach dong. Thu CRLF truoc, roi LF. Phai dung 1 cho."""
    for nl in ("\r\n", "\n"):
        c = nl.join(cu)
        if s.count(c) == 1:
            return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung 1 cho: %s" % ten)


# ============================================================ 1. Core: struct + GDI_KYNANG_MOBILE
P = "Sources/Core/Src/GameDataDef.h"
s = doc(P)
if "nTuDung" in s:
    print("da co nTuDung, bo qua:", P)
else:
    s = thay(s, [
        "\tint\tnLaAura;\t// RA  : 1 = ky nang TRO (bat/tat) chu khong phai danh",
        "};",
    ], [
        "\tint\tnLaAura;\t// RA  : 1 = ky nang TRO (bat/tat) chu khong phai danh",
        "\t// [ANDROID 10/09 BUFF] them o CUOI de khong doi bo cuc phan tren",
        "\tint\tnTuDung;\t// RA  : 1 = ky nang TU DUNG (buff / hoi phuc len minh hoac dong doi, khong nham ke dich)",
        "\tchar szTen[64];\t// RA  : ten ky nang (ghi tren bang chon)",
        "};",
    ], "struct KJxKyNangHoi")
    ghi(P, s)
    print("da va:", P)

P = "Sources/Core/Src/CoreShell.cpp"
s = doc(P)
if "nTuDung" in s:
    print("da co nTuDung, bo qua:", P)
else:
    s = thay(s, ["\t\tpHoi->nLaAura = 0;"], [
        "\t\tpHoi->nLaAura = 0;",
        "\t\tpHoi->nTuDung = 0;\t// [ANDROID 10/09 BUFF]",
        "\t\tpHoi->szTen[0] = 0;",
    ], "khoi tao pHoi")
    s = thay(s, ["\t\tpHoi->nLaAura  = pKN->IsAura() ? 1 : 0;"], [
        "\t\tpHoi->nLaAura  = pKN->IsAura() ? 1 : 0;",
        "\t\t// [ANDROID 10/09 BUFF] ky nang KHONG nham ke dich ma len MINH / DONG DOI (buff, hoi phuc, doi trang thai)",
        "\t\tpHoi->nTuDung = (!pKN->IsTargetEnemy() && (pKN->IsTargetSelf() || pKN->IsTargetAlly()",
        "\t\t\t|| pKN->GetSkillStyle() == SKILL_SS_InitiativeNpcState)) ? 1 : 0;",
        "\t\tif (pKN->GetSkillName())",
        "\t\t{",
        "\t\t\tstrncpy(pHoi->szTen, pKN->GetSkillName(), sizeof(pHoi->szTen) - 1);",
        "\t\t\tpHoi->szTen[sizeof(pHoi->szTen) - 1] = 0;",
        "\t\t}",
    ], "pHoi->nLaAura tinh")
    ghi(P, s)
    print("da va:", P)

# ============================================================ 2. JxCanDieuKhien.cpp
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# 2a. include cua so ky nang (de tu dong bang khi cua so dong)
s = thay(s, ['#include "../Ui/ShortcutKey.h"\t// [ANDROID 10/09 OTRONG] mo bang ky nang khi cham o trong'], [
    '#include "../Ui/ShortcutKey.h"\t// [ANDROID 10/09 OTRONG] mo bang ky nang khi cham o trong',
    '#include "../Ui/UiCase/UiSkillsNew.h"\t// %s tu dong bang chon khi cua so ky nang dong' % DAU,
    '#include "../Ui/UiCase/UiSkills.h"',
], "include ShortcutKey")

# 2b. trang thai
s = thay(s, ["static int\t\t\ts_nKNDaDocGan = 0;"], [
    "static int\t\t\ts_nKNDaDocGan = 0;",
    "//\t%s bang 3 nut khi cham mot ky nang trong bang ky nang; o CHINH cung gan duoc" % DAU,
    "#define\tKYNANG_CHON_CHINH\t100\t\t// s_nKNOChon = dang chon O CHINH",
    "#define\tBC_RONG\t\t\t\t176",
    "#define\tBC_CAO_NUT\t\t\t34",
    "#define\tBC_CAO_TEN\t\t\t22",
    "static int\t\t\ts_nBCBat = 0;\t\t// bang chon dang mo",
    "static KUiGameObject s_BCKN;\t\t\t// ky nang dang chon tren bang",
    "static char\t\t\ts_szBCTen[64];",
    "static int\t\t\ts_nBCX = 0, s_nBCY = 0;",
    "static KUiGameObject s_KNChinhGan;\t\t// ky nang nguoi choi gan cho O CHINH (Chinh= trong KyNangMobile.ini)",
    "static int\t\t\ts_nKNDaApChinh = 0;",
    "static KUiGameObject s_KNCho;\t\t\t// ky nang dang cho nguoi choi cham o phu de gan (tu bang chon)",
    "static char\t\t\ts_szKNBao[96];\t\t// dong thong bao ngan tren man (3 giay)",
    "static unsigned int\ts_uKNBaoLuc = 0;",
    "",
    "static void KyNang_Bao(const char* pszChu)",
    "{",
    "\tstrncpy(s_szKNBao, pszChu, sizeof(s_szKNBao) - 1);",
    "\ts_szKNBao[sizeof(s_szKNBao) - 1] = 0;",
    "\ts_uKNBaoLuc = (unsigned int)GetTickCount();",
    "}",
], "khai bao s_nKNDaDocGan")

# 2c. DocGan / GhiGan: them Chinh=
s = thay(s, ["\tmemset(s_KNGan, 0, sizeof(s_KNGan));"], [
    "\tmemset(s_KNGan, 0, sizeof(s_KNGan));",
    "\tmemset(&s_KNChinhGan, 0, sizeof(s_KNChinhGan));\t// %s" % DAU,
], "memset s_KNGan")
s = thay(s, [
    "\t\t\ts_KNGan[nO].uId    = (unsigned int)nMa;",
    "\t\t}",
    "\t}",
    "\tfclose(pTep);",
], [
    "\t\t\ts_KNGan[nO].uId    = (unsigned int)nMa;",
    "\t\t}",
    "\t\telse if (sscanf(szDong, \"Chinh=%d,%d\", &nLoai, &nMa) == 2)",
    "\t\t{\t// %s ky nang gan cho O CHINH" % DAU,
    "\t\t\ts_KNChinhGan.uGenre = (unsigned int)nLoai;",
    "\t\t\ts_KNChinhGan.uId    = (unsigned int)nMa;",
    "\t\t}",
    "\t}",
    "\tfclose(pTep);",
], "DocGan vong lap")
s = thay(s, [
    "\t\tif (s_KNGan[i].uId)",
    "\t\t\tfprintf(pTep, \"O%d=%u,%u\\n\", i, s_KNGan[i].uGenre, s_KNGan[i].uId);",
    "\t}",
    "\tfclose(pTep);",
], [
    "\t\tif (s_KNGan[i].uId)",
    "\t\t\tfprintf(pTep, \"O%d=%u,%u\\n\", i, s_KNGan[i].uGenre, s_KNGan[i].uId);",
    "\t}",
    "\tif (s_KNChinhGan.uId)\t// %s" % DAU,
    "\t\tfprintf(pTep, \"Chinh=%u,%u\\n\", s_KNChinhGan.uGenre, s_KNChinhGan.uId);",
    "\tfclose(pTep);",
], "GhiGan")

# 2d. HoiCore: tra them nTuDung + ten
s = thay(s, [
    "static bool KyNang_HoiCore(int nSkillId, int* pTamDanh, int* pLaAura)",
    "{",
    "\tKJxKyNangHoi oHoi;",
    "",
    "\tif (pTamDanh) *pTamDanh = 0;",
    "\tif (pLaAura)  *pLaAura  = 0;",
], [
    "static bool KyNang_HoiCore(int nSkillId, int* pTamDanh, int* pLaAura, int* pTuDung = NULL, char* pszTen = NULL)",
    "{",
    "\tKJxKyNangHoi oHoi;",
    "",
    "\tif (pTamDanh) *pTamDanh = 0;",
    "\tif (pLaAura)  *pLaAura  = 0;",
    "\tif (pTuDung)  *pTuDung  = 0;\t// [ANDROID 10/09 BUFF]",
    "\tif (pszTen)   pszTen[0] = 0;",
], "HoiCore dau")
s = thay(s, [
    "\tif (pLaAura)  *pLaAura  = oHoi.nLaAura;",
], [
    "\tif (pLaAura)  *pLaAura  = oHoi.nLaAura;",
    "\tif (pTuDung)  *pTuDung  = oHoi.nTuDung;\t// [ANDROID 10/09 BUFF]",
    "\tif (pszTen)",
    "\t{",
    "\t\tstrncpy(pszTen, oHoi.szTen, 63);",
    "\t\tpszTen[63] = 0;",
    "\t}",
], "HoiCore ra")

# 2e. DocBang: ap o chinh da gan (mot lan)
s = thay(s, ["\ts_KNChinh = oTay.IMmediaSkill[0];"], [
    "\ts_KNChinh = oTay.IMmediaSkill[0];",
    "\t// %s o chinh nguoi choi tu gan (Chinh= trong KyNangMobile.ini): ap MOT lan khi da vao game" % DAU,
    "\t// (co danh sach ky nang) va ky nang danh trai hien tai khac.",
    "\tKyNang_DocGan();",
    "\tif (!s_nKNDaApChinh && s_nKNCo1 > 0 && s_KNChinhGan.uId)",
    "\t{",
    "\t\ts_nKNDaApChinh = 1;",
    "\t\tif (s_KNChinh.uId != s_KNChinhGan.uId)",
    "\t\t{",
    "\t\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&s_KNChinhGan, 0);",
    "\t\t\tg_DebugLog(\"[KYNANG] ap ky nang o chinh da gan: %u\", s_KNChinhGan.uId);",
    "\t\t}",
    "\t}",
], "DocBang s_KNChinh")

# 2f. BatDau: che do gan - o chinh + ky nang cho san
s = thay(s, [
    "\tif (s_nKNCheDoGan)",
    "\t{",
    "\t\ts_nKNOChon = (nNut - 1 > 0) ? (nNut - 2) : -1;\t// chi o phu moi gan duoc",
    "\t\ts_nKNDangCam = -1;",
    "\t\treturn;",
    "\t}",
], [
    "\tif (s_nKNCheDoGan)",
    "\t{",
    "\t\t// %s o CHINH cung gan duoc (chu: \"o ky nang chinh khong doi ky nang duoc\")" % DAU,
    "\t\ts_nKNOChon = (nNut == 1) ? KYNANG_CHON_CHINH : (nNut - 2);",
    "\t\ts_nKNDangCam = -1;",
    "\t\t// dang co ky nang cho san (bam \"Gan o phu\" tren bang chon luc het o trong): gan ngay vao o vua cham",
    "\t\tif (s_KNCho.uId)",
    "\t\t{",
    "\t\t\tKUiGameObject oCho = s_KNCho;",
    "",
    "\t\t\tmemset(&s_KNCho, 0, sizeof(s_KNCho));",
    "\t\t\tJxKyNang_GanKyNang(oCho.uGenre, oCho.uId);",
    "\t\t}",
    "\t\treturn;",
    "\t}",
], "BatDau che do gan")

# 2g. GanKyNang: o chinh
s = thay(s, [
    "bool JxKyNang_GanKyNang(unsigned int uGenre, unsigned int uId)",
    "{",
    "\tif (!s_nKNCheDoGan || s_nKNOChon < 0 || s_nKNOChon >= KYNANG_SO_PHU)",
    "\t\treturn false;",
    "\tif (uId == 0)",
    "\t\treturn false;",
], [
    "bool JxKyNang_GanKyNang(unsigned int uGenre, unsigned int uId)",
    "{",
    "\tif (!s_nKNCheDoGan || uId == 0)",
    "\t\treturn false;",
    "\t// %s o CHINH: dat lam ky nang danh TRAI cua Core va nho lai de mo game van con" % DAU,
    "\tif (s_nKNOChon == KYNANG_CHON_CHINH)",
    "\t{",
    "\t\tKUiGameObject o;",
    "",
    "\t\tmemset(&o, 0, sizeof(o));",
    "\t\to.uGenre = uGenre;",
    "\t\to.uId    = uId;",
    "\t\tKyNang_DocGan();",
    "\t\ts_KNChinhGan = o;",
    "\t\tKyNang_GhiGan();",
    "\t\tif (g_pCoreShell)",
    "\t\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 0);",
    "\t\tKyNang_Bao(\"%s\");" % vn(u"Đã gắn vào ô chính"),
    "\t\tg_DebugLog(\"[KYNANG] gan ky nang %u vao O CHINH\", uId);",
    "\t\ts_nKNOChon = -1;",
    "\t\ts_nKNCheDoGan = 0;",
    "\t\treturn true;",
    "\t}",
    "\tif (s_nKNOChon < 0 || s_nKNOChon >= KYNANG_SO_PHU)",
    "\t\treturn false;",
], "GanKyNang dau")

# 2h. DanhMotPhat: buff cham la dung
s = thay(s, [
    "\tif (!KyNang_HoiCore((int)o.uId, &nTam, &nAura))",
    "\t\treturn;",
    "\tif (nAura)",
], [
    "\tint nTuDung = 0;",
    "",
    "\tif (!KyNang_HoiCore((int)o.uId, &nTam, &nAura, &nTuDung))",
    "\t\treturn;",
    "\tif (!nAura && nTuDung)",
    "\t{",
    "\t\t// [ANDROID 10/09 BUFF] ky nang TU DUNG (buff / hoi phuc len minh, dong doi): cham la dung NGAY tai",
    "\t\t// cho nhan vat (nhan vat luon o giua khung ve), khong can keo ngam, khong can co ke dich - chu:",
    "\t\t// \"cac o phu bo cac ky nang buff thi chi can kich vao se tu su dung\". Dung mot lan moi cham.",
    "\t\tif (nNut > 0)",
    "\t\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 1);",
    "\t\tg_pCoreShell->UseSkill(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, (int)o.uId);",
    "\t\ts_nKNDangCam = -1;\t\t// khong lap lai khi con giu",
    "\t\treturn;",
    "\t}",
    "\tif (nAura)",
], "DanhMotPhat HoiCore")

# 2i. bang chon: ham cong khai + ve, dat truoc JxKyNang_Ve
BANG = [
    "//---------------------------------------------------------------------------",
    "// %s BANG 3 NUT khi cham mot ky nang trong bang ky nang (chu: \"mo bang ky nang ra bam vao" % DAU,
    "// ky nang nao thi hien thong tin ky nang va kem 3 nut gan ky nang chinh - ky nang phu - go ky nang khoi o\").",
    "// Thong tin ky nang: cua so ky nang tu hien khi con tro dung tren o (cham = con tro toi do, KSdlApp).",
    "static void HopChuNhat(int x0, int y0, int x1, int y1, unsigned int uMau)",
    "{",
    "\tKRUShadow o;",
    "",
    "\to.oPosition.nX = x0; o.oPosition.nY = y0;",
    "\to.oEndPos.nX = x1;   o.oEndPos.nY = y1;",
    "\to.Color.Color_dw = uMau;",
    "\tg_pRepresentShell->DrawPrimitives(1, &o, RU_T_SHADOW, true);",
    "}",
    "",
    "void JxKyNang_MoBangChon(unsigned int uGenre, unsigned int uId, int x, int y)",
    "{",
    "\tint nCao = BC_CAO_TEN + BC_CAO_NUT * 3;",
    "",
    "\tif (uId == 0)",
    "\t\treturn;",
    "\tmemset(&s_BCKN, 0, sizeof(s_BCKN));",
    "\ts_BCKN.uGenre = uGenre;",
    "\ts_BCKN.uId    = uId;",
    "\tKyNang_HoiCore((int)uId, NULL, NULL, NULL, s_szBCTen);",
    "\ts_nBCX = x + 6;",
    "\ts_nBCY = y - 8;",
    "\tif (s_nBCX + BC_RONG > SCREEN_WIDTH - 4)\ts_nBCX = x - 50 - BC_RONG;",
    "\tif (s_nBCX < 4)\t\t\t\t\t\t\ts_nBCX = 4;",
    "\tif (s_nBCY + nCao > SCREEN_HEIGHT - 4)\ts_nBCY = SCREEN_HEIGHT - 4 - nCao;",
    "\tif (s_nBCY < 4)\t\t\t\t\t\t\ts_nBCY = 4;",
    "\ts_nBCBat = 1;",
    "\tg_DebugLog(\"[KYNANG] bang chon: ky nang %u (%s) tai %d,%d\", uId, s_szBCTen, s_nBCX, s_nBCY);",
    "}",
    "",
    "//\tGo ky nang khoi moi o phu dang giu no. O chinh khong go duoc (Core luon can mot ky nang danh trai).",
    "static void KyNang_GoKhoiO(const KUiGameObject* p)",
    "{",
    "\tint i, nGo = 0;",
    "",
    "\tKyNang_DocGan();",
    "\tfor (i = 0; i < KYNANG_SO_PHU; i++)",
    "\t{",
    "\t\tif (s_KNGan[i].uId == p->uId && s_KNGan[i].uGenre == p->uGenre)",
    "\t\t{",
    "\t\t\tmemset(&s_KNGan[i], 0, sizeof(s_KNGan[i]));",
    "\t\t\tnGo++;",
    "\t\t}",
    "\t}",
    "\tif (nGo)",
    "\t{",
    "\t\tKyNang_GhiGan();",
    "\t\tKyNang_Bao(\"%s\");" % vn(u"Đã gỡ khỏi ô phụ"),
    "\t}",
    "\telse if (s_KNChinh.uId == p->uId)",
    "\t\tKyNang_Bao(\"%s\");" % vn(u"Ô chính chỉ thay được, không gỡ được"),
    "\telse",
    "\t\tKyNang_Bao(\"%s\");" % vn(u"Kỹ năng này chưa nằm ở ô nào"),
    "\tg_DebugLog(\"[KYNANG] go ky nang %u khoi %d o\", p->uId, nGo);",
    "}",
    "",
    "int JxKyNang_ChamBangChon(int x, int y)",
    "{",
    "\tint nCao = BC_CAO_TEN + BC_CAO_NUT * 3;",
    "\tint nNut;",
    "\tKUiGameObject o;",
    "",
    "\tif (!s_nBCBat)",
    "\t\treturn 0;",
    "\tif (x < s_nBCX || x >= s_nBCX + BC_RONG || y < s_nBCY || y >= s_nBCY + nCao)",
    "\t{",
    "\t\ts_nBCBat = 0;\t\t// cham ra ngoai: dong bang, cu cham di tiep",
    "\t\treturn 0;",
    "\t}",
    "\tnNut = (y - s_nBCY - BC_CAO_TEN) / BC_CAO_NUT;\t// < 0 = dong ten",
    "\to = s_BCKN;",
    "\ts_nBCBat = 0;",
    "\tif (y - s_nBCY < BC_CAO_TEN)",
    "\t\treturn 1;",
    "\tif (nNut == 0)",
    "\t{\t// gan o CHINH",
    "\t\ts_nKNCheDoGan = 1;",
    "\t\ts_nKNOChon = KYNANG_CHON_CHINH;",
    "\t\tJxKyNang_GanKyNang(o.uGenre, o.uId);",
    "\t}",
    "\telse if (nNut == 1)",
    "\t{\t// gan o PHU: o da chon truoc (cham o trong) -> gan luon; khong thi o trong dau tien; het o thi cho cham",
    "\t\tint i;",
    "",
    "\t\tKyNang_DocGan();",
    "\t\tif (s_nKNCheDoGan && s_nKNOChon >= 0 && s_nKNOChon < KYNANG_SO_PHU)",
    "\t\t\tJxKyNang_GanKyNang(o.uGenre, o.uId);",
    "\t\telse",
    "\t\t{",
    "\t\t\tfor (i = 0; i < KYNANG_SO_PHU; i++)",
    "\t\t\t{",
    "\t\t\t\tif (s_KNGan[i].uId == 0)",
    "\t\t\t\t\tbreak;",
    "\t\t\t}",
    "\t\t\tif (i < KYNANG_SO_PHU)",
    "\t\t\t{",
    "\t\t\t\tchar szBao[96];",
    "",
    "\t\t\t\ts_nKNCheDoGan = 1;",
    "\t\t\t\ts_nKNOChon = i;",
    "\t\t\t\tJxKyNang_GanKyNang(o.uGenre, o.uId);",
    "\t\t\t\t_snprintf(szBao, sizeof(szBao), \"%%s %%d\", \"%s\", i + 1);" % vn(u"Đã gắn vào ô phụ"),
    "\t\t\t\tszBao[sizeof(szBao) - 1] = 0;",
    "\t\t\t\tKyNang_Bao(szBao);",
    "\t\t\t}",
    "\t\t\telse",
    "\t\t\t{",
    "\t\t\t\ts_KNCho = o;",
    "\t\t\t\ts_nKNCheDoGan = 1;",
    "\t\t\t\ts_nKNOChon = -1;",
    "\t\t\t\tKyNang_Bao(\"%s\");" % vn(u"Hết ô trống: chạm ô phụ muốn thay"),
    "\t\t\t}",
    "\t\t}",
    "\t}",
    "\telse",
    "\t\tKyNang_GoKhoiO(&o);",
    "\treturn 1;",
    "}",
    "",
    "static void KyNang_VeBangChon()",
    "{",
    "\tstatic const char* s_szNut[3] = { \"%s\", \"%s\", \"%s\" };" % (vn(u"Gắn ô chính"), vn(u"Gắn ô phụ"), vn(u"Gỡ khỏi ô")),
    "\tint i, nY;",
    "",
    "\tif (!s_nBCBat || g_pRepresentShell == NULL)",
    "\t\treturn;",
    "\t// cua so ky nang dong roi thi bang cung dong",
    "\tif (KUiSkillsNew::GetIfVisible() == NULL && KUiSkills::GetIfVisible() == NULL)",
    "\t{",
    "\t\ts_nBCBat = 0;",
    "\t\treturn;",
    "\t}",
    "\tHopChuNhat(s_nBCX, s_nBCY, s_nBCX + BC_RONG, s_nBCY + BC_CAO_TEN + BC_CAO_NUT * 3, 0xE0101820);",
    "\tKyNang_VeChu(s_szBCTen[0] ? s_szBCTen : \"%s\", s_nBCX + 8, s_nBCY + 4, 0xFFFFD24A);" % vn(u"Kỹ năng"),
    "\tnY = s_nBCY + BC_CAO_TEN;",
    "\tfor (i = 0; i < 3; i++, nY += BC_CAO_NUT)",
    "\t{",
    "\t\tHopChuNhat(s_nBCX + 4, nY + 3, s_nBCX + BC_RONG - 4, nY + BC_CAO_NUT - 3, i == 2 ? 0xC0603030 : 0xC0304868);",
    "\t\tKyNang_VeChu(s_szNut[i], s_nBCX + 14, nY + 10, 0xFFFFFFFF);",
    "\t}",
    "}",
    "",
]
s = thay(s, ["void JxKyNang_Ve()", "{"], BANG + ["void JxKyNang_Ve()", "{"], "dau JxKyNang_Ve")

# 2j. Ve: to sang o chinh dang chon; cuoi ham ve bang chon + thong bao
s = thay(s, ["\t\tif (s_nKNCheDoGan && i > 0 && (i - 1) == s_nKNOChon)"], [
    "\t\tif (s_nKNCheDoGan && ((i > 0 && (i - 1) == s_nKNOChon) || (i == 0 && s_nKNOChon == KYNANG_CHON_CHINH)))",
], "to sang o dang chon")
s = thay(s, [
    "\t\t\tVeAnh2(s_szKNAnhTen, s_nKNNgamX, s_nKNNgamY, nKhung);",
    "\t\t}",
    "\t}",
    "}",
    "",
    "void JxIconNpc_Ve()",
], [
    "\t\t\tVeAnh2(s_szKNAnhTen, s_nKNNgamX, s_nKNNgamY, nKhung);",
    "\t\t}",
    "\t}",
    "",
    "\t// %s bang 3 nut + dong thong bao ngan" % DAU,
    "\tKyNang_VeBangChon();",
    "\tif (s_szKNBao[0])",
    "\t{",
    "\t\tif ((unsigned int)GetTickCount() - s_uKNBaoLuc < 3000)",
    "\t\t\tKyNang_VeChu(s_szKNBao, SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 + 70, 0xFFFFD24A);",
    "\t\telse",
    "\t\t\ts_szKNBao[0] = 0;",
    "\t}",
    "}",
    "",
    "void JxIconNpc_Ve()",
], "cuoi JxKyNang_Ve")
ghi(P, s)
print("da va:", P)

# ============================================================ 3. JxCanDieuKhien.h
P = "Sources/S3Client/Platform/JxCanDieuKhien.h"
s = doc(P)
if "JxKyNang_MoBangChon" not in s:
    s = thay(s, ["bool JxKyNang_GanKyNang(unsigned int uGenre, unsigned int uId);"], [
        "bool JxKyNang_GanKyNang(unsigned int uGenre, unsigned int uId);",
        "// %s bang 3 nut (gan chinh / gan phu / go) khi cham mot ky nang trong bang ky nang." % DAU,
        "void JxKyNang_MoBangChon(unsigned int uGenre, unsigned int uId, int x, int y);",
        "int  JxKyNang_ChamBangChon(int x, int y);\t// 1 = bang dang mo da nhan cu cham nay",
    ], "khai bao .h")
    ghi(P, s)
    print("da va:", P)

# ============================================================ 4. UiSkillsNew.cpp + UiSkills.cpp
for P in ["Sources/S3Client/Ui/UiCase/UiSkillsNew.cpp", "Sources/S3Client/Ui/UiCase/UiSkills.cpp"]:
    s = doc(P)
    if DAU in s:
        print("da va roi, bo qua:", P)
        continue
    s = thay(s, [
        "\t\tif (oGan.uGenre != CGOG_NOTHING && JxKyNang_GanKyNang(oGan.uGenre, oGan.uId))",
        "\t\t\treturn;",
        "\t}",
        "#endif",
    ], [
        "\t\tif (oGan.uGenre != CGOG_NOTHING && !JxKyNang_GanKyNang(oGan.uGenre, oGan.uId))",
        "\t\t{",
        "\t\t\t// %s khong o che do gan: mo bang 3 nut (gan chinh / gan phu / go) ngay canh o." % DAU,
        "\t\t\t// Va KHONG BAO GIO nhac ky nang len tay nua (chu: \"khong cho lay ky nang len tay khi bam",
        "\t\t\t// vao xem thong tin ky nang\") - thong tin ky nang van hien nhu cu (con tro dung tren o).",
        "\t\t\tint nX = 0, nY = 0, nW = 0, nH = 0;",
        "",
        "\t\t\tpPickPos->pWnd->GetAbsolutePos(&nX, &nY);",
        "\t\t\tpPickPos->pWnd->GetSize(&nW, &nH);",
        "\t\t\tJxKyNang_MoBangChon(oGan.uGenre, oGan.uId, nX + nW, nY);",
        "\t\t}",
        "\t\treturn;",
        "\t}",
        "#endif",
    ], "hook OnSkillPickDrop " + P)
    ghi(P, s)
    print("da va:", P)

# ============================================================ 5. KSdlApp.cpp: bang chon nhan cham truoc
P = "Sources/S3Client/Platform/KSdlApp.cpp"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    s = thay(s, [
        "\t\t\t// [ANDROID 10/09 GANTOADO] Dang sua giao dien thi KHONG cho nut ky nang nuot",
        "\t\t\t// cu cham - phai de no di xuong UiToaDo thi moi keo cum nut di duoc.",
        "\t\t\tif (!UiToaDo_DangSua())",
        "\t\t\t{",
        "\t\t\t\tint nNutKN = JxKyNang_TrungNut(m_nChamX0, m_nChamY0);",
    ], [
        "\t\t\t// %s bang 3 nut canh ky nang dang mo: cham vao no thi bang nhan, khong cho roi" % DAU,
        "\t\t\t// xuong cua so ky nang ben duoi; cham ra ngoai thi bang tu dong, cu cham di tiep.",
        "\t\t\tif (JxKyNang_ChamBangChon(m_nChamX0, m_nChamY0))",
        "\t\t\t{",
        "\t\t\t\tm_nCham = CHAM_KHONG;",
        "\t\t\t\treturn true;",
        "\t\t\t}",
        "\t\t\t// [ANDROID 10/09 GANTOADO] Dang sua giao dien thi KHONG cho nut ky nang nuot",
        "\t\t\t// cu cham - phai de no di xuong UiToaDo thi moi keo cum nut di duoc.",
        "\t\t\tif (!UiToaDo_DangSua())",
        "\t\t\t{",
        "\t\t\t\tint nNutKN = JxKyNang_TrungNut(m_nChamX0, m_nChamY0);",
    ], "KSdlApp cham xuong")
    ghi(P, s)
    print("da va:", P)
