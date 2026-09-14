# -*- coding: latin-1 -*-
# [VATROI 14/09 c] Vat pham nam dat DUNG THANG LEN trong cot sang (chu 14/09 09:0x: "do nem ra da co mau theo loai do nhung no khong giong
# nhu 3d: do rot ra se dung thang len theo cot sang"). Core/Src/KObj.cpp (chi JX_MOBILE, sau va_nguon_vatroi_1409.py + va_nguon_haoquang_1409.py):
#   - icon vat pham (24x24, cx=cy=0 = goc trai tren tai diem dat) duoc nhac len: 12 px (de tam icon = diem chan cot) + NoiCao px dung len
#     trong 350 ms sau khi cham dat (ease-out) + nhap nhe +-3 px chu ky 1,4 s (VatRoi_DoCao). Bat/tat [VatRoi] Noi, do cao NoiCao.
#   - chan cot sang / tam vong loe doi sang TAM icon (anh sinh lai: anh_vatroi_cotsang.py neo (12,100) / (20,20)).
#   - Nhat do khong doi: KObjSet::SearchObjAt xet theo toa do ban do, khong theo o anh.
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_vatroi_1409_c.py [goc]
import io, os, sys

GOC = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith('--') else os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KQ = []
R = '\r\n'

def doc(p):
    with io.open(p, 'r', encoding='latin-1', newline='') as f:
        return f.read()

def ghi(p, s):
    with io.open(p, 'w', encoding='latin-1', newline='') as f:
        f.write(s)

def va(p, cu, moi, dau_hieu, so_lan=1):
    s = doc(p)
    if dau_hieu in s:
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:50])); return
    n = s.count(cu)
    if n != so_lan:
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:90]))
    cao_truoc = sum(1 for c in s if ord(c) >= 0x80)
    s2 = s.replace(cu, moi)
    cao_sau = sum(1 for c in s2 if ord(c) >= 0x80)
    if cao_truoc != cao_sau:
        raise SystemExit('LOI: so byte cao doi %d -> %d o %s' % (cao_truoc, cao_sau, p))
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:50]))

KO = os.path.join(GOC, 'Sources', 'Core', 'Src', 'KObj.cpp')

# 1) bang luc cham dat rieng cho hoat anh dung len + khoa cau hinh
va(KO,
   'static unsigned int s_uVrLoeLuc[MAX_OBJECT];' + R,
   'static unsigned int s_uVrLoeLuc[MAX_OBJECT];' + R +
   'static unsigned int s_uVrNoiLuc[MAX_OBJECT];\t// [VATROI 14/09 c] luc cham dat, cho hoat anh dung thang len (giu sau khi loe xong)' + R +
   'static int          s_nVrNoi = 1, s_nVrNoiCao = 8;\t// [VatRoi] Noi=1: vat pham dung thang len trong cot sang (chu: "do rot ra se dung thang len theo cot sang"), NoiCao px' + R,
   '[VATROI 14/09 c] luc cham dat')

# 2) doc khoa Noi / NoiCao + log
va(KO,
   '\tg_DebugLog("[VATROI] cot sang=%d (tu mau %d, alpha %d) loe=%d thu=%d", s_nVrCotSang, s_nVrCotSangTu, s_nVrAlpha, s_nVrLoe, s_nVrThu);' + R,
   '\ts_nVrNoi       = GetPrivateProfileInt("VatRoi", "Noi", 1, szCfg);\t// [VATROI 14/09 c] dung thang len' + R +
   '\ts_nVrNoiCao    = GetPrivateProfileInt("VatRoi", "NoiCao", 8, szCfg);' + R +
   '\tif (s_nVrNoiCao < 0) s_nVrNoiCao = 0;' + R +
   '\tif (s_nVrNoiCao > 40) s_nVrNoiCao = 40;' + R +
   '\tg_DebugLog("[VATROI] cot sang=%d (tu mau %d, alpha %d) loe=%d thu=%d noi=%d cao %d", s_nVrCotSang, s_nVrCotSangTu, s_nVrAlpha, s_nVrLoe, s_nVrThu, s_nVrNoi, s_nVrNoiCao);' + R,
   'GetPrivateProfileInt("VatRoi", "Noi"')

