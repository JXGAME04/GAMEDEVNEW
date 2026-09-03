# -*- coding: latin-1 -*-
# gen_congthanh_tables.py - sinh Sources/Core/Src/KCongThanhTables.h cho AUTO CONG THANH CHIEN (WAuto).
#
# Moi chuoi marker duoc TRICH THANG tu script Lua song cua may chu (byte-for-byte, doc/ghi
# latin-1) nen chuoi trong .h la RAW TCVN3 - y het gen_tongkim_tables.py. KHONG go tay
# tieng Viet vao .h.
#
# O TRAP: client KHONG nap du lieu trap (KRegion::GetTrap tra 0 ngoai _SERVER, Region_C.dat
# chi co NPC/Obj) nen toa do o trap phai doc offline tu Region_S.dat cua may chu:
#   - map 221 (chien truong): thu muc map tren dia may chu (MapList.ini 221=...)
#   - map 222/223 (hau phuong, cung mot thu muc map): KHONG co tren dia -> doc tu pak Linux
#     D:\ServerLinux\server1\pak\maps.pak (muc nen UCL NRV2B - giai nen thuan Python).
#   Ket qua duoc cache o ReverseTools\congthanh_trap_cache.json de lan sau khong can pak.
# Trap id trong Region_S.dat = g_FileName2Id(duong dan script) (KFilePath.cpp:442) - doi
# chieu bang cach bam MOI .lua duoi bin\server\script.
#
# Chay:  python ReverseTools/gen_congthanh_tables.py [<bin\server>] [<maps.pak Linux>]
import io
import json
import os
import re
import struct
import sys

SRV_BIN = sys.argv[1] if len(sys.argv) > 1 else r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
PAK_LINUX = sys.argv[2] if len(sys.argv) > 2 else r"D:\ServerLinux\server1\pak\maps.pak"
HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "..", "Sources", "Core", "Src", "KCongThanhTables.h")
CACHE = os.path.join(HERE, "congthanh_trap_cache.json")

SRV = os.path.join(SRV_BIN, "script")
CW = os.path.join(SRV, "missions", "citywar_city")
CG = os.path.join(SRV, "missions", "citywar_global")

NL = chr(10)
BS = chr(92)
TAB = chr(9)


def rd(path):
    with io.open(path, "r", encoding="latin-1", newline="") as f:
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
            raise ValueError("marker co ky tu dieu khien")
        else:
            out.append(ch)
    return '"%s"' % ''.join(out)


def pick(text, pat, what, flags=0):
    m = re.search(pat, text, flags)
    if not m:
        raise SystemExit("KHONG trich duoc marker: %s" % what)
    return m.group(1)


def num(text, name):
    m = re.search(re.escape(name) + r"\s*=\s*(\d+)", text)
    if not m:
        raise SystemExit("KHONG doc duoc hang so %s" % name)
    return int(m.group(1))


def line_with(text, ascii_anchor, what):
    for ln in text.splitlines():
        if ascii_anchor in ln:
            return ln
    raise SystemExit("KHONG thay dong chua %r (%s)" % (ascii_anchor, what))


def say_after(text, ascii_anchor, what, stop):
    """Cau Say("...") dau tien SAU dong chua ascii_anchor; cat toi ky tu stop dau tien."""
    lines = text.splitlines()
    for i, ln in enumerate(lines):
        if ascii_anchor in ln:
            for j in range(i, min(i + 6, len(lines))):
                m = re.search(r'Say\("([^"]*)"', lines[j])
                if m:
                    s = m.group(1)
                    k = s.find(stop)
                    return s[:k] if k > 0 else s
    raise SystemExit("KHONG thay Say sau %r (%s)" % (ascii_anchor, what))


