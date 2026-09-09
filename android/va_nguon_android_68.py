# -*- coding: utf-8 -*-
#
# [ANDROID 10/09 KHINHCONG b] + [ANDROID 10/09 LUAN b]  -  lam DUNG THEO BAN THAM KHAO (chu: "doc code de lam,
# khong duoc do mo"):
#
# 1. Khinh cong (210) - D:\USVOLAM\Jx1mClientMobile\Classes\gamescene\KgameWorld.cpp:4387 (R158/R171):
#      bPointCast = !TargetEnemy && !TargetAlly && !TargetSelf && !TargetObj && !TargetOther
#      huong = Npc.m_Dir (0 xuong, 16 trai, 32 len, 48 phai, nguoc chieu kim); goc = (-90 - 5,625*dir) do
#      tam = getBackAttackRadius() cua CAP DA HOC, it nhat 60
#      diem = (W/2 + cos*0,9*tam, H/2 - sin*0,45*tam)   (ep dan isometric; dang keo can thi theo huong can)
#    Nhat ky 14:2x cua chu: ta tha ve (520,662) = ngoai man hinh (huong can = 0, khong ep dan) va lap lai
#    moi 200 ms (cat ngang cu nhay). Nay: dung m_Dir + ep dan + tam theo cap + nhay MOT lan moi cham.
#    Them: xu ngua roi thi GIU LENH toi 6 giay de tu danh khi da xuong ngua (Core TIME_RIDE 5 giay).
# 2. Vong sang luan chuyen - KuiAutoPlay.cpp:5240 DoSkillBuff(): moi MAX_SKILLAURA_COUNT = 15 nhip (~0,5 s)
#    "bam" o vong sang KE TIEP theo thu tu o (auxiliaryskillCallback -> SetAuraSkill). Doi mac dinh 3000 -> 500 ms.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
DAU = "[ANDROID 10/09 KHINHCONG b]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay(s, cu, moi, ten, so=1):
    for nl in ("\r\n", "\n"):
        c = nl.join(cu)
        if s.count(c) == so:
            return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho: %s" % (so, ten))


# ============================================================ Core
P = "Sources/Core/Src/GameDataDef.h"
s = doc(P)
if "nTamCap" in s:
    print("da co nTamCap, bo qua:", P)
else:
    s = thay(s, ["\tint\tnCanDiem;\t// RA  : 1 = ky nang can mot DIEM (khinh cong...): khong nham ai ca (khong dich/minh/dong doi)", "};"], [
        "\tint\tnCanDiem;\t// RA  : 1 = ky nang can mot DIEM (khinh cong...): khong nham ai ca (khong dich/minh/dong doi)",
        "\tint\tnHuong;\t\t// RA  : huong nhan vat dang quay (Npc.m_Dir 0..63: 0 xuong, 16 trai, 32 len, 48 phai)",
        "\tint\tnTamCap;\t// RA  : tam danh theo CAP DA HOC (ban tham khao R171: radius theo cap)",
        "};",
    ], "struct nHuong/nTamCap")
    ghi(P, s)
    print("da va:", P)

P = "Sources/Core/Src/CoreShell.cpp"
s = doc(P)
if "nTamCap" in s:
    print("da co nTamCap, bo qua:", P)
