# -*- coding: latin-1 -*-
# [VATROI 14/09 d] Vat pham nam dat ve bang ICON DUNG (anh hanh trang) lo lung trong cot sang (chu 14/09 09:2x: "do nem ra da nam troi noi
# nhung chua dung thang len theo cot sang - van nam bep, chi troi noi len thoi"): anh "nam dat" (objdata ImageName) la anh ve theo goc nam nen
# nhac len van thay bep. Nay: client tra ten icon tu bang goc cua KItemGenerator (ItemGen.GetEquipmentCommonAttrib theo detail/particular +
# thu level 1..10 khop TEN vat pham may chu gui trong OBJ_ADD_SYNC; thuoc: GetMedicineCommonAttrib theo detail + level 1..5), doc kich thuoc
# bang GetImageParam, ve icon voi day icon dat tren chan cot (tam anh nam dat) + do cao VatRoi_DoCao (dung len 350 ms + nhap). Loai khong tra
# duoc icon (nhiem vu, nguyen lieu...) thi nhac anh nam dat len nhu ban c. Chay SAU va_nguon_vatroi_1409_c.py. Core/Src/KObj.cpp chi JX_MOBILE.
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_vatroi_1409_d.py [goc]
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

# 1) include + bang icon theo chi so vat the (dat ngay sau khai bao s_uVrNoiLuc)
va(KO,
   'static unsigned int s_uVrNoiLuc[MAX_OBJECT];\t// [VATROI 14/09 c] luc cham dat, cho hoat anh dung thang len (giu sau khi loe xong)' + R,
   'static unsigned int s_uVrNoiLuc[MAX_OBJECT];\t// [VATROI 14/09 c] luc cham dat, cho hoat anh dung thang len (giu sau khi loe xong)' + R +
   '// [VATROI 14/09 d] icon DUNG (anh hanh trang) cho vat pham nam dat: ten anh + kich thuoc tra theo m_nID cua vat the (tim mot lan)' + R +
   '#include "KItemGenerator.h"' + R +
   'static int          s_nVrIcID[MAX_OBJECT];\t\t// m_nID da tra (0 = chua)' + R +
   'static char         s_szVrIcon[MAX_OBJECT][80];\t// ten anh icon ("" = khong co -> nhac anh nam dat len nhu ban c)' + R +
   'static short        s_nVrIcW[MAX_OBJECT], s_nVrIcH[MAX_OBJECT];' + R +
   'static char         s_nVrIcOn[MAX_OBJECT];\t\t// 1 = m_Image dang mang ten icon (doi ten thi phai xoa uImage)' + R,
   '[VATROI 14/09 d] icon DUNG (anh hanh trang) cho vat pham nam dat')

