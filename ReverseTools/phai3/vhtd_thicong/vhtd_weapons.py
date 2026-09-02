# -*- coding: ascii -*-
r"""vhtd_weapons.py [VHTD 02/09] - VU HON (phai 11, vu khi THUAN) / TIEU DAO (phai 12, vu khi CAM):
noi DU LIEU VU KHI tu client VLTK Level Up (D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw) vao cay chay that
E:\...\bin\server\settings + bin\client\settings. KHONG dong den ma C++.

  python vhtd_weapons.py --kiem        DRY-RUN: in moi hang se THEM / SUA, KHONG ghi gi
  python vhtd_weapons.py --ghi-that    ghi that (chi bat SAU KHI chu duyet; thieu co -> SystemExit("CHUA DUYET"))
  tuy chon:  --skill-thuan=53   PhysicsSkillID danh thuong cho particular 7/8 (mac dinh 53 = "Cong kich vat ly", giong don dao P1)
             --skill-cam=2135   PhysicsSkillID danh thuong cho cam (mac dinh 2135 "Cong kich vat ly gan", EqtLimit 103, WeaponSkill 1)
             --obj-cam=N        doi cot 5 (objdata vat roi) cua 10 hang Moc Cam; VLTK ghi 481 = "Kim Bao Thach" (sai?),
                                484 = "Co Cam" (obj_wq_011.spr). Mac dinh GIU 481 nhu VLTK va chi canh bao.

Ma loai vu khi (da do): meleeweapon.txt VLTK cot 3 ParticularType = 0..8 (10 hang/loai): 6 Trien Thu, 7 Tinh Che Dao Thuan,
8 Tinh Che Thuan Dao. rangeweapon.txt VLTK: 0..3, 3 = Moc Cam (DetailType 1). skills.txt EqtLimit 7/8 (thuan), 103 = 100+3 (cam,
KSkills.cpp:270 cong MAX_MELEEWEAPON_PARTICULARTYPE_NUM=100 cho vu khi tam xa). Ma 11/12 CHI la nValue[2] cua addphysicsdamage_p
trong wuhuntang.lua (11 = Dao Phap/P7, 12 = Thuan Phap/P8) va yeu cau mon phai (magic_requiremenpai 39) trong hang item.

Chi so engine (phai giu dung vi tri hang):
  KItemGenerator.CPP:302/478/1415  record i = ParticularType*10 + Level - 1  -> P6 = 60..69, P7 = 70..79, P8 = 80..89 (melee, JX1 co 60)
                                                                            -> P3 = 30..39 (range, JX1 co 30)
  KItemChangeRes.cpp:86-93 GetWeaponRes: melee row = P*10+L+2 -> MeleeRes id = P*10+L+1 (72..81 -> 270, 82..91 -> 271, 62..71 -> 2)
                                          range row = P*10+L+1 = 32..41 -> RangeRes DATA-ROW 30..39 (JX1 id 31..40; VLTK id 32..41 vi
                                          JX1 co hang thua "1|2|<ten>" o dau) -> 278. Gia tri tra ve = phan - 2 = chi so hang bang NpcRes.
  KObjSet.cpp:340/356 ObjData: doc row nDataID+1, kiem nDataID < GetHeight -> DataID phai = chi so hang (noi 463..484 lien tuc).
  KCore.cpp:420-445 ClientWeaponSkill.txt: g_nMeleeWeaponSkill[P] / g_nRangeWeaponSkill[P] (mang 100 o).
Doc/ghi latin-1, giu CRLF, ban luu <tep>.truoc_vhtd_0209 (tao 1 lan), idempotent (chay lai -> "da co", khong ghi).
"""
import io, os, sys, shutil, collections

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")

RAW = r"D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw"
BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
SRV = os.path.join(BIN, "server", "settings")
CLI = os.path.join(BIN, "client", "settings")
BAK = ".truoc_vhtd_0209"
TAB = "\t"

ARGS = sys.argv[1:]
KIEM = "--kiem" in ARGS
GHI_THAT = "--ghi-that" in ARGS


