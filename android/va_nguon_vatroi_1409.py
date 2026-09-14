# -*- coding: latin-1 -*-
# [VATROI 14/09] Hieu ung VAT PHAM ROI tren dat (chu 14/09 01:0x: "lam muc 1 truoc: hieu ung anh sang tuy theo loai vat pham"), chep cach
# game 3D Kiem Vong Giang Ho (StillObject.mDropLightObj = cot sang theo pham chat, cmn_drop_flash = loe luc cham dat, xem BANGIAO §6):
#   Core/Src/KObj.cpp (chi JX_MOBILE, trong #ifndef _SERVER): cot sang \spr\vatroi\cotsang.spr ve TRUOC anh vat pham, nhan mau ten
#   (= pham chat) bang IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST, tho nhe; loe \spr\vatroi\loe.spr 6 khung khi m_nDropState 1 -> 0.
#   Bang phu theo m_nIndex (khong doi lop KObj, khong doi giao thuc). Anh: android/anh_vatroi_cotsang.py. config.ini [VatRoi].
# Idempotent, latin-1, neo CRLF (KObj.cpp co chu GBK/TCVN3 -> giu byte cao). Dung: python android/va_nguon_vatroi_1409.py [goc] [--config <ini> ...]
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

# ---------- khoi ham phu truoc KObj::Draw ----------
va(KO,
   '#ifndef _SERVER' + R + 'void KObj::Draw()' + R + '{' + R,
   '#ifdef JX_MOBILE' + R +
   '// [VATROI 14/09] Hieu ung VAT PHAM ROI tren dat (chu: "lam muc 1 truoc: hieu ung anh sang tuy theo loai vat pham"; chep cach game 3D' + R +
   '// Kiem Vong Giang Ho: StillObject.mDropLightObj = cot sang theo pham chat, cmn_drop_flash = loe luc cham dat):' + R +
   '//  - cot sang \\spr\\vatroi\\cotsang.spr (anh TRANG, android/anh_vatroi_cotsang.py, neo o chan cot) ve TRUOC anh vat pham, nhan mau ten' + R +
   '//    (= pham chat: xanh / hoang kim / do tim / bach kim; trang va do hong khong co - [VatRoi] CotSangTu), tho nhe chu ky 1,6 s; chi khi' + R +
   '//    vat da nam dat (m_nDropState != 1).' + R +
   '//  - loe \\spr\\vatroi\\loe.spr 6 khung vong dan ra 0,42 s ngay khi m_nDropState 1 -> 0 (cham dat), ve SAU anh vat pham, nhan mau ten.' + R +
   '//  Bang phu theo m_nIndex, khong doi lop KObj, khong doi giao thuc. config.ini [VatRoi]: CotSang=1 CotSangTu=1 CotSangAlpha=170 Loe=1.' + R +
   'static int          s_nVrDoc = 0, s_nVrCotSang = 1, s_nVrCotSangTu = 1, s_nVrAlpha = 170, s_nVrLoe = 1;' + R +
   'static unsigned int s_uVrLoeLuc[MAX_OBJECT];' + R +
   'static KRUImage     s_VrCot, s_VrLoe;' + R + R +
   'static void VatRoi_DocCfg()' + R +
   '{' + R +
   '\tchar szCfg[MAX_PATH];' + R +
   '\tif (s_nVrDoc)' + R +
   '\t\treturn;' + R +
   '\ts_nVrDoc = 1;' + R +
   '\tGetCurrentDirectory(MAX_PATH, szCfg); strcat(szCfg, "\\\\Config.ini");' + R +
   '\ts_nVrCotSang   = GetPrivateProfileInt("VatRoi", "CotSang", 1, szCfg);' + R +
   '\ts_nVrCotSangTu = GetPrivateProfileInt("VatRoi", "CotSangTu", 1, szCfg);' + R +
   '\ts_nVrAlpha     = GetPrivateProfileInt("VatRoi", "CotSangAlpha", 170, szCfg);' + R +
   '\ts_nVrLoe       = GetPrivateProfileInt("VatRoi", "Loe", 1, szCfg);' + R +
   '\tif (s_nVrAlpha < 30) s_nVrAlpha = 30; if (s_nVrAlpha > 255) s_nVrAlpha = 255;' + R +
   '\tmemset(&s_VrCot, 0, sizeof(s_VrCot)); memset(&s_VrLoe, 0, sizeof(s_VrLoe)); memset(s_uVrLoeLuc, 0, sizeof(s_uVrLoeLuc));' + R +
   '\tstrcpy(s_VrCot.szImage, "\\\\spr\\\\vatroi\\\\cotsang.spr"); strcpy(s_VrLoe.szImage, "\\\\spr\\\\vatroi\\\\loe.spr");' + R +
   '\tg_DebugLog("[VATROI] cot sang=%d (tu mau %d, alpha %d) loe=%d", s_nVrCotSang, s_nVrCotSangTu, s_nVrAlpha, s_nVrLoe);' + R +
   '}' + R + R +
   'static void VatRoi_ChamDat(int nIndex)' + R +
   '{' + R +
   '\tif (nIndex > 0 && nIndex < MAX_OBJECT)' + R +
   '\t\ts_uVrLoeLuc[nIndex] = (unsigned int)GetTickCount();' + R +
   '}' + R + R +
   'static void VatRoi_DatAnh(KRUImage& a, int nFrame, int x, int y, unsigned int uAlpha, DWORD dwMau)' + R +
   '{' + R +
   '\ta.nFrame = nFrame; a.nType = ISI_T_SPR;' + R +
   '\ta.oPosition.nX = x; a.oPosition.nY = y; a.oPosition.nZ = 0;' + R +
   '\ta.nISPosition = IMAGE_IS_POSITION_INIT; a.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;' + R +
   '\ta.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;\t// nhan mau (rgb != 0) + alpha' + R +
   '\tif ((dwMau & 0x00ffffff) == 0) dwMau = 0x00ffffff;' + R +
   '\ta.Color.Color_dw = (uAlpha << 24) | (dwMau & 0x00ffffff);' + R +
   '}' + R + R +
   '// cot sang: ve truoc anh vat pham; vat dang roi thi chua co; pham chat duoi CotSangTu, do hong (2) hay ngoai bang (>= 6) thi khong' + R +
   'static void VatRoi_VeDuoi(int nColorID, DWORD dwMau, int nDropState, int x, int y)' + R +
   '{' + R +
   '\tunsigned int uTho;' + R +
   '\tVatRoi_DocCfg();' + R +
   '\tif (!s_nVrCotSang || nDropState == 1 || nColorID < s_nVrCotSangTu || nColorID == 2 || nColorID >= 6)' + R +
   '\t\treturn;' + R +
   '\tuTho = (unsigned int)GetTickCount() % 1600; if (uTho >= 800) uTho = 1600 - uTho;\t// tam giac 0..800: tho nhe' + R +
   '\tVatRoi_DatAnh(s_VrCot, 0, x, y, (unsigned int)s_nVrAlpha * (200 + uTho / 8) / 300, dwMau);\t// alpha x 0,67..1,0' + R +
   '\tg_pRepresent->DrawPrimitives(1, &s_VrCot, RU_T_IMAGE, 0);' + R +
   '}' + R + R +
   '// loe: ve sau anh vat pham, 6 khung x 70 ms ke tu luc cham dat' + R +
   'static void VatRoi_VeTren(int nIndex, DWORD dwMau, int x, int y)' + R +
   '{' + R +
   '\tunsigned int uDa; int nKhung;' + R +
   '\tVatRoi_DocCfg();' + R +
   '\tif (!s_nVrLoe || nIndex <= 0 || nIndex >= MAX_OBJECT || !s_uVrLoeLuc[nIndex])' + R +
   '\t\treturn;' + R +
   '\tuDa = (unsigned int)GetTickCount() - s_uVrLoeLuc[nIndex];' + R +
   '\tif (uDa >= 420) { s_uVrLoeLuc[nIndex] = 0; return; }' + R +
   '\tnKhung = (int)(uDa / 70); if (nKhung > 5) nKhung = 5;' + R +
   '\tVatRoi_DatAnh(s_VrLoe, nKhung, x, y, 240 - (unsigned int)nKhung * 36, dwMau);' + R +
   '\tg_pRepresent->DrawPrimitives(1, &s_VrLoe, RU_T_IMAGE, 0);' + R +
   '}' + R +
   '#endif' + R +
   '#ifndef _SERVER' + R + 'void KObj::Draw()' + R + '{' + R,
   'static void VatRoi_VeDuoi(')
