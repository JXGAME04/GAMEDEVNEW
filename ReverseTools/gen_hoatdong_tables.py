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
    ("HDM_SAY_HETMANG", pick(qtwtrap, r'Say\("(S[^"]+? t[^"]+? vong [^"]+? qu\Â¸)'.replace("\\", ""),
        "het mang") if False else pick(qtwtrap, r'Say\("(S[^"]{0,6} l[^"]{0,6}n t[^"]{0,60}qu[^"]{0,4})"\.\.',
        "so lan tu vong vuot")),
    ("HDM_SAY_DAYNGUOI", pick(qtwtrap, r'cityname\.\."(\] s[^"]+?qu[^"]{0,4})"\.\.', "so nguoi vuot")),
    ("HDM_SAY_SAIKHU", pick(qtwtrap, r'Say\("(Khu v[^"]+?)", 0\)', "sai khu chuan bi")),
    ("HDM_SAY_CHUATOI", pick(qtwsign, r'"<dec><npc>"\.\.cityname\.\."(Ch[^"]+?giÃ¢y lÃ¡t)\.?"'
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

# ================================================================ TIN SU
qpost = rd(SC, "task", "tollgate", "messenger", "posthouse.lua")
qruk = rd(SC, "task", "tollgate", "messenger", "messenger_turerukou.lua")
qwag = rd(SC, "task", "tollgate", "messenger", "wagoner.lua")
qdq = rd(SC, "global", "npcchucnang", "dichquan.lua")
qxp = rd(SC, "global", "npcchucnang", "xaphu.lua")
qkill = rd(SC, "task", "tollgate", "killbosshead.lua")
qtnpc = rd(SC, "task", "tollgate", "tinsu_addnpc.lua")

# Dich quan trong thanh (AddNpcNew 377 + dichquan.lua): Thanh Do 11 + Dai Ly 162
qtdo = rd(SC, "startgame", "thanh", "thanhdo.lua")
qdly = rd(SC, "startgame", "thanh", "daily.lua")
m = re.search(r"AddNpcNew\(377,1,11,(\d+)\*32,(\d+)\*32,[^)]*dichquan", qtdo)
TS_DQ_11 = (int(m.group(1)), int(m.group(2)))
m = re.search(r"AddNpcNew\(377,1,162,(\d+)\*32,(\d+)\*32,[^)]*dichquan", qdly)
TS_DQ_162 = (int(m.group(1)), int(m.group(2)))

# Dich quan TRONG AI 395 (tinsu_addnpc): {377,395,x,y,...turerukou...}
m = re.search(r"\{377,395,(\d+),(\d+),[^}]*turerukou[^}]*\}", qtnpc)
TS_DQ_AI = (int(m.group(1)), int(m.group(2)))
# ten hien thi "Dich quan" (TCVN3) trong bang tinsu_dialognpc
TS_TEN_DQ = pick(qtnpc, r'turerukou\.lua",\s*"([^"]+)"', "ten Dich quan")

# Tieu Tran - NPC CUA RA cua ai 395 (dong {842,395,x,y,...turenpc...,"Tieu Tran"})
m = re.search(r"\{842,395,(\d+),(\d+),[^}]*turenpc[^}]*\"([^\"]+)\"\}", qtnpc)
TS_TT_XY = (int(m.group(1)), int(m.group(2)))
TS_TEN_TT = m.group(3)
qtnp = rd(SC, "task", "tollgate", "messenger", "qianbaoku", "messenger_turenpc.lua")
qbcsu = rd(SC, "event", "tongwar", "tongwar_signup.lua")
qbcax = rd(SC, "missions", "tongwar", "tongwar_autoexec.lua")
qbccf = rd(SC, "missions", "tongwar", "npc", "chefu.lua")
qhar = rd(SC, "missions", "bairenleitai", "hundred_arena.lua")

# Xa phu khu bao danh Bang Chien: dong AddNpc {393,...,x,y} + ten "Xa phu"
m = re.search(r"\{\s*393,\s*(\d+),\s*(\d+),", qbcax)
BC_XAPHU = (int(m.group(1)), int(m.group(2)))

# 9 ruong map 395 (killbosshead: {844,100,395,x,y,...})
TS_RUONG = []
for m in re.finditer(r"(?m)^\{844,100,395,(\d+),(\d+),0,\"([^\"]+)\",1,", qkill):
    TS_RUONG.append((int(m.group(1)), int(m.group(2))))
if len(TS_RUONG) != 9:
    raise SystemExit("chi trich duoc %d ruong (mong 9)" % len(TS_RUONG))
TS_TEN_RUONG = pick(qkill, r'(?m)^\{844,100,395,\d+,\d+,0,"(B[^"]*?)\s*\d",1,', "ten Bao ruong")
TS_TEN_GIU = pick(qkill, r'(?m)^\{849,100,395,[^,]+,\d+,0,"(B[^"]*?)\s*\d",1,', "ten Thu Ho Gia")

# tuyen di (citygo posthouse): 2 dong song: {1204,1,...,1203,11,162,x,y} - dich den
TS_VE = []
for m in re.finditer(r"\{1204,(\d),[^{}]*?,1203,(\d+),(\d+),(\d+),(\d+)\}", qpost):
    TS_VE.append((int(m.group(1)), int(m.group(2)), int(m.group(3)),
                  int(m.group(4)), int(m.group(5))))
if len(TS_VE) != 2:
    raise SystemExit("citygo song phai co 2 dong (%d)" % len(TS_VE))

MARK_TS = [
    ("HDM_OPT_TINSU", pick(qdq, r'"([^"/]+)/especiallymessenger"', "muc Nhiem vu Tin Su")),
    ("HDM_OPT_TSNHAN", pick(qpost, r'"([^"/]+)/messenger_ido"', "Ta bang long!")),
    ("HDM_OPT_TSGIAO", pick(qpost, r'"([^"/]+)/messenger_finishtask"', "Ta den de giao").rstrip()),
    ("HDM_OPT_TSTHUONG", pick(qpost, r'"([^"/]+)/messenger_treasureprize"', "Nhan lanh phan thuong")),
    ("HDM_OPT_TSXAPHU", pick(qxp, r'"([^"/]+)/messenger_wagoner"', "Di noi dac biet")),
    ("HDM_OPT_TSMUON", pick(qwag, r'"([^"/]+)/messenger_storewagoner"', "Muon")),
    ("HDM_OPT_TSBATDAU", pick(qruk, r'"([^"/]+)/ture_try_starttask"', "Bat dau nhiem vu").rstrip()),
    ("HDM_OPT_TSTIEPTUC", pick(qruk, r'"([^"/]+)/ture_continuetask"', "Tiep tuc nhiem vu")),
    ("HDM_OPT_TSROI", pick(qruk, r'"([^"/]+)/ture_movecity"', "Roi khoi khu vuc")),
    ("HDM_OPT_TSMUONRA", pick(qtnp, r'AddOptEntry\("([^"]+)",messenger_main\)', "Ta muon di ra")),
    ("HDM_OPT_TSDUNGRA", pick(qtnp, r'"([^"/]+)/messenger_comeback"', "Dung! Ta muon roi khoi")),
    ("HDM_OPT_TSCHUAXONG", pick(qtnp, r'"([^"/]+)/messenger_icantdo"', "Ta chua hoan thanh khao nghiem")),
    ("HDM_SAY_TSDOITRUONG", pick(qruk, r'"[^"]*?(kh«ng ph¶i ®éi tr­ëng)', "khong phai doi truong")),
    ("HDM_SAY_BCKHONGBANG", pick(qbcsu, r'(Ch\xada gia nh\xcbp bang)', "chua gia nhap bang")),
    ("HDM_SAY_BCKHONGLM", pick(qbcsu, r'(kh\xabng c\xe3 li\xaan minh)', "khong co lien minh")),
    ("HDM_SAY_BCTHIEUCAP", pick(qbcsu, r'(Ph\xb6i \xae\xb9t c\xcap )', "phai dat cap ..")),
    ("HDM_SAY_BCDOCAM", pick(qbcsu, r'(Kh\xabng \xae\xad\xeec ph\xd0p mang v\xcbt ph\xc8m)', "khong duoc mang vat pham")),
    ("HDM_MSG_BNMET", pick(qhar, r'(Ta m\xd6t r\xe5i)', "co thu het suat")),
    ("HDM_MSG_DADOC", pick(qhar, r'(\xa7\xb7 \xae\xe4c s\xed)', "da doc si roi")),
    ("HDM_MSG_TSCHUAHA", pick(rd(SC, "task", "tollgate", "messenger", "qianbaoku", "90", "tureboss90.lua"), r'(ch\xada h\xb9 \xae\xad\xeec ng\xad\xeai gi\xf7 r\xad\xacng)', "chua ha nguoi giu ruong")),
    ("HDM_MSG_NGHERO", pick(qhar, r'(V\xcby ng\xad\xaci \xae\xb7 nghe r\xe2 r\xe5i)', "nhan buff ok")),
    ("HDM_SAY_TSHETLUOT", pick(qpost, r'"[^"]*?(H\xabm nay ng[^"]{0,20} v[^"]{0,10}t v[^"]{0,60})\."', "het luot")),
    ("HDM_SAY_TSMETMOI", pick(qpost, r'"[^"]*?(H\xabm nay ng[^"]{0,20} m[^"]{0,10}t m[^"]{0,10}i r[^"]{0,6}i)\.', "het luot can lenh bai")),
    ("HDM_SAY_TSQUAMET", pick(qpost, r'"[^"]*?(h\xabm nay ng[^"]{0,20} qu[^"]{0,8} m[^"]{0,10}t r[^"]{0,6}i)\.', "qua met (prize)")),
    ("HDM_SAY_TSDANGCO", pick(qpost, r'"[^"]*?(kh\xabng th[^"]{0,10} ti[^"]{0,10}p nh[^"]{0,10}n nhi[^"]{0,20}m v[^"]{0,30} gi[^"]{0,10}ng nhau)', "dang co nhiem vu")),
    ("HDM_SAY_TSTHIEUCAP", pick(qpost, r'"[^"]*?(ch[^"]{0,10}a \xae\xf1 90)', "thieu cap 90")),
    ("HDM_SAY_TSTHIEUO", pick(qpost, r'"[^"]*?(kh\xabng \xae\xf1 ch[^"]{0,10} tr[^"]{0,10}ng)', "thieu o tui")),
]
for _n, _v in MARK_TS:
    if not _v or len(_v) < 4:
        raise SystemExit("marker TS hong: %s = %r" % (_n, _v))

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
A("// ===== TIN SU (nhiem vu dua tin qua Thien Bao Kho map 395) =====")
A("#define HD_TS_MAP_AI\t395\t// ai Thien Bao Kho")
A("#define HD_TS_TSK_MA\t1201\t// ma 5 ruong phai mo (theo THU TU)")
A("#define HD_TS_TSK_DA\t1202\t// cac ruong da mo (prefix cua 1201)")
A("#define HD_TS_TSK_TT\t1203\t// 0 khong/that bai; 10 da nhan; 20 trong ai; 21 tam ngung; 25/30 xong")
A("#define HD_TS_TSK_TUYEN\t1204\t// 1 = Thanh Do->Dai Ly, 2 = nguoc lai")
A("#define HD_TS_TSK_NGAY\t4128\t// so luot hom nay (YYMMDD*256 + n)")
A("static const HDPoint g_HDTSDq11 = { %d, %d };\t// Dich quan Thanh Do" % TS_DQ_11)
A("static const HDPoint g_HDTSDq162 = { %d, %d };\t// Dich quan Dai Ly" % TS_DQ_162)
A("static const HDPoint g_HDTSDqAi = { %d, %d };\t// Dich quan trong ai 395" % TS_DQ_AI)
A("static const HDPoint g_HDTSTieuTran = { %d, %d };\t// Tieu Tran - NPC CUA RA (sau trong ai)" % TS_TT_XY)
A("static const HDPoint g_HDBCXaPhu = { %d, %d };\t// Xa phu khu bao danh BC (ve thanh)" % BC_XAPHU)
A("// tuyen: { tuyen, map nhan, map dich, x dich, y dich } (citygo posthouse.lua)")
A("static const short g_HDTSVe[2][5] = {")
for t in TS_VE:
    A("\t{ %d, %d, %d, %d, %d }," % t)
A("};")
A("// 9 Bao ruong map 395 (ruong i+1; Thu Ho Gia dung canh, lech +2 o x)")
A("static const HDPoint g_HDTSRuong[9] = {")
for p in TS_RUONG:
    A("\t{ %d, %d }," % p)
A("};")
A("")
for _n, _v in MARK_TS:
    A("static const char %s[] = %s;" % (_n, cstr(_v)))
A("")
A("// ten NPC (ha thuong CHI ASCII nhu g_StrLower)")
A("static const char HDM_NPC_BNVAO[] = %s;" % cstr(lowascii(BN_NPC_TEN)))
A("static const char HDM_NPC_BCVAO[] = %s;" % cstr(lowascii(BC_NPC_TEN)))
A("static const char HDM_NPC_COTHU[] = %s;" % cstr(lowascii(
    pick(qhead, r'nNpcId = 1571,\s*szName = "([^"]+)"', "ten Co Thu"))))
A("static const char HDM_NPC_DICHQUAN[] = %s;" % cstr(lowascii(TS_TEN_DQ)))
A("static const char HDM_NPC_TSRUONG[] = %s;" % cstr(lowascii(TS_TEN_RUONG)))
A("static const char HDM_NPC_TSGIU[] = %s;" % cstr(lowascii(TS_TEN_GIU)))
A("static const char HDM_NPC_TIEUTRAN[] = %s;" % cstr(lowascii(TS_TEN_TT)))
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
