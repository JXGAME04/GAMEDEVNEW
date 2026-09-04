# -*- coding: utf-8 -*-
r"""nan_wauto_dlu.py - NAN TOAN BO TOA DO WAuto.rc khi DOI FONT HOP THOAI.

Chu game 04/09 duyet: muon font giong auto Thai (Microsoft Sans Serif 8) MA GIAO DIEN
KHONG BI CO LAI.

Vi sao phai nan: moi toa do trong .rc la DIALOG UNIT, ma
    1 dlu ngang = baseX / 4 px      1 dlu doc = baseY / 8 px
voi baseX/baseY do tu chinh font cua hop thoai:
    Segoe UI 9         -> 7 x 15 px
    Microsoft Sans 8   -> 6 x 13 px
Doi font khong nan = ca giao dien co lai 86 % ngang, 87 % doc (loi dot 7).
Nan bu: dlu_moi = dlu_cu * 7/6 (ngang), * 15/13 (doc) -> so PIXEL giu nguyen.

CACH LAM TRON - QUAN TRONG:
KHONG nan rieng x va w. Neu lam the, hai o LIEN KE (x1 + w1 == x2) se bi ho hoac chong
nhau 1 dlu sau khi lam tron. Thay vao do nan BIEN: x_moi = round(x * ty le),
phai_moi = round((x + w) * ty le), roi w_moi = phai_moi - x_moi. Nhu vay moi bien chung
cua hai o canh nhau deu ra CUNG mot gia tri -> lien ke duoc giu nguyen tuyet doi.

Chay:
    python nan_wauto_dlu.py --thu     # chi in, khong ghi
    python nan_wauto_dlu.py           # ghi that (co sao luu .truoc_nan)
"""
import io
import os
import re
import shutil
import sys

RC = r"E:\Src_Auto_Ngoai\WAuto\WAuto\WAuto.rc"
MIRROR = r"D:\GAMEDEVNEW\WAutoUI\WAuto.rc"
THU = "--thu" in sys.argv

FONT_CU = ('9', "Segoe UI")
FONT_MOI = ('8', "Microsoft Sans Serif")
TX = 7.0 / 6.0      # ngang: base 7 -> 6
TY = 15.0 / 13.0    # doc:   base 15 -> 13


def r(v, ty):
    return int(round(v * ty))


def nan_hcn(x, y, w, h):
    """Nan mot hinh chu nhat, giu LIEN KE bang cach nan BIEN."""
    x2 = r(x, TX)
    w2 = r(x + w, TX) - x2
    y2 = r(y, TY)
    h2 = r(y + h, TY) - y2
    return x2, y2, w2, h2


# cac dong co toa do trong .rc, moi kieu mot mau
# nhom cuoi cua moi mau la "x, y, w, h" (COMBOBOX co them chieu cao THA XUONG o vi tri h)
MAU = [
    # LTEXT/RTEXT/CTEXT/PUSHBUTTON/DEFPUSHBUTTON/GROUPBOX "nhan", ID, x, y, w, h
    re.compile(r'^(\s*(?:LTEXT|RTEXT|CTEXT|PUSHBUTTON|DEFPUSHBUTTON|GROUPBOX)\s+".*?"\s*,\s*'
               r'[A-Za-z_0-9]+\s*,\s*)(-?\d+)(\s*,\s*)(-?\d+)(\s*,\s*)(-?\d+)(\s*,\s*)(-?\d+)(.*)$'),
    # ICON/EDITTEXT/COMBOBOX/LISTBOX/SCROLLBAR ID, x, y, w, h
    re.compile(r'^(\s*(?:EDITTEXT|COMBOBOX|LISTBOX|SCROLLBAR|CONTROL_PLACEHOLDER)\s+'
               r'[A-Za-z_0-9]+\s*,\s*)(-?\d+)(\s*,\s*)(-?\d+)(\s*,\s*)(-?\d+)(\s*,\s*)(-?\d+)(.*)$'),
]
# CONTROL "...", ID, "lop", style..., x, y, w, h   (bon so CUOI dong)
MAU_CONTROL = re.compile(r'^(\s*CONTROL\s+.*?,\s*)(-?\d+)(\s*,\s*)(-?\d+)(\s*,\s*)(-?\d+)(\s*,\s*)(-?\d+)(\s*)$')
MAU_DIALOG = re.compile(r'^(\s*[A-Za-z_0-9]+\s+DIALOGEX\s+)(-?\d+)(\s*,\s*)(-?\d+)(\s*,\s*)'
                        r'(-?\d+)(\s*,\s*)(-?\d+)(.*)$')
MAU_FONT = re.compile(r'^(\s*FONT\s+)(\d+)(\s*,\s*")([^"]+)(".*)$')


