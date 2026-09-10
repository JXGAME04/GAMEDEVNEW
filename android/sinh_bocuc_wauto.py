# -*- coding: utf-8 -*-
r"""[ANDROID 11/09 WAUTO B2] BO SINH BO CUC + BANG C cho trang noi dung cua khung WAuto trong game.

Vao: android/wauto_bang.json (sinh_bang_wauto.py).
Ra:  android/du_lieu_ghi_de/ui/ui3/uiwauto_tabN.ini   (N = 0..14)  - vi tri / co / nhan (TCVN3) cua tung o trong khung 720x432
     Sources/S3Client/Ui/UiCase/UiWAutoBang.h           - bang C: moi o -> loai, khe (slot) trong kho widget, offset trong autoData,
                                                          danh sach lua chon (TCVN3). UiWAutoTrang.cpp chay theo bang nay.
     --xem: anh xem truoc tung tab (scratchpad\xem_tabN.png) de soat bo cuc khong can may ao.

[B2 b] Chu (11/09): "phai lam cho dep - chu phai hien thi day du - thang hang theo tung muc - phai ke o de phan biet tung nhom".
Cach xep (KHONG con nhan toa do hop thoai PC nhu ban dau):
  * HANG: dieu khien cung y (+-3 don vi hop thoai) = mot hang. NHOM: hang cach nhau >= 18 don vi, hoac hang chi co MOT dong chu
    (tieu de: "Tuy chon nhan vat", "Thiet lap ky nang"...) -> mo nhom moi; moi nhom ve mot O (KWndHopNhomWA: nen toi + vien + tieu de).
  * Trong nhom, hang kieu "nhan + dieu khien" dat dieu khien tu cot chung col1 = nhan dai nhat cua nhom -> THANG HANG. Hang toan tick
    (13h23 / 17h50...) chia o deu. Rong nhan = 6 px x so ky tu (font 12) -> khong bao gio cat chu; dai qua thi xuong dong (lui vao col1).
  * Trang 720x432, vung noi dung y 94..380, 2 cot x 336 (moi cot mot chong nhom, nhom dai qua thi tach sang cot sau). Buoc hang thu
    28 -> 26 -> 24 px cho toi khi vua 2 cot; van khong vua thi 3 cot.
  * Anh: tick_chon 26, hop_chon_120/160/200 (mui ten), nut_do_60/84/120/160 (android/anh_wauto_vnku.py, nen do). Danh sach = o tam (B18).
Chu: TCVN3 qua vn_edit.vn(); chu HOA co dau (khong co trong TCVN3) tu ha thanh chu thuong.
Dung: python android/sinh_bocuc_wauto.py [--xem]
"""
import io
import json
import os
import sys
import unicodedata

GOC = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

DAU = "[ANDROID 11/09 WAUTO B2]"
KW, KH = 720, 432
RAIL = 19
Y0, Y1 = 94, 382              # vung noi dung (duoi dai tab 70+22, tren nut BAT/TAT 384)
CAC_BUOC = (28, 26, 24)       # buoc hang thu lan luot
GUTTER = 10
PAD_L, PAD_R, PAD_T, PAD_B, TIEU_DE_H, CACH_NHOM = 6, 6, 6, 3, 18, 4
TICK = 24                     # = tick_chon.spr (anh_wauto_vnku.py TICK); phai <= buoc hang nho nhat (24)
NHAN_H = 16
EDIT_W, EDIT_H = 44, 20       # 6 chu so (MaxLen=6) = 36 px + le
CHON_RONG, CHON_H = (120, 160, 200), 24
NUT_RONG, NUT_H = (60, 84, 120, 160), 22
DSACH_HANG = 3
FONT = 12
CHU_W = 6                     # font 12: moi byte TCVN3 = 6 px (KWndLabeledButton: nMaxLen = Width*2/Font)
KHOANG = 6                    # khoang giua cac dieu khien trong hang
KHOANG_DON_VI = 4             # chu don vi ("van", "giay", "phut") ngay sau o nhap: sat hon
POOL = dict(tick=22, nhap=12, chon=12, nhan=32, nut=12, dsach=2, hop=8)
KIND = dict(tick=1, nhap=2, chon=3, nhan=4, nut=5, dsach=6)
KIEU = {"int": 0, "short": 1, "UINT": 2, "unsigned int": 2, "char": 3, None: 4}
GAP_NHOM = 18                 # don vi hop thoai PC: hang cach nhau tu day tro len -> nhom moi

