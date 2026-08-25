# -*- coding: utf-8 -*-
r"""[25/08] QUET TOAN BO NPC + DONG CHUC NANG cua chuoi TIN SU.

Mo phong dung engine JX1: moi file goc mot lua_State; Include = dofile de quy vao
CUNG state (khong guard). Voi moi state: gom moi ham DINH NGHIA va moi ham DUOC GOI
tu menu ("nhan/ham", AddOptEntry(..., ham), "#ham(...)"), roi bao ham NIL.
Cung kiem: %upvalue tren ham chua dinh nghia TRUOC diem dung (Lua 4 dong bang luc nap).
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding='utf-8', errors='replace')

ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"

# cac script GOC cua chuoi Tin Su (moi cai = mot state rieng nhu engine)
GOC = [
    # NPC thoai trong 3 ai
    r"script\task\tollgate\messenger\qianbaoku\messenger_turenpc.lua",      # Tieu Tran (ai 3)
    r"script\task\tollgate\messenger\messenger_turerukou.lua",              # Dich quan ai 3
    r"script\task\tollgate\messenger\shanshenmiao\messenger_temnpc.lua",    # Nap Lan Thanh Thanh (ai 2)
    r"script\task\tollgate\messenger\messenger_templerukou.lua",            # Dich quan ai 2
    r"script\task\tollgate\messenger\fengzhiqi\messenger_flynpc.lua",       # Canh trung (ai 1)
    r"script\task\tollgate\messenger\messenger_flyrukou.lua",               # Dich quan ai 1
    # ruong + quai + trap map 395 (ai 3 cap 90)
    r"script\task\tollgate\messenger\qianbaoku\90\tureboss90.lua",
    r"script\task\tollgate\messenger\qianbaoku\90\turebug90.lua",
    r"script\task\tollgate\messenger\trap\trap_qianbaoku.lua",
    # NPC nhan/tra nhiem vu o thanh (posthouse duoc Include tu items)
    r"script\item\messenger\toll_jinpaixinshi.lua",
    r"script\item\xinshirenwu\xinshibaoxiang.lua",
    r"script\item\xinshirenwu\qianbaokuling.lua",
    # sinh NPC luc boot
    r"script\task\tollgate\tinsu_addnpc.lua",
]

re_inc  = re.compile(r'Include\s*\(\s*"([^"]+)"', re.I)
re_def  = re.compile(r'^\s*function\s+([A-Za-z_][A-Za-z0-9_]*)\s*[\(:]|^\s*function\s+[A-Za-z_][A-Za-z0-9_.:]*[.:]([A-Za-z_][A-Za-z0-9_]*)\s*\(')
re_asg  = re.compile(r'^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*function\b')
# "nhan/ham" trong chuoi menu; bo /no neu muon nhung cu kiem luon
re_opt  = re.compile(r'"[^"]*/\s*#?([A-Za-z_][A-Za-z0-9_]*)\s*(?:\([^)"]*\))?\s*"')
re_ent  = re.compile(r'AddOptEntry\s*\([^,]+,\s*([A-Za-z_][A-Za-z0-9_]*)')
re_up   = re.compile(r'%([A-Za-z_][A-Za-z0-9_]*)')

def strip_comment(l):
    # Lua 4: -- den het dong (bo qua truong hop -- trong chuoi cho don gian)
    i = l.find("--")
    return l[:i] if i >= 0 else l

def resolve(p):
    p = p.replace("/", "\\").lstrip("\\")
    c = os.path.join(ROOT, p)
    if os.path.isfile(c): return c
    if p.lower().startswith("script\\"):
        c2 = os.path.join(ROOT, "scriptjx2" + p[6:])
        if os.path.isfile(c2): return c2
    return None

def load_state(root_rel):
    """tra ve (defs, calls, upmiss, missing_includes, order)"""
    defs, order = set(), []
    calls = []          # (ten_ham, file, dong)
    upmiss = []         # (%ten, file, dong)  - upvalue chua co luc nap
    inc_missing = []
    seen_now = set()    # chong lap vo han (engine KHONG guard nhung de quy vo han se treo that; giu de tool chay)
    def dofile(rel, depth):
        f = resolve(rel)
        if not f:
            inc_missing.append(rel); return
        key = f.lower()
        if key in seen_now or depth > 12: return
        seen_now.add(key)
        try:
            s = io.open(f, "r", encoding="latin-1", newline="").read()
        except IOError:
            inc_missing.append(rel); return
        order.append(os.path.relpath(f, ROOT))
        for no, raw in enumerate(s.split("\n"), 1):
            l = strip_comment(raw)
            m = re_inc.search(l)
            if m: dofile(m.group(1), depth + 1)
            md = re_def.search(l)
            if md: defs.add(md.group(1) or md.group(2))
            ma = re_asg.search(l)
            if ma: defs.add(ma.group(1))
            for mo in re_opt.finditer(l):
                calls.append((mo.group(1), os.path.basename(f), no))
            for me in re_ent.finditer(l):
                calls.append((me.group(1), os.path.basename(f), no))
            for mu in re_up.finditer(l):
                nm = mu.group(1)
                if nm not in defs:              # chua dinh nghia TRUOC dong nay
                    upmiss.append((nm, os.path.basename(f), no))
    dofile(root_rel, 0)
    return defs, calls, upmiss, inc_missing, order

tong_loi = 0
for rel in GOC:
    f = resolve(rel)
    ten = os.path.basename(rel)
    if not f:
        print("[MAT FILE] %s" % rel); tong_loi += 1; continue
    defs, calls, upmiss, incmiss, order = load_state(rel)
    loi = []
    for nm, src, no in calls:
        if nm not in defs:
            loi.append("  NIL menu  : '%s'  (goi tu %s:%d)" % (nm, src, no))
    for nm, src, no in upmiss:
        # %ham ma den cuoi cung van khong co dinh nghia o dau => chac chan nil
        if nm not in defs:
            loi.append("  NIL upval : %%%s  (%s:%d)" % (nm, src, no))
        else:
            loi.append("  UPVAL SAU : %%%s dinh nghia SAU diem dung (%s:%d) - Lua4 dong bang = nil" % (nm, src, no))
    for rel2 in incmiss:
        loi.append("  MAT Include: %s" % rel2)
    if loi:
        tong_loi += len(loi)
        print("== %s  (%d file trong state)" % (ten, len(order)))
        seen=set()
        for x in loi:
            if x in seen: continue
            seen.add(x); print(x)
        print()
print("TONG van de: %d" % tong_loi)
