# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 KYNANG F] Ngam co MUI TEN tai vi tri chon + danh theo diem + kiem tra ngua.
#
# Theo dung ban tham khao (KuiMyMenu.cpp:847-912 va :970):
#   - Keo tu nut ra -> doi do lech keo thanh mot DIEM tren man hinh:
#         diem = giua man hinh + do_lech * (tam_danh / ban_kinh_keo)
#     va EP TRUC Y CON MOT NUA vi goc nhin nghieng (isometric) - ban tham khao ghi ro
#     "*_Beilv/2.0f" o moi nhanh huong.
#   - Dat anh mui ten o diem do (attackSpr_dir cua ban tham khao).
#   - Tha ngon -> customSkillAttack(ky nang, diem) tuc UseSkill(x, y, ky nang) - danh
#     THEO DIEM, khong phai khoa vao mot con.
#
# Nut chinh va nut phu khac chuc nang (KgameWorld.cpp:3015 va :4364):
#   - nut chinh: danh thuong bang ky nang danh TRAI, tu chon muc tieu;
#   - nut phu  : dat lam ky nang danh PHAI roi danh theo diem.
#
# Truoc khi danh, ca hai deu qua khau hoi Core GDI_KYNANG_MOBILE (ban va 41): ky nang
# nay dung duoc khi dang cuoi ngua khong - neu chi dung duoc duoi ngua thi TU XUONG NGUA
# va thoi lan nay (dung nhu he tu danh lam, KPlayer.cpp:12610).

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
DAU = "[ANDROID 09/09 KYNANG F]"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# ---------------------------------------------------------------- 1. ban kinh keo
CU = '\ts_nKNCoIcon  = GetPrivateProfileInt("Cham", "KyNangCoIcon", 32, szCfg);'
assert s.count(CU) == 1
MOI = NL.join([
    CU,
    '\ts_nKNBanKinhKeo = GetPrivateProfileInt("Cham", "KyNangBanKinhKeo", 60, szCfg);',
    "\tif (s_nKNBanKinhKeo < 20) s_nKNBanKinhKeo = 20;",
])
s = s.replace(CU, MOI)

CU = "static int\t\t\ts_nKNCoIcon = 32;\t// co THAT cua bieu tuong ky nang (DrawSkillIcon bo qua Width/Height)"
assert s.count(CU) == 1
MOI = NL.join([
    CU,
    "//\t[ANDROID 09/09 KYNANG F] keo het ban kinh nay = ngam xa bang dung TAM DANH cua ky nang",
    "//\t(ban tham khao: _Beilv = _BackAttackRadius / radius, KuiMyMenu.cpp:310, radius = 60).",
    "static int\t\t\ts_nKNBanKinhKeo = 60;",
])
s = s.replace(CU, MOI)

# ---------------------------------------------------------------- 2. hoi Core
CU = NL.join([
    "void JxKyNang_BatDau(int nNut, int x, int y)",
])
assert s.count(CU) == 1
MOI = NL.join([
    "//\t[ANDROID 09/09 KYNANG F] Hoi Core truoc khi danh: ky nang nay dung duoc luc dang",
    "//\tcuoi ngua khong, tam danh bao nhieu, co phai ky nang tro khong.",
    "//\tTra ve true = danh tiep duoc; false = lan nay thoi (vua gui lenh len/xuong ngua,",
    "//\thoac ky nang khong dung duoc luc nay).",
    "static bool KyNang_HoiCore(int nSkillId, int* pTamDanh, int* pLaAura)",
    "{",
    "\tKJxKyNangHoi oHoi;",
    "",
    "\tif (pTamDanh) *pTamDanh = 0;",
    "\tif (pLaAura)  *pLaAura  = 0;",
    "\tif (g_pCoreShell == NULL || nSkillId <= 0)",
    "\t\treturn false;",
    "\tmemset(&oHoi, 0, sizeof(oHoi));",
    "\toHoi.nSkillId = nSkillId;",
    "\tif (!g_pCoreShell->GetGameData(GDI_KYNANG_MOBILE, (KUPARAM)&oHoi, 0))",
    "\t\treturn true;\t\t// khong hoi duoc thi cu danh, dung nhu truoc",
    "\tif (pTamDanh) *pTamDanh = oHoi.nTamDanh;",
    "\tif (pLaAura)  *pLaAura  = oHoi.nLaAura;",
    "\tif (oHoi.nNgua != 0)",
    "\t{",
    "\t\tg_DebugLog(\"[KYNANG] ky nang %d: xu ngua = %d -> lan nay chua danh\",",
    "\t\t\tnSkillId, oHoi.nNgua);",
    "\t\treturn false;",
    "\t}",
    "\treturn true;",
    "}",
    "",
    CU,
])
s = s.replace(CU, MOI)

