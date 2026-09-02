# -*- coding: utf-8 -*-
"""vhtd_lua_split.py [VHTD 02/09e] - tách bảng SKILLS={ a={...}, b={...} } của wuhuntang.lua / xiaoyao.lua (server + client) thành
SKILLS={} rồi SKILLS.a={...}; SKILLS.b={...}; ...  GỐC LỖI: Lua 4.0 (stack cố định 1024 khi lua_open(0)) đánh giá MỘT constructor
lồng 5 tầng với hàng chục trường treo trên stack -> "stack Overflow" ngay tại chunk (lua4 của engine: chạy chính thì vừa khít,
dofile/khách gọi từ C thì tràn) -> client ScriptError 4 (KLuaScript::ExecuteCode) -> SKILLS không tồn tại -> mọi kỹ năng Vũ Hồn
không có thuộc tính. Tách từng bảng = mỗi câu lệnh một constructor nhỏ.
Giữ nguyên byte mọi phần khác (chú thích GBK, hàm cuối tệp). Idempotent. Kiểm bằng lua4.exe (--lua4=<exe>): dofile + so giá trị
GetSkillLevelData mọi (bảng, thuộc tính) cấp 1/10/20 trước-sau. DUNG: python vhtd_lua_split.py [--kiem] [--lua4=path]
"""
import io, os, sys, re, shutil, subprocess, tempfile
KIEM = "--kiem" in sys.argv
LUA4 = next((a.split("=", 1)[1] for a in sys.argv if a.startswith("--lua4=")), None)
BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
BAK = ".truoc_vhtd_split_0209"
FILES = [os.path.join(BIN, side, "script", "skill", n + ".lua") for n in ("wuhuntang", "xiaoyao") for side in ("server", "client")]

def split_skills(s):
    nl = "\r\n" if "\r\n" in s else "\n"
    m = re.search(r"(?m)^SKILLS\s*=\s*\{[ \t]*\r?$", s)
    if not m:
        if re.search(r"(?m)^SKILLS\s*=\s*\{\}", s): return None   # da tach
        raise SystemExit("khong thay 'SKILLS={' dau dong")
    start = m.end()                      # sau dong SKILLS={
    # quet ngoac de tim dau } dong bang SKILLS (bo qua chuoi va chu thich)
    i = s.index("{", m.start())           # vi tri '{' (m.end() la cuoi dong, sau CR)
    depth = 0; j = i; n = len(s)
    entries = []                         # (name, value_start, value_end) o depth 1
    cur_name = None; cur_start = None
    while j < n:
        c = s[j]
        if c == "-" and s.startswith("--", j):
            k = s.find("\n", j); j = n if k < 0 else k; continue
        if c == '"' or c == "'":
            q = c; j += 1
            while j < n and s[j] != q:
                if s[j] == "\\": j += 1
                j += 1
            j += 1; continue
        if c == "{":
            depth += 1
            if depth == 2 and cur_name is not None and cur_start is None: cur_start = j
        elif c == "}":
            depth -= 1
            if depth == 1 and cur_name is not None and cur_start is not None:
                entries.append((cur_name, cur_start, j + 1)); cur_name = None; cur_start = None
            if depth == 0: break
        elif depth == 1 and cur_name is None and (c.isalpha() or c == "_"):
            mm = re.match(r"(\w+)\s*=", s[j:])
            if mm:
                cur_name = (mm.group(1), j, j + mm.end()); j += mm.end(); continue
        j += 1
    if depth != 0: raise SystemExit("ngoac SKILLS khong dong")
    end_brace = j                        # vi tri '}' dong SKILLS
    if not entries: raise SystemExit("khong tim thay bang con")
    # dung lai: SKILLS={}  + moi bang: SKILLS.<ten>=<value>  (giu chu thich/khoang trang giua cac bang nguyen ven, bo dau ',' phan cach)
    out = [s[:m.start()], "SKILLS={}\t-- [VHTD 02/09e] tach bang: tranh 'stack Overflow' Lua 4 khi 1 constructor lon (client ScriptError 4)", nl]
    pos = start
    for (name, name_pos, name_end), vs, ve in entries:
        between = s[pos:name_pos]
        between = re.sub(r"^[ \t]*,", "", between)          # dau ',' cua bang truoc
        out.append(between.replace(nl + "\t", nl) if between.strip() else between)
        out.append("SKILLS." + name + "=")
        out.append(s[name_end:vs])          # chu thich/xuong dong giua "=" va "{" (vd jingzhongbaoguo= --...)
        out.append(s[vs:ve])
        pos = ve
        # bo dau ',' ngay sau value
        while pos < end_brace and s[pos] in " \t": pos += 1
        if pos < end_brace and s[pos] == ",": pos += 1
    tail_between = s[pos:end_brace]
    out.append(tail_between if tail_between.strip() else "")
    # sau dau '}' dong SKILLS: bo qua ky tu '}' va co the ';' roi noi phan con lai
    rest = s[end_brace + 1:]
    out.append(rest)
    return "".join(out), len(entries)

