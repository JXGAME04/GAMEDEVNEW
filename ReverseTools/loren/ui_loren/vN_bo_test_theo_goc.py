# -*- coding: utf-8 -*-
"""vN_bo_test_theo_goc.py - Bo test lo ren phat nguyen lieu DUNG NHU BAN GOC.

BAN GOC CO SAN MOT NPC THU NGHIEM lam dung viec nay:
  D:\\ServerLinux\\server1\\script\\global\\general\\thunghiem\\trangbitim.lua
    :101-106  menu sau muc: Hien 1 / An 1 / Hien 2 / An 2 / Hien 3 / An 3
    :136-157  phat vat pham:
        AddItem(6,1,149,1,0,0,0)                        -- HIEN 1: CHI 1 vien
        AddItem(6,1,150,1,0,0,0) ... (6,1,150,1,4,0,0)  -- AN 1: 5 vien, series 0..4
        AddItem(6,1,151,1,0,0,0)                        -- HIEN 2: 1 vien
        AddItem(6,1,152,1,0,0,0) ... series 0..4        -- AN 2: 5 vien
        AddItem(6,1,153,1,0,0,0)                        -- HIEN 3: 1 vien
        AddItem(6,1,154,1,0,0,0) ... series 0..4        -- AN 3: 5 vien
  => 3 vien HIEN x1 + 3 vien AN x5 he = 18 vien. Dung y chu game:
     "cac vien thuoc tinh AN phai co he - da do se co loai An la no se co 5 he".
  Ly do: o AN (pos chan) bat buoc ngu hanh vien khoang = ngu hanh trang bi
  (equip_enchase.lua:75 Linux / :67 JX1), nen vien AN phai co du 5 he de chon;
  o HIEN khong rang buoc ngu hanh nen chi can mot vien.

HAI CHO BO TEST CU LAM SAI:
  1. Phat NHAM DAY: phat 199..204 = KHOANG THUOC TINH (da MANG SAN phep).
     Ban goc **khong bao gio phat truc tiep day nay** - da quet ca cay Linux,
     khong mot AddItem/GiveAward/bang roi do nao cap 6/1/200..205. Chung CHI
     sinh ra tu hai cong thuc: magic_distill (rut phep tu trang bi xanh sang
     nguyen khoang) va ore_upgrade (gop 3 vien cung cap).
     Phai phat NGUYEN KHOANG rong: Linux 149..154 -> **JX1 148..153**.
  2. Truyen nLevel = 6..9 => da chua co gi ma da hien "Pham chat thuoc tinh: 6".
     Ban goc truyen nLevel = 1 (`AddItem(...,1,<series>,0,0)` - tham so 4).

BANG MA (Linux -> JX1 deu lech -1; so vi tri pos KHONG doi):
      pos 1 HIEN1  Linux 149 -> JX1 148
      pos 2 AN1    Linux 150 -> JX1 149
      pos 3 HIEN2  Linux 151 -> JX1 150
      pos 4 AN2    Linux 152 -> JX1 151
      pos 5 HIEN3  Linux 153 -> JX1 152
      pos 6 AN3    Linux 154 -> JX1 153
  (magic_distill.lua:38-39 Linux `149<=ptc<=154, pos=ptc-148`;
   JX1 `:38-39` da nan thanh `pos = ptc - 147`.)

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_testgoc lan dau).
"""
import io
import os
import shutil
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\viemde")
from bangtxt import uni2tcvn, tcvn2uni  # noqa: E402

DICH = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\test_loren_admin.lua"
HAU_TO = ".truoc_testgoc"

CHO_PHEP_HOA = set("\u0102\u00c2\u0110\u00ca\u00d4\u01a0\u01af")


