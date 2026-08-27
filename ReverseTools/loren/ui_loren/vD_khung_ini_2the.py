# -*- coding: ascii -*-
"""vD_khung_ini_2the.py - SINH 2 tep khung CON THIEU cua he lo ren 7 khung:

  A. Khamnam.ini  - the KHAM NAM   (goc VLTK: update03__<CHU TRUNG: dau tao_trang bi kham nam>.ini)
  B. Dopho.ini    - the DO PHO HK  (goc VLTK: update03__<CHU TRUNG: dau tao_hoang kim do pho>.ini)

Vi sao can:
  - The KHAM NAM hien dang MUON bo cuc cua the TRICH LAY: ca KUiDistill lan KUiEnchase
    deu doc SCHEME_INI_OUTIN = "khamnam/Lay.ini" (UiCompoundItem.cpp:32, :2092, :3042).
    Ban goc VLTK co tep RIENG cho the nay, lech 18 o so voi ban trich lay.
  - The DO PHO HOANG KIM chua co bo cuc nao ca. (Va cung CHUA co lop KUiAtlas -
    UiCompoundItem.h chi co m_AtlasPadBtn, moi dong m_AtlasPad deu bi chu thich:
    :220 :221 :413 :491 ... => Dopho.ini la DU LIEU CHO SAN cho dot sau.)

Nguon goc  : D:\\GAMEDEVNEW\\ReverseTools\\pak_vltk\\ra_vltk\\khung_ini\\update03__*.ini
Ma doc that: D:\\GAMEDEVNEW\\Sources\\S3Client\\Ui\\UiCase\\UiCompoundItem.cpp (chi DOC, 27/08).
  !! Tep .cpp nay DANG BI SUA SONG SONG (do 3769 -> 3811 dong ngay trong luc doc)
     nen moi so dong duoi day la ~UOC LUONG, hay tra theo TEN HAM/TEN CHUOI.
  - KUiEnchase::Initialize   (~:2986) doc Khamnammain.ini [RuleInfo] Enchase, Buff[600].
  - KUiEnchase::LoadScheme   (~:3066) doc Lay.ini: Main, CtrlEnchaseItemMap[0..10] =
      BigBox SmallBox1 SmallBox2 Consume1..8, "DistillBtn"(!), "CancleBtn",
      GuideList, GuideList_Scroll (+ "_Btn" do KWndScrollBar tu ghep),
      EquipPos, TextColor/Font, CryolitePos, PropMinePos, ConsumePos,
      EquipEffect, ConsumeEffect.
  - KUiEnchase::ProcessEnchase (~:3200..3720) doc [ReturnInfo] khoa 12..21, char Buff[64].
  - KUiCompoundItem::LoadScheme (~:232) doc "AtlasBtn" trong Khamnammain.ini (nut the do pho).

BA CHO BUOC PHAI LECH KHOI BAN GOC (ma C++ doi khac - xem bao cao):
  1. [ItemBox] luoi 2x4 -> TACH thanh Consume1..8 (y het v40 lam cho Lay.ini).
  2. Them section BI DANH [DistillBtn] = ban sao nguyen van [EnchaseBtn]:
     ma doc dung ten "DistillBtn" (m_Enchase.Init(&Ini,"DistillBtn"), ~:3078).
     GIU CA HAI ten => chay duoc voi ma HIEN TAI, va van chay neu sau nay sua ma
     thanh "EnchaseBtn". Day la cach it rui ro nhat.
  3. Them section BI DANH [EquipPos] = ban sao Pos cua [PurpleEquipPos]:
     ma doc Ini.GetInteger2("EquipPos","Pos",&nX,&nY) (~:3084), ma KIniFile::GetInteger2
     (KIniFile.cpp:989) KHONG dat gia tri mac dinh khi thieu khoa => nX,nY la RAC
     tren stack => nhan "Trang bi tim" bay ra toa do ngau nhien.
  4. [ReturnInfo] goc (12 khoa, la bang KET QUA MAY CHU: ket qua -1..10 -> khoa 1..12)
     doi ten thanh [ReturnInfoVLTK] (khong ma nao doc, giu nguyen 100% byte goc),
     va dat [ReturnInfo] MOI = khoa 12..21 dung theo ma JX1 (lay tu bang RET_LAY
     cua v40_khung_ini.py - CUNG MOT nguon chu, khong chep tay).
     Neu de nguyen 12 khoa goc: khoa 12 tra ve nham cau, khoa 13..21 RONG (im lang),
     va moi khoa goc deu > 63 byte nen char Buff[64] cat cut giua cau.

Mac dinh: DIEN TAP - sinh 2 tep vao ra_khamnam2\\ + in bang doi chieu, KHONG dung den client.
--ghi   : chep 2 ini sang E:\\...\\bin\\client\\Ui\\Ui3\\khamnam\\ (sao luu .truoc_uiloren2)
          + chep spr thieu tu spr_khung\\<uid>.spr sang duong dan GBK tuong ung.

Doc/ghi latin-1 (giu nguyen byte GBK + TCVN3). Chuoi moi cho nguoi choi: TCVN3 qua
bangtxt.uni2tcvn, co cham vong tron uni->tcvn->uni va cam chu HOA co dau ngoai
A( A^ D- E^ O^ O+ U+.
"""
import io
import os
import sys
import shutil

