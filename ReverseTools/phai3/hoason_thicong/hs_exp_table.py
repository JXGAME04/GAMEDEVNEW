# -*- coding: utf-8 -*-
"""hs_exp_table.py [HOASON 01/09] - bang kinh nghiem ky nang 90/120/150 Hoa Son cho JX1 magic_level_exp.txt (server + client)
Gia tri = skill_skillexp_v trong Linux script\\skill\\huashan.lua (Linux doc truong nay: KSkill+0x11c, 0x080E4F9D / 0x080E5F21),
KHONG lay 5 dong trong settings\\npc\\player\\magic_level_exp.txt Linux (khac gia tri, Linux khong dung).
"""
import io, os, re, math, sys
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
REL = r"settings\npc\player\magic_level_exp.txt"
L = open(r"D:\ServerLinux\server1\script\skill\huashan.lua", "rb").read().replace(b"\r\n", b"\n").decode("latin-1")
def SkillExpFunc(Exp0, a, Level, Time, Range): return math.floor(Exp0 * (a ** (Level - 1)) * Time * Range / 2)
def bang(nm):
    i = L.find("\t" + nm + "={"); j = L.find("skill_skillexp_v", i); k = L.find("}}", j)
    blk = L[j:k + 2].split("{{", 1)[1]
    out = {}
    for lv, ex in re.findall(r"\{\s*(\d+)\s*,\s*([^{}]+?)\s*\}", blk):
        out[int(lv)] = int(eval(ex.strip(), {"SkillExpFunc": SkillExpFunc, "floor": math.floor}))
    return out
SK = {1364: "duoming_start", 1365: "zixia_jianqi", 1369: "jiujian_start", 1382: "pishi_poyu", 1384: "shenguang_xuanrao1"}
lin = open(r"D:\ServerLinux\server1\settings\skills.txt", "rb").read().split(b"\n")
ten = {}
for l in lin[1:]:
    c = l.rstrip(b"\r").split(b"\t")
    if len(c) > 2 and c[2].isdigit() and int(c[2]) in SK: ten[int(c[2])] = c[0].decode("latin-1")
for p in (os.path.join(SRV, REL), os.path.join(CLI, REL)):
    d = io.open(p, "r", encoding="latin-1", newline="").read()
    nl = "\r\n" if "\r\n" in d else "\n"
    lines = d.split(nl)
    ncol = len(lines[0].split("\t"))
    have = set(l.split("\t")[0] for l in lines)
    add = []
    for sid, nm in SK.items():
        if str(sid) in have: print("  da co", sid); continue
        tb = bang(nm)
        vals = [str(tb.get(lv, "")) for lv in range(1, ncol - 3 + 1)]
        row = "\t".join([str(sid), ten[sid], "10"] + vals)
        add.append(row); print("  +", sid, ten[sid].encode("latin-1")[:20], "muc:", len(tb), "L1=%s L%d=%s" % (tb[1], max(tb), tb[max(tb)]))
    if add:
        if lines[-1] == "": lines = lines[:-1]
        io.open(p, "w", encoding="latin-1", newline="").write(nl.join(lines + add) + nl)
        print("  ghi", p, "+%d dong (cot=%d)" % (len(add), ncol))
