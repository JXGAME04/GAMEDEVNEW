# -*- coding: latin-1 -*-
# [GONMAN 14/09 c] Chu 21:3x: "ban chua tat thong bao khi bo ky nang vao o ky nang".
#
# 1. LOI DO BAN b GAY RA (nang hon ca loi bao): khi go dong bao "Da gan vao o phu N", ban b xoa CA KHOI
#    trong nhanh "con o trong" cua KyNang_ChamBangChon, ke ca ba dong LAM VIEC:
#         s_nKNCheDoGan = 1;  s_nKNOChon = i;  JxKyNang_GanKyNang(o.uGenre, o.uId);
#    -> bam "Phim phu" trong bang chon khi CHUA cham o nao thi khong gan duoc gi, im lang. Khoi phuc ba dong,
#    chi bo dong bao.
#
# 2. Tat NOT hai dong bao con lai tren duong BO KY NANG VAO O (chu nhac lan thu hai):
#      - "Het o trong: cham o phu muon thay"  (KyNang_ChamBangChon, nhanh het o)
#      - "Ky nang ho tro: khong gan vao o duoc"  (JxKyNang_GanKyNang, [KNHOTRO 14/09])
#    Viec van chay y het: het o thi van vao che do gan cho nguoi choi cham o muon thay; ky nang ho tro van
#    bi chan. Nhat ky g_DebugLog GIU NGUYEN de con soi duoc.
#
#    GIU LAI cac thong bao cua duong GO ky nang ("Da go khoi o phu", "O chinh chi thay duoc, khong go duoc",
#    "Ky nang nay chua nam o o nao", "O chinh ve ky nang co ban") - chu noi ve luc BO VAO o, khong phai luc go.
#
# Chi JX_MOBILE (ca tep chi bien dich cho mobile); kiem --pc phai DAT.
# Neo dinh chu tieng Viet thi DOC TU TEP (khong go byte bang tay).
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_gonman_1409_c.py [goc]
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

def dong_chua(p, mau):
    """Nguyen mot dong (tu vi tri mau den het dong, khong ke CRLF) - de lam neo cho dong co chu tieng Viet."""
    s = doc(p)
    i = s.find(mau)
    if i < 0:
        raise SystemExit('LOI: khong thay %r trong %s' % (mau, p))
    j = s.find(R, i)
    return s[i:j]

CD = os.path.join(GOC, 'Sources', 'S3Client', 'Platform', 'JxCanDieuKhien.cpp')

# ---- 1. khoi phuc ba dong lam viec o nhanh "con o trong" ----
va(CD,
   T * 3 + 'if (i < KYNANG_SO_PHU)' + R +
   T * 3 + '{' + R +
   T * 4 + '// [GONMAN 14/09 b] gan vao o phu: khong bao nua (xem chu thich tren)' + R +
   T * 3 + '}' + R,
   T * 3 + 'if (i < KYNANG_SO_PHU)' + R +
   T * 3 + '{' + R +
   T * 4 + '// [GONMAN 14/09 c] ban b go dong bao "Da gan vao o phu N" nhung go nham CA ba dong lam viec' + R +
   T * 4 + '//   ben duoi -> bam "Phim phu" luc chua cham o nao thi khong gan duoc gi. Giu lai ba dong,' + R +
   T * 4 + '//   chi khong bao nua (chu: "tat thong bao khi bo ky nang vao o ky nang").' + R +
   T * 4 + 's_nKNCheDoGan = 1;' + R +
   T * 4 + 's_nKNOChon = i;' + R +
   T * 4 + 'JxKyNang_GanKyNang(o.uGenre, o.uId);' + R +
   T * 3 + '}' + R,
   '[GONMAN 14/09 c] ban b go dong bao "Da gan vao o phu N" nhung go nham')

# ---- 2a. bo dong bao "Het o trong" ----
_D = dong_chua(CD, 'KyNang_Bao("H')          # dong "Het o trong: cham o phu muon thay" (co chu Viet)
va(CD,
   T * 4 + _D + R,
   T * 4 + '// [GONMAN 14/09 c] het o trong: khong bao nua, van vao che do gan de nguoi choi cham o muon thay.' + R,
   '[GONMAN 14/09 c] het o trong: khong bao nua')

# ---- 2b. bo dong bao "Ky nang ho tro: khong gan vao o duoc" ----
# neo "KyNang_Bao(\"K" trung hai dong (1598 va 1901) -> lay dong NGAY TRUOC dong g_DebugLog cua khoi chan,
# dong g_DebugLog do la ASCII va duy nhat.
_MOC = 'g_DebugLog("[KYNANG] ky nang %u khong dung duoc (khong co trong bang danh trai lan phai)'
_s = doc(CD)
_i = _s.find(_MOC)
if _i < 0:
    raise SystemExit('LOI: khong thay moc khoi chan ky nang ho tro')
_i = _s.rfind(R, 0, _i) + len(R)          # dau dong g_DebugLog
_j = _s.rfind(R, 0, _i - len(R)) + len(R)  # dau dong truoc do = dong KyNang_Bao
_D2 = _s[_j:_i - len(R)].lstrip(T)
if 'KyNang_Bao(' not in _D2:
    raise SystemExit('LOI: dong truoc g_DebugLog khong phai KyNang_Bao: %r' % _D2[:80])
va(CD,
   T * 2 + _D2 + R,
   T * 2 + '// [GONMAN 14/09 c] khong bao nua theo y chu; van chan, va nhat ky ben duoi con ghi ly do.' + R,
   '[GONMAN 14/09 c] khong bao nua theo y chu')

for k in KQ:
    print(k)
