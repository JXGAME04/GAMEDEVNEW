# -*- coding: utf-8 -*-
r"""[ANDROID 10/09] Thanh duoi cho dien thoai: khung Main = khung_chat_new cua VNKU thu nho, GIU cac icon.

Chu: "toi muon xoa cai main nay ma de lai cac icon thay vao cai main cua vnku" + "thanh do nho chu
khong to nhu vay". Cong cu nay sinh ra HAI thu, ghi vao lop ghi de android\du_lieu_ghi_de\ va (neu co)
thu muc du lieu D:\jx1_android_data:
  spr\uinew\uiplayerbar\khung_chat_mobile.spr   khung VNKU thu nho he so k, goc trai duoi, nen 1040x604
  ui\ui3\uiplayerbar.ini, uitoolscontrolbar.ini  ban GOC cua cay client PC + doi anh Main + dat lai vi tri

Dung:  python android\thanh_duoi_mobile.py [k]        (k mac dinh 0.5; 0.6 = to hon mot chut)
Ban PC khong doi gi: chi ini trong lop ghi de Android.

Toa do vung trong anh goc 1343x211 (do bang pixel):  o vuong trai x185..252 y100..175 | dai giay x303..1009
y100..175 | o vuong phai x1029..1097 va x1124..1192 | dai hoa van tren x300..1090 y55..95.
Khung ve giả lập 1040x604; may khac thi SCREEN_WIDTH/HEIGHT khac (xem KSdlApp [DPG]) - toa do ini van tuyet doi
nhu moi ini khac cua game.
"""
import io
import os
import re
import sys

from PIL import Image

GOC = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(GOC, "..", "ReverseTools", "tongkim_chat"))
from ghi_spr import ghi_spr_tu_anh, kiem_spr  # noqa: E402
from spr2png import decode  # noqa: E402

VNKU_KHUNG = r"C:\Users\nguye\Downloads\NHACTAI\VNKU_ui\spr\Spr\UiNew\UiPlayerBar\khung_chat_new.spr"   # CHI DOC
NGUON_PC = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\ui\ui3"                          # ini goc
GHI_DE = os.path.join(GOC, "du_lieu_ghi_de")
DU_LIEU = r"D:\jx1_android_data"
SW, SH = 1040, 604
GOC_W, GOC_H = 1343, 211
DX, DY = 3, 1          # chu dat KUiPlayerBar|Main=3,1 trong UiToaDo -> ini = man hinh - (3,1)


