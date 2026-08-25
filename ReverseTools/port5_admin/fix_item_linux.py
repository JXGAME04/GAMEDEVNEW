# -*- coding: utf-8 -*-
r"""[24/08] VA ITEM HOAT DONG cho dung ban Linux (41 tac nhan / 34 phat hien / 20 xac nhan).

A1 [CHUC NANG] exp_jx1.lua dung AddOwnExp -> KPlayer::DirectAddExp -> LevelUp() dat m_nExp = 0
   (KPlayer.cpp:2629) => len cap la MAT SACH exp du, va chi len DUNG 1 CAP.
   Vd cap 90 exp 0 an Qua Dai Hoang Kim (200tr): can 131tr len 91 -> exp ve 0 -> MAT 69tr.
   Dung: AddSumExp (ScriptFuns.cpp:8786) cong TUNG CAP mot - cac item exp lon cua JX1 deu dung
   (goldenseed.lua:19, huihuangzhiguo*.lua:21, ib\lbdiemexp.lua:12).
   Sua 1 dong => va luon 4864 + 3205 + 3207.

B1 [HIEN THI - nang nhat] bang item ban CLIENT thieu 8 dong cuoi (4858 vs 4866 dong).
   Engine tra bang bang CHI SO MANG = particular (KItemGenerator.CPP:1367) nen client
   GetRecord(4857)/GetRecord(4864) = NULL => O DO TRANG. Chep 8 dong server sang client.
   CHU Y: dong cuoi bang client KHONG co CRLF -> phai them truoc khi noi.

B2 [HIEN THI] 4 item Thanh Bao 3204-3207 dung CHUNG 1 icon (taskobj056/387) va mo ta con la
   chuoi dat cho tieng Trung. Ban Linux co icon RIENG + mo ta tieng Viet san
   (D:\ServerLinux\server1\settings\item\004\magicscript.txt:3811-3814) -> chep nguyen byte.

B3 [HIEN THI] Bua Trieu Thu Ve 3204: cot xep chong = 0 (khong gop), ban Linux = 50.

B4 [HIEN THI] Hoang Chan Don 2273: mo ta chep nham cua Tu Kim Chan Don ("gia tang 12 thanh
   cong luc") trong khi item chi cong 2 ty kinh nghiem.
"""
import io, os, shutil

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
LIN = r"D:\ServerLinux\server1\settings\item\004\magicscript.txt"
ENC = "latin-1"

def rd_lines(p):
    b = io.open(p, "rb").read()
    return b.decode(ENC), b

def bak(p, tag):
    q = p + ".truoc_" + tag
    if not os.path.isfile(q): shutil.copyfile(p, q)

# ================= A1: AddOwnExp -> AddSumExp =================
p = os.path.join(SRV, r"script\lib\awardtype\exp_jx1.lua")
s = io.open(p, "r", encoding=ENC, newline="").read()
if "AddSumExp" in s:
    print("A1: da va")
else:
    NL = "\r\n" if "\r\n" in s else "\n"
    old = "\t\tAddOwnExp(nExp)"
    assert s.count(old) == 1, "A1 anchor = %d" % s.count(old)
    s = s.replace(old,
        "\t\t-- [FIX 24/08] AddOwnExp -> DirectAddExp -> LevelUp() dat m_nExp = 0" + NL +
        "\t\t-- (KPlayer.cpp:2629) => MAT SACH exp du khi len cap, va chi len DUNG 1 cap." + NL +
        "\t\t-- AddSumExp (ScriptFuns.cpp:8786) cong TUNG CAP mot nen khong mat gi." + NL +
        "\t\tAddSumExp(nExp)", 1)
    bak(p, "expfix_2408"); io.open(p, "w", encoding=ENC, newline="").write(s)
    print("A1: exp_jx1.lua AddOwnExp -> AddSumExp (va luon 4864/3205/3207)")

# ================= doc 3 bang =================
srv_p = os.path.join(SRV, r"settings\item\magicscript.txt")
cli_p = os.path.join(CLI, r"settings\item\magicscript.txt")
srv_txt, _ = rd_lines(srv_p)
cli_txt, _ = rd_lines(cli_p)
lin_txt, _ = rd_lines(LIN)
NL = "\r\n"
srv = srv_txt.split(NL)
cli = cli_txt.split(NL)
lin = lin_txt.split(NL)
print("server %d dong | client %d dong | linux %d dong" % (len(srv), len(cli), len(lin)))

