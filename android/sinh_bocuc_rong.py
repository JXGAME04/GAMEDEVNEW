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
a = sys.argv[1:]
i = 0
while i < len(a):
    if a[i] == "--rong": W1 = int(a[i + 1]); i += 2
    elif a[i] == "--cao": H1 = int(a[i + 1]); i += 2
    elif a[i] == "--chuan": CHUAN = True; W1, H1 = W0, H0; i += 1
    else: i += 1
DX, DY = W1 - W0, H1 - H0
NGUON = r"android\du_lieu_ghi_de\ui\uitoado_macdinh.ini"
DICH = [r"android\du_lieu_ghi_de\ui\uitoado_macdinh_rong.ini", r"D:\jx1_android_data\ui\uitoado_macdinh_rong.ini"]
if CHUAN:
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
                "KUiPlayerBar|Options": (729, 47)}
NHOM_PHAI_KHOA = ("KUiTaskTrace|Main",)                 # bang nhiem vu: dat theo nhom phai du x < 728
HOP_THOAI = ("KUiESCDlg|Main", "KUiOptions|Main", "KUiInformation|Main", "KUiMailManager|Main", "KUiAuctionManager|Main",
             "KUiTongJX2|Main")


def doc_bang(p):
    bang = []; nl = "\n"
    s = io.open(p, encoding="latin-1", newline="").read()
    nl = "\r\n" if "\r\n" in s else "\n"
    for d in s.split(nl):
        m = re.match(r"^([^;=\[][^=]*)=(-?\d+),(-?\d+)(?:,(\d+))?(?:,(\d+))?(?:,-?\d+)?(?:,-?\d+)?\s*$", d)   # 4 hoac 6 truong (neo bo qua, gan lai)
        if m and m.group(1).strip().lower() != "manhinh":
            bang.append([m.group(1).strip(), int(m.group(2)), int(m.group(3)), int(m.group(4) or 1000), int(m.group(5) or 0)])
    return bang, nl


def lop_cua(khoa):
    return khoa.split("|")[0] if "|" in khoa else ""


def tuyet_doi(khoa, x, y):
    c = CHA.get(lop_cua(khoa)) if "|" in khoa and khoa.split("|")[1] != "Main" else None
    return (x + c[0], y + c[1]) if c else (x, y)


def tuong_doi_moi(khoa, ax, ay):
    c = CHA_MOI.get(lop_cua(khoa)) if "|" in khoa and khoa.split("|")[1] != "Main" else None
    return (ax - c[0], ay - c[1]) if c else (ax, ay)