# 3) cham dat: ghi ca luc dung len
va(KO,
   '\t\ts_uVrLoeLuc[nIndex] = (unsigned int)GetTickCount();' + R,
   '\t\ts_uVrLoeLuc[nIndex] = s_uVrNoiLuc[nIndex] = (unsigned int)GetTickCount();\t// [VATROI 14/09 c]' + R,
   's_uVrLoeLuc[nIndex] = s_uVrNoiLuc[nIndex]')

# 4) ham do cao
va(KO,
   'static void VatRoi_DatAnh(KRUImage& a, int nFrame, int x, int y, unsigned int uAlpha, DWORD dwMau)' + R,
   '// [VATROI 14/09 c] do cao icon vat pham so voi diem dat (chu: "do rot ra se dung thang len theo cot sang" nhu game 3D): 12 px (tam icon' + R +
   '// 24x24 = diem chan cot) + NoiCao px dung len trong 350 ms sau khi cham dat (ease-out 2t - t^2) + nhap nhe +-3 px chu ky 1,4 s (song tam giac,' + R +
   '// khong can sin). 0 khi tat cong tac hoac tat Noi, hoac vat dang roi (m_nDropState 1: hoat anh roi tu quan).' + R +
   'static int VatRoi_DoCao(int nIndex, int nDropState)' + R +
   '{' + R +
   '\tunsigned int uNay, uDa;' + R +
   '\tint nLen, nNhap;' + R +
   '\tVatRoi_DocCfg();' + R +
   '\tif (!s_nVrBat || !s_nVrNoi || nDropState == 1)' + R +
   '\t\treturn 0;' + R +
   '\tuNay = (unsigned int)GetTickCount();' + R +
   '\tnLen = s_nVrNoiCao;' + R +
   '\tif (nIndex > 0 && nIndex < MAX_OBJECT && s_uVrNoiLuc[nIndex])' + R +
   '\t{' + R +
   '\t\tuDa = uNay - s_uVrNoiLuc[nIndex];' + R +
   '\t\tif (uDa < 350)' + R +
   '\t\t\tnLen = (int)((unsigned int)s_nVrNoiCao * (700 * uDa - uDa * uDa) / (350 * 350));' + R +
   '\t}' + R +
   '\tnNhap = (int)(uNay % 1400); if (nNhap >= 700) nNhap = 1400 - nNhap;\t// 0..700..0' + R +
   '\treturn 12 + nLen + nNhap * 6 / 700 - 3;' + R +
   '}' + R + R +
   'static void VatRoi_DatAnh(KRUImage& a, int nFrame, int x, int y, unsigned int uAlpha, DWORD dwMau)' + R,
   '[VATROI 14/09 c] do cao icon vat pham')

# 5) hook: nhac icon len (chi vat pham; cot sang / loe van ve tai diem dat)
va(KO,
   '\t\tm_Image.oPosition.nY = y;// - m_cImage.m_nCgYpos * 2;' + R +
   '\t\tstrcpy(m_Image.szImage, m_cImage.m_szName);' + R +
   '\t}' + R,
   '\t\tm_Image.oPosition.nY = y;// - m_cImage.m_nCgYpos * 2;' + R +
   '\t\tstrcpy(m_Image.szImage, m_cImage.m_szName);' + R +
   '\t}' + R +
   '#ifdef JX_MOBILE' + R +
   '\tif (m_nKind == Obj_Kind_Item)' + R +
   '\t\tm_Image.oPosition.nY -= VatRoi_DoCao(m_nIndex, m_nDropState);\t// [VATROI 14/09 c] dung thang len trong cot sang (nhat do theo toa do ban do, khong anh huong)' + R +
   '#endif' + R,
   '[VATROI 14/09 c] dung thang len trong cot sang')

print('\n'.join(KQ))