def opt(name, default):
    for a in ARGS:
        if a.startswith(name + "="):
            return a.split("=", 1)[1]
    return default


SKILL_THUAN = int(opt("--skill-thuan", "53"))
SKILL_CAM = int(opt("--skill-cam", "2135"))
OBJ_CAM = opt("--obj-cam", "")          # "" = giu nguyen VLTK

if not KIEM and not GHI_THAT:
    raise SystemExit("CHUA DUYET: chay 'python vhtd_weapons.py --kiem' de xem ke hoach; --ghi-that chi bat sau khi chu duyet.")
if KIEM and GHI_THAT:
    raise SystemExit("Chon MOT trong hai: --kiem hoac --ghi-that")

# ---------------------------------------------------------------- TCVN3 -> unicode (chi de IN ra man hinh, khong ghi vao tep)
_TCVN = {u'\u0102': 0xA1, u'\u00C2': 0xA2, u'\u00CA': 0xA3, u'\u00D4': 0xA4, u'\u01A0': 0xA5, u'\u01AF': 0xA6, u'\u0110': 0xA7,
         u'\u0103': 0xA8, u'\u00E2': 0xA9, u'\u00EA': 0xAA, u'\u00F4': 0xAB, u'\u01A1': 0xAC, u'\u01B0': 0xAD, u'\u0111': 0xAE,
         u'\u00E0': 0xB0, u'\u1EA3': 0xB1, u'\u00E3': 0xB2, u'\u00E1': 0xB3, u'\u1EA1': 0xB4, u'\u1EB1': 0xB5, u'\u1EB3': 0xB6,
         u'\u1EB5': 0xB7, u'\u1EAF': 0xB8, u'\u1EB7': 0xB9, u'\u1EA7': 0xBA, u'\u1EA9': 0xBB, u'\u1EAB': 0xBC, u'\u1EA5': 0xBD,
         u'\u1EAD': 0xBE, u'\u00E8': 0xBF, u'\u1EBB': 0xC0, u'\u1EBD': 0xC1, u'\u00E9': 0xC2, u'\u1EB9': 0xC3, u'\u1EC1': 0xC4,
         u'\u1EC3': 0xC5, u'\u1EC5': 0xC6, u'\u1EBF': 0xC7, u'\u1EC7': 0xC8, u'\u00EC': 0xC9, u'\u1EC9': 0xCA, u'\u0129': 0xCB,
         u'\u00ED': 0xCC, u'\u1ECB': 0xCD, u'\u00F2': 0xCE, u'\u1ECF': 0xCF, u'\u00F5': 0xD0, u'\u00F3': 0xD1, u'\u1ECD': 0xD2,
         u'\u1ED3': 0xD3, u'\u1ED5': 0xD4, u'\u1ED7': 0xD5, u'\u1ED1': 0xD6, u'\u1ED9': 0xD7, u'\u1EDD': 0xD8, u'\u1EDF': 0xD9,
         u'\u1EE1': 0xDA, u'\u1EDB': 0xDB, u'\u1EE3': 0xDC, u'\u00F9': 0xDD, u'\u1EE7': 0xDE, u'\u0169': 0xDF, u'\u00FA': 0xE0,
         u'\u1EE5': 0xE1, u'\u1EEB': 0xE2, u'\u1EED': 0xE3, u'\u1EEF': 0xE4, u'\u1EE9': 0xE5, u'\u1EF1': 0xE6, u'\u1EF3': 0xE7,
         u'\u1EF7': 0xE8, u'\u1EF9': 0xE9, u'\u00FD': 0xEA, u'\u1EF5': 0xEB}
_INV = dict((chr(v), k) for k, v in _TCVN.items())


def dec(s):
    return "".join(_INV.get(ch, ch) for ch in s).strip()


