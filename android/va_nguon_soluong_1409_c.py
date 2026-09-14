# -*- coding: latin-1 -*-
# [SOLUONG 14/09 SUA] Chu 16:2x: "item trong hanh trang - ruong do khong hien so luong nhu truoc".
#
# NGUYEN NHAN (da chung minh bang nhat ky tren may ao, khong doan):
#   [VEVATPHAM 12/09 e] them nhanh MOBILE trong KItem::PaintItem: "ve nguyen co, dat khung anh chinh giua o"
#   (dieu kien !g_nJxKeoAnhVatPham && oCo > 0) - nhanh nay ve xong roi RETURN, ma khoi ve SO LUONG nam o CUOI ham.
#   Nen tu 12/09 moi mon chong trong hanh trang / ruong / cua hang deu mat so. Chu thich cu ghi "(so luong mon chong
#   o phim tat khong ve, giong ban goc)" - nguoi viet tuong nhanh nay chi dung cho O PHIM TAT, that ra no la duong
#   MAC DINH cua moi o to hon anh mon (hanh trang 31x31 va 44x44, anh 26 px).
#   Bang chung (nhat ky [SOLUONG] 16:3x, hanh trang mo): "mon 6 o ve 581,127 | chong 1 so 500 | ophim 0 |
#   o that 581,127 31x31 | keo 0" -> mon CO so luong 500, khong phai o phim tat, di dung nhanh keo 0 (return som),
#   va tren man hinh khong co chu so nao.
#
# SUA: ve so luong ngay trong nhanh do, TRUOC return; dat theo O THAT (g_nJxVeVatPham*) chu khong theo luoi 27 px
#   cua ban PC, nen o to nho the nao so cung nam goc duoi phai o. Van bo qua o phim tat (!ispos_immediacy) giong ban goc.
#   Go nhat ky chan doan dat sai cho (cuoi ham, khong bao gio chay); giu nhat ky o dau ham (loc mon co so luong).
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_soluong_1409_c.py [goc]
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
    if moi == '' and cu not in s:
        KQ.append('bo qua (da go): %s' % dau_hieu[:60]); return      # xoa: idempotent theo NEO, khong theo dau hieu
    if moi != '' and dau_hieu in s:
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:60])); return
    n = s.count(cu)
    if n != so_lan:
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:110]))
    s2 = s.replace(cu, moi)
    if cao(s2) != cao(s):
        raise SystemExit('LOI: so byte cao doi o %s' % p)
    ghi(p, s2)
    KQ.append(('da go: %s' if moi == '' else 'da va: %s') % (os.path.basename(p) + ' : ' + dau_hieu[:60]))

KI = os.path.join(GOC, 'Sources', 'Core', 'Src', 'KItem.cpp')

# ---- 1. go nhat ky chan doan dat o CUOI ham (sau nhanh return som -> khong bao gio chay) ----
va(KI,
   '#ifdef JX_MOBILE' + R +
   T + 'if (g_nJxNhatKyVatPham3 > 0 && bPaintStack)' + R +
   T + '{' + T + '// [SOLUONG 14/09] chan doan: chu 16:2x "khong hien so luong o hanh trang / ruong"' + R +
   T + T + 'g_nJxNhatKyVatPham3--;' + R +
   T + T + 'g_DebugLog("[SOLUONG] mon %d o %d,%d | chong %d so %d | tat nhanh %d | chu se ve tai %d,%d | o goc %d,%d khung %dx%d | %s",' + R +
   T + T + T + '(int)m_CommonAttrib.nItemGenre, nX, nY, (int)(IsStack() ? 1 : 0), GetStackNum(), (int)(ispos_immediacy ? 1 : 0),' + R +
   T + T + T + 'nX + (m_CommonAttrib.nWidth * 27) - 1 * 12 / 2, nY + (m_CommonAttrib.nHeight) + 12 + 1,' + R +
   T + T + T + 'g_nJxVeVatPhamX, g_nJxVeVatPhamY, g_nJxVeVatPhamW, g_nJxVeVatPhamH, m_Image.szImage);' + R +
   T + '}' + R +
   '#endif' + R,
   '',
   'go nhat ky chan doan dat cuoi ham')

# ---- 2. ve so luong trong nhanh "ve nguyen co, can giua o" truoc khi return ----
NEO = (T * 3 + 'g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, TRUE);' + R +
       T * 3 + 'return;' + T + '// da ve xong (so luong mon chong o phim tat khong ve, giong ban goc)' + R)
va(KI,
   NEO,
   T * 3 + 'g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, TRUE);' + R +
   T * 3 + '// [SOLUONG 14/09] chu: "item trong hanh trang - ruong do khong hien so luong nhu truoc". Nhanh nay return som nen' + R +
   T * 3 + '// khoi ve so luong o CUOI ham khong bao gio chay -> tu [VEVATPHAM 12/09 e] moi mon chong trong hanh trang /' + R +
   T * 3 + '// ruong / cua hang mat so (chu thich cu tuong nhanh nay chi dung cho o phim tat). Ve o day, dat theo O THAT' + R +
   T * 3 + '// (g_nJxVeVatPham*) chu khong theo luoi 27 px cua ban PC: o to nho the nao so cung nam goc duoi phai o.' + R +
   T * 3 + 'if (IsStack() && bPaintStack && !ispos_immediacy)' + R +
   T * 3 + '{' + R +
   T * 4 + 'int nNum = GetStackNum();' + R +
   T * 4 + 'if (nNum >= 1 && nNum < 10000)' + R +
   T * 4 + '{' + R +
   T * 5 + 'char szNum[8];' + R +
   T * 5 + 'int nFontSize = 12;' + R +
   T * 5 + 'int nLen = sprintf(szNum, "%d", nNum);' + R +
   T * 5 + 'int nSoX = g_nJxVeVatPhamX + g_nJxVeVatPhamW - nLen * nFontSize / 2 - 1;' + R +
   T * 5 + 'int nSoY = g_nJxVeVatPhamY + g_nJxVeVatPhamH - nFontSize - 1;' + R +
   T * 5 + 'if (nSoX < g_nJxVeVatPhamX) nSoX = g_nJxVeVatPhamX;' + R +
   T * 5 + 'if (nSoY < g_nJxVeVatPhamY) nSoY = g_nJxVeVatPhamY;' + R +
   T * 5 + 'g_pRepresent->OutputText(nFontSize, szNum, KRF_ZERO_END, nSoX, nSoY, 0xFFFFFF00);' + R +
   T * 4 + '}' + R +
   T * 3 + '}' + R +
   T * 3 + 'return;' + T + '// da ve xong' + R,
   '[SOLUONG 14/09] chu: "item trong hanh trang - ruong do khong hien so luong nhu truoc"')

for k in KQ:
    print(k)
