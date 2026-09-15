# -*- coding: latin-1 -*-
# [WACHON 14/09] Chu 17:0x: "WAuto - kich vao ra chon danh sach skill va item, chon xong khong hien thi".
#
# NGUYEN NHAN (chung minh bang nhat ky may ao + doc ma, khong doan):
#   KUiWAutoTrang::DienChon (UiWAutoTrang.cpp:496) khai `int i, v = 0;` roi tung nhanh TU doc gia tri bang LayInt(p):
#   nhanh WA_V_ST_BOSS doc, nhanh WA_V_TK_RUONG doc, nhanh chung cuoi ham doc (co chu thich [WAUTO 12/09] "CAC NHANH TREN
#   moi tu doc gia tri"). RIENG nhanh WA_NGUON_CHIEU (moi hop chon KY NANG / VAT PHAM: Bat ho tro #1-3, Ky nang danh boss,
#   Sinh luc / Noi luc, Vong sang #1-2, Doi vu khi...) KHONG doc -> v luon = 0 -> luon in "Khong thiet lap", du gia tri da
#   ghi dung vao autoData.
#   Bang chung may ao 17:1x: chon "Toa Vong Vo Nga" o "Bat ho tro #1" -> nhat ky "[WAUTO-UI] IDC_COMBO_2_SP1 = 157" va
#   "[WAUTO] ghi \APdata\3758889385.dat: 7644 byte" (da luu), nhung o van hien "Khong thiet lap".
#   Doi chieu ban PC: WAuto.cpp nap lai hop chon tu autoData moi lan ve (xem muc doi chieu trong ban giao), khong co duong
#   nao hien "khong thiet lap" khi gia tri khac 0.
#
# SUA: doc gia tri o dau nhanh CHIEU, dung LayInt(p) nhu cac nhanh khac. Mot dong.
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_wauto_chon_1409.py [goc]
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
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:60])); return
    n = s.count(cu)
    if n != so_lan:
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:110]))
    s2 = s.replace(cu, moi)
    if cao(s2) != cao(s):
        raise SystemExit('LOI: so byte cao doi o %s' % p)
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:60]))

TR = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiWAutoTrang.cpp')

va(TR,
   T + 'else if (p->nNguon == WA_NGUON_CHIEU)' + R +
   T + '{' + R +
   T + T + 'if (m_nChieu < 0)' + R +
   T + T + T + 'NapChieu();' + R,
   T + 'else if (p->nNguon == WA_NGUON_CHIEU)' + R +
   T + '{' + R +
   T + T + 'v = LayInt(p);' + T + '// [WACHON 14/09] chu: "chon xong khong hien thi" - nhanh nay QUEN doc gia tri (v van = 0 tu' + R +
   T + T + T + T + T + '// dau ham) nen moi hop chon ky nang / vat pham luon in "Khong thiet lap" du autoData da ghi dung' + R +
   T + T + T + T + T + '// (nhat ky may ao: IDC_COMBO_2_SP1 = 157 ma o van trong). Cac nhanh khac deu tu doc, xem chu thich [WAUTO 12/09] duoi.' + R +
   T + T + 'if (m_nChieu < 0)' + R +
   T + T + T + 'NapChieu();' + R,
   '[WACHON 14/09] chu: "chon xong khong hien thi"')

for k in KQ:
    print(k)