# ---------------------------------------------------------------- doc / ghi tep bang (latin-1, giu CRLF)
class Tep(object):
    def __init__(self, path):
        self.path = path
        self.text = io.open(path, "r", encoding="latin-1", newline="").read()
        self.nl = "\r\n" if "\r\n" in self.text else "\n"
        parts = self.text.split(self.nl)
        self.trailing = (parts[-1] == "")          # tep ket thuc bang xuong dong
        if self.trailing:
            parts = parts[:-1]
        self.lines = parts
        self.orig_lines = list(parts)
        self.thaydoi = []                          # mo ta tung thay doi

    @property
    def header(self):
        return self.lines[0].split(TAB)

    def rows(self):
        return [ln.split(TAB) for ln in self.lines[1:]]

    def data_rows(self):
        return [ln.split(TAB) for ln in self.lines[1:] if ln.strip()]

    def append_row(self, cols, mota):
        # bo cac dong trang cuoi tep truoc khi noi (JX1 hay co 1 dong trang cuoi)
        while self.lines and not self.lines[-1].strip():
            self.lines.pop()
        self.lines.append(TAB.join(cols))
        self.thaydoi.append("THEM " + mota)

    def set_row(self, idx_line, cols, mota):
        self.lines[idx_line] = TAB.join(cols)
        self.thaydoi.append("SUA  " + mota)

    def changed(self):
        return self.lines != self.orig_lines

    def render(self):
        s = self.nl.join(self.lines)
        return s + self.nl if self.trailing else s

    def ghi(self):
        if not self.changed():
            return False
        if not os.path.exists(self.path + BAK):
            shutil.copy2(self.path, self.path + BAK)
        io.open(self.path, "w", encoding="latin-1", newline="").write(self.render())
        return True


def loi(msg):
    raise SystemExit("LOI: " + msg)


def hex_(s):
    return s.encode("latin-1").hex()


def bang(title):
    print("")
    print("=" * 100)
    print(title)
    print("=" * 100)


def same_bytes(p1, p2):
    return open(p1, "rb").read() == open(p2, "rb").read()


# ---------------------------------------------------------------- VLTK nguon
V_MELEE = os.path.join(RAW, "settings__item__004__meleeweapon.txt")
V_RANGE = os.path.join(RAW, "settings__item__004__rangeweapon.txt")
V_MRES = os.path.join(RAW, "settings__item__meleeres.txt")                   # slistcache.pak: 91 hang (62..91 thuan/Trien Thu)
V_MRES2 = os.path.join(RAW, "settings__item__004__meleeres__update01.txt")    # update01.pak: ban cu 61 hang (phai la tap con)
V_RRES = os.path.join(RAW, "settings__item__004__rangeres.txt")
V_RRES2 = os.path.join(RAW, "settings__item__rangeres.txt")
V_OBJ = os.path.join(RAW, "settings__obj__objdata.txt")

MELEE_NEW_P = ["6", "7", "8"]     # P6 Trien Thu PHAI di kem de P7/P8 nam dung chi so 70..89 (record i = P*10+L-1)
RANGE_NEW_P = ["3"]
MELEE_RES_IDS = list(range(62, 92))   # ids 62..91 trong meleeres VLTK (62-71 -> 2, 72-81 -> 270, 82-91 -> 271)
RANGE_RES_ROWS = list(range(30, 40))  # DATA-ROW (0-based) 30..39 cua RangeRes JX1 (= dong tep 31..40, engine row 32..41 = P3 L1..10,
                                      # id JX1 31..40 vi hang dau "1|2|<ten>" la hang thua; VLTK id 32..41) -> 278
RANGE_RES_PART = "278"
OBJ_FIRST_NEW = 463
CWS_HDR = ["DetailType", "ParticularType", "PhysicsSkillID"]
TEN_P = {"6": "TrienThu", "7": "DaoThuan", "8": "ThuanDao", "3": "MocCam"}

TONG = collections.OrderedDict()