# ================= B1: bo sung 8 dong cuoi cho client =================
if len(cli) < len(srv):
    bak(cli_p, "them8dong_2408")
    # bo dong rong cuoi (neu co) roi noi
    while cli and cli[-1].strip() == "":
        cli.pop()
    them = srv[len(cli):]
    them = [x for x in them if x.strip() != ""]
    cli.extend(them)
    io.open(cli_p, "w", encoding=ENC, newline="").write(NL.join(cli) + NL)
    print("B1: da them %d dong vao bang client (nay %d dong)" % (len(them), len(cli)))
    cli_txt, _ = rd_lines(cli_p); cli = cli_txt.split(NL)
else:
    print("B1: bang client da du dong")

# ================= B2/B3/B4: sua cot theo ban Linux =================
def cols(line): return line.split("\t")
def put(lines, idx, col, val):
    c = cols(lines[idx])
    while len(c) <= col: c.append("")
    c[col] = val
    lines[idx] = "\t".join(c)

# xac dinh chi so cot tu dong tieu de
hdr = cols(srv[0])
def col_of(name, default):
    for i, h in enumerate(hdr):
        if h.strip().lower() == name.lower(): return i
    return default
C_PARTI = col_of("ParticularType", 3)
C_IMG   = col_of("ImageName", 4)
C_OBJ   = col_of("ObjIdx", 5)
C_INTRO = col_of("Intro", 8)
C_STACK = col_of("nMaxStack", 12)
print("chi so cot: parti=%d img=%d obj=%d intro=%d stack=%d" % (C_PARTI, C_IMG, C_OBJ, C_INTRO, C_STACK))

# lap ban do particular -> dong (ban Linux)
lin_map = {}
for i, l in enumerate(lin):
    c = cols(l)
    if len(c) > C_PARTI:
        try: lin_map[int(c[C_PARTI])] = c
        except ValueError: pass

def find_row(lines, parti):
    for i, l in enumerate(lines):
        c = cols(l)
        if len(c) > C_PARTI:
            try:
                if int(c[C_PARTI]) == parti: return i
            except ValueError: pass
    return -1

changed = {"srv": 0, "cli": 0}
for tag, lines, path in (("srv", srv, srv_p), ("cli", cli, cli_p)):
    n = 0
    for parti in (3204, 3205, 3206, 3207):
        i = find_row(lines, parti)
        if i < 0 or parti not in lin_map: continue
        lc = lin_map[parti]
        # B2: icon + objidx + mo ta lay NGUYEN BYTE tu Linux
        if len(lc) > C_IMG and cols(lines[i])[C_IMG] != lc[C_IMG]:
            put(lines, i, C_IMG, lc[C_IMG]); n += 1
        if len(lc) > C_OBJ and cols(lines[i])[C_OBJ] != lc[C_OBJ]:
            put(lines, i, C_OBJ, lc[C_OBJ]); n += 1
        if len(lc) > C_INTRO and cols(lines[i])[C_INTRO] != lc[C_INTRO]:
            put(lines, i, C_INTRO, lc[C_INTRO]); n += 1
        # B3: chi rieng 3204 xep chong 50 (Linux); 3205-3207 giu 0 nhu Linux
        if parti == 3204 and len(lc) > C_STACK:
            put(lines, i, C_STACK, "50"); n += 1
    # B4: mo ta Hoang Chan Don 2273
    i = find_row(lines, 2273)
    if i >= 0:
        moi = ("S\xf8 d\xf4ng nh\xacn 2 t\xfd \xaei\xd3m kinh nghi\xd6m (b\xdc gi\xedi h\xb9n theo c\xa5p nh\xa9n v\xadt). "
               "30 ng\xb5y ch\xd8 d\xf9ng \xae\xdaîc 1 l\xa7n.")
        cur = cols(lines[i])
        if len(cur) > C_INTRO and "12 th" in cur[C_INTRO]:
            put(lines, i, C_INTRO, moi); n += 1
    if n:
        bak(path, "cotlinux_2408")
        io.open(path, "w", encoding=ENC, newline="").write(NL.join(lines) + ("" if lines[-1] == "" else NL))
    changed[tag] = n
    print("%s: sua %d o" % (tag, n))

print("XONG")
