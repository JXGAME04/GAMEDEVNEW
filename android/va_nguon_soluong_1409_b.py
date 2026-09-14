# -*- coding: latin-1 -*-
# [SOLUONG 14/09 b] Chan doan lan 1 KHONG ket luan duoc: bo dem nhat ky bi O PHIM TAT (mon thuoc ve MOI KHUNG) tieu het trong ~1 giay,
# truoc khi kip mo hanh trang; va cho dat log lai nam SAU nhanh "ve nguyen co, can giua o" (nhanh nay return som).
# Nay: chuyen log len DAU KItem::PaintItem (sau khoi sidx, truoc moi nhanh ve) de bat MOI cu ve du di nhanh nao.
# Idempotent, latin-1, neo CRLF. Chay SAU va_nguon_soluong_1409.py. Dung: python android/va_nguon_soluong_1409_b.py [goc]
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

CU = ('#ifdef JX_MOBILE' + R +
      T + 'if (g_nJxNhatKyVatPham3 > 0 && bPaintStack)' + R +
      T + '{' + T + '// [SOLUONG 14/09] chan doan: chu 16:2x "khong hien so luong o hanh trang / ruong"' + R +
      T + T + 'g_nJxNhatKyVatPham3--;' + R +
      T + T + 'g_DebugLog("[SOLUONG] mon %d o %d,%d | chong %d so %d | tat nhanh %d | chu se ve tai %d,%d | o goc %d,%d khung %dx%d | %s",' + R +
      T + T + T + '(int)m_CommonAttrib.nItemGenre, nX, nY, (int)(IsStack() ? 1 : 0), GetStackNum(), (int)(ispos_immediacy ? 1 : 0),' + R +
      T + T + T + 'nX + (m_CommonAttrib.nWidth * 27) - 1 * 12 / 2, nY + (m_CommonAttrib.nHeight) + 12 + 1,' + R +
      T + T + T + 'g_nJxVeVatPhamX, g_nJxVeVatPhamY, g_nJxVeVatPhamW, g_nJxVeVatPhamH, m_Image.szImage);' + R +
      T + '}' + R +
      '#endif' + R)
va(KI, CU, '', '[SOLUONG] mon %d o %d,%d')   # go log cu (dat sau nhanh return som nen khong bao gio chay cho hanh trang)

NEO = T + 'm_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;'
va(KI,
   NEO,
   '#ifdef JX_MOBILE' + R +
   T + 'if (g_nJxNhatKyVatPham3 > 0 && bPaintStack)' + R +
   T + '{' + T + '// [SOLUONG 14/09 b] chan doan (chu 16:2x "khong hien so luong o hanh trang / ruong"): ghi TRUOC khi re nhanh ve,' + R +
   T + T + '// vi nhanh "ve nguyen co can giua o" return som nen log dat o cuoi ham khong bao gio chay.' + R +
   T + T + 'g_nJxNhatKyVatPham3--;' + R +
   T + T + 'g_DebugLog("[SOLUONG] mon %d o ve %d,%d | chong %d so %d | ophim %d | o that %d,%d %dx%d | keo %d | %s",' + R +
   T + T + T + '(int)m_CommonAttrib.nItemGenre, nX, nY, (int)(IsStack() ? 1 : 0), GetStackNum(), (int)(ispos_immediacy ? 1 : 0),' + R +
   T + T + T + 'g_nJxVeVatPhamX, g_nJxVeVatPhamY, g_nJxVeVatPhamW, g_nJxVeVatPhamH, g_nJxKeoAnhVatPham, m_Image.szImage);' + R +
   T + '}' + R +
   '#endif' + R +
   NEO,
   '[SOLUONG] mon %d o ve %d,%d')

# g_nJxKeoAnhVatPham khai bao ngoai ham -> can extern trong ham (cac nhanh duoi da extern rieng, o day khai bao som)
va(KI,
   T + 'if (g_nJxNhatKyVatPham3 > 0 && bPaintStack)' + R +
   T + '{' + T + '// [SOLUONG 14/09 b]',
   T + 'extern int g_nJxKeoAnhVatPham;' + R +
   T + 'if (g_nJxNhatKyVatPham3 > 0 && bPaintStack)' + R +
   T + '{' + T + '// [SOLUONG 14/09 b]',
   'extern int g_nJxKeoAnhVatPham;' + R + T + 'if (g_nJxNhatKyVatPham3 > 0')

for k in KQ:
    print(k)
