# -*- coding: utf-8 -*-
r"""v26 - GOI XEM TRANG BI: CAP THUOC TINH TRUYEN BANG int, DUNG NHU BAN LINUX.

DO DUOC TU NHI PHAN (D:\ServerLinux\jx_linux_y, ELF32 EM_386):

  Trong CUNG MOT HAM dong goi 0x081F9430:
      0x081F9485  88 48 0b                     mov byte  [eax+0x0b], cl
                                               <- nLuck, 1 byte
      0x081F9488  8b 8a e4 01 00 00 / 89 48 0c mov dword [eax+0x0c], ecx
                                               <- nGeneratorLevel[0], 4 byte
                  ... lap lai cho [1]..[5], stride 4
  Cung trinh bien dich, cung ham: mot truong ra opcode 88 (MOV r/m8), sau truong
  kia ra 89 (MOV r/m32). Do la chon THEO KIEU TRUONG NGUON, khong phai mac dinh.

  Quet toan bo doan ma (~830.000 lenh): moi truy cap vao +0x1E0..+0x200 deu RONG
  4 BYTE - 0 luot 1 byte, 0 luot 2 byte. Khong co duong thu hep nao.

  Vi sao ban goc phai dung int: magicattriblevel_index.txt co 48 nhom trai kin
  1..480, tuc khong gian gia tri vuot 255 la thiet ke goc. Ban Linux KHONG dung
  co che bu (khong tach MAGIC_ID + LEVEL, khong de client tu tra bang) - no
  truyen thang chi so phang.

BAN JX1 dang cat xuong 1 byte:
  KProtocol.h:2182   BYTE m_btMagicLevel[MAX_ITEM_MAGICLEVEL];
  KPlayer.cpp:6533   ep (BYTE)
=> khoang mot nua so thuoc tinh sai khi nguoi khac soi do tim cua minh.

DA KIEM TRUOC KHI SUA:
  * #pragma pack(1) (KProtocol.h:18-19) - khong co dem, kich thuoc la chinh xac
  * KProtocol.cpp:115 khai sizeof(VIEW_EQUIP_SYNC) -> bang kich thuoc TU CAP NHAT
  * ben mo goi doc vao mang int trung gian, khong phai sua:
        KViewItem.cpp:92   nMagicParam[j] = pView->m_sInfo[i].m_btMagicLevel[j];
  * chi ba noi dung VIEW_EQUIP_SYNC: KPlayer (gui), KViewItem (nhan), va
    MultiServer\GatewayRobot\Player.cpp:146 - nhung GatewayRobot KHONG co trong
    JXAll.sln nen khong duoc build
  * CoreShell.cpp:2337-2345 dung mot struct KHAC (GameDataDef.h:596
    short m_btMagicLevel[16], chua duoc 480) - khong lien quan

*** DAY LA DOI GOI TIN. May chu va client PHAI thay cung mot luc. ***
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CR = chr(13)
NL = chr(10)
TAB = chr(9)
SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def dem_dong(t):
    crlf = t.count(CR + NL)
    return crlf, t.count(NL) - crlf


def theo_eol(t, s):
    crlf, lf = dem_dong(t)
    s = s.replace(CR + NL, NL)
    if crlf > lf:
        s = s.replace(NL, CR + NL)
    return s


def khoi(*d):
    return "".join(x + NL for x in d)


CAP = [
    ("KProtocol.h", "SViewItemInfo dung int",
     TAB + "BYTE" + TAB + "m_btMagicLevel[MAX_ITEM_MAGICLEVEL];",
     TAB + "// [LOREN] Ban Linux truyen truong nay bang int 32-bit: trong cung ham" + NL +
     TAB + "// dong goi 0x081F9430, nLuck ra opcode 88 (MOV r/m8) con sau o cap" + NL +
     TAB + "// thuoc tinh ra 89 (MOV r/m32), stride 4. Quet ca doan ma: moi truy" + NL +
     TAB + "// cap vao +0x1E0..+0x200 deu rong 4 byte, khong luot nao 1 hay 2 byte." + NL +
     TAB + "// Bat buoc phai vay: magicattriblevel_index.txt trai kin 1..480." + NL +
     TAB + "int" + TAB + TAB + "m_btMagicLevel[MAX_ITEM_MAGICLEVEL];"),

    ("KPlayer.cpp", "bo ep (BYTE)",
     khoi(TAB * 3 + "pInfo->m_btMagicLevel[j] = (BYTE)Item[nIdx].m_GeneratorParam.nGeneratorLevel[j];"),
     khoi(TAB * 3 + "// [LOREN] khong ep (BYTE) nua - chi so dong chay toi 480",
          TAB * 3 + "pInfo->m_btMagicLevel[j] = Item[nIdx].m_GeneratorParam.nGeneratorLevel[j];")),
]


def main():
    print("GOI XEM TRANG BI: CAP THUOC TINH BANG int")
    print("=" * 78)
    loi = 0
    for ten, nhan, tim, thay in CAP:
        p = os.path.join(SRC, ten)
        goc = doc(p)
        a = theo_eol(goc, tim)
        b = theo_eol(goc, thay)
        if b in goc:
            print("  BO QUA  %-16s %s" % (ten, nhan))
            continue
        n = goc.count(a)
        if n != 1:
            print("  *** LOI %-16s %s: moc neo %d lan" % (ten, nhan, n))
            loi += 1
            continue
        t = goc.replace(a, b, 1)
        c0, l0 = dem_dong(goc)
        c1, l1 = dem_dong(t)
        if min(c1, l1) > min(c0, l0):
            print("  *** LOI %-16s tang loai xuong dong thieu so" % ten)
            loi += 1
            continue
        if not os.path.isfile(p + ".truoc_loren"):
            io.open(p + ".truoc_loren", "wb").write(io.open(p, "rb").read())
        io.open(p, "wb").write(t.encode("latin-1"))
        print("  OK      %-16s %s (CRLF %d->%d)" % (ten, nhan, c0, c1))
    print("=" * 78)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    if loi == 0:
        print("  LUU Y: day la DOI GOI TIN - may chu va client phai thay cung luc.")
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
