# -*- coding: utf-8 -*-
"""v35 - CUU CAC BYTE TCVN3/GBK BI CONG CU EDIT (UTF-8) NGHIEN NAT THANH EF BF BD.

TAI NAN: hom nay toi sua KItem.cpp / KItem.h / KInventory.cpp / KCore.cpp bang
cong cu Edit (doc-ghi UTF-8) thay vi cac patcher latin-1. Moi byte TCVN3/GBK
trong 4 tep bi doi thanh U+FFFD (EF BF BD) => moi chuoi tieng Viet ma client ve
(dong khoa, chu giai...) thanh rac. Day dung la bay da ghi so
(jx1m-edit-pha-encoding) ma van giam phai.

CACH CUU: doi chieu voi ban git HEAD (byte sach).
  - dong hien tai KHONG chua FFFD  -> giu nguyen (gom ca sua doi that cua toi
    va cua phien khac).
  - dong hien tai CO FFFD -> tim dong tuong ung ben HEAD bang difflib; neu
    thay FFFD bang mot ky tu bat ky ma khop dong HEAD thi lay dong HEAD.
  - khong khop duoc -> BAO TO, dung lai cho nguoi xem (khong doan).

Ghi bang latin-1, khong dung Edit/Write cua cong cu nua.
"""
import difflib
import io
import re
import subprocess
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

NL = chr(10)
CR = chr(13)
FFFD = chr(0xFFFD)
GOC = r"D:\GAMEDEVNEW"

TEP = [
    "Sources/Core/Src/KItem.cpp",
    "Sources/Core/Src/KItem.h",
    "Sources/Core/Src/KInventory.cpp",
    "Sources/Core/Src/KCore.cpp",
]


def head_bytes(rel):
    r = subprocess.run(["git", "-C", GOC, "show", "HEAD:" + rel],
                       capture_output=True)
    if r.returncode != 0:
        raise RuntimeError("git show that bai: %s" % rel)
    return r.stdout


def khop(dong_fffd, dong_head):
    """Chuoi FFFD lien nhau ung voi >=1 ky tu bat ky (GBK 2 byte -> 2 FFFD,
    TCVN3 1 byte -> 1 FFFD)."""
    DAU = chr(1)
    mau = re.sub("[" + chr(0x80) + "-" + chr(0x10FFF0) + "]+", DAU, dong_fffd)
    mau = "".join(".+?" if c == DAU else re.escape(c) for c in mau)
    return re.fullmatch(mau, dong_head, re.S) is not None


def main():
    ghi = "--ghi" in sys.argv
    print("CUU BYTE FFFD TU GIT HEAD  %s" % ("[GHI THAT]" if ghi else "[DIEN TAP]"))
    print("=" * 84)
    loi = 0
    for rel in TEP:
        p = GOC + "\\" + rel.replace("/", "\\")
        cur_b = io.open(p, "rb").read()
        try:
            cur = cur_b.decode("utf-8")
        except UnicodeDecodeError:
            print("  *** %s khong phai UTF-8 thuan - xem tay" % rel)
            loi += 1
            continue
        head = head_bytes(rel).decode("latin-1")
        A = head.replace(CR + NL, NL).split(NL)          # HEAD (byte sach)
        B = cur.replace(CR + NL, NL).split(NL)           # hien tai (co FFFD)

        sm = difflib.SequenceMatcher(None,
                                     [l.replace(FFFD, "\x00") for l in B],
                                     A, autojunk=False)
        # anh xa dong B -> dong A qua cac khoi equal la khong du (dong FFFD
        # khong bao gio 'equal'); dung get_opcodes va thu khop trong khoi replace
        ra = list(B)
        n_cuu = 0
        n_bo = []
        ops = sm.get_opcodes()
        for tag, b0, b1, a0, a1 in ops:
            if tag in ("equal", "insert"):
                continue
            # khoi replace/delete: thu khop tung dong FFFD voi dong HEAD
            heads = A[a0:a1]
            dung = 0
            for bi in range(b0, b1):
                if all(ord(c) < 128 for c in B[bi]):
                    continue
                tim = None
                for hi in range(dung, len(heads)):
                    if khop(B[bi], heads[hi]):
                        tim = hi
                        break
                if tim is not None:
                    ra[bi] = heads[tim]
                    dung = tim + 1
                    n_cuu += 1
                else:
                    n_bo.append(bi + 1)
        con = sum(1 for l in ra if any(ord(c) >= 0x300 for c in l))
        print("  %-34s cuu %3d dong, CON HONG %d dong %s"
              % (rel.split("/")[-1], n_cuu, con,
                 ("-> dong " + ",".join(str(x) for x in n_bo[:8])) if n_bo else ""))
        if con:
            loi += 1
            continue
        if ghi:
            eol = CR + NL if cur.count(CR + NL) > cur.count(NL) - cur.count(CR + NL) else NL
            out = eol.join(ra)
            io.open(p + ".hong_fffd_2708", "wb").write(cur_b)
            io.open(p, "wb").write(out.encode("latin-1"))
    print("=" * 84)
    if loi:
        print("  CO %d TEP CHUA CUU DUOC HET - KHONG GHI TEP DO" % loi)
        return 1
    print("  %s" % ("DA GHI XONG - kiem lai bang FFFD=0 roi build" if ghi
                    else "dien tap xong, them --ghi de ghi"))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