def kiem_header_weapon(jh, vh, ten):
    if len(jh) != len(vh):
        loi("%s: so cot header khac (JX1 %d, VLTK %d)" % (ten, len(jh), len(vh)))
    khac = [i for i in range(len(jh)) if jh[i] != vh[i]]
    # VLTK doi ten 3 cot 1..3 sang ItemGenre/DetailType/ParticularType; engine doc theo CHI SO cot (KBasPropTbl.cpp:84-105
    # ::LoadRecord GetInteger(nRow, n+1)) nen khac ten o 3 cot nay chap nhan duoc.
    la = [i for i in khac if i not in (1, 2, 3)]
    if la:
        loi("%s: header khac o cot %s (JX1=%s VLTK=%s)" % (ten, la, [hex_(jh[i]) for i in la], [vh[i] for i in la]))
    return khac


def kiem_thu_tu(rows, ten, so_p):
    """moi hang i phai co P*10+L-1 == i (P cot 3, L cot 11)"""
    if len(rows) != so_p * 10:
        loi("%s: JX1 co %d hang, mong %d (P0..P%d x 10 muc)" % (ten, len(rows), so_p * 10, so_p - 1))
    for i, c in enumerate(rows):
        if int(c[3]) * 10 + int(c[11]) - 1 != i:
            loi("%s: hang %d co P=%s L=%s khong khop chi so (record i = P*10+L-1)" % (ten, i, c[3], c[11]))


def buoc_weapon(tree, ten_tep, v_path, new_p, so_p_cu, detail_mong, obj_override=""):
    p = os.path.join(tree, "item", ten_tep)
    J = Tep(p)
    V = Tep(v_path)
    jh, vh = J.header, V.header
    khac = kiem_header_weapon(jh, vh, ten_tep)
    jrows = J.data_rows()
    kiem_thu_tu(jrows, ten_tep, so_p_cu)
    vrows = [c for c in V.data_rows() if c[3] in new_p]
    if len(vrows) != 10 * len(new_p):
        loi("%s VLTK: tim thay %d hang P%s, mong %d" % (ten_tep, len(vrows), new_p, 10 * len(new_p)))
    co = set((c[3], c[11]) for c in jrows)
    print("  %s: %d hang JX1 (P0..P%d), header khac ten o cot %s (chap nhan, engine doc theo chi so)" % (p, len(jrows), so_p_cu - 1, khac))
    them = 0
    for c in vrows:
        if len(c) != len(vh):
            loi("%s VLTK hang P%s L%s co %d cot" % (ten_tep, c[3], c[11], len(c)))
        if c[1] != "0" or c[2] != detail_mong:
            loi("%s VLTK hang P%s L%s: ItemGenre=%s DetailType=%s (mong 0/%s)" % (ten_tep, c[3], c[11], c[1], c[2], detail_mong))
        c = list(c)
        ghi_chu = ""
        if obj_override and c[3] == "3":
            ghi_chu = " [obj %s -> %s theo --obj-cam]" % (c[5], obj_override)
            c[5] = obj_override
        key = (c[3], c[11])
        i_moi = int(c[3]) * 10 + int(c[11]) - 1
        mota = "%s record i=%d P=%s L=%s '%s' spr=%s obj=%s %sx%s gia=%s | co ban: %s | yeu cau: %s%s" % (
            ten_tep, i_moi, c[3], c[11], dec(c[0]), c[4], c[5], c[6], c[7], c[10],
            " ".join(c[13:34]).strip(), " ".join(c[34:]).strip(), ghi_chu)
        if key in co:
            print("    [=] da co P%s L%s" % key)
            continue
        if i_moi != len(jrows) + them:
            loi("%s: hang moi P%s L%s se roi vao chi so %d thay vi %d" % (ten_tep, c[3], c[11], len(jrows) + them, i_moi))
        J.append_row(c, mota)
        print("    [+] " + mota)
        them += 1
    TONG[p] = (them, J)
    return J


