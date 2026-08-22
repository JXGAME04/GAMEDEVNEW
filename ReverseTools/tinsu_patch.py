# -*- coding: utf-8 -*-
"""Va cay Tin Su da chep tu Linux sang du an (21/08). Moi sua deu assert so lan khop.
Moi tep doc/ghi latin-1 newline='' (giu nguyen byte GBK/TCVN3)."""
import io, os, re, shutil, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
SC = os.path.join(SRV, "script")
LNX = r"D:\ServerLinux\server1"
NEWID = {30301: 4857, 30529: 4858, 30537: 4859, 30506: 4860, 30507: 4861, 30006: 4862, 30505: 4863}
IMG_SUB = {   # anh VNG khong co trong bat ky pak nao cua du an -> anh thay the cung ho
    30529: BS + "spr" + BS + "item" + BS + "script" + BS + "item_huangjintupu.spr",
    30537: BS + "spr" + BS + "item" + BS + "script" + BS + "item_huangjintupu.spr",
    30506: BS + "spr" + BS + "item" + BS + "script" + BS + "canglangling.spr",
    30507: BS + "spr" + BS + "item" + BS + "script" + BS + "item_xuantiekuang.spr",
}
SCRIPT_NEW = {30301: BS + "script" + BS + "item" + BS + "event" + BS + "kinhmach" + BS + "honnguyenchandon.lua"}


def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) > 127)


def sub_count(s, pat, rep, n, flags=0):
    new, k = re.subn(pat, rep, s, flags=flags)
    assert k == n, "%r: mong %d, duoc %d" % (pat, n, k)
    return new


def remap_ids(rel, mapping, expect):
    """Thay (6,1,OLD) -> (6,1,NEW) trong 1 tep; expect = tong so lan."""
    p = os.path.join(SC, rel)
    s = rd(p); h = hi(s); total = 0
    for old, new in mapping.items():
        s, k = re.subn(r"(6\s*,\s*1\s*,\s*)%d\b" % old, r"\g<1>%d" % new, s)
        total += k
    assert total == expect, "%s: mong %d thay the, duoc %d" % (rel, expect, total)
    assert hi(s) == h
    wr(p, s); print("  remap %-55s %d id" % (rel, total))


# ---------- 1. posthouse.lua ----------
p = os.path.join(SC, r"task\tollgate\messenger\posthouse.lua")
s = rd(p); h = hi(s)
s = sub_count(s, r"if \( GetLevel\(\) < 120 \) then", "if ( GetLevel() < 90 ) then\t-- [TIN SU 21/08] 120 -> 90 (lenh chu game)", 1)
# dong thong bao ngay sau: "...chua du 120. hay tiep tuc..." -> 90 (chi dong chua DescLink_YiGuan + '120')
lines = s.split("\n")
hit = [i for i, l in enumerate(lines) if "GetLevel() < 90" in l]
assert len(hit) == 1, hit
assert "DescLink_YiGuan" in lines[hit[0] + 1] and "120" in lines[hit[0] + 1]
lines[hit[0] + 1] = lines[hit[0] + 1].replace("120", "90", 1)
s = "\n".join(lines)
# AddGoldItem: bang GoldItem.txt du an 205 = Muc Tuc, 206 = Kiem Bai (Linux nguoc lai) -> hoan doi
s = sub_count(s, r"AddGoldItem\(0,205\)", "AddGoldItem(0,99205)", 1)
s = sub_count(s, r"AddGoldItem\(0,206\)", "AddGoldItem(0,205)\t-- [TIN SU 21/08] Linux 206 Muc Tuc = du an GoldItem 205", 1)
s = sub_count(s, r"AddGoldItem\(0,99205\)", "AddGoldItem(0,206)\t-- [TIN SU 21/08] Linux 205 Kiem Bai = du an GoldItem 206", 1)
assert hi(s) == h
wr(p, s); print("  posthouse.lua: cap 90 + hoan doi AddGoldItem 205/206")
remap_ids(r"task\tollgate\messenger\posthouse.lua",
          {402: 401, 885: 884, 886: 885, 887: 886, 888: 887, 889: 888, 2566: 2575, 2812: 3430, 2813: 3431, 30229: 4847}, 11)

# ---------- 2. xinshibaoxiang.lua ----------
m = {2744: 3362, 30191: 2953, 4134: 4752, 30228: 4846, 30229: 4847, 3203: 4844, 30289: 4848, 3811: 4429, 3810: 4428, 2812: 3430}
m.update(NEWID)
remap_ids(r"item\xinshirenwu\xinshibaoxiang.lua", m, 19)

# ---------- 3. wuxingfu.lua ----------
remap_ids(r"item\xinshirenwu\wuxingfu.lua", {2806: 3424, 2807: 3425, 2808: 3426, 2809: 3427, 2810: 3428, 2811: 3429}, 10)

# ---------- 4. 3 NPC cua ai: Include partner (JX2 PARTNER lib) -> chu thich ----------
for rel in (r"task\tollgate\messenger\fengzhiqi\messenger_flynpc.lua",
            r"task\tollgate\messenger\qianbaoku\messenger_turenpc.lua",
            r"task\tollgate\messenger\shanshenmiao\messenger_temnpc.lua"):
    p = os.path.join(SC, rel); s = rd(p); h = hi(s)
    s = sub_count(s, r'^Include \("\\\\script\\\\task\\\\partner\\\\master\\\\partner_master_main.lua"\);',
                  '--[[JX1 21/08: he PARTNER (dong hanh) la JX2, khong port; phan dung no trong tep nay da bi chu thich san o Linux]] --Include ("\\\\script\\\\task\\\\partner\\\\master\\\\partner_master_main.lua");', 1, re.M)
    assert hi(s) == h
    wr(p, s); print("  partner Include -> chu thich:", rel.split(BS)[-1])

