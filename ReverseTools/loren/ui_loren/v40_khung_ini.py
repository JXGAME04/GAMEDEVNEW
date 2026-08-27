# -*- coding: ascii -*-
"""v40_khung_ini.py - SINH 4 tep khung Ui\\Ui3\\khamnam\\{Khamnammain,Tinhluyen,Chetao,Lay}.ini
tu bo GOC VLTK da rut (update03) + doi chieu voi ma doc scheme UiCompoundItem.cpp.

Nguon goc  : D:\\GAMEDEVNEW\\ReverseTools\\pak_vltk\\ra_vltk\\khung_ini\\update03__*.ini
Ma doc that: D:\\GAMEDEVNEW\\Sources\\S3Client\\Ui\\UiCase\\UiCompoundItem.cpp (da doc tay 27/08):
  - KUiCompoundItem::LoadScheme(Khamnammain): Main CompoundBtn DistillBtn ForgeBtn
      EnchaseBtn AtlasBtn CloseBtn; cac pad doc RuleInfo keys
      Compound/UpCryolite/UpPropMine/Distill/Forge/Enchase (Buff 512/600).
  - KUiCompoundOne/Two/Three::LoadScheme(Tinhluyen): Main Ore1..3 CompoundBtn CancleBtn
      GuideList GuideList_Scroll(+_Btn: WndScrollBar.cpp:85 tu doc "<sec>_Btn")
      Box_0..2(Pos) TextColor(Font) Effect_0..2; ReturnInfo keys 1..16
      (One:1,2,3,4,5  Two:6,7,8,4,9  Three:10,11,12,13,14,15,4,16 - DA RE-KEY so voi goc 1..12!).
  - KUiForge::LoadScheme(Chetao): Main BigBox SmallBox ForgeBtn CancleBtn GuideList
      GuideList_Scroll(+_Btn) EquipPos CryolitePos TextColor(Font) EquipEffect;
      ReturnInfo keys 1..4.
  - KUiDistill+KUiEnchase::LoadScheme(Lay - DUNG CHUNG 1 TEP): Main BigBox SmallBox1
      SmallBox2 Consume1..8 DistillBtn (Enchase CUNG doc "DistillBtn": dong 3050!)
      CancleBtn GuideList GuideList_Scroll(+_Btn) EquipPos CryolitePos PropMinePos
      ConsumePos TextColor EquipEffect ConsumeEffect; ReturnInfo keys 1..21
      (Distill:1..11  Enchase:12..21).
  - Consume1..8: goc VLTK dung [ItemBox] luoi (KWndObjectMatrix: cell = W//HUnits x H//VUnits,
      goc o = Left+col*cellW, Top+row*cellH, thu tu hang truoc cot sau) -> TACH thanh 8 section.

Mac dinh: DIEN TAP - sinh 4 tep vao ra_khamnam\\ + in bang doi chieu, KHONG dung den client.
--ghi   : chep 4 ini sang E:\\...\\bin\\client\\Ui\\Ui3\\khamnam\\ (sao luu .truoc_uiloren neu
          tep dich da co va khac) + chep spr thieu tu spr_khung\\<uid>.spr sang duong dan GBK.

Doc/ghi latin-1 (giu nguyen byte GBK + TCVN3). Chuoi moi cho nguoi choi: TCVN3 qua
bangtxt.uni2tcvn, co cham vong tron uni->tcvn->uni va cam chu HOA co dau ngoai A^ E^ O^ A( D O+ U+.
"""
import io
import os
import sys
import shutil

SRC_DIR = r'D:\GAMEDEVNEW\ReverseTools\pak_vltk\ra_vltk\khung_ini'
OUT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'ra_khamnam')
CLIENT = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client'
DEST_DIR = os.path.join(CLIENT, 'Ui', 'Ui3', 'khamnam')
SPR_KHUNG = r'D:\GAMEDEVNEW\ReverseTools\pak_vltk\ra_vltk\spr_khung'
PAK_ID_DIR = r'D:\GAMEDEVNEW\ReverseTools\viemde'
HAU_TO_SAO_LUU = '.truoc_uiloren'

