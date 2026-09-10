# -*- coding: utf-8 -*-
r"""[UITOADO 12/09 RONG] Sinh bo cuc mac dinh cho DIEN THOAI MAN RONG (khung ve ~1371x617, ti le >= 1,9) tu bo cuc may ao 1040x604.

Chu (02:00, anh dien thoai 21:9): "UI chua di theo kich thuoc man hinh", "cac icon phia tren van nam vi tri cu", "kich vao cac icon
qua nho". Quy tac (toa do trong tep UiToaDo la TUONG DOI cha; cha toan man hinh -> ~tuyet doi):
  * Nhom PHAI (x >= 728 trong 1040): cong dX = 1371 - 1040 -> sat mep phai: ban do nho, Bao Vat, cot icon phai, cum ky nang, Rec...
  * Thanh duoi (KUiPlayerBar|Main + con): dich +dX/2 -> thanh chat, o phim tat nam GIUA man hinh (con khong co muc di theo cha).
  * Hang icon tren (Status/Items/Skills/Faction/Team/Sit/Friend/Options): phong 1,3 lan + gian cach 80 px, dat giua-phai.
  * Cot icon phai (Exchange/Horse/Run/WAuto/NhatDo): phong 1,3 lan, gian 65 px; PK 1,1 lan.
  * Hop thoai giua (ESC, Options, Information, MailManager, AuctionManager): +dX/2. Con lai (HP/MP trai, chat trai...) giu nguyen.
  * Duoi (y >= 423): +dY = 617 - 604.
Ghi: android\du_lieu_ghi_de\ui\uitoado_macdinh_rong.ini (+ D:\jx1_android_data\ui\). Game chon tep nay khi ti le man >= 1,9
(UiToaDo.cpp [UITOADO 12/09 RONG]); ManHinh=1371,617 de may khac neo tiep theo mep.
Dung: python android\sinh_bocuc_rong.py [--rong 1371] [--cao 617]
"""
import io
import os
import re
import sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
W0, H0 = 1040, 604
W1, H1 = 1371, 617
CHUAN = False          # --chuan: chi GAN NEO cho bo cuc may ao 1040x604 (khong doi vi tri, khong phong) -> uitoado_macdinh.ini
NGUYEN = True          # man rong: GIU NGUYEN bo cuc may ao (chu 07:40: "lay ban toa do dang chay o PC roi lam lai"), chi dich theo neo;
                       # --phong: kieu cu (hang icon tren / cot phai phong 1,3 va gian ra, o phim tat doi cho) - chu che "lon xon"
a = sys.argv[1:]
i = 0
while i < len(a):
    if a[i] == "--rong": W1 = int(a[i + 1]); i += 2
    elif a[i] == "--cao": H1 = int(a[i + 1]); i += 2
    elif a[i] == "--chuan": CHUAN = True; W1, H1 = W0, H0; i += 1
    elif a[i] == "--phong": NGUYEN = False; i += 1
    else: i += 1
if CHUAN:
    NGUYEN = True
DX, DY = W1 - W0, H1 - H0
NGUON = r"android\du_lieu_ghi_de\ui\uitoado_macdinh.ini"
DICH = [r"android\du_lieu_ghi_de\ui\uitoado_macdinh_rong.ini", r"D:\jx1_android_data\ui\uitoado_macdinh_rong.ini"]
if CHUAN:
    # nguon cua tep chuan = bo cuc chu dang chay tren may ao (userdata) - KHONG doc lai tep mac dinh da sinh (tranh troi: mot lan
    # chay cu tru 40 px cum ky nang, 893 -> 853)
    NGUON = r"D:\jx1_android_data\userdata\UiToaDo.ini"
    DICH = [r"android\du_lieu_ghi_de\ui\uitoado_macdinh.ini", r"D:\jx1_android_data\ui\uitoado_macdinh.ini"]

