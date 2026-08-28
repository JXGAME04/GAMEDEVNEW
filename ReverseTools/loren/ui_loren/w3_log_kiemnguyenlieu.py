# -*- coding: utf-8 -*-
"""w3_log_kiemnguyenlieu.py - LOG CHAN DOAN cho duong kiem nguyen lieu lo ren.

CHU GAME: "kham nam bo phuc duyen va thuy tinh vao se duoc tang ti le opt cao,
nhung toi bo thuy tinh va phuc duyen vao bao SAI NGUYEN LIEU".

DA LOAI TRU HET bang cach doc ma (khong doan):
  - `foundryresdemand.ini` CO du: ConsumeItem_2/3/4 = Thuy Tinh
    (Genre=4 DetailType=238/239/240 PtcType=1 Stackable=1);
    ConsumeItem_5/6/7 = Phuc Duyen (Genre=6 DetailType=1 PtcType=121/122/123).
    Ma da nan dung: Thuy Tinh la genre 4 nen KHONG thuoc dai dich -1 (chi genre 6
    moi dich), va ca hai ban Linux/JX1 deu ghi 238/239/240; Phuc Duyen JX1
    121/122/123 = Linux 122/123/124 - 1. Doi chieu `itemvaluescript.txt` hai ban.
  - `ResScheme_1` (so do dang chay, `KCore.cpp:276` goi Init() mac dinh 1) CO
    dong `Enchase_EnhanceItem=ConsumeItem`.
  - `s_szKhoa[14]` CO ten "Enchase_EnhanceItem" nen khoa duoc nap.
  - `CtrlEnchaseItemMap` du 11 o (BigBox, SmallBox1/2, Consume1..8).
  - Phong may chu `m_EnchaseItem[outinpart_num]`, `LuaGetIdItem` case 9 nhan
    nSocket 0..10 => du 11 o.
  - `LR_UI_GomPhong(9, 11, 3)` tach dung: o 0..2 CHINH, o 3..10 PHU.
  - `Check()` va `CheckTuChon()` duoc goi RIENG cho hai vector (KItemCompound
    :1072 va :1075), khong gop.
  - `KhopTietDoan` chi chan khi mon DANG XEP CHONG ma tiet doan khong cho -
    ConsumeItem_* deu Stackable=1 nen khong dinh.

=> Khong con gia thuyet nao dung vung. Theo dung luat "trieu chung lap sau va =
   gia thuyet sai, lan nguoc DUONG MA, dung va chong", buoc tiep la DO THAT chu
   khong doan tiep.

MIENG VA (chi THEM LOG, khong doi mot dong logic nao):
In ra console + GameServer.log, moi khi mot trong hai ham kiem TU CHOI:
    [LOREN-KIEM] type=5 CHINH n=3 -> ma 8
    [LOREN-KIEM]   o0 genre=0 detail=0 ptc=3   cap=10 he=2 chong=1 nature=1
    [LOREN-KIEM]   o1 genre=6 detail=1 ptc=146 cap=7  he=0 chong=1 nature=0
    [LOREN-KIEM]   ...
    [LOREN-KIEM]   khoa Enchase_EnhanceItem: co=1, 18 tiet doan
    [LOREN-KIEM]   o2 KHONG khop tiet doan nao
Nho vay biet CHINH XAC mon nao truot va truot o dieu kien nao, thay vi doan.

Log chi in khi TU CHOI nen khong lam ngap console luc choi binh thuong.
Go bo bang: python w3_log_kiemnguyenlieu.py --go

Tep dich: Sources\\Core\\Src\\KItemCompound.cpp (dung chung client + server)
=> build CA HAI cau hinh.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_logkiem lan dau).
"""
import io
import os
import shutil
import sys

DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemCompound.cpp"
HAU_TO = ".truoc_logkiem"

T = "\t"
NHAN = "[LOREN 27/08] LOG CHAN DOAN duong kiem nguyen lieu"

# ---- ham in mot mon, chen ngay truoc KhopTietDoan ----
NEO_HAM = "BOOL KFoundryResDemand::KhopTietDoan(const KResDemand& D, int nItemIdx)"

HAM = [
    "// " + NHAN + ".",
    "// Chi IN, khong doi logic. Goi tu Check()/CheckTuChon() khi TU CHOI, de biet",
    "// chinh xac mon nao truot thay vi doan. Go bo bang w3_log_kiemnguyenlieu.py --go",
    "static void sLoRenInMon(const char* szNhan, int nThuTu, int nItemIdx)",
    "{",
    T + "if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)",
    T + "{",
    T*2 + "printf(\"[LOREN-KIEM]   %s o%d: chi so vat pham KHONG HOP LE (%d)\\n\",",
    T*3 + "   szNhan, nThuTu, nItemIdx);",
    T*2 + "return;",
    T + "}",
    T + "KItem* p = &Item[nItemIdx];",
    T + "printf(\"[LOREN-KIEM]   %s o%d: genre=%d detail=%d ptc=%d cap=%d he=%d chong=%d nature=%d\\n\",",
    T*2 + "   szNhan, nThuTu, p->GetGenre(), p->GetDetailType(), p->GetParticular(),",
    T*2 + "   p->m_CommonAttrib.nLevel, p->m_CommonAttrib.nSeries,",
    T*2 + "   p->GetStackNum(), p->m_CommonAttrib.nItemNature);",
    "}",
    "",
]