sys.path.insert(0, PAK_ID_DIR)
from pak_id import file_name_to_id  # noqa: E402
from bangtxt import uni2tcvn, tcvn2uni  # noqa: E402

# Ten tep nguon (ten that tren dia la chu Trung unicode)
F_MAIN = 'update03__' + '\u94f8\u9020\u88c5\u5907\u754c\u9762' + '.ini'   # giao dien chinh
F_COMP = 'update03__' + '\u94f8\u9020_\u77ff\u77f3\u5408\u6210' + '.ini'  # khoang thach hop thanh
F_LAY = 'update03__' + '\u94f8\u9020_\u77ff\u77f3\u63d0\u53d6' + '.ini'   # khoang thach trich lay
F_FORGE = 'update03__' + '\u94f8\u9020_\u88c5\u5907\u6253\u9020' + '.ini'  # trang bi che tao

CANH_BAO = []
LOI_TO = []


def bao_loi_to(msg):
    LOI_TO.append(msg)
    print('!!! LOI TO: %s' % msg)


def canh_bao(msg):
    CANH_BAO.append(msg)
    print('  ~ canh bao: %s' % msg)


def doc(p):
    with io.open(p, 'rb') as f:
        return f.read().decode('latin-1')


def eol_cua(text):
    return '\r\n' if '\r\n' in text else '\n'


CAM_HOA = set()  # chu HOA co dau duoc phep trong TCVN3
CHO_PHEP_HOA = set('\u0102\u00c2\u0110\u00ca\u00d4\u01a0\u01af')  # A( A^ D- E^ O^ O+ U+


def sang_tcvn(u):
    """uni2tcvn + kiem chu HOA cam + cham vong tron."""
    for ch in u:
        if ord(ch) > 127 and ch.isupper() and ch not in CHO_PHEP_HOA:
            bao_loi_to('chu HOA co dau khong ma hoa duoc TCVN3: %r trong %r' % (ch, u))
            return None
    t = uni2tcvn(u)
    try:
        t.encode('latin-1')
    except UnicodeEncodeError:
        bao_loi_to('uni2tcvn tra ve ky tu ngoai latin-1: %r' % u)
        return None
    if tcvn2uni(t) != u:
        bao_loi_to('cham vong tron uni->tcvn->uni THAT BAI: %r' % u)
        return None
    return t


