# -*- coding: latin-1 -*-
# [SOLUONG 14/09 d] Soi cheo phien do nhip: nhanh KEO anh ([Ui] KeoAnhVatPham=1) KHONG return nen roi xuong khoi cuoi ham,
# van dat so theo luoi 27 px cua ban PC -> ai bat khoa do se thay so lech nhu cu. Nay khoi cuoi ham cung dat theo O THAT khi
# co (g_nJxVeVatPham* > 0), khong co thi giu y nguyen cong thuc cu (ban PC + o dung bang co anh mon).
# Idempotent, latin-1, neo CRLF. Chay SAU _c. Dung: python android/va_nguon_soluong_1409_d.py [goc]
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

KI = os.path.join(GOC, 'Sources', 'Core', 'Src', 'KItem.cpp')

NEO = (T * 3 + 'g_pRepresent->OutputText(nFontSize, szNum, KRF_ZERO_END,' + R +
       T * 4 + 'nX + (m_CommonAttrib.nWidth * 27) - nLen * (nFontSize ) / 2,' + R +
       T * 4 + 'nY + (m_CommonAttrib.nHeight) + nFontSize + 1, 0xFFFFFF00);' + R)
va(KI,
   NEO,
   T * 3 + 'int nSoX = nX + (m_CommonAttrib.nWidth * 27) - nLen * (nFontSize ) / 2;' + R +
   T * 3 + 'int nSoY = nY + (m_CommonAttrib.nHeight) + nFontSize + 1;' + R +
   '#ifdef JX_MOBILE' + R +
   T * 3 + '// [SOLUONG 14/09 d] (soi cheo phien do nhip) nhanh KEO anh ([Ui] KeoAnhVatPham=1) roi xuong day, neu van dat theo luoi' + R +
   T * 3 + '// 27 px cua ban PC thi so lech khoi o to cua mobile. Co o that thi dat goc duoi phai o, giong nhanh ve nguyen co.' + R +
   T * 3 + 'if (g_nJxVeVatPhamW > 0 && g_nJxVeVatPhamH > 0)' + R +
   T * 3 + '{' + R +
   T * 4 + 'nSoX = g_nJxVeVatPhamX + g_nJxVeVatPhamW - nLen * nFontSize / 2 - 1;' + R +
   T * 4 + 'nSoY = g_nJxVeVatPhamY + g_nJxVeVatPhamH - nFontSize - 1;' + R +
   T * 4 + 'if (nSoX < g_nJxVeVatPhamX) nSoX = g_nJxVeVatPhamX;' + R +
   T * 4 + 'if (nSoY < g_nJxVeVatPhamY) nSoY = g_nJxVeVatPhamY;' + R +
   T * 3 + '}' + R +
   '#endif' + R +
   T * 3 + 'g_pRepresent->OutputText(nFontSize, szNum, KRF_ZERO_END, nSoX, nSoY, 0xFFFFFF00);' + R,
   '[SOLUONG 14/09 d] (soi cheo phien do nhip) nhanh KEO anh')

for k in KQ:
    print(k)