def buoc_meleeres(tree):
    p = os.path.join(tree, "item", "MeleeRes.txt")
    J = Tep(p)
    V = Tep(V_MRES)
    V2 = Tep(V_MRES2)
    vmap = dict((int(c[0]), c[1]) for c in V.data_rows())
    v2map = dict((int(c[0]), c[1]) for c in V2.data_rows())
    if max(vmap) < 91:
        loi("meleeres VLTK (%s) chi toi id %d, mong 91" % (V_MRES, max(vmap)))
    lech = [i for i in v2map if vmap.get(i) != v2map[i]]
    if lech:
        loi("ban meleeres update01 (cu) khac ban slistcache o id %s" % lech[:10])
    if J.header[:2] != V.header[:2]:
        loi("MeleeRes header khac: JX1 %s VLTK %s" % ([hex_(x) for x in J.header[:2]], [hex_(x) for x in V.header[:2]]))
    jrows = J.rows()
    for k, c in enumerate(jrows):
        if not c[0].strip():
            continue
        if int(c[0]) != k + 1:
            loi("MeleeRes JX1: dong du lieu %d co id %s (mong id = chi so + 1)" % (k, c[0]))
    lech = [i for i in range(1, 62) if jrows[i - 1][1] != vmap.get(i)]
    print("  %s: %d hang, id lien tuc 1..%d; id 1..61 (P0..P5) %s VLTK" % (
        p, len(jrows), len(jrows), "TRUNG" if not lech else "KHAC o id %s" % lech[:10]))
    sua = 0
    for id_ in MELEE_RES_IDS:
        if id_ not in vmap:
            loi("VLTK meleeres thieu id %d" % id_)
        part = vmap[id_]
        P, L = (id_ - 2) // 10, (id_ - 2) % 10 + 1      # id = P*10+L+1
        line_idx = id_                                     # dong 0 = header -> id k o dong k
        c = J.lines[line_idx].split(TAB)
        if int(c[0]) != id_:
            loi("MeleeRes: dong %d id %s != %d" % (line_idx, c[0], id_))
        while len(c) < 3:
            c.append("")
        ten = "VHTD P%d %s L%d (VLTK meleeres id %d)" % (P, TEN_P[str(P)], L, id_)
        mota = "MeleeRes id %d (engine row %d = P%d L%d): phan %s -> %s (m_WeaponType %d) ten cu '%s'" % (
            id_, id_ + 1, P, L, c[1], part, int(part) - 2, hex_(c[2]) if any(ord(ch) > 127 for ch in c[2]) else c[2])
        if c[1] == part and c[2] == ten:
            print("    [=] MeleeRes id %d da = %s" % (id_, part))
            continue
        J.set_row(line_idx, [c[0], part, ten], mota)
        print("    [~] " + mota)
        sua += 1
    TONG[p] = (sua, J)
    return J


def buoc_rangeres(tree):
    p = os.path.join(tree, "item", "RangeRes.txt")
    J = Tep(p)
    if not same_bytes(V_RRES, V_RRES2):
        loi("hai ban rangeres VLTK khac nhau")
    V = Tep(V_RRES)
    if J.header[:2] != V.header[:2]:
        loi("RangeRes header khac")
    vrows = V.data_rows()
    # VLTK: dong du lieu k (0-based) co id k+2 (bat dau id 2); 10 hang cuoi (id 32..41) -> 278
    v_new = [c for c in vrows if int(c[0]) >= 32]
    if len(v_new) != 10 or set(c[1] for c in v_new) != set([RANGE_RES_PART]):
        loi("VLTK rangeres: mong 10 hang id 32..41 -> 278, thay %s" % [(c[0], c[1]) for c in v_new])
    jrows = J.rows()
    for k, c in enumerate(jrows):
        if c[0].strip() and int(c[0]) != k + 1:
            loi("RangeRes JX1: dong du lieu %d co id %s" % (k, c[0]))
    print("  %s: %d hang; hang dau '%s' la hang THUA (VLTK bat dau id 2) -> id JX1 = id VLTK - 1; engine doc theo HANG" % (
        p, len(jrows), " | ".join(jrows[0][:2])))
    sua = 0
    for k in RANGE_RES_ROWS:
        line_idx = k + 1                # J.lines[0] = header = engine row 1 -> J.lines[k+1] = engine row k+2
        row_engine = k + 2              # = 3*10 + L + 1 (KItemChangeRes.cpp:86,93: nRow-1 voi nRow = P*10+L+2)
        L = row_engine - 31
        c = J.lines[line_idx].split(TAB)
        while len(c) < 3:
            c.append("")
        if int(c[0]) != k + 1:
            loi("RangeRes: dong du lieu %d co id %s (mong %d)" % (k, c[0], k + 1))
        ten = "VHTD P3 MocCam L%d (VLTK rangeres id %d)" % (L, row_engine)
        mota = "RangeRes dong du lieu %d (id JX1 %s, engine row %d = P3 L%d, VLTK id %d): phan %s -> %s (m_WeaponType %d)" % (
            k, c[0], row_engine, L, row_engine, c[1], RANGE_RES_PART, int(RANGE_RES_PART) - 2)
        if c[1] == RANGE_RES_PART and c[2] == ten:
            print("    [=] RangeRes dong %d da = %s" % (k, RANGE_RES_PART))
            continue
        if c[1] != "21":
            print("    [!] RangeRes dong %d phan hien tai %s (mong 21) - van sua" % (k, c[1]))
        J.set_row(line_idx, [c[0], RANGE_RES_PART, ten], mota)
        print("    [~] " + mota)
        sua += 1
    TONG[p] = (sua, J)
    return J