# ------------------------------------------------------------------
# Bang ReturnInfo MOI theo dung khoa ma UiCompoundItem.cpp doc (da re-key).
# Moi gia tri <= 62 byte vi hau het cho doc bang char Buff[64].
# ------------------------------------------------------------------
RET_TINHLUYEN = [
    ('1', 'C\u1ea7n \u0111\u1eb7t nh\u1eabn v\u00e0o \u00f4 nguy\u00ean li\u1ec7u.',
     'o Nhan trong (KUiCompoundOne:770)'),
    ('2', 'C\u1ea7n \u0111\u1eb7t d\u00e2y chuy\u1ec1n ho\u1eb7c h\u1ed9 ph\u00f9 v\u00e0o \u00f4 nguy\u00ean li\u1ec7u.',
     'o Day chuyen trong (:790)'),
    ('3', 'C\u1ea7n \u0111\u1eb7t ng\u1ecdc b\u1ed9i ho\u1eb7c h\u01b0\u01a1ng nang v\u00e0o \u00f4 nguy\u00ean li\u1ec7u.',
     'o Ngoc boi trong (:810)'),
    ('4', 'B\u1ea1n kh\u00f4ng mang \u0111\u1ee7 ti\u1ec1n \u0111\u1ec3 ti\u1ebfn h\u00e0nh thao t\u00e1c n\u00e0y.',
     'thieu tien - ca 3 pad dung chung (:828 :1304 :1857)'),
    ('5', '\u0110ang ti\u1ebfn h\u00e0nh h\u1ee3p th\u00e0nh huy\u1ec1n tinh...',
     'bat dau hop thanh (:852)'),
    ('6', 'C\u1ea7n \u0111\u1eb7t \u0111\u1ee7 3 mi\u1ebfng huy\u1ec1n tinh v\u00e0o 3 \u00f4 nguy\u00ean li\u1ec7u.',
     'thieu huyen tinh (KUiCompoundTwo:1202/1225/1247)'),
    ('7', '\u0110\u1eb3ng c\u1ea5p 3 mi\u1ebfng huy\u1ec1n tinh ph\u1ea3i gi\u1ed1ng nhau.',
     'nDetail lech (:1268)'),
    ('8', 'Huy\u1ec1n tinh \u0111\u00e3 \u0111\u1ea1t c\u1ea5p t\u1ed1i \u0111a, kh\u00f4ng th\u1ec3 th\u0103ng c\u1ea5p.',
     'nDetail >= 13 (:1286)'),
    ('9', '\u0110ang ti\u1ebfn h\u00e0nh th\u0103ng c\u1ea5p huy\u1ec1n tinh...',
     'bat dau thang cap (:1328)'),
    ('10', 'C\u1ea7n \u0111\u1eb7t \u0111\u1ee7 3 kho\u00e1ng th\u1ea1ch thu\u1ed9c t\u00ednh v\u00e0o 3 \u00f4.',
     'thieu khoang thach (KUiCompoundThree:1684/1712/1740)'),
    ('11', 'Ba kho\u00e1ng th\u1ea1ch ph\u1ea3i c\u00f9ng ch\u1ee7ng lo\u1ea1i.',
     'nDetail lech (:1763)'),
    ('12', 'Ng\u0169 h\u00e0nh 3 kho\u00e1ng th\u1ea1ch ph\u1ea3i gi\u1ed1ng nhau.',
     'nSeries lech (:1783)'),
    ('13', '\u0110\u1eb3ng c\u1ea5p 3 kho\u00e1ng th\u1ea1ch ph\u1ea3i gi\u1ed1ng nhau.',
     'nLevel lech (:1802)'),
    ('14', 'Thu\u1ed9c t\u00ednh 3 kho\u00e1ng th\u1ea1ch ph\u1ea3i gi\u1ed1ng nhau.',
     'nOption lech (:1821)'),
    ('15', 'Kho\u00e1ng th\u1ea1ch \u0111\u00e3 \u0111\u1ea1t c\u1ea5p t\u1ed1i \u0111a, kh\u00f4ng th\u1ec3 th\u0103ng c\u1ea5p.',
     'nLevel >= 10 (:1839)'),
    ('16', '\u0110ang ti\u1ebfn h\u00e0nh th\u0103ng c\u1ea5p kho\u00e1ng th\u1ea1ch...',
     'bat dau thang cap (:1880)'),
]

RET_CHETAO = [
    ('1', 'C\u1ea7n \u0111\u1eb7t trang b\u1ecb xanh ho\u1eb7c tr\u1eafng v\u00e0o \u00f4 trang b\u1ecb.',
     'BigBox trong (KUiForge:2787)'),
    ('2', 'C\u1ea7n \u0111\u1eb7t huy\u1ec1n tinh kho\u00e1ng th\u1ea1ch v\u00e0o \u00f4 nguy\u00ean li\u1ec7u.',
     'SmallBox trong (:2807)'),
    ('3', 'B\u1ea1n kh\u00f4ng mang \u0111\u1ee7 ti\u1ec1n \u0111\u1ec3 ti\u1ebfn h\u00e0nh thao t\u00e1c n\u00e0y.',
     'thieu tien CASH_FORGE (:2825)'),
    ('4', '\u0110ang ti\u1ebfn h\u00e0nh ch\u1ebf t\u1ea1o trang b\u1ecb Huy\u1ec1n Tinh...',
     'bat dau che tao (:2849)'),
]

_THIEU_NL = 'C\u00f2n thi\u1ebfu nguy\u00ean li\u1ec7u, h\u00e3y \u0111\u1eb7t \u0111\u1ee7 nguy\u00ean li\u1ec7u r\u1ed3i th\u1eed l\u1ea1i.'
_THIEU_TIEN = 'B\u1ea1n kh\u00f4ng mang \u0111\u1ee7 ti\u1ec1n \u0111\u1ec3 ti\u1ebfn h\u00e0nh thao t\u00e1c n\u00e0y.'
_NGU_HANH = 'Ng\u0169 h\u00e0nh c\u1ee7a trang b\u1ecb v\u00e0 kho\u00e1ng th\u1ea1ch kh\u00f4ng \u0111\u1ed3ng nh\u1ea5t.'