def main():
    bang, nl = doc_bang(NGUON)
    co = {b[0]: b for b in bang}
    for k, (x, y) in INI_MAC_DINH.items():
        if k not in co:
            bang.append([k, x, y, 1000, 0]); co[k] = bang[-1]
    # o phim tat 1-4 (KUiPlayerBar|Item_0..3, ini 932/972 x 205/245, 36 px): khoi 2x2 ben TRAI cot icon phai, phong 1,25 (45 px)
    for k, (x, y) in {"KUiPlayerBar|Item_0": (932, 205), "KUiPlayerBar|Item_1": (972, 205),
                      "KUiPlayerBar|Item_2": (932, 245), "KUiPlayerBar|Item_3": (972, 245)}.items():
        if k not in co:
            bang.append([k, x, y, 1000, 0]); co[k] = bang[-1]
    O_PHIM = {"KUiPlayerBar|Item_0": (1198, 206), "KUiPlayerBar|Item_1": (1246, 206),
              "KUiPlayerBar|Item_2": (1198, 254), "KUiPlayerBar|Item_3": (1246, 254)}
    ra = []
    hang_tren = dict((k, i) for i, (k, _) in enumerate(HANG_TREN))
    cot_phai = dict(COT_PHAI)
    for khoa, x, y, tile, cocb in bang:
        lop = lop_cua(khoa)
        la_main = ("|" in khoa and khoa.split("|")[1] == "Main") or "|" not in khoa
        neo_x, neo_y = 0, 0                                     # [UITOADO 12/09 NEO] 0 trai/tren, 1 giua, 2 phai/duoi
        if lop in CHA_MOI and la_main:                       # cha toan man hinh: vi tri moi (tuyet doi)
            nx, ny = CHA_MOI[lop]
            ra.append((khoa, nx, ny, tile, cocb, 1 if lop == "KUiPlayerBar" else 0, 0)); continue
        ax, ay = tuyet_doi(khoa, x, y)
        if khoa in hang_tren:
            if not CHUAN:
                i = hang_tren[khoa]
                ax, ay, tile = HANG_TREN_X0 + i * HANG_TREN_GIAN, HANG_TREN_Y + 1, HANG_TREN_TILE   # tuyet doi (hang cu: rel 47 + cha 1)
            neo_x = 1
        elif khoa in cot_phai:
            if not CHUAN:
                ax, ay, tile = COT_PHAI_X, cot_phai[khoa] + 14, COT_PHAI_TILE
            neo_x = 2
        elif khoa in O_PHIM:
            if not CHUAN:
                ax, ay = O_PHIM[khoa]; tile = 1250
            neo_x = 2
        elif khoa == "KUiToolsControlBar|PK":
            ax += DX; neo_x = 2
            if not CHUAN: tile = 1100
        elif khoa.startswith("KyNang") or ax >= 728 or khoa in NHOM_PHAI_KHOA:
            ax += DX - (CUM_KYNANG_LUI if khoa.startswith("KyNang") else 0)   # nhom phai (cum ky nang lui mot chut)
            neo_x = 2
            if ay >= 423 or khoa.startswith("KyNang"): ay += DY; neo_y = 2      # cum ky nang: ca nhom cung dich
        elif lop in ("KUiPlayerBar", "KUiAuctionIcon", "KUiMailIcon") or khoa in HOP_THOAI:
            if khoa == "KUiPlayerBar|HideChat":
                neo_x = 0                                       # tab mep trai: giu
            else:
                ax += DX // 2; neo_x = 1                        # thanh duoi / hop thoai: giua
            if ay >= 423: ay += DY; neo_y = 2
        else:
            if ay >= 423: ay += DY; neo_y = 2                   # trai / tren: giu
        if neo_y == 0 and ay >= H1 * 0.65: neo_y = 2
        nx, ny = tuong_doi_moi(khoa, ax, ay)
        ra.append((khoa, nx, ny, tile, cocb, neo_x, neo_y))
    dau = ["; [UITOADO 12/09 RONG] Bo cuc mac dinh cho dien thoai man rong (sinh boi android/sinh_bocuc_rong.py tu uitoado_macdinh.ini).",
           "; Game chon tep nay khi khung ve co ti le >= 1,9 (UiToaDo.cpp). Moi dong: <lop>|<muc> = Left,Top,TiLe,Co,NeoX,NeoY",
           "; (tuong doi cha; neo: 0 trai/tren, 1 giua, 2 phai/duoi - khung ve khac ManHinh thi dich theo neo, [UITOADO 12/09 NEO]).",
           "[Pos]", "ManHinh=%d,%d" % (W1, H1)]
    dong = dau + ["%s=%d,%d,%d,%d,%d,%d" % r for r in ra]
    for p in DICH:
        io.open(p, "w", encoding="latin-1", newline="").write(nl.join(dong) + nl)
    print("da ghi %d muc -> %s (khung %dx%d, dX=%d dY=%d)" % (len(ra), DICH[0], W1, H1, DX, DY))
    for r in ra:
        if r[0] in hang_tren or r[0] in cot_phai or r[0].startswith("KyNang0") or r[0].endswith("|Main") and lop_cua(r[0]) in CHA_MOI:
            print("   %-32s %5d %5d %5d" % r[:4])


if __name__ == "__main__":
    main()