def buoc_objdata(tree):
    p = os.path.join(tree, "obj", "ObjData.txt")
    J = Tep(p)
    V = Tep(V_OBJ)
    if J.header != V.header:
        loi("ObjData header khac")
    jrows, vrows = J.data_rows(), V.data_rows()
    for k, c in enumerate(vrows):
        if int(c[1]) != k + 1 or len(c) != len(V.header):
            loi("ObjData VLTK: dong %d DataID %s cot %d" % (k, c[1], len(c)))
    for k, c in enumerate(jrows):
        if int(c[1]) != k + 1:
            loi("ObjData JX1: dong %d DataID %s (mong lien tuc)" % (k, c[1]))
    n = len(jrows)
    print("  %s: %d hang (DataID 1..%d); VLTK %d hang" % (p, n, n, len(vrows)))
    if n < OBJ_FIRST_NEW - 1:
        loi("ObjData JX1 chi co %d hang, mong >= %d" % (n, OBJ_FIRST_NEW - 1))
    them = 0
    for k in range(OBJ_FIRST_NEW - 1, len(vrows)):
        c = vrows[k]
        mota = "ObjData DataID %s '%s' Kind=%s img=%s drop=%s" % (c[1], dec(c[0]), c[2], c[4], c[43])
        if k < n:
            if jrows[k] != c:
                loi("ObjData JX1 da co DataID %s nhung khac VLTK: %s" % (c[1], [(i, jrows[k][i], c[i]) for i in range(len(c)) if jrows[k][i] != c[i]][:4]))
            print("    [=] da co " + mota)
            continue
        J.append_row(c, mota)
        print("    [+] " + mota)
        them += 1
    TONG[p] = (them, J)
    return J


def buoc_cws(tree):
    p = os.path.join(tree, "ClientWeaponSkill.txt")
    J = Tep(p)
    if J.header != CWS_HDR:
        loi("ClientWeaponSkill header %s != %s" % (J.header, CWS_HDR))
    co = {}
    for c in J.data_rows():
        co.setdefault((c[0], c[1]), set()).add(c[2])
    print("  %s: %d hang; cap (detail,particular) hien co: %s" % (p, len(J.data_rows()), sorted(co)))
    them = 0
    for d, pt, sk in [("0", "7", SKILL_THUAN), ("0", "8", SKILL_THUAN), ("1", "3", SKILL_CAM)]:
        mota = "ClientWeaponSkill detail=%s particular=%s -> PhysicsSkillID %d (g_n%sWeaponSkill[%s])" % (
            d, pt, sk, "Melee" if d == "0" else "Range", pt)
        if (d, pt) in co:
            print("    [=] da co (%s,%s) -> %s%s" % (d, pt, sorted(co[(d, pt)]), "" if str(sk) in co[(d, pt)] else "  [!] KHAC skill de xuat %d" % sk))
            continue
        J.append_row([d, pt, str(sk)], mota)
        print("    [+] " + mota)
        them += 1
    TONG[p] = (them, J)
    return J