RET_LAY = [
    ('1', 'C\u1ea7n \u0111\u1eb7t trang b\u1ecb m\u00e0u xanh v\u00e0o \u00f4 trang b\u1ecb.',
     'BigBox trong (KUiDistill:2229)'),
    ('2', 'C\u1ea7n \u0111\u1eb7t huy\u1ec1n tinh kho\u00e1ng th\u1ea1ch v\u00e0o \u00f4 huy\u1ec1n tinh.',
     'SmallBox1 trong (:2256)'),
    ('3', 'C\u1ea7n \u0111\u1eb7t nguy\u00ean th\u1ea1ch ho\u1eb7c kho\u00e1ng nguy\u00ean v\u00e0o \u00f4 kho\u00e1ng th\u1ea1ch.',
     'SmallBox2 trong (:2277)'),
    ('4', _THIEU_NL, 'thieu nguyen lieu o Consume (:2304)'),
    ('5', _THIEU_NL, 'thieu nguyen lieu o Consume (:2325)'),
    ('6', _THIEU_NL, 'thieu nguyen lieu o Consume (:2349)'),
    ('7', _THIEU_NL, 'thieu nguyen lieu o Consume (:2374)'),
    ('8', _THIEU_NL, 'thieu nguyen lieu o Consume (:2399)'),
    ('9', _NGU_HANH, 'nSeries lech (:2422)'),
    ('10', _THIEU_TIEN, 'thieu tien (:2440)'),
    ('11', '\u0110ang ti\u1ebfn h\u00e0nh l\u1ea5y thu\u1ed9c t\u00ednh kho\u00e1ng th\u1ea1ch...',
     'bat dau trich lay (:2464)'),
    ('12', 'C\u1ea7n \u0111\u1eb7t Trang b\u1ecb Huy\u1ec1n Tinh v\u00e0o \u00f4 trang b\u1ecb.',
     'BigBox trong (KUiEnchase:3194)'),
    ('13', 'C\u1ea7n \u0111\u1eb7t huy\u1ec1n tinh kho\u00e1ng th\u1ea1ch v\u00e0o \u00f4 huy\u1ec1n tinh.',
     'SmallBox1 trong (:3221)'),
    ('14', 'C\u1ea7n \u0111\u1eb7t kho\u00e1ng th\u1ea1ch thu\u1ed9c t\u00ednh v\u00e0o \u00f4 kho\u00e1ng th\u1ea1ch.',
     'SmallBox2 trong (:3242)'),
    ('15', _THIEU_NL, 'thieu nguyen lieu o Consume (:3290...)'),
    ('16', 'Nguy\u00ean li\u1ec7u kh\u00f4ng \u0111\u00fang ch\u1ee7ng lo\u1ea1i, h\u00e3y thay nguy\u00ean li\u1ec7u kh\u00e1c.',
     'nOption != 53 (:3269...)'),
    ('17', 'Nguy\u00ean li\u1ec7u \u0111\u1eb7t sai v\u1ecb tr\u00ed, h\u00e3y ki\u1ec3m tra l\u1ea1i c\u00e1c \u00f4.',
     'nOption o truoc da la 53 (:3324...)'),
    ('18', _NGU_HANH, 'nSeries lech (:3579)'),
    ('19', 'V\u1ecb tr\u00ed n\u00e0y kh\u00f4ng th\u1ec3 kh\u1ea3m thu\u1ed9c t\u00ednh kho\u00e1ng th\u1ea1ch n\u00e0y.',
     '!bCanSet (:3597)'),
    ('20', _THIEU_TIEN, 'thieu tien (:3614)'),
    ('21', '\u0110ang ti\u1ebfn h\u00e0nh kh\u1ea3m n\u1ea1m trang b\u1ecb...',
     'bat dau kham nam (:3638)'),
]


