# -*- coding: utf-8 -*-
"""B5 - VA DOT 2 (theo ket qua phan bien 7 huong + log chay that ScriptError.log).

CHAN 1  settings\\TimerTask.txt THIEU timer 28/29 (PLD) va 41/42/43 (Vuot Ai)
        => StartMissionTimer ban ra nhung KHONG CO script chay => ca 2 hoat dong
        khong tien trien duoc. Ban Linux: TimerTask.txt dong 29,30,42,43,44.
CHAN 2  chuangguang30.lua:249 'FORBITMAP_LIST' nil (khai o item\\heart_head.lua ban
        Linux, KHONG duoc chep vi JX1 da co ban khac) => chunk dut o dong 273 =>
        Init()+RegistAll() (274-275) khong chay => VUOT AI CHET HOAN TOAN.
        Va theo dung khuon dot TONGCASTLE 23/08 (tongcastle\\game.lua:164).
        BANG CHUNG CHAY THAT: script\\missions\\challengeoftime\\ScriptError.log
CHAN 3  \\settings\\trigger_challengeoftime.lua KHONG BAO GIO duoc nap: g_IniScriptEngine
        (KSortScript.cpp:51-66) chi LoadAllScript("\\script") + ("\\scriptjx2\\tong_vn").
        DynamicExecute -> g_GetScript tra NULL => lich Vuot Ai khong chay.
        Va bang ReLoadScript trong HD3_DriverInit (KSortScript.cpp:347 ReLoadScript
        goi LoadScriptToSortListA = THEM MOI vao cay, khong doi tep phai co san)
        => khong can build lai DLL.
CHAN 4  activitysys\\config\\41\\extend.lua: thieu 3 tep anh em (head/variables/data)
        => pActivity nil. Hoat dong 41 KHONG thuoc 3 hoat dong -> chep lay -> GO BO.
        BANG CHUNG: script\\activitysys\\config\\41\\ScriptError.log
NANG 5  kill_level.lua thieu OnRevive (JX1 goi khi NPC hoi sinh) => 160 loi/dot.
        BANG CHUNG: ScriptError.log 160x ngay 25/08.
NANG 6  Nhan menu Lenh Bai Admin chua dau '/' => ScriptFuns.cpp:717 strstr(pAnswer,"/")
        cat o dau '/' DAU TIEN => goi ham sai.
        BANG CHUNG: ScriptError.log cFuncName:( Phong Lang Do / Vuot ai): test)
"""
import io, os, shutil, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"


