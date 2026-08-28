# -*- coding: utf-8 -*-
"""vJ_intro_khoang.py - RUT NGAN Intro cua 6 khoang thuoc tinh cho VUA 127 byte.

GOC SAP "re chuot vao khoang = dis client" (da truy den cung):
  KBASICPROP_MAGICSCRIPT::m_szIntro la char[SZBUFLEN_1] = 128 byte
  (KBasPropTbl.h:14 + :329). Cot Intro cua 6 vien khoang 199..204 trong
  settings\\item\\magicscript.txt dai 164..183 byte => KTabFile::GetValue
  (KTabFile.cpp:475-479) CAT con 127 byte, va cho cat roi DUNG GIUA mot the
  "<color=...>".

  Voi vien Phu Dung Thach (ptc 202) chuoi cut thanh "...thay doi <color=yell":
  the HO. Khi ve mo ta, TEncodeCtrl (Text.cpp:592-640) gap "<color=" roi di
  TIM dau ">" - vong tim nay chi chan bang do dai CA chu giai (Text.cpp:623-627)
  nen no bat duoc dau ">" cua the " <color=Fire>" ma KItem.cpp:1507 vua noi vao
  phia sau => nParamLen ~ 21. Sau do Text.cpp:700-701:
        static char Color[13];
        memcpy(Color, pParamBuffer, nParamLen);   // KHONG chan
        Color[nParamLen] = 0;                     // ghi o chi so 21
  => ghi 9 byte ra ngoai dem 13 byte. Dung bay da ghi trong so tay du an:
  "ten mau >= 13 ky tu lam hong bo nho".

MIENG VA NAY chi dong vao DU LIEU: viet lai Intro cua 6 dong cho <= 127 byte VA
moi the mau deu dong. Hieu luc NGAY khi nap lai bang, KHONG phai build.
Van con phai va Text.cpp (kep nParamLen) o dot rieng - client khong duoc sap vi
mot chuoi la; mieng do lam sau vi phai build lai Engine.

Ap cho CA HAI cay (server + client) de hai ben giong nhau tung byte.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_introkhoang lan dau).
"""
import io
import os
import shutil
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\viemde")
from bangtxt import uni2tcvn, tcvn2uni  # noqa: E402

CAY = [
    r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt",
    r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\settings\item\magicscript.txt",
]
HAU_TO = ".truoc_introkhoang"
GIOI_HAN = 127          # KTabFile cat o 127; phai <= muc nay
COT_INTRO = 9           # cot 9 = Intro (dem tu 1)

CHO_PHEP_HOA = set("\u0102\u00c2\u0110\u00ca\u00d4\u01a0\u01af")


def tcvn(u):
    for ch in u:
        if ord(ch) > 127 and ch.isupper() and ch not in CHO_PHEP_HOA:
            raise SystemExit("!!! LOI TO: chu HOA co dau khong ma hoa duoc TCVN3: %r" % ch)
    t = uni2tcvn(u)
    if tcvn2uni(t) != u:
        raise SystemExit("!!! LOI TO: cham vong tron that bai: %r" % u)
    return t


# Intro moi: giu dung y nghia goc (khoang co thuoc tinh, kham len trang bi Huyen
# Tinh cung ngu hanh), bo phan dai dong. KHONG dung the <color> de khoi rui ro
# - chu van doc duoc binh thuong.
NOI_DUNG = {
    199: u"Kho\u00e1ng th\u1ea1ch thu\u1ed9c t\u00ednh. Kh\u1ea3m l\u00ean trang b\u1ecb "
         u"Huy\u1ec1n Tinh c\u00f9ng ng\u0169 h\u00e0nh \u0111\u1ec3 th\u00eam thu\u1ed9c t\u00ednh.",
    200: u"Kho\u00e1ng th\u1ea1ch thu\u1ed9c t\u00ednh. Kh\u1ea3m l\u00ean trang b\u1ecb "
         u"Huy\u1ec1n Tinh c\u00f9ng ng\u0169 h\u00e0nh \u0111\u1ec3 th\u00eam thu\u1ed9c t\u00ednh.",
    201: u"Kho\u00e1ng th\u1ea1ch thu\u1ed9c t\u00ednh. Kh\u1ea3m l\u00ean trang b\u1ecb "
         u"Huy\u1ec1n Tinh c\u00f9ng ng\u0169 h\u00e0nh \u0111\u1ec3 th\u00eam thu\u1ed9c t\u00ednh.",
    202: u"Kho\u00e1ng th\u1ea1ch thu\u1ed9c t\u00ednh. Kh\u1ea3m l\u00ean trang b\u1ecb "
         u"Huy\u1ec1n Tinh c\u00f9ng ng\u0169 h\u00e0nh \u0111\u1ec3 th\u00eam thu\u1ed9c t\u00ednh.",
    203: u"Kho\u00e1ng th\u1ea1ch thu\u1ed9c t\u00ednh. Kh\u1ea3m l\u00ean trang b\u1ecb "
         u"Huy\u1ec1n Tinh c\u00f9ng ng\u0169 h\u00e0nh \u0111\u1ec3 th\u00eam thu\u1ed9c t\u00ednh.",
    204: u"Kho\u00e1ng th\u1ea1ch thu\u1ed9c t\u00ednh. Kh\u1ea3m l\u00ean trang b\u1ecb "
         u"Huy\u1ec1n Tinh c\u00f9ng ng\u0169 h\u00e0nh \u0111\u1ec3 th\u00eam thu\u1ed9c t\u00ednh.",
}