else:
    s = thay(s, ["\t\tpHoi->nCanDiem = 0;\t// [ANDROID 10/09 KHINHCONG]"], [
        "\t\tpHoi->nCanDiem = 0;\t// [ANDROID 10/09 KHINHCONG]",
        "\t\tpHoi->nHuong = 0;",
        "\t\tpHoi->nTamCap = 0;",
    ], "khoi tao nHuong")
    s = thay(s, ["\t\tpHoi->nCanDiem = (!pKN->IsAura() && !pKN->IsTargetEnemy() && !pKN->IsTargetSelf() && !pKN->IsTargetAlly()) ? 1 : 0;"], [
        "\t\t// %s dung dieu kien bPointCast cua ban tham khao (KgameWorld.cpp:4394): khong nham AI ca" % DAU,
        "\t\tpHoi->nCanDiem = (!pKN->IsAura() && !pKN->IsTargetEnemy() && !pKN->IsTargetSelf() && !pKN->IsTargetAlly()",
        "\t\t\t&& !pKN->IsTargetObj()) ? 1 : 0;",
        "\t\tpHoi->nHuong = Npc[nToi].m_Dir;",
        "\t\t{\t// tam theo CAP DA HOC (ban tham khao R171: GetSkill(id, cap)->radius)",
        "\t\t\tint nCap = Npc[nToi].m_SkillList.GetCurrentLevel(pHoi->nSkillId);",
        "\t\t\tKSkill* pCap = (nCap > 1) ? (KSkill*)g_SkillManager.GetSkill(pHoi->nSkillId, nCap) : NULL;",
        "\t\t\tpHoi->nTamCap = pCap ? pCap->GetAttackRadius() : pKN->GetAttackRadius();",
        "\t\t}",
    ], "nCanDiem tinh")
    ghi(P, s)
    print("da va:", P)

# ============================================================ Client
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# 1. trang thai cho xuong ngua
s = thay(s, ["#define\tKYNANG_XOAY_KHUNG\t16"], [
    "#define\tKYNANG_XOAY_KHUNG\t16",
    "//\t%s xu ngua xong thi GIU LENH de tu danh khi da xuong (Core TIME_RIDE = 5 giay)" % DAU,
    "static unsigned int\ts_uKNDoiNguaDen = 0;\t// > 0: dang cho xuong/len ngua, thu lai toi luc nay",
    "static int\t\t\ts_nKNNgonGiu = 0;\t\t// ngon tay dang that su de tren nut",
], "khai bao KYNANG_XOAY_KHUNG")

# 2. luan chuyen: 500 ms nhu MAX_SKILLAURA_COUNT = 15 nhip (~0,5 s)
s = thay(s, ["static int\t\t\ts_nKNLuanMs = 3000;\t\t\t// [Cham] LuanChuyenMs"], [
    "static int\t\t\ts_nKNLuanMs = 500;\t\t\t// [Cham] LuanChuyenMs - ban tham khao KuiAutoPlay.cpp:92 MAX_SKILLAURA_COUNT 15 nhip ~0,5 s",
], "s_nKNLuanMs")
s = thay(s, ["\ts_nKNLuanMs = GetPrivateProfileInt(\"Cham\", \"LuanChuyenMs\", 3000, szCfg);\t// [ANDROID 10/09 LUAN]"], [
    "\ts_nKNLuanMs = GetPrivateProfileInt(\"Cham\", \"LuanChuyenMs\", 500, szCfg);\t// [ANDROID 10/09 LUAN] 0,5 s nhu ban tham khao",
], "DocCaiDat LuanChuyenMs")
s = thay(s, ["\tif (s_nKNLuanMs < 500) s_nKNLuanMs = 500;"], ["\tif (s_nKNLuanMs < 100) s_nKNLuanMs = 100;"], "chan LuanChuyenMs")

