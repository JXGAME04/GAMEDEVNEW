# -*- coding: latin-1 -*-
# [VATROI 14/09 e] Icon dung CO GIAN vua cot sang + dat dung diem dat + ten vat pham nam tren icon (chu 14/09 09:5x, anh Fold 7: "ten trang bi
# bi lech va hinh anh qua to, lam nho lai cho bang cot sang").
#   - Anh nam dat cua vat pham (objdata \spr\obj\item\obj_wq_*.spr) neo DAY DUOI (ImageCgXpos/Ypos ~ 60/105) -> diem dat = (x, y), KHONG phai
#     (x+12, y+12) nhu ban c/d gia dinh (icon 24x24 cx=cy=0). Nay cot sang / loe / icon deu dat tai (x, y); ten (DrawInfo can giua x) khop.
#   - Icon ve bang RU_T_IMAGE_STRETCH o toa do man hinh (g_pRepresent->CoordinateTransform tu toa do the gioi) co gian ti le min(1,
#     IconRong/w, IconCao/h) ([VatRoi] IconRong=28 IconCao=44): 1x1 giu nguyen, 2x4 (48x96) -> 22x44, 1x4 -> 11x44. Ve o nhanh default cua
#     switch (SAU cot sang, TRUOC loe), m_Image (anh nam dat) khong ve nua khi co icon.
#   - Ten vat pham: DrawInfo cong them chieu cao icon + do lo lung vao nHeightOff.
#   - VatRoi_DoCao tra do LO LUNG (0..NoiCao+3), -1 = tat; khong con cong 12.
# Chay SAU va_nguon_vatroi_1409_d.py. Core/Src/KObj.cpp chi JX_MOBILE. Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_vatroi_1409_e.py [goc]
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

# 1) khoa cau hinh IconRong / IconCao + anh ve icon
va(KO,
   'static int          s_nVrNoi = 1, s_nVrNoiCao = 8;',
   'static int          s_nVrIcRong = 28, s_nVrIcCao = 44;\t// [VATROI 14/09 e] [VatRoi] IconRong/IconCao: khung toi da cua icon dung (co gian giu ti le)' + R +
   'static KRUImage     s_VrIcAnh;\t\t\t\t\t\t\t// [VATROI 14/09 e] don vi ve icon dung (RU_T_IMAGE_STRETCH, toa do man hinh)' + R +
   'static int          s_nVrNoi = 1, s_nVrNoiCao = 8;',
   '[VATROI 14/09 e] [VatRoi] IconRong/IconCao')

va(KO,
   '\tif (s_nVrNoiCao > 40) s_nVrNoiCao = 40;' + R,
   '\tif (s_nVrNoiCao > 40) s_nVrNoiCao = 40;' + R +
   '\ts_nVrIcRong    = GetPrivateProfileInt("VatRoi", "IconRong", 28, szCfg);\t// [VATROI 14/09 e]' + R +
   '\ts_nVrIcCao     = GetPrivateProfileInt("VatRoi", "IconCao", 44, szCfg);' + R +
   '\tif (s_nVrIcRong < 8) s_nVrIcRong = 8;' + R +
   '\tif (s_nVrIcCao < 8) s_nVrIcCao = 8;' + R,
   'GetPrivateProfileInt("VatRoi", "IconRong"')

# 2) VatRoi_DoCao: tra do lo lung, -1 = tat (khong cong 12 nua)
va(KO,
   '\tif (!s_nVrBat || !s_nVrNoi || nDropState == 1)' + R + '\t\treturn 0;' + R + '\tuNay = (unsigned int)GetTickCount();' + R + '\tnLen = s_nVrNoiCao;' + R,
   '\tif (!s_nVrBat || !s_nVrNoi || nDropState == 1)' + R + '\t\treturn -1;\t// [VATROI 14/09 e] -1 = tat' + R + '\tuNay = (unsigned int)GetTickCount();' + R + '\tnLen = s_nVrNoiCao;' + R,
   '[VATROI 14/09 e] -1 = tat')

va(KO,
   '\treturn 12 + nLen + nNhap * 6 / 700 - 3;' + R,
   '\tnNhap = nLen + nNhap * 6 / 700 - 3;\t// [VATROI 14/09 e] chi do LO LUNG tren diem dat (0..NoiCao+3); diem dat = (x, y) neo day anh nam dat' + R +
   '\treturn nNhap > 0 ? nNhap : 0;' + R,
   '[VATROI 14/09 e] chi do LO LUNG')

