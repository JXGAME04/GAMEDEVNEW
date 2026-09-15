# -*- coding: latin-1 -*-
# [GONMAN 14/09 b] Chu 20:2x, sau khi thu ban 109142015:
#   1. "bo ve o vuong mau vang o o an ky nang moi vua lam": che do gan dang to mot O VUONG VANG len chinh nut hai mui ten
#      (nut nay gio la nut AN het nut ky nang) - JxCanDieuKhien.cpp ~2055. Bo han o vuong do.
#   2. "viet lai o vuong mau vang nhu tren hinh cho dep": dau O DANG CHON trong che do gan cung dang la o vuong vang de len
#      nut TRON (anh chu chup) - thay bang VONG TRON sang effect_skill.spr (cung anh dung cho nut dang cam), vua khit nut
#      tron nen nhin gon; thieu anh thi moi lui ve o vuong nhu cu.
#   3. "tat may thong bao khi chon ky nang vao o ky nang": bo hai dong huong dan ve giua man trong che do gan
#      ("Che do gan: cham vao o ky nang muon doi" / "Giu mo bang ky nang, cham mot ky nang de gan vao o dang sang")
#      va hai thong bao khi gan XONG ("Da gan vao o chinh", "Da gan vao o phu N"). GIU lai cac thong bao bao LY DO khong lam
#      duoc (o chinh chi thay duoc, ky nang nay chua nam o o nao, het o trong...) vi khong co chung thi nguoi choi cham
#      ma khong hieu sao khong an.
# Chi JX_MOBILE (ca tep nay chi bien dich cho mobile); kiem --pc phai DAT.
# Neo dinh chu tieng Viet thi DOC TU TEP (khong go byte bang tay).
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_gonman_1409_b.py [goc]
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
    mong = cao(s) - cao(cu) * so_lan + cao(moi) * so_lan   # xoa chu Viet thi so byte cao GIAM dung bang phan xoa
    if cao(s2) != mong:
        raise SystemExit('LOI: so byte cao %d, mong %d, o %s' % (cao(s2), mong, p))
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:58]))

def khoi_tu(p, dau_mau, cuoi_mau):
    """Tra ve nguyen khoi tu dong chua dau_mau den het dong chua cuoi_mau (ke ca CRLF cuoi) - de lam neo
    cho doan co chu tieng Viet ma khong phai go byte cao."""
    s = doc(p)
    i = s.find(dau_mau)
    if i < 0:
        raise SystemExit('LOI: khong thay %r' % dau_mau)
    i = s.rfind(R, 0, i) + len(R)
    j = s.find(cuoi_mau, i)
    if j < 0:
        raise SystemExit('LOI: khong thay %r' % cuoi_mau)
    j = s.find(R, j) + len(R)
    return s[i:j]

CD = os.path.join(GOC, 'Sources', 'S3Client', 'Platform', 'JxCanDieuKhien.cpp')

# ---- 1 + 3a: bo o vuong vang tren nut hai mui ten va hai dong huong dan ----
KHOI = khoi_tu(CD, 'if (s_nKNCheDoGan)' + R + T + T + '{' + R + T + T + T + 'OVuong(nGX, nGY', 'nGX - 300, nGY - 38, 0xFFFFD24A);')
KHOI = KHOI + T + T + '}' + R      # lay ca dau dong khoi de xoa han, khong de lai ma chet
va(CD,
   KHOI,
   T + T + '// [GONMAN 14/09 b] chu: "bo ve o vuong mau vang o o an ky nang moi vua lam" + "tat may thong bao khi chon' + R +
   T + T + '//   ky nang vao o ky nang": nut nay gio la nut AN het nut ky nang, khong to o vuong vang len no nua,' + R +
   T + T + '//   cung khong ve hai dong huong dan giua man. O DANG CHON van sang (vong tron o vong ve phia tren).' + R +
   T + T + '//   (Khoi to o vuong + hai dong huong dan cu da xoa han o day.)' + R,
   '[GONMAN 14/09 b] chu: "bo ve o vuong mau vang o o an ky nang')

# ---- 2: o dang chon -> vong tron sang thay vi o vuong ----
va(CD,
   T + T + '// [ANDROID 09/09 GAN] o dang cho gan: to nen sang cho de nhan' + R +
   T + T + 'if (s_nKNCheDoGan && ((i > 0 && (i - 1) == s_nKNOChon) || (i == 0 && s_nKNOChon == KYNANG_CHON_CHINH)))' + R +
   T + T + T + 'OVuong(nX, nY, nR, 0x80FFD24A);' + R,
   T + T + '// [ANDROID 09/09 GAN] o dang cho gan: to nen sang cho de nhan' + R +
   T + T + '// [GONMAN 14/09 b] chu: "viet lai o vuong mau vang nhu tren hinh cho dep" - nut ky nang la hinh TRON ma dau' + R +
   T + T + '//   nay la o vuong vang de len, nhin chong. Dung vong tron sang effect_skill.spr (cung anh cua nut dang cam),' + R +
   T + T + '//   vua khit nut; thieu anh thi moi lui ve o vuong nhu cu.' + R +
   T + T + 'if (s_nKNCheDoGan && ((i > 0 && (i - 1) == s_nKNOChon) || (i == 0 && s_nKNOChon == KYNANG_CHON_CHINH)))' + R +
   T + T + '{' + R +
   T + T + T + 'if (CoAnh(s_szKNAnhNgam))' + R +
   T + T + T + T + 'VeAnhCo(s_szKNAnhNgam, nX, nY, nR * 2 + 12, 0);' + R +
   T + T + T + 'else' + R +
   T + T + T + T + 'OVuong(nX, nY, nR, 0x80FFD24A);' + R +
   T + T + '}' + R,
   '[GONMAN 14/09 b] chu: "viet lai o vuong mau vang nhu tren hinh cho dep"')

# ---- 3b: bo hai thong bao khi gan xong ----
_s = doc(CD)
_i = _s.find('s_KNChinhGan = o;')
if _i < 0:
    raise SystemExit('LOI: khong thay cho gan o chinh')
_j = _s.find('KyNang_Bao(', _i)
_k = _s.find(R, _j) + len(R)
DONG_CHINH = _s[_s.rfind(R, 0, _j) + len(R):_k]      # dong KyNang_Bao("Da gan vao o chinh");
va(CD, DONG_CHINH,
   T + T + '// [GONMAN 14/09 b] chu: "tat may thong bao khi chon ky nang vao o ky nang" - gan xong khong bao nua' + R +
   T + T + '// (o do sang len la du thay); cac thong bao bao LY DO khong gan duoc thi van giu.' + R,
   '[GONMAN 14/09 b] chu: "tat may thong bao khi chon ky nang vao o ky nang" - gan xong khong bao nua')

_s = doc(CD)
_i = _s.find('_snprintf(szBao, sizeof(szBao), "%s %d"')
if _i < 0:
    raise SystemExit('LOI: khong thay cho bao "da gan vao o phu"')
_i = _s.rfind('char szBao[96];', 0, _i)
_i = _s.rfind(R, 0, _i) + len(R)
_j = _s.find('KyNang_Bao(szBao);', _i)
_j = _s.find(R, _j) + len(R)
KHOI_PHU = _s[_i:_j]
va(CD, KHOI_PHU,
   T + T + T + T + '// [GONMAN 14/09 b] gan vao o phu: khong bao nua (xem chu thich tren)' + R,
   '[GONMAN 14/09 b] gan vao o phu: khong bao nua')

for k in KQ:
    print(k)
