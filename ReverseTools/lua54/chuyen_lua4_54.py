#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""chuyen_lua4_54.py - [LUA54 05/09] BO CHUYEN script Lua 4.0 -> Lua 5.4 (byte-safe, giu TCVN3 + CRLF).

Chi doi CU PHAP khong con hop le / doi nghia o 5.4; ten ham thu vien cu (getn, format, floor...)
GIU NGUYEN vi lop lua4compat.lua cung cap lai. Cac phep bien doi:
  1. %ten          -> ten            (upvalue Lua 4 -> upvalue that cua 5.4)
  2. for a, b in T do -> for a, b in pairs(T) do   (Lua 4 'in' nhan bang; 5.4 can iterator)
  3. "\c" thoat la -> "c"            (Lua 4 bo dau gach cheo; 5.4 bao loi cu phap)
  4. function(...)  -> chen 'local arg = {n = select("#", ...), ...}' dau than ham
                       (Lua 4 tu tao bang arg; 5.4 khong)
  5. [[ [[ ]] ]] long nhau -> [=[ ]=]   (5.4 khong cho long)
Moi thay doi duoc ghi vao bao cao (tep:dong: truoc -> sau).

Chay:  python chuyen_lua4_54.py <thu muc nguon> <thu muc dich> [--baocao tep.txt]
       (sao chep ca cay, doi .lua; tep khac copy nguyen)
"""
import io, os, sys, re, shutil

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

ESC_OK = set("abfnrtv\\\"'\n0123456789")
KW = set("and do else elseif end for function if in local nil not or repeat return then until while break".split())


def token_hoa(src):
    """Tach token co vi tri (start, end) de sua tai cho. kind: cmt str name num op upval ws."""
    toks = []
    i, n = 0, len(src)
    while i < n:
        c = src[i]
        if c in " \t\r\n":
            j = i
            while j < n and src[j] in " \t\r\n": j += 1
            toks.append(["ws", i, j]); i = j; continue
        if c == "-" and src.startswith("--", i):
            if src.startswith("--[[", i):
                j = src.find("]]", i + 4); j = n if j < 0 else j + 2
            else:
                j = src.find("\n", i); j = n if j < 0 else j
            toks.append(["cmt", i, j]); i = j; continue
        if c == "[" and src.startswith("[[", i):
            depth, j = 1, i + 2
            while j < n and depth:
                if src.startswith("[[", j): depth += 1; j += 2
                elif src.startswith("]]", j): depth -= 1; j += 2
                else: j += 1
            toks.append(["lstr", i, j]); i = j; continue
        if c in "'\"":
            q, j = c, i + 1
            while j < n and src[j] != q:
                if src[j] == "\\": j += 2; continue
                j += 1
            toks.append(["str", i, j + 1]); i = j + 1; continue
        if c == "%" and i + 1 < n and (src[i + 1].isalpha() or src[i + 1] == "_"):
            j = i + 1
            while j < n and (src[j].isalnum() or src[j] == "_"): j += 1
            toks.append(["upval", i, j]); i = j; continue
        if c.isdigit() or (c == "." and i + 1 < n and src[i + 1].isdigit()):
            # so theo luat Lua 4 (llex.c read_number): chu so, mot dau '.', phan mu e/E[+-]chu so; KHONG nuot chu cai
            # (Lua 4 doc '7then' thanh 7 + then; 5.4 bao 'malformed number' -> bo chuyen chen dau cach)
            j = i
            while j < n and src[j].isdigit(): j += 1
            if j < n and src[j] == ".":
                j += 1
                while j < n and src[j].isdigit(): j += 1
            if j < n and src[j] in "eE":
                k2 = j + 1
                if k2 < n and src[k2] in "+-": k2 += 1
                if k2 < n and src[k2].isdigit():
                    j = k2
                    while j < n and src[j].isdigit(): j += 1
            toks.append(["num", i, j])
            if j < n and (src[j].isalpha() or src[j] == "_"):
                toks.append(["numkw", j, j])      # danh dau: can chen dau cach
            i = j; continue
        if c.isalpha() or c == "_" or ord(c) >= 128:
            j = i
            while j < n and (src[j].isalnum() or src[j] == "_" or ord(src[j]) >= 128): j += 1
            toks.append(["name", i, j]); i = j; continue
        if src.startswith("...", i):
            toks.append(["op", i, i + 3]); i += 3; continue
        two = src[i:i + 2]
        if two in ("==", "~=", "<=", ">=", ".."):
            toks.append(["op", i, i + 2]); i += 2; continue
        toks.append(["op", i, i + 1]); i += 1
    return toks


def dong_cua(src, pos):
    return src.count("\n", 0, pos) + 1


def sua_chuoi(s):
    """Sua day thoat la trong literal ' " : \c -> c (nhu Lua 4)."""
    out = []
    i, n = 0, len(s)
    while i < n:
        c = s[i]
        if c == "\\" and i + 1 < n:
            d = s[i + 1]
            if d in ESC_OK:
                out.append(c); out.append(d); i += 2; continue
            out.append(d); i += 2; continue            # bo dau \
        out.append(c); i += 1
    return "".join(out)