SRC_DIR = r'D:\GAMEDEVNEW\ReverseTools\pak_vltk\ra_vltk\khung_ini'
OUT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'ra_khamnam2')
CLIENT = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client'
DEST_DIR = os.path.join(CLIENT, 'Ui', 'Ui3', 'khamnam')
SPR_KHUNG = r'D:\GAMEDEVNEW\ReverseTools\pak_vltk\ra_vltk\spr_khung'
PAK_ID_DIR = r'D:\GAMEDEVNEW\ReverseTools\viemde'
HAU_TO_SAO_LUU = '.truoc_uiloren2'

sys.path.insert(0, PAK_ID_DIR)
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pak_id import file_name_to_id            # noqa: E402
from bangtxt import uni2tcvn, tcvn2uni        # noqa: E402
# Lay DUNG bang chu cua v40 (khoa 12..21 = phan KUiEnchase) - mot nguon chu duy nhat.
from v40_khung_ini import RET_LAY             # noqa: E402

# Ten tep nguon (ten that tren dia la chu Trung unicode)
F_EN = 'update03__' + '\u94f8\u9020_\u88c5\u5907\u9576\u5d4c' + '.ini'    # dau tao_trang bi kham nam
F_ATLAS = 'update03__' + '\u94f8\u9020_\u9ec4\u91d1\u56fe\u8c31' + '.ini'  # dau tao_hoang kim do pho
F_ATLAS_04 = 'update04__' + '\u94f8\u9020_\u9ec4\u91d1\u56fe\u8c31' + '.ini'  # ban nap SAU (doi chieu)

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


CHO_PHEP_HOA = set('\u0102\u00c2\u0110\u00ca\u00d4\u01a0\u01af')  # A( A^ D- E^ O^ O+ U+


def sang_tcvn(u):
    """uni2tcvn + kiem chu HOA cam + cham vong tron (y het v40)."""
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
# Cong cu thao tac van ban ini theo dong (y het v40)
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
    """{section: {key: value}} - khoa lay theo lan XUAT HIEN DAU
    (giong KIniFile::GetKeyValue duyet danh sach tu dau, KIniFile.cpp:888)."""
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


def lay_block_tho(lines, ten_sec):
    """Tra ve (dau, cuoi, cac dong noi dung) cua mot section, bo dong trong cuoi."""
    vi_tri = tim_dong(lines, '[%s]' % ten_sec)
    if len(vi_tri) != 1:
        return None
    dau = vi_tri[0]
    cuoi = het_block(lines, dau)
    noi_dung = lines[dau:cuoi]
    while noi_dung and not noi_dung[-1].strip():
        noi_dung.pop()
    return dau, cuoi, noi_dung


