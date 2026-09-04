# -*- coding: utf-8 -*-
r"""do_wauto_bo_cuc.py - DO KIEM BO CUC WAuto: nhan nao BI CAT, o nao CHONG NHAU.

Vi sao can: moi toa do trong WAuto.rc la DIALOG UNIT, ma 1 dlu = f(font hop thoai).
Doi dong FONT la doi het kich thuoc THAT cua ca 499 dieu khien:
    Segoe UI 9          -> base unit 7 x 15 px  (1 dlu ngang = 7/4 px, doc = 15/8 px)
    Microsoft Sans 8    -> base unit 6 x 13 px  (ngang 6/4, doc 13/8)
=> doi tu Segoe UI 9 sang MS Sans 8 lam ca giao dien CO LAI: ngang 86 %, doc 87 %.
Do la loi "bop kich thuoc lam mat chu lech cac thanh phan" chu game bao 04/09.

Script nay doc thang WAuto.rc, quy doi tung o ra PIXEL bang dung base unit cua font
dang khai bao, roi DO BE RONG CHU THAT bang GDI de biet nhan nao khong vua o.
Cung do luon cac o CHONG LEN NHAU trong cung mot tab.

Chay: python do_wauto_bo_cuc.py [duong_dan_WAuto.rc]
"""
import ctypes as C
import io
import os
import re
import sys
from ctypes import wintypes as W

RC = sys.argv[1] if len(sys.argv) > 1 else r"E:\Src_Auto_Ngoai\WAuto\WAuto\WAuto.rc"

g32 = C.WinDLL("gdi32", use_last_error=True)
u32 = C.WinDLL("user32", use_last_error=True)

VARIABLE_PITCH, FF_SWISS, OUT_TT_PRECIS = 2, 0x20, 4
VIETNAMESE_CHARSET, CLEARTYPE_QUALITY, FW_NORMAL = 163, 5, 400
DT_CALCRECT, DT_SINGLELINE, DT_NOPREFIX = 0x400, 32, 0x800


class LOGFONTW(C.Structure):
    _fields_ = [("lfHeight", C.c_long), ("lfWidth", C.c_long), ("lfEscapement", C.c_long),
                ("lfOrientation", C.c_long), ("lfWeight", C.c_long), ("lfItalic", C.c_byte),
                ("lfUnderline", C.c_byte), ("lfStrikeOut", C.c_byte), ("lfCharSet", C.c_byte),
                ("lfOutPrecision", C.c_byte), ("lfClipPrecision", C.c_byte),
                ("lfQuality", C.c_byte), ("lfPitchAndFamily", C.c_byte),
                ("lfFaceName", C.c_wchar * 32)]


class TEXTMETRICW(C.Structure):
    _fields_ = [("tmHeight", C.c_long), ("tmAscent", C.c_long), ("tmDescent", C.c_long),
                ("tmInternalLeading", C.c_long), ("tmExternalLeading", C.c_long),
                ("tmAveCharWidth", C.c_long), ("tmMaxCharWidth", C.c_long),
                ("tmWeight", C.c_long), ("tmOverhang", C.c_long),
                ("tmDigitizedAspectX", C.c_long), ("tmDigitizedAspectY", C.c_long),
                ("tmFirstChar", C.c_wchar), ("tmLastChar", C.c_wchar),
                ("tmDefaultChar", C.c_wchar), ("tmBreakChar", C.c_wchar),
                ("tmItalic", C.c_byte), ("tmUnderlined", C.c_byte), ("tmStruckOut", C.c_byte),
                ("tmPitchAndFamily", C.c_byte), ("tmCharSet", C.c_byte)]


