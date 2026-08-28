# -*- coding: utf-8 -*-
"""vZ_transitemval_phienban2.py - PORT `_transItemValImpl_2` TU BAN LINUX.

GOC THAT cua "kham nam bao that bai, mat het nguyen lieu":
JX1 goi CUNG thuat toan chia xac suat PHIEN BAN 1, trong khi ban Linux chay
PHIEN BAN 2. Bang chung nguyen van, `itemvalue_header.lua:246` ban Linux:

    -- 使用版本2的价值量概率转移函数        ("dung ham chuyen xac suat phien ban 2")
    transItemValImpl = _transItemValImpl_2;

Ban Linux goi qua BIEN `transItemValImpl` (dong 42), va cuoi tep gan bien do
bang `_transItemValImpl_2`. Ban JX1 da bo lop gian tiep nay, goi thang
`calcProbLoop` - tuc ket cung o PHIEN BAN 1.

KHAC NHAU O DAU (day moi la diem quyet dinh):
  - Phien ban 1: chia xac suat cho TOAN BO day cua, theo ti le
    dDivVal/gia_tri. Cua re nhat LUON duoc chia mot phan.
  - Phien ban 2: tim vi tri cua nguyen lieu trong day da sap xep roi CHI rai
    xac suat cho 4 CUA LAN CAN (idx-2, idx-1, idx, idx+1), noi suy tuyen tinh
    voi `dHiProb = 1 - dLowProb` nen TONG LUON = 1. Cua nao nam ngoai vung
    lan can duoc DUNG 0%.

Trong kham nam, `finalCompound` chen them mot cua mang gia tri TRANG BI HIEN
TAI (`nEquipVal`) - trung cua do la "that bai": giu trang bi, mat nguyen lieu.
Cua nay luon la cua THAP NHAT day. Voi phien ban 2, chi can nguyen lieu vuot
qua cua dich thu hai la cua that bai roi ra ngoai vung lan can => 0%.

DO BANG SO tren chinh day cua trong log cua chu game
(Logs/KSG_CompoundLog_Prob.txt, cua that bai = 40.500.000):
    SrcVal        212.340.000  -> PB1: hong 77,97%   PB2: hong 52,67%
    SrcVal        300.000.000  ->                    PB2: hong 28,52%
    SrcVal        405.000.000  ->                    PB2: hong  0,78%
    SrcVal        420.000.000  ->                    PB2: hong  0,00%  <= 100% THANH CONG
    SrcVal        900.000.000  ->                    PB2: hong  0,00%
    SrcVal      6.000.000.000  ->                    PB2: hong  0,00%
Voi phien ban 1 thi cua that bai KHONG BAO GIO ve 0 du bo bao nhieu nguyen
lieu - dung nhu trieu chung. Chu game xac nhan: chay ban Linux thi kham 100%
thanh cong, chi ep Huyen Tinh moi co that bai.

DA LOAI TRU TRUOC DO (kiem that, khong doan):
  - `settings/item/001/itemvalue/*` : JX1 KHOP 100% ban Linux ver 001 (7/7 tep
    byte-identical). (Luc dau toi so nham voi ver 004 va tuong `ore.txt` bi chia
    10 - ver 000..003 cua Linux dung DUNG so nhu JX1. Bang KHONG SAI.)
  - `itemvalue/equip_enchasable.lua` : hai ban giong het tung dong (97/97).
  - `compound/equip_enchase.lua`     : logic trung khop ban Linux, ke ca nhanh
    that bai (giu trang bi, xoa nguyen lieu con lai) va viec chen cua nEquipVal.
  => Khong sua gi trong nhung tep do. Chi thieu dung lop chon thuat toan.

MIENG VA (3 diem, deu la khoi phuc nguyen van ban Linux):
  1. `arydDesProb[i] = 1;`  ->  `= 0;`
     Ban Linux khoi tao 0 (dong 28); chinh `_transItemValImpl_1` moi dat lai 1
     cho rieng no (dong 84-86). De 1 san la sai voi phien ban 2.
  2. `calcProbLoop( arydDesItemValSort, arydDesProb, 1, getn(...), dSrcItemVal );`
     ->  `transItemValImpl( dSrcItemVal, arydDesItemValSort, arydDesProb );`
     (khoi phuc lop gian tiep, dung thu tu doi so cua ban Linux dong 42)
  3. Them vao cuoi tep: `_transItemValImpl_1` (boc calcProbLoop - giu duong lui),
     hai hang `LOW_NEAR_PERCENT`/`HI_NEAR_PERCENT` = 0.95, `_transItemValImpl_2`
     port 1:1 tu Linux 185-241, va dong gan `transItemValImpl = _transItemValImpl_2;`

Khong dong toi `calcProbLoop` (ban vong-lap chong tran ngan xep Lua van giu
nguyen, van dung duoc qua `_transItemValImpl_1`).

ANH HUONG (dung nhu ban Linux - dung chung cho ca ba noi goi TransItemVal):
  - `equip_enchase.lua`    kham nam        -> 100% thanh cong khi bo du nguyen lieu
  - `compound_header.lua`  ep Huyen Tinh   -> VAN co that bai (dung nhu chu game noi)
  - `atlas.lua`            do pho Hoang Kim
Ba noi nay o ban Linux cung dung chung phien ban 2, nen day la khoi phuc
nguyen trang, khong phai doi can bang.

KHONG PHAI BUILD: chi la kich ban Lua, nap lai script la chay.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_pb2 lan dau).
"""
import io
import os
import shutil
import sys

