# -*- coding: latin-1 -*-
# [GONMAN 14/09] Chu 20:0x, ba viec don man hinh:
#   1. BO nut noi "Chinh giao dien" o goc phai tren man choi (chi ban dien thoai). Van mo trinh chinh duoc tu
#      Cai dat > TUY CHON > nut "Chinh giao dien" (UiOptions.cpp:248, 320) nen khong mat duong vao.
#   2. BO nut "O da giau / Cat o giau" trong trinh chinh: hang nut A con Doi o - Goc o - Hoan tac.
#      O nao DANG giau van giau; muon hien lai thi bam "Xoa het" (tra bo cuc ve mac dinh cua game).
#   3. Nut hai mui ten (doi che do gan ky nang, spr\Ui3\UiSkillControl\switch_assign_mode.spr) doi thanh
#      nut AN HET CAC NUT KY NANG: cham = an / hien lai ca cum (nut chinh + 8 o phu), rieng nut nay van hien.
#      Cham cung thoat che do gan (giu duong huy). Vao che do gan van co: bang chon ky nang tu dat
#      s_nKNCheDoGan = 1 (JxCanDieuKhien.cpp:1888/1910/1920), gan xong tu thoat (1575).
#      KHONG nho qua lan choi sau: mo lai game thi nut ky nang hien lai.
# Ban PC khong doi mot dong (rao JX_MOBILE); kiem --pc phai DAT.
# KHONG go byte tieng Viet bang tay: neo nao dinh chu Viet thi DOC TU TEP (bay da mac mot lan 20:1x).
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_gonman_1409.py [goc]
import io, os, sys

GOC = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith('--') else os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KQ = []
R = '\r\n'
T = '\t'

def doc(p):
    with io.open(p, 'r', encoding='latin-1', newline='') as f:
        return f.read()

def ghi(p, s):
    with io.open(p, 'w', encoding='latin-1', newline='') as f:
        f.write(s)

def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)

def va(p, cu, moi, dau_hieu, so_lan=1):
    s = doc(p)
    if dau_hieu in s:
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:58])); return
    n = s.count(cu)
    if n != so_lan:
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:110]))
    s2 = s.replace(cu, moi)
    if cao(s2) != cao(s):
        raise SystemExit('LOI: so byte cao doi o %s' % p)
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:58]))

def dong_chua(p, mau):
    """Tra ve nguyen mot dong (khong ke thut dau dong, khong ke CRLF) chua chuoi ASCII mau - de lam neo
    cho nhung dong co chu tieng Viet ma khong phai go byte cao bang tay."""
    s = doc(p)
    i = s.find(mau)
    if i < 0:
        raise SystemExit('LOI: khong thay %r trong %s' % (mau, p))
    j = s.find(R, i)
    return s[i:j]

TD = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'Elem', 'UiToaDoMobile.inc')
CD = os.path.join(GOC, 'Sources', 'S3Client', 'Platform', 'JxCanDieuKhien.cpp')

# ============================ 1. bo nut noi "Chinh giao dien" ============================
va(TD,
   T + T + 'if (UiToaDo_ChoPhep())' + R +
   T + T + '{' + R +
   T + T + T + 'int nX, nY, nW, nH;' + R,
   '#ifndef JX_MOBILE' + T + '// [GONMAN 14/09] chu: "bo chinh giao dien o man hinh di" - dien thoai khong ve nut noi nua;' + R +
   T + T + T + T + T + '// vao trinh chinh bang Cai dat > TUY CHON > "Chinh giao dien" (UiOptions.cpp)' + R +
   T + T + 'if (UiToaDo_ChoPhep())' + R +
   T + T + '{' + R +
   T + T + T + 'int nX, nY, nW, nH;' + R,
   '[GONMAN 14/09] chu: "bo chinh giao dien o man hinh di"')

_DONG = dong_chua(TD, 'SuaGd_VeChuCo(12, "Ch')          # dong ve chu "Chinh giao dien" (co chu Viet -> doc tu tep)
va(TD,
   T + T + T + _DONG + R + T + T + '}' + R,
   T + T + T + _DONG + R + T + T + '}' + R + '#endif' + R,
   '#endif' + R + T + T + 'if (s_szThongBao[0]')

va(TD,
   T + T + 'Wnd_GetCursorPos(&x, &y);' + R +
   T + T + 'if (TrongNutMo(x, y) == false)' + R,
   '#ifdef JX_MOBILE' + R +
   T + T + 'return false;' + T + '// [GONMAN 14/09] bo nut noi -> cham ngoai trinh chinh khong mo gi nua' + R +
   '#else' + R +
   T + T + 'Wnd_GetCursorPos(&x, &y);' + R +
   T + T + 'if (TrongNutMo(x, y) == false)' + R,
   '[GONMAN 14/09] bo nut noi -> cham ngoai trinh chinh')

va(TD,
   T + T + 'UiToaDo_BatTat();' + R +
   T + T + 'return true;' + R +
   T + '}' + R,
   T + T + 'UiToaDo_BatTat();' + R +
   T + T + 'return true;' + R +
   '#endif' + R +
   T + '}' + R,
   '#endif' + R + T + '}' + R)