def tcvn(u):
    for ch in u:
        if ord(ch) > 127 and ch.isupper() and ch not in CHO_PHEP_HOA:
            raise SystemExit("!!! LOI TO: chu HOA co dau khong ma hoa duoc TCVN3: %r" % ch)
    t = uni2tcvn(u)
    if tcvn2uni(t) != u:
        raise SystemExit("!!! LOI TO: cham vong tron that bai: %r" % u)
    return t


# Khoi cu (da sua o dot truoc) - phat khoang THUOC TINH du 5 he. Thay tron.
NEO = (
    "\t-- Sau loai khoang, moi loai DU CA 5 NGU HANH (Kim Moc Thuy Hoa Tho).\n"
    "\t-- Dung ban goc: da phat ra CHUA CO GI - khong thuoc tinh, khong pham chat -\n"
    "\t-- chi mang ngu hanh. Thuoc tinh chi co khi RUT tu trang bi xanh qua the\n"
    "\t-- Trich lay roi kham sang. Vi vay:\n"
    "\t--   nLevel  = 0  (dong \"Pham chat thuoc tinh\" doc chinh o nay)\n"
    "\t--   nSeries = 1..5 ngu hanh that\n"
    "\t--   nLuck   = 0 va sau o nMagLvl = 0 (chua co thuoc tinh nao)\n"
    "\tfor i = 1, getn( TBKHOANG_TEST ) do\n"
    "\t\tfor he = 1, 5 do\n"
    "\t\t\tif( AddItemEx( nVer, 0, 0, 6, 1, TBKHOANG_TEST[i][1], 0, he, 0, 0 ) > 0 ) then\n"
    "\t\t\t\tnDem = nDem + 1;\n"
    "\t\t\tend\n"
    "\t\tend\n"
    "\tend\n"
)

MOI = (
    "\t-- NGUYEN KHOANG (rong) - chep dung cach ban goc phat:\n"
    "\t--   trangbitim.lua:136-157 (NPC thu nghiem cua ban Linux)\n"
    "\t-- O HIEN (pos le) khong rang buoc ngu hanh -> chi 1 vien.\n"
    "\t-- O AN  (pos chan) BAT BUOC cung ngu hanh voi trang bi\n"
    "\t--   (equip_enchase.lua:67) -> phat du 5 he de co ma chon.\n"
    "\t-- Ban goc dung nLevel = 1, va KHONG truyen o phep nao (da rong).\n"
    "\t-- KHONG phat day 199..204 (khoang DA mang phep): ban goc khong bao gio\n"
    "\t-- phat truc tiep, chung chi sinh ra qua the Trich lay / gop 3 vien.\n"
    "\tfor i = 1, getn( TBNGUYENKHOANG ) do\n"
    "\t\tlocal nPtc = TBNGUYENKHOANG[i][1];\n"
    "\t\tif( TBNGUYENKHOANG[i][3] == 1 ) then\n"
    "\t\t\tfor he = 0, 4 do\n"
    "\t\t\t\tif( AddItemEx( nVer, 0, 0, 6, 1, nPtc, 1, he, 0, 0 ) > 0 ) then\n"
    "\t\t\t\t\tnDem = nDem + 1;\n"
    "\t\t\t\tend\n"
    "\t\t\tend\n"
    "\t\telse\n"
    "\t\t\tif( AddItemEx( nVer, 0, 0, 6, 1, nPtc, 1, 0, 0, 0 ) > 0 ) then\n"
    "\t\t\t\tnDem = nDem + 1;\n"
    "\t\t\tend\n"
    "\t\tend\n"
    "\tend\n"
)