# Tieu de tay cho nhom KHONG co dong chu tieu de trong WAuto.rc: (tab, IDC cua muc dau nhom) -> tieu de.
# Chi ghi cai chac chan; tab nao lam ky o buoc B3+ thi bo sung o do.
TIEU_DE_TAY = {
    (9, "IDC_CHECKBOX_9_ON"): "Tham gia",
    (9, "IDC_STRING_9_PHE"): "Trong trận",
    (9, "IDC_STRING_9_VE"): "Sau trận",
    (14, "IDC_CHECKBOX_15_ON"): "Tham gia",
    (14, "IDC_STRING_15_BD"): "Báo danh",
    (14, "IDC_STRING_15_VE"): "Sau trận",
    (8, "IDC_CHECKBOX_8_ON"): "Nhiệm vụ Dã Tẩu",
    (7, "IDC_CHECKBOX_7_F"): "Chế độ AutoPK",
    (7, "IDC_STRING_7_PY"): "Ưu tiên và bùa",
    (2, "IDC_CHECKBOX_2_F"): "Mục tiêu",
    (2, "IDC_STRING_2_SA1"): "Vòng sáng",
    (2, "IDC_STRING_2_SC"): "Đổi vũ khí",
    (10, "IDC_CHECKBOX_10_LAP"): "Chiến đội",
    (13, "IDC_CHECKBOX_14_ON"): "Chuỗi chiêu",
    (13, "IDC_CHECKBOX_14_TC"): "Tiền chiêu",
    (12, "IDC_CHECKBOX_13_ON"): "Săn boss",
    (11, "IDC_CHECKBOX_12_BN"): "Bách Nhân Lôi Đài",
    (11, "IDC_CHECKBOX_12_BC"): "Bang Chiến",
    (11, "IDC_STRING_12_TSG"): "Tín Sứ",
    (6, "IDC_COMBO_6_TI"): "Mời / vào nhóm",
    (6, "IDC_COMBO_6_NE"): "Danh sách tên",
    (6, "IDC_CHECKBOX_6_LEA"): "Rời nhóm",
    (3, "IDC_CHECKBOX_3_CD"): "Chạy tọa độ",
    (3, "IDC_CHECKBOX_3_UH"): "Trên đường",
    (4, "IDC_CHECKBOX_4_PI"): "Nhặt",
    (4, "IDC_CHECKBOX_4_FT"): "Lọc",
    (4, "IDC_CHECKBOX_4_PRZ"): "Giữ đồ quý",
    (5, "IDC_CHECKBOX_5_R"): "Về thành và mua bán",
    (0, "IDC_STRING_0_L"): "Nhân vật",
    (0, "IDC_CHECKBOX_0_NTS"): "Máy",
}


def tcvn(s):
    """tieng Viet -> TCVN3 (latin-1). Chu HOA co dau -> ha thuong tung chu; van hong -> bo dau."""
    s = s.replace("&", "").replace("\\n", " ").strip()
    try:
        return vn(s)
    except Exception:
        pass
    out = []
    for ch in s:
        try:
            vn(ch)
            out.append(ch)
        except Exception:
            try:
                vn(ch.lower())
                out.append(ch.lower())
            except Exception:
                out.append("".join(c for c in unicodedata.normalize("NFD", ch) if unicodedata.category(c) != "Mn"))
    return vn("".join(out))


def rong_chu(s):
    """rong PHAN CHU (px) - font 12: 6 px / byte TCVN3"""
    return CHU_W * len(tcvn(s)) if s else 0


NHAN_DEM = FONT + 2           # KWndText -> OutputRichText chi ve floor((Width - font) / (font / 2)) ky tu -> Width = 6n + font + 2 (do 20:39: "Ph", "Di som (phut")


