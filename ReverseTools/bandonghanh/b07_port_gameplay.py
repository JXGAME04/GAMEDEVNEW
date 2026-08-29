# -*- coding: utf-8 -*-
"""BAN DONG HANH - G5: port ~60 tep script gameplay tu Linux sang JX1.

Cac viec:
 1. Dung bang doi ID ITEM theo TEN (bay #7): sach 834..884/901/1063, cuon 447..827, 829.
 2. Chep byte-exact + NAN id trong noi dung: script\task\partner\**, npclevelscript,
    item skillbook/reward, deps (task_debug).
 3. Nan bang: skill_requirement.txt (cot 1) + index_taskid.txt (cot 4) o CA server+client.
 4. Chep event_killnpc.txt (bang event giet quai 429 dong).
 5. Sinh bdh_compat.lua (Msg2player alias + Add/RemovePlayerEvent stub) va
    bdh_killhook.lua (dem giet quai -> reward_killfinish) + va danhquai.lua (2 dong).
 6. Sinh driver te dan gio chan (bdh_jitan_driver duoc goi tu timerserver - va 1 dong).
 7. syncheck moi tep .lua da sinh/chep.
KHONG port: functions_partner*.lua (GM helper), random_taskbook/task_tools (G6 TASKSYS).
Idempotent; sao luu .truoc_bdh_g5 cho tep bi va.
"""
import io
import os
import re
import shutil
import subprocess
import sys
import unicodedata
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

LIN = r"D:\ServerLinux\server1"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
SYN = (r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto"
       r"\0064b491-1b33-4139-acb8-6928fda45bcd\scratchpad\luac\syncheck.exe")

