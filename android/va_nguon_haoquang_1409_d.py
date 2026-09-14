# -*- coding: latin-1 -*-
# [HAOQUANG 14/09 d] BO vong hao quang DO MAC cua nguoi choi (chu 14/09 10:2x: "bo vong do o player di"). Giu vong quai/boss + sang vat roi.
#   Core/Src/KNpc.cpp: cat nhanh kind_player trong HaoQuang_Ve, bo s_nHqTrangBi / TrangBiTu / bang anh 7 -> 6 (bo vongnguoi.spr),
#                      JxHaoQuang_DatBat(int nQuai). UiOptions2.h/.cpp: bo OPTION_I_HQTRANGBI, MAX_TOGGLE_BTN_COUNT 10 -> 9, ten/luu/ap.
#   Du lieu: ui/ui3/uioptions2.ini bo "8=Vong do mac", "9=Sang vat roi" -> 8; config.ini [HaoQuang] viet lai (bo TrangBiTu); xoa
#            spr/haoquang/vongnguoi.spr (lop ghi de, may ao, dt_v4 - nho --chi-manifest); sinh_uioptions2_haoquang.py + anh_haoquang_vong.py cap nhat.
# Chay SAU cac kich ban a/b/c. Idempotent, latin-1, neo CRLF. Dung: python android/va_nguon_haoquang_1409_d.py [goc] [D:\jx1_android_data_dt_v4]
import io, os, sys, re

ARGS = [a for a in sys.argv[1:] if not a.startswith('--')]
GOC = ARGS[0] if ARGS else os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DT = ARGS[1] if len(ARGS) > 1 else None
KQ = []
R = '\r\n'

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
        KQ.append('bo qua (da co): %s : %s' % (os.path.basename(p), dau_hieu[:50])); return
    if moi == '' and cu not in s:
        KQ.append('bo qua (da xoa): %s : %s' % (os.path.basename(p), dau_hieu[:50])); return
    n = s.count(cu)
    if n != so_lan:
        raise SystemExit('LOI: neo gap %d lan (can %d) trong %s: %r' % (n, so_lan, p, cu[:90]))
    s2 = s.replace(cu, moi)
    if cao(s) != cao(s2):
        raise SystemExit('LOI: so byte cao doi o %s' % p)
    ghi(p, s2)
    KQ.append('da va: %s : %s' % (os.path.basename(p), dau_hieu[:50]))

def cat_khoi(p, dau, cuoi, dau_hieu):
    """Cat [dau, cuoi) (cuoi = lan xuat hien dau tien SAU dau). Idempotent: khong thay 'dau' = da cat."""
    s = doc(p)
    i = s.find(dau)
    if i < 0:
        KQ.append('bo qua (da cat): %s : %s' % (os.path.basename(p), dau_hieu)); return
    j = s.find(cuoi, i)
    if j < 0:
        raise SystemExit('LOI: khong thay cuoi khoi %s' % dau_hieu)
    s2 = s[:i] + s[j:]
    if cao(s) != cao(s2):
        raise SystemExit('LOI: khoi cat co byte cao (%s)' % dau_hieu)
    ghi(p, s2)
    KQ.append('da cat: %s : %s (%d ky tu)' % (os.path.basename(p), dau_hieu, j - i))

KN = os.path.join(GOC, 'Sources', 'Core', 'Src', 'KNpc.cpp')
OH = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiOptions2.h')
OC = os.path.join(GOC, 'Sources', 'S3Client', 'Ui', 'UiCase', 'UiOptions2.cpp')

# ---------------- KNpc.cpp ----------------
cat_khoi(KN,
         '\telse if (p->m_Kind == kind_player && p->m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex)' + R,
         '\tif (s_nHqThu && p->m_Kind != kind_player)' + R,
         'nhanh vong do mac cua nguoi choi')

va(KN,
   '\tint x = 0, y = 0, nAnh = -1, i, nMau = 0, nIdx, c;' + R,
   '\tint x = 0, y = 0, nAnh = -1;\t// [HAOQUANG 14/09 d] bo vong do mac (chu: "bo vong do o player di")' + R,
   '[HAOQUANG 14/09 d] bo vong do mac')