# 3. HoiCore: them huong + tam theo cap; xu ngua -> giu lenh
s = thay(s, ["\tint* pCanDiem = NULL, int bChiHoi = 0)"], ["\tint* pCanDiem = NULL, int bChiHoi = 0, int* pHuong = NULL, int* pTamCap = NULL)"], "HoiCore chu ky 68")
s = thay(s, ["\tif (pCanDiem) *pCanDiem = 0;\t// [ANDROID 10/09 KHINHCONG]"], [
    "\tif (pCanDiem) *pCanDiem = 0;\t// [ANDROID 10/09 KHINHCONG]",
    "\tif (pHuong)   *pHuong   = 0;\t// %s" % DAU,
    "\tif (pTamCap)  *pTamCap  = 0;",
], "HoiCore khoi tao 68")
s = thay(s, ["\tif (pCanDiem) *pCanDiem = oHoi.nCanDiem;\t// [ANDROID 10/09 KHINHCONG]"], [
    "\tif (pCanDiem) *pCanDiem = oHoi.nCanDiem;\t// [ANDROID 10/09 KHINHCONG]",
    "\tif (pHuong)   *pHuong   = oHoi.nHuong;\t\t// %s" % DAU,
    "\tif (pTamCap)  *pTamCap  = oHoi.nTamCap;",
], "HoiCore ra 68")
s = thay(s, [
    "\tif (oHoi.nNgua != 0)",
    "\t{",
    "\t\tg_DebugLog(\"[KYNANG] ky nang %d: xu ngua = %d -> lan nay chua danh\",",
    "\t\t\tnSkillId, oHoi.nNgua);",
    "\t\treturn false;",
    "\t}",
], [
    "\tif (oHoi.nNgua != 0)",
    "\t{",
    "\t\tg_DebugLog(\"[KYNANG] ky nang %d: xu ngua = %d -> lan nay chua danh\",",
    "\t\t\tnSkillId, oHoi.nNgua);",
    "\t\t// %s giu lenh toi 6 giay: JxKyNang_Nhip thu lai moi nhip, xuong ngua xong la danh" % DAU,
    "\t\tif (!bChiHoi && s_uKNDoiNguaDen == 0)",
    "\t\t\ts_uKNDoiNguaDen = (unsigned int)GetTickCount() + 6000;",
    "\t\treturn false;",
    "\t}",
], "HoiCore nNgua")

# 4. DanhMotPhat: hoi them huong/tam; diem theo ban tham khao; nhay mot lan
s = thay(s, [
    "\tint nTuDung = 0, nCanDiem = 0;",
    "",
    "\tif (!KyNang_HoiCore((int)o.uId, &nTam, &nAura, &nTuDung, NULL, &nCanDiem))",
    "\t\treturn;",
], [
    "\tint nTuDung = 0, nCanDiem = 0, nHuong = 0, nTamCap = 0;",
    "",
    "\tif (!KyNang_HoiCore((int)o.uId, &nTam, &nAura, &nTuDung, NULL, &nCanDiem, 0, &nHuong, &nTamCap))",
    "\t\treturn;",
    "\ts_uKNDoiNguaDen = 0;\t// %s hoi duoc = khong con vuong ngua" % DAU,
], "DanhMotPhat HoiCore 68")
i0 = s.index("\tif (nCanDiem && !bCoNgam)")
i1 = s.index("\tif (nNut > 0)", i0)
nl = "\r\n" if "\r\n" in s[i0:i1] else "\n"
DIEM = [
    "\tif (nCanDiem && !bCoNgam)",
    "\t{",
    "\t\t// %s LAM DUNG NHU BAN THAM KHAO (KgameWorld.cpp:4387, R158/R171): huong = m_Dir cua nhan vat" % DAU,
    "\t\t// (0 xuong, 16 trai, 32 len, 48 phai, nguoc chieu kim), goc = (-90 - 5,625*dir) do; tam theo CAP DA",
    "\t\t// HOC, it nhat 60; diem = (W/2 + cos*0,9*tam, H/2 - sin*0,45*tam) (ep dan isometric). Dang keo can",
    "\t\t// thi lay huong can. (Truoc: huong can + khong ep dan -> (520,662) ngoai man, NewJump fail im lang.)",
    "\t\tint nDir = JxCan_DangCam() ? s_nHuong : nHuong;",
    "\t\tdouble fA = (-90.0 - 5.625 * (double)nDir) * 3.14159265358979 / 180.0;",
    "\t\tdouble fR = (double)((nTamCap > 0) ? nTamCap : nTam);",
    "",
    "\t\tif (fR < 60.0) fR = 60.0;",
    "\t\tnNgamX = SCREEN_WIDTH / 2 + (int)(cos(fA) * 0.9 * fR);",
    "\t\tnNgamY = SCREEN_HEIGHT / 2 - (int)(sin(fA) * 0.45 * fR);",
    "\t\tbCoNgam = 1;",
    "\t\tg_DebugLog(\"[KYNANG] ky nang can diem %u: huong %d tam %d -> (%d,%d)\", o.uId, nDir, (int)fR, nNgamX, nNgamY);",
    "\t}",
    "",
]
s = s[:i0] + nl.join(DIEM) + s[i1:]
s = thay(s, [
    "\t\tif (bCoNgam)",
    "\t\t{",
    "\t\t\tg_pCoreShell->UseSkill(nNgamX, nNgamY, (int)o.uId);\t// tha ky nang o cho o xanh",
    "\t\t\treturn;",
    "\t\t}",
], [
    "\t\tif (bCoNgam)",
    "\t\t{",
    "\t\t\tg_pCoreShell->UseSkill(nNgamX, nNgamY, (int)o.uId);\t// tha ky nang o cho o xanh",
    "\t\t\tif (nCanDiem)",
    "\t\t\t\ts_nKNDangCam = -1;\t// %s nhay MOT lan moi cham; lap lai 200 ms se cat ngang cu nhay" % DAU,
    "\t\t\treturn;",
    "\t\t}",
], "cast ngam nut phu")
s = thay(s, [
    "\telse if (bCoNgam)",
    "\t{",
    "\t\tg_pCoreShell->UseSkill(nNgamX, nNgamY, (int)o.uId);\t// nut chinh, danh theo huong",
    "\t\treturn;",
    "\t}",
], [
    "\telse if (bCoNgam)",
    "\t{",
    "\t\tg_pCoreShell->UseSkill(nNgamX, nNgamY, (int)o.uId);\t// nut chinh, danh theo huong",
    "\t\tif (nCanDiem)",
    "\t\t\ts_nKNDangCam = -1;\t// %s" % DAU,
    "\t\treturn;",
    "\t}",
], "cast ngam nut chinh")