# cay da va bang ban dau (co dong trong sau #endif -> ban Windows du mot dong trong, kiem --pc HONG)
va(KO,
   '#endif' + R + R + '#ifndef _SERVER' + R + 'void KObj::Draw()' + R,
   '#endif' + R + '#ifndef _SERVER' + R + 'void KObj::Draw()' + R,
   '#endif' + R + '#ifndef _SERVER' + R + 'void KObj::Draw()')

# ---------- trong Draw: cot sang truoc anh, loe sau anh ----------
va(KO,
   '\t\tstrcpy(m_Image.szImage, m_cImage.m_szName);' + R + '\t}' + R + R + '\tswitch(m_nKind)' + R,
   '\t\tstrcpy(m_Image.szImage, m_cImage.m_szName);' + R + '\t}' + R + R +
   '#ifdef JX_MOBILE' + R +
   '\tif (m_nKind == Obj_Kind_Item)' + R +
   '\t\tVatRoi_VeDuoi(m_nColorID, m_dwNameColor, m_nDropState, x, y);\t// [VATROI 14/09] cot sang duoi vat pham (ve truoc anh)' + R +
   '#endif' + R +
   '\tswitch(m_nKind)' + R,
   'VatRoi_VeDuoi(m_nColorID, m_dwNameColor, m_nDropState, x, y);')
