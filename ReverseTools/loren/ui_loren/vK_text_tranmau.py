# -*- coding: utf-8 -*-
"""vK_text_tranmau.py - CHAN TRAN bo dem ten mau trong TEncodeCtrl (Engine\\Src\\Text.cpp).

GOC SAP "re chuot vao khoang thuoc tinh = dis client" (da truy den cung, co so do):

  KBASICPROP_MAGICSCRIPT::m_szIntro la char[SZBUFLEN_1] = 128 byte
  (KBasPropTbl.h:14 + :329). Intro cua 6 vien khoang 199..204:
      ban LINUX goc : 186..191 byte
      ban JX1       : 164..183 byte
  Ca hai deu DAI HON 128 => KTabFile::GetValue (KTabFile.cpp:475-479) cat con
  127 byte, va cho cat roi DUNG GIUA mot the "<color=...>".
  Vi du Phu Dung Thach: "...thay doi <color=yell" - the HO.

  Khi ve mo ta, TEncodeCtrl gap "<color=" roi DI TIM dau ">". Vong tim chi chan
  bang do dai CA chu giai (Text.cpp:623-627) nen no bat duoc dau ">" cua the
  " <color=Fire>" ma KItem.cpp:1507 noi vao phia sau => nParamLen ~ 21.
  Roi Text.cpp:700-701:
        static char Color[13];
        memcpy(Color, pParamBuffer, nParamLen);   // KHONG chan
        Color[nParamLen] = 0;                     // ghi o chi so 21
  => ghi de 9 byte ra ngoai. Dung bay da ghi trong so tay du an:
  "ten mau >= 13 ky tu lam hong bo nho".

  Nhanh o tren (Text.cpp:679 `else if (nParamLen < 8)`) CO chan, nhung khi ten
  mau khong khop bang mau thi ma roi thang xuong dong 700 KHONG chan.

MIENG VA: kep nParamLen ve vua bo dem TRUOC moi memcpy. Ba cho cung mot loi:
    1. KTC_INLINE_PIC  (szPic[MAXPICTOKENLEN])  - chot o dong 659 viet
       `nParamLen == 0 && nParamLen >= MAXPICTOKENLEN` (`&&` le ra la `||`) nen
       KHONG BAO GIO chan; giu nguyen dieu kien do (sua no la doi hanh vi), chi
       them kep truoc memcpy.
    2. KTC_COLOR        (Color[13])
    3. KTC_BORDER_COLOR (Color[13])

Day la lo hong an toan bo nho THAT: client khong duoc sap vi mot chuoi la, du
chuoi do tu bang du lieu nao toi. Giu NGUYEN du lieu goc, chi sua engine.

Text.cpp bien dich vao Engine.vcxproj (DynamicLibrary). Engine.dll dang chay la
ban "Release|Win32" (da doi chieu bam voi bin\\client\\Engine.dll).

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_tranmau lan dau).
"""
import io
import os
import shutil
import sys

DICH = r"D:\GAMEDEVNEW\Sources\Engine\Src\Text.cpp"
HAU_TO = ".truoc_tranmau"

KEP_PIC = (
    "\t\t\t// [LOREN 27/08] chot o dong tren viet `&&` (le ra `||`) nen khong bao\n"
    "\t\t\t// gio chan; kep o day cho chac, khong doi dieu kien cu.\n"
    "\t\t\tif (nParamLen > (int)sizeof(szPic) - 1)\n"
    "\t\t\t\tnParamLen = (int)sizeof(szPic) - 1;\n"
)

KEP_COLOR = (
    "\t\t// [LOREN 27/08] CHAN TRAN: nhanh `nParamLen < 8` o tren chi chan khi ten\n"
    "\t\t// mau khop bang; khi khong khop ma roi thang xuong day. Mot chuoi bi cat\n"
    "\t\t// cut giua the (vd Intro vat pham dai hon 127 byte) lam vong tim '>' bat\n"
    "\t\t// nham dau '>' cua the phia sau => nParamLen co the ~21 > Color[13].\n"
    "\t\tif (nParamLen > (int)sizeof(Color) - 1)\n"
    "\t\t\tnParamLen = (int)sizeof(Color) - 1;\n"
)

MIENG = [
    ("M1 KTC_INLINE_PIC (szPic)",
     "\t\t{\n"
     "\t\t\tmemcpy(szPic, pParamBuffer, nParamLen);\n"
     "\t\t\tszPic[nParamLen] = 0;\n",
     "\t\t{\n" + KEP_PIC +
     "\t\t\tmemcpy(szPic, pParamBuffer, nParamLen);\n"
     "\t\t\tszPic[nParamLen] = 0;\n"),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vK_text_tranmau - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi_truoc = sum(1 for c in raw if ord(c) > 127)

    if "[LOREN 27/08] CHAN TRAN" in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    moi = raw

    # --- M1: szPic ---
    for ten, cu, thay in MIENG:
        cu_e = cu.replace("\n", eol)
        thay_e = thay.replace("\n", eol)
        if moi.count(cu_e) != 1:
            print("!!! LOI TO: %s - moc neo xuat hien %d lan (can 1)" % (ten, moi.count(cu_e)))
            return 1
        moi = moi.replace(cu_e, thay_e, 1)
        print("  %s: moc neo trung 1 lan" % ten)

    # --- M2 + M3: hai cho memcpy(Color, ...) khong chan ---
    neo_color = ("\t\tmemcpy(Color, pParamBuffer, nParamLen);\n"
                 "\t\tColor[nParamLen] = 0;\n").replace("\n", eol)
    dem = moi.count(neo_color)
    if dem != 2:
        print("!!! LOI TO: cho memcpy(Color) khong chan xuat hien %d lan (can dung 2:"
              " KTC_COLOR va KTC_BORDER_COLOR)" % dem)
        return 1
    moi = moi.replace(neo_color, KEP_COLOR.replace("\n", eol) + neo_color, 2)
    print("  M2+M3 KTC_COLOR / KTC_BORDER_COLOR: moc neo trung dung 2 lan")

    hi_sau = sum(1 for c in moi if ord(c) > 127)
    if hi_sau != hi_truoc:
        print("!!! LOI TO: byte cao %d -> %d" % (hi_truoc, hi_sau))
        return 1
    print("  byte cao %d (khong doi) | CRLF %d -> %d"
          % (hi_truoc, raw.count("\r\n"), moi.count("\r\n")))

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % sao)
    with io.open(DICH, "wb") as f:
        f.write(moi.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != moi:
        print("!!! LOI TO: doc lai tu dia KHONG khop")
        return 1
    print("  DA GHI + doc lai tu dia: khop.")
    print("\n  => build Engine.vcxproj cau hinh \"Release|Win32\" (dung ban dang chay)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