def c_str(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


def hang_hoa(rows):
    """gom dieu khien theo hang (y +-3), moi hang sap theo x"""
    rows = sorted(rows, key=lambda r: (r["y"], r["x"]))
    hang = []
    for r in rows:
        if hang and abs(r["y"] - hang[-1][0]["y"]) <= 3:
            hang[-1].append(r)
        else:
            hang.append([r])
    for h in hang:
        h.sort(key=lambda r: r["x"])
    return hang


# ---------------------------------------------------------------- nhom
class Nhom:
    def __init__(self, tieu_de=None):
        self.tieu_de = tieu_de
        self.hang = []          # moi hang: danh sach dieu khien (dict cua json)
        self.tiep = False       # phan tiep cua nhom bi tach cot


def gop_hang_nut(hang):
    """cac hang CHI co nut nam ben phai danh sach (Doc/Len/Xuong/Xoa..., RC x >= 100, trong tam cao cua danh sach) -> gom thanh
    MOT hang dat ngay sau danh sach; cac hang chi-nut lien tiep khac cung gom lai."""
    ds = [r for h in hang for r in h if r["kind"] == "dsach"]
    quanh = []
    ra = []
    for h in hang:
        chi_nut = all(r["kind"] == "nut" for r in h) and h[0]["x"] >= 100
        if chi_nut and ds:                      # tab co danh sach: moi hang chi-nut ben phai deu la nut cua danh sach (RC cao list = 1 hang)
            quanh += h
            continue
        if chi_nut and ra and all(r["kind"] == "nut" for r in ra[-1]) and ra[-1][0]["x"] >= 100:
            ra[-1] = ra[-1] + h
        else:
            ra.append(list(h))
    if quanh:
        for i, h in enumerate(ra):
            if any(r["kind"] == "dsach" for r in h):
                ra.insert(i + 1, quanh)
                break
    return ra


def chia_nhom(n, hang):
    nhom = []
    cur = None
    y_truoc = None
    hang = gop_hang_nut(hang)
    for h in hang:
        y = min(r["y"] for r in h)
        chu = [r for r in h if r["kind"] == "chu"]
        khac = [r for r in h if r["kind"] != "chu"]
        nhan_don = (len(h) == 1 and h[0]["kind"] == "chu" and h[0]["label"])
        tieu_de_hang = (len(h) >= 2 and h[0]["kind"] == "chu" and h[0]["label"] and not h[0]["label"].endswith(":")
                        and not khac == [] and all(r["kind"] == "tick" and r["x"] >= 100 for r in khac) and len(chu) == 1)
        tay = TIEU_DE_TAY.get((n, h[0]["idc"]))
        moi = cur is None or (y_truoc is not None and y - y_truoc >= GAP_NHOM) or nhan_don or tieu_de_hang or tay is not None
        if moi:
            cur = Nhom(tay)
            nhom.append(cur)
        bang_dau = (moi and len(h) >= 3 and all(r["kind"] == "chu" for r in h) and h[0]["label"] and not cur.tieu_de)
        if nhan_don:
            cur.tieu_de = h[0]["label"].rstrip(":").strip()
        elif bang_dau:
            # hang tieu de cot o dau nhom ("Hoi phuc | Muc 1 | Muc 2 | 1/1000 giay"): chu dau = tieu de nhom, con lai = tieu de cot
            cur.tieu_de = h[0]["label"].strip()
            cur.hang.append([dict(kind="trong", idc="", label="", x=0, y=h[0]["y"], w=0, h=0)] + h[1:])
        elif tieu_de_hang:
            cur.tieu_de = h[0]["label"].strip()
            cur.hang.append(khac)
        else:
            cur.hang.append(h)
        y_truoc = max(r["y"] for r in h)
    return [g for g in nhom if g.hang or g.tieu_de]


# ---------------------------------------------------------------- do rong tung o
def rong_chon(r):
    lc = [str(v) for v in (r.get("lua_chon") or []) if not str(v).startswith("dong:")]
    cach = str(r.get("cach", ""))
    if cach.startswith("chieu"):
        can = 160
    elif lc:
        can = max(rong_chu(v) for v in lc) + 34
    else:
        can = 160
    for w in CHON_RONG:
        if w >= can:
            return w
    return CHON_RONG[-1]


def rong_nut(r):
    can = rong_chu(r["label"]) + 24
    for w in NUT_RONG:
        if w >= can:
            return w
    return NUT_RONG[-1]


def rong_muc(r, con_lai):
    k = r["kind"]
    if k == "trong":
        return 0
    if k == "tick":
        return TICK + 4 + rong_chu(r["label"]) + NHAN_DEM
    if k == "chu":
        return rong_chu(r["label"] or "-") + NHAN_DEM
    if k == "onhap":
        if r.get("cach") == "chuoi":
            return max(EDIT_W, min(200, con_lai))
        return EDIT_W
    if k == "chon":
        return rong_chon(r)
    if k == "nut":
        return rong_nut(r)
    return con_lai


def co_lai_chon(h, IW, lui=0, khoang=KHOANG):
    """-> dict r-> rong: neu ca hang khong vua IW thi co dan cac hop chon (toi thieu 120) roi moi chiu xuong dong"""
    w = {id(r): rong_muc(r, IW) for r in h}
    def tong():
        return lui + sum(w.values()) + khoang * (len(h) - 1)
    while tong() > IW:
        rong_nhat = None
        for r in h:
            if r["kind"] == "chon" and w[id(r)] > CHON_RONG[0] and (rong_nhat is None or w[id(r)] > w[id(rong_nhat)]):
                rong_nhat = r
        if rong_nhat is None:
            break
        w[id(rong_nhat)] = max(x for x in CHON_RONG if x < w[id(rong_nhat)])
    return w


def loai_hang(h):
    kinds = [r["kind"] for r in h]
    if "dsach" in kinds:
        return "dsach"
    if len(h) == 1:
        return "don"
    if all(k == "tick" for k in kinds):
        return "tick"
    if kinds[0] == "trong":
        return "bang"
    if all(k == "chu" for k in kinds) and len(h) >= 3:
        return "bang"
    if kinds[0] in ("tick", "chu") and all(k in ("onhap", "chon", "nut", "chu") for k in kinds[1:]):
        return "nhan"
    return "tron"


# ---------------------------------------------------------------- xep mot nhom -> danh sach o
class O:
    """mot o da dat: kind, x, y (tuong doi goc trang), w, h, r (json), extra"""
    def __init__(self, kind, x, y, w, h, r=None, **kw):
        self.kind, self.x, self.y, self.w, self.h, self.r = kind, x, y, w, h, r
        self.kw = kw


def xep_nhom(nh, IW, P):
    """-> (danh sach O tuong doi goc vung hang (0,0) cua nhom, so dong)"""
    o = []
    # col1: nhan dai nhat trong cac hang kieu "nhan"
    col1 = 0
    for h in nh.hang:
        if loai_hang(h) == "nhan" and any(r["kind"] in ("onhap", "chon", "nut") for r in h[1:]):
            col1 = max(col1, rong_muc(h[0], IW))
    col1 = min(col1 + KHOANG, int(IW * 0.42))
    dong = 0
    x_thu_hai = [30]          # x cua o thu hai o hang truoc (de hang con "13h23 / 17h50" lui vao thang hang)

    def dat(r, x, dong, w=None):
        k = r["kind"]
        y = dong * P
        if k == "trong":
            return
        if k == "tick":
            o.append(O("tick", x, y + (P - TICK) // 2, TICK, TICK, r))
            if r["label"]:
                o.append(O("nhan", x + TICK + 4, y + (P - NHAN_H) // 2 + 1, rong_chu(r["label"]) + NHAN_DEM, NHAN_H, r, text=r["label"], cua_tick=True))
        elif k == "chu":
            o.append(O("nhan", x, y + (P - NHAN_H) // 2 + 1, w or rong_muc(r, IW), NHAN_H, r, text=r["label"] or "-"))
        elif k == "onhap":
            o.append(O("nhap", x, y + (P - EDIT_H) // 2, w or rong_muc(r, IW), EDIT_H, r))
        elif k == "chon":
            o.append(O("chon", x, y + (P - CHON_H) // 2, w or rong_chon(r), CHON_H, r))
        elif k == "nut":
            o.append(O("nut", x, y + (P - NUT_H) // 2, w or rong_nut(r), NUT_H, r))

    for h in nh.hang:
        lh = loai_hang(h)
        if lh == "dsach":
            ds = [r for r in h if r["kind"] == "dsach"][0]
            o.append(O("dsach", 0, dong * P + 2, IW, DSACH_HANG * P - 4, ds))
            dong += DSACH_HANG
            khac = [r for r in h if r["kind"] != "dsach"]
            x = 0
            for r in khac:
                w = rong_muc(r, IW - x)
                if x + w > IW and x > 0:
                    dong += 1
                    x = 0
                dat(r, x, dong, w)
                x += w + KHOANG
            if khac:
                dong += 1
        elif lh == "don":
            dat(h[0], 30 if h[0]["x"] >= 40 else 0, dong)
            dong += 1
        elif lh == "tick":
            n = len(h)
            moi_dong = 3 if n >= 3 else 2
            lui = x_thu_hai[-1] if h[0]["x"] >= 40 else 0
            slot = (IW - lui) // moi_dong
            if any(rong_muc(r, IW) > slot for r in h):        # nhan dai: xep noi tiep
                x = lui
                for r in h:
                    w = rong_muc(r, IW)
                    if x + w > IW and x > lui:
                        dong += 1
                        x = lui
                    dat(r, x, dong)
                    x += w + KHOANG
                dong += 1
            else:
                for i, r in enumerate(h):
                    if i and i % moi_dong == 0:
                        dong += 1
                    dat(r, lui + (i % moi_dong) * slot, dong)
                dong += 1
        elif lh == "bang":
            dat(h[0], 0, dong, rong_muc(h[0], IW))
            x = col1
            for r in h[1:]:
                dat(r, x, dong, EDIT_W)
                x += EDIT_W + KHOANG
            dong += 1
        elif lh == "nhan":
            w0 = rong_muc(h[0], IW)
            dat(h[0], 0, dong, w0 if h[0]["kind"] == "chu" else None)
            x = col1 if w0 + KHOANG <= col1 else None
            wr = co_lai_chon(h[1:], IW, lui=max(col1, w0 + KHOANG))
            if x is None:
                # nhan dai hon col1: noi tiep cung dong neu ca hang con vua (sau khi co hop chon), khong thi xuong dong (lui vao col1)
                tong = sum(wr.values()) + KHOANG * (len(h) - 2)
                if w0 + KHOANG + tong <= IW or all(r["kind"] == "chu" for r in h[1:]):
                    x = w0 + KHOANG
                else:
                    dong += 1
                    x = col1
            x_thu_hai.append(x)
            for r in h[1:]:
                w = wr[id(r)] if r["kind"] == "chon" else rong_muc(r, IW - x)
                if x + w > IW and x > col1:
                    dong += 1
                    x = col1
                    w = rong_muc(r, IW - x)
                dat(r, x, dong, w)
                x += w + KHOANG
            dong += 1
        else:                                                   # tron: xep noi tiep
            x = x_thu_hai[-1] if h[0]["x"] >= 40 else 0
            lui = x
            wr = co_lai_chon(h, IW, lui=lui, khoang=KHOANG)
            vt_tick = [i for i, r in enumerate(h) if r["kind"] == "tick"]
            giua = None                                         # hang 2 tick ("SinhLuc < [ ]   Noi luc < [ ]"): tick 2 o giua cot
            if len(vt_tick) == 2 and vt_tick[0] == 0 and h[vt_tick[1]]["x"] >= 90:
                nua = IW // 2
                if lui + sum(wr[id(r)] + KHOANG for r in h[:vt_tick[1]]) <= nua and nua + sum(wr[id(r)] + KHOANG for r in h[vt_tick[1]:]) - KHOANG <= IW:
                    giua = (vt_tick[1], nua)
            truoc = None
            for i, r in enumerate(h):
                if giua and i == giua[0]:
                    x = giua[1]
                elif truoc is not None and truoc["kind"] == "onhap" and r["kind"] == "chu":
                    x -= KHOANG - KHOANG_DON_VI                 # chu don vi ngay sau o nhap: sat hon
                w = wr[id(r)] if r["kind"] == "chon" else rong_muc(r, IW - x)
                if x + w > IW and x > lui:
                    dong += 1
                    x = lui
                    w = rong_muc(r, IW)
                dat(r, x, dong, w)
                x += w + KHOANG
                if i == 0:
                    x_thu_hai.append(x)
                truoc = r
            dong += 1
    return o, dong


def cao_nhom(nh, so_dong, P):
    return (TIEU_DE_H if nh.tieu_de else PAD_T) + so_dong * P + PAD_B


def xep_tab(nhom, ncot, P, cho_tach=True):
    """-> danh sach (nhom, cot, y_top, cac O tuyet doi trong trang, so dong) hoac None neu khong vua.
    cho_tach=False: khong tach nhom sang cot khac (tru nhom cao hon ca cot)"""
    CW = (KW - 2 * RAIL - (ncot - 1) * GUTTER) // ncot
    IW = CW - PAD_L - PAD_R
    ra = []
    cot, y = 0, Y0
    hang_doi = list(nhom)
    while hang_doi:
        nh = hang_doi.pop(0)
        o, so_dong = xep_nhom(nh, IW, P)
        ch = cao_nhom(nh, so_dong, P)
        if y + ch > Y1:
            # khong vua cho con lai: tach nhom theo hang neu con cho >= 2 hang (phan sau sang cot ke, khong tieu de);
            # khong tach duoc thi doi ca nhom sang cot ke
            k = len(nh.hang) - 1
            tach = None
            if not cho_tach and y > Y0:
                k = 0                                       # chua thu tach: doi ca nhom sang cot ke truoc
            while k >= 2:
                a = Nhom(nh.tieu_de); a.hang = nh.hang[:k]; a.tiep = nh.tiep
                oa, da = xep_nhom(a, IW, P)
                if y + cao_nhom(a, da, P) <= Y1:
                    tach = (a, oa, da)
                    break
                k -= 1
            if tach is None and y > Y0:
                cot += 1
                y = Y0
                if y + ch > Y1:
                    hang_doi.insert(0, nh)
                    continue
            elif tach is None:
                return None
            else:
                a, oa, da = tach
                b = Nhom(None); b.hang = nh.hang[k:]; b.tiep = True
                hang_doi.insert(0, b)
                nh = a
                o, so_dong = oa, da
                ch = cao_nhom(nh, so_dong, P)
        if cot >= ncot:
            return None
        x0 = RAIL + cot * (CW + GUTTER)
        y_hang = y + (TIEU_DE_H if nh.tieu_de else PAD_T)
        for q in o:
            q.x += x0 + PAD_L
            q.y += y_hang
        ra.append((nh, cot, y, CW, ch, o, so_dong))
        y += ch + CACH_NHOM
    return ra


# ---------------------------------------------------------------- ini + bang C
def sinh_tab(n, rows, ten):
    rows = [r for r in rows if r["kind"] != "khung"]
    hang = hang_hoa(rows)
    nhom = chia_nhom(n, hang)
    bo_cuc = None
    for ncot, cho_tach in ((2, False), (2, True), (3, False), (3, True)):
        for P in CAC_BUOC:
            bo_cuc = xep_tab(nhom, ncot, P, cho_tach)
            if bo_cuc:
                break
        if bo_cuc:
            break
    if not bo_cuc:
        raise SystemExit("tab %d: khong xep vua" % n)
    ini = ["; %s Bo cuc trang '%s' (tab %d) - SINH BANG MAY tu WAuto.rc, khong sua tay (sinh_bocuc_wauto.py --xem de xem truoc)." % (DAU, tcvn(ten), n),
           "; %d hang, %d nhom, %d cot, buoc %d px. Chu thich chi o dong rieng." % (len(hang), len(nhom), ncot, P),
           "[Trang]", "Left=0", "Top=0", "Width=%d" % KW, "Height=%d" % KH, "DummyWnd=1", ""]
    dem = dict(tick=0, nhap=0, chon=0, nhan=0, nut=0, dsach=0, hop=0)
    muc = []
    luachon = []
    canh_bao = []

    def slot(kind):
        i = dem[kind]
        if i >= POOL[kind]:
            canh_bao.append("tab %d: het khe %s (%d)" % (n, kind, POOL[kind]))
            return None
        dem[kind] += 1
        return i

    def muc_nhan(x, y, w, text, mau="230,230,230", halign=0):
        s = slot("nhan")
        if s is None:
            return None
        ini.extend(["[Nhan%d]" % s, "Left=%d" % x, "Top=%d" % y, "Width=%d" % max(int(w), 12), "Height=%d" % NHAN_H,
                    "Font=%d" % FONT, "Color=%s" % mau, "BorderColor=0,0,0", "HAlign=%d" % halign, "Text=%s" % tcvn(text), ""])
        return s

    xem = []          # cho --xem: (kind, x, y, w, h, text)
    mau_nhom = -1
    for (nh, cot, y, CW, ch, o, so_dong) in bo_cuc:
        s = slot("hop")
        x0 = RAIL + cot * (CW + GUTTER)
        if not nh.tiep:
            mau_nhom += 1                                       # chu: "co mau phan biet cac nhom chuc nang"
        if s is not None:
            ini.extend(["[Hop%d]" % s, "Left=%d" % x0, "Top=%d" % y, "Width=%d" % CW, "Height=%d" % ch,
                        "Mau=%d" % (mau_nhom % 6), "TieuDe=%s" % (tcvn(nh.tieu_de) if nh.tieu_de else ""), ""])
        xem.append(("hop", x0, y, CW, ch, nh.tieu_de or "", mau_nhom % 6))
        for q in o:
            r = q.r
            off = r.get("off") if r else None
            kieu = KIEU.get(r.get("kieu"), 4) if (r and off is not None) else 4
            co = (r.get("co") or 0) if r else 0
            chi_so = (r.get("chi_so") or 0) if r else 0
            if q.kind == "tick":
                s = slot("tick")
                if s is None:
                    continue
                ini.extend(["[Tick%d]" % s, "Left=%d" % q.x, "Top=%d" % q.y, "Width=%d" % TICK, "Height=%d" % TICK,
                            "Trans=0", r"Image=\spr\uinew\uiautonew\tick_chon.spr", "Up=0", "Down=1", "CheckBox=1", ""])
                # nhan cua tick di ngay sau trong danh sach o
                ns = 255
                muc.append([KIND["tick"], s, ns, chi_so, off, kieu, co, -1, 0, r["idc"]])
                xem.append(("tick", q.x, q.y, TICK, TICK, ""))
            elif q.kind == "nhan":
                mau = "230,230,230" if q.kw.get("cua_tick") else "255,252,178"
                ns = muc_nhan(q.x, q.y, q.w, q.kw["text"], mau=mau)
                if q.kw.get("cua_tick") and muc and muc[-1][0] == KIND["tick"] and muc[-1][2] == 255:
                    muc[-1][2] = 255 if ns is None else ns
                xem.append(("nhan", q.x, q.y, q.w, q.h, q.kw["text"]))
            elif q.kind == "nhap":
                s = slot("nhap")
                if s is None:
                    continue
                chuoi = (r.get("cach") == "chuoi")
                ini.extend(["[Nhap%d]" % s, "Left=%d" % q.x, "Top=%d" % q.y, "Width=%d" % q.w, "Height=%d" % EDIT_H,
                            "Font=%d" % FONT, "Color=255,255,255", "BorderColor=60,50,30",
                            "MaxLen=%d" % (max(co - 1, 1) if chuoi else 6), "Type=%d" % (2 if chuoi else 1), ""])
                muc.append([KIND["nhap"], s, 255, chi_so, off, 3 if chuoi else kieu, co, -1, 0, r["idc"]])
                xem.append(("nhap", q.x, q.y, q.w, EDIT_H, ""))
            elif q.kind == "chon":
                s = slot("chon")
                if s is None:
                    continue
                ini.extend(["[Chon%d]" % s, "Left=%d" % q.x, "Top=%d" % q.y, "Width=%d" % q.w, "Height=%d" % CHON_H,
                            "Trans=0", "Image=\\spr\\uinew\\uiautonew\\hop_chon_%d.spr" % q.w, "Up=0", "Down=1", "LabelXOffset=-9", "LabelYOffset=5",
                            "Font=%d" % FONT, "Color=255,255,255", "OverColor=255,255,160", "SelectColor=255,255,0", "DisableColor=140,120,120", ""])
                lc = r.get("lua_chon") or []
                cach = str(r.get("cach", ""))
                nguon = 0
                if cach.startswith("chieu"):
                    nguon = 1
                elif any(str(v).startswith("dong:") for v in lc):
                    nguon = 2
                tinh = [str(v) for v in lc if not str(v).startswith("dong:")]
                li = -1
                if tinh:
                    li = len(luachon)
                    luachon.append(tinh)
                muc.append([KIND["chon"], s, 255, chi_so, off, kieu, co, li, nguon, r["idc"]])
                xem.append(("chon", q.x, q.y, q.w, CHON_H, (tinh[0] if tinh else ("(chiêu)" if nguon == 1 else "(chưa hỗ trợ)"))))
            elif q.kind == "nut":
                s = slot("nut")
                if s is None:
                    continue
                ini.extend(["[Nut%d]" % s, "Left=%d" % q.x, "Top=%d" % q.y, "Width=%d" % q.w, "Height=%d" % NUT_H,
                            "Trans=0", "Image=\\spr\\uinew\\uiautonew\\nut_do_%d.spr" % q.w, "Up=0", "Down=1", "LabelYOffset=4",
                            "Font=%d" % FONT, "Color=230,230,230", "OverColor=255,255,160", "SelectColor=255,255,0", "DisableColor=140,120,120",
                            "Label=%s" % tcvn(r["label"]), ""])
                muc.append([KIND["nut"], s, 255, 0, None, 4, 0, -1, 2, r["idc"]])
                xem.append(("nut", q.x, q.y, q.w, NUT_H, r["label"]))
            elif q.kind == "dsach":
                s = slot("dsach")
                if s is None:
                    continue
                sh = slot("hop")
                if sh is not None:
                    ini.extend(["[Hop%d]" % sh, "Left=%d" % q.x, "Top=%d" % q.y, "Width=%d" % q.w, "Height=%d" % q.h, "TieuDe=", ""])
                ns = muc_nhan(q.x, q.y + (q.h - NHAN_H) // 2, q.w, "(danh sách - bước B18)", mau="150,150,150", halign=1)
                muc.append([KIND["dsach"], s, 255 if ns is None else ns, 0, None, 4, 0, -1, 2, r["idc"]])
                xem.append(("dsach", q.x, q.y, q.w, q.h, "(danh sách - B18)"))
    return ini, muc, luachon, canh_bao, dict(hang=len(hang), nhom=len(nhom), cot=ncot, buoc=P, dem=dem), xem


# ---------------------------------------------------------------- xem truoc
def ve_xem(n, ten, xem, out):
    from PIL import Image, ImageDraw, ImageFont
    sys.path.insert(0, os.path.join(GOC, "android"))
    sys.path.insert(0, os.path.join(GOC, "ReverseTools", "tongkim_chat"))
    from bo_cuc_vnku_mobile import doc_spr
    G = os.path.join(GOC, "android", "du_lieu_ghi_de", "spr", "uinew", "uiautonew")
    _, _, kk, _ = doc_spr(os.path.join(G, "khung_wauto.spr"))
    im = kk[0].copy()
    d = ImageDraw.Draw(im)
    try:
        f = ImageFont.truetype("arial.ttf", 12)
        fb = ImageFont.truetype("arialbd.ttf", 12)
    except Exception:
        f = fb = ImageFont.load_default()
    _, _, tick, _ = doc_spr(os.path.join(G, "tick_chon.spr"))
    hop = {}
    for w in CHON_RONG:
        hop[w] = doc_spr(os.path.join(G, "hop_chon_%d.spr" % w))[2][0]
    nut = {}
    for w in NUT_RONG:
        nut[w] = doc_spr(os.path.join(G, "nut_do_%d.spr" % w))[2][0]
    d.text((KW // 2 - 40, 30), "tab %d: %s" % (n, ten), font=fb, fill=(255, 255, 255, 255))
    BANG_MAU = [((150, 110, 60), (60, 40, 10)), ((60, 140, 140), (10, 50, 50)), ((80, 110, 180), (15, 25, 60)),
                ((90, 150, 80), (15, 50, 15)), ((150, 90, 160), (50, 15, 60)), ((190, 120, 50), (70, 35, 10))]
    for it in xem:
        k, x, y, w, h, t = it[:6]
        if k == "hop":
            vien, nen = BANG_MAU[it[6] % 6]
            d.rectangle((x, y, x + w - 1, y + h - 1), fill=(0, 0, 0, 70), outline=vien + (255,))
            if t:
                d.rectangle((x + 8, y + 2, x + 8 + len(t) * 7 + 6, y + 16), fill=nen + (255,), outline=vien + (255,))
                d.text((x + 11, y + 2), t, font=fb, fill=(235, 235, 235, 255))
        elif k == "tick":
            im.alpha_composite(tick[0], (x, y))
        elif k == "nhan":
            d.rectangle((x, y, x + w - 1, y + h - 1), outline=(60, 60, 60, 255))
            d.text((x + 1, y + 1), t, font=f, fill=(230, 230, 230, 255))
        elif k == "nhap":
            d.rectangle((x - 3, y - 2, x + w + 2, y + h), fill=(12, 14, 18, 200), outline=(150, 135, 95, 255))
            d.text((x + 2, y + 3), "123", font=f, fill=(255, 255, 255, 255))
        elif k == "chon":
            im.alpha_composite(hop[w], (x, y))
            d.text((x + 6, y + 5), t[:int((w - 24) / 6.5)], font=f, fill=(255, 255, 255, 255))
        elif k == "nut":
            im.alpha_composite(nut[w], (x, y))
            d.text((x + 6, y + 4), t, font=f, fill=(230, 230, 230, 255))
        elif k == "dsach":
            d.rectangle((x, y, x + w - 1, y + h - 1), fill=(0, 0, 0, 90), outline=(120, 100, 70, 255))
            d.text((x + w // 2 - 50, y + h // 2 - 6), t, font=f, fill=(150, 150, 150, 255))
    im.save(out)


def main():
    xem_truoc = "--xem" in sys.argv
    d = json.load(io.open(os.path.join(GOC, "android", "wauto_bang.json"), encoding="utf-8"))
    ten_tab = d["ten_tab"]
    h = ["// %s BANG dieu khien cho trang noi dung cua khung WAuto trong game - SINH BANG MAY (android/sinh_bocuc_wauto.py)" % DAU,
         "// tu WAutoUI/WAuto.rc + SaveRoleData + ipc_shared.h. KHONG SUA TAY: chay lai bo sinh. Chuoi: TCVN3.",
         "// Doc boi UiWAutoTrang.cpp (chi Android).",
         "#ifndef UiWAutoBang_H", "#define UiWAutoBang_H", "#ifdef JX_ANDROID", "",
         "#define WA_MUC_TICK\t1", "#define WA_MUC_NHAP\t2", "#define WA_MUC_CHON\t3", "#define WA_MUC_NHAN\t4", "#define WA_MUC_NUT\t5", "#define WA_MUC_DSACH\t6",
         "#define WA_KIEU_INT\t0", "#define WA_KIEU_SHORT\t1", "#define WA_KIEU_UINT\t2", "#define WA_KIEU_CHUOI\t3", "#define WA_KIEU_KHONG\t4",
         "#define WA_NGUON_TINH\t0", "#define WA_NGUON_CHIEU\t1", "#define WA_NGUON_KHAC\t2",
         "#define WA_SIZEOF_AUTODATA\t%d" % d["sizeof_autoData"], "",
         "struct WAUiMuc",
         "{",
         "\tunsigned char\tnLoai;\t\t// WA_MUC_*",
         "\tunsigned char\tnKhe;\t\t// khe trong kho widget cung loai (Tick%d / Nhap%d / Chon%d / Nut%d trong ini)",
         "\tunsigned char\tnKheNhan;\t// khe nhan chu di kem (o tick), 255 = khong",
         "\tunsigned char\tnChiSo;\t\t// chi so trong mang (bTKGio[4]...)",
         "\tint\t\t\t\tnOff;\t\t// offset trong autoData (-1 = khong noi)",
         "\tunsigned char\tnKieu;\t\t// WA_KIEU_*",
         "\tunsigned short\tnCo;\t\t// co truong (byte) - chuoi: do dai toi da",
         "\tconst char* const*\tpLuaChon;\t// hop chon: cac dong (TCVN3), NULL neu nguon dong",
         "\tunsigned char\tnLuaChon;",
         "\tunsigned char\tnNguon;\t\t// WA_NGUON_*",
         "\tconst char*\t\tszIdc;\t\t// ten IDC goc ben WAuto.exe (de doi chieu / nhat ky)",
         "};",
         "struct WAUiTab { const WAUiMuc* pMuc; int nMuc; const char* szIni; int nHang; int nCot; };", ""]
    tabs_c = []
    tong_canh_bao = []
    sp = os.path.join(os.environ.get("TEMP", GOC), "wauto_xem")
    if xem_truoc:
        os.makedirs(sp, exist_ok=True)
    for n in range(15):
        rows = d["tabs"].get(str(n), [])
        ini, muc, luachon, canh_bao, tk, xem = sinh_tab(n, rows, ten_tab[n])
        tong_canh_bao += canh_bao
        p = os.path.join(GOC, "android", "du_lieu_ghi_de", "ui", "ui3", "uiwauto_tab%d.ini" % n)
        io.open(p, "w", encoding="latin-1", newline="\r\n").write("\n".join(ini) + "\n")
        for li, lc in enumerate(luachon):
            h.append("static const char* const s_LuaChon_%d_%d[] = { %s };" % (n, li, ", ".join(c_str(tcvn(x)) for x in lc)))
        h.append("static const WAUiMuc s_Muc_%d[] = {" % n)
        for (kind, s, ns, cs, off, kieu, co, li, nguon, idc) in muc:
            h.append("\t{ %d, %d, %d, %d, %d, %d, %d, %s, %d, %d, %s }," % (
                kind, s, ns, cs, -1 if off is None else off, kieu, co,
                ("s_LuaChon_%d_%d" % (n, li)) if li >= 0 else "NULL", len(luachon[li]) if li >= 0 else 0, nguon, c_str(idc)))
        h.append("};")
        tabs_c.append("\t{ s_Muc_%d, %d, \"UiWAuto_Tab%d.ini\", %d, %d }," % (n, len(muc), n, tk["hang"], tk["cot"]))
        print("tab %2d %-10s hang=%2d nhom=%d cot=%d buoc=%d  %s" % (n, ten_tab[n], tk["hang"], tk["nhom"], tk["cot"], tk["buoc"], tk["dem"]))
        if xem_truoc:
            ve_xem(n, ten_tab[n], xem, os.path.join(sp, "xem_tab%d.png" % n))
    h.append("static const WAUiTab s_WAUiTab[15] = {")
    h.extend(tabs_c)
    h.extend(["};", "", "#endif // JX_ANDROID", "#endif", ""])
    p = os.path.join(GOC, "Sources", "S3Client", "Ui", "UiCase", "UiWAutoBang.h")
    io.open(p, "w", encoding="latin-1", newline="\r\n").write("\n".join(h))
    print("da ghi", p, "+ 15 tep uiwauto_tabN.ini" + (" + xem truoc o " + sp if xem_truoc else ""))
    for c in tong_canh_bao:
        print("CANH BAO:", c)


if __name__ == "__main__":
    main()