va(KO,
   '\tdefault:' + R + '\t\tg_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);\t' + R + '\t\tbreak;' + R + '\t}' + R + '#ifdef SWORDONLINE_SHOW_DBUG_INFO' + R,
   '\tdefault:' + R + '\t\tg_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);\t' + R + '\t\tbreak;' + R + '\t}' + R +
   '#ifdef JX_MOBILE' + R +
   '\tif (m_nKind == Obj_Kind_Item)' + R +
   '\t\tVatRoi_VeTren(m_nIndex, m_dwNameColor, x, y);\t// [VATROI 14/09] loe luc cham dat (ve sau anh)' + R +
   '#endif' + R +
   '#ifdef SWORDONLINE_SHOW_DBUG_INFO' + R,
   'VatRoi_VeTren(m_nIndex, m_dwNameColor, x, y);')

# ---------- cham dat: m_nDropState 1 -> 0 (trong Activate, nhanh client) ----------
va(KO,
   '\t\t\t\t\tm_Image.uImage = 0;' + R + '\t\t\t\t}' + R + '\t\t\t}' + R + '\t\t\tnMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;' + R,
   '\t\t\t\t\tm_Image.uImage = 0;' + R +
   '#ifdef JX_MOBILE' + R +
   '\t\t\t\t\tVatRoi_ChamDat(m_nIndex);\t// [VATROI 14/09] loe luc cham dat' + R +
   '#endif' + R +
   '\t\t\t\t}' + R + '\t\t\t}' + R + '\t\t\tnMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;' + R,
   'VatRoi_ChamDat(m_nIndex);')

# [VATROI 14/09 b] go loi: [VatRoi] Thu=1 -> tien roi (Obj_Kind_Money) cung co cot sang + loe (may ao: auto nhat do ngay, tien roi nhieu
# hon nen de chup anh kiem); mac dinh 0.
va(KO,
   'static int          s_nVrDoc = 0, s_nVrCotSang = 1, s_nVrCotSangTu = 1, s_nVrAlpha = 170, s_nVrLoe = 1;' + R,
   'static int          s_nVrDoc = 0, s_nVrCotSang = 1, s_nVrCotSangTu = 1, s_nVrAlpha = 170, s_nVrLoe = 1, s_nVrThu = 0;' + R,
   's_nVrLoe = 1, s_nVrThu = 0;')