def khoi_returninfo(bang, eol):
    """Sinh block [ReturnInfo] moi (TCVN3), chu thich ASCII theo tung khoa."""
    out = [';Bang ReturnInfo theo DUNG khoa ma UiCompoundItem.cpp doc (JX1 da re-key so voi goc VLTK).',
           ';Sinh boi v40_khung_ini.py - sua thi sua o patcher, dung sua tay.',
           '[ReturnInfo]']
    for khoa, u, ghi_chu in bang:
        t = sang_tcvn(u)
        if t is None:
            return None
        if len(t) > 62:
            canh_bao('ReturnInfo khoa %s dai %d byte > 62 (char Buff[64] se cat)' % (khoa, len(t)))
        out.append(';%s = %s' % (khoa, ghi_chu))
        out.append('%s=%s' % (khoa, t))
    return eol.join(out) + eol


# ------------------------------------------------------------------
# Cong cu thao tac van ban ini theo dong
# ------------------------------------------------------------------

def tim_dong(lines, dong_can):
    return [i for i, ln in enumerate(lines) if ln.strip() == dong_can]


def het_block(lines, i_header):
    """Tra ve chi so dong DAU TIEN sau block (dong bat dau bang ';' hoac '[')."""
    j = i_header + 1
    while j < len(lines):
        s = lines[j].strip()
        if s.startswith(';') or s.startswith('['):
            return j
        j += 1
    return len(lines)


def phan_tich_ini(text):
    """Parser don gian: {section: {key: value}} - khoa lay theo lan XUAT HIEN DAU (giong KIniFile tim tu dau)."""
    ket = {}
    thu_tu = []
    sec = None
    for ln in text.split('\n'):
        s = ln.strip().rstrip('\r').strip()
        if not s or s.startswith(';'):
            continue
        if s.startswith('[') and s.endswith(']'):
            sec = s[1:-1]
            if sec not in ket:
                ket[sec] = {}
                thu_tu.append(sec)
            continue
        if sec is not None and '=' in s:
            k, v = s.split('=', 1)
            k = k.strip()
            if k not in ket[sec]:
                ket[sec][k] = v.strip()
    return ket, thu_tu


# ------------------------------------------------------------------
# Sinh tung tep
# ------------------------------------------------------------------

def sinh_khamnammain():
    text = doc(os.path.join(SRC_DIR, F_MAIN))
    # Giu nguyen byte: moi section ma LoadScheme doc deu da co trong tep goc.
    return text


def sinh_tinhluyen():
    text = doc(os.path.join(SRC_DIR, F_COMP))
    eol = eol_cua(text)
    lines = text.split(eol)
    vi_tri = tim_dong(lines, '[ReturnInfo]')
    if len(vi_tri) != 1:
        bao_loi_to('%s: mong 1 dong [ReturnInfo], thay %d' % (F_COMP, len(vi_tri)))
        return None
    i = vi_tri[0]
    if i > 0 and lines[i - 1].strip().startswith(';'):
        i -= 1  # nuot ca dong chu thich GBK cu
    khoi = khoi_returninfo(RET_TINHLUYEN, eol)
    if khoi is None:
        return None
    return eol.join(lines[:i]) + khoi


def sinh_chetao():
    text = doc(os.path.join(SRC_DIR, F_FORGE))
    eol = eol_cua(text)
    lines = text.split(eol)
    # 1) Bo [ForgeBtn] TRUNG LAP dau tien (Left=246, Tip chep nham tu trang hop thanh)
    vi_tri = tim_dong(lines, '[ForgeBtn]')
    if len(vi_tri) != 2:
        bao_loi_to('%s: mong 2 dong [ForgeBtn] (1 trung lap), thay %d' % (F_FORGE, len(vi_tri)))
        return None
    dau, cuoi = vi_tri[0], het_block(lines, vi_tri[0])
    block1 = eol.join(lines[dau:cuoi])
    if 'Left=246' not in block1:
        bao_loi_to('%s: block [ForgeBtn] dau tien khong chua Left=246 - nguon da doi, xem lai tay' % F_FORGE)
        return None
    lines = lines[:dau] + lines[cuoi:]
    # 2) Thay [ReturnInfo]
    vi_tri = tim_dong(lines, '[ReturnInfo]')
    if len(vi_tri) != 1:
        bao_loi_to('%s: mong 1 dong [ReturnInfo], thay %d' % (F_FORGE, len(vi_tri)))
        return None
    i = vi_tri[0]
    if i > 0 and lines[i - 1].strip().startswith(';'):
        i -= 1
    khoi = khoi_returninfo(RET_CHETAO, eol)
    if khoi is None:
        return None
    return eol.join(lines[:i]) + khoi