def tach_itembox(lines, cfg, ten_tep):
    """[ItemBox] luoi -> Consume1..8, theo KWndObjectMatrix::Init
    (WndObjContainer.cpp:434-437: unit = Width/HUnits, Height/VUnits)."""
    vi_tri = tim_dong(lines, '[ItemBox]')
    if len(vi_tri) != 1:
        bao_loi_to('%s: mong 1 dong [ItemBox], thay %d' % (ten_tep, len(vi_tri)))
        return None
    ib = cfg.get('ItemBox', {})
    try:
        left = int(ib['Left']); top = int(ib['Top'])
        w = int(ib['Width']); h = int(ib['Height'])
        hu = int(ib['HUnits']); vu = int(ib['VUnits'])
    except (KeyError, ValueError):
        bao_loi_to('%s: [ItemBox] thieu Left/Top/Width/Height/HUnits/VUnits' % ten_tep)
        return None
    if hu * vu != 8:
        bao_loi_to('%s: [ItemBox] HUnits*VUnits = %d != 8' % (ten_tep, hu * vu))
        return None
    cw = max(1, w // hu)
    ch = max(1, h // vu)
    khoi = [';Consume1..8 tach tu [ItemBox] luoi %dx%d o %d,%d - cell %dx%d'
            % (hu, vu, left, top, cw, ch),
            ';(toa do theo KWndObjectMatrix: goc o = Left+col*cellW, Top+row*cellH)']
    for i in range(8):
        row, col = i // hu, i % hu
        khoi += ['[Consume%d]' % (i + 1),
                 'Left=%d' % (left + col * cw),
                 'Top=%d' % (top + row * ch),
                 'Width=%d' % cw,
                 'Height=%d' % ch,
                 '']
    dau = vi_tri[0]
    cuoi = het_block(lines, dau)
    if dau > 0 and lines[dau - 1].strip().startswith(';'):
        dau -= 1
    return lines[:dau] + khoi + lines[cuoi:]


# ------------------------------------------------------------------
# A. Khamnam.ini
# ------------------------------------------------------------------
DAU_DE_EN = [
    ';==================================================================',
    ';Khamnam.ini - THE KHAM NAM (KUiEnchase). Sinh boi vD_khung_ini_2the.py',
    ';Goc VLTK: khung_ini\\update03__<dau tao_trang bi kham nam>.ini - CHEP NGUYEN VAN,',
    ';chi lech 4 cho vi ma C++ (UiCompoundItem.cpp) doi khac - xem chu thich tai cho.',
    ';SUA THI SUA O PATCHER, DUNG SUA TAY.',
    ';==================================================================',
]


def sinh_khamnam():
    text = doc(os.path.join(SRC_DIR, F_EN))
    eol = eol_cua(text)
    lines = text.split(eol)
    cfg, _ = phan_tich_ini(text)

    # (1) [ItemBox] -> Consume1..8
    lines = tach_itembox(lines, cfg, F_EN)
    if lines is None:
        return None

    # (2) Them bi danh [DistillBtn] = ban sao [EnchaseBtn] (ma doc ten "DistillBtn" :3050)
    kq = lay_block_tho(lines, 'EnchaseBtn')
    if kq is None:
        bao_loi_to('%s: khong thay dung 1 section [EnchaseBtn]' % F_EN)
        return None
    dau, cuoi, noi_dung = kq
    if 'DistillBtn' in cfg:
        bao_loi_to('%s: ban goc DA co [DistillBtn] - xem lai tay, dung chen bi danh' % F_EN)
        return None
    bi_danh = ['',
               ';[UILOREN-vD] BI DANH cua [EnchaseBtn] o tren, y het tung khoa.',
               ';KUiEnchase::LoadScheme (UiCompoundItem.cpp:3050) doc DUNG ten "DistillBtn"',
               ';chu khong phai "EnchaseBtn" -> thieu section nay thi nut Kham nam thanh',
               ';cua so 0x0 vo hinh (KWndWindow::Init WndWindow.cpp:254 van tra ve TRUE).',
               ';Giu CA HAI ten de chay duoc ca truoc lan sau khi sua ma.',
               '[DistillBtn]'] + noi_dung[1:] + ['']
    # chen ngay sau block [EnchaseBtn] (truoc dong ';' mo dau block ke tiep)
    lines = lines[:cuoi] + bi_danh + lines[cuoi:]

    # (3) Them bi danh [EquipPos] = Pos cua [PurpleEquipPos] (ma doc "EquipPos" :3056)
    if 'PurpleEquipPos' not in cfg:
        bao_loi_to('%s: ban goc khong co [PurpleEquipPos] - nguon da doi' % F_EN)
        return None
    if 'EquipPos' in cfg:
        bao_loi_to('%s: ban goc DA co [EquipPos] - xem lai tay' % F_EN)
        return None
    pos = cfg['PurpleEquipPos'].get('Pos')
    if not pos:
        bao_loi_to('%s: [PurpleEquipPos] thieu khoa Pos' % F_EN)
        return None
    kq = lay_block_tho(lines, 'PurpleEquipPos')
    if kq is None:
        bao_loi_to('%s: khong thay dung 1 section [PurpleEquipPos]' % F_EN)
        return None
    dau, cuoi, _ = kq
    bi_danh = ['',
               ';[UILOREN-vD] BI DANH cua [PurpleEquipPos] o tren.',
               ';KUiEnchase::LoadScheme (:3056) doc Ini.GetInteger2("EquipPos","Pos",..) va',
               ';KIniFile::GetInteger2 (KIniFile.cpp:989) KHONG dat mac dinh khi thieu khoa',
               ';-> nX,nY giu RAC tren stack -> nhan "Trang bi tim" bay ra toa do ngau nhien.',
               '[EquipPos]',
               'Pos=%s' % pos,
               '']
    lines = lines[:cuoi] + bi_danh + lines[cuoi:]

    # (4) [ReturnInfo] goc -> [ReturnInfoVLTK]; dat [ReturnInfo] moi khoa 12..21
    vi_tri = tim_dong(lines, '[ReturnInfo]')
    if len(vi_tri) != 1:
        bao_loi_to('%s: mong 1 dong [ReturnInfo], thay %d' % (F_EN, len(vi_tri)))
        return None
    i = vi_tri[0]
    lines[i] = '[ReturnInfoVLTK]'
    ghi_chu_goc = [
        ';[UILOREN-vD] Bang duoi day la [ReturnInfo] NGUYEN VAN cua ban goc VLTK:',
        ';no la bang KET QUA MAY CHU (foundry result -1..10 -> khoa 1..12), KHONG phai',
        ';bang thong bao kiem tra phia client ma JX1 dung. Doi ten thanh [ReturnInfoVLTK]',
        ';(khong ma nao doc) de giu nguyen 100% byte goc ma khong de len [ReturnInfo] that.',
    ]
    lines = lines[:i] + ghi_chu_goc + lines[i:]

    khoi = khoi_returninfo_enchase(eol)
    if khoi is None:
        return None
    return eol.join(DAU_DE_EN) + eol + eol.join(lines) + eol + eol + khoi


def khoi_returninfo_enchase(eol):
    """[ReturnInfo] khoa 12..21 - DUNG khoa ma KUiEnchase::ProcessEnchase doc."""
    out = [';[UILOREN-vD] [ReturnInfo] THAT: khoa 12..21 dung theo ma JX1',
           ';(KUiEnchase::ProcessEnchase, UiCompoundItem.cpp:3222..3680, char Buff[64]).',
           ';Chu lay tu bang RET_LAY cua v40_khung_ini.py - CUNG mot nguon voi Lay.ini.',
           '[ReturnInfo]']
    for khoa, u, ghi_chu in RET_LAY:
        if int(khoa) < 12:
            continue  # 1..11 la phan KUiDistill, khong thuoc the Kham nam
        t = sang_tcvn(u)
        if t is None:
            return None
        if len(t) > 63:
            canh_bao('Khamnam.ini [ReturnInfo] khoa %s dai %d byte > 63 '
                     '(char Buff[64] + strncpy se CAT, KStrBase.cpp:268)' % (khoa, len(t)))
        out.append(';%s = %s' % (khoa, ghi_chu))
        out.append('%s=%s' % (khoa, t))
    return eol.join(out) + eol


# ------------------------------------------------------------------
# B. Dopho.ini - CHEP NGUYEN VAN 100%, chua co ma nao doc
# ------------------------------------------------------------------
DAU_DE_ATLAS = [
    ';==================================================================',
    ';Dopho.ini - THE DO PHO HOANG KIM. Sinh boi vD_khung_ini_2the.py',
    ';Goc VLTK: khung_ini\\update03__<dau tao_hoang kim do pho>.ini - CHEP NGUYEN VAN 100%,',
    ';KHONG sua mot byte nao (ngoai khoi chu thich nay).',
    ';CHUA CO MA NAO DOC TEP NAY: UiCompoundItem.h khong co lop KUiAtlas, chi co',
    ';m_AtlasPadBtn; moi dong m_AtlasPad deu bi chu thich (UiCompoundItem.cpp:220,221,',
    ';413,428,443,458,473,491). Day la DU LIEU CHO SAN cho dot sau.',
    ';Luu y ban pak: update04__ (nap SAU update03__) chu thich het block [PreviewBtn].',
    ';Ban nay giu [PreviewBtn] SONG theo dung nguon chu game chi dinh (update03).',
    ';==================================================================',
]


def sinh_dopho():
    text = doc(os.path.join(SRC_DIR, F_ATLAS))
    eol = eol_cua(text)
    return eol.join(DAU_DE_ATLAS) + eol + text


# ------------------------------------------------------------------
# Doi chieu voi yeu cau cua ma C++ (doc tay tu UiCompoundItem.cpp)
# ------------------------------------------------------------------
YEU_CAU = {
    'Khamnam.ini': {
        'sections': ['Main', 'BigBox', 'SmallBox1', 'SmallBox2'] +
                    ['Consume%d' % i for i in range(1, 9)] +
                    ['DistillBtn', 'CancleBtn', 'GuideList', 'GuideList_Scroll',
                     'GuideList_Scroll_Btn', 'EquipPos', 'CryolitePos', 'PropMinePos',
                     'ConsumePos', 'TextColor', 'EquipEffect', 'ConsumeEffect', 'ReturnInfo'],
        'keys': {'ReturnInfo': [str(i) for i in range(12, 22)],
                 'TextColor': ['Font'],
                 'EquipPos': ['Pos'], 'CryolitePos': ['Pos'],
                 'PropMinePos': ['Pos'], 'ConsumePos': ['Pos']},
        # Ban goc VLTK KHONG co [CancleBtn] cho the nay -> nut Huy vo hinh 0x0.
        # Day la dung nhu ban goc, khong tu them nut ban goc khong co.
        'cho_phep_thieu': {'CancleBtn':
                           'ban goc VLTK khong co - nut Huy thanh cua so 0x0 vo hinh, '
                           'vo hai (dung nhu ban goc). Muon co nut Huy phai them tay.'},
    },
    'Dopho.ini': {
        'sections': [],   # chua co lop KUiAtlas -> khong co doi hoi nao tu ma
        'keys': {},
        'cho_phep_thieu': {},
    },
}


def doi_chieu(ten, text):
    cfg, thu_tu = phan_tich_ini(text)
    yc = YEU_CAU[ten]
    if not yc['sections']:
        print('  (chua co ma C++ nao doc tep nay - khong co bang doi chieu bat buoc)')
        print('  section co trong tep (%d): %s' % (len(thu_tu), ', '.join(thu_tu)))
    else:
        thieu = [s for s in yc['sections'] if s not in cfg]
        thua = [s for s in thu_tu if s not in yc['sections']]
        that_su_thieu = [s for s in thieu if s not in yc['cho_phep_thieu']]
        print('  section ma C++ doc: %d - %s' % (len(yc['sections']),
              'DU HET' if not thieu else 'thieu: %s' % ', '.join(thieu)))
        for s in thieu:
            if s in yc['cho_phep_thieu']:
                canh_bao('%s thieu [%s] CO CHU Y: %s' % (ten, s, yc['cho_phep_thieu'][s]))
        if that_su_thieu:
            bao_loi_to('%s thieu section: %s (cua so se trang!)' % (ten, ', '.join(that_su_thieu)))
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

    # Bay TEncodeText (Text.cpp:458): byte > 0x80 nuot LUON byte ke tiep.
    # Run le byte cao ngay truoc '<' => dau '<' bi an mat, the <enter>/<color> chet.
    for sec in sorted(cfg):
        for k, v in sorted(cfg[sec].items()):
            for pos, ch in enumerate(v):
                if ch != '<':
                    continue
                run = 0
                j = pos - 1
                while j >= 0 and ord(v[j]) > 0x80:
                    run += 1
                    j -= 1
                if run % 2 == 1:
                    canh_bao('%s [%s] %s: run %d byte cao LE ngay truoc "<" o offset %d '
                             '- TEncodeText se nuot dau "<" (them 1 dau cach)'
                             % (ten, sec, k, run, pos))
    # Do dai: cac cho doc bang char Buff[64] (ReturnInfo) - g_StrCpyLen dung
    # strncpy(dest,src,64) roi dest[63]=0 (KStrBase.cpp:268) => giu toi da 63 byte.
    co_ma_doc = bool(yc['sections'])
    for sec in ('ReturnInfo', 'ReturnInfoVLTK'):
        for k, v in sorted(cfg.get(sec, {}).items()):
            if len(v) <= 63:
                continue
            if co_ma_doc and sec == 'ReturnInfo':
                canh_bao('%s [ReturnInfo] %s dai %d byte > 63 - char Buff[64] SE CAT'
                         % (ten, k, len(v)))
            else:
                print('  (ghi chu, CHUA co ma doc) [%s] %s dai %d byte > 63 - neu sau nay '
                      'viet ma doc bang char Buff[64] thi se bi cat' % (sec, k, len(v)))
    # Tip cua nut: KWndButton doc vao m_szTip[256] roi TEncodeText (WndButton.cpp:120)
    for sec in sorted(cfg):
        v = cfg[sec].get('Tip')
        if v is not None and len(v) > 255:
            canh_bao('%s [%s] Tip dai %d byte > 255 (m_szTip[256] se cat)' % (ten, sec, len(v)))
    return cfg


def kiem_spr(cac_cfg, ghi):
    """Gom moi Image= trong 2 tep sinh ra, kiem co tren dia chua; thieu thi lay spr_khung."""
    print('\n=== KIEM SPR TREN DIA (%s) ===' % CLIENT)
    da_xet = set()
    for ten in sorted(cac_cfg):
        for sec, keys in cac_cfg[ten].items():
            duong = keys.get('Image', '')
            if not duong or duong in da_xet:
                continue
            da_xet.add(duong)
            rel = duong.lstrip('\\')
            dia = os.path.join(CLIENT, *rel.split('\\'))
            uid = '%08X' % file_name_to_id(duong)
            nguon = os.path.join(SPR_KHUNG, uid + '.spr')
            if os.path.isfile(dia):
                print('  CO SAN  %s  %s' % (uid, ascii(duong)))
            elif os.path.isfile(nguon):
                if ghi:
                    os.makedirs(os.path.dirname(dia), exist_ok=True)
                    shutil.copy2(nguon, dia)
                    print('  DA CHEP %s (%d byte) -> %s'
                          % (uid, os.path.getsize(nguon), ascii(duong)))
                else:
                    print('  SE CHEP %s (%d byte, co trong spr_khung) -> %s'
                          % (uid, os.path.getsize(nguon), ascii(duong)))
            else:
                canh_bao('THIEU SPR ca dia lan spr_khung: uid %s duong %s '
                         '(anh se trang cho nay - can rut them tu pak)' % (uid, ascii(duong)))


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


def doi_chieu_ban_pak():
    """Bao ro khac biet giua update03 va update04 cua tep do pho (update04 nap SAU)."""
    p3 = os.path.join(SRC_DIR, F_ATLAS)
    p4 = os.path.join(SRC_DIR, F_ATLAS_04)
    if not os.path.isfile(p4):
        return
    c3, t3 = phan_tich_ini(doc(p3))
    c4, t4 = phan_tich_ini(doc(p4))
    chi_03 = [s for s in t3 if s not in c4]
    chi_04 = [s for s in t4 if s not in c3]
    print('\n=== DOI CHIEU BAN PAK do pho: update03 (dang dung) vs update04 (nap SAU) ===')
    print('  chi co trong update03: %s' % (', '.join(chi_03) or '(khong)'))
    print('  chi co trong update04: %s' % (', '.join(chi_04) or '(khong)'))
    if chi_03:
        canh_bao('ban update04 (nap sau, tuc ban NGUOI CHOI THAY) da chu thich het: %s. '
                 'Dang sinh theo update03 dung nhu chu game chi dinh - can chu game chot lai.'
                 % ', '.join(chi_03))


def main():
    ghi = '--ghi' in sys.argv[1:]
    print('=== vD_khung_ini_2the - %s ==='
          % ('GHI THAT' if ghi else 'DIEN TAP (khong dung den client)'))

    for f in (F_EN, F_ATLAS):
        p = os.path.join(SRC_DIR, f)
        if not os.path.isfile(p):
            bao_loi_to('khong thay tep nguon: %s' % ascii(p))
    if LOI_TO:
        return 1

    bo = {
        'Khamnam.ini': sinh_khamnam(),
        'Dopho.ini': sinh_dopho(),
    }
    if any(v is None for v in bo.values()) or LOI_TO:
        print('\n!!! CO LOI TO - KHONG GHI GI CA')
        return 1

    os.makedirs(OUT_DIR, exist_ok=True)
    cac_cfg = {}
    for ten in sorted(bo):
        text = bo[ten]
        print('\n=== %s ===' % ten)
        cac_cfg[ten] = doi_chieu(ten, text)
        ra = os.path.join(OUT_DIR, ten)
        with io.open(ra, 'wb') as f:
            f.write(text.encode('latin-1'))
        print('  sinh: %s (%d byte)' % (ra, len(text)))

    kiem_spr(cac_cfg, ghi)
    doi_chieu_ban_pak()

    if ghi and not LOI_TO:
        print('\n=== CHEP SANG CLIENT: %s ===' % DEST_DIR)
        os.makedirs(DEST_DIR, exist_ok=True)
        for ten in sorted(bo):
            ghi_mot_tep(os.path.join(DEST_DIR, ten), bo[ten])
    elif ghi:
        print('\n!!! CO LOI TO - KHONG CHEP SANG CLIENT')
        return 1

    print('\n=== TONG KET: %d loi to, %d canh bao ===' % (len(LOI_TO), len(CANH_BAO)))
    for c in CANH_BAO:
        print('  ~ %s' % c)
    if not ghi:
        print('(dien tap xong - muon ghi that chay lai voi --ghi)')
    return 1 if LOI_TO else 0


if __name__ == '__main__':
    sys.stdout.reconfigure(errors='backslashreplace')
    sys.exit(main())