def kiem_the_can(s):
    """Moi '<' phai co '>' dong sau no; khong duoc bo lung."""
    i = 0
    while True:
        i = s.find("<", i)
        if i < 0:
            return True
        j = s.find(">", i)
        if j < 0:
            return False
        i = j + 1


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vJ_intro_khoang - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    moi_tcvn = {}
    for ptc, u in NOI_DUNG.items():
        t = tcvn(u)
        if len(t) > GIOI_HAN:
            print("!!! LOI TO: Intro moi cua ptc %d dai %d byte > %d" % (ptc, len(t), GIOI_HAN))
            return 1
        if not kiem_the_can(t):
            print("!!! LOI TO: Intro moi cua ptc %d co the '<' khong dong" % ptc)
            return 1
        moi_tcvn[ptc] = t
    print("  Intro moi: %d byte moi dong (gioi han %d), the mau: khong dung" %
          (len(list(moi_tcvn.values())[0]), GIOI_HAN))

    ke_hoach = []
    for p in CAY:
        if not os.path.isfile(p):
            print("!!! LOI TO: khong thay %s" % p); return 1
        raw = io.open(p, "rb").read().decode("latin-1")
        eol = "\r\n" if "\r\n" in raw else "\n"
        dong = raw.split(eol)
        hi_truoc = sum(1 for c in raw if ord(c) > 127)
        sua = 0
        print("\n--- %s" % p)
        for i, ln in enumerate(dong):
            if not ln.strip():
                continue
            c = ln.split("\t")
            if len(c) < COT_INTRO:
                continue
            try:
                ptc = int(c[3])
            except (ValueError, IndexError):
                continue
            if ptc not in moi_tcvn or c[1].strip() != "6" or c[2].strip() != "1":
                continue
            cu = c[COT_INTRO - 1]
            if cu == moi_tcvn[ptc]:
                continue
            print("   dong %-5d ptc %-4d  %3d byte -> %3d byte  | %s"
                  % (i, ptc, len(cu), len(moi_tcvn[ptc]), tcvn2uni(c[0])[:22]))
            c[COT_INTRO - 1] = moi_tcvn[ptc]
            dong[i] = "\t".join(c)
            sua += 1
        if sua == 0:
            print("   DA CO - bo qua (idempotent)")
            continue
        if sua != len(moi_tcvn):
            print("!!! LOI TO: sua %d dong, mong %d - KHONG ghi gi" % (sua, len(moi_tcvn)))
            return 1
        moi_raw = eol.join(dong)
        print("   byte cao %d -> %d | so dong %d (khong doi)"
              % (hi_truoc, sum(1 for ch in moi_raw if ord(ch) > 127), len(dong)))
        ke_hoach.append((p, moi_raw))

    if not ke_hoach:
        print("\nKhong co gi de ap.")
        return 0
    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    for p, noi_dung in ke_hoach:
        sao = p + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
            print("  sao luu -> %s" % sao)
        with io.open(p, "wb") as f:
            f.write(noi_dung.encode("latin-1"))
        if io.open(p, "rb").read().decode("latin-1") != noi_dung:
            print("!!! LOI TO: doc lai tu dia KHONG khop: %s" % p)
            return 1
        print("  DA GHI %s" % p)
    return 0


if __name__ == "__main__":
    sys.exit(main())