va(KO,
   '\ts_nVrLoe       = GetPrivateProfileInt("VatRoi", "Loe", 1, szCfg);' + R,
   '\ts_nVrLoe       = GetPrivateProfileInt("VatRoi", "Loe", 1, szCfg);' + R +
   '\ts_nVrThu       = GetPrivateProfileInt("VatRoi", "Thu", 0, szCfg);\t// [VATROI 14/09 b] go loi: tien roi cung co hieu ung' + R,
   '"VatRoi", "Thu"')
va(KO,
   '\tg_DebugLog("[VATROI] cot sang=%d (tu mau %d, alpha %d) loe=%d", s_nVrCotSang, s_nVrCotSangTu, s_nVrAlpha, s_nVrLoe);' + R,
   '\tg_DebugLog("[VATROI] cot sang=%d (tu mau %d, alpha %d) loe=%d thu=%d", s_nVrCotSang, s_nVrCotSangTu, s_nVrAlpha, s_nVrLoe, s_nVrThu);' + R,
   'loe=%d thu=%d"')
va(KO,
   '\tif (m_nKind == Obj_Kind_Item)' + R + '\t\tVatRoi_VeDuoi(m_nColorID, m_dwNameColor, m_nDropState, x, y);',
   '\tif (m_nKind == Obj_Kind_Item || (s_nVrThu && m_nKind == Obj_Kind_Money))' + R + '\t\tVatRoi_VeDuoi(m_nColorID, m_dwNameColor, m_nDropState, x, y);',
   '(s_nVrThu && m_nKind == Obj_Kind_Money))' + R + '\t\tVatRoi_VeDuoi(')
va(KO,
   '\tif (m_nKind == Obj_Kind_Item)' + R + '\t\tVatRoi_VeTren(m_nIndex, m_dwNameColor, x, y);',
   '\tif (m_nKind == Obj_Kind_Item || (s_nVrThu && m_nKind == Obj_Kind_Money))' + R + '\t\tVatRoi_VeTren(m_nIndex, m_dwNameColor, x, y);',
   '(s_nVrThu && m_nKind == Obj_Kind_Money))' + R + '\t\tVatRoi_VeTren(')

# [VATROI 14/09 c] Thu=2: moi vat the ban do (cay, den, thung...) cung co cot sang - chi de kiem diem neo / mau nhuom tren may ao trong thanh
va(KO,
   '\tif (m_nKind == Obj_Kind_Item || (s_nVrThu && m_nKind == Obj_Kind_Money))' + R + '\t\tVatRoi_VeDuoi(',
   '\tif (m_nKind == Obj_Kind_Item || (s_nVrThu && m_nKind == Obj_Kind_Money) || s_nVrThu >= 2)' + R + '\t\tVatRoi_VeDuoi(',
   '|| s_nVrThu >= 2)' + R + '\t\tVatRoi_VeDuoi(')

# ---------- config.ini [VatRoi] ----------
KHOI = [
    '',
    '[VatRoi]',
    '; [VATROI 14/09] vat pham roi tren dat (chep game 3D): cot sang duoi vat theo pham chat + loe luc cham dat (anh spr\\vatroi\\, sinh boi',
    ';   android/anh_vatroi_cotsang.py). CotSang=1 bat cot sang; CotSangTu = pham chat toi thieu co cot: 0 trang, 1 xanh, 3 hoang kim,',
    ';   4 do tim, 5 bach kim (do hong (2) khong bao gio); CotSangAlpha 30..255 do dam; Loe=1 vong sang dan ra luc vat vua cham dat',
    'CotSang=1',
    'CotSangTu=1',
    'CotSangAlpha=170',
    'Loe=1',
]
def va_config(p):
    s = doc(p)
    if '[VatRoi]' in s:
        KQ.append('bo qua (da co): %s : [VatRoi]' % p); return
    nl = '\r\n' if s.count('\r\n') > s.count('\n') // 2 else '\n'
    if not s.endswith(nl): s += nl
    ghi(p, s + nl.join(KHOI) + nl)
    KQ.append('da va: %s : [VatRoi]' % p)
va_config(os.path.join(GOC, 'android', 'du_lieu_ghi_de', 'config.ini'))
if '--config' in sys.argv:
    for p in sys.argv[sys.argv.index('--config') + 1:]:
        if os.path.isfile(p): va_config(p)
        else: KQ.append('khong co tep: %s' % p)
print('\n'.join(KQ))