def sua_lstr(s):
    """[[ ... [[ ... ]] ... ]] -> [=[ ... ]=] (giu noi dung)."""
    if s.count("[[") <= 1:
        return s
    return "[=[" + s[2:-2] + "]=]"


def chuyen(src, rel, baocao):
    toks = token_hoa(src)
    # ---- chuan bi cac vi tri sua (pos, end, new)
    edits = []
    sonhap = {"UPVAL": 0, "FORIN": 0, "ESCAPE": 0, "VARARG": 0, "LSTR": 0}
    ntok = len(toks)
    # bo qua ws/cmt khi nhin token ke
    def ke(k, buoc=1):
        j = k + buoc
        while 0 <= j < ntok and toks[j][0] in ("ws", "cmt"):
            j += buoc
        return j if 0 <= j < ntok else None
    def txt(k):
        return src[toks[k][1]:toks[k][2]] if k is not None else ""
    # Vung bieu thuc 'for ... in EXPR do' se duoc boc pairs(...) va tu bo % ben trong -> upval trong vung do khong sua rieng
    vung_forin = []
    for k, t in enumerate(toks):
        kind, a, b = t
        if kind == "numkw":
            edits.append((a, a, " ")); sonhap["NUMKW"] = sonhap.get("NUMKW", 0) + 1
            baocao.append("%s:%d  so dinh chu (vd 7then) -> chen dau cach" % (rel, dong_cua(src, a)))
            continue
        if kind == "upval":
            if any(x <= a < y for x, y in vung_forin):
                continue
            edits.append((a, a + 1, ""))          # bo dau %
            sonhap["UPVAL"] += 1
            baocao.append("%s:%d  %%%s -> %s" % (rel, dong_cua(src, a), src[a + 1:b], src[a + 1:b]))
        elif kind == "str":
            s = src[a:b]
            s2 = sua_chuoi(s)
            if s2 != s:
                edits.append((a, b, s2)); sonhap["ESCAPE"] += 1
                baocao.append("%s:%d  %s -> %s" % (rel, dong_cua(src, a), s.replace("\n", "\\n")[:60], s2.replace("\n", "\\n")[:60]))
        elif kind == "lstr":
            s = src[a:b]
            s2 = sua_lstr(s)
            if s2 != s:
                edits.append((a, b, s2)); sonhap["LSTR"] += 1
                baocao.append("%s:%d  [[ long nhau -> [=[ ]=]" % (rel, dong_cua(src, a)))
        elif kind == "name" and src[a:b] == "for":
            # tim 'in' truoc 'do' (cung cau)
            j = ke(k)
            names = []
            while j is not None and txt(j) not in ("=", "in", "do") and toks[j][0] in ("name", "op", "ws", "cmt"):
                if toks[j][0] == "name": names.append(txt(j))
                j = ke(j)
            if j is not None and txt(j) == "in":
                # bieu thuc tu sau 'in' den 'do' (cung muc ngoac)
                m = ke(j)
                dau = toks[m][1] if m is not None else None
                depth = 0; q = m; cuoi = None
                while q is not None:
                    s = txt(q)
                    if s in ("(", "{", "["): depth += 1
                    elif s in (")", "}", "]"): depth -= 1
                    elif s == "do" and depth == 0:
                        cuoi = toks[q][1]; break
                    q = q + 1 if q + 1 < ntok else None
                if dau is not None and cuoi is not None:
                    bieu = src[dau:cuoi].rstrip()
                    dau_bieu = dau
                    cuoi_bieu = dau + len(bieu)
                    # da la pairs(...) / next, ... / ham() thi thoi
                    if not re.match(r"^(pairs|ipairs|next)\b", bieu) and "," not in bieu:
                        bieu_moi = re.sub(r"%(?=[A-Za-z_])", "", bieu)      # %upvalue trong bieu thuc -> bo %
                        if bieu_moi != bieu:
                            sonhap["UPVAL"] += bieu.count("%") - bieu_moi.count("%")
                        vung_forin.append((dau_bieu, cuoi_bieu))
                        khoang = "" if dau_bieu > 0 and src[dau_bieu - 1] in " \t\r\n" else " "   # 'in(tb)' -> 'in pairs((tb))'
                        edits.append((dau_bieu, cuoi_bieu, khoang + "pairs(" + bieu_moi + ")")); sonhap["FORIN"] += 1
                        baocao.append("%s:%d  for %s in %s -> pairs(%s)" % (rel, dong_cua(src, a), ", ".join(names), bieu, bieu_moi))
        elif kind == "name" and src[a:b] == "function":
            # tham so: ( ... )
            j = ke(k)
            while j is not None and txt(j) != "(":
                j = ke(j)
            if j is None: continue
            q = j; vararg = False
            while q is not None and txt(q) != ")":
                if txt(q) == "...": vararg = True
                q = q + 1 if q + 1 < ntok else None
            if vararg and q is not None:
                # chen sau ')' : giu thut dau dong cua dong ke
                pos = toks[q][2]
                edits.append((pos, pos, " local arg = {n = select(\"#\", ...), ...};")); sonhap["VARARG"] += 1
                baocao.append("%s:%d  function(...) -> + local arg = {n=select('#',...), ...}" % (rel, dong_cua(src, a)))
    if not edits:
        return src, sonhap
    edits.sort(key=lambda e: e[0])
    out = []; last = 0
    for a, b, new in edits:
        if a < last:      # chong lan (khong xay ra) -> bo qua
            continue
        out.append(src[last:a]); out.append(new); last = b
    out.append(src[last:])
    return "".join(out), sonhap


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    nguon, dich = sys.argv[1], sys.argv[2]
    bc_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "chuyen_baocao.txt")
    if "--baocao" in sys.argv:
        bc_path = sys.argv[sys.argv.index("--baocao") + 1]
    baocao = []; tong = {}; so_tep = 0; so_doi = 0
    for d, dirs, fs in os.walk(nguon):
        rel_d = os.path.relpath(d, nguon)
        dd = os.path.join(dich, rel_d) if rel_d != "." else dich
        os.makedirs(dd, exist_ok=True)
        for f in fs:
            p = os.path.join(d, f); q = os.path.join(dd, f)
            if not f.lower().endswith(".lua"):
                shutil.copyfile(p, q); continue
            so_tep += 1
            src = io.open(p, "r", encoding="latin-1", newline="").read()
            rel = os.path.relpath(p, nguon).replace("\\", "/")
            moi, sn = chuyen(src, rel, baocao)
            for k, v in sn.items(): tong[k] = tong.get(k, 0) + v
            if moi != src: so_doi += 1
            io.open(q, "w", encoding="latin-1", newline="").write(moi)
    head = ["chuyen_lua4_54: %d tep .lua, %d tep co thay doi" % (so_tep, so_doi),
            "  " + ", ".join("%s=%d" % kv for kv in sorted(tong.items())), ""]
    io.open(bc_path, "a", encoding="utf-8").write("\n".join(head + baocao) + "\n")
    print("\n".join(head[:2]))
    print("bao cao:", bc_path)


if __name__ == "__main__":
    main()
