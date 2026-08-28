# -*- coding: utf-8 -*-
"""x6_dopho_thongbao_ui.py - VA "bo do pho vao la mat, o khong hien".

DA DO RA GOC bang hai buoc, khong doan:

BUOC 1 - soi o cua chu game: MAY CHU NHAN TOT
    --- Do pho Hoang Kim ---
    o 0 (chinh) genre=6 detail=1 ptc=243 cap=1 nguhanh=0
=> mon vao phong 10 binh thuong, khong mat di dau.

BUOC 2 - log phia client (`loren_dopho.log`):
    [DOPHO] LoadScheme: nap \\Ui\\ui3\\khamnam/Dopho.ini = 1     <- bo cuc OK
    [DOPHO] UpdateAllItem: GDI tra nCount=11
    [DOPHO]   mon 0: uId=0 genre=0 Region.v=0
    [DOPHO]   mon 1: uId=0 genre=0 Region.v=83963920            <- rac
    ... (ca 11 o deu uId=0)
Hai dieu doc duoc:
  a) MOI o deu uId=0 => `m_AtlasItem` cua CLIENT rong (Region.v la rac vi chi
     duoc gan trong nhanh co uId).
  b) Ca tep log chi co DUNG MOT lan `UpdateAllItem` - tuc lan mo the, luc phong
     con rong. Sau khi bo do vao KHONG co dong "UpdateItem ... khop o" nao,
     cung khong co dong `CapNhatNguyenLieu`.
=> Client KHONG HE DUOC BAO de ve lai.

GOC THAT - `GameSpaceChangedNotify.cpp` thieu nhanh cho UOC_ATLAS_ITEM.
Tep nay phan phoi tin "o vua doi" ve dung cua so. Cho cua so lo ren no co DUNG
SAU nhanh, moi nhanh mot container:
    UOC_COMPONE_ITEM / UOC_COMPTWO_ITEM / UOC_COMPTHREE_ITEM /
    UOC_DISTILL_ITEM / UOC_FORGE_ITEM   / UOC_ENCHASE_ITEM
KHONG co nhanh cho UOC_ATLAS_ITEM => tin ve o Do pho roi vao hu khong, cua so
khong bao gio biet ma ve.

Day la nhanh THU BA bi bo sot cung mot kieu (sau `KItemList.cpp:3754` o w9 va
`KUiCompoundItem::UpdateItem` o w6): cu them mot container moi la phai ra soat
DU CA BA cho.

MIENG VA: them nhanh UOC_ATLAS_ITEM, nhan ban nguyen khuon UOC_ENCHASE_ITEM.

Tep dich: Sources\\S3Client\\Ui\\GameSpaceChangedNotify.cpp => build S3Client.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_tbui lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 28/08] Do pho: bao UI khi o doi"
DICH = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\GameSpaceChangedNotify.cpp"
HAU_TO = ".truoc_tbui"

CU = [
    T*3 + "else if (pObject->eContainer == UOC_ENCHASE_ITEM)	// [UILOREN] khuon UOC_TREMBLE_ITEM",
    T*3 + "{",
    T*4 + "KUiCompoundItem* pCompItem = KUiCompoundItem::GetIfVisible();",
    T*4 + "if (pCompItem)",
    T*5 + "pCompItem->UpdateItem((KUiObjAtRegion*)uParam, nParam);",
    T*3 + "}",
]

MOI = CU + [
    T*3 + "// " + NHAN + " - THIEU nhanh nay thi tin \"o vua doi\" cua",
    T*3 + "// the Do pho roi vao hu khong, cua so khong bao gio biet ma ve lai",
    T*3 + "// (do bang log: ca tep chi co DUNG MOT lan UpdateAllItem luc mo the).",
    T*3 + "else if (pObject->eContainer == UOC_ATLAS_ITEM)",
    T*3 + "{",
    T*4 + "KUiCompoundItem* pCompItem = KUiCompoundItem::GetIfVisible();",
    T*4 + "if (pCompItem)",
    T*5 + "pCompItem->UpdateItem((KUiObjAtRegion*)uParam, nParam);",
    T*3 + "}",
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== x6_dopho_thongbao_ui - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    dong = raw.split(eol)
    vt = [i for i in range(len(dong) - len(CU) + 1) if dong[i:i + len(CU)] == CU]
    if len(vt) != 1:
        print("!!! LOI TO: moc neo khop %d lan (can 1)" % len(vt))
        return 1
    i = vt[0]
    print("  ok  chen nhanh UOC_ATLAS_ITEM sau dong %d" % (i + len(CU)))
    dong = dong[:i] + MOI + dong[i + len(CU):]

    nd = eol.join(dong)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    if nd.count("{") - raw.count("{") != nd.count("}") - raw.count("}"):
        print("!!! LOI TO: ngoac lech")
        return 1
    # chot: dem so nhanh bao cho cua so lo ren
    n = nd.count("KUiCompoundItem* pCompItem = KUiCompoundItem::GetIfVisible();")
    print("  chot: so nhanh bao cua so lo ren = %d (truoc do 6, nay phai la 7)" % n)
    if n != 7:
        print("!!! LOI TO: mong 7 nhanh")
        return 1
    print("  byte cao %d (khong doi) | ngoac can bang" % hi0)

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI. => build S3Client, dat lai Game.exe")
    return 0


if __name__ == "__main__":
    sys.exit(main())
