# -*- coding: utf-8 -*-
"""y1_danhsach_trang_atlas.py - VA "bam Ket hop khong hien gi" (goc THAT).

DA DO RA bang log client cua chu game - luong phia client CHAY SACH:
    [DOPHO] WndProc: BUTTON_CLICK, la nut Ket hop = 1, m_nStatus=0
    [DOPHO] Breathe: m_nStatus 0 -> 1
    [DOPHO] Breathe: m_nStatus 1 -> 2
    [DOPHO] PlayEffect: khung=5 / MAX=25
    [DOPHO] PlayEffect: khung=15 / MAX=25
    [DOPHO] PlayEffect: khung=25 / MAX=25
    [DOPHO] Breathe: m_nStatus 2 -> 3
    [DOPHO] ProcessAtlas: GUI LENH LR_UI_Atlas len may chu
=> lenh DA GUI DI. Loi nam ben MAY CHU.

GOC: `KProtocolProcess::UiCommandScript` case 7 (nut bam cua so lo ren) co mot
DANH SACH TRANG ten ham duoc phep goi:

    static const char* s_szLRFuns[] =
    {
        "LR_UI_MotOre", "LR_UI_HaiOre", "LR_UI_BaOre",
        "LR_UI_Distill", "LR_UI_Forge", "LR_UI_Enchase",
    };

KHONG co "LR_UI_Atlas". May chu nhan goi, duyet het danh sach, khong khop ten
nao => IM LANG BO QUA, khong chay kich ban nao ca. Vi the khong co ca thong bao
loi - dung nhu "bam khong hien gi".

Chot an ninh nay la DUNG (chi cho ham trong danh sach, chan client tu che ten
ham); chi thieu ten cua trang moi.

DAY LA NHANH THU NAM bi bo sot khi them mot trang lo ren. Danh sach DAY DU
phai ra soat khi them trang moi:
    1. KItemList.cpp:3754            duong dat vat pham (keo-tha)        -> w9
    2. KUiCompoundItem::UpdateItem   duong phan phoi trong cua so        -> w6
    3. GameSpaceChangedNotify        duong bao tin "o vua doi"           -> x6
    4. KUiCompoundItem::Breathe      duong chay NHIP cua trang           -> x8
    5. UiCommandScript case 7        DANH SACH TRANG ten ham duoc goi    -> y1

MIENG VA: them "LR_UI_Atlas" vao s_szLRFuns.

Tep dich: Sources\\Core\\Src\\KProtocolProcess.cpp => build Core CA HAI cau hinh.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_dstrang lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 28/08] them LR_UI_Atlas vao danh sach trang"
DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KProtocolProcess.cpp"
HAU_TO = ".truoc_dstrang"

CU = [
    T*5 + '"LR_UI_MotOre", "LR_UI_HaiOre", "LR_UI_BaOre",',
    T*5 + '"LR_UI_Distill", "LR_UI_Forge", "LR_UI_Enchase",',
]

MOI = [
    T*5 + '"LR_UI_MotOre", "LR_UI_HaiOre", "LR_UI_BaOre",',
    T*5 + '"LR_UI_Distill", "LR_UI_Forge", "LR_UI_Enchase",',
    T*5 + '// ' + NHAN + ': thieu ten nay thi may chu',
    T*5 + '// nhan goi roi IM LANG bo qua - bam nut khong hien gi, ke ca thong bao loi.',
    T*5 + '"LR_UI_Atlas",',
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== y1_danhsach_trang_atlas - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if "LR_UI_Atlas" in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    dong = raw.split(eol)
    vt = [i for i in range(len(dong) - len(CU) + 1) if dong[i:i + len(CU)] == CU]
    if len(vt) != 1:
        print("!!! LOI TO: moc neo khop %d lan (can 1)" % len(vt))
        for i, l in enumerate(dong):
            if "LR_UI_MotOre" in l:
                print("    dong %d: %r" % (i + 1, l[:88]))
        return 1
    i = vt[0]
    print("  ok  them \"LR_UI_Atlas\" vao danh sach (dong %d)" % (i + len(CU)))
    dong = dong[:i] + MOI + dong[i + len(CU):]

    nd = eol.join(dong)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    if nd.count("{") != raw.count("{") or nd.count("}") != raw.count("}"):
        print("!!! LOI TO: ngoac doi")
        return 1
    # chot: dem so ten trong danh sach
    kh = nd.split("s_szLRFuns[] =")[1].split("};")[0]
    n = kh.count("\"LR_UI_")
    print("  chot: danh sach co %d ten (truoc 6, nay phai 7)" % n)
    if n != 7:
        print("!!! LOI TO: mong 7 ten")
        return 1
    print("  byte cao %d (khong doi) | ngoac khong doi" % hi0)

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
    print("  DA GHI. => build Core CA HAI cau hinh, dat CoreServer.dll")
    return 0


if __name__ == "__main__":
    sys.exit(main())
