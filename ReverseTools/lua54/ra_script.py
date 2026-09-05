#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""ra_script.py - [LUA54] RA TOAN BO CAY SCRIPT (ban 5.4 da chuyen): tim phan can sua cho GON va TANG HIEU NANG.

Doc byte-safe (latin-1), khong sua gi. Cho moi tep .lua dem:
  * ham Lua 4 cu (goi qua shim): getn/tinsert/tremove, strfind/strsub/strlen/strlower/strupper/strrep/strbyte/
    strchar/gsub/format, floor/ceil/abs/sqrt/max/min/mod/random, date/clock, dostring/call/foreach/foreachi
  * mau HIEU NANG THAP:
      - while ... getn(...)      : dieu kien while goi getn moi vong (for thi chi tinh 1 lan)
      - s = s .. x trong vong lap : ghep chuoi O(n^2)
      - Include( trong THAN HAM  : nap lai tep moi lan goi
      - dostring(               : bien dich chuoi moi lan goi
      - ham > 300 dong, tep > 3000 dong
      - bien toan cuc gan o dau cot (khong local) - o nhiem _G, tra bang toan cuc cham hon local
  * dau hieu can xem tay: true/false dung lam bien, %upvalue, arg
Ket qua: bang tong hop + top tep theo "no hien dai hoa" va theo "rui ro hieu nang", ghi Markdown.

Chay: python ra_script.py <thu muc script> [--baocao ra_baocao.md] [--top 40]
"""
import io, os, re, sys, argparse
from collections import Counter, defaultdict

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

HAM_CU = {
    "bang": ["getn", "tinsert", "tremove", "foreach", "foreachi", "sort"],
    "chuoi": ["strfind", "strsub", "strlen", "strlower", "strupper", "strrep", "strbyte", "strchar", "gsub", "format"],
    "toan": ["floor", "ceil", "abs", "sqrt", "max", "min", "mod", "random", "randomseed"],
    "os": ["date", "clock"],
    "dong": ["dostring", "call", "dofile"],
}
TEN_CU = [h for v in HAM_CU.values() for h in v]
RE_GOI = re.compile(r"(?<![\w.:])(" + "|".join(TEN_CU) + r")\s*\(")
RE_DINH_NGHIA = re.compile(r"(?:^|[^\w.])(?:function\s+(" + "|".join(TEN_CU) + r")\s*\(|local\s+(?:function\s+)?(" + "|".join(TEN_CU) + r")\b|(" + "|".join(TEN_CU) + r")\s*=[^=])")
RE_WHILE_GETN = re.compile(r"\bwhile\b[^\n]*\bgetn\s*\(")
RE_INCLUDE = re.compile(r"\bInclude\s*\(")
RE_DOSTRING = re.compile(r"\bdostring\s*\(")
RE_FUNC = re.compile(r"^\s*(?:local\s+)?function\b|\bfunction\s*\(", re.M)
RE_GLOBAL_GAN = re.compile(r"^([A-Za-z_][A-Za-z0-9_]*)\s*=[^=]", re.M)
RE_TRUEFALSE_BIEN = re.compile(r"\b(?:true|false)\s*=[^=]|\blocal\s+(?:true|false)\b|\bfunction\s+\w+\s*\([^)]*\b(?:true|false)\b")
RE_UPVAL = re.compile(r"%[A-Za-z_]")
RE_ARG = re.compile(r"\barg\b")
RE_CONCAT_GAN = re.compile(r"^\s*([A-Za-z_][\w.\[\]]*)\s*=\s*\1\s*\.\.", re.M)


def bo_chu_thich_va_chuoi(s):
    """Xoa chu thich (--, --[[ ]]) va noi dung chuoi de dem cho dung. Giu so dong."""
    ra = []
    i, n = 0, len(s)
    while i < n:
        c = s[i]
        if c == "-" and s.startswith("--", i):
            if s.startswith("--[[", i):
                j = s.find("]]", i + 4)
                j = n if j < 0 else j + 2
            else:
                j = s.find("\n", i)
                j = n if j < 0 else j
            ra.append("\n" * s.count("\n", i, j)); i = j; continue
        if c in "\"'":
            j = i + 1
            while j < n and s[j] != c:
                if s[j] == "\\": j += 1
                if j < n and s[j] == "\n": break
                j += 1
            ra.append('""'); i = j + 1; continue
        if c == "[" and s.startswith("[[", i):
            j = s.find("]]", i + 2)
            j = n if j < 0 else j + 2
            ra.append('""' + "\n" * s.count("\n", i, j)); i = j; continue
        ra.append(c); i += 1
    return "".join(ra)


def do_dai_ham(code):
    """Do dai (dong) cua tung ham theo function/end can bang tho."""
    ket = []
    mo = []
    tk = re.compile(r"\b(function|if|for|while|do|repeat|end|until)\b")
    dong = 1
    for m in tk.finditer(code):
        dong = code.count("\n", 0, m.start()) + 1
        w = m.group(1)
        if w == "function": mo.append(("f", dong))
        elif w in ("if", "for", "while", "repeat"): mo.append(("b", dong))
        elif w == "do":
            # 'for..do' / 'while..do' da mo o tu khoa truoc; 'do' don le mo khoi
            j = m.start() - 1
            while j >= 0 and code[j] in " \t": j -= 1
            truoc = code[max(0, j - 40):j + 1]
            if not re.search(r"\b(for|while)\b[^\n]*$", truoc): mo.append(("b", dong))
        elif w in ("end", "until"):
            if mo:
                loai, d0 = mo.pop()
                if loai == "f": ket.append(dong - d0 + 1)
    return ket


def ra_tep(p, goc):
    raw = io.open(p, "r", encoding="latin-1", newline="").read()
    code = bo_chu_thich_va_chuoi(raw)
    r = {"tep": os.path.relpath(p, goc).replace("\\", "/"), "dong": raw.count("\n") + 1}
    goi = Counter(m.group(1) for m in RE_GOI.finditer(code))
    dn = set()
    for m in RE_DINH_NGHIA.finditer(code):
        dn.add(m.group(1) or m.group(2) or m.group(3))
    r["goi_cu"] = goi
    r["dinh_nghia_trung_ten"] = sorted(dn)
    r["tong_cu"] = sum(goi.values())
    r["while_getn"] = len(RE_WHILE_GETN.findall(code))
    r["dostring"] = len(RE_DOSTRING.findall(code))
    # Include trong than ham: Include o dong co thut dau (khong o cot 0)
    r["include_tong"] = len(RE_INCLUDE.findall(code))
    r["include_trong_ham"] = sum(1 for m in RE_INCLUDE.finditer(code) if code.rfind("\n", 0, m.start()) >= 0 and code[code.rfind("\n", 0, m.start()) + 1:m.start()].strip() != "")
    # ghep chuoi tich luy (x = x .. ...) - dem, va xem co trong vong lap khong (thut dau >= 1 muc)
    cg = RE_CONCAT_GAN.findall(code)
    r["concat_tichluy"] = len(cg)
    r["concat_trong_lap"] = 0
    for m in RE_CONCAT_GAN.finditer(code):
        truoc = code[max(0, m.start() - 2000):m.start()]
        if re.search(r"\b(for|while|repeat)\b[^\n]*\n(?:(?!\bend\b).)*$", truoc, re.S):
            r["concat_trong_lap"] += 1
    r["so_ham"] = len(RE_FUNC.findall(code))
    dd = do_dai_ham(code)
    r["ham_dai"] = sum(1 for d in dd if d > 300)
    r["ham_dai_nhat"] = max(dd) if dd else 0
    r["global_gan"] = len(set(RE_GLOBAL_GAN.findall(code)))
    r["truefalse_bien"] = len(RE_TRUEFALSE_BIEN.findall(code))
    r["upval"] = len(RE_UPVAL.findall(code))
    r["arg"] = len(RE_ARG.findall(code))
    r["diem_hieunang"] = (r["while_getn"] * 5 + r["dostring"] * 4 + r["include_trong_ham"] * 4 +
                          r["concat_trong_lap"] * 2 + r["ham_dai"] * 2 + (2 if r["dong"] > 3000 else 0))
    return r


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("goc")
    ap.add_argument("--baocao", default="ra_baocao.md")
    ap.add_argument("--top", type=int, default=40)
    a = ap.parse_args()
    ds = []
    for dp, _, fs in os.walk(a.goc):
        for f in fs:
            if f.lower().endswith(".lua"):
                try:
                    ds.append(ra_tep(os.path.join(dp, f), a.goc))
                except Exception as e:
                    print("LOI", f, e)
    tong = Counter()
    for r in ds:
        tong.update(r["goi_cu"])
    n_tep = len(ds)
    n_dong = sum(r["dong"] for r in ds)
    out = []
    out.append("# Ra cay script %s - %d tep, %d dong\n" % (a.goc, n_tep, n_dong))
    out.append("## 1. Goi ham Lua 4 cu (qua shim lua4compat) - tong theo ten\n")
    out.append("| nhom | ham | so lan goi | so tep |\n|---|---|---:|---:|")
    for nhom, hs in HAM_CU.items():
        for h in hs:
            if tong[h]:
                out.append("| %s | %s | %d | %d |" % (nhom, h, tong[h], sum(1 for r in ds if r["goi_cu"][h])))
    out.append("\nTong goi cu: %d trong %d tep.\n" % (sum(tong.values()), sum(1 for r in ds if r["tong_cu"])))
    # dinh nghia trung ten (khong duoc doi ten may moc)
    trung = defaultdict(list)
    for r in ds:
        for t in r["dinh_nghia_trung_ten"]:
            trung[t].append(r["tep"])
    if trung:
        out.append("## 2. Tep TU DINH NGHIA ham trung ten shim (doi ten may moc phai TRANH cac tep nay)\n")
        for t, fs in sorted(trung.items()):
            out.append("- `%s`: %d tep: %s" % (t, len(fs), ", ".join(fs[:6]) + (" ..." if len(fs) > 6 else "")))
    out.append("\n## 3. Mau hieu nang thap - tong\n")
    for k, ten in [("while_getn", "while ... getn(...) (goi getn moi vong)"), ("dostring", "dostring( (bien dich moi lan goi)"),
                   ("include_trong_ham", "Include( trong than ham (nap lai tep moi lan)"), ("concat_trong_lap", "x = x .. y trong vong lap (O(n^2))"),
                   ("ham_dai", "ham > 300 dong"), ("truefalse_bien", "true/false dung lam bien"), ("arg", "dung 'arg' (vararg cu)")]:
        n = sum(r[k] for r in ds); nt = sum(1 for r in ds if r[k])
        out.append("- %s: **%d** cho / %d tep" % (ten, n, nt))
    out.append("- tep > 3000 dong: %s" % ", ".join("%s(%d)" % (r["tep"], r["dong"]) for r in ds if r["dong"] > 3000))
    out.append("\n## 4. Top %d tep theo RUI RO HIEU NANG\n" % a.top)
    out.append("| tep | dong | while_getn | dostring | Include/ham | concat/lap | ham>300 | diem |\n|---|---:|---:|---:|---:|---:|---:|---:|")
    for r in sorted(ds, key=lambda x: -x["diem_hieunang"])[:a.top]:
        if r["diem_hieunang"] == 0: break
        out.append("| %s | %d | %d | %d | %d | %d | %d | %d |" % (r["tep"], r["dong"], r["while_getn"], r["dostring"], r["include_trong_ham"], r["concat_trong_lap"], r["ham_dai"], r["diem_hieunang"]))
    out.append("\n## 5. Top %d tep theo SO GOI HAM CU (no hien dai hoa)\n" % a.top)
    out.append("| tep | dong | goi cu | chi tiet |\n|---|---:|---:|---|")
    for r in sorted(ds, key=lambda x: -x["tong_cu"])[:a.top]:
        ct = ", ".join("%s:%d" % (k, v) for k, v in r["goi_cu"].most_common(6))
        out.append("| %s | %d | %d | %s |" % (r["tep"], r["dong"], r["tong_cu"], ct))
    out.append("\n## 6. Tep can xem tay\n")
    for k, ten in [("truefalse_bien", "true/false lam bien"), ("while_getn", "while getn"), ("dostring", "dostring"), ("include_trong_ham", "Include trong ham")]:
        fs = [("%s(%d)" % (r["tep"], r[k])) for r in ds if r[k]]
        if fs:
            out.append("- **%s** (%d tep): %s" % (ten, len(fs), ", ".join(fs[:40]) + (" ..." if len(fs) > 40 else "")))
    txt = "\n".join(out) + "\n"
    io.open(a.baocao, "w", encoding="utf-8", newline="\n").write(txt)
    print("\n".join(out[:4]))
    print("... bao cao day du:", a.baocao)
    print("tong goi cu: %d; tep co rui ro hieu nang: %d; while_getn: %d; dostring: %d; Include trong ham: %d; concat trong lap: %d" % (
        sum(tong.values()), sum(1 for r in ds if r["diem_hieunang"]), sum(r["while_getn"] for r in ds), sum(r["dostring"] for r in ds),
        sum(r["include_trong_ham"] for r in ds), sum(r["concat_trong_lap"] for r in ds)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