va(KN,
   'static int         s_nHqDoc = 0, s_nHqQuai = 1, s_nHqTrangBi = 1, s_nHqAlpha = 220, s_nHqTrangBiTu = 3, s_nHqThu = 0, s_nHqBao = 0;' + R,
   'static int         s_nHqDoc = 0, s_nHqQuai = 1, s_nHqAlpha = 220, s_nHqThu = 0, s_nHqBao = 0;\t// [HAOQUANG 14/09 d] bo s_nHqTrangBi/TrangBiTu' + R,
   '[HAOQUANG 14/09 d] bo s_nHqTrangBi/TrangBiTu')

va(KN,
   'static KRUImage    s_HqAnh[7];' + R +
   'static const char* s_HqTen[7] = { "\\\\spr\\\\haoquang\\\\vongquai.spr", "\\\\spr\\\\haoquang\\\\vongboss.spr",' + R +
   '\t"\\\\spr\\\\haoquang\\\\vongtronvang.spr", "\\\\spr\\\\haoquang\\\\vongtrontim.spr", "\\\\spr\\\\haoquang\\\\vongtronxanh.spr",' + R +
   '\t"\\\\spr\\\\haoquang\\\\vongtrondo.spr", "\\\\spr\\\\haoquang\\\\vongnguoi.spr" };\t// [HAOQUANG 14/09 c] 6 = vong do mac kieu 3D (anh trang, nhuom)' + R +
   'static const int   s_HqKhung[7] = { 12, 16, 30, 30, 30, 30, 16 };' + R +
   'static const int   s_HqMs[7]    = { 70, 70, 50, 50, 80, 50, 70 };' + R,
   'static KRUImage    s_HqAnh[6];\t// [HAOQUANG 14/09 d] 0 quai, 1 boss, 2..5 anh that (chi ThuAnh=1 dung 5); bo vongnguoi.spr' + R +
   'static const char* s_HqTen[6] = { "\\\\spr\\\\haoquang\\\\vongquai.spr", "\\\\spr\\\\haoquang\\\\vongboss.spr",' + R +
   '\t"\\\\spr\\\\haoquang\\\\vongtronvang.spr", "\\\\spr\\\\haoquang\\\\vongtrontim.spr", "\\\\spr\\\\haoquang\\\\vongtronxanh.spr",' + R +
   '\t"\\\\spr\\\\haoquang\\\\vongtrondo.spr" };' + R +
   'static const int   s_HqKhung[6] = { 12, 16, 30, 30, 30, 30 };' + R +
   'static const int   s_HqMs[6]    = { 70, 70, 50, 50, 80, 50 };' + R,
   '[HAOQUANG 14/09 d] 0 quai, 1 boss')

va(KN,
   'extern "C" void JxHaoQuang_DatBat(int nQuai, int nTrangBi)' + R + '{' + R + '\ts_nHqQuai    = nQuai ? 1 : 0;' + R + '\ts_nHqTrangBi = nTrangBi ? 1 : 0;' + R + '}' + R,
   'extern "C" void JxHaoQuang_DatBat(int nQuai)\t// [HAOQUANG 14/09 d] chi con cong tac vong quai' + R + '{' + R + '\ts_nHqQuai = nQuai ? 1 : 0;' + R + '}' + R,
   '[HAOQUANG 14/09 d] chi con cong tac vong quai')

va(KN,
   '\ts_nHqTrangBiTu = GetPrivateProfileInt("HaoQuang", "TrangBiTu", 3, szCfg);' + R,
   '',
   'KHONG-CO-DAU-HIEU-TrangBiTu-' + ('x' if 'GetPrivateProfileInt("HaoQuang", "TrangBiTu"' not in doc(KN) else 'y'))

