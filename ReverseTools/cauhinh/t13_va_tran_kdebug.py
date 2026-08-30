# -*- coding: utf-8 -*-
"""t13_va_tran_kdebug.py - ap LAI rieng phan va TRAN BO DEM trong g_DebugLog.

BOI CANH: t06 tim ra tran that o Engine\\Src\\KDebug.cpp:49-54
      char buffer[256];
      int n = vsprintf(buffer, Fmt, va);      <- KHONG co chan do dai
  Diem goi ScriptFuns.cpp:2483 in ca cau lenh Lua:
      g_DebugLog("[WLLS] DynamicExecute LOI: %.128s -> %.200s", ...)
  = it nhat 128 + 200 + phan co dinh > 256 byte => DAM NGAN XEP.

  t06b hoan tac ca tep de bo phan ghi ra tep (vi Core lien ket engine.lib dung
  san va Engine.vcxproj khong build duoc), nhung da VUT LUON ca mieng va tran -
  bo phan bien bat dung. Nay ap lai RIENG phan tran, khong dinh gi toi ghi tep.

LUU Y THANG THAN: mieng va nay CHUA CO TAC DUNG cho toi khi Engine build lai
duoc (KWin32App.cpp thieu ipc_shared.h). No nam trong ma nguon de lan build
duoc dau tien la co, va de nguoi sau khong tuong da an.

Chi doi 3 dong, khong dong gi khac. Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

P = r"D:\GAMEDEVNEW\Sources\Engine\Src\KDebug.cpp"
NHAN = "[TRANLOG 29/08]"
T = "\t"

CU = [
    T*2 + "char buffer[256];",
    T*2 + "va_list va;",
    "",
    T*2 + "COPYDATASTRUCT data;",
    T*2 + "va_start(va, Fmt);",
    T*2 + "int n = vsprintf(buffer, Fmt, va);",
    T*2 + "va_end(va);",
]
MOI = [
    T*2 + "// " + NHAN + " TRAN BO DEM THAT: vsprintf khong co chan do dai, ma",
    T*2 + "// diem goi ScriptFuns.cpp:2483 in ca cau lenh Lua",
    T*2 + "// (\"%.128s -> %.200s\" = hon 330 byte) vao buffer 256 byte => dam",
    T*2 + "// ngan xep. Dung _vsnprintf co chan va tu ket NUL.",
    T*2 + "char buffer[2048];",
    T*2 + "va_list va;",
    "",
    T*2 + "COPYDATASTRUCT data;",
    T*2 + "va_start(va, Fmt);",
    T*2 + "int n = _vsnprintf(buffer, sizeof(buffer) - 1, Fmt, va);",
    T*2 + "va_end(va);",
    T*2 + "if (n < 0)\t\t\t// bi cat bot: _vsnprintf khong tu ket NUL",
    T*3 + "n = (int)sizeof(buffer) - 1;",
    T*2 + "buffer[n] = 0;",
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t13_va_tran_kdebug - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    raw = doc(P)
    if NHAN in raw:
        print("  DA VA - bo qua")
        return 0
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    cu = eol.join(CU)
    n = raw.count(cu)
    if n != 1:
        print("!!! LOI TO: moc khop %d lan (can 1)" % n)
        print("    moc:\n%s" % cu.replace("\t", "->"))
        return 1
    nd = raw.replace(cu, eol.join(MOI))
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    for cap in ("{}", "()"):
        if nd.count(cap[0]) - nd.count(cap[1]) != raw.count(cap[0]) - raw.count(cap[1]):
            print("!!! LOI TO: ngoac %s lech" % cap)
            return 1
    print("  doi buffer[256]+vsprintf -> buffer[2048]+_vsnprintf co chan")
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + ".truoc_tranlog"
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI KDebug.cpp")
    print("\nCHUA CO TAC DUNG cho toi khi Engine.vcxproj build lai duoc.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