# ---------------- 1. marker hoi thoai / tin ----------------
s_xaphu = rd(os.path.join(SRV, "global", "npcchucnang", "xaphu.lua"))
s_stat = rd(os.path.join(CG, "station_ctc.lua"))
s_trap = rd(os.path.join(CW, "zhongzhuan_map", "trap.lua"))
s_camper = rd(os.path.join(CW, "camper.lua"))
s_chefu = rd(os.path.join(CW, "zhongzhuan_map", "chefu.lua"))
s_mission = rd(os.path.join(CW, "mission.lua"))
s_head = rd(os.path.join(CW, "head.lua"))
s_ctrap1 = rd(os.path.join(CW, "chengzhan_map", "ctrap1.lua"))
s_ctrap2 = rd(os.path.join(CW, "chengzhan_map", "ctrap2.lua"))
s_infoc = rd(os.path.join(CG, "infocenter_head.lua"))

ANY = '([^"' + NL + '/]*)'
MARK = []
MARK.append(("CTM_OPT_DICHIENTRUONG", pick(s_xaphu, '"' + ANY + '/GoCityWar"', "dong Di Chien truong cong thanh (xaphu.lua)")))
MARK.append(("CTM_OPT_CONG", pick(s_stat, '"' + ANY + '/GoCityWarAttack"', "dong Ben cong").strip()))
MARK.append(("CTM_OPT_THU", pick(s_stat, '"' + ANY + '/GoCityWarDefend"', "dong Ben thu").strip()))
MARK.append(("CTM_OPT_KHONGBEN", pick(s_stat, '"' + ANY + '/CancelGoCityWar"', "dong Khong ben nao").strip()))
# cau thoai chon phe: format("... ben cong<%s>, ben thu<%s> ...", GetCityAreaName(CityID), Tong1, Tong2)
ln = line_with(s_stat, "GetCityAreaName(CityID), Tong1, Tong2", "cau thoai chon phe")
fmt = ln[ln.index('format("') + 8:ln.index('", GetCityAreaName')]
i1 = fmt.index("<%s>")
a = fmt[:i1]
MARK.append(("CTM_SAY_BENCONG", a[a.rindex(", ") + 2:] + "<"))
fmt2 = fmt[i1 + 4:]
i2 = fmt2.index("<%s>")
b = fmt2[:i2]
MARK.append(("CTM_SAY_BENTHU", b[b.rindex(", ") + 2:] + "<"))
MARK.append(("CTM_SAY_THANPHAN", say_after(s_stat, "GetItemCountEx(CardTab[CityID * 2]) < 1", "than phan chua phu hop", "!")))
MARK.append(("CTM_SAY_TAPHOP", say_after(s_trap, "GetMissionV(MS_STATE) == 0", "phe ta dang tap hop", "!")))
MARK.append(("CTM_SAY_QUANGAN", say_after(s_trap, "JoinWithCard(Camp, 0) == 0", "vao bang qua ngan", ",")))
MARK.append(("CTM_SAY_KHONGLENHBAI", say_after(s_trap, "if (UI == 1) then", "khong co lenh bai", "!")))
# "... da day, tam thoi khong the gia nhap!" -> lay 2 chu cuoi truoc dau phay
s_day = say_after(s_camper, "GetMSPlayerCount(MISSIONID,Camp) >= MAX_CAMP1COUNT", "phe da day", ",")
MARK.append(("CTM_SAY_DAYNGUOI", " ".join(s_day.split(" ")[-2:])))
MARK.append(("CTM_OPT_ROIDAUTRUONG", pick(s_chefu, '"' + ANY + '/LeaveChefu"', "dong Roi khoi dau truong").strip()))
MARK.append(("CTM_OPT_THANHTHI", pick(s_chefu, '"' + ANY + '/StationFun"', "dong thanh thi da di qua").strip()))
# tin toan may chu: RunMission "%s cong thanh chien chinh thuc bat dau! ..."
ln = line_with(s_mission, "GetGamerName(1), GetGamerName(2))", "tin khai chien")
fmt = ln[ln.index('format("') + 8:]
MARK.append(("CTM_NEWS_KHAICHIEN", fmt[fmt.index("%s ") + 3:fmt.index("!")]))
# GameWin: str = "Chung cuoc" .. "phe thu:" ...
MARK.append(("CTM_NEWS_CHUNGCUOC", pick(s_head, r'str\s*=\s*"([^"]+)"\s*\.\.\s*"', "tin chung cuoc")))
# Su gia cong thanh (map 53) - chi de ghi chu / doi chieu
MARK.append(("CTM_OPT_LOIDAI", pick(s_infoc, '"' + ANY + '/PreEnterGame"', "dong tham gia loi dai").strip()))
# ten NPC Tuy Quan duoc Y (ha chu ASCII de khop DT_FindNpcName) - dien sau khi doc head.lua
MARK.append(("CTM_NPC_DUOCY", None))