# ---- trong CheckTuChon: in khi tu choi ----
CU_TC = [
    T + "for (int i = 0; i < nCount; i++)",
    T + "{",
    T*2 + "if (!KhopMotKhoa(pnItem[i], nKhoa))",
    T*3 + "return FOUNDRY_RESULT_RULE_ERROR;",
    T + "}",
]
MOI_TC = [
    T + "for (int i = 0; i < nCount; i++)",
    T + "{",
    T*2 + "if (!KhopMotKhoa(pnItem[i], nKhoa))",
    T*2 + "{",
    T*3 + "// " + NHAN,
    T*3 + "printf(\"[LOREN-KIEM] type=%d O TU CHON n=%d -> tu choi (ma 8) tai o thu %d\\n\",",
    T*4 + "   nCompoundType, nCount, i);",
    T*3 + "printf(\"[LOREN-KIEM]   khoa so %d: co=%d, %d tiet doan\\n\",",
    T*4 + "   nKhoa, (int)m_bCoKhoa[nKhoa], (int)m_aryKhoa[nKhoa].size());",
    T*3 + "for (int j = 0; j < nCount; j++)",
    T*4 + "sLoRenInMon(\"TUCHON\", j, pnItem[j]);",
    T*3 + "return FOUNDRY_RESULT_RULE_ERROR;",
    T*2 + "}",
    T + "}",
]

# ---- trong Check: in khi thieu nhom bat buoc ----
CU_CK = [
    T*2 + "if (!bDu)",
    T*3 + "return FOUNDRY_RESULT_LACK_RESOURCE;",
]
MOI_CK = [
    T*2 + "if (!bDu)",
    T*2 + "{",
    T*3 + "// " + NHAN,
    T*3 + "printf(\"[LOREN-KIEM] type=%d O CHINH n=%d -> thieu NHOM BAT BUOC thu %d (ma 4)\\n\",",
    T*4 + "   nCompoundType, nCount, g);",
    T*3 + "for (int j = 0; j < nCount; j++)",
    T*4 + "sLoRenInMon(\"CHINH\", j, pnItem[j]);",
    T*3 + "return FOUNDRY_RESULT_LACK_RESOURCE;",
    T*2 + "}",
]


def ap(dong, cu, moi, ten):
    """Thay mot khoi dong lien tiep, doi khop DUNG 1 lan."""
    vt = []
    for i in range(len(dong) - len(cu) + 1):
        if dong[i:i + len(cu)] == cu:
            vt.append(i)
    if len(vt) != 1:
        print("!!! LOI TO: khoi %s khop %d lan (can 1)" % (ten, len(vt)))
        return None
    i = vt[0]
    print("  %s: dong %d..%d" % (ten, i + 1, i + len(cu)))
    return dong[:i] + moi + dong[i + len(cu):]


def main():
    ghi = "--ghi" in sys.argv[1:]
    go = "--go" in sys.argv[1:]
    print("=== w3_log_kiemnguyenlieu - %s ==="
          % ("GO BO" if go else ("GHI THAT" if ghi else "DIEN TAP")))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if go:
        sao = DICH + HAU_TO
        if not os.path.isfile(sao):
            print("!!! khong co ban sao luu %s - khong go duoc" % sao)
            return 1
        if not ghi:
            print("  se khoi phuc tu %s (chay kem --ghi de lam that)" % os.path.basename(sao))
            return 0
        shutil.copy2(sao, DICH)
        print("  DA KHOI PHUC tu ban sao luu.")
        return 0

    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    dong = raw.split(eol)

    # 1) chen ham in
    vt = [i for i, l in enumerate(dong) if l == NEO_HAM]
    if len(vt) != 1:
        print("!!! LOI TO: moc neo KhopTietDoan khop %d lan (can 1)" % len(vt))
        return 1
    dong = dong[:vt[0]] + HAM + dong[vt[0]:]
    print("  ham sLoRenInMon: chen %d dong truoc dong %d" % (len(HAM), vt[0] + 1))

    # 2) CheckTuChon
    dong = ap(dong, CU_TC, MOI_TC, "CheckTuChon")
    if dong is None:
        return 1
    # 3) Check
    dong = ap(dong, CU_CK, MOI_CK, "Check (nhom bat buoc)")
    if dong is None:
        return 1

    moi = eol.join(dong)

    hi1 = sum(1 for c in moi if ord(c) > 127)
    if hi1 != hi0:
        print("!!! LOI TO: byte cao %d -> %d" % (hi0, hi1))
        return 1
    if moi.count("{") != moi.count("}"):
        print("!!! LOI TO: ngoac lech {%d }%d" % (moi.count("{"), moi.count("}")))
        return 1
    try:
        moi.encode("latin-1")
    except UnicodeEncodeError as e:
        print("!!! LOI TO: ky tu ngoai latin-1: %s" % e)
        return 1
    print("  byte cao %d (khong doi) | ngoac {%d }%d can bang | CRLF %d -> %d"
          % (hi0, moi.count("{"), moi.count("}"), raw.count("\r\n"), moi.count("\r\n")))

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % os.path.basename(sao))
    with io.open(DICH, "wb") as f:
        f.write(moi.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != moi:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI + doc lai tu dia: khop.")
    print("\n  => build Core CA HAI cau hinh, dat CoreServer.dll, thu lai va xem console")
    return 0


if __name__ == "__main__":
    sys.exit(main())