va(KN,
   '\tg_DebugLog("[HAOQUANG] quai=%d trang bi=%d alpha=%d trang bi tu=%d thu=%d", s_nHqQuai, s_nHqTrangBi, s_nHqAlpha, s_nHqTrangBiTu, s_nHqThu);' + R,
   '\tg_DebugLog("[HAOQUANG] quai=%d alpha=%d thu=%d thu anh=%d", s_nHqQuai, s_nHqAlpha, s_nHqThu, s_nHqThuAnh);' + R,
   '[HAOQUANG] quai=%d alpha=%d thu=%d thu anh=%d')

va(KN,
   '//  - TRANG BI (chi nhan vat cua MINH - client khong biet do nguoi khac dang mac, khong doi giao thuc): pham chat cao nhat dang mac' + R +
   '//    >= [HaoQuang] TrangBiTu (3 hoang kim) -> vong hao quang co san cua JX1: hoang kim vongtronvang.spr, do tim vongtrontim.spr,' + R +
   '//    bach kim vongtronxanh.spr (30 khung, giu mau goc).' + R,
   '//  - TRANG BI (vong duoi chan nhan vat minh theo pham chat do mac, ban c dung vongnguoi.spr): DA BO 14/09 10:2x theo chu ("bo vong do o player di").' + R,
   'DA BO 14/09 10:2x theo chu')

va(KN,
   'Alpha / TrangBiTu / Thu (1 = moi quai thuong cung co vong xanh, 2 = ca minh',
   'Alpha / Thu (1 = moi quai thuong cung co vong xanh, 3 = ca NPC thoai',
   'Alpha / Thu (1 = moi quai thuong cung co vong xanh, 3 = ca NPC thoai')

# ---------------- UiOptions2.h ----------------
va(OH,
   '\tOPTION_I_HQTRANGBI,\t\t// vong hao quang trang bi cua minh' + R,
   '',
   'KHONG-CO-DAU-HIEU-HQTRANGBI-' + ('x' if 'OPTION_I_HQTRANGBI' not in doc(OH) else 'y'))

va(OH,
   '#define MAX_TOGGLE_BTN_COUNT 10\t// [HAOQUANG 14/09] 7 -> 10',
   '#define MAX_TOGGLE_BTN_COUNT 9\t// [HAOQUANG 14/09 d] 10 -> 9 (bo Vong do mac theo chu); [HAOQUANG 14/09] 7 -> 10',
   '[HAOQUANG 14/09 d] 10 -> 9')

# ---------------- UiOptions2.cpp ----------------
va(OC,
   'extern "C" void JxHaoQuang_DatBat(int nQuai, int nTrangBi);',
   'extern "C" void JxHaoQuang_DatBat(int nQuai);',
   'JxHaoQuang_DatBat(int nQuai);')

va(OC,
   '#define MAX_TOGGLE_BTN_COUNT 10\t// [HAOQUANG 14/09] 10 nut;',
   '#define MAX_TOGGLE_BTN_COUNT 9\t// [HAOQUANG 14/09 d] 9 nut (bo Vong do mac);',
   '[HAOQUANG 14/09 d] 9 nut')

va(OC,
   '\t"HaoQuangQuai", "HaoQuangTrangBi", "SangVatRoi",\t// [HAOQUANG 14/09]' + R,
   '\t"HaoQuangQuai", "SangVatRoi",\t// [HAOQUANG 14/09] (14/09 d: bo HaoQuangTrangBi)' + R,
   '(14/09 d: bo HaoQuangTrangBi)')

va(OC,
   '\tcase OPTION_I_HQQUAI:' + R + '\tcase OPTION_I_HQTRANGBI:\t// [HAOQUANG 14/09] ap ngay' + R +
   '\t\tJxHaoQuang_DatBat(m_ToggleItemList[OPTION_I_HQQUAI].bEnable, m_ToggleItemList[OPTION_I_HQTRANGBI].bEnable);' + R,
   '\tcase OPTION_I_HQQUAI:\t// [HAOQUANG 14/09] ap ngay' + R +
   '\t\tJxHaoQuang_DatBat(m_ToggleItemList[OPTION_I_HQQUAI].bEnable);' + R,
   'JxHaoQuang_DatBat(m_ToggleItemList[OPTION_I_HQQUAI].bEnable);')

