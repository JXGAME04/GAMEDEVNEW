# -*- coding: utf-8 -*-
"""x5_log_client_dopho.py - LOG PHIA CLIENT cho the Do pho.

DA DO DUOC (soi o cua chu game): may chu NHAN TOT - phong 10 co
    o 0 (chinh) genre=6 detail=1 ptc=243 cap=1 nguhanh=0
=> loi nam han o phia CLIENT: khong ve mon vao o.

Con ba khau co the hong, va chung dan toi ba cach sua khac nhau:
  (A) `GDI_ATLAS_ITEM` tra ve 0 mon  -> danh sach m_AtlasItem cua CLIENT rong,
      tuc duong DONG BO tu may chu ve client chua dat mon vao pos_atlas.
  (B) tra ve co mon nhung `UpdateItem` khong khop `CtrlAtlasItemMap[i].nPosition`
      voi `pItem->Region.v` -> anh xa o sai.
  (C) khop roi ma van khong thay -> o chua duoc Init (LoadScheme truot, vi du
      khong nap duoc Dopho.ini).

Log nay in ca ba khau ra tep `loren_dopho.log` canh Game.exe:
    [DOPHO] LoadScheme: nap Dopho.ini = 1 | so o Init = 9
    [DOPHO] UpdateAllItem: GDI tra nCount=1
    [DOPHO]   mon 0: uId=1234 genre=1 Region.v=0
    [DOPHO]   UpdateItem: Region.v=0 -> khop o 0 (AtlasBox), HoldObject
    [DOPHO] CapNhatNguyenLieu: uId=1234 ptc=243 -> hang bang=0
Nhin ba dong dau la biet ngay hong khau nao.

Chi THEM lenh ghi log, khong doi mot dong logic nao.
Go bo: python x5_log_client_dopho.py --go --ghi

Tep dich: Sources\\S3Client\\Ui\\UiCase\\UiCompoundItem.cpp => build S3Client.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_logdopho lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 28/08] log chan doan the Do pho"
DICH = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiCompoundItem.cpp"
HAU_TO = ".truoc_logdopho"


def K(*d):
    return list(d)


HAM = K(
    "// " + NHAN + " - go bo bang x5_log_client_dopho.py --go --ghi",
    "static void sDoPhoLog(const char* szDinhDang, ...)",
    "{",
    T + "FILE* f = fopen(\"loren_dopho.log\", \"a\");",
    T + "if (!f)",
    T*2 + "return;",
    T + "va_list ap;",
    T + "va_start(ap, szDinhDang);",
    T + "vfprintf(f, szDinhDang, ap);",
    T + "va_end(ap);",
    T + "fprintf(f, \"\\n\");",
    T + "fclose(f);",
    "}",
    "",
)

MIENG = [
    ("ham ghi log",
     K("KUiAtlas::KUiAtlas()"),
     HAM + K("KUiAtlas::KUiAtlas()")),

    ("log LoadScheme",
     K(T*2 + "m_Atlas.Init(&Ini, \"AtlasBtn\");"),
     K(T*2 + "sDoPhoLog(\"[DOPHO] LoadScheme: nap %s = 1\", Buff);",
       T*2 + "m_Atlas.Init(&Ini, \"AtlasBtn\");")),

    ("log LoadScheme that bai",
     K(T + "sprintf(Buff, \"%s\\\\%s\", pScheme, SCHEME_INI_ATLAS);",
       T + "if (Ini.Load(Buff))"),
     K(T + "sprintf(Buff, \"%s\\\\%s\", pScheme, SCHEME_INI_ATLAS);",
       T + "if (!Ini.Load(Buff))",
       T*2 + "sDoPhoLog(\"[DOPHO] LoadScheme: KHONG NAP DUOC %s\", Buff);",
       T + "if (Ini.Load(Buff))")),

    ("log UpdateAllItem",
     K(T + "int nCount = g_pCoreShell->GetGameData(GDI_ATLAS_ITEM, (unsigned int)&Item, 0);"),
     K(T + "int nCount = g_pCoreShell->GetGameData(GDI_ATLAS_ITEM, (unsigned int)&Item, 0);",
       T + "sDoPhoLog(\"[DOPHO] UpdateAllItem: GDI tra nCount=%d\", nCount);")),

    # Neo phai HEP: mau `if (Item[i].Obj.uGenre != CGOG_NOTHING)` co o CA 7 pad.
    # Dung `CapNhatNguyenLieu();` lam moc - chi KUiAtlas moi co.
    ("log tung mon",
     K(T*2 + "if (Item[i].Obj.uGenre != CGOG_NOTHING)",
       T*3 + "UpdateItem(&Item[i], true);",
       T + "}",
       T + "CapNhatNguyenLieu();		// [LOREN 28/08] Do pho: ve ten nguyen lieu tren trang"),
     K(T*2 + "sDoPhoLog(\"[DOPHO]   mon %d: uId=%d genre=%d Region.v=%d\",",
       T*3 + "i, (int)Item[i].Obj.uId, (int)Item[i].Obj.uGenre, (int)Item[i].Region.v);",
       T*2 + "if (Item[i].Obj.uGenre != CGOG_NOTHING)",
       T*3 + "UpdateItem(&Item[i], true);",
       T + "}",
       T + "CapNhatNguyenLieu();		// [LOREN 28/08] Do pho: ve ten nguyen lieu tren trang")),

    ("log UpdateItem khop o",
     K(T*3 + "if (CtrlAtlasItemMap[i].nPosition == pItem->Region.v)",
       T*3 + "{"),
     K(T*3 + "if (CtrlAtlasItemMap[i].nPosition == pItem->Region.v)",
       T*3 + "{",
       T*4 + "sDoPhoLog(\"[DOPHO]   UpdateItem: Region.v=%d -> khop o %d (%s)\",",
       T*5 + "(int)pItem->Region.v, i,",
       T*5 + "CtrlAtlasItemMap[i].pIniSection ? CtrlAtlasItemMap[i].pIniSection : \"NULL\");")),

    ("log CapNhatNguyenLieu",
     K(T + "int nPtc = g_pCoreShell->GetGameData(GDI_ITEM_PARTICULAR, 0, (int)Obj.uId);"),
     K(T + "int nPtc = g_pCoreShell->GetGameData(GDI_ITEM_PARTICULAR, 0, (int)Obj.uId);",
       T + "sDoPhoLog(\"[DOPHO] CapNhatNguyenLieu: uId=%d ptc=%d\", (int)Obj.uId, nPtc);")),

    ("log tra bang",
     K(T + "if (nHang < 2)",
       T*2 + "return;"),
     K(T + "sDoPhoLog(\"[DOPHO] CapNhatNguyenLieu: hang bang=%d (cao=%d)\", nHang, Tab.GetHeight());",
       T + "if (nHang < 2)",
       T*2 + "return;")),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    go = "--go" in sys.argv[1:]
    print("=== x5_log_client_dopho - %s ==="
          % ("GO BO" if go else ("GHI THAT" if ghi else "DIEN TAP")))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if go:
        sao = DICH + HAU_TO
        if not os.path.isfile(sao):
            print("!!! khong co ban sao luu - khong go duoc")
            return 1
        if not ghi:
            print("  se khoi phuc tu %s (chay kem --ghi)" % os.path.basename(sao))
            return 0
        shutil.copy2(sao, DICH)
        print("  DA KHOI PHUC.")
        return 0

    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    dong = raw.split(eol)
    for ten, cu, moi in MIENG:
        vt = [i for i in range(len(dong) - len(cu) + 1) if dong[i:i + len(cu)] == cu]
        if len(vt) != 1:
            print("  !!! %-26s khop %d lan (can 1)" % (ten, len(vt)))
            return 1
        print("  ok  %-26s dong %d" % (ten, vt[0] + 1))
        dong = dong[:vt[0]] + moi + dong[vt[0] + len(cu):]

    nd = eol.join(dong)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    if nd.count("{") - raw.count("{") != nd.count("}") - raw.count("}"):
        print("!!! LOI TO: ngoac lech")
        return 1
    # can <stdarg.h> cho va_list
    if "#include <stdarg.h>" not in nd:
        nd = nd.replace("#include \"KWin32.h\"", "#include <stdarg.h>" + eol + "#include \"KWin32.h\"", 1)
        print("  ok  them #include <stdarg.h>")
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
    print("  DA GHI. => build S3Client, dat Game.exe, mo the Do pho roi doc")
    print("     E:\\...\\bin\\client\\loren_dopho.log")
    return 0


if __name__ == "__main__":
    sys.exit(main())