# 3) VatRoi_DungIcon: khong doi m_Image nua; chi danh dau co icon; khong co icon thi nhac anh nam dat len do lo lung
va(KO,
   '\tconst char* szIcon = (nCao > 0) ? VatRoi_TimIcon(p) : NULL;' + R,
   '\tconst char* szIcon = (nCao >= 0) ? VatRoi_TimIcon(p) : NULL;\t// [VATROI 14/09 e]' + R,
   'szIcon = (nCao >= 0) ? VatRoi_TimIcon(p)')

va(KO,
   '\t\tp->m_Image.oPosition.nY -= nCao;' + R + '\t\treturn;' + R,
   '\t\tif (nCao > 0) p->m_Image.oPosition.nY -= nCao;\t// [VATROI 14/09 e] khong co icon: anh nam dat lo lung' + R + '\t\treturn;' + R,
   '[VATROI 14/09 e] khong co icon: anh nam dat lo lung')

va(KO,
   '\tif (!s_nVrIcOn[i]) { s_nVrIcOn[i] = 1; p->m_Image.uImage = 0; }' + R +
   '\tstrcpy(p->m_Image.szImage, szIcon);' + R +
   '\tp->m_Image.nFrame = 0;' + R +
   '\tp->m_Image.oPosition.nX = x + 12 - s_nVrIcW[i] / 2;' + R +
   '\tp->m_Image.oPosition.nY = y + 12 - s_nVrIcH[i] - (nCao - 12);' + R,
   '\ts_nVrIcOn[i] = 1;\t// [VATROI 14/09 e] icon ve o nhanh default cua switch (VatRoi_VeIconDung) SAU cot sang, co gian vua cot; m_Image (anh nam dat) khong ve' + R,
   '[VATROI 14/09 e] icon ve o nhanh default')

# 4) ham ve icon co gian + chieu cao cho ten (dat truoc VatRoi_DatAnh)
va(KO,
   'static void VatRoi_DatAnh(KRUImage& a, int nFrame, int x, int y, unsigned int uAlpha, DWORD dwMau)' + R,
   '// [VATROI 14/09 e] ti le co gian icon: min(1, IconRong/w, IconCao/h) (phan nghin)' + R +
   'static int VatRoi_TiLeIcon(int w, int h)' + R +
   '{' + R +
   '\tint k = 1000, k2;' + R +
   '\tif (w > s_nVrIcRong) k = s_nVrIcRong * 1000 / w;' + R +
   '\tif (h > s_nVrIcCao) { k2 = s_nVrIcCao * 1000 / h; if (k2 < k) k = k2; }' + R +
   '\treturn k < 50 ? 50 : k;' + R +
   '}' + R + R +
   '// [VATROI 14/09 e] chieu cao cong them cho TEN vat pham (DrawInfo): icon dung + lo lung, 0 khi khong co icon dung' + R +
   'static int VatRoi_CaoTen(int nIndex)' + R +
   '{' + R +
   '\tint nCao;' + R +
   '\tif (nIndex <= 0 || nIndex >= MAX_OBJECT || !s_nVrIcOn[nIndex] || s_nVrIcH[nIndex] <= 0)' + R +
   '\t\treturn 0;' + R +
   '\tnCao = VatRoi_DoCao(nIndex, 0);' + R +
   '\tif (nCao < 0) nCao = 0;' + R +
   '\treturn nCao + s_nVrIcH[nIndex] * VatRoi_TiLeIcon(s_nVrIcW[nIndex], s_nVrIcH[nIndex]) / 1000;' + R +
   '}' + R + R +
   '// [VATROI 14/09 e] ve icon dung CO GIAN tai diem dat (x, y): doi sang toa do man hinh bang g_pRepresent->CoordinateTransform (cung ham' + R +
   '// Represent3 dung cho moi anh the gioi, gom ca lia/zoom/lac) roi RU_T_IMAGE_STRETCH (chi nhan toa do man hinh). Tra 1 = da ve (bo ve m_Image).' + R +
   'static int VatRoi_VeIconDung(KObj* p, int x, int y)' + R +
   '{' + R +
   '\tint i = p->m_nIndex, nCao, k, w2, h2, sx, sy;' + R +
   '\tKRUImage& a = s_VrIcAnh;' + R +
   '\tif (i <= 0 || i >= MAX_OBJECT || !s_nVrIcOn[i] || !s_szVrIcon[i][0] || s_nVrIcW[i] <= 0)' + R +
   '\t\treturn 0;' + R +
   '\tnCao = VatRoi_DoCao(i, p->m_nDropState);' + R +
   '\tif (nCao < 0)' + R +
   '\t\treturn 0;' + R +
   '\tk = VatRoi_TiLeIcon(s_nVrIcW[i], s_nVrIcH[i]);' + R +
   '\tw2 = s_nVrIcW[i] * k / 1000; if (w2 < 1) w2 = 1;' + R +
   '\th2 = s_nVrIcH[i] * k / 1000; if (h2 < 1) h2 = 1;' + R +
   '\tsx = x; sy = y;' + R +
   '\tg_pRepresent->CoordinateTransform(sx, sy, 0);' + R +
   '\tif (strcmp(a.szImage, s_szVrIcon[i]) != 0)' + R +
   '\t{' + R +
   '\t\tmemset(&a, 0, sizeof(a));' + R +
   '\t\tstrcpy(a.szImage, s_szVrIcon[i]);' + R +
   '\t}' + R +
   '\ta.uImage = 0; a.nISPosition = IMAGE_IS_POSITION_INIT; a.nType = ISI_T_SPR; a.nFrame = 0;' + R +
   '\ta.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA; a.bRenderFlag = 0; a.Color.Color_dw = 0xffffffff;' + R +
   '\ta.oPosition.nX = sx - w2 / 2; a.oPosition.nY = sy - h2 - nCao; a.oPosition.nZ = 0;' + R +
   '\ta.oEndPos.nX = a.oPosition.nX + w2; a.oEndPos.nY = a.oPosition.nY + h2; a.oEndPos.nZ = 0;' + R +
   '\tg_pRepresent->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, 1);' + R +
   '\treturn 1;' + R +
   '}' + R + R +
   'static void VatRoi_DatAnh(KRUImage& a, int nFrame, int x, int y, unsigned int uAlpha, DWORD dwMau)' + R,
   '[VATROI 14/09 e] ve icon dung CO GIAN tai diem dat')