def bo_cuc(k):
    """Toa do MAN HINH cua tung o theo he so thu nho k. Khung dat goc trai duoi."""
    w, h = int(round(GOC_W * k)), int(round(GOC_H * k))
    L, T = 0, SH - h
    X = lambda a: L + int(round(a * k))  # noqa: E731
    Y = lambda a: T + int(round(a * k))  # noqa: E731
    ra = {"k": k, "khung": (L, T, w, h)}
    gx0, gx1 = X(303), X(1009)
    gy = (Y(100) + Y(175)) // 2 - 18
    bd = gx0 + (gx1 - gx0 - 9 * 37 + 1) // 2
    for i in range(9):                                   # vat pham 1..9 tren dai giay
        ra["Item_%d" % i] = (bd + i * 37, gy)
    ra["ChannelBtn"] = ((X(185) + X(252)) // 2 - 10, (Y(100) + Y(175)) // 2 - 10)   # o vuong trai
    ra["ImediaLeftSkill"] = ((X(1029) + X(1097)) // 2 - 18, gy)                     # T
    ra["ImediaRightSkill"] = ((X(1124) + X(1192)) // 2 - 18, gy)                    # P
    bx0, bx1 = X(300), X(1090)
    by = (Y(55) + Y(95)) // 2
    ra["InputEdit"] = (bx0 + 12, by - 9, bx1 - bx0 - 90, 18)                        # go chat: dai hoa van
    ra["Face"] = (bx1 - 66, by - 11)
    ra["SendBtn"] = (bx1 - 36, by - 11)
    x, y = 16, T - 30                                                                # hang icon ngay tren khung
    for ten in ["Status", "Items", "ItemEx", "Skills", "Team", "Faction", "Friend", "Options", "AutoPlay", "HideChat"]:
        ra[ten] = (x, y)
        x += 30
    x += 6
    for ten in ["Run", "Sit", "Horse", "Exchange", "PK", "Rec"]:
        ra[ten] = (x, y)
        x += 30
    return ra


def anh_khung(k):
    goc, _ = decode(VNKU_KHUNG)
    L, T, w, h = bo_cuc(k)["khung"]
    nen = Image.new("RGBA", (SW, SH), (0, 0, 0, 0))
    nen.alpha_composite(goc.convert("RGBA").resize((w, h), Image.LANCZOS), (L, T))
    return nen


def dat_khoa(s, muc, khoa, gia_tri):
    """Dat khoa=gia_tri trong [muc]; muc trung ten nhieu lan -> sua ca hai."""
    ra, pos, n = [], 0, 0
    for m in re.finditer(r"^\[" + re.escape(muc) + r"\][ \t]*\r?\n", s, re.M):
        dau = m.end()
        m2 = re.search(r"^\[", s[dau:], re.M)
        cuoi = dau + m2.start() if m2 else len(s)
        than = s[dau:cuoi]
        mk = re.search(r"^" + re.escape(khoa) + r"=.*$", than, re.M)
        if mk:
            than = than[:mk.start()] + "%s=%s" % (khoa, gia_tri) + than[mk.end():]
        else:
            than_r = than.rstrip("\r\n")
            than = than_r + "\r\n%s=%s" % (khoa, gia_tri) + than[len(than_r):]
        ra.append(s[pos:dau] + than)
        pos, n = cuoi, n + 1
    ra.append(s[pos:])
    if n == 0:
        raise SystemExit("khong co muc [%s]" % muc)
    return "".join(ra)


def dat_vitri(s, muc, o):
    s = dat_khoa(s, muc, "Left", o[0] - DX)
    s = dat_khoa(s, muc, "Top", o[1] - DY)
    if len(o) == 4:
        s = dat_khoa(s, muc, "Width", o[2])
        s = dat_khoa(s, muc, "Height", o[3])
    return s


def ghi_moi_noi(duong_tuong_doi, du_lieu_bytes):
    for goc in [GHI_DE] + ([DU_LIEU] if os.path.isdir(DU_LIEU) else []):
        p = os.path.join(goc, duong_tuong_doi)
        os.makedirs(os.path.dirname(p), exist_ok=True)
        io.open(p, "wb").write(du_lieu_bytes)
        print("  ghi", p)


def main():
    k = float(sys.argv[1]) if len(sys.argv) > 1 else 0.5
    bc = bo_cuc(k)
    # ---- khung .spr
    tmp = os.path.join(GOC, "_khung_tmp.spr")
    ghi_spr_tu_anh(anh_khung(k), tmp)
    ok, msg = kiem_spr(tmp, bat_buoc=True)
    print(msg)
    ghi_moi_noi(os.path.join("spr", "uinew", "uiplayerbar", "khung_chat_mobile.spr"), io.open(tmp, "rb").read())
    os.remove(tmp)
    # ---- uiplayerbar.ini (tu ban goc cua cay client PC)
    s = io.open(os.path.join(NGUON_PC, "uiplayerbar.ini"), encoding="latin-1", newline="").read()
    s = dat_khoa(s, "Main", "Image", r"\Spr\UiNew\UiPlayerBar\khung_chat_mobile.spr")
    s = dat_khoa(s, "Main", "Image1024", r"\Spr\UiNew\UiPlayerBar\khung_chat_mobile.spr")
    s = s.replace("; Giao dien man hinh\r\n", "; Giao dien man hinh\r\n; [ANDROID 10/09] Khung Main = khung_chat_new cua VNKU "
                  "thu nho %.2f (khung_chat_mobile.spr) goc trai duoi; icon giu nguyen, dat lai vi tri "
                  "(android\\thanh_duoi_mobile.py).\r\n" % k, 1)
    for muc in ["Status", "Items", "ItemEx", "Skills", "Team", "Faction", "Friend", "Options", "AutoPlay", "HideChat",
                "ChannelBtn", "Face", "SendBtn", "ImediaLeftSkill", "ImediaRightSkill", "InputEdit"] + \
               ["Item_%d" % i for i in range(9)]:
        s = dat_vitri(s, muc, bc[muc])
    ghi_moi_noi(os.path.join("ui", "ui3", "uiplayerbar.ini"), s.encode("latin-1"))
    # ---- uitoolscontrolbar.ini
    s = io.open(os.path.join(NGUON_PC, "uitoolscontrolbar.ini"), encoding="latin-1", newline="").read()
    s = s.replace("; Ui Designed Muaroilangtham\r\n", "; Ui Designed Muaroilangtham\r\n; [ANDROID 10/09] Main ve goc 0,0; "
                  "sau nut cong cu xep tiep hang icon chuc nang ngay tren khung thanh duoi (android\\thanh_duoi_mobile.py).\r\n", 1)
    for khoa, gt in [("Left", 0), ("Top", 0), ("Width", SW), ("Height", SH)]:
        s = dat_khoa(s, "Main", khoa, gt)
    for muc in ["Run", "Sit", "Horse", "Exchange", "PK", "Rec"]:
        s = dat_vitri(s, muc, bc[muc])
    ghi_moi_noi(os.path.join("ui", "ui3", "uitoolscontrolbar.ini"), s.encode("latin-1"))
    L, T, w, h = bc["khung"]
    print("xong: k=%.2f, khung %dx%d tai (%d,%d); hang icon y=%d; vat pham tu x=%d y=%d" % (
        k, w, h, L, T, bc["Status"][1], bc["Item_0"][0], bc["Item_0"][1]))


if __name__ == "__main__":
    main()