def lua_dump(exe, luafile):
    """chay lua4: dofile + in GetSkillLevelData cho moi (bang, thuoc tinh) cap 1/10/20"""
    d = os.path.dirname(luafile); base = os.path.basename(luafile)
    test = ("dofile(\"%s\")\nfor tb, v in SKILLS do if type(v) == \"table\" then for k, _ in v do "
            "for lv = 1, 20, 9 do local r = GetSkillLevelData(k, tb, lv) if type(r) == \"string\" then print(tb, k, lv, r) else print(tb, k, lv, tostring(r)) end end end end end\n") % base
    tp = os.path.join(d, "_t_dump.lua")
    io.open(tp, "w", encoding="latin-1", newline="").write(test)
    try:
        r = subprocess.run([exe, "_t_dump.lua"], cwd=d, capture_output=True, timeout=60)
        return r.returncode, r.stdout, r.stderr
    finally:
        os.remove(tp)

for p in FILES:
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    res = split_skills(s)
    if res is None: print("  [=] %s da tach" % p); continue
    s2, ne = res
    if sum(1 for c in s2 if ord(c) >= 0x80) != sum(1 for c in s if ord(c) >= 0x80): raise SystemExit("lech byte cao " + p)
    if LUA4:
        # so ket qua truoc/sau: ban goc chay bang lua4 lam CHUNK CHINH (khong dofile) de tranh tran; ban moi chay qua dofile
        d = os.path.dirname(p)
        tmp_old = os.path.join(d, "_t_old.lua"); tmp_new = os.path.join(d, "_t_new.lua")
        io.open(tmp_old, "w", encoding="latin-1", newline="").write(s); io.open(tmp_new, "w", encoding="latin-1", newline="").write(s2)
        try:
            rc0, o0, e0 = lua_dump(LUA4, tmp_old); rc1, o1, e1 = lua_dump(LUA4, tmp_new)
        finally:
            os.remove(tmp_old); os.remove(tmp_new)
        print("  lua4 %s: goc(dofile) rc=%d loi=%r | moi rc=%d loi=%r | dong %d/%d | giong: %s" % (os.path.basename(p), rc0, e0[:60], rc1, e1[:60], o0.count(b"\n"), o1.count(b"\n"), "CO" if (o0 == o1 and o1) else ("goc tran, chi kiem moi" if rc0 != 0 and rc1 == 0 and o1 else "KHAC")))
        if rc1 != 0: raise SystemExit("ban moi vay loi lua4: " + e1.decode("latin-1")[:200])
    if not KIEM:
        if not os.path.exists(p + BAK): shutil.copy2(p, p + BAK)
        io.open(p, "w", encoding="latin-1", newline="").write(s2)
    print("  [+] %s: tach %d bang%s" % (p, ne, " (KIEM)" if KIEM else ""))
print("XONG.")
