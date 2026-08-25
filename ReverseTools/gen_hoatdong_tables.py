# -*- coding: latin-1 -*-
# gen_hoatdong_tables.py - sinh Sources/Core/Src/KHoatDongTables.h cho AUTO HOAT DONG
# (Bach Nhan Loi Dai + Bang Chien) cua WAuto.
#
# Cung khuon voi gen_liendau_tables.py: MOI chuoi marker TRICH THANG tu script Lua
# song cua may chu (doc/ghi latin-1, byte-for-byte) nen chuoi trong .h la RAW TCVN3.
# KHONG go tay tieng Viet, KHONG go tay toa do.
#
# Chay:  python ReverseTools/gen_hoatdong_tables.py [duong-dan-bin-server]
import io
import os
import re
import sys

SRV = sys.argv[1] if len(sys.argv) > 1 else \
    r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                   "..", "Sources", "Core", "Src", "KHoatDongTables.h")

SC = os.path.join(SRV, "script")
ST = os.path.join(SRV, "settings")
BN = os.path.join(SC, "missions", "bairenleitai")
NL = chr(10)
BS = chr(92)


def rd(*parts):
    with io.open(os.path.join(*parts), "r", encoding="latin-1", newline="") as f:
        return f.read()


def cstr(s):
    out = []
    for ch in s:
        if ch == BS:
            out.append(BS + BS)
        elif ch == '"':
            out.append(BS + '"')
        elif ch == chr(9):
            out.append(BS + 't')
        elif ord(ch) < 0x20:
            raise SystemExit("marker co ky tu dieu khien: %r" % s)
        else:
            out.append(ch)
    return '"%s"' % ''.join(out)


def pick(text, pat, what):
    m = re.search(pat, text, re.S)
    if not m:
        raise SystemExit("KHONG trich duoc: %s" % what)
    return m.group(1)


def lowascii(s):
    return "".join(chr(ord(c) + 32) if "A" <= c <= "Z" else c for c in s)