va(OC,
   '\t\t\tpSetting->GetInteger(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2[OPTION_I_HQTRANGBI], true, &bOptionsEnable[OPTION_I_HQTRANGBI]);' + R,
   '',
   'KHONG-CO-DAU-HIEU-LOAD-HQTRANGBI-' + ('x' if 'ls_ToggleOptionName2[OPTION_I_HQTRANGBI]' not in doc(OC) else 'y'))

va(OC,
   '\t\tJxHaoQuang_DatBat(bOptionsEnable[OPTION_I_HQQUAI], bOptionsEnable[OPTION_I_HQTRANGBI]);\t// [HAOQUANG 14/09]' + R,
   '\t\tJxHaoQuang_DatBat(bOptionsEnable[OPTION_I_HQQUAI]);\t// [HAOQUANG 14/09]' + R,
   'JxHaoQuang_DatBat(bOptionsEnable[OPTION_I_HQQUAI]);')

# ---------------- du lieu: uioptions2.ini (bo 8=Vong do mac, 9 -> 8), config [HaoQuang] viet lai, xoa vongnguoi.spr ----------------
sys.path.insert(0, os.path.join(os.path.expanduser('~'), '.claude', 'skills', 'swordonline-dev', 'scripts'))
from vn_edit import vn
TEN_BO = vn(u'V\u00f2ng \u0111\u1ed3 m\u1eb7c')
DATA = [os.path.join(GOC, 'android', 'du_lieu_ghi_de'), r'D:\jx1_android_data'] + ([DT] if DT else [])
for goc in DATA:
    p = os.path.join(goc, 'ui', 'ui3', 'uioptions2.ini')
    if os.path.isfile(p):
        s = doc(p)
        nl = R if s.count(R) > s.count('\n') // 2 else '\n'
        if nl + '8=' + TEN_BO in s:
            s2 = s.replace(nl + '8=' + TEN_BO, '', 1)
            i = s2.find(nl + '9=')
            if i < 0:
                raise SystemExit('khong thay 9= trong ' + p)
            s2 = s2[:i] + nl + '8=' + s2[i + len(nl) + 2:]
            s2 = s2.replace('7 vong hao quang quai tinh anh/boss (Core KNpc.cpp), 8 vong hao quang trang bi cua minh, 9 cot sang + loe vat pham roi (Core KObj.cpp)',
                            '7 vong hao quang quai tinh anh/boss (Core KNpc.cpp), 8 cot sang + loe vat pham roi (Core KObj.cpp); 14/09 d bo Vong do mac theo chu', 1)
            if cao(s2) != cao(s) - cao(TEN_BO):
                raise SystemExit('byte cao lech o ' + p)
            ghi(p, s2); KQ.append('ini: bo Vong do mac: ' + p)
        else:
            KQ.append('ini: da bo: ' + p)
    p = os.path.join(goc, 'config.ini')
    if os.path.isfile(p):
        s = doc(p)
        if '[HaoQuang]' in s and 'TrangBiTu=' in s:
            i = s.index('[HaoQuang]'); j = s.find(R + '[', i + 1); j = len(s) if j < 0 else j
            khoi = s[i:j]
            thu = re.search(r'(?m)^Thu=(\d+)', khoi); thuanh = re.search(r'(?m)^ThuAnh=(\d+)', khoi); alpha = re.search(r'(?m)^Alpha=(\d+)', khoi)
            moi = ('[HaoQuang]' + R +
                   '; [HAOQUANG 14/09] vong hao quang duoi chan quai tinh anh / boss theo loai (chep game 3D halo_npc_*/halo_boss_red): anh trang' + R +
                   ';   spr\\haoquang\\vongquai.spr / vongboss.spr sinh boi android/anh_haoquang_vong.py, nhuom mau: xanh duong quai xanh, hoang kim, tim su kien,' + R +
                   ';   do chien truong, xanh la khac. BAT/TAT o Cai dat > Toi uu (Vong quai). Alpha 30..255 do dam; Thu=1 moi quai thuong cung co vong xanh' + R +
                   ';   (thu may ao), 3 = ca NPC thoai; ThuAnh go loi (1 vongtrondo.spr that, 2 khong nhuom, 3 nhuom trang). 14/09 d: BO vong do mac cua minh theo chu.' + R +
                   'Alpha=%s' % (alpha.group(1) if alpha else '220') + R +
                   'Thu=%s' % (thu.group(1) if thu else '0') + R +
                   'ThuAnh=%s' % (thuanh.group(1) if thuanh else '0') + R)
            s2 = s[:i] + moi + s[j:]
            if cao(s2) != cao(s):
                raise SystemExit('byte cao lech o ' + p)
            ghi(p, s2); KQ.append('config: viet lai [HaoQuang]: ' + p)
        else:
            KQ.append('config: da bo TrangBiTu hoac khong co [HaoQuang]: ' + p)
    p = os.path.join(goc, 'spr', 'haoquang', 'vongnguoi.spr')
    if os.path.isfile(p):
        os.remove(p); KQ.append('xoa: ' + p)