def thong_tin_skills():
    p = os.path.join(SRV, "skills.txt")
    if not os.path.exists(p):
        return
    T = Tep(p)
    h = T.header
    if "EqtLimit" not in h or "SkillId" not in h:
        return
    ie, ii, iname = h.index("EqtLimit"), h.index("SkillId"), h.index("SkillName")
    cnt = collections.Counter()
    vd = collections.defaultdict(list)
    for c in T.data_rows():
        if ie < len(c) and c[ie] in ("6", "7", "8", "103"):
            cnt[c[ie]] += 1
            vd[c[ie]].append("%s %s" % (c[ii], dec(c[iname])))
    print("  skills.txt server: EqtLimit 7=%d 8=%d 103=%d hang (6=%d)" % (cnt["7"], cnt["8"], cnt["103"], cnt["6"]))
    for k in ("7", "8", "103"):
        print("    EqtLimit %s: %s" % (k, "; ".join(vd[k])))


def main():
    bang("VHTD vu khi thuan/cam - %s" % ("KIEM (khong ghi)" if KIEM else "GHI THAT"))
    for rel in ("item\\meleeweapon.txt", "item\\rangeweapon.txt", "item\\MeleeRes.txt", "item\\RangeRes.txt", "obj\\ObjData.txt", "ClientWeaponSkill.txt"):
        a, b = os.path.join(SRV, rel), os.path.join(CLI, rel)
        for q in (a, b):
            if not os.path.exists(q):
                loi("thieu tep " + q)
        print("  server == client (%s): %s" % (rel, same_bytes(a, b)))
    if OBJ_CAM:
        print("  --obj-cam=%s: cot 5 (vat roi) cua 10 hang Moc Cam se doi tu 481 -> %s" % (OBJ_CAM, OBJ_CAM))
    else:
        print("  [!] Moc Cam VLTK tro objdata 481 = 'Kim Bao Thach' (sjzh_jin.spr); 484 = 'Co Cam' (obj_wq_011.spr). Giu 481 (dung --obj-cam=484 de doi).")
    thong_tin_skills()

    teps = []
    for tree in (SRV, CLI):
        bang("CAY: " + tree)
        print("[1] meleeweapon.txt  (+30 hang P6/P7/P8 VLTK -> record 60..89)")
        teps.append(buoc_weapon(tree, "meleeweapon.txt", V_MELEE, MELEE_NEW_P, 6, "0"))
        print("[2] rangeweapon.txt  (+10 hang P3 Moc Cam -> record 30..39)")
        teps.append(buoc_weapon(tree, "rangeweapon.txt", V_RANGE, RANGE_NEW_P, 3, "1", OBJ_CAM))
        print("[3] MeleeRes.txt     (id 62..91: phan -> 2 / 270 / 271)")
        teps.append(buoc_meleeres(tree))
        print("[4] RangeRes.txt     (dong du lieu 30..39 = id JX1 31..40 = engine row 32..41: phan -> 278)")
        teps.append(buoc_rangeres(tree))
        print("[5] ObjData.txt      (+22 hang DataID 463..484, 480 = Dao Thuan, 484 = Co Cam)")
        teps.append(buoc_objdata(tree))
        print("[6] ClientWeaponSkill.txt (+3 hang)")
        teps.append(buoc_cws(tree))

    bang("TONG KET")
    for p, (n, J) in TONG.items():
        print("  %-95s %3d thay doi %s" % (p, n, "(se ghi)" if J.changed() else "(khong doi)"))
    if KIEM:
        print("\nKIEM: KHONG ghi gi. Chay lai voi --ghi-that sau khi chu duyet.")
        return
    if not GHI_THAT:
        raise SystemExit("CHUA DUYET")
    da_ghi = 0
    for J in teps:
        if J.ghi():
            da_ghi += 1
            print("  DA GHI %s (ban luu %s)" % (J.path, J.path + BAK))
    print("\nXONG: %d tep da ghi." % da_ghi)


if __name__ == "__main__":
    main()