# ---------------- 2. toa do / hang so (head.lua, ctrap*.lua) ----------------
def pos3(text, name):
    m = re.search(re.escape(name) + r"\s*=\s*\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\}", text)
    if not m:
        raise SystemExit("KHONG doc duoc %s" % name)
    return int(m.group(1)), int(m.group(2)), int(m.group(3))


def posxy_list(text, name):
    m = re.search(re.escape(name) + r"\s*=\s*" + NL + r"?\s*\{(.*?)" + NL + r"\};", text, re.S)
    if not m:
        raise SystemExit("KHONG doc duoc bang %s" % name)
    return [(int(x), int(y)) for x, y in re.findall(r"x\s*=\s*(\d+)\s*,\s*y\s*=\s*(\d+)", m.group(1))]


CAMP1 = pos3(s_head, "CS_CampPos1")
CAMP2 = pos3(s_head, "CS_CampPos2")
STONE = posxy_list(s_head, "StonePos")
DOOR = posxy_list(s_head, "DoorPos")
# Tuy Quan duoc Y: DoctorPos = { {x, y, "ten"}, ... } (mps) - NPC Sale(53) trong doanh
m = re.search(r"DoctorPos\s*=\s*" + NL + r"?\s*\{(.*?)" + NL + r"\}", s_head, re.S)
if not m:
    raise SystemExit("KHONG doc duoc DoctorPos")
DUOCY = re.findall(r'\{\s*(\d+)\s*,\s*(\d+)\s*,\s*"([^"]*)"\s*\}', m.group(1))
if not DUOCY:
    raise SystemExit("DoctorPos rong")
DUOCY_TEN = "".join((c.lower() if "A" <= c <= "Z" else c) for c in DUOCY[0][2])  # ha chu ASCII nhu g_StrLower
OUTER = re.findall(r"\{\s*(22[23])\s*,\s*(\d+)\s*,\s*(\d+)\s*\}", s_head[s_head.index("OuterPos"):s_head.index("OuterPos") + 200])
NPC_TRU_THU = num(s_head, "STONENPCID1")
NPC_TRU_CONG = num(s_head, "STONENPCID2")
NPC_CONG = num(s_head, "DOORNPCID")
MAP_TRAN = CAMP1[0]
MAP_HP_THU = int(OUTER[0][0])
MAP_HP_CONG = int(OUTER[1][0])
NGAY_BANG = int(pick(s_trap, r"GetJoinTongTime\(\)\s*>=\s*(\d+)", "GetJoinTongTime")) // 1440
sp1 = re.findall(r"SetPos\((\d+)\s*,\s*(\d+)\)", s_ctrap1)
sp2 = re.findall(r"SetPos\((\d+)\s*,\s*(\d+)\)", s_ctrap2)
RA_THU = (int(sp1[0][0]), int(sp1[0][1]))
RA_CONG = (int(sp2[0][0]), int(sp2[0][1]))

