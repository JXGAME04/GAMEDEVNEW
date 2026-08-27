# -*- coding: utf-8 -*-
"""v29 - DO 9 CO XEP CHONG VAO magicscript.txt (cot 22..30).

DO DUOC:
  Bang magicscript.txt cua JX1 DA CO SAN dung khuon 30 cot cua ban Linux, ke ca
  chin cot co:
      22 yeu cau cung cap        23 yeu cau cung ngu hanh   24 yeu cau cung may man
      25..30 yeu cau MagicLevel1..6 giong nhau
  Nhung TOAN BO chin cot deu bang 0 - khuon co ma du lieu chua ai dien.
  (Linux: cot22 co 51 dong, cot23 3 dong, cot24 0 dong, cot25..30 = 27/4/4/4/3/3.)

  Ban Linux nap chin co nay tu chinh magicscript.txt. Tep phu magicscript_stack.txt
  chi la lop va de len. Nen cach gon va dung nhat cho JX1 la DIEN THANG vao bang
  chinh, khong can them tep va them bo nap.

  Gia tri lay tu bang phu DA NAN MA (v28): 511/543 dong tra duoc, 32 dong bao loi
  va bi loai. Chi lay CHIN COT CO, KHONG dung cot 21 (max chong) cua Linux vi JX1
  da co co che rieng o cot 13 (nMaxStack) - dung cot 21 se doi cach xep chong cua
  511 vat pham dang song, ngoai pham vi viec lo ren.

  Dat co chi lam viec xep chong CHAT HON, khong bao gio long hon, nen khong the
  lam hong do da co trong tui nguoi choi.

AN TOAN:
  * KHONG doi so cot cua bat ky dong nao (dong nao thieu cot thi bo qua va bao).
    Dong THUA cot lam lech luoi KTabFile -> tung lam sap GameServer 26/08.
  * Giu nguyen byte cua moi o khac.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, "D:/GAMEDEVNEW/ReverseTools/viemde")
import bangtxt

TAB = chr(9)
CR = chr(13)
NL = chr(10)

LNX = "D:/ServerLinux/server1/settings/item/004/magicscript.txt"
PHU = "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/settings/item/magicscript_stack.txt"
RA = "D:/GAMEDEVNEW/ReverseTools/loren/ra/settings/item/magicscript.txt"

CO_TEN = ["cung cap", "cung ngu hanh", "cung may man",
          "ML1", "ML2", "ML3", "ML4", "ML5", "ML6"]


def doc(p):
    t = io.open(p, "rb").read().decode("latin-1")
    return [l.split(TAB) for l in t.replace(CR + NL, NL).split(NL) if l.strip()]


def chuan(s):
    return " ".join(bangtxt.tcvn2uni(s).strip().lower().split())


def main():
    ghi = "--ghi" in sys.argv
    L = doc(LNX)
    J = doc(RA)
    O = doc(PHU)
    print("DO 9 CO XEP CHONG VAO magicscript.txt  %s"
          % ("[GHI THAT]" if ghi else "[DIEN TAP]"))
    print("=" * 86)

    kho = {}
    for i in range(1, len(J)):
        r = J[i]
        if len(r) < 4:
            continue
        kho.setdefault(chuan(r[0]), []).append(i)

    dat = 0
    bo = []
    for r in O[1:]:
        try:
            idx = int(r[0])
        except ValueError:
            continue
        if idx < 1 or idx >= len(L):
            continue
        co = [r[i].strip() if i < len(r) else "0" for i in range(2, 11)]
        if not any(c == "1" for c in co):
            continue                                # dong khong co co: bo qua
        nguon = L[idx]
        ung = kho.get(chuan(nguon[0]), [])
        ung = sorted(ung, key=lambda u: abs(u - idx))
        if not ung:
            bo.append((r[0], "JX1 khong co ten '%s'"
                       % bangtxt.tcvn2uni(nguon[0]).strip()))
            continue
        if len(ung) > 1 and abs(ung[0] - idx) == abs(ung[1] - idx):
            bo.append((r[0], "trung ten, hai ung vien cach deu"))
            continue
        d = ung[0]
        if len(J[d]) != 30:
            bo.append((r[0], "dong JX1 %d chi co %d cot (can 30)" % (d, len(J[d]))))
            continue
        for k in range(9):
            J[d][21 + k] = co[k]
        ten_co = [CO_TEN[k] for k in range(9) if co[k] == "1"]
        print("    dong %-5d %-30s %s/%s/%s  [%s]"
              % (d, bangtxt.tcvn2uni(J[d][0]).strip()[:30],
                 J[d][1], J[d][2], J[d][3], ", ".join(ten_co)))
        dat += 1

    print()
    print("  da dat co cho %d dong" % dat)
    if bo:
        print("  --- BO QUA (bao loi, khong doan bua) ---")
        for a, b in bo:
            print("    chi so %-6s %s" % (a, b))

    # chot an toan: so cot khong duoc doi
    sc = {}
    for r in J:
        sc[len(r)] = sc.get(len(r), 0) + 1
    print("  phan bo so cot sau khi sua: %s" % sorted(sc.items()))
    if max(sc) > len(J[0]):
        print("  *** LOI: co dong THUA cot so voi header - dung lai")
        return 1

    if ghi:
        if not os.path.isfile(RA + ".truoc_co"):
            io.open(RA + ".truoc_co", "wb").write(io.open(RA, "rb").read())
        t = (CR + NL).join(TAB.join(r) for r in J) + CR + NL
        io.open(RA, "wb").write(t.encode("latin-1"))
        print("  DA GHI %s" % RA)
    else:
        print("  (chay lai voi --ghi de ghi that)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