# 2) ham tim icon + ham dung icon (truoc VatRoi_DatAnh)
va(KO,
   'static void VatRoi_DatAnh(KRUImage& a, int nFrame, int x, int y, unsigned int uAlpha, DWORD dwMau)' + R,
   '// [VATROI 14/09 d] tra ten anh icon dung cua vat pham nam dat (NULL = khong tra duoc). Client chi biet genre/detail/particular + TEN' + R +
   '// (OBJ_ADD_SYNC), khong biet level -> thu tung level trong bang goc (ItemGen) va khop ten. Ket qua nho theo m_nID; anh chua nap (GetImageParam' + R +
   '// that bai) thi khung sau thu lai.' + R +
   'static const char* VatRoi_TimIcon(KObj* p)' + R +
   '{' + R +
   '\tstatic KItem s_VrTam;' + R +
   '\tKImageParam sP;' + R +
   '\tint i = p->m_nIndex, lv, nOk = 0;' + R +
   '\tif (i <= 0 || i >= MAX_OBJECT || p->m_nID == 0)' + R +
   '\t\treturn NULL;' + R +
   '\tif (s_nVrIcID[i] == p->m_nID)' + R +
   '\t\treturn s_szVrIcon[i][0] ? s_szVrIcon[i] : NULL;' + R +
   '\ts_nVrIcID[i] = p->m_nID; s_szVrIcon[i][0] = 0; s_nVrIcW[i] = s_nVrIcH[i] = 0;' + R +
   '\tif (p->m_nGenre == item_equip)' + R +
   '\t{' + R +
   '\t\tfor (lv = 1; lv <= 10 && !nOk; lv++)' + R +
   '\t\t\tif (ItemGen.GetEquipmentCommonAttrib(p->m_nDetailType, p->m_nParticularType, lv, 0, &s_VrTam) && strcmp(s_VrTam.GetName(), p->m_szName) == 0)' + R +
   '\t\t\t\tnOk = 1;' + R +
   '\t}' + R +
   '\telse if (p->m_nGenre == item_medicine)' + R +
   '\t{' + R +
   '\t\tfor (lv = 1; lv <= 5 && !nOk; lv++)' + R +
   '\t\t\tif (ItemGen.GetMedicineCommonAttrib(p->m_nDetailType, lv, &s_VrTam) && strcmp(s_VrTam.GetName(), p->m_szName) == 0)' + R +
   '\t\t\t\tnOk = 1;' + R +
   '\t}' + R +
   '\tif (!nOk || !s_VrTam.GetImageName()[0])' + R +
   '\t\treturn NULL;' + R +
   '\tstrncpy(s_szVrIcon[i], s_VrTam.GetImageName(), 79); s_szVrIcon[i][79] = 0;' + R +
   '\tmemset(&sP, 0, sizeof(sP));' + R +
   '\tif (!g_pRepresent->GetImageParam(s_szVrIcon[i], &sP, ISI_T_SPR) || sP.nWidth <= 0 || sP.nHeight <= 0)' + R +
   '\t{' + R +
   '\t\ts_szVrIcon[i][0] = 0; s_nVrIcID[i] = 0;\t// chua nap: khung sau hoi lai' + R +
   '\t\treturn NULL;' + R +
   '\t}' + R +
   '\ts_nVrIcW[i] = sP.nWidth; s_nVrIcH[i] = sP.nHeight;' + R +
   '\tif (s_nVrThu)' + R +
   '\t\tg_DebugLog("[VATROI] icon dung vat %d \'%s\' (%d/%d/%d) -> %s %dx%d", p->m_nID, p->m_szName, p->m_nGenre, p->m_nDetailType, p->m_nParticularType, s_szVrIcon[i], sP.nWidth, sP.nHeight);' + R +
   '\treturn s_szVrIcon[i];' + R +
   '}' + R + R +
   '// [VATROI 14/09 d] goi trong KObj::Draw sau khi m_Image da tro anh nam dat: co icon -> thay bang icon dung (day icon dat tren chan cot = tam' + R +
   '// anh nam dat (x+12, y+12), nhac len theo VatRoi_DoCao - 12); khong co -> nhac anh nam dat len nhu ban c. Doi ten anh thi xoa uImage.' + R +
   'static void VatRoi_DungIcon(KObj* p, int x, int y)' + R +
   '{' + R +
   '\tint i = p->m_nIndex;' + R +
   '\tint nCao = VatRoi_DoCao(i, p->m_nDropState);' + R +
   '\tconst char* szIcon = (nCao > 0) ? VatRoi_TimIcon(p) : NULL;' + R +
   '\tif (i <= 0 || i >= MAX_OBJECT)' + R +
   '\t\treturn;' + R +
   '\tif (!szIcon)' + R +
   '\t{' + R +
   '\t\tif (s_nVrIcOn[i]) { s_nVrIcOn[i] = 0; p->m_Image.uImage = 0; }' + R +
   '\t\tp->m_Image.oPosition.nY -= nCao;' + R +
   '\t\treturn;' + R +
   '\t}' + R +
   '\tif (!s_nVrIcOn[i]) { s_nVrIcOn[i] = 1; p->m_Image.uImage = 0; }' + R +
   '\tstrcpy(p->m_Image.szImage, szIcon);' + R +
   '\tp->m_Image.nFrame = 0;' + R +
   '\tp->m_Image.oPosition.nX = x + 12 - s_nVrIcW[i] / 2;' + R +
   '\tp->m_Image.oPosition.nY = y + 12 - s_nVrIcH[i] - (nCao - 12);' + R +
   '}' + R + R +
   'static void VatRoi_DatAnh(KRUImage& a, int nFrame, int x, int y, unsigned int uAlpha, DWORD dwMau)' + R,
   '[VATROI 14/09 d] tra ten anh icon dung')

# 3) hook: thay lenh nhac anh nam dat (ban c) bang VatRoi_DungIcon
va(KO,
   '\t\tm_Image.oPosition.nY -= VatRoi_DoCao(m_nIndex, m_nDropState);\t// [VATROI 14/09 c] dung thang len trong cot sang (nhat do theo toa do ban do, khong anh huong)' + R,
   '\t\tVatRoi_DungIcon(this, x, y);\t// [VATROI 14/09 d] icon DUNG (anh hanh trang) lo lung trong cot sang; khong co icon thi nhac anh nam dat len (ban c)' + R,
   '[VATROI 14/09 d] icon DUNG (anh hanh trang) lo lung')

print('\n'.join(KQ))