def sinh_lay():
    text = doc(os.path.join(SRC_DIR, F_LAY))
    eol = eol_cua(text)
    lines = text.split(eol)
    # 1) TACH [ItemBox] luoi -> Consume1..8 (theo KWndObjectMatrix::Init WndObjContainer.cpp:424)
    vi_tri = tim_dong(lines, '[ItemBox]')
    if len(vi_tri) != 1:
        bao_loi_to('%s: mong 1 dong [ItemBox], thay %d' % (F_LAY, len(vi_tri)))
        return None
    cfg, _ = phan_tich_ini(text)
    ib = cfg.get('ItemBox', {})
    try:
        left = int(ib['Left']); top = int(ib['Top'])
        w = int(ib['Width']); h = int(ib['Height'])
        hu = int(ib['HUnits']); vu = int(ib['VUnits'])
    except (KeyError, ValueError):
        bao_loi_to('%s: [ItemBox] thieu Left/Top/Width/Height/HUnits/VUnits' % F_LAY)
        return None
    if hu * vu != 8:
        bao_loi_to('%s: [ItemBox] HUnits*VUnits = %d != 8' % (F_LAY, hu * vu))
        return None
    cw = max(1, w // hu)
    ch = max(1, h // vu)
    khoi_cons = [';Consume1..8 tach tu [ItemBox] luoi %dx%d o %d,%d - cell %dx%d'
                 % (hu, vu, left, top, cw, ch),
                 ';(toa do theo KWndObjectMatrix: goc o = Left+col*cellW, Top+row*cellH)']
    for i in range(8):
        row, col = i // hu, i % hu
        khoi_cons += ['[Consume%d]' % (i + 1),
                      'Left=%d' % (left + col * cw),
                      'Top=%d' % (top + row * ch),
                      'Width=%d' % cw,
                      'Height=%d' % ch,
                      '']
    dau = vi_tri[0]
    cuoi = het_block(lines, dau)
    if dau > 0 and lines[dau - 1].strip().startswith(';'):
        dau -= 1
    lines = lines[:dau] + khoi_cons + lines[cuoi:]
    # 2) Thay [ReturnInfo] (Distill 1..11 + Enchase 12..21 dung chung tep nay)
    vi_tri = tim_dong(lines, '[ReturnInfo]')
    if len(vi_tri) != 1:
        bao_loi_to('%s: mong 1 dong [ReturnInfo], thay %d' % (F_LAY, len(vi_tri)))
        return None
    i = vi_tri[0]
    if i > 0 and lines[i - 1].strip().startswith(';'):
        i -= 1
    khoi = khoi_returninfo(RET_LAY, eol)
    if khoi is None:
        return None
    return eol.join(lines[:i]) + khoi


# ------------------------------------------------------------------
# Doi chieu voi yeu cau cua LoadScheme (doc tay tu UiCompoundItem.cpp)
# ------------------------------------------------------------------
YEU_CAU = {
    'Khamnammain.ini': {
        'sections': ['Main', 'CompoundBtn', 'DistillBtn', 'ForgeBtn', 'EnchaseBtn',
                     'AtlasBtn', 'CloseBtn', 'RuleInfo'],
        'keys': {'RuleInfo': ['Compound', 'UpCryolite', 'UpPropMine',
                              'Distill', 'Forge', 'Enchase']},
    },
    'Tinhluyen.ini': {
        'sections': ['Main', 'Ore1', 'Ore2', 'Ore3', 'CompoundBtn', 'CancleBtn',
                     'GuideList', 'GuideList_Scroll', 'GuideList_Scroll_Btn',
                     'Box_0', 'Box_1', 'Box_2', 'TextColor',
                     'Effect_0', 'Effect_1', 'Effect_2', 'ReturnInfo'],
        'keys': {'ReturnInfo': [str(i) for i in range(1, 17)],
                 'TextColor': ['Font'],
                 'Box_0': ['Pos'], 'Box_1': ['Pos'], 'Box_2': ['Pos']},
    },
    'Chetao.ini': {
        'sections': ['Main', 'BigBox', 'SmallBox', 'ForgeBtn', 'CancleBtn',
                     'GuideList', 'GuideList_Scroll', 'GuideList_Scroll_Btn',
                     'EquipPos', 'CryolitePos', 'TextColor', 'EquipEffect', 'ReturnInfo'],
        'keys': {'ReturnInfo': [str(i) for i in range(1, 5)],
                 'TextColor': ['Font'],
                 'EquipPos': ['Pos'], 'CryolitePos': ['Pos']},
    },
    'Lay.ini': {
        'sections': ['Main', 'BigBox', 'SmallBox1', 'SmallBox2'] +
                    ['Consume%d' % i for i in range(1, 9)] +
                    ['DistillBtn', 'CancleBtn', 'GuideList', 'GuideList_Scroll',
                     'GuideList_Scroll_Btn', 'EquipPos', 'CryolitePos', 'PropMinePos',
                     'ConsumePos', 'TextColor', 'EquipEffect', 'ConsumeEffect', 'ReturnInfo'],
        'keys': {'ReturnInfo': [str(i) for i in range(1, 22)],
                 'TextColor': ['Font'],
                 'EquipPos': ['Pos'], 'CryolitePos': ['Pos'],
                 'PropMinePos': ['Pos'], 'ConsumePos': ['Pos']},
    },
}


def doi_chieu(ten, text):
    cfg, thu_tu = phan_tich_ini(text)
    yc = YEU_CAU[ten]
    thieu = [s for s in yc['sections'] if s not in cfg]
    thua = [s for s in thu_tu if s not in yc['sections']]
    print('  section can co : %d - %s' % (len(yc['sections']),
          'DU HET' if not thieu else 'THIEU: %s' % ', '.join(thieu)))
    if thieu:
        bao_loi_to('%s thieu section: %s (cua so se trang!)' % (ten, ', '.join(thieu)))
    if thua:
        print('  section thua (khong lop nao doc, vo hai): %s' % ', '.join(thua))
    for sec, keys in sorted(yc['keys'].items()):
        if sec not in cfg:
            continue
        thieu_k = [k for k in keys if k not in cfg[sec]]
        if thieu_k:
            bao_loi_to('%s [%s] thieu khoa: %s' % (ten, sec, ', '.join(thieu_k)))
        else:
            print('  [%s] du %d/%d khoa' % (sec, len(keys), len(keys)))
    # Kiem tra bay <color>/<enter> bi TEncodeText nuot: run byte cao LE ngay truoc '<'
    for sec in ('RuleInfo', 'ReturnInfo'):
        for k, v in sorted(cfg.get(sec, {}).items()):
            for pos, ch in enumerate(v):
                if ch == '<':
                    run = 0
                    j = pos - 1
                    while j >= 0 and ord(v[j]) > 0x80:
                        run += 1
                        j -= 1
                    if run % 2 == 1:
                        canh_bao('%s [%s] %s: run %d byte cao LE ngay truoc "<" o offset %d '
                                 '- TEncodeText se nuot dau "<" (them 1 dau cach)' % (ten, sec, k, run, pos))
            if sec == 'RuleInfo' and len(v) > 511:
                canh_bao('%s [RuleInfo] %s dai %d byte > 511 (Buff[512] cua One/Two/Three/Distill/Forge se cat)'
                         % (ten, k, len(v)))
    return cfg


def kiem_spr(cac_cfg, ghi):
    """Gom moi Image= trong 4 tep sinh ra, kiem co tren dia chua; thieu thi doi chieu spr_khung."""
    print('\n=== KIEM SPR TREN DIA (%s) ===' % CLIENT)
    da_xet = set()
    for ten, cfg in cac_cfg.items():
        for sec, keys in cfg.items():
            duong = keys.get('Image', '')
            if not duong or duong in da_xet:
                continue
            da_xet.add(duong)
            rel = duong.lstrip('\\')
            dia = os.path.join(CLIENT, *rel.split('\\'))
            uid = '%08X' % file_name_to_id(duong)
            nguon = os.path.join(SPR_KHUNG, uid + '.spr')
            ten_in = duong.encode('latin-1').decode('latin-1')
            if os.path.isfile(dia):
                print('  CO SAN  %s  %s' % (uid, ascii(ten_in)))
            elif os.path.isfile(nguon):
                if ghi:
                    os.makedirs(os.path.dirname(dia), exist_ok=True)
                    shutil.copy2(nguon, dia)
                    print('  DA CHEP %s -> %s' % (uid, ascii(ten_in)))
                else:
                    print('  SE CHEP %s (co trong spr_khung) -> %s' % (uid, ascii(ten_in)))
            else:
                canh_bao('THIEU SPR ca dia lan spr_khung: uid %s duong %s '
                         '(anh se trang cho nay - can rut them tu pak)' % (uid, ascii(ten_in)))


def ghi_mot_tep(duong, text):
    moi = text.encode('latin-1')
    if os.path.isfile(duong):
        cu = io.open(duong, 'rb').read()
        if cu == moi:
            print('  da giong het, bo qua: %s' % duong)
            return
        sao_luu = duong + HAU_TO_SAO_LUU
        if not os.path.isfile(sao_luu):
            shutil.copy2(duong, sao_luu)
            print('  sao luu: %s' % sao_luu)
    with io.open(duong, 'wb') as f:
        f.write(moi)
    print('  GHI: %s (%d byte)' % (duong, len(moi)))


def main():
    ghi = '--ghi' in sys.argv[1:]
    print('=== v40_khung_ini - %s ===' % ('GHI THAT' if ghi else 'DIEN TAP (khong dung den client)'))

    for f in (F_MAIN, F_COMP, F_LAY, F_FORGE):
        p = os.path.join(SRC_DIR, f)
        if not os.path.isfile(p):
            bao_loi_to('khong thay tep nguon: %s' % ascii(p))
    if LOI_TO:
        return 1

    bo = {
        'Khamnammain.ini': sinh_khamnammain(),
        'Tinhluyen.ini': sinh_tinhluyen(),
        'Chetao.ini': sinh_chetao(),
        'Lay.ini': sinh_lay(),
    }
    if any(v is None for v in bo.values()) or LOI_TO:
        print('\n!!! CO LOI TO - KHONG GHI GI CA')
        return 1

    os.makedirs(OUT_DIR, exist_ok=True)
    cac_cfg = {}
    for ten, text in bo.items():
        print('\n=== %s ===' % ten)
        cac_cfg[ten] = doi_chieu(ten, text)
        ra = os.path.join(OUT_DIR, ten)
        with io.open(ra, 'wb') as f:
            f.write(text.encode('latin-1'))
        print('  sinh: %s (%d byte)' % (ra, len(text)))

    kiem_spr(cac_cfg, ghi)

    if ghi and not LOI_TO:
        print('\n=== CHEP SANG CLIENT: %s ===' % DEST_DIR)
        os.makedirs(DEST_DIR, exist_ok=True)
        for ten, text in bo.items():
            ghi_mot_tep(os.path.join(DEST_DIR, ten), text)
    elif ghi:
        print('\n!!! CO LOI TO - KHONG CHEP SANG CLIENT')
        return 1

    print('\n=== TONG KET: %d loi to, %d canh bao ===' % (len(LOI_TO), len(CANH_BAO)))
    if not ghi:
        print('(dien tap xong - muon ghi that chay lai voi --ghi)')
    return 1 if LOI_TO else 0


if __name__ == '__main__':
    sys.stdout.reconfigure(errors='backslashreplace')
    sys.exit(main())