# 5. BatDau / Nha / Nhip: giu lenh khi cho xuong ngua
s = thay(s, ["\ts_nKNDangCam = nNut - 1;\t// 0 = nut chinh, 1..8 = o phu", "\ts_nKNNgonX = x;"], [
    "\ts_nKNDangCam = nNut - 1;\t// 0 = nut chinh, 1..8 = o phu",
    "\ts_nKNNgonGiu = 1;\t\t\t// %s" % DAU,
    "\ts_uKNDoiNguaDen = 0;",
    "\ts_nKNNgonX = x;",
], "BatDau giu")
s = thay(s, [
    "bool JxKyNang_Nha()",
    "{",
    "\tbool bCo = (s_nKNDangCam >= 0);",
    "",
    "\ts_nKNDangCam = -1;",
], [
    "bool JxKyNang_Nha()",
    "{",
    "\tbool bCo = (s_nKNDangCam >= 0);",
    "",
    "\ts_nKNNgonGiu = 0;",
    "\t// %s dang cho xuong/len ngua: giu lenh, JxKyNang_Nhip thu lai roi tu nha" % DAU,
    "\tif (bCo && s_uKNDoiNguaDen && (unsigned int)GetTickCount() < s_uKNDoiNguaDen)",
    "\t\treturn bCo;",
    "\ts_nKNDangCam = -1;",
], "Nha giu")
s = thay(s, [
    "\ts_uKNDanhLuc = uNay;",
    "\tKyNang_DanhMotPhat();",
    "}",
], [
    "\ts_uKNDanhLuc = uNay;",
    "\tKyNang_DanhMotPhat();",
    "\t// %s ngon da nha: het cho ngua (danh xong hoac qua 6 giay) thi tha lenh" % DAU,
    "\tif (s_uKNDoiNguaDen && uNay >= s_uKNDoiNguaDen)",
    "\t\ts_uKNDoiNguaDen = 0;",
    "\tif (!s_nKNNgonGiu && !s_uKNDoiNguaDen)",
    "\t{",
    "\t\ts_nKNDangCam = -1;",
    "\t\ts_nKNDichIdx = 0;",
    "\t\ts_nKNCoNgam = 0;",
    "\t\ts_uKNDanhLuc = 0;",
    "\t}",
    "}",
], "Nhip tha lenh")
ghi(P, s)
print("da va:", P)
