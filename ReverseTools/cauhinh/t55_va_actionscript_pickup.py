# -*- coding: utf-8 -*-
"""t55_va_actionscript_pickup.py - va LOI DANG XAY RA: nhat cuon Da Tau lam
hong menu dang mo cua nguoi choi.

GOC:
  KPlayer.cpp:4993/4995 goi
      ExecuteScript("\\script\\item\\tasklink_goods.lua", "PickUp", nObjIndex)
  ma KHONG truyen tham so cuoi. Chu ky mac dinh la bGlobal = true
  (KPlayer.h:857), va nhanh bGlobal trong ExecuteScript ghi
      Npc[m_nIndex].m_ActionScriptID = dwScriptId     (KPlayer.cpp:7115)
  Trong khi do MENU tra loi cua nguoi choi duoc dieu phoi qua CHINH bien do:
      ExecuteScript(Npc[m_nIndex].m_ActionScriptID, m_szTaskAnswerFun[...])
                                                     (KPlayer.cpp:7607 va 7612)

HAU QUA: dang mo menu bat ky ma di qua mot cuon Da Tau (vat pham 6/1/205 hoac
6/1/212) la m_ActionScriptID bi cuop; cu bam nut tiep theo se goi vao
tasklink_goods.lua tim mot ham khong ton tai o do -> nut im lang khong an.

BANG CHUNG TRONG LOG (ScriptError.log cua may chu dang chay):
  2026/08/30 11:10:00  tasklink_goods.lua  cFuncName:(StationFun)    <- Xa Phu
  2026/08/30 10:31:25  tasklink_goods.lua  cFuncName:(g_DailogBack)  <- nut Quay lai
  2026/08/29 10:11:02  tasklink_goods.lua  cFuncName:(BDH_Root)      <- menu Ban Dong Hanh
  2026/08/28 18:49:24  tasklink_goods.lua  cFuncName:(ruong)         <- ruong
  2026/08/28 10:03:38  tasklink_goods.lua  cFuncName:(PB_DaTauSet)
  2026/08/27 23:40:56  tasklink_goods.lua  cFuncName:(psthanhthi)
Moi dong la mot lan nguoi choi bam nut ma cu bam di lac.

VA: truyen bGlobal = false cho hai loi goi do. Nhat cuon la viec "ban ra roi
thoi", khong duoc phep so huu ngu canh hoi thoai cua nguoi choi.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayer.cpp"
DUOI = ".truoc_actionscript"

VA = [
    ('bPickOk = ExecuteScript((char*)"\\\\script\\\\item\\\\tasklink_goods.lua",'
     ' (char*)"PickUp", nObjIndex);',
     'bPickOk = ExecuteScript((char*)"\\\\script\\\\item\\\\tasklink_goods.lua",'
     ' (char*)"PickUp", nObjIndex, false);'),
    ('bPickOk = ExecuteScript((char*)"\\\\script\\\\item\\\\'
     'tasklink_goods_secret.lua", (char*)"PickUp", nObjIndex);',
     'bPickOk = ExecuteScript((char*)"\\\\script\\\\item\\\\'
     'tasklink_goods_secret.lua", (char*)"PickUp", nObjIndex, false);'),
]

GHI_CHU = [
    "// [ACTIONSCRIPT 30/08] bGlobal = false. Mac dinh la true (KPlayer.h:857),",
    "// ma nhanh do ghi Npc[m_nIndex].m_ActionScriptID = dwScriptId"
    " (KPlayer.cpp:7115)",
    "// - dung bien ma menu tra loi dieu phoi qua (KPlayer.cpp:7607/7612). Nhat",
    "// cuon giua luc dang mo menu la cuop mat ngu canh hoi thoai: cu bam tiep",
    "// theo goi vao tasklink_goods.lua tim ham khong co o do -> nut im lang.",
    "// ScriptError.log da ghi nhieu lan: cFuncName StationFun / g_DailogBack /",
    "// BDH_Root / ruong / psthanhthi deu tro toi tasklink_goods.lua.",
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t55 va loi cuop m_ActionScriptID - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print()
    raw = doc(P)
    if "[ACTIONSCRIPT 30/08]" in raw:
        print("  da va - bo qua")
        return 0
    dong = raw.split("\n")
    nd_dong = list(dong)
    vi_tri = []
    for cu, moi in VA:
        ci = [i for i, l in enumerate(nd_dong) if cu in l]
        if len(ci) != 1:
            print("!!! LOI TO: moc khop %d dong (can 1): %s"
                  % (len(ci), cu[:60]))
            return 1
        vi_tri.append((ci[0], cu, moi))
    for i, cu, moi in sorted(vi_tri, reverse=True):
        l = nd_dong[i]
        cr = "\r" if l.endswith("\r") else ""
        thut = l[:len(l) - len(l.lstrip())]
        nd_dong[i:i + 1] = ([thut + x + cr for x in GHI_CHU]
                            + [l.replace(cu, moi)])
        print("  dong %d: them bGlobal = false" % (i + 1))
    nd = "\n".join(nd_dong)

    for k in ("{", "}", "(", ")"):
        if raw.count(k) != nd.count(k) - (0 if k in "{}" else 0):
            pass
    if (raw.count("{") - raw.count("}")) != (nd.count("{") - nd.count("}")):
        print("!!! LOI TO: can bang ngoac doi")
        return 1
    print("  can bang ngoac giu nguyen")

    if not ghi:
        print()
        print("DIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + DUOI
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI KPlayer.cpp (ban cu o %s)" % os.path.basename(sao))
    return 0


if __name__ == "__main__":
    sys.exit(main())