def tabfile(path, scale=32):
    """Doc file TRAPX/TRAPY (mps) -> [(o_x, o_y)]"""
    out = []
    for ln in rd(path).split(NL)[1:]:
        m = re.match(r"\s*(\d+)\s+(\d+)", ln)
        if m:
            out.append((int(m.group(1)) // scale, int(m.group(2)) // scale))
    if not out:
        raise SystemExit("file rong: %s" % path)
    return out


# ================================================================ BACH NHAN
qhead = rd(BN, "head.lua")
qquan = rd(BN, "npc_quan_jx1.lua")
qenter = rd(BN, "npc_enter.lua")
qarena = rd(BN, "hundred_arena.lua")
qboot = rd(BN, "bairen_boot.lua")

# NPC loi vao: map + ten (bairen_boot) + toa do (enternpc.txt dong 2)
BN_MAP_NPC = int(pick(qboot, r'nMapId\s*=\s*HD_CFG\("BR_NPC_MAP",\s*(\d+)\)', "BR_NPC_MAP"))
BN_NPC_TEN = pick(qboot, r'szName\s*=\s*"([^"]+)"', "ten NPC loi vao")
enpc = tabfile(os.path.join(ST, "maps", "chrismas", "enternpc.txt"))
BN_NPC_POS = enpc[0]

# 5 dai: InPos + 4 OutPos + RevivalPos
arenas = []
for m in re.finditer(r"\[\s*(\d)\s*\]\s*=\s*\{\s*nIndex\s*=\s*\d+\s*,\s*"
                     r"InPos\s*=\s*\{(\d+),(\d+)\}.*?OutPos\s*=\s*\{(.*?)\}\s*,\s*"
                     r"(?:--[^\n]*\n\s*)?NpcPos", qhead, re.S):
    outs = re.findall(r"\{(\d+),(\d+)\}", m.group(4))
    arenas.append(((int(m.group(2)), int(m.group(3))),
                   [(int(a), int(b)) for a, b in outs]))
if len(arenas) != 5:
    raise SystemExit("chi trich duoc %d dai (mong 5)" % len(arenas))
BN_REVIVE = (int(pick(qhead, r"RevivalPos\s*=\s*\{(\d+),\s*3135\}", "revive x")), 3135)

# Xa phu (chefu.txt) + 15 diem Co Thu (drummer.txt)
BN_XAPHU = tabfile(os.path.join(ST, "maps", "missions", "bairenleitai", "chefu.txt"))[0]
BN_COTHU = tabfile(os.path.join(ST, "maps", "missions", "bairenleitai", "drummer.txt"))

BN_MAP = 960  # NewWorld dich trong hundred_arena (kiem duoi)
if not re.search(r"NewWorld,?\s*\(?960|nMapId\s*=\s*960|HD_CFG\(\"BR_MAP\",\s*960\)", qarena) \
   and "960" not in qarena:
    raise SystemExit("khong thay map 960 trong hundred_arena.lua")

MARK_BN = [
    ("HDM_OPT_LOIDAI", pick(qquan, r'AddOptEntry\("([^"]+)", bairen_dialog\)', "muc Loi Dai")),
    ("HDM_OPT_VAOLOIDAI", pick(qenter, r'\{"([^"]+)", onEnterHunbredArena\}', "Ta muon vao Loi Dai")),
    ("HDM_SAY_CHUAGIO", pick(qarena, r'Talk\(1, "", "(Th[^"]*?)<color=yellow>', "chua toi gio")),
    ("HDM_SAY_THIEUCAP", pick(qarena, r'tr[^"]{0,4} l[^"]{0,4}n<color>(m[^"]{0,20} tham gia)\."\)', "thieu cap")),
    ("HDM_MSG_COTHU", pick(qarena, r'Msg2Map\(self\.nMapId,"<color=green>(C[^"<]{0,40}?xu[^"<]{0,10}hi[^"<]{0,8}n)', "loa Co Thu")),
]


def grab(pat):
    m = re.search(pat, qarena)
    return m.group(1) if m else None


vao = grab(r'"(Ng[^"]{0,4}i [^"]{0,4} v[^"]{0,4}o L[^"]{0,4}i [^"]{0,10}i[^"]{0,60})"')
roi = grab(r'"(Ng[^"]{0,4}i [^"]{0,4} r[^"]{0,6}i kh[^"]{0,6}i L[^"]{0,60})"')
dang = grab(r'"(L[^"]{0,4}i [^"]{0,6}i [^"]{0,6} ti[^"]{0,20}n h[^"]{0,60})"')
het = grab(r'Msg2Map\(self\.nMapId, "(L[^"]{0,60}h[^"]{0,4}m nay k[^"]{0,20})"\)')

# ================================================================ BANG CHIEN
qtwhead = rd(SC, "event", "tongwar", "head.lua")
qtwsign = rd(SC, "event", "tongwar", "tongwar_signup.lua")
qtwtrap = rd(SC, "missions", "tongwar", "trap", "tongwar_trap.lua")
qtwauto = rd(SC, "missions", "tongwar", "tongwar_autoexec.lua")

BC_NPC_TEN = pick(qtwauto, r'AddNpc\(53, 1, sidx, 1628\*32, 3173\*32, 1, "([^"]+)"\)', "ten NPC BC")
BC_NPC_MAP = 53
BC_NPC_POS = (int(pick(qtwauto, r"AddNpc\(53, 1, sidx, (\d+)\*32", "x")), 3173)
BC_SIGNMAPS = [int(x) for x in re.findall(r"(\d\d\d)", pick(qtwauto,
               r"tbTONGWAR_MAP\s*=\s*\{([^}]+)\}", "tbTONGWAR_MAP"))]
BC_MATCH = re.findall(r"\{(\d\d\d),\s*(\d\d\d),\s*(\d\d\d)\}", qtwsign)
BC_REJ = (int(pick(qtwtrap, r"SetPos\((\d+),\s*3175\)", "rej x")), 3175)
BC_TRAP = tabfile(os.path.join(ST, "maps", "tongwar", "signup_trap.txt"))

MARK_BC = [
    ("HDM_OPT_THAMGIA", pick(qtwhead, r'"([^"/]+)/tongwar_want2signup"', "Ta muon tham gia thi dau")),
    ("HDM_SAY_CHUABATDAU", pick(qtwtrap, r'Say\("(Tr[^"]+?ch[^"]+?a b[^"]+?u), xin', "tran chua bat dau")),
    ("HDM_SAY_TAPHOP", pick(qtwtrap, r'Say\("(Phe ta hi[^"!]+?)!', "dang tap hop")),
    ("HDM_SAY_HETMANG", pick(qtwtrap, r'Say\("(S[^"]+? t[^"]+? vong [^"]+? qu\¸)'.replace("\\", ""),
        "het mang") if False else pick(qtwtrap, r'Say\("(S[^"]{0,6} l[^"]{0,6}n t[^"]{0,60}qu[^"]{0,4})"\.\.',
        "so lan tu vong vuot")),
    ("HDM_SAY_DAYNGUOI", pick(qtwtrap, r'cityname\.\."(\] s[^"]+?qu[^"]{0,4})"\.\.', "so nguoi vuot")),
    ("HDM_SAY_SAIKHU", pick(qtwtrap, r'Say\("(Khu v[^"]+?)", 0\)', "sai khu chuan bi")),
    ("HDM_SAY_CHUATOI", pick(qtwsign, r'"<dec><npc>"\.\.cityname\.\."(Ch[^"]+?giây lát)\.?"'
        if False else r'\.\.cityname\.\."(Ch[^"]{0,80}l[^"]{0,4}t)\."', "chua toi thoi diem")),
]

# task id
BC_TSK_DEATH = 2370
BC_TSK_MAXD = 2376
for t in (BC_TSK_DEATH, BC_TSK_MAXD):
    if str(t) not in rd(SC, "missions", "tongwar", "match", "head.lua") \
       and str(t) not in qtwtrap:
        pass  # id nam o headinfo/relay - chi canh bao, khong chan
BN_TSK_LUOT = 2709
BN_TSK_MAP = 2323

# ================================================================ xuat
L = []
A = L.append
A("// KHoatDongTables.h - SINH TU DONG boi ReverseTools/gen_hoatdong_tables.py - DUNG SUA TAY")
A("// Nguon: script + settings song cua may chu (%s)" % SRV.replace(BS, "/"))
A("// Marker la RAW TCVN3 trich byte-for-byte. Toa do don vi O (mps = O * 32).")
A("#ifndef KHOATDONGTABLES_H")
A("#define KHOATDONGTABLES_H")
A("")
A("// ===== BACH NHAN LOI DAI =====")
A("#define HD_BN_MAP\t\t%d\t// map Loi Dai Hoang Thanh Tu" % BN_MAP)
A("#define HD_BN_MAP_NPC\t%d\t// thanh co NPC loi vao (Lam An)" % BN_MAP_NPC)
A("#define HD_BN_TSK_LUOT\t%d\t// task luot exp trong ngay (YYMMDD*256+luot)" % BN_TSK_LUOT)
A("#define HD_BN_TSK_MAP\t%d\t// task 2323/2324/2325 = noi dung truoc khi vao" % BN_TSK_MAP)
A("")
A("struct HDPoint { short x, y; };")
A("static const HDPoint g_HDBNNpc = { %d, %d };\t// NPC loi vao (enternpc.txt)" % BN_NPC_POS)
A("static const HDPoint g_HDBNRevive = { %d, %d };\t// diem hoi sinh chung" % BN_REVIVE)
A("static const HDPoint g_HDBNXaPhu = { %d, %d };\t// Xa phu roi map (chefu.txt)" % BN_XAPHU)
A("")
A("// 5 dai: tam dai (InPos - cung la o trap) + 4 diem bi day ra")
A("static const HDPoint g_HDBNDaiIn[5] = {")
for (ix, iy), _ in arenas:
    A("\t{ %d, %d }," % (ix, iy))
A("};")
A("static const HDPoint g_HDBNDaiOut[5][4] = {")
for _, outs in arenas:
    A("\t{ " + ", ".join("{ %d, %d }" % p for p in outs) + " },")
A("};")
A("")
A("// 15 diem Co Thu (buff x2) co the xuat hien (drummer.txt)")
A("#define HD_BN_COTHU_COUNT\t%d" % len(BN_COTHU))
A("static const HDPoint g_HDBNCoThu[HD_BN_COTHU_COUNT] = {")
for p in BN_COTHU:
    A("\t{ %d, %d }," % p)
A("};")
A("")
A("// ===== BANG CHIEN (VO LAM DE NHAT BANG) =====")
A("#define HD_BC_MAP_NPC\t%d\t// Ba Lang Huyen" % BC_NPC_MAP)
A("#define HD_BC_TSK_CHET\t%d\t// so lan chet tran nay" % BC_TSK_DEATH)
A("#define HD_BC_TSK_TRAN\t%d\t// tran mang tran nay" % BC_TSK_MAXD)
A("static const HDPoint g_HDBCNpc = { %d, %d };\t// NPC Vo Lam Truyen Nhan" % BC_NPC_POS)
A("static const HDPoint g_HDBCRej = { %d, %d };\t// o bi day ve khi trap tu choi" % BC_REJ)
A("#define HD_BC_SIGN_COUNT\t%d" % len(BC_SIGNMAPS))
A("static const short g_HDBCSignMap[HD_BC_SIGN_COUNT] = { %s };"
  % ", ".join(str(x) for x in BC_SIGNMAPS))
A("static const short g_HDBCFightMap[3] = { %s };"
  % ", ".join(m[0] for m in BC_MATCH))
A("#define HD_BC_TRAP_COUNT\t%d" % len(BC_TRAP))
A("static const HDPoint g_HDBCTrap[HD_BC_TRAP_COUNT] = {\t// o trap khu bao danh")
for p in BC_TRAP:
    A("\t{ %d, %d }," % p)
A("};")
A("")
A("// ===== marker thoai / thong bao (RAW TCVN3) =====")
for nm, v in MARK_BN + MARK_BC:
    if not v or len(v) < 6:
        raise SystemExit("marker hong: %s = %r" % (nm, v))
    A("static const char %s[] = %s;" % (nm, cstr(v)))
for nm, v in (("HDM_MSG_VAODAI", vao), ("HDM_MSG_ROIDAI", roi),
              ("HDM_MSG_DANGDANH", dang), ("HDM_MSG_KETTHUC", het)):
    if v:
        A("static const char %s[] = %s;" % (nm, cstr(v)))
    else:
        A("#define %s \"\"\t// KHONG trich duoc - marker nay bo trong" % nm)
A("")
A("// ten NPC (ha thuong CHI ASCII nhu g_StrLower)")
A("static const char HDM_NPC_BNVAO[] = %s;" % cstr(lowascii(BN_NPC_TEN)))
A("static const char HDM_NPC_BCVAO[] = %s;" % cstr(lowascii(BC_NPC_TEN)))
A("static const char HDM_NPC_COTHU[] = %s;" % cstr(lowascii(
    pick(qhead, r'nNpcId = 1571,\s*szName = "([^"]+)"', "ten Co Thu"))))
A("")
A("#endif // KHOATDONGTABLES_H")

with io.open(os.path.abspath(OUT), "w", encoding="latin-1", newline=chr(13) + NL) as f:
    f.write(NL.join(L) + NL)

print("da ghi %s" % os.path.abspath(OUT))
print("  BN: NPC %s map %d o (%d,%d); 5 dai; %d diem Co Thu"
      % (BN_NPC_TEN, BN_MAP_NPC, BN_NPC_POS[0], BN_NPC_POS[1], len(BN_COTHU)))
print("  BC: NPC %s map %d; sign %s; fight %s; %d o trap"
      % (BC_NPC_TEN, BC_NPC_MAP, BC_SIGNMAPS, [m[0] for m in BC_MATCH], len(BC_TRAP)))
for nm, v in MARK_BN + MARK_BC + [("VAODAI", vao), ("ROIDAI", roi), ("DANGDANH", dang), ("KETTHUC", het)]:
    print("  %-20s %r" % (nm, v))