# ============================ 2. bo nut "O da giau" trong trinh chinh ============================
va(TD,
   T + 'aNut[0] = SG_NUT_CHEDO;' + R +
   T + 'aNut[1] = SG_NUT_GIAU;' + R +
   T + 'aNut[2] = SG_NUT_MACDINH;' + R +
   T + 'aNut[3] = SG_NUT_HOANTAC;' + R +
   T + 'return 4;' + R,
   T + '// [GONMAN 14/09] chu: "bo luon o da giau o chinh giao dien" - bo nut Giau/hien (va che do "hien cac o da giau").' + R +
   T + '//   O nao DANG giau van giau; muon hien lai thi bam "Xoa het" (tra bo cuc ve mac dinh cua game).' + R +
   T + 'aNut[0] = SG_NUT_CHEDO;' + R +
   '#ifndef JX_MOBILE' + R +
   T + 'aNut[1] = SG_NUT_GIAU;' + R +
   T + 'aNut[2] = SG_NUT_MACDINH;' + R +
   T + 'aNut[3] = SG_NUT_HOANTAC;' + R +
   T + 'return 4;' + R +
   '#else' + R +
   T + 'aNut[1] = SG_NUT_MACDINH;' + R +
   T + 'aNut[2] = SG_NUT_HOANTAC;' + R +
   T + 'return 3;' + R +
   '#endif' + R,
   '[GONMAN 14/09] chu: "bo luon o da giau o chinh giao dien"')

# ============================ 3. nut hai mui ten = an het nut ky nang ============================
va(CD,
   'static int' + T * 3 + 's_nKNCheDoGan = 0;' + T + '// 1 = dang o che do gan',
   'static int' + T * 3 + 's_nKNCheDoGan = 0;' + T + '// 1 = dang o che do gan' + R +
   'static int' + T * 3 + 's_nKNAnHet = 0;' + T + '// [GONMAN 14/09] 1 = an het cum nut ky nang (chu: nut hai mui ten lam lai thanh nut an);' + R +
   T * 5 + '// rieng nut hai mui ten van ve de bam lai. Khong nho qua lan choi sau.',
   's_nKNAnHet = 0;')

va(CD,
   T + T + 's_nKNCheDoGan = !s_nKNCheDoGan;' + R +
   T + T + 's_nKNOChon = -1;' + R +
   T + T + 's_nKNDangCam = -1;' + R +
   T + T + 'g_DebugLog("[KYNANG] che do gan = %d", s_nKNCheDoGan);' + R +
   T + T + 'return;' + R,
   T + T + '// [GONMAN 14/09] chu: "nut nhu tren hinh lam lai thanh nut an het cac nut ky nang".' + R +
   T + T + '//   Cham = an / hien lai ca cum; dang o che do gan thi cham cung thoat che do gan (giu duong huy nhu cu).' + R +
   T + T + '//   Vao che do gan van co: bang chon ky nang tu dat s_nKNCheDoGan = 1, gan xong tu thoat.' + R +
   T + T + 's_nKNCheDoGan = 0;' + R +
   T + T + 's_nKNOChon = -1;' + R +
   T + T + 's_nKNDangCam = -1;' + R +
   T + T + 's_nKNAnHet = !s_nKNAnHet;' + R +
   T + T + 'g_DebugLog("[KYNANG] an het nut ky nang = %d", s_nKNAnHet);' + R +
   T + T + 'return;' + R,
   '[GONMAN 14/09] chu: "nut nhu tren hinh lam lai thanh nut an het cac nut ky nang"')

va(CD,
   T + '// Xet o phu TRUOC roi moi den nut chinh: cung o phu vong sat nut chinh, uu tien' + R +
   T + '// o nho de cham vao ria cung khong bi nut to nuot mat.' + R,
   T + 'if (s_nKNAnHet)' + R +
   T + T + 'return 0;' + T + '// [GONMAN 14/09] dang an cum: cham roi thang xuong ban do (chi nut hai mui ten o tren con bat)' + R +
   T + '// Xet o phu TRUOC roi moi den nut chinh: cung o phu vong sat nut chinh, uu tien' + R +
   T + '// o nho de cham vao ria cung khong bi nut to nuot mat.' + R,
   '[GONMAN 14/09] dang an cum: cham roi thang xuong ban do')

va(CD,
   T + '// Ve tu o phu ra ngoai roi den nut chinh, de nut chinh nam TREN CUNG.' + R +
   T + 'for (i = KYNANG_SO_PHU; i >= 0; i--)' + R,
   T + '// Ve tu o phu ra ngoai roi den nut chinh, de nut chinh nam TREN CUNG.' + R +
   T + '// [GONMAN 14/09] dang an het: bo qua ca cum, chi ve nut hai mui ten o duoi ham nay.' + R +
   T + 'for (i = s_nKNAnHet ? -1 : KYNANG_SO_PHU; i >= 0; i--)' + R,
   '[GONMAN 14/09] dang an het: bo qua ca cum')

for k in KQ:
    print(k)