def main():
    s = io.open(RC, encoding="utf-16", newline="").read()
    # BAY: WAuto.rc co vai dong ket thuc bang LF DON giua bien CRLF - vi du dong 277
    # dinh lien HAI dieu khien (IDC_CHECKBOX_4_LV va IDC_EDITOR_4_LV). Tach theo CRLF
    # thi hai o do thanh MOT dong, regex khong khop -> BO SOT: doi font xong chung co
    # lai trong khi ca giao dien thi khong, thanh ra lech hang chuc pixel.
    # Chuan hoa het ve CRLF (chuan .rc tren Windows) truoc khi nan.
    nl = "\r\n"
    s = s.replace("\r\n", "\n").replace("\n", nl)
    L = s.split(nl)

    if 'FONT %s, "%s"' % FONT_MOI in s:
        print("!! WAuto.rc DA o font %s %s - dung chay lai (se nan hai lan)." % (FONT_MOI[1], FONT_MOI[0]))
        sys.exit(1)

    # gop dong noi tiep: mot dieu khien co the viet tren 2-3 dong, phai gop de doc
    # bon so cuoi, roi tra ve dung so dong cu de khong lam xao tron dinh dang.
    nDK = nCombo = nDlg = nFont = 0
    da_nan = set()      # BAY: buoc gop ben duoi tung nan LAI dong da nan o buoc 1
    i = 0               # (ra gap doi ty le) va nhay qua hai dong khac -> phai danh dau.
    while i < len(L):
        l = L[i]
        m = MAU_FONT.match(l)
        if m:
            L[i] = "%s%s%s%s%s" % (m.group(1), FONT_MOI[0], m.group(3), FONT_MOI[1], m.group(5))
            nFont += 1
            i += 1
            continue
        m = MAU_DIALOG.match(l)
        if m:
            x, y, w, h = (int(m.group(k)) for k in (2, 4, 6, 8))
            x2, y2, w2, h2 = nan_hcn(x, y, w, h)
            L[i] = "%s%d%s%d%s%d%s%d%s" % (m.group(1), x2, m.group(3), y2,
                                           m.group(5), w2, m.group(7), h2, m.group(9))
            nDlg += 1
            print("  hop thoai: %d x %d dlu -> %d x %d dlu" % (w, h, w2, h2))
            i += 1
            continue

        xong = False
        for mau in MAU:
            m = mau.match(l)
            if m:
                x, y, w, h = (int(m.group(k)) for k in (2, 4, 6, 8))
                # COMBOBOX: tham so thu 4 la chieu cao THA XUONG (van la dlu doc) -> nan nhu h
                x2, y2, w2, h2 = nan_hcn(x, y, w, h)
                L[i] = "%s%d%s%d%s%d%s%d%s" % (m.group(1), x2, m.group(3), y2,
                                               m.group(5), w2, m.group(7), h2, m.group(9))
                nDK += 1
                da_nan.add(i)
                if l.lstrip().startswith("COMBOBOX"):
                    nCombo += 1
                xong = True
                break
        if xong:
            i += 1
            continue

        # CONTROL: bon so o CUOI - co the nam o dong nay hoac dong tiep theo
        m = MAU_CONTROL.match(l)
        if m:
            x, y, w, h = (int(m.group(k)) for k in (2, 4, 6, 8))
            x2, y2, w2, h2 = nan_hcn(x, y, w, h)
            L[i] = "%s%d%s%d%s%d%s%d%s" % (m.group(1), x2, m.group(3), y2,
                                           m.group(5), w2, m.group(7), h2, m.group(9))
            nDK += 1
            da_nan.add(i)
        i += 1

    # kiem: con dong CONTROL nao chua duoc nan khong (viet tren nhieu dong)
    sot = []
    for i, l in enumerate(L):
        t = l.strip()
        if t.startswith("CONTROL ") and not MAU_CONTROL.match(l):
            sot.append(i + 1)
    if sot:
        print("  (%d dong CONTROL viet tren nhieu dong - se nan o buoc gop ben duoi)" % len(sot))

    s2 = nl.join(L)

    # ---- buoc gop: xu ly cac CONTROL viet tren nhieu dong ----
    # gom cum dong tu "CONTROL" toi dong ket thuc bang bon so
    L2 = s2.split(nl)
    i = 0
    nGop = 0
    while i < len(L2):
        if (i not in da_nan and L2[i].strip().startswith("CONTROL ")
                and not re.search(r"-?\d+\s*,\s*-?\d+\s*,\s*-?\d+\s*,\s*-?\d+\s*$", L2[i])):
            j = i
            while j + 1 < len(L2):
                j += 1
                if j in da_nan:     # dong nay da nan o buoc 1 - dung lai, dung nan hai lan
                    break
                m = re.match(r'^(.*?)(-?\d+)(\s*,\s*)(-?\d+)(\s*,\s*)(-?\d+)(\s*,\s*)(-?\d+)(\s*)$', L2[j])
                if m and L2[j].strip():
                    x, y, w, h = (int(m.group(k)) for k in (2, 4, 6, 8))
                    x2, y2, w2, h2 = nan_hcn(x, y, w, h)
                    L2[j] = "%s%d%s%d%s%d%s%d%s" % (m.group(1), x2, m.group(3), y2,
                                                    m.group(5), w2, m.group(7), h2, m.group(9))
                    nGop += 1
                    break
                if L2[j].strip().startswith("CONTROL ") or L2[j].strip() in ("BEGIN", "END"):
                    break
            i = j + 1
            continue
        i += 1
    s2 = nl.join(L2)

    print("Da nan: %d dieu khien mot dong (%d combo), %d dieu khien nhieu dong, %d hop thoai, %d dong FONT"
          % (nDK, nCombo, nGop, nDlg, nFont))
    print("Ty le: ngang x %.4f (7/6), doc x %.4f (15/13)" % (TX, TY))

    if THU:
        print("(--thu: khong ghi)")
        return
    if not os.path.exists(RC + ".truoc_nan"):
        shutil.copyfile(RC, RC + ".truoc_nan")
    io.open(RC, "w", encoding="utf-16", newline="").write(s2)
    if os.path.isdir(os.path.dirname(MIRROR)):
        shutil.copyfile(RC, MIRROR)
    print("da ghi WAuto.rc (+ mirror WAutoUI)")


if __name__ == "__main__":
    main()