# ---------------------------------------------------------------- 3. keo = tinh diem ngam
CU = NL.join([
    "void JxKyNang_Keo(int x, int y)",
    "{",
    "\tint nX, nY, dx, dy;",
    "",
    "\tif (s_nKNDangCam < 0)",
    "\t\treturn;",
    "\ts_nKNNgonX = x;",
    "\ts_nKNNgonY = y;",
    "\tKyNang_TamNut(s_nKNDangCam, &nX, &nY);",
    "\tdx = x - nX;",
    "\tdy = y - nY;",
    "\t// Keo chua du xa = chua ngam (tranh rung tay lam doi muc tieu). Ban tham khao",
    "\t// cung co vung chet nay (KuiMyMenu m_ptJXTouch0 / m_bJXAimKeo).",
    "\tif (dx * dx + dy * dy < KYNANG_NGUONG_NGAM * KYNANG_NGUONG_NGAM)",
    "\t{",
    "\t\ts_nKNDichIdx = 0;",
    "\t\treturn;",
    "\t}",
    "\ts_nKNDichIdx = KyNang_TimDich(dx, dy, &s_nKNDichX, &s_nKNDichY);",
    "}",
])
assert s.count(CU) == 1, "khong tim thay JxKyNang_Keo (%d)" % s.count(CU)
MOI = NL.join([
    "void JxKyNang_Keo(int x, int y)",
    "{",
    "\tint nX, nY, dx, dy, nTam = 0;",
    "\tKUiGameObject o;",
    "",
    "\tif (s_nKNDangCam < 0)",
    "\t\treturn;",
    "\ts_nKNNgonX = x;",
    "\ts_nKNNgonY = y;",
    "\tKyNang_TamNut(s_nKNDangCam, &nX, &nY);",
    "\tdx = x - nX;",
    "\tdy = y - nY;",
    "\t// Keo chua du xa = chua ngam (tranh rung tay lam doi muc tieu). Ban tham khao",
    "\t// cung co vung chet nay (KuiMyMenu m_ptJXTouch0 / m_bJXAimKeo).",
    "\tif (dx * dx + dy * dy < KYNANG_NGUONG_NGAM * KYNANG_NGUONG_NGAM)",
    "\t{",
    "\t\ts_nKNDichIdx = 0;",
    "\t\ts_nKNCoNgam = 0;",
    "\t\treturn;",
    "\t}",
    "\ts_nKNDichIdx = KyNang_TimDich(dx, dy, &s_nKNDichX, &s_nKNDichY);",
    "",
    "\t// [ANDROID 09/09 KYNANG F] DIEM NGAM tren man hinh, dung phep cua ban tham khao:",
    "\t//   diem = giua man hinh + do_lech_keo * (tam_danh / ban_kinh_keo)",
    "\t// truc Y ep con MOT NUA vi goc nhin nghieng (isometric).",
    "\t// Nhan vat luon o giua khung ve nen giua man hinh = cho nhan vat dung.",
    "\tif (KyNang_CuaNut(s_nKNDangCam, &o))",
    "\t{",
    "\t\tKJxKyNangHoi oH;",
    "",
    "\t\tmemset(&oH, 0, sizeof(oH));",
    "\t\toH.nSkillId = (int)o.uId;",
    "\t\tif (g_pCoreShell && g_pCoreShell->GetGameData(GDI_KYNANG_MOBILE, (KUPARAM)&oH, 0))",
    "\t\t\tnTam = oH.nTamDanh;",
    "\t}",
    "\tif (nTam < 40)",
    "\t\tnTam = 40;\t\t// ky nang khong co tam (danh gan) van phai ngam duoc mot doan",
    "\t{",
    "\t\tint nDai = (int)sqrt((double)(dx * dx + dy * dy));",
    "\t\tint nXa  = nDai * nTam / s_nKNBanKinhKeo;",
    "",
    "\t\tif (nXa > nTam)",
    "\t\t\tnXa = nTam;\t\t// keo qua xa cung chi toi duoc tam danh",
    "\t\tif (nDai < 1)",
    "\t\t\tnDai = 1;",
    "\t\ts_nKNNgamX = SCREEN_WIDTH  / 2 + dx * nXa / nDai;",
    "\t\ts_nKNNgamY = SCREEN_HEIGHT / 2 + dy * nXa / nDai / 2;\t// ep Y con mot nua",
    "\t\ts_nKNCoNgam = 1;",
    "\t}",
    "}",
])
s = s.replace(CU, MOI)