# 5) nhanh default cua switch: co icon dung thi tu ve, bo ve m_Image
va(KO,
   '\tdefault:' + R + '\t\tg_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);\t' + R,
   '\tdefault:' + R +
   '#ifdef JX_MOBILE' + R +
   '\t\tif (m_nKind == Obj_Kind_Item && VatRoi_VeIconDung(this, x, y))\t// [VATROI 14/09 e] icon dung co gian, ve sau cot sang' + R +
   '\t\t\tbreak;' + R +
   '#endif' + R +
   '\t\tg_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);\t' + R,
   '[VATROI 14/09 e] icon dung co gian, ve sau cot sang')

# 6) ten vat pham nam tren icon dung (DrawInfo o truoc khoi static -> khai bao truoc)
va(KO,
   '//Edit by Phong Kieu code van luong money duoi dat' + R + 'void KObj::DrawInfo()' + R,
   '#ifdef JX_MOBILE' + R +
   'static int VatRoi_CaoTen(int nIndex);\t// [VATROI 14/09 e] dinh nghia o khoi VATROI truoc KObj::Draw' + R +
   '#endif' + R +
   '//Edit by Phong Kieu code van luong money duoi dat' + R + 'void KObj::DrawInfo()' + R,
   '[VATROI 14/09 e] dinh nghia o khoi VATROI truoc KObj::Draw')

va(KO,
   '    nHeightOff = OBJ_SHOW_NAME_Y_OFF;' + R + '    dwColor = this->m_dwNameColor;' + R + '    if (m_nKind == Obj_Kind_Money)' + R,
   '    nHeightOff = OBJ_SHOW_NAME_Y_OFF;' + R +
   '#ifdef JX_MOBILE' + R +
   '    nHeightOff += VatRoi_CaoTen(m_nIndex);\t// [VATROI 14/09 e] ten nam tren icon dung + lo lung' + R +
   '#endif' + R +
   '    dwColor = this->m_dwNameColor;' + R + '    if (m_nKind == Obj_Kind_Money)' + R,
   '[VATROI 14/09 e] ten nam tren icon dung')


# 7) ten sat icon hon: bot 12 px (OBJ_SHOW_NAME_Y_OFF 48 la cho anh nam dat ~24 px)
va(KO,
   '	return nCao + s_nVrIcH[nIndex] * VatRoi_TiLeIcon(s_nVrIcW[nIndex], s_nVrIcH[nIndex]) / 1000;' + R,
   '	nCao += s_nVrIcH[nIndex] * VatRoi_TiLeIcon(s_nVrIcW[nIndex], s_nVrIcH[nIndex]) / 1000 - 12;	// [VATROI 14/09 e2] bot 12 px cho ten sat icon' + R +
   '	return nCao > 0 ? nCao : 0;' + R,
   '[VATROI 14/09 e2] bot 12 px cho ten sat icon')

print('\n'.join(KQ))