# cha toan man hinh va vi tri (tuyet doi) cua no trong bo cuc goc / bo cuc rong
CHA = {"KUiPlayerBar": (1, 1), "KUiToolsControlBar": (9, 14)}
CHA_MOI = {"KUiPlayerBar": (1 + DX // 2, 1), "KUiToolsControlBar": (9, 14)}
# hang icon tren: (khoa, rong goc) theo thu tu trai -> phai; phong 1,3; gian 80
HANG_TREN = [("KUiPlayerBar|Status", 60), ("KUiPlayerBar|Items", 73), ("KUiPlayerBar|Skills", 49), ("KUiPlayerBar|Faction", 55),
             ("KUiPlayerBar|Team", 47), ("KUiToolsControlBar|Sit", 47), ("KUiPlayerBar|Friend", 57), ("KUiPlayerBar|Options", 47)]
HANG_TREN_X0, HANG_TREN_Y, HANG_TREN_GIAN, HANG_TREN_TILE = 495, 47, 80, 1300
# cot icon phai: khoa -> (y moi); x sat mep phai; phong 1,3
COT_PHAI = [("KUiToolsControlBar|Exchange", 147), ("KUiToolsControlBar|Horse", 212), ("KUiToolsControlBar|Run", 277),
            ("KUiToolsControlBar|WAuto", 342), ("KUiToolsControlBar|NhatDo", 422)]      # WAuto cao 56x1,3=73 -> NhatDo 422
CUM_KYNANG_LUI = 40          # cum ky nang lui sang trai 40 px de khong cham cot icon phai (da phong 1,3)
COT_PHAI_X, COT_PHAI_TILE = W1 - 9 - 70, 1300          # tuyet doi ~1292 (icon 47..54 px -> 61..70 px)
# mac dinh ini cho cac con chua co trong bo cuc (Left, Top tuong doi cha)
INI_MAC_DINH = {"KUiPlayerBar|Status": (330, 47), "KUiPlayerBar|Items": (380, 47), "KUiPlayerBar|Skills": (448, 47),
                "KUiPlayerBar|Faction": (501, 47), "KUiPlayerBar|Team": (561, 47), "KUiPlayerBar|Friend": (668, 47),
                "KUiPlayerBar|Options": (729, 47),
                "KUiToolsControlBar|NhatDo": (868, 362)}      # nut nhat (B2 i) chua co trong userdata cua chu -> lay vi tri ini
NHOM_PHAI_KHOA = ("KUiTaskTrace|Main",)
# [UITOADO 12/09 CAO] cua so goc khong co trong userdata/cua_so_ui.json (Init bang muc khac 'Main'): bang thong bao he thong
# (chu do) nam ngay tren o nhap chat -> neo DUOI; khung log chat neo DUOI de giu khoang cach toi o nhap chat nhu PC (man 4:3)
THEM_TAY = {"KSysMsgCentrePad|SysRoom": (0, 469)}
NEO_DUOI_KHOA = ("KUiMsgCentrePad|Main", "KSysMsgCentrePad|SysRoom")
# chu 09:10 + 12/09: khung log chat va bang thong bao he thong PHAI GIU MEP TRAI (tam 18 % nen luat theo tam se cho ra "giua")
NEO_TRAI_KHOA = ("KUiMsgCentrePad|Main", "KSysMsgCentrePad|SysRoom")
# tab an/hien khung log chat: di cung khung log chat (duoi)
NEO_DUOI_CON = ("KUiPlayerBar|HideChat",)                 # bang nhiem vu: dat theo nhom phai du x < 728
# [UITOADO 12/09 TOANBO b] chu 08:30: thanh HP/MP/EXP, khung chat + tab kenh, chien lenh phai di CUNG hang icon / thanh chat (giua),
# khong dung o mep trai trong khi hang icon vao giua -> neo GIUA
NEO_GIUA_KHOA = ("KUiHeaderControlBar|Main", "KUiChienLenh|Main", "KUiTongJX2|Main")   # chu 09:10: khung log chat (MsgCentrePad) GIU TRAI nhu cu
HOP_THOAI = ("KUiESCDlg|Main", "KUiOptions|Main", "KUiInformation|Main", "KUiMailManager|Main", "KUiAuctionManager|Main",
             "KUiTongJX2|Main")


NEO_CU = {}            # khoa -> (neoX, neoY) doc duoc tu tep nguon (6 truong); tep rong dung dung neo nay


def doc_bang(p):
    bang = []; nl = "\n"
    s = io.open(p, encoding="latin-1", newline="").read()
    nl = "\r\n" if "\r\n" in s else "\n"
    for d in s.split(nl):
        m = re.match(r"^([^;=\[][^=]*)=(-?\d+),(-?\d+)(?:,(\d+))?(?:,(\d+))?(?:,(-?\d+))?(?:,(-?\d+))?\s*$", d)   # 4 hoac 6 truong
        if m and m.group(1).strip().lower() != "manhinh":
            k = m.group(1).strip()
            bang.append([k, int(m.group(2)), int(m.group(3)), int(m.group(4) or 1000), int(m.group(5) or 0)])
            if m.group(6) is not None and m.group(7) is not None:
                NEO_CU[k] = (int(m.group(6)), int(m.group(7)))
    return bang, nl


def neo_tam(cx, cy, w, h):
    """neo tu suy theo TAM cua so trong khung thiet ke w x h. Chu 08:30: HUD trai (thanh HP/MP, chat) tach khoi hang icon / thanh chat
    (giua) -> chi neo TRAI khi tam < 12 % (tab sat mep), con lai GIUA; phai >= 65 %. Doc: tren < 35 %, duoi >= 65 %."""
    nx = 0 if cx * 100 < w * 12 else (2 if cx * 100 >= w * 65 else 1)
    ny = 0 if cy * 100 < h * 35 else (2 if cy * 100 >= h * 65 else 1)
    return nx, ny


O_GOC = {}


def doc_o_thuc():
    r"""[UITOADO 12/09 CAO b] Vi tri + KICH CO THAT cua moi cua so khi game chay dung khung thiet ke 1040x604
    (android\o_thuc_1040x604.txt - ban Android ghi ra khi [Ui] NhatKyBoCuc=2, chinh la bo cuc ban PC).
    Dung de suy neo theo TAM THAT cua cua so thay vi doan theo goc tren-trai."""
    import re
    d = {}
    p = os.path.join("android", "o_thuc_1040x604.txt")   # KHONG de trong du_lieu_ghi_de: tep tham chieu, khong can tai ve may
    if not os.path.isfile(p):
        return d
    mau = re.compile(r"(\S+) = abs (-?\d+),(-?\d+) rel (-?\d+),(-?\d+) (\d+)x(\d+)")
    for ln in io.open(p, encoding="latin-1"):
        m = mau.match(ln.strip())
        if m and m.group(1) not in d:
            ax, ay, rx, ry, w, h = [int(v) for v in m.groups()[1:7]]
            if w > 0 and h > 0 and ax > -100:
                d[m.group(1)] = (ax, ay, w, h)
                O_GOC[m.group(1)] = (ax == rx and ay == ry)
    return d


O_THUC = doc_o_thuc()


def them_toan_bo(bang, co):
    r"""[UITOADO 12/09 TOANBO] moi cua so trong game (android\du_lieu_ghi_de\ui\cua_so_ui.json, ini thiet ke 800x600) chua co
    trong bo cuc cua chu -> them muc: neo theo tam, vi tri doi sang khung 1040x604 (x + 240*neo/2, y + 4*neo/2)"""
    import json
    p = os.path.join("android", "du_lieu_ghi_de", "ui", "cua_so_ui.json")
    if not os.path.isfile(p):
        return 0
    d = json.load(io.open(p, encoding="utf-8"))
    so = 0
    for k in sorted(d):
        L, T, W, H = d[k]
        if k in co or W <= 0 or H <= 0 or (W >= 790 and H >= 590):
            continue
        nx, ny = neo_tam(L + W // 2, T + H // 2, 800, 600)
        bang.append([k, L + (W0 - 800) * nx // 2, T + (H0 - 600) * ny // 2, 1000, 0])
        co[k] = bang[-1]
        NEO_CU[k] = (nx, ny)
        so += 1
    return so


def goc_khac_main():
    """[GOC 12/09] {lop: ten muc cua so GOC} cho lop ma cua so goc khong ten "Main".
    Cua so goc nhan ra tu o_thuc_1040x604.txt: dong co abs == rel (khong co cha)."""
    d = {}
    for k, (ax, ay, w, h) in O_THUC.items():
        if "|" not in k:
            continue
        lop, muc = k.split("|", 1)
        if muc == "Main" or lop in d:
            continue
        if O_GOC.get(k):
            d[lop] = muc
    return d


GOC_LOP = {}


def khoa_cha(khoa, co):
    """[GOC 12/09] Khoa cua so GOC (cha) cua mot o con; None neu chinh no la goc / khong co cha trong bang."""
    if "|" not in khoa:
        return None
    lop, muc = khoa.split("|", 1)
    g = GOC_LOP.get(lop, "Main")
    if muc == g:
        return None
    k = lop + "|" + g
    return k if k in co else None


def lop_cua(khoa):
    return khoa.split("|")[0] if "|" in khoa else ""


def tuyet_doi(khoa, x, y):
    c = CHA.get(lop_cua(khoa)) if "|" in khoa and khoa.split("|")[1] != "Main" else None
    return (x + c[0], y + c[1]) if c else (x, y)


def tuong_doi_moi(khoa, ax, ay):
    c = CHA_MOI.get(lop_cua(khoa)) if "|" in khoa and khoa.split("|")[1] != "Main" else None
    return (ax - c[0], ay - c[1]) if c else (ax, ay)


def main():
    GOC_LOP.update(goc_khac_main())                               # [GOC 12/09] lop co cua so goc khong ten "Main"
    bang, nl = doc_bang(NGUON)
    co = {b[0]: b for b in bang}
    for k, (x, y) in INI_MAC_DINH.items():
        if k not in co:
            bang.append([k, x, y, 1000, 0]); co[k] = bang[-1]
    for k, (x, y) in THEM_TAY.items():                            # [UITOADO 12/09 CAO] cua so goc chua co muc, vi tri ini 1040x604
        if k not in co:
            bang.append([k, x, y, 1000, 0]); co[k] = bang[-1]
    # o phim tat 1-4 (KUiPlayerBar|Item_0..3, ini 932/972 x 205/245, 36 px): khoi 2x2 ben TRAI cot icon phai, phong 1,25 (45 px)
    for k, (x, y) in {"KUiPlayerBar|Item_0": (932, 205), "KUiPlayerBar|Item_1": (972, 205),
                      "KUiPlayerBar|Item_2": (932, 245), "KUiPlayerBar|Item_3": (972, 245)}.items():
        if k not in co:
            bang.append([k, x, y, 1000, 0]); co[k] = bang[-1]
    O_PHIM = {"KUiPlayerBar|Item_0": (1198, 206), "KUiPlayerBar|Item_1": (1246, 206),
              "KUiPlayerBar|Item_2": (1198, 254), "KUiPlayerBar|Item_3": (1246, 254)}
    # [DANGNHAP 12/09] cac bang TRUOC khi vao game (menu, dang nhap, chon may chu, ket noi, chon que) 800x600 o goc trai -> neo GIUA
    GIUA_TRUOC_GAME = {"KUiInit|Main": (800, 0), "KUiLogin|Main": (800, 0), "KUiConnectInfo|Main": (800, 0),
                       "KUiSelNativePlace|Main": (800, 0), "KUiSelServer|Main": (281, 61),
                       "KUiSelPlayer|SelRole": (800, 0), "KUiNewPlayer|NewPlayer": (800, 0)}   # chu 07:40: chon nhan vat van mot ben
    ra = []
    for k, (rong, y) in GIUA_TRUOC_GAME.items():
        if k not in co:
            bang.append([k, 0, y, 1000, 0]); co[k] = bang[-1]
    if CHUAN:
        print("them %d cua so tu cua_so_ui.json (neo theo tam)" % them_toan_bo(bang, co))
    hang_tren = dict((k, i) for i, (k, _) in enumerate(HANG_TREN))
    cot_phai = dict(COT_PHAI)
    # cua so goc ('<lop>|Main', KyNang*) xu ly TRUOC de o con thua duoc neo cua cha (tep cua chu liet ke con truoc cha)
    bang = sorted(bang, key=lambda b: 0 if ("|" not in b[0] or b[0].split("|")[1] == "Main") else 1)
    for khoa, x, y, tile, cocb in bang:
        lop = lop_cua(khoa)
        la_main = ("|" in khoa and khoa.split("|")[1] == "Main") or "|" not in khoa
        neo_x, neo_y = 0, 0                                     # [UITOADO 12/09 NEO] 0 trai/tren, 1 giua, 2 phai/duoi
        if khoa in GIUA_TRUOC_GAME:                           # bang truoc khi vao game: can giua theo khung
            ra.append((khoa, (W1 - GIUA_TRUOC_GAME[khoa][0]) // 2, GIUA_TRUOC_GAME[khoa][1], tile, cocb, 1, 0)); continue
        if lop in CHA_MOI and la_main:                       # cha toan man hinh: vi tri moi (tuyet doi)
            nx, ny = CHA_MOI[lop]
            # KUiPlayerBar|Main (anh khung chat nam DUOI cung anh 800x600) neo GIUA-DUOI; con cua no thua neo doc (cung dich voi khung)
            nxo, nyo = (1, 2) if lop == "KUiPlayerBar" else (0, 0)
            NEO_CU[khoa] = (nxo, nyo)
            ra.append((khoa, nx, ny + (DY if nyo == 2 else 0), tile, cocb, nxo, nyo)); continue
        if not CHUAN and NGUYEN and khoa in NEO_CU:
            # tep rong: dung DUNG neo da luu trong tep chuan, CUNG cong thuc voi game (DoiCaTepTheoNeo): toa do con la tuong doi cha
            # -> dich = DX * (neo con - neo cha) / 2 khi '<lop>|Main' co trong bang (moi lop, khong chi KUiPlayerBar/KUiToolsControlBar)
            neo_x, neo_y = NEO_CU[khoa]
            cha_x = cha_y = 0
            k_cha = khoa_cha(khoa, co)
            if k_cha:
                cha_x, cha_y = NEO_CU.get(k_cha, (0, 0))
            ra.append((khoa, x + DX * (neo_x - cha_x) // 2, y + DY * (neo_y - cha_y) // 2, tile, cocb, neo_x, neo_y)); continue
        ax, ay = tuyet_doi(khoa, x, y)
        if khoa in hang_tren:
            if not NGUYEN:
                i = hang_tren[khoa]
                ax, ay, tile = HANG_TREN_X0 + i * HANG_TREN_GIAN, HANG_TREN_Y + 1, HANG_TREN_TILE   # tuyet doi (hang cu: rel 47 + cha 1)
            else:
                ax += DX // 2                                   # hang icon tren di cung thanh duoi (giua)
            neo_x = 1
        elif khoa in cot_phai:
            if not NGUYEN:
                ax, ay, tile = COT_PHAI_X, cot_phai[khoa] + 14, COT_PHAI_TILE
            else:
                ax += DX
            neo_x = 2
        elif khoa in O_PHIM:
            if not NGUYEN:
                ax, ay = O_PHIM[khoa]; tile = 1250
            else:
                ax += DX
            neo_x = 2
        elif khoa == "KUiToolsControlBar|PK":
            ax += DX; neo_x = 2
            if not NGUYEN: tile = 1100
        elif khoa.startswith("KyNang") or ax >= 728 or khoa in NHOM_PHAI_KHOA:
            ax += DX - (CUM_KYNANG_LUI if (khoa.startswith("KyNang") and not NGUYEN) else 0)   # nhom phai
            neo_x = 2
            if ay >= 423 or khoa.startswith("KyNang"): ay += DY; neo_y = 2      # cum ky nang: ca nhom cung dich
        elif lop in ("KUiPlayerBar", "KUiAuctionIcon", "KUiMailIcon") or khoa in HOP_THOAI or khoa in NEO_GIUA_KHOA:
            if khoa == "KUiPlayerBar|HideChat":
                neo_x = 0                                       # tab mep trai: giu
            else:
                ax += DX // 2; neo_x = 1                        # thanh duoi / hop thoai / HUD trai-giua: giua
            if ay >= 423: ay += DY; neo_y = 2
            elif khoa in HOP_THOAI: ay += DY // 2; neo_y = 1     # [UITOADO 12/09 CAO] hop thoai: giua ca chieu doc (man 4:3)
        else:
            if khoa in O_THUC:                                    # [UITOADO 12/09 CAO b] neo theo TAM THAT khi game chay 1040x604
                tx, ty, tw, th = O_THUC[khoa]
                neo_x, neo_y = neo_tam(tx + tw // 2, ty + th // 2, W0, H0)
            elif khoa in NEO_CU:                                  # muc them tu cua_so_ui.json (chuan): giu neo da suy theo tam
                neo_x, neo_y = NEO_CU[khoa]
            elif ay >= 423: neo_y = 2
            if khoa in NEO_DUOI_KHOA: neo_y = 2                    # [CAO] log chat, thong bao he thong: sat o nhap chat
            if khoa in NEO_TRAI_KHOA: neo_x = 0
            ax += DX * neo_x // 2
            ay += DY * neo_y // 2
        if neo_y == 0 and ay >= H1 * 0.65: neo_y = 2
        # con cua KUiPlayerBar / KUiToolsControlBar: neo DOC = neo doc cua cha (o nam tren khung cha, khong duoc troi khoi khung:
        # chu 09:10 "nut chon kenh chat o o nhap tut xuong 13 px")
        if lop in CHA and not la_main:
            # [UITOADO 12/09 CAO] man 4:3 (may tinh bang 1152x864): con o NUA TREN khung cha 800x600 (y < 302: hang icon, cho,
            # nut theo doi, o phim 1-4, gio) neo TREN; nua duoi (o nhap chat, ky nang tuc thoi) neo DUOI cung thanh chat
            neo_y = 2 if (lop == "KUiPlayerBar" and (y >= 302 or khoa in NEO_DUOI_CON)) else 0
            if CHUAN:
                ay = tuyet_doi(khoa, x, y)[1]
        # [UITOADO 12/09 TOANBO b] o CON (toa do tuong doi cha, cha '<lop>|Main' co trong bang) khong thuoc nhom nao -> THUA neo cua cha
        # (truoc: tu suy theo toa do nho -> "trai" -> game dich con lech -331 so voi cha: icon chien lenh, nut thu, nut dau gia mat hinh)
        if lop not in CHA and khoa_cha(khoa, co) \
                and khoa not in hang_tren and khoa not in cot_phai and khoa not in O_PHIM:
            k_cha = khoa_cha(khoa, co)
            if k_cha in NEO_CU:
                neo_x, neo_y = NEO_CU[k_cha]
            ax, ay = tuyet_doi(khoa, x, y)                      # khong dich (dich theo cha o game / o tep rong)
        NEO_CU.setdefault(khoa, (neo_x, neo_y))
        nx, ny = tuong_doi_moi(khoa, ax, ay)
        ra.append((khoa, nx, ny, tile, cocb, neo_x, neo_y))
    dau = ["; [UITOADO 12/09 RONG] Bo cuc mac dinh cho dien thoai man rong (sinh boi android/sinh_bocuc_rong.py tu uitoado_macdinh.ini).",
           "; Game chon tep nay khi khung ve co ti le >= 1,9 (UiToaDo.cpp). Moi dong: <lop>|<muc> = Left,Top,TiLe,Co,NeoX,NeoY",
           "; (tuong doi cha; neo: 0 trai/tren, 1 giua, 2 phai/duoi - khung ve khac ManHinh thi dich theo neo, [UITOADO 12/09 NEO]).",
           "[Pos]", "ManHinh=%d,%d" % (W1, H1)]
    # [GOC 12/09] lop co cua so goc khong ten "Main" (ban do nho: KUiMiniMap|MiniMap): bao cho bo neo biet de o con tru neo cua cha
    for lop, muc in sorted(GOC_LOP.items()):
        dau.append("Goc.%s=%s" % (lop, muc))
    dong = dau + ["%s=%d,%d,%d,%d,%d,%d" % r for r in ra]
    for p in DICH:
        io.open(p, "w", encoding="latin-1", newline="").write(nl.join(dong) + nl)
    print("da ghi %d muc -> %s (khung %dx%d, dX=%d dY=%d)" % (len(ra), DICH[0], W1, H1, DX, DY))
    for r in ra:
        if r[0] in hang_tren or r[0] in cot_phai or r[0].startswith("KyNang0") or r[0].endswith("|Main") and lop_cua(r[0]) in CHA_MOI:
            print("   %-32s %5d %5d %5d" % r[:4])


if __name__ == "__main__":
    main()
