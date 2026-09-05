#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""quet_lua4.py - [LUA54 05/09] QUET TOAN BO script Lua 4.0 de phan loai khac biet voi Lua 5.4.

Khong dung regex mu: co bo tach token Lua 4 (chu thich --, chuoi ' " [[ ]] long nhau, so, ten, %ten upvalue)
nen '%d' trong chuoi hay 'getn' trong chu thich KHONG bi dem nham. Doc latin-1 (giu byte TCVN3).

Muc quet (ma -> y nghia):
  UPVAL     %ten (Lua 4: ban sao chi doc; 5.4: tham chieu). Kem co RUI_RO neu cung ham ngoai co gan lai 'ten ='
            sau vi tri tao closure (ngu nghia co the doi).
  FORIN     for k, v in t do  (5.4: can pairs())
  LIB       goi ham thu vien Lua 4 theo ten cu (getn, tinsert, strfind, format, floor, mod, random, date, ...)
  IO        ham I/O Lua 4 (readfrom, writeto, write, read, closefile, appendto, ...)
  TAG       newtag/settag/settagmethod/gettagmethod/copytagmethods (5.4: metatable)
  VARARG    ham co tham so ... va dung bang arg (5.4 khong tu tao arg)
  DOSTRING  dostring(  (5.4: load)
  CALL      call(f, {args}, "x", handler)  (5.4: pcall/xpcall)
  GLOBALS   globals() / getglobal / setglobal / rawsetglobal / rawgetglobal / foreachvar / nextvar
  ESCAPE    day thoat la trong chuoi (\c khong hop le o 5.4 -> loi cu phap)
  KEYWORD   ten bien trung tu khoa moi: true false goto (5.4) global (5.5)
  DIV       phep chia / (5.4 ra so thuc: 10/2 = 5.0; in chuoi thanh "5.0" neu khong va loi Lua)
  FMTD      format("...%d...") (5.4 loi neu doi so khong nguyen)
  BIGINT    hang so nguyen > 2^31 (Lua 4 %d tran)
  FORASSIGN gan lai bien dieu khien for (5.5 cam)
  NEXT      next(t, k)  (giu nguyen, chi thong ke)

Chay:  python quet_lua4.py <thu muc script> [<thu muc script 2> ...]
Ghi:   quet_lua4_baocao.txt (tom tat + danh sach), quet_lua4.json (moi lan xuat hien, cho bo chuyen)
"""
import io, os, re, sys, json, collections

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

LIB4 = set("""getn tinsert tremove sort foreach foreachi next
strlen strsub strlower strupper strchar strrep ascii strbyte format strfind gsub
abs ceil floor mod frexp ldexp sqrt min max log log10 exp deg rad sin cos tan asin acos atan atan2 random randomseed PI
date clock time
tonumber tostring type print assert error rawget rawset rawgettable rawsettable dofile
collectgarbage gcinfo""".split())
IO4 = set("readfrom writeto appendto read write closefile openfile seek flush remove rename tmpname getenv exit execute setlocale".split())
TAG4 = set("newtag settag tag settagmethod gettagmethod copytagmethods".split())
GLB4 = set("globals getglobal setglobal rawsetglobal rawgetglobal foreachvar nextvar".split())
KW54 = set("true false goto".split())
KW55 = set(["global"])
KW4 = set("and do else elseif end for function if in local nil not or repeat return then until while break".split())
ESC_OK = set("abfnrtv\\\"'\n0123456789")

# ------------------------------------------------------------------ tokenizer
TK_NAME, TK_NUM, TK_STR, TK_OP, TK_UPVAL = "name", "num", "str", "op", "upval"


def tokenize(src):
    """-> list of (kind, text, line). Chu thich bi bo. Chuoi tra ve NOI DUNG THO (ke ca dau ngoac)."""
    toks = []
    i, n, line = 0, len(src), 1
    esc_bad = []      # (line, ky tu)
    while i < n:
        c = src[i]
        if c == "\n":
            line += 1; i += 1; continue
        if c in " \t\r":
            i += 1; continue
        # chu thich
        if c == "-" and src.startswith("--", i):
            if src.startswith("--[[", i):
                j = src.find("]]", i + 4)
                if j < 0: j = n
                line += src.count("\n", i, j)
                i = j + 2; continue
            j = src.find("\n", i)
            if j < 0: j = n
            i = j; continue
        # chuoi dai [[ ]] (long nhau)
        if c == "[" and src.startswith("[[", i):
            depth, j = 1, i + 2
            while j < n and depth:
                if src.startswith("[[", j): depth += 1; j += 2
                elif src.startswith("]]", j): depth -= 1; j += 2
                else: j += 1
            toks.append((TK_STR, src[i:j], line)); line += src.count("\n", i, j); i = j; continue
        # chuoi ' "
        if c in "'\"":
            q, j = c, i + 1
            while j < n and src[j] != q:
                if src[j] == "\\":
                    if j + 1 < n and src[j + 1] not in ESC_OK:
                        esc_bad.append((line, src[j + 1]))
                    j += 2
                    continue
                if src[j] == "\n":
                    line += 1
                j += 1
            toks.append((TK_STR, src[i:j + 1], line)); i = j + 1; continue
        # upvalue %ten
        if c == "%" and i + 1 < n and (src[i + 1].isalpha() or src[i + 1] == "_"):
            j = i + 1
            while j < n and (src[j].isalnum() or src[j] == "_"): j += 1
            toks.append((TK_UPVAL, src[i + 1:j], line)); i = j; continue
        # so
        if c.isdigit() or (c == "." and i + 1 < n and src[i + 1].isdigit()):
            j = i
            while j < n and (src[j].isalnum() or src[j] in ".+-" and (src[j] in "+-" and src[j - 1] in "eE" or src[j] == ".")): j += 1
            toks.append((TK_NUM, src[i:j], line)); i = j; continue
        # ten
        if c.isalpha() or c == "_" or ord(c) >= 128:
            j = i
            while j < n and (src[j].isalnum() or src[j] == "_" or ord(src[j]) >= 128): j += 1
            toks.append((TK_NAME, src[i:j], line)); i = j; continue
        # toan tu (2 ky tu truoc)
        two = src[i:i + 2]
        if two in ("==", "~=", "<=", ">=", ".."):
            if src.startswith("...", i):
                toks.append((TK_OP, "...", line)); i += 3; continue
            toks.append((TK_OP, two, line)); i += 2; continue
        toks.append((TK_OP, c, line)); i += 1
    return toks, esc_bad


def quet_tep(path, rel):
    src = io.open(path, "r", encoding="latin-1").read()
    toks, esc_bad = tokenize(src)
    ra = collections.defaultdict(list)     # ma -> [(line, chi tiet)]
    for ln, ch in esc_bad:
        ra["ESCAPE"].append((ln, "\\" + ch))
    # theo doi ham dang mo de tim VARARG va FORASSIGN / UPVAL rui ro
    stack_fn = []     # moi muc: dict(line, vararg, has_arg, closures: [(name, line)], assigns: [(name, line)])
    for_vars = []     # (ten bien for, muc lồng, dong)
    depth_block = 0
    for idx, (kind, text, ln) in enumerate(toks):
        prev = toks[idx - 1] if idx else (TK_OP, "", 0)
        nxt = toks[idx + 1] if idx + 1 < len(toks) else (TK_OP, "", 0)
        if kind == TK_NAME:
            if text == "function":
                # tham so: tim '(' ... ')'
                j = idx + 1
                while j < len(toks) and toks[j][1] != "(": j += 1
                k = j
                while k < len(toks) and toks[k][1] != ")": k += 1
                params = [t[1] for t in toks[j + 1:k]]
                stack_fn.append(dict(line=ln, vararg=("..." in params), has_arg=False, closures=[], assigns=[]))
                depth_block += 1
                continue
            if text in ("do", "then", "repeat"):
                depth_block += 1
                continue
            if text == "for":
                # for x = ... hoac for k, v in ...
                names = []
                j = idx + 1
                while j < len(toks) and toks[j][1] not in ("=", "in"):
                    if toks[j][0] == TK_NAME: names.append(toks[j][1])
                    j += 1
                if j < len(toks) and toks[j][1] == "in":
                    ra["FORIN"].append((ln, "for " + ", ".join(names) + " in"))
                for nm in names:
                    for_vars.append((nm, depth_block + 1, ln))
                continue
            if text == "end":
                depth_block -= 1
                # dong ham?
                # (khong phan biet chinh xac end cua for/if vs function -> dung dem lồng: function tang depth, end giam)
                for_vars = [fv for fv in for_vars if fv[1] <= depth_block]
                if stack_fn and stack_fn[-1].get("depth_at_open", None) is None:
                    stack_fn[-1]["depth_at_open"] = None
                continue
            if text == "until":
                depth_block -= 1
                continue
            # goi ham thu vien: ten theo sau la '(' hoac chuoi
            is_call = (nxt[1] == "(" or nxt[0] == TK_STR) and not (prev[1] in (".", ":"))
            if is_call:
                if text in LIB4:
                    ra["LIB"].append((ln, text))
                    if text == "format" and nxt[1] == "(":
                        # doi so dau la chuoi co %d?
                        s = toks[idx + 2][1] if idx + 2 < len(toks) and toks[idx + 2][0] == TK_STR else ""
                        if re.search(r"%[-+ #0-9.]*[dicuxXo]", s):
                            ra["FMTD"].append((ln, s[:40]))
                    if text == "next":
                        ra["NEXT"].append((ln, text))
                elif text in IO4:
                    ra["IO"].append((ln, text))
                elif text in TAG4:
                    ra["TAG"].append((ln, text))
                elif text in GLB4:
                    ra["GLOBALS"].append((ln, text))
                elif text == "dostring":
                    ra["DOSTRING"].append((ln, text))
                elif text == "call":
                    ra["CALL"].append((ln, text))
            if text == "arg" and stack_fn:
                stack_fn[-1]["has_arg"] = True
                if stack_fn[-1]["vararg"]:
                    ra["VARARG"].append((ln, "arg trong ham(...) mo o dong %d" % stack_fn[-1]["line"]))
            if text in KW54 and prev[1] != "." and nxt[1] != "=":
                # 'true'/'false' dung nhu bien (Lua 4 khong co) -> se thanh hang boolean o 5.4
                ra["KEYWORD"].append((ln, text))
            elif text in KW54 and nxt[1] == "=":
                ra["KEYWORD"].append((ln, text + " = (gan!)"))
            elif text in KW55 and (nxt[1] == "=" or nxt[1] == "("):
                ra["KEYWORD"].append((ln, text + " (5.5)"))
            # gan lai bien for
            if nxt[1] == "=" and for_vars and any(text == fv[0] for fv in for_vars):
                ra["FORASSIGN"].append((ln, text + " ="))
            # gan trong ham (de danh dau UPVAL rui ro)
            if nxt[1] == "=" and stack_fn:
                stack_fn[-1]["assigns"].append((text, ln))
            continue
        if kind == TK_UPVAL:
            ra["UPVAL"].append((ln, "%" + text))
            if len(stack_fn) >= 2:
                stack_fn[-2]["closures"].append((text, ln))
            continue
        if kind == TK_OP and text == "/":
            ra["DIV"].append((ln, "/"))
            continue
        if kind == TK_NUM:
            try:
                v = float(text)
                if v >= 2147483648 and "." not in text and "e" not in text.lower():
                    ra["BIGINT"].append((ln, text))
            except ValueError:
                pass
    # UPVAL rui ro: closure %x ma ham ngoai gan 'x =' sau dong tao closure
    for fn in stack_fn_all_closed(toks):
        pass
    return ra


def stack_fn_all_closed(toks):
    return []


def danh_dau_upval_rui_ro(path, ra):
    """Heuristic don gian: voi moi %x o dong L, tim 'x =' (khong phai ==) trong 150 dong sau L cung tep."""
    if "UPVAL" not in ra:
        return
    lines = io.open(path, "r", encoding="latin-1").read().split("\n")
    rr = []
    for ln, nm in ra["UPVAL"]:
        x = nm[1:]
        pat = re.compile(r"(?<![\w.:])" + re.escape(x) + r"\s*=[^=]")
        for k in range(ln, min(len(lines), ln + 150)):
            s = lines[k]
            if "--" in s:
                s = s.split("--", 1)[0]
            if pat.search(s):
                rr.append((ln, nm + " (gan lai o dong %d)" % (k + 1)))
                break
    if rr:
        ra["UPVAL_RUIRO"] = rr


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    here = os.path.dirname(os.path.abspath(__file__))
    tong = collections.Counter()       # ma -> so lan
    tep_theo_ma = collections.defaultdict(set)
    lib_ten = collections.Counter()
    io_ten = collections.Counter()
    chi_tiet = {}                       # rel -> {ma: [(line, ct)]}
    so_tep = 0; so_byte = 0; loi_doc = []
    for goc in sys.argv[1:]:
        nhan = os.path.basename(os.path.normpath(goc))
        for d, _, fs in os.walk(goc):
            for f in fs:
                if not f.lower().endswith(".lua"):
                    continue
                p = os.path.join(d, f)
                rel = nhan + "/" + os.path.relpath(p, goc).replace("\\", "/")
                so_tep += 1; so_byte += os.path.getsize(p)
                try:
                    ra = quet_tep(p, rel)
                    danh_dau_upval_rui_ro(p, ra)
                except Exception as e:
                    loi_doc.append((rel, str(e)[:80])); continue
                if ra:
                    chi_tiet[rel] = {k: v for k, v in ra.items()}
                for ma, ds in ra.items():
                    tong[ma] += len(ds); tep_theo_ma[ma].add(rel)
                    if ma == "LIB":
                        for _, t in ds: lib_ten[t] += 1
                    if ma == "IO":
                        for _, t in ds: io_ten[t] += 1
    bc = []
    bc.append("quet_lua4: %d tep .lua, %.1f MB, %d tep loi doc" % (so_tep, so_byte / 1048576.0, len(loi_doc)))
    bc.append("")
    bc.append("%-12s %8s %8s  y nghia" % ("MA", "lan", "tep"))
    YN = dict(UPVAL="%ten upvalue (bo % -> tham chieu)", UPVAL_RUIRO="%ten ma bien ngoai bi gan lai sau do -> phai ra tay",
              FORIN="for k,v in t -> pairs()", LIB="goi ham thu vien ten cu (shim)", IO="ham I/O Lua 4 (shim io.*)",
              TAG="tag method -> metatable (tay)", VARARG="arg trong ham ... (chen local arg = {...})",
              DOSTRING="dostring -> load (shim)", CALL="call -> pcall (shim)", GLOBALS="globals/getglobal/setglobal (shim _G)",
              ESCAPE="day thoat la trong chuoi -> loi cu phap 5.4 (bo chuyen sua)", KEYWORD="ten trung tu khoa moi",
              DIV="phep chia / (5.4 ra so thuc)", FMTD="format %d (5.4 loi neu khong nguyen -> shim)",
              BIGINT="hang so > 2^31", FORASSIGN="gan lai bien for (5.5 cam)", NEXT="next()")
    for ma in ["UPVAL", "UPVAL_RUIRO", "FORIN", "LIB", "IO", "TAG", "VARARG", "DOSTRING", "CALL", "GLOBALS", "ESCAPE", "KEYWORD", "DIV", "FMTD", "BIGINT", "FORASSIGN", "NEXT"]:
        bc.append("%-12s %8d %8d  %s" % (ma, tong.get(ma, 0), len(tep_theo_ma.get(ma, ())), YN.get(ma, "")))
    bc.append("")
    bc.append("HAM THU VIEN Lua 4 theo ten (so lan):")
    bc.append("  " + ", ".join("%s=%d" % kv for kv in lib_ten.most_common()))
    bc.append("HAM I/O Lua 4 theo ten:")
    bc.append("  " + ", ".join("%s=%d" % kv for kv in io_ten.most_common()))
    bc.append("")
    for ma in ["UPVAL_RUIRO", "FORIN", "TAG", "VARARG", "DOSTRING", "CALL", "GLOBALS", "ESCAPE", "KEYWORD", "BIGINT", "FORASSIGN", "IO"]:
        ds = []
        for rel, m in chi_tiet.items():
            for ln, ct in m.get(ma, []):
                ds.append("%s:%d  %s" % (rel, ln, ct))
        if ds:
            bc.append("== %s (%d) ==" % (ma, len(ds)))
            bc.extend("  " + x for x in ds[:400])
            if len(ds) > 400:
                bc.append("  ... (%d nua)" % (len(ds) - 400))
            bc.append("")
    if loi_doc:
        bc.append("LOI DOC:")
        bc.extend("  %s  %s" % x for x in loi_doc)
    io.open(os.path.join(here, "quet_lua4_baocao.txt"), "w", encoding="utf-8").write("\n".join(bc) + "\n")
    io.open(os.path.join(here, "quet_lua4.json"), "w", encoding="utf-8").write(json.dumps(chi_tiet, ensure_ascii=True))
    print("\n".join(bc[:24]))
    print("da ghi quet_lua4_baocao.txt + quet_lua4.json")


if __name__ == "__main__":
    main()