def V(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


def rd(rel):
    return io.open(os.path.join(JX1, rel), encoding="latin-1", newline="").read()


def wr(rel, d, backup=False):
    p = os.path.join(JX1, rel)
    if backup:
        b = p + ".truoc_3hd_2508"
        if not os.path.exists(b):
            shutil.copyfile(p, b)
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    m = os.path.join(MIRROR, rel)
    os.makedirs(os.path.dirname(m), exist_ok=True)
    io.open(m, "w", encoding="latin-1", newline="").write(d)


# ---------- CHAN 1: TimerTask.txt ----------
rel = os.path.join("settings", "TimerTask.txt")
d = rd(rel)
nl = "\r\n" if "\r\n" in d else "\n"
add = [
    ("28", r"\script\missions\fengling_ferry\fld_landingtimer.lua"),
    ("29", r"\script\missions\fengling_ferry\fld_smalltimer.lua"),
    ("41", r"\script\missions\challengeoftime\timer_match.lua"),
    ("42", r"\script\missions\challengeoftime\timer_board.lua"),
    ("43", r"\script\missions\challengeoftime\timer_close.lua"),
]
have = set()
for ln in d.split(nl):
    c = ln.split("\t")
    if len(c) >= 2 and c[0].strip().isdigit():
        have.add(c[0].strip())
new_rows = [(k, v) for k, v in add if k not in have]
if new_rows:
    body = d.rstrip(nl)
    for k, v in new_rows:
        body += nl + "%s\t%s" % (k, v)
    wr(rel, body + nl, backup=True)
    print("CHAN 1: them %d timer -> %s" % (len(new_rows), ", ".join(k for k, _ in new_rows)))
else:
    print("CHAN 1: da co du (bo qua)")

# ---------- CHAN 2: FORBITMAP_LIST guard ----------
rel = os.path.join("script", "missions", "challengeoftime", "chuangguang30.lua")
d = rd(rel)
if "FORBITMAP_LIST = FORBITMAP_LIST or {}" not in d:
    # chen ngay TRUOC dong dinh nghia SetForbitItem (hoac dau tien dung FORBITMAP_LIST)
    lines = d.split(nl if nl in d else "\n")
    NL = nl if nl in d else "\n"
    idx = next(i for i, l in enumerate(lines) if "FORBITMAP_LIST" in l)
    guard = ("-- [3HD 25/08] FORBITMAP_LIST khai o item" + chr(92) + "heart_head.lua ban Linux -"
             " tep do KHONG duoc chep (JX1 da co ban khac) => nil => chunk dut o dong 273,"
             " Init()+RegistAll() khong chay => Vuot Ai chet. Khuon va y het TONGCASTLE 23/08"
             " (tongcastle" + chr(92) + "game.lua:164).")
    lines[idx:idx] = [guard, "FORBITMAP_LIST = FORBITMAP_LIST or {}", ""]
    wr(rel, NL.join(lines))
    print("CHAN 2: them guard FORBITMAP_LIST truoc dong %d" % (idx + 1))
else:
    print("CHAN 2: da co guard")

# ---------- CHAN 4: go activitysys\config\41 ----------
d41 = os.path.join(JX1, "script", "activitysys", "config", "41")
if os.path.isdir(d41):
    dead = os.path.join(JX1, "script", "activitysys", "config", "41.KHONG_DUNG_3HD")
    if os.path.exists(dead):
        shutil.rmtree(dead)
    shutil.move(d41, dead)
    m41 = os.path.join(MIRROR, "script", "activitysys", "config", "41")
    if os.path.isdir(m41):
        shutil.rmtree(m41)
    print("CHAN 4: doi ten config\\41 -> 41.KHONG_DUNG_3HD (hoat dong 41 khong thuoc 3 HD)")
else:
    print("CHAN 4: khong con config\\41")

# ---------- NANG 5: kill_level.lua OnRevive ----------
rel = os.path.join("script", "task", "tollgate", "killer", "kill_level.lua")
d = rd(rel)
if "function OnRevive" not in d:
    NL = "\r\n" if "\r\n" in d else "\n"
    d = d.rstrip(NL) + NL + NL + (
        "-- [3HD 25/08] JX1 goi OnRevive moi lan NPC hoi sinh (KNpc.cpp). Ban Linux khong" + NL +
        "-- co ham nay o ActionScript nen sinh 160 loi 'attempt to call a nil value' moi dot" + NL +
        "-- (da thay trong ScriptError.log 25/08). Ham rong = giu dung hanh vi ban Linux." + NL +
        "function OnRevive()" + NL + "end" + NL)
    wr(rel, d)
    print("NANG 5: them OnRevive rong vao kill_level.lua")
else:
    print("NANG 5: da co OnRevive")

# ---------- NANG 6: bo dau '/' trong nhan menu ----------
# 6a) lenhbaiadmin.lua
rel = os.path.join("script", "item", "lenhbaiadmin.lua")
d = rd(rel)
old6a = V("Hoạt động Linux (Săn Boss Sát Thủ / Phong Lăng Độ / Vượt ải): test")
new6a = V("Hoạt động Linux - Săn Boss Sát Thủ, Phong Lăng Độ, Vượt ải (test)")
if old6a in d:
    wr(rel, d.replace(old6a, new6a))
    print("NANG 6a: bo 2 dau '/' trong nhan menu lenhbaiadmin")
else:
    print("NANG 6a: khong tim thay nhan cu")

# 6b) hd3_admin.lua - "(1082/1192/1193/1217)"
rel = os.path.join("script", "item", "hd3_admin.lua")
d = rd(rel)
old6b = V("Xem biến nhiệm vụ (1082/1192/1193/1217)")
new6b = V("Xem biến nhiệm vụ (1082, 1192, 1193, 1217)")
if old6b in d:
    wr(rel, d.replace(old6b, new6b))
    print("NANG 6b: bo 3 dau '/' trong nhan menu hd3_admin")
else:
    print("NANG 6b: khong tim thay nhan cu")