# ---------------------------------------------------------------- 4. nha ngon
CU = NL.join([
    "bool JxKyNang_Nha()",
    "{",
    "\tint nNut = s_nKNDangCam;",
    "\tint nDich = s_nKNDichIdx;",
    "\tKUiGameObject o;",
    "",
    "\ts_nKNDangCam = -1;",
    "\ts_nKNDichIdx = 0;",
    "\tif (nNut < 0 || g_pCoreShell == NULL)",
    "\t\treturn false;",
    "\tif (!KyNang_CuaNut(nNut, &o))",
    "\t\treturn false;",
    "",
    "\t// O phu: dat luon lam ky nang danh TRAI (GOI_SET_IMMDIA_SKILL, y het",
    "\t// UiSkillTree.cpp:156) de o tren thanh trang thai doi theo va lan sau cham",
    "\t// thang vao dich la danh bang dung ky nang nay. Nut chinh thi khong can -",
    "\t// no VON dung ky nang danh trai.",
    "\tif (nNut > 0)",
    "\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 0);",
    "",
    "\t// Khong ngam duoc con nao trong huong keo thi danh con gan nhat (= kieu 0",
    "\t// cua ban tham khao: cham la danh, tu chon muc tieu).",
    "\tif (nDich == 0)",
    "\t\tnDich = KyNang_TimDich(0, 0, NULL, NULL);",
    "\tif (nDich == 0)",
    "\t\treturn true;\t\t// khong co dich: van coi la da xu ly",
    "\tg_pCoreShell->LockSomeoneUseSkill(nDich, (int)o.uId);",
    "\treturn true;",
    "}",
])
assert s.count(CU) == 1, "khong tim thay JxKyNang_Nha (%d)" % s.count(CU)
MOI = NL.join([
    "bool JxKyNang_Nha()",
    "{",
    "\tint nNut = s_nKNDangCam;",
    "\tint nDich = s_nKNDichIdx;",
    "\tint bCoNgam = s_nKNCoNgam;",
    "\tint nNgamX = s_nKNNgamX, nNgamY = s_nKNNgamY;",
    "\tint nTam = 0, nAura = 0;",
    "\tKUiGameObject o;",
    "",
    "\ts_nKNDangCam = -1;",
    "\ts_nKNDichIdx = 0;",
    "\ts_nKNCoNgam = 0;",
    "\tif (nNut < 0 || g_pCoreShell == NULL)",
    "\t\treturn false;",
    "\tif (!KyNang_CuaNut(nNut, &o))",
    "\t\treturn false;",
    "",
    "\t// [ANDROID 09/09 KYNANG F] Khau kiem tra ngua - dung nhu he tu danh lam.",
    "\t// Neu ky nang chi dung duoc duoi ngua ma dang cuoi thi Core da gui lenh xuong ngua,",
    "\t// lan nay khong danh; nguoi choi bam lai la danh.",
    "\tif (!KyNang_HoiCore((int)o.uId, &nTam, &nAura))",
    "\t\treturn true;",
    "\tif (nAura)",
    "\t{",
    "\t\t// Ky nang TRO (noi cong / trang thai): khong phai danh. Chi dat lam ky nang",
    "\t\t// dang dung; bat/tat aura chua noi duoc tu day - xem muc \"con lai\" ban giao.",
    "\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 0);",
    "\t\treturn true;",
    "\t}",
    "",
    "\tif (nNut > 0)",
    "\t{",
    "\t\t// [ANDROID 09/09 KYNANG F] NUT PHU: ban tham khao dat lam ky nang danh PHAI roi",
    "\t\t// danh THEO DIEM (KgameWorld.cpp:4437 SetRightSkill + UseSkill). Lam y vay - nho",
    "\t\t// the nut phu va nut chinh khong dam chan nhau: nut chinh van giu ky nang danh trai.",
    "\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 1);",
    "\t\tif (bCoNgam)",
    "\t\t{",
    "\t\t\tg_pCoreShell->UseSkill(nNgamX, nNgamY, (int)o.uId);\t// danh theo huong da ngam",
    "\t\t\treturn true;",
    "\t\t}",
    "\t}",
    "\telse if (bCoNgam)",
    "\t{",
    "\t\t// NUT CHINH co ngam: danh thuong theo huong do.",
    "\t\tg_pCoreShell->UseSkill(nNgamX, nNgamY, (int)o.uId);",
    "\t\treturn true;",
    "\t}",
    "",
    "\t// Khong ngam: cham la danh - tu chon con gan nhat (kieu 0 cua ban tham khao).",
    "\tif (nDich == 0)",
    "\t\tnDich = KyNang_TimDich(0, 0, NULL, NULL);",
    "\tif (nDich == 0)",
    "\t\treturn true;\t\t// khong co dich: van coi la da xu ly",
    "\tg_pCoreShell->LockSomeoneUseSkill(nDich, (int)o.uId);",
    "\treturn true;",
    "}",
])
s = s.replace(CU, MOI)