# ---------------- kich ban sinh: sinh_uioptions2_haoquang.py (2 ten), anh_haoquang_vong.py (khong sinh vongnguoi) ----------------
p = os.path.join(GOC, 'android', 'sinh_uioptions2_haoquang.py')
s = doc(p)
if 'u"V\\u00f2ng \\u0111\\u1ed3 m\\u1eb7c", ' in s:
    s = s.replace('u"V\\u00f2ng \\u0111\\u1ed3 m\\u1eb7c", ', '', 1)
    s = s.replace('# 7, 8, 9', '# 7, 8 (14/09 d: bo Vong do mac)', 1)
    s = s.replace('7 vong hao quang quai tinh anh/boss (Core KNpc.cpp), 8 vong hao quang trang bi cua minh, 9 cot sang + loe vat pham roi (Core KObj.cpp)',
                  '7 vong hao quang quai tinh anh/boss (Core KNpc.cpp), 8 cot sang + loe vat pham roi (Core KObj.cpp); 14/09 d bo Vong do mac theo chu', 1)
    s = s.replace('[ToggleOptionsName] 7..9 = Vong quai / Vong do mac / Sang vat roi', '[ToggleOptionsName] 7..8 = Vong quai / Sang vat roi (14/09 d: bo Vong do mac)', 1)
    ghi(p, s); KQ.append('sinh_uioptions2_haoquang.py: 2 ten')
else:
    KQ.append('sinh_uioptions2_haoquang.py: da sua')

p = os.path.join(GOC, 'android', 'anh_haoquang_vong.py')
b = open(p, 'rb').read().decode('utf-8')
if 'n3 = ghi_spr(nguoi, os.path.join(dich, "vongnguoi.spr"), 72, 36, 70)' in b:
    b = b.replace('    nguoi = [vong_nguoi(i, 16, 72) for i in range(16)]            # 144x72\n', '    # [HAOQUANG 14/09 d] vongnguoi.spr (vong do mac cua minh) DA BO theo chu; ham vong_nguoi giu lai de tham khao\n', 1)
    b = b.replace('        n3 = ghi_spr(nguoi, os.path.join(dich, "vongnguoi.spr"), 72, 36, 70)\n', '', 1)
    b = b.replace('        print("da ghi:", dich, "vongquai.spr %d B, vongboss.spr %d B, vongnguoi.spr %d B" % (n1, n2, n3))', '        print("da ghi:", dich, "vongquai.spr %d B, vongboss.spr %d B" % (n1, n2))', 1)
    # xem truoc: bo hang vong nguoi
    i = b.find('        for i, m in enumerate([(255, 217, 78), (230, 90, 255), (200, 240, 255), (90, 230, 90)]):')
    j = b.find('        nen.convert("RGB").save(XEM)')
    if i > 0 and j > i:
        b = b[:i] + b[j:]
    open(p, 'wb').write(b.encode('utf-8')); KQ.append('anh_haoquang_vong.py: khong sinh vongnguoi')
else:
    KQ.append('anh_haoquang_vong.py: da sua')

print('\n'.join(KQ))
