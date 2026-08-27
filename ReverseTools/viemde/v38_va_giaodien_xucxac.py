# -*- coding: utf-8 -*-
r"""v38 - va hai loi giao dien xuc xac lo ra khi chu game mo thu cua so.

LOI 1: DONG HO KHONG HIEN.
    Muc [TimeInfo] cua "\xCD\xB6\xC9\xAB\xD7\xD3.ini" chi co Left/Top/Text -
    KHONG co Width, Height, Font, Color. KWndText::Init doc tu do ra mot o
    RONG 0x0 va mau den => khong ve gi ca. Phai tu dat kich thuoc + mau sau Init.

LOI 2: NHAN NUT BI CAT ("Tham du .." / "Huy bo n..").
    Nut rong 63 px, Font=12. Nhan "Tham du nhan" / "Huy bo nhan" dai gap doi cho.
    Ban goc co san HAI muc nut CHONG NHAU o cung cho:
        [BtnNeed]  x=97  Label="Can"          <- 3 chu, VUA khung
        [BtnGreed] x=100 Label="Tham du nhan" <- qua dai
    Tuc ban goc du dinh dung [BtnNeed] cho khung 3 chu. Doi sang [BtnNeed], va
    dat lai nhan nut kia thanh "Bo qua" cho vua.

Ca hai nhan deu la byte TCVN3:  "Can" = C\xC7n  ,  "Bo qua" = B\xE1 qua
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

P = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiDiceItem.cpp"
HAU_TO = ".truoc_vagd_2608"

VA = [
    # --- LOI 2: dung [BtnNeed] thay [BtnGreed]
    ('            ms_pSelf->m_BtnNeed[i].Init(&IniRow, "BtnGreed");    // "Tham du nhan"\r\n'
     '            ms_pSelf->m_BtnGiveUp[i].Init(&IniRow, "BtnClose");  // "Huy bo nhan"\r\n',

     '            // Ban goc co HAI muc nut chong nhau o cung cho: [BtnNeed] nhan "Can"\r\n'
     '            // (3 chu, vua khung 63px) va [BtnGreed] nhan "Tham du nhan" (dai gap\r\n'
     '            // doi, bi cat cut). Anh nen "3 chu" nen [BtnNeed] moi la y do goc.\r\n'
     '            ms_pSelf->m_BtnNeed[i].Init(&IniRow, "BtnNeed");     // nhan "Can"\r\n'
     '            ms_pSelf->m_BtnGiveUp[i].Init(&IniRow, "BtnClose");\r\n'
     '            // "Huy bo nhan" cung qua dai cho khung 63px -> rut con "Bo qua".\r\n'
     '            // Chuoi la byte TCVN3 (B\\xE1 qua), giong moi tep giao dien khac.\r\n'
     '            ms_pSelf->m_BtnGiveUp[i].SetLabel("B\\xE1 qua");\r\n',
     "dung [BtnNeed] + rut gon nhan nut huy"),

    # --- LOI 1: dat kich thuoc + mau cho dong ho
    ('            ms_pSelf->m_Time[i].Init(&IniRow, "TimeInfo");\r\n',

     '            ms_pSelf->m_Time[i].Init(&IniRow, "TimeInfo");\r\n'
     '            // [TimeInfo] trong .ini CHI co Left/Top/Text - khong Width/Height/\r\n'
     '            // Color, nen sau Init o chu rong 0x0 va mau den => KHONG VE GI.\r\n'
     '            // Phai tu dat. O bat dau tai x=80 trong hang rong 312 nen con 224px.\r\n'
     '            ms_pSelf->m_Time[i].SetSize(224, 16);\r\n'
     '            ms_pSelf->m_Time[i].SetTextColor(0x00FFFCB2);   // vang nhat, y nhu nut\r\n',
     "dat kich thuoc + mau cho dong ho dem nguoc"),
]


def main():
    d = io.open(P, "rb").read().decode("latin-1")

    for cu, moi, nhan in VA:
        if moi in d:
            print("   = da co san:", nhan)
            continue
        if d.count(cu) != 1:
            print("!! MO NEO khong duy nhat (%d lan): %s" % (d.count(cu), nhan))
            return 2

    for cu, moi, nhan in VA:
        if moi in d:
            continue
        d = d.replace(cu, moi, 1)
        print("   > " + nhan)

    b = d.encode("latin-1")
    if b.count(b"\n") - b.count(b"\r\n"):
        print("!! sinh ra dong LF le")
        return 2
    bak = P + HAU_TO
    if not os.path.isfile(bak):
        shutil.copy2(P, bak)
    tmp = P + ".tmp"
    io.open(tmp, "wb").write(b)
    os.replace(tmp, P)

    print()
    d = io.open(P, "rb").read().decode("latin-1")
    print('   dung "BtnNeed"     : %s' % ("CO" if '"BtnNeed"' in d else "KHONG"))
    print("   SetLabel nut huy   : %s" % ("CO" if "SetLabel(" in d else "KHONG"))
    print("   SetSize dong ho    : %s" % ("CO" if "SetSize(224, 16)" in d else "KHONG"))
    print("   SetTextColor       : %s" % ("CO" if "SetTextColor(" in d else "KHONG"))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