spec = importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec); spec.loader.exec_module(dec2)
spec2 = importlib.util.spec_from_file_location("rt", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\recon_tinhnang.py")
rt = importlib.util.module_from_spec(spec2); spec2.loader.exec_module(rt)

CR = chr(13); LF = chr(10); T = chr(9)


# ============================ 1. BANG DOI ID ============================
def norm(s):
    s = unicodedata.normalize("NFC", s.strip().lower())
    s = s.replace('"', "").replace("'", "")
    return " ".join(s.split())


def build_item_map():
    jx_rows = open(SRV + r"\settings\item\magicscript.txt", "rb").read().split(b"\n")
    ln_rows = open(LIN + r"\settings\item\004\magicscript.txt", "rb").read().split(b"\n")
    jx_by_name = {}
    for i in range(1, len(jx_rows)):
        c = jx_rows[i].split(b"\t")
        if not c or not c[0].strip():
            continue
        try:
            nm = norm(rt.tcvn3(c[0].decode("latin-1")))
        except Exception:
            continue
        jx_by_name.setdefault(nm, []).append(i - 1)

    can_map = list(range(447, 828)) + [829] + list(range(834, 885)) + [901, 1063]
    m = {}
    loi = []
    for lid in can_map:
        ln = lid + 1
        if ln >= len(ln_rows):
            loi.append((lid, "EOF"))
            continue
        nm = norm(dec2.decline2(ln_rows[ln].split(b"\t")[0]))
        ids = jx_by_name.get(nm, [])
        if len(ids) == 1:
            m[lid] = ids[0]
        elif len(ids) > 1:
            # uu tien ban lech -1 (khuon da thay)
            if lid - 1 in ids:
                m[lid] = lid - 1
            else:
                loi.append((lid, "TRUNG %s" % ids))
        else:
            loi.append((lid, "KHONG THAY [%s]" % nm[:40]))
    return m, loi


# ============================ 2. CHEP + NAN ============================
COPY_DIRS = [r"script\task\partner"]
COPY_FILES = [
    r"script\npclevelscript\partner_task.lua",
    r"script\npclevelscript\partner_task_boss.lua",
    r"script\item\partner_skillbook_h.lua",
    r"script\item\partner_skillbook_given.lua",
    r"script\item\partner_skillbook_learnt.lua",
    r"script\item\partner_skillbook_resist.lua",
    r"script\item\partner_skillbook_ultimate.lua",
    r"script\item\reward_partner.lua",
    r"script\item\reward\reward_education.lua",
    r"script\task\system\task_debug.lua",
    r"settings\npc\player\event_killnpc.txt",
]
BO_QUA = ("task_gmscript.lua",)          # GM console goc - vo hai nhung khoi port


def nan_id(noidung, imap):
    """thay 6,1,<id> + {<id>, trong bang jitan + AddItem(6, 1, id."""
    def rep_tuple(mo):
        lid = int(mo.group(2))
        if lid in imap:
            return mo.group(1) + str(imap[lid]) + mo.group(3)
        return mo.group(0)
    # 6,1,<id> (co/khong khoang trang)
    noidung = re.sub(r"(6\s*,\s*1\s*,\s*)(\d{3,4})(\s*[,)])", rep_tuple, noidung)
    return noidung


def nan_jitan(noidung, imap):
    # bang TB_JITAN_AWARD_BOOK: {<id>, min, max}
    def rep(mo):
        lid = int(mo.group(2))
        if lid in imap:
            return mo.group(1) + str(imap[lid]) + mo.group(3)
        return mo.group(0)
    return re.sub(r"([{]\s*)(8[3-9]\d|90[01]|1063)(\s*,)", rep, noidung)


def port_scripts(imap):
    files = []
    for d in COPY_DIRS:
        base = os.path.join(LIN, d)
        for dp, dns, fns in os.walk(base):
            for fn in fns:
                if fn in BO_QUA:
                    continue
                files.append(os.path.relpath(os.path.join(dp, fn), LIN))
    files += COPY_FILES
    n_new = 0
    for rel in files:
        src = os.path.join(LIN, rel)
        dst = os.path.join(SRV, rel)
        if not os.path.exists(src):
            print("  !!! thieu nguon:", rel)
            continue
        raw = open(src, "rb").read()
        if rel.endswith(".lua"):
            s = raw.decode("latin-1")
            s = nan_id(s, imap)
            if "partner_jitan.lua" in rel:
                s = nan_jitan(s, imap)
            raw2 = s.encode("latin-1")
        else:
            raw2 = raw
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        if os.path.exists(dst) and open(dst, "rb").read() == raw2:
            continue
        open(dst, "wb").write(raw2)
        n_new += 1
    print("port_scripts: %d tep ghi moi/cap nhat (tong %d)" % (n_new, len(files)))


# ============================ 3. NAN BANG ============================
def nan_bang(imap):
    # skill_requirement.txt: cot 1 = SkillBookId (Linux) -> JX1
    for base in (SRV,):
        p = os.path.join(base, r"settings\partner\skill_requirement.txt")
        if os.path.exists(p + ".bdh_nanned"):
            print("skill_requirement: da nan (marker), bo qua")
            break
        rows = open(p, "rb").read().split(b"\n")
        out = [rows[0]]
        n = 0
        for r in rows[1:]:
            c = r.split(b"\t")
            if c and c[0].strip().isdigit():
                lid = int(c[0])
                if lid in imap:
                    c[0] = str(imap[lid]).encode()
                    n += 1
                out.append(b"\t".join(c))
            else:
                out.append(r)
        open(p, "wb").write(b"\n".join(out))
        print("skill_requirement:", n, "dong nan")

    # index_taskid.txt: cot 4 particular  (server + client)
    for base in (SRV, CLI):
        p = os.path.join(base, r"settings\task\partner\reward\index_taskid.txt")
        if os.path.exists(p + ".bdh_nanned"):
            print("index_taskid: da nan (marker), bo qua")
            continue
        rows = open(p, "rb").read().split(b"\n")
        out = [rows[0]]
        n = 0
        for r in rows[1:]:
            c = r.split(b"\t")
            if len(c) > 4 and c[3].strip().isdigit():
                lid = int(c[3])
                if lid in imap:
                    c[3] = str(imap[lid]).encode()
                    n += 1
                out.append(b"\t".join(c))
            else:
                out.append(r)
        open(p, "wb").write(b"\n".join(out))
        print("index_taskid (%s): %d dong nan" % (os.path.basename(base), n))


# ============================ 5. COMPAT + HOOK ============================
COMPAT = r"""-- SINH TU DONG [BDH-G5] bdh_compat.lua - lop tuong thich JX1 cho script partner Linux
-- (Msg2player viet thuong + he AddPlayerEvent ma JX1 khong co trong engine)
Msg2player = Msg2Player

IncludeLib("FILESYS")
TabFile_Load("\\settings\\npc\\player\\event_killnpc.txt", "bdh_killevent")

-- he dem giet quai lam bang hook danhquai.lua (BDH_OnKillNpc trong bdh_killhook.lua)
function AddPlayerEvent(nEventId)
	return 1
end
function RemovePlayerEvent(nEventId)
	SetTask(1236, 0)
	return 1
end
"""

KILLHOOK = r"""-- SINH TU DONG [BDH-G5] bdh_killhook.lua - dem giet quai cho cuon tu luyen dong hanh
-- Duoc danhquai.lua Include + goi trong OnDeathMonsterDaTau (PlayerIndex DA duoc dat).
-- event_killnpc.txt: cot 4=TaskID(dem) 6=KillCount 7=MapID 8=NpcTemplateID
Include("\\script\\task\\partner\\bdh_compat.lua")
Include("\\script\\item\\reward_partner.lua")

function BDH_OnKillNpc(nNpcIdx)
	local nEvent = GetTask(1237)
	if (nEvent < 6 or nEvent > 386) then
		return
	end
	local nRow = nEvent + 1
	local nTpl = tonumber(TabFile_GetCell("bdh_killevent", nRow, 8, "-1"))
	local nNeed = tonumber(TabFile_GetCell("bdh_killevent", nRow, 6, "0"))
	if (nTpl == nil or nNeed == nil or nNeed <= 0) then
		return
	end
	if (nTpl ~= -1 and GetNpcSettingIdx(nNpcIdx) ~= nTpl) then
		return
	end
	local nCount = GetTask(1236) + 1
	SetTask(1236, nCount)
	if (nCount >= nNeed) then
		reward_killfinish(PlayerIndex, 1236, nEvent)
	end
end
"""

JITAN_DRIVER = r"""-- SINH TU DONG [BDH-G5] bdh_jitan_driver.lua - te dan Truong Ca Mon thuc tinh gio chan
-- Goc: partner_jitan.lua chi DOC gb task; ban Linux thieu driver -> viet theo mo ta
-- trong thoai NPC ("moi gio chan linh luc te dan thuc tinh, dung 1 lan/toan server").
Include("\\script\\lib\\gb_taskfuncs.lua")

BDH_JITAN_KEY = "changgemen_jitan_bdh"

function BDH_JitanTick()
	local nYr, nMo, nDy, nHr, nMi = GetTimeNow()
	if (mod(nHr, 2) ~= 0) then
		return
	end
	if (nMi ~= 0) then
		return
	end
	local i
	for i = 1, 5 do
		gb_SetTask(BDH_JITAN_KEY, i, 1)
	end
end
"""


def sinh_compat():
    duong = [
        (SRV + r"\script\task\partner\bdh_compat.lua", COMPAT),
        (SRV + r"\script\task\partner\bdh_killhook.lua", KILLHOOK),
        (SRV + r"\script\task\partner\train\bdh_jitan_driver.lua", JITAN_DRIVER),
    ]
    for p, s in duong:
        data = s.replace("\n", "\r\n").encode("latin-1")
        if not (os.path.exists(p) and open(p, "rb").read() == data):
            open(p, "wb").write(data)
            print("sinh:", os.path.basename(p))

    # gb key GBK trong partner_jitan.lua goc la "chuoi GBK" - dong bo driver dung
    # cung key: doi partner_jitan.lua sang key ASCII cua driver (2 cho).
    p = SRV + r"\script\task\partner\train\partner_jitan.lua"
    s = open(p, "rb").read().decode("latin-1")
    if "changgemen_jitan_bdh" not in s:
        s2, n = re.subn(r'LG_PARTNER_JITAN_NAME\s*=\s*"[^"]*"',
                        'LG_PARTNER_JITAN_NAME = "changgemen_jitan_bdh"', s)
        if n == 1:
            open(p, "wb").write(s2.encode("latin-1"))
            print("partner_jitan: key gb -> changgemen_jitan_bdh")
        else:
            print("!!! khong thay LG_PARTNER_JITAN_NAME (n=%d)" % n)


def va_danhquai():
    p = SRV + r"\script\tinhnang\datau\danhquai.lua"
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    if "bdh_killhook" in s:
        print("danhquai: da va")
        return
    shutil.copyfile(p, p + ".truoc_bdh_g5")
    # 1) Include dau tep
    i = s.find("Include(")
    if i < 0:
        i = 0
    them = 'Include("\\\\script\\\\task\\\\partner\\\\bdh_killhook.lua")\t-- [BDH-G5] dem giet quai cuon dong hanh' + CR + LF
    s = s[:i] + them + s[i:]
    # 2) goi hook sau khi PlayerIndex duoc dat trong OnDeathMonsterDaTau
    neo = "PlayerIndex = nPlayerIndex"
    j = s.find(neo)
    assert j > 0, "khong thay neo PlayerIndex trong danhquai"
    j2 = j + len(neo)
    s = s[:j2] + CR + LF + T + 'call(BDH_OnKillNpc, {nNpcIndex}, "x")\t-- [BDH-G5]' + s[j2:]
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("danhquai: da va 2 cho")


def va_timerserver_jitan():
    p = SRV + r"\script\timerserver.lua"
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    if "bdh_jitan_driver" in s:
        print("timerserver: jitan da va")
        return
    neo = 'Include("\\\\script\\\\partner\\\\partner_test_bdh.lua")'
    assert neo in s, "khong thay neo test trong timerserver"
    s = s.replace(neo, neo + CR + LF +
                  'Include("\\\\script\\\\task\\\\partner\\\\train\\\\bdh_jitan_driver.lua")\t-- [BDH-G5] te dan gio chan', 1)
    neo2 = '\tcall(BDH_TestTick, {}, "x")'
    i = s.find(neo2)
    assert i > 0
    j = i + len(neo2)
    # chen sau dong test (con phan comment cua dong do -> tim het dong)
    k = s.find(LF, j)
    s = s[:k + 1] + '\tcall(BDH_JitanTick, {}, "x")\t-- [BDH-G5] te dan' + CR + LF + s[k + 1:]
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("timerserver: da va jitan tick")


# ============================ 7. SYNCHECK ============================
def syncheck_all():
    loi = 0
    tong = 0
    for dp, dns, fns in os.walk(SRV + r"\script\task\partner"):
        for fn in fns:
            if not fn.endswith(".lua"):
                continue
            tong += 1
            r = subprocess.run([SYN, os.path.join(dp, fn)], capture_output=True, text=True)
            if "LOI CU PHAP" in (r.stdout + r.stderr):
                loi += 1
                print("LOI CU PHAP:", fn)
    for rel in COPY_FILES:
        if not rel.endswith(".lua"):
            continue
        tong += 1
        r = subprocess.run([SYN, os.path.join(SRV, rel)], capture_output=True, text=True)
        if "LOI CU PHAP" in (r.stdout + r.stderr):
            loi += 1
            print("LOI CU PHAP:", rel)
    print("syncheck: %d tep, %d loi cu phap" % (tong, loi))
    return loi


def main():
    imap, loi = build_item_map()
    print("bang doi id: %d muc, %d loi" % (len(imap), len(loi)))
    for l in loi[:12]:
        print("   !!!", l)
    if loi:
        print("DUNG: co id chua phan giai duoc")
        return 1
    # in vai muc chot
    for k in (447, 827, 829, 834, 884, 901, 1063):
        print("   %d -> %d" % (k, imap[k]))
    port_scripts(imap)
    nan_bang(imap)
    sinh_compat()
    va_danhquai()
    va_timerserver_jitan()
    return syncheck_all()


if __name__ == "__main__":
    raise SystemExit(main())
