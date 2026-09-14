# -*- coding: latin-1 -*-
# [HAOQUANG 14/09 e] Phan b cua va_nguon_haoquang_1409.py (chay SAU no): chan doan anh vong + khoa thu [HaoQuang] ThuAnh
#   ThuAnh=1: vong quai dung anh THAT vongtrondo.spr (khong nhuom) - thu duong ve;  ThuAnh=2: vongquai.spr khong nhuom (ALPHA thuong);
#   ThuAnh=3: vongquai.spr nhuom trang (COLOR_ADJUST 0xFFFFFF). Moi 3 s ghi log [HAOQUANG] anh ...: ten, uImage, kieu ve, mau,
#   GetImageParam (nap duoc khong, so khung, kich thuoc) de biet anh sinh boi PIL co duoc Represent3 nap khong.
# Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_haoquang_1409_b.py [goc]
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

KN = os.path.join(GOC, 'Sources', 'Core', 'Src', 'KNpc.cpp')

# 1) bang anh 5 -> 6 (them vongtrondo.spr that de thu duong ve) + khoa ThuAnh
va(KN,
   'static int         s_nHqDoc = 0, s_nHqQuai = 1, s_nHqTrangBi = 1, s_nHqAlpha = 220, s_nHqTrangBiTu = 3, s_nHqThu = 0, s_nHqBao = 0;' + R +
   'static KRUImage    s_HqAnh[5];' + R +
   'static const char* s_HqTen[5] = { "\\\\spr\\\\haoquang\\\\vongquai.spr", "\\\\spr\\\\haoquang\\\\vongboss.spr",' + R +
   '\t"\\\\spr\\\\haoquang\\\\vongtronvang.spr", "\\\\spr\\\\haoquang\\\\vongtrontim.spr", "\\\\spr\\\\haoquang\\\\vongtronxanh.spr" };' + R +
   'static const int   s_HqKhung[5] = { 12, 16, 30, 30, 30 };' + R +
   'static const int   s_HqMs[5]    = { 70, 70, 50, 50, 80 };' + R,
   'static int         s_nHqDoc = 0, s_nHqQuai = 1, s_nHqTrangBi = 1, s_nHqAlpha = 220, s_nHqTrangBiTu = 3, s_nHqThu = 0, s_nHqBao = 0;' + R +
   'static int         s_nHqThuAnh = 0;\t// [HAOQUANG 14/09 e] [HaoQuang] ThuAnh: 1 vong quai = vongtrondo.spr that, 2 vongquai.spr khong nhuom, 3 nhuom trang' + R +
   'static KRUImage    s_HqAnh[6];' + R +
   'static const char* s_HqTen[6] = { "\\\\spr\\\\haoquang\\\\vongquai.spr", "\\\\spr\\\\haoquang\\\\vongboss.spr",' + R +
   '\t"\\\\spr\\\\haoquang\\\\vongtronvang.spr", "\\\\spr\\\\haoquang\\\\vongtrontim.spr", "\\\\spr\\\\haoquang\\\\vongtronxanh.spr",' + R +
   '\t"\\\\spr\\\\haoquang\\\\vongtrondo.spr" };' + R +
   'static const int   s_HqKhung[6] = { 12, 16, 30, 30, 30, 30 };' + R +
   'static const int   s_HqMs[6]    = { 70, 70, 50, 50, 80, 50 };' + R,
   '[HAOQUANG 14/09 e] [HaoQuang] ThuAnh')

# 2) doc khoa ThuAnh
va(KN,
   '\ts_nHqThu       = GetPrivateProfileInt("HaoQuang", "Thu", 0, szCfg);' + R,
   '\ts_nHqThu       = GetPrivateProfileInt("HaoQuang", "Thu", 0, szCfg);' + R +
   '\ts_nHqThuAnh    = GetPrivateProfileInt("HaoQuang", "ThuAnh", 0, szCfg);\t// [HAOQUANG 14/09 e]' + R,
   'GetPrivateProfileInt("HaoQuang", "ThuAnh"')

# 3) log chan doan moi 3 s sau DrawPrimitives
va(KN,
   '\tg_pRepresent->DrawPrimitives(1, &a, RU_T_IMAGE, 0);' + R +
   '\tif (!(s_nHqBao & (1 << nAnh)))' + R,
   '\tg_pRepresent->DrawPrimitives(1, &a, RU_T_IMAGE, 0);' + R +
   '\tif (s_nHqThu)\t// [HAOQUANG 14/09 e] chan doan: anh co nap duoc khong (moi 3 s)' + R +
   '\t{' + R +
   '\t\tstatic unsigned int s_uHqAnhLuc = 0;' + R +
   '\t\tif ((unsigned int)GetTickCount() - s_uHqAnhLuc > 3000)' + R +
   '\t\t{' + R +
   '\t\t\tKImageParam sP;' + R +
   '\t\t\tint nOk;' + R +
   '\t\t\tmemset(&sP, 0, sizeof(sP));' + R +
   '\t\t\ts_uHqAnhLuc = (unsigned int)GetTickCount();' + R +
   '\t\t\tnOk = g_pRepresent->GetImageParam(a.szImage, &sP, ISI_T_SPR) ? 1 : 0;' + R +
   '\t\t\tg_DebugLog("[HAOQUANG] anh %d \'%s\' uImage %u isPos %d kieu %d mau %08X khung %d: param %d (khung %d itv %d %dx%d)", nAnh, a.szImage,' + R +
   '\t\t\t\t(unsigned int)a.uImage, (int)a.nISPosition, (int)a.bRenderStyle, (unsigned int)a.Color.Color_dw, a.nFrame, nOk,' + R +
   '\t\t\t\t(int)sP.nNumFrames, (int)sP.nInterval, (int)sP.nWidth, (int)sP.nHeight);' + R +
   '\t\t}' + R +
   '\t}' + R +
   '\tif (!(s_nHqBao & (1 << nAnh)))' + R,
   '[HAOQUANG 14/09 e] chan doan: anh co nap duoc khong')

# 4) ap ThuAnh cho vong quai (nAnh 0/1) truoc khi ve
va(KN,
   '\tif (nAnh < 0)' + R + '\t\treturn;' + R + '\tp->GetNpcRes()->GetPos(&x, &y);' + R,
   '\tif (nAnh < 0)' + R + '\t\treturn;' + R +
   '\tif (s_nHqThuAnh && nAnh <= 1)\t// [HAOQUANG 14/09 e] thu duong ve' + R +
   '\t{' + R +
   '\t\tif (s_nHqThuAnh == 1) { nAnh = 5; dwMau = 0; }' + R +
   '\t\telse if (s_nHqThuAnh == 2) dwMau = 0;' + R +
   '\t\telse dwMau = 0xFFFFFF;' + R +
   '\t}' + R +
   '\tp->GetNpcRes()->GetPos(&x, &y);' + R,
   '[HAOQUANG 14/09 e] thu duong ve')

print('\n'.join(KQ))