# ---------------------------------------------------------------- 5. ve mui ten
CU = NL.join([
    "\t\tif (s_nKNDichIdx)",
    "\t\t\tKyNang_VeVongDich(s_nKNDichX, s_nKNDichY);",
    "\t}",
    "}",
])
assert s.count(CU) == 1, "khong tim thay cuoi JxKyNang_Ve (%d)" % s.count(CU)
MOI = NL.join([
    "\t\tif (s_nKNDichIdx)",
    "\t\t\tKyNang_VeVongDich(s_nKNDichX, s_nKNDichY);",
    "",
    "\t\t// [ANDROID 09/09 KYNANG F] MUI TEN tai vi tri dang ngam - chu: \"huong do se co",
    "\t\t// mui ten theo vi tri chon\". Ban tham khao dat anh attackSpr_dir o dung diem nay.",
    "\t\tif (s_nKNCoNgam && CoAnh(s_szKNAnhTen))",
    "\t\t\tVeAnh(s_szKNAnhTen, s_nKNNgamX, s_nKNNgamY);",
    "\t}",
    "}",
])
s = s.replace(CU, MOI)

# ---------------------------------------------------------------- 6. include math
CU = '#include "KDebug.h"'
assert s.count(CU) >= 1
if "#include <math.h>" not in s:
    s = s.replace(CU, CU + NL + "#include <math.h>", 1)

io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va:", P)