class Do(object):
    """Do chu bang dung font hop thoai, va tinh base unit nhu Windows tinh."""

    def __init__(self, ten, pt):
        self.hdcS = u32.GetDC(0)
        self.hdc = g32.CreateCompatibleDC(self.hdcS)
        lf = LOGFONTW()
        lf.lfHeight = -(pt * 96 // 72)
        lf.lfWeight = FW_NORMAL
        lf.lfCharSet = VIETNAMESE_CHARSET
        lf.lfQuality = CLEARTYPE_QUALITY
        lf.lfOutPrecision = OUT_TT_PRECIS
        lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS
        lf.lfFaceName = ten
        self.hf = g32.CreateFontIndirectW(C.byref(lf))
        g32.SelectObject(self.hdc, self.hf)
        tm = TEXTMETRICW()
        g32.GetTextMetricsW(self.hdc, C.byref(tm))
        self.tm = tm
        # base unit: Windows lay be rong TRUNG BINH cua 52 chu cai (MapDialogRect),
        # KHONG phai tmAveCharWidth - do dung cach cua tai lieu MSDN.
        abc = u"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        r = W.RECT(0, 0, 0, 0)
        u32.DrawTextW(self.hdc, abc, -1, C.byref(r), DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX)
        self.baseX = (r.right + 26) // 52
        self.baseY = tm.tmHeight
        self.ten, self.pt = ten, pt

    def rong(self, t):
        r = W.RECT(0, 0, 0, 0)
        u32.DrawTextW(self.hdc, t, -1, C.byref(r), DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX)
        return r.right

    def px_x(self, dlu):
        return dlu * self.baseX // 4

    def px_y(self, dlu):
        return dlu * self.baseY // 8


# --------------------------------------------------------------- doc WAuto.rc
def doc_rc(p):
    s = io.open(p, encoding="utf-16", newline="").read().replace("\r\n", "\n")
    font = re.search(r'^FONT\s+(\d+)\s*,\s*"([^"]+)"', s, re.M)
    pt, ten = (int(font.group(1)), font.group(2)) if font else (9, "Segoe UI")
    # gop dong noi tiep (dieu khien viet tren 2-3 dong)
    dong = []
    tam = ""
    for l in s.split("\n"):
        t = l.strip()
        if not t:
            continue
        tam = (tam + " " + t).strip() if tam else t
        if tam.rstrip().endswith(","):
            continue
        dong.append(tam)
        tam = ""
    ds = []
    hop = ""          # ten hop thoai dang doc - o cua HAI hop thoai khac nhau
    for l in dong:    # thi KHONG THE chong nhau, phai tach ra
        m = re.match(r'^(\w+)\s+DIALOGEX', l)
        if m:
            hop = m.group(1)
            continue
        m = re.match(r'^(LTEXT|RTEXT|CTEXT|PUSHBUTTON|DEFPUSHBUTTON|GROUPBOX)\s+"(.*?)"\s*,\s*'
                     r'([A-Za-z_0-9]+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)', l)
        if m:
            ds.append((hop, m.group(1), m.group(2), m.group(3),
                       int(m.group(4)), int(m.group(5)), int(m.group(6)), int(m.group(7))))
            continue
        m = re.match(r'^CONTROL\s+"(.*?)"\s*,\s*([A-Za-z_0-9]+)\s*,\s*"(\w+)"\s*,(.*?),\s*'
                     r'(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*$', l)
        if m:
            # PHAI phan biet O TICH (mat ~16 px cho o vuong + khoang cach) voi NUT BAM
            # ve tay (BS_OWNERDRAW - chu chiem tron nut, chi mat ~8 px dem hai ben).
            co = m.group(4)
            loai = "CONTROL/" + m.group(3)
            if "AUTOCHECKBOX" in co or "AUTORADIOBUTTON" in co or "AUTO3STATE" in co:
                loai = "OTICH"
            elif "OWNERDRAW" in co or "PUSHBUTTON" in co:
                loai = "NUT"
            ds.append((hop, loai, m.group(1), m.group(2),
                       int(m.group(5)), int(m.group(6)), int(m.group(7)), int(m.group(8))))
            continue
        m = re.match(r'^(EDITTEXT|COMBOBOX)\s+([A-Za-z_0-9]+)\s*,\s*'
                     r'(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)', l)
        if m:
            ds.append((hop, m.group(1), "", m.group(2),
                       int(m.group(3)), int(m.group(4)), int(m.group(5)), int(m.group(6))))
    return ten, pt, ds


def main():
    ten, pt, ds = doc_rc(RC)
    d = Do(ten, pt)
    print('WAuto.rc dang khai bao FONT %d, "%s"' % (pt, ten))
    print("base unit do duoc: %d x %d px  (1 dlu ngang = %.2f px, doc = %.2f px)"
          % (d.baseX, d.baseY, d.baseX / 4.0, d.baseY / 8.0))
    print("doc %d dieu khien co toa do\n" % len(ds))

    # ---- so sanh hai font de thay muc do CO LAI ----
    d2 = Do("Microsoft Sans Serif", 8)
    print("DOI CHIEU: Microsoft Sans Serif 8 -> base %d x %d  => ngang %.0f %%, doc %.0f %% so voi %s %d"
          % (d2.baseX, d2.baseY, 100.0 * d2.baseX / d.baseX, 100.0 * d2.baseY / d.baseY, ten, pt))
    print("   (hop thoai chinh 160 x 430 dlu = %d x %d px voi %s, con %d x %d px voi MS Sans 8)\n"
          % (d.px_x(160), d.px_y(430), ten, d2.px_x(160), d2.px_y(430)))

    # ---- nhan nao khong vua o ----
    dem = 0
    print("=== NHAN BI CAT (be rong chu > be rong o) ===")
    for hop, kind, nhan, idc, x, y, w, h in ds:
        if not nhan or kind.startswith("EDITTEXT") or kind.startswith("COMBOBOX"):
            continue
        rong_px = d.px_x(w)
        chu_px = d.rong(nhan)
        if kind == "OTICH":
            chu_px += 16        # o vuong 13 px + 3 px khoang cach
        elif kind in ("NUT", "PUSHBUTTON", "DEFPUSHBUTTON"):
            chu_px += 8         # dem hai ben trong nut
        elif kind == "GROUPBOX":
            chu_px += 12        # vien + khoang ho hai ben tieu de
        if chu_px > rong_px:
            dem += 1
            print("  %-22s %-30s o %3d dlu = %3d px < chu %3d px  (thieu %d px)"
                  % (idc, '"' + nhan[:28] + '"', w, rong_px, chu_px, chu_px - rong_px))
    print("  -> %d nhan bi cat\n" % dem)

    # ---- KHONG do "o chong nhau" ----
    # WAuto co MOT hop thoai duy nhat, 17 tab deu nam CHONG LEN NHAU o cung vung
    # toa do roi an/hien theo dai ID (ShowTab). Chong nhau o day la CO Y, do se ra
    # hang nghin cap vo nghia. Muon do that thi phai biet dieu khien nao thuoc tab
    # nao - thong tin do nam trong ShowTab chu khong co trong WAuto.rc.
    print("(bo qua phep do chong nhau: 17 tab von xep chong roi an/hien theo dai ID)")


if __name__ == "__main__":
    main()