# Bang nguyen khoang moi, dat ngay sau bang TBKHOANG_TEST cu.
NEO_BANG = "TBKHOANG_TEST =\n"
BANG_MOI_MAU = (
    "-- NGUYEN KHOANG (rong) cua JX1 - Linux 149..154 lech -1 sang 148..153.\n"
    "-- Cot 3: 1 = o AN (pos chan, bat buoc cung ngu hanh -> phat du 5 he)\n"
    "--        0 = o HIEN (pos le, khong rang buoc -> 1 vien)\n"
    "TBNGUYENKHOANG =\n"
    "{\n"
    "\t{ 148, \"@T1@\", 0 },\n"
    "\t{ 149, \"@T2@\", 1 },\n"
    "\t{ 150, \"@T3@\", 0 },\n"
    "\t{ 151, \"@T4@\", 1 },\n"
    "\t{ 152, \"@T5@\", 0 },\n"
    "\t{ 153, \"@T6@\", 1 },\n"
    "}\n"
    "\n"
    "TBKHOANG_TEST =\n"
)

TEN = [
    u"Nguy\u00ean Kho\u00e1ng hi\u1ec7n 1",
    u"Nguy\u00ean Th\u1ea1ch \u1ea9n 1",
    u"Nguy\u00ean Kho\u00e1ng hi\u1ec7n 2",
    u"Nguy\u00ean Th\u1ea1ch \u1ea9n 2",
    u"Nguy\u00ean Kho\u00e1ng hi\u1ec7n 3",
    u"Nguy\u00ean Th\u1ea1ch \u1ea9n 3",
]

CAU_CU = tcvn(u"G\u1ed3m Huy\u1ec1n Tinh c\u1ea5p 1 t\u1edbi 10 v\u00e0 s\u00e1u lo\u1ea1i "
              u"kho\u00e1ng tr\u1ed1ng \u0111\u1ee7 5 ng\u0169 h\u00e0nh.")
CAU_MOI = tcvn(u"G\u1ed3m Huy\u1ec1n Tinh c\u1ea5p 1 t\u1edbi 10 v\u00e0 nguy\u00ean kho\u00e1ng "
               u"r\u1ed7ng: \u00f4 hi\u1ec7n 1 vi\u00ean, \u00f4 \u1ea9n \u0111\u1ee7 5 ng\u0169 h\u00e0nh.")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vN_bo_test_theo_goc - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH); return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi_truoc = sum(1 for c in raw if ord(c) > 127)

    if "TBNGUYENKHOANG" in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    bang = BANG_MOI_MAU
    for k, u in enumerate(TEN, 1):
        bang = bang.replace("@T%d@" % k, tcvn(u))

    neo_bang = NEO_BANG.replace("\n", eol)
    if raw.count(neo_bang) != 1:
        print("!!! LOI TO: moc neo bang xuat hien %d lan (can 1)" % raw.count(neo_bang)); return 1
    moi = raw.replace(neo_bang, bang.replace("\n", eol), 1)
    print("  M1 them bang TBNGUYENKHOANG: moc neo trung 1 lan")

    neo = NEO.replace("\n", eol)
    if moi.count(neo) != 1:
        print("!!! LOI TO: moc neo vong phat xuat hien %d lan (can 1)" % moi.count(neo)); return 1
    moi = moi.replace(neo, MOI.replace("\n", eol), 1)
    print("  M2 vong phat -> nguyen khoang theo goc: moc neo trung 1 lan")

    if moi.count(CAU_CU) == 1:
        moi = moi.replace(CAU_CU, CAU_MOI, 1)
        print("  M3 cau thong bao: da sua theo")
    else:
        print("  M3 cau thong bao: khong thay (bo qua)")

    print("  byte cao %d -> %d | CRLF %d -> %d"
          % (hi_truoc, sum(1 for c in moi if ord(c) > 127),
             raw.count("\r\n"), moi.count("\r\n")))
    try:
        moi.encode("latin-1")
    except UnicodeEncodeError as e:
        print("!!! LOI TO: ky tu ngoai latin-1: %s" % e); return 1

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao); print("  sao luu -> %s" % sao)
    with io.open(DICH, "wb") as f:
        f.write(moi.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != moi:
        print("!!! LOI TO: doc lai KHONG khop"); return 1
    print("  DA GHI + doc lai tu dia: khop.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