DICH = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
        r"\script\item\itemvalue\itemvalue_header.lua")
HAU_TO = ".truoc_pb2"

T = "\t"
NHAN = "[LOREN 27/08] KHOI PHUC LOP CHON THUAT TOAN"

CU_1 = T*2 + "arydDesProb[i] = 1;"
MOI_1 = T*2 + "arydDesProb[i] = 0;" + T + "-- " + NHAN + ": ban Linux khoi tao 0"

CU_2 = T + "calcProbLoop( arydDesItemValSort, arydDesProb, 1, getn( arydDesItemValSort ), dSrcItemVal );"
MOI_2 = T + "transItemValImpl( dSrcItemVal, arydDesItemValSort, arydDesProb );"

# Khoi them vao cuoi tep. Dat NGAY TRUOC `-- \xb2\xe5\xc8\xeb\xc5\xc5\xd0\xf2` (inssort) - tuc sau
# khi `calcProbLoop` da duoc dinh nghia.
NEO_3 = "function inssort( aryNumber, nStartIdx, nEndIdx )"

KHOI_3 = [
    "-- " + NHAN + " (port nguyen van tu ban Linux).",
    "-- Ban Linux goi qua BIEN `transItemValImpl` roi cuoi tep gan bien do bang",
    "-- `_transItemValImpl_2` (itemvalue_header.lua:246 - chu thich goc ghi ro",
    "-- \"dung ham chuyen xac suat gia tri PHIEN BAN 2\"). Ban JX1 truoc day bo lop",
    "-- gian tiep nay va goi thang `calcProbLoop`, tuc ket cung o PHIEN BAN 1.",
    "--",
    "-- Khac nhau quyet dinh:",
    "--   PB1 chia xac suat cho TOAN BO day cua theo ti le => cua RE NHAT luon",
    "--       duoc chia mot phan. Trong kham nam, cua re nhat chinh la cua",
    "--       \"giu nguyen trang bi\" = THAT BAI => khong bao gio ve 0%.",
    "--   PB2 chi rai xac suat cho 4 CUA LAN CAN quanh gia tri nguyen lieu,",
    "--       noi suy tuyen tinh (dHiProb = 1 - dLowProb) nen tong luon = 1.",
    "--       Bo du nguyen lieu la cua that bai nam ngoai vung => DUNG 0%.",
    "-- Do tren day cua that trong log: nguyen lieu 212.340.000 -> PB1 hong",
    "-- 77,97% / PB2 hong 52,67%; tu 420.000.000 tro len PB2 hong 0,00%.",
    "",
    "-- Phien ban 1 (2005.1.12): giu lai lam duong lui, khong xoa.",
    "function _transItemValImpl_1( dSrcItemVal, arydDesItemValSort, arydDesItemProbSort )",
    T + "for i = 1, getn( arydDesItemValSort ) do",
    T*2 + "arydDesItemProbSort[i] = 1;",
    T + "end",
    T + "calcProbLoop( arydDesItemValSort, arydDesItemProbSort, 1, getn( arydDesItemValSort ), dSrcItemVal );",
    "end",
    "",
    "LOW_NEAR_PERCENT\t= 0.95;\t\t-- phan xac suat cua cua THAP hon lien ke",
    "HI_NEAR_PERCENT\t\t= 0.95;\t\t-- phan xac suat cua cua CAO hon lien ke",
    "",
    "-- Phien ban 2 (ban Linux dong 185-241) - port 1:1, khong doi mot phep tinh.",
    "function _transItemValImpl_2( dSrcItemVal, arydDesItemValSort, arydDesItemProbSort )",
    T + "-- tim cua dau tien co gia tri > gia tri nguyen lieu",
    T + "local nSrcItemValIdx = 0;",
    T + "for i = 1, getn( arydDesItemValSort ) do",
    T*2 + "if( arydDesItemValSort[i] > dSrcItemVal ) then",
    T*3 + "nSrcItemValIdx = i;",
    T*3 + "break;",
    T*2 + "end",
    T + "end",
    "",
    T + "-- nguyen lieu >= MOI cua: 100% trung cua CAO NHAT",
    T + "if( nSrcItemValIdx == 0 ) then",
    T*2 + "arydDesItemProbSort[getn( arydDesItemValSort )] = 1;",
    T*2 + "return",
    T + "end",
    T + "-- nguyen lieu < MOI cua: khong trung cua nao (tat ca giu 0%)",
    T + "if( nSrcItemValIdx == 1 ) then",
    T*2 + "return",
    T + "end",
    "",
    T + "-- gia tri tron cua hai cua THAP",
    T + "local dLowValSum = 0;",
    T + "if( nSrcItemValIdx > 2 ) then",
    T*2 + "dLowValSum = arydDesItemValSort[nSrcItemValIdx - 1] * LOW_NEAR_PERCENT +",
    T*4 + " arydDesItemValSort[nSrcItemValIdx - 2] * ( 1 - LOW_NEAR_PERCENT );",
    T + "else",
    T*2 + "dLowValSum = arydDesItemValSort[1];",
    T + "end",
    T + "-- gia tri tron cua hai cua CAO",
    T + "local dHiValSum = 0;",
    T + "if( nSrcItemValIdx < getn( arydDesItemValSort ) ) then",
    T*2 + "dHiValSum = arydDesItemValSort[nSrcItemValIdx] * HI_NEAR_PERCENT +",
    T*4 + " arydDesItemValSort[nSrcItemValIdx + 1] * ( 1 - HI_NEAR_PERCENT );",
    T + "else",
    T*2 + "dHiValSum = arydDesItemValSort[getn( arydDesItemValSort )];",
    T + "end",
    "",
    T + "local dLowProb = ( dHiValSum - dSrcItemVal ) / ( dHiValSum - dLowValSum );",
    T + "local dHiProb = 1 - dLowProb;",
    "",
    T + "if( nSrcItemValIdx > 2 ) then",
    T*2 + "arydDesItemProbSort[nSrcItemValIdx - 1] = dLowProb * LOW_NEAR_PERCENT;",
    T*2 + "arydDesItemProbSort[nSrcItemValIdx - 2] = dLowProb * ( 1 - LOW_NEAR_PERCENT );",
    T + "else",
    T*2 + "arydDesItemProbSort[1] = dLowProb;",
    T + "end",
    T + "if( nSrcItemValIdx < getn( arydDesItemValSort ) ) then",
    T*2 + "arydDesItemProbSort[nSrcItemValIdx] = dHiProb * HI_NEAR_PERCENT;",
    T*2 + "arydDesItemProbSort[nSrcItemValIdx + 1] = dHiProb * ( 1 - HI_NEAR_PERCENT );",
    T + "else",
    T*2 + "arydDesItemProbSort[getn( arydDesItemValSort )] = dHiProb;",
    T + "end",
    "end",
    "",
    "-- Dung phien ban 2 (giong ban Linux dong 246).",
    "transItemValImpl = _transItemValImpl_2;",
    "",
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vZ_transitemval_phienban2 - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    dong = raw.split(eol)

    # ---- diem 1 ----
    vt1 = [i for i, l in enumerate(dong) if l == CU_1]
    if len(vt1) != 1:
        print("!!! LOI TO: `arydDesProb[i] = 1;` xuat hien %d lan (can 1)" % len(vt1))
        return 1
    dong[vt1[0]] = MOI_1
    print("  1) dong %d: khoi tao xac suat 1 -> 0" % (vt1[0] + 1))

    # ---- diem 2 ----
    vt2 = [i for i, l in enumerate(dong) if l == CU_2]
    if len(vt2) != 1:
        print("!!! LOI TO: dong goi `calcProbLoop(...)` xuat hien %d lan (can 1)" % len(vt2))
        return 1
    dong[vt2[0]] = MOI_2
    print("  2) dong %d: goi thang calcProbLoop -> goi qua transItemValImpl" % (vt2[0] + 1))

    # ---- diem 3 ----
    vt3 = [i for i, l in enumerate(dong) if l.strip() == NEO_3]
    if len(vt3) != 1:
        print("!!! LOI TO: moc neo inssort xuat hien %d lan (can 1)" % len(vt3))
        return 1
    i3 = vt3[0]
    # lui len qua cac dong chu thich/trong ngay truoc inssort
    while i3 > 0 and (dong[i3 - 1].strip().startswith("--") or dong[i3 - 1].strip() == ""):
        i3 -= 1
    dong = dong[:i3] + KHOI_3 + dong[i3:]
    print("  3) chen %d dong (PB1 + PB2 + dong gan) truoc inssort, tai dong %d"
          % (len(KHOI_3), i3 + 1))

    moi = eol.join(dong)

    # ---------------- chot an toan ----------------
    hi1 = sum(1 for c in moi if ord(c) > 127)
    if hi1 != hi0:
        print("!!! LOI TO: byte cao %d -> %d" % (hi0, hi1))
        return 1
    try:
        moi.encode("latin-1")
    except UnicodeEncodeError as e:
        print("!!! LOI TO: ky tu ngoai latin-1: %s" % e)
        return 1
    for can in ("function _transItemValImpl_1(", "function _transItemValImpl_2(",
                "transItemValImpl = _transItemValImpl_2;"):
        if moi.count(can) != 1:
            print("!!! LOI TO: `%s` xuat hien %d lan (can 1)" % (can, moi.count(can)))
            return 1
    if moi.count("function calcProbLoop(") != 1:
        print("!!! LOI TO: mat dinh nghia calcProbLoop")
        return 1
    n_fn = sum(1 for l in dong if l.strip().startswith("function "))
    n_end = sum(1 for l in dong if l.strip() == "end")
    print("  byte cao %d (khong doi) | CRLF %d -> %d | function=%d end=%d | dong %d"
          % (hi0, raw.count("\r\n"), moi.count("\r\n"), n_fn, n_end, len(dong)))

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
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI + doc lai tu dia: khop.")
    print("\n  => Nap lai script la chay, KHONG phai build.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
