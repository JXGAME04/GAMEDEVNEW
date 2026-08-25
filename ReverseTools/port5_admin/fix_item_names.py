# -*- coding: utf-8 -*-
r"""[24/08 kiem chung] 2 loi that:
1. Item 3204-3207 (bua Thu Ve + 3 Than Moc Lenh) VAN GIU TEN GBK goc
   (越南自定义道具535..538) -> nguoi choi thay chu Trung/rac. Doi sang ten tieng Viet
   dung nhu tbTreeItem trong guideperson.lua.
2. tongwar_autoexec.lua khong Include gi ma van goi GetTabFileHeight/GetTabFileData -
   chi chay nho state cua startgame vo tinh co ham do qua duong vong bairen_boot.
   Tat Bach Nhan la trap 6 map Bang Chien HONG AM THAM. Them Include cho doc lap.
"""
import io, os, shutil

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
C = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\tongcastle\jx1_edits"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)

# ---- ten TCVN3 (lay nguyen tu guideperson.lua de nhat quan) ----
GP = rd(os.path.join(E, r"script\missions\tongcastle\guideperson.lua"))
import re
names = {}
for m in re.finditer(r'szName\s*=\s*"([^"]+)",\s*nId\s*=\s*(\d+)', GP):
    names[int(m.group(2))] = m.group(1)
assert len(names) == 3, "khong doc duoc ten Than Moc Lenh: %r" % names
names[3204] = "B\xf9a Tri\xd6u Th\xf1 V\xd6"     # "Bùa Triệu Thủ Vệ" TCVN3
print("Ten se dat:")
for k in sorted(names): print("   %d -> %r" % (k, names[k]))

# ---- sua magicscript.txt (server + client neu co) ----
for root, tag in ((E, "server"), (C, "client")):
    p = os.path.join(root, r"settings\item\magicscript.txt")
    if not os.path.isfile(p):
        print("%-7s: khong co magicscript.txt - bo qua" % tag); continue
    s = rd(p)
    NL = "\r\n" if "\r\n" in s else "\n"
    ls = s.split(NL)
    n_fix = 0
    for line_no in range(1, len(ls)):
        c = ls[line_no].split("\t")
        if len(c) < 4: continue
        try: parti = int(c[3])
        except ValueError: continue
        if parti in names and c[1] == "6" and c[2] == "1":
            if c[0] != names[parti]:
                c[0] = names[parti]
                ls[line_no] = "\t".join(c)
                n_fix += 1
    if n_fix:
        shutil.copyfile(p, p + ".truoc_doiten_2408")
        wr(p, NL.join(ls))
    print("%-7s: doi ten %d item" % (tag, n_fix))

# ---- them Include vao tongwar_autoexec.lua ----
p = os.path.join(E, r"script\missions\tongwar\tongwar_autoexec.lua")
s = rd(p)
if "script\\\\lib\\\\file.lua" in s:
    print("tongwar_autoexec: da co Include file.lua")
else:
    NL = "\r\n" if "\r\n" in s else "\n"
    ins = ('Include("\\\\script\\\\lib\\\\file.lua")\t-- [KIEM CHUNG 24/08] tep nay goi GetTabFileHeight/' + NL +
           '-- GetTabFileData nhung truoc day KHONG Include gi: chi chay nho state startgame vo tinh co' + NL +
           '-- ham do qua duong vong bairen_boot. Tat Bach Nhan la trap 6 map Bang Chien hong am tham.' + NL +
           '-- (engine tu anh xa script\\lib\\file.lua -> scriptjx2\\lib\\file.lua, xem sJX2RemapScriptPath)' + NL)
    s = ins + s
    shutil.copyfile(p, p + ".truoc_include_2408")
    wr(p, s)
    print("tongwar_autoexec: DA THEM Include file.lua")

d = os.path.join(r"D:\GAMEDEVNEW\serverscript_jx2\tongwar\jx1_edits", r"script\missions\tongwar")
if not os.path.isdir(d): os.makedirs(d)
shutil.copyfile(p, os.path.join(d, "tongwar_autoexec.lua"))
mp = os.path.join(E, r"settings\item\magicscript.txt")
d2 = os.path.join(MIR, "settings", "item")
if not os.path.isdir(d2): os.makedirs(d2)
shutil.copyfile(mp, os.path.join(d2, "magicscript.txt"))
print("XONG")
