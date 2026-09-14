# -*- coding: latin-1 -*-
# [SOLUONG 14/09] Chu 16:2x: "item trong hanh trang - ruong do khong hien so luong nhu truoc (item co so luong thuong hien so ngay o item)".
# BUOC 1 - CHAN DOAN (chua sua gi ve hinh): ghi nhat ky moi lan ve mot mon co bPaintStack, cho biet IsStack / so luong / toa do chu se ve /
# co o that, de biet chu KHONG duoc ve hay ve SAI CHO. Gated bang bo dem san co g_nJxNhatKyVatPham3 (config [Ui] NhatKyVatPham),
# chi JX_MOBILE, khong doi mot dong nao cua ban PC.
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_soluong_1409.py [goc]
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

# Neo: khoi ve so luong trong KItem::PaintItem (giu nguyen, chi CHEN log truoc no)
NEO = (T + 'if (IsStack() && bPaintStack && !ispos_immediacy)')
va(KI,
   NEO,
   '#ifdef JX_MOBILE' + R +
   T + 'if (g_nJxNhatKyVatPham3 > 0 && bPaintStack)' + R +
   T + '{' + T + '// [SOLUONG 14/09] chan doan: chu 16:2x "khong hien so luong o hanh trang / ruong"' + R +
   T + T + 'g_nJxNhatKyVatPham3--;' + R +
   T + T + 'g_DebugLog("[SOLUONG] mon %d o %d,%d | chong %d so %d | tat nhanh %d | chu se ve tai %d,%d | o goc %d,%d khung %dx%d | %s",' + R +
   T + T + T + '(int)m_CommonAttrib.nItemGenre, nX, nY, (int)(IsStack() ? 1 : 0), GetStackNum(), (int)(ispos_immediacy ? 1 : 0),' + R +
   T + T + T + 'nX + (m_CommonAttrib.nWidth * 27) - 1 * 12 / 2, nY + (m_CommonAttrib.nHeight) + 12 + 1,' + R +
   T + T + T + 'g_nJxVeVatPhamX, g_nJxVeVatPhamY, g_nJxVeVatPhamW, g_nJxVeVatPhamH, m_Image.szImage);' + R +
   T + '}' + R +
   '#endif' + R +
   NEO,
   '[SOLUONG] mon %d o %d,%d')

for k in KQ:
    print(k)
