# -*- coding: utf-8 -*-
"""p07_kiem_ham_relay.py  [RELAYHT 06/09]

KIEM MOT KICH BAN RELAY GOI HAM MA RELAY KHONG CO.

Bay that: goi mot ham khong ton tai trong Lua = "attempt to call a nil value" -> chet
GIUA CHUNG, phan sau khong chay, ma nhat ky chi co mot dong kho hieu. Tren relay lai
cang kho thay vi khong co nguoi choi nao bao. Nen kiem TRUOC khi dat len.

Cach lam: gom ten ham that su duoc dang ky trong nguon C++ cua relay
(g_RelayScriptFuns / g_ShareDataFuns / g_TaskCentreFuns / g_RelayRpcFuns /
GameScriptFuns), cong voi ham chuan cua Lua 5.4 + shim lua4compat, roi doi chieu voi
moi loi goi ham toan cuc trong tep .lua.

Chay:
    python p07_kiem_ham_relay.py <tep .lua hoac thu muc>
Ma thoat 1 neu tim thay ham la.
"""
import io
import os
import re
import sys

SRC = r"D:\GAMEDEVNEW_wt_relay\Sources\MultiServer\S3Relay"
SHIM = r"D:\GAMEDEVNEW_wt_relay\Sources\Library\Lua54\lua4compat.lua"

# Ham chuan Lua 5.4 + cac ham toan cuc kieu Lua 4 ma shim cap (bo sung neu thieu)
CHUAN = set("""
assert collectgarbage dofile error getmetatable ipairs load loadstring next pairs
pcall print rawequal rawget rawlen rawset require select setmetatable tonumber
tostring type unpack xpcall
format strlen strsub strfind strlower strupper strrep strbyte strchar gsub gfind
abs ceil floor max min mod random randomseed sqrt sin cos tan exp log log10 pow
date time clock difftime getn setn tinsert tremove sort foreach foreachi
call rawgettable rawsettable globals settag newtag tag copytagmethods settagmethod
gettagmethod dostring seek openfile closefile readfrom writeto appendto read write
exit getenv remove rename tmpname setlocale
""".split())


def ham_relay():
    ten = set()
    for f in os.listdir(SRC):
        if not f.lower().endswith(".cpp"):
            continue
        d = io.open(os.path.join(SRC, f), encoding="latin-1", newline="").read()
        # { "TenHam", HamC },
        for m in re.finditer(r'\{\s*"([A-Za-z_][A-Za-z0-9_]*)"\s*,', d):
            ten.add(m.group(1))
    return ten


def ham_shim():
    ten = set()
    if os.path.exists(SHIM):
        d = io.open(SHIM, encoding="utf-8", errors="replace").read()
        for m in re.finditer(r'^\s*function\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(', d, re.M):
            ten.add(m.group(1))
        for m in re.finditer(r'^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*function', d, re.M):
            ten.add(m.group(1))
    return ten


def dinh_nghia_trong(p):
    """Ten ham/bien toan cuc mot tep dinh nghia."""
    if not os.path.exists(p):
        return set()
    d = io.open(p, encoding="latin-1", newline="").read()
    d = re.sub(r"--[^\n]*", " ", d)
    ten = set(re.findall(r"function\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(", d))
    ten |= set(re.findall(r"function\s+[A-Za-z_][A-Za-z0-9_.]*[:.]([A-Za-z_][A-Za-z0-9_]*)\s*\(", d))
    ten |= set(re.findall(r"^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*function", d, re.M))
    return ten


def theo_include(p, goc_cay, muc=0, da=None):
    """Gom ten ham cua moi tep ma p Include (de quy, nhu engine)."""
    if da is None:
        da = set()
    if muc > 3 or p.lower() in da or not os.path.exists(p):
        return set()
    da.add(p.lower())
    d = io.open(p, encoding="latin-1", newline="").read()
    ten = set()
    for m in re.finditer(r'Include\s*\(\s*"([^"]+)"', d):
        rel = m.group(1).replace("\\\\", "\\").lstrip("\\")
        con = os.path.join(goc_cay, rel)
        ten |= dinh_nghia_trong(con)
        ten |= theo_include(con, goc_cay, muc + 1, da)
    return ten


def kiem(p, biet):
    d = io.open(p, encoding="latin-1", newline="").read()

    # bo chu thich dong va chuoi de khong bao nham
    d = re.sub(r"--\[\[.*?\]\]", " ", d, flags=re.S)
    d = re.sub(r"--[^\n]*", " ", d)
    d = re.sub(r"\[\[.*?\]\]", '""', d, flags=re.S)
    d = re.sub(r'"[^"\n]*"', '""', d)
    d = re.sub(r"'[^'\n]*'", "''", d)

    # ham TU DINH NGHIA trong chinh tep + bien cuc bo
    tu_co = set(re.findall(r"function\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(", d))
    tu_co |= set(re.findall(r"function\s+[A-Za-z_][A-Za-z0-9_.]*[:.]([A-Za-z_][A-Za-z0-9_]*)\s*\(", d))
    tu_co |= set(re.findall(r"local\s+([A-Za-z_][A-Za-z0-9_]*)", d))
    tu_co |= set(re.findall(r"^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=", d, re.M))

    la = {}
    for m in re.finditer(r"(?<![\w.:])([A-Za-z_][A-Za-z0-9_]*)\s*\(", d):
        ten = m.group(1)
        if ten in ("function", "if", "while", "for", "return", "and", "or", "not", "end", "then", "do", "elseif", "local"):
            continue
        if ten in biet or ten in tu_co:
            continue
        dong = d[: m.start()].count("\n") + 1
        la.setdefault(ten, []).append(dong)
    return la


def main():
    goc = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_relay\serverscript_relay"
    biet = CHUAN | ham_relay() | ham_shim()
    print("Bang doi chieu: %d ten (C++ relay + Lua chuan + shim)" % len(biet))

    ds = []
    if os.path.isfile(goc):
        ds = [goc]
    else:
        for d, _, fs in os.walk(goc):
            for f in fs:
                if f.lower().endswith(".lua") and not f.lower().endswith(".utf8.lua"):
                    ds.append(os.path.join(d, f))

    tong_la = 0
    goc_cay = goc if os.path.isdir(goc) else os.path.dirname(goc)
    for p in sorted(ds):
        la = kiem(p, biet | theo_include(p, goc_cay))
        rel = os.path.relpath(p, goc if os.path.isdir(goc) else os.path.dirname(goc))
        if la:
            print("\n  %s" % rel)
            for ten in sorted(la):
                print("      HAM LA: %-28s dong %s" % (ten, ", ".join(str(x) for x in la[ten][:6])))
                tong_la += 1
        else:
            print("  %-52s OK" % rel)

    print("\n%s" % ("KHONG co ham la." if tong_la == 0 else "CO %d ten ham la -- xem lai truoc khi dat len relay." % tong_la))
    sys.exit(1 if tong_la else 0)


if __name__ == "__main__":
    main()
