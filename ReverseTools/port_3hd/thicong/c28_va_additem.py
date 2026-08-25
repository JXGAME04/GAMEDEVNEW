# -*- coding: ascii -*-
"""C28 - LOI CHAN: 10 loi goi AddItem chi co 6 THAM SO trong script da port.
JX1 LuaAddItem (ScriptFuns.cpp): `if (nParamNum < 7) { push 0; return; }` => KHONG
TAO ITEM VA IM LANG. Engine Linux chap nhan 6 nen ban goc viet 6.
Hau qua that: giet boss KHONG ra sat thu lenh; hop thanh KHONG ra sat thu gian;
thuong Vuot Ai khong vao tui.
Va: them tham so thu 7 = nItemLevel[0] = 0 (vat pham thuong, khong phai trang bi).
Vi tri:
  lib_killlevel.lua  8 cho  AddItem(6,1,398,<cap>,series,0)      -> them ,0
  nieshichen.lua     1 cho  AddItem(6,1,399,level,attr[i],0)     -> them ,0
  award.lua          1 cho  AddItem(item[2]..item[7])            -> them ,0
"""
import io, os, re, sys, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
TEP = [
    r"script\task\tollgate\killer\lib_killlevel.lua",
    r"script\task\tollgate\killer\nieshichen.lua",
    r"script\missions\challengeoftime\award.lua",
]


def dem_arg(s):
    out, cur, dep = [], "", 0
    for ch in s:
        if ch in "({[":
            dep += 1
        elif ch in ")}]":
            if dep == 0:
                break
            dep -= 1
        if ch == "," and dep == 0:
            out.append(cur)
            cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur)
    return len(out)


tong = 0
for rel in TEP:
    p = os.path.join(JX1, rel)
    d = io.open(p, encoding="latin-1", newline="").read()
    lines = d.split("\n")
    sua = 0
    for i, line in enumerate(lines):
        if line.lstrip().startswith("--"):
            continue
        moi = line
        # xu ly tu PHAI sang TRAI de khong lech chi so khi chen
        for m in reversed(list(re.finditer(r"\bAddItem\s*\(", moi))):
            phan = moi[m.end():]
            if dem_arg(phan) >= 7:
                continue
            # tim dau ')' dong cua loi goi nay
            dep = 0
            k = -1
            for j, ch in enumerate(phan):
                if ch in "({[":
                    dep += 1
                elif ch in ")}]":
                    if dep == 0:
                        k = j
                        break
                    dep -= 1
            if k < 0:
                continue
            moi = moi[:m.end() + k] + ", 0" + moi[m.end() + k:]
            sua += 1
        if moi != line:
            lines[i] = moi
    if sua:
        io.open(p, "w", encoding="latin-1", newline="").write("\n".join(lines))
        dst = os.path.join(MIR, rel)
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.copyfile(p, dst)
        tong += sua
        print("  OK %-50s +%d tham so" % (rel.rsplit("\\", 1)[-1], sua))
print("da va", tong, "loi goi AddItem (them tham so thu 7 = 0)")