# ---------- 5. checkinmap.lua + honnguyenchandon.lua ----------
os.makedirs(os.path.join(SC, "vng_feature"), exist_ok=True)
dst = os.path.join(SC, r"vng_feature\checkinmap.lua")
if not os.path.exists(dst):
    shutil.copyfile(os.path.join(LNX, r"script\vng_feature\checkinmap.lua"), dst); print("  chep vng_feature/checkinmap.lua")
src = rd(os.path.join(LNX, r"script\vng_event\item\hunyuanzenyuan.lua")); h = hi(src)
src = sub_count(src, r'^Include\("\\\\script\\\\activitysys\\\\playerfunlib.lua"\)',
                'Include("\\\\script\\\\activitysys\\\\playerfunlib.lua")\nInclude("\\\\script\\\\vng_feature\\\\checkinmap.lua")\t-- JX1: moi tep mot lua_State, phai Include tuong minh\nInclude("\\\\script\\\\lib\\\\lib_task.lua")\t-- TASK_CHANGNGUYENDAN', 1, re.M)
src = sub_count(src, r"PlayerFunLib:AddTask\(4000,tbVngZenYuan.nCountZenYuan\)",
                "PlayerFunLib:AddTask(TASK_CHANGNGUYENDAN,tbVngZenYuan.nCountZenYuan)\t-- JX1: diem Chan Nguyen kinh mach = task 362 (Linux 4000)", 1)
src = sub_count(src, r"GetTask\(4000\)", "GetTask(TASK_CHANGNGUYENDAN)", 1)
assert hi(src) == h
wr(os.path.join(SC, r"item\event\kinhmach\honnguyenchandon.lua"), src); print("  tao item/event/kinhmach/honnguyenchandon.lua (tu hunyuanzenyuan.lua)")

# ---------- 6. magicscript.txt: bind Script + 7 item moi ----------
P = os.path.join(SRV, r"settings\item\magicscript.txt")
raw = io.open(P, "rb").read()
assert b"\r\n" in raw
rows = raw.split(b"\r\n")
assert rows[-1] != b""   # khong co CRLF cuoi
BIND = {
    884: r"\script\item\messenger\toll_mupaixinshi.lua", 885: r"\script\item\messenger\toll_tongpaixinshi.lua",
    886: r"\script\item\messenger\toll_yinpaixinshi.lua", 887: r"\script\item\messenger\toll_jinpaixinshi.lua",
    888: r"\script\item\messenger\toll_yucixinshi.lua",
    3424: r"\script\item\xinshirenwu\wuxingfu.lua", 3425: r"\script\item\xinshirenwu\chejinfu.lua",
    3426: r"\script\item\xinshirenwu\chemufu.lua", 3427: r"\script\item\xinshirenwu\cheshuifu.lua",
    3428: r"\script\item\xinshirenwu\chehuofu.lua", 3429: r"\script\item\xinshirenwu\chetufu.lua",
    3430: r"\script\item\xinshirenwu\xinshibaoxiang.lua", 3431: r"\script\item\xinshirenwu\qianbaokuling.lua",
    4752: r"\script\item\event\kinhmach\channguyendan.lua",
}
for pid, scr in BIND.items():
    c = rows[pid + 1].split(b"\t")
    assert c[1] == b"6" and c[2] == b"1" and int(c[3]) == pid, (pid, c[:4])
    assert c[9] in (b"0", b""), (pid, c[9])
    c[9] = scr.encode("latin-1")
    rows[pid + 1] = b"\t".join(c)
print("  bind Script cho %d item" % len(BIND))
# 7 item moi, id = dong - 2, noi tiep 4856
lnx = {}
for ln in io.open(os.path.join(LNX, r"settings\item\004\magicscript.txt"), "rb").read().split(b"\r\n"):
    c = ln.split(b"\t")
    if len(c) > 20 and c[1] == b"6" and c[2] == b"1" and c[3].isdigit() and int(c[3]) in NEWID:
        lnx[int(c[3])] = c
assert len(lnx) == 7, lnx.keys()
assert len(rows) - 2 == 4856, len(rows)
tmpl_tail = rows[4857].split(b"\t")[14:]   # duoi trong cua dong cuoi hien co
for old in sorted(NEWID, key=lambda k: NEWID[k]):
    new = NEWID[old]; c = lnx[old]
    img = IMG_SUB.get(old, c[4].decode("latin-1")).encode("latin-1")
    scr = SCRIPT_NEW.get(old, "0").encode("latin-1")   # du an: 0 = khong script (khong co noscript.lua)
    # du an: Name Genre Detail Particular Image ObjIdx W H Intro Script Price ShortKey nMaxStack PickExecute
    row = [c[0], b"6", b"1", str(new).encode(), img, c[5], c[6], c[7], c[8], scr, c[10], b"1", c[20] if c[20] != b"" else b"0", b"0"] + tmpl_tail
    assert len(rows) - 2 + 1 == new
    rows.append(b"\t".join(row))
    print("  + item %d <- Linux %d | %s | anh %s" % (new, old, c[0].decode("latin-1").encode("latin-1").decode("gbk", "replace"), img.decode("latin-1")))
io.open(P, "wb").write(b"\r\n".join(rows))
print("  magicscript.txt: %d dong, id cuoi %d" % (len(rows), len(rows) - 2))
print("OK")