# ---------------- 3. o trap tu du lieu map ----------------
def name2id(s):
    uid = 0
    idx = 0
    for ch in s:
        c = ord(ch)
        if 65 <= c <= 90:
            c += 32
        elif c > 127:
            c -= 256
        idx += 1
        uid = (((uid + idx * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF
        uid &= 0xFFFFFFFF
    return uid ^ 0x12345678


def filename2id(s):
    uid = 0
    for i, ch in enumerate(s):
        c = ord(ch)
        if c > 127:
            c -= 256
        uid = (((uid + (i + 1) * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF
        uid &= 0xFFFFFFFF
    return uid ^ 0x12345678


def nrv2b(src, dstlen):
    bb = 0
    ilen = 0
    dst = bytearray()
    last = 1

    def getbit():
        nonlocal bb, ilen
        if bb & 0x7F:
            bb = (bb * 2) & 0xFFFF
        else:
            bb = src[ilen] * 2 + 1
            ilen += 1
        return (bb >> 8) & 1

    while True:
        while getbit():
            dst.append(src[ilen])
            ilen += 1
        m_off = 1
        while True:
            m_off = m_off * 2 + getbit()
            if getbit():
                break
        if m_off == 2:
            m_off = last
        else:
            m_off = (m_off - 3) * 256 + src[ilen]
            ilen += 1
            if m_off == 0xFFFFFFFF:
                break
            m_off += 1
            last = m_off
        m_len = getbit()
        m_len = m_len * 2 + getbit()
        if m_len == 0:
            m_len += 1
            while True:
                m_len = m_len * 2 + getbit()
                if getbit():
                    break
            m_len += 2
        if m_off > 0xD00:
            m_len += 1
        for _ in range(m_len + 1):
            dst.append(dst[len(dst) - m_off])
        if len(dst) >= dstlen:
            break
    return bytes(dst[:dstlen])


class Pak:
    def __init__(self, p):
        self.f = open(p, "rb")
        sig, count, ioff, doff = struct.unpack("<4sIII", self.f.read(16))
        if sig != b"PACK":
            raise SystemExit("khong phai pak: %s" % p)
        self.f.seek(ioff)
        raw = self.f.read(count * 16)
        self.idx = {}
        for i in range(count):
            uid, off, size, cf = struct.unpack("<IIiI", raw[i * 16:i * 16 + 16])
            self.idx[uid] = (off, size, cf)

    def read(self, name):
        uid = name2id(name)
        if uid not in self.idx:
            return None
        off, size, cf = self.idx[uid]
        csize = cf & 0x00FFFFFF
        method = cf & 0xFF000000
        self.f.seek(off)
        if method == 0 or csize == 0:
            return self.f.read(size)
        return nrv2b(self.f.read(csize), size)


def parse_region(d, col, row, traps, npcs):
    cnt = struct.unpack_from("<I", d, 0)[0]
    secs = [struct.unpack_from("<II", d, 4 + 8 * i) for i in range(cnt)]
    head = 4 + 8 * cnt
    off, ln = secs[1]
    if ln >= 12:
        n = struct.unpack_from("<I", d, head + off)[0]
        for i in range(n):
            cx, cy, nc, res, tid = struct.unpack_from("<BBBBI", d, head + off + 12 + 8 * i)
            for j in range(nc):
                traps.setdefault(tid, []).append((col * 16 + cx + j, row * 32 + cy))
    off, ln = secs[2]
    if ln >= 12:
        n = struct.unpack_from("<I", d, head + off)[0]
        q = head + off + 12
        for i in range(n):
            tpl, px, py = struct.unpack_from("<iii", d, q)
            name = d[q + 16:q + 48].split(b"\0")[0].decode("latin-1")
            camp, series, slen = struct.unpack_from("<BBH", d, q + 56)
            scr = d[q + 60:q + 60 + slen].split(b"\0")[0].decode("latin-1")
            npcs.append((tpl, px // 32, py // 32, name, scr.lower()))
            q += 60 + slen


def rect_of(wor_text):
    for ln in wor_text.splitlines():
        if ln.strip().lower().startswith("rect="):
            return [int(x) for x in ln.strip()[5:].split(",")]
    raise SystemExit("wor khong co rect=")


def dump_dir(mapdir):
    rect = rect_of(rd(mapdir + ".wor"))
    traps, npcs = {}, []
    for row in range(rect[1], rect[3] + 1):
        vd = os.path.join(mapdir, "v_%03d" % row)
        if not os.path.isdir(vd):
            continue
        names = {n.lower(): n for n in os.listdir(vd)}
        for col in range(rect[0], rect[2] + 1):
            n = names.get("%03d_region_s.dat" % col)
            if not n:
                continue
            parse_region(open(os.path.join(vd, n), "rb").read(), col, row, traps, npcs)
    return traps, npcs


def dump_pak(pak, mapname):
    base = BS + "maps" + BS + mapname
    wor = pak.read(base + ".wor")
    if wor is None:
        return None
    rect = rect_of(wor.decode("latin-1"))
    traps, npcs = {}, []
    for row in range(rect[1], rect[3] + 1):
        for col in range(rect[0], rect[2] + 1):
            d = pak.read(base + BS + ("v_%03d" % row) + BS + ("%03d_Region_S.dat" % col))
            if d is None:
                continue
            parse_region(d, col, row, traps, npcs)
    return traps, npcs


def hash_scripts():
    H = {}
    for dp, dn, fn in os.walk(SRV):
        for f in fn:
            if not f.lower().endswith(".lua"):
                continue
            rel = BS + os.path.relpath(os.path.join(dp, f), SRV_BIN).replace("/", BS)
            for v in (rel.lower(), rel):
                H[filename2id(v)] = rel.lower()
    return H


def maplist_dir(mapid):
    s = rd(os.path.join(SRV_BIN, "settings", "MapList.ini"))
    for ln in s.splitlines():
        if ln.startswith("%d=" % mapid):
            return ln.split("=", 1)[1].strip()
    raise SystemExit("MapList.ini khong co map %d" % mapid)


def by_script(traps, H):
    out = {}
    for tid, cells in traps.items():
        scr = H.get(tid)
        if not scr:
            continue
        out[os.path.basename(scr)] = sorted(set(cells))
    return out


cache = {}
if os.path.exists(CACHE):
    with io.open(CACHE, "r", encoding="utf-8") as f:
        cache = json.load(f)
H = hash_scripts()

# 221: thu muc map tren dia
d221 = os.path.join(SRV_BIN, "maps", maplist_dir(MAP_TRAN))
if os.path.isdir(d221):
    t221, n221 = dump_dir(d221)
    T221 = by_script(t221, H)
    cache["221"] = T221
    print("221: doc tu dia %s -> %s" % (d221.encode("latin-1").decode("gbk", "replace"), sorted(T221.keys())))
elif "221" in cache:
    T221 = cache["221"]
    print("221: dung cache")
else:
    raise SystemExit("KHONG co du lieu map 221 (thu muc %s) va khong co cache" % d221)

# 222/223: thu muc map cung ten -> pak Linux
hp_dir = maplist_dir(MAP_HP_THU)
d_hp = os.path.join(SRV_BIN, "maps", hp_dir)
T_HP = None
NPC_HP = None
if os.path.isdir(d_hp):
    thp, nhp = dump_dir(d_hp)
    T_HP = by_script(thp, H)
    NPC_HP = nhp
    print("222/223: doc tu dia")
elif os.path.exists(PAK_LINUX):
    r = dump_pak(Pak(PAK_LINUX), hp_dir)
    if r:
        T_HP = by_script(r[0], H)
        NPC_HP = r[1]
        print("222/223: doc tu pak Linux %s" % PAK_LINUX)
if T_HP is not None:
    cache["hp"] = T_HP
    cache["hp_npc"] = [list(x) for x in NPC_HP]
elif "hp" in cache:
    T_HP = cache["hp"]
    NPC_HP = [tuple(x) for x in cache["hp_npc"]]
    print("222/223: dung cache")
else:
    raise SystemExit("KHONG co du lieu map 222/223 (thu muc %s, pak %s) va khong co cache" % (d_hp, PAK_LINUX))
with io.open(CACHE, "w", encoding="utf-8") as f:
    json.dump(cache, f, indent=1, sort_keys=True)


def cells(name, *scripts):
    out = []
    for s in scripts:
        if s not in T_HP and s not in T221:
            raise SystemExit("KHONG thay trap %s trong du lieu map" % s)
        out += [tuple(c) for c in (T221.get(s) or T_HP.get(s))]
    return out


TRAP_HP = [tuple(c) for c in T_HP["trap.lua"]]
TRAP_THU = cells("thu", "ctrap1.lua", "ctrap1b.lua", "ctrap1c.lua")
TRAP_CONG = cells("cong", "ctrap2.lua", "ctrap2b.lua", "ctrap2c.lua")
TRAP_RA_THU = cells("ra thu", "trap1.lua")
TRAP_RA_CONG = cells("ra cong", "trap2.lua")
XAPHU_HP = [(n[1], n[2]) for n in NPC_HP if n[4].endswith("chefu.lua")]
if not XAPHU_HP:
    raise SystemExit("KHONG thay NPC chefu.lua tren map hau phuong")

# ---------------- 4. ghi .h ----------------
out = []
w = out.append
w("// KCongThanhTables.h - SINH TU DONG boi ReverseTools/gen_congthanh_tables.py - DUNG SUA TAY")
w("// Nguon: script song cua may chu (%s) + Region_S.dat map 221/222/223" % SRV.replace(BS, "/"))
w("// Marker la RAW TCVN3 trich thang tu Lua (byte-for-byte) - giong KTongKimTables.h.")
w("// O trap doc tu du lieu vung (client KHONG nap trap - KRegion::GetTrap tra 0 ngoai _SERVER).")
w("#ifndef KCONGTHANHTABLES_H")
w("#define KCONGTHANHTABLES_H")
w("")
w("// ===== map / NPC / hang so (head.lua, trap.lua, MapList.ini) =====")
w("#define CT_MAP_TRAN%s%d%s// chien truong cong thanh" % (TAB, MAP_TRAN, TAB))
w("#define CT_MAP_HP_THU%s%d%s// hau phuong ben THU" % (TAB, MAP_HP_THU, TAB))
w("#define CT_MAP_HP_CONG%s%d%s// hau phuong ben CONG" % (TAB, MAP_HP_CONG, TAB))
w("#define CT_NPC_CONG%s%d%s// NPC cong thanh (DOORNPCID)" % (TAB, NPC_CONG, TAB))
w("#define CT_NPC_TRU_THU%s%d%s// Long tru ben thu dang giu (STONENPCID1)" % (TAB, NPC_TRU_THU, TAB))
w("#define CT_NPC_TRU_CONG%s%d%s// Long tru ben cong da chiem (STONENPCID2)" % (TAB, NPC_TRU_CONG, TAB))
w("#define CT_SO_CONG%s%d" % (TAB, len(DOOR)))
w("#define CT_SO_TRU%s%d" % (TAB, len(STONE)))
w("#define CT_NGAY_BANG%s%d%s// phai o trong bang du N ngay moi duoc tham chien (GetJoinTongTime)" % (TAB, NGAY_BANG, TAB))
w("")
w("// ===== marker hoi thoai / tin (RAW TCVN3) =====")
for name, s in MARK:
    if name == "CTM_NPC_DUOCY":
        s = DUOCY_TEN
    w("static const char %s[] = %s;" % (name, cstr(s)))
w("")
w("// ===== toa do (don vi O; MPS = O * 32) =====")
w("struct CTPoint { short x, y; };")
w("static const CTPoint g_CTDoanhThu  = { %d, %d };%s// CS_CampPos1 - doanh ben thu (map %d)" % (CAMP1[1], CAMP1[2], TAB, CAMP1[0]))
w("static const CTPoint g_CTDoanhCong = { %d, %d };%s// CS_CampPos2 - doanh ben cong" % (CAMP2[1], CAMP2[2], TAB))
w("static const CTPoint g_CTRaThu  = { %d, %d };%s// ctrap1.lua SetPos - diem ra tran ben thu" % (RA_THU[0], RA_THU[1], TAB))
w("static const CTPoint g_CTRaCong = { %d, %d };%s// ctrap2.lua SetPos - diem ra tran ben cong" % (RA_CONG[0], RA_CONG[1], TAB))
w("static const CTPoint g_CTNgoai = { %d, %d };%s// OuterPos - diem dap tren hau phuong sau tran" % (int(OUTER[0][1]), int(OUTER[0][2]), TAB))
w("// 3 cong thanh (DoorPos, tu MPS) - NPC %d camp 1, chi ben cong danh duoc" % NPC_CONG)
w("static const CTPoint g_CTCong[CT_SO_CONG] = {")
for x, y in DOOR:
    w("%s{ %d, %d }," % (TAB, x // 32, y // 32))
w("};")
w("// 3 Long tru (StonePos, tu MPS) - NPC %d (thu giu) / %d (cong chiem)" % (NPC_TRU_THU, NPC_TRU_CONG))
w("static const CTPoint g_CTTru[CT_SO_TRU] = {")
for x, y in STONE:
    w("%s{ %d, %d }," % (TAB, x // 32, y // 32))
w("};")
w("// Tuy Quan duoc Y trong doanh map 221 (DoctorPos, tu MPS) - NPC Sale(53): shop thuoc thuong")
w("#define CT_DUOCY_COUNT%s%d" % (TAB, len(DUOCY)))
w("static const CTPoint g_CTDuocY[CT_DUOCY_COUNT] = {")
for x, y, ten in DUOCY:
    w("%s{ %d, %d }," % (TAB, int(x) // 32, int(y) // 32))
w("};")
w("// Xa Phu tren hau phuong 222/223 (chefu.lua, tu Npc_S.dat)")
w("#define CT_XAPHU_HP_COUNT%s%d" % (TAB, len(XAPHU_HP)))
w("static const CTPoint g_CTXaPhuHP[CT_XAPHU_HP_COUNT] = {")
for x, y in XAPHU_HP:
    w("%s{ %d, %d }," % (TAB, x, y))
w("};")


def wtab(name, cnt_name, pts, note):
    w("// %s" % note)
    w("#define %s%s%d" % (cnt_name, TAB, len(pts)))
    w("static const CTPoint %s[%s] = {" % (name, cnt_name))
    for x, y in sorted(pts):
        w("%s{ %d, %d }," % (TAB, x, y))
    w("};")


wtab("g_CTTrapHP", "CT_TRAPHP_COUNT", TRAP_HP, "o trap TAP KET tren hau phuong 222/223 (zhongzhuan_map/trap.lua -> JoinCamp -> map 221)")
wtab("g_CTTrapThu", "CT_TRAPTHU_COUNT", TRAP_THU, "o trap cua doanh ben THU tren map 221 (ctrap1/1b/1c.lua -> ra tran)")
wtab("g_CTTrapCong", "CT_TRAPCONG_COUNT", TRAP_CONG, "o trap cua doanh ben CONG tren map 221 (ctrap2/2b/2c.lua -> ra tran)")
wtab("g_CTTrapRaThu", "CT_TRAPRATHU_COUNT", TRAP_RA_THU, "o trap RA KHOI tran ben thu (trap1.lua -> ve 222) - tranh dam vao")
wtab("g_CTTrapRaCong", "CT_TRAPRACONG_COUNT", TRAP_RA_CONG, "o trap RA KHOI tran ben cong (trap2.lua -> ve 223) - tranh dam vao")
w("")
w("#endif // KCONGTHANHTABLES_H")
w("")

with io.open(OUT, "w", encoding="latin-1", newline="") as f:
    f.write(NL.join(out))
print("ghi %s: %d marker, trap HP %d / thu %d / cong %d / ra %d+%d, Xa Phu HP %d" % (
    os.path.normpath(OUT), len(MARK), len(TRAP_HP), len(TRAP_THU), len(TRAP_CONG), len(TRAP_RA_THU), len(TRAP_RA_CONG), len(XAPHU_HP)))
