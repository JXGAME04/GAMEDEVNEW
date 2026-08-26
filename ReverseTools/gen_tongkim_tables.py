# -*- coding: latin-1 -*-
# gen_tongkim_tables.py - sinh Sources/Core/Src/KTongKimTables.h cho AUTO TONG KIM (WAuto).
#
# Moi chuoi marker duoc TRICH THANG tu script Lua song cua may chu (byte-for-byte,
# doc/ghi latin-1) nen chuoi trong .h la RAW TCVN3 - dung y het cach
# gen_datau_tables.py lam cho KDaTauTables.h. KHONG go tay tieng Viet vao .h.
#
# Chay:  python ReverseTools/gen_tongkim_tables.py [duong-dan-thu-muc-script-server]
import io
import os
import re
import sys

SRV = sys.argv[1] if len(sys.argv) > 1 else \
    r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                   "..", "Sources", "Core", "Src", "KTongKimTables.h")

TK = os.path.join(SRV, "tinhnang", "tong_kim_tcap")
TRAP = os.path.join(SRV, "maps", "tongkim", "trap")

NL = chr(10)
BS = chr(92)


def rd(path):
    with io.open(path, "r", encoding="latin-1", newline="") as f:
        return f.read()


def cstr(s):
    """Chuoi C giu nguyen byte (latin-1); chi thoat nhung ky tu bat buoc."""
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


def toado(text, name):
    """Doc bang toa do Lua dang NAME = { {x,y}, ... } -> [(x,y), ...]"""
    m = re.search(re.escape(name) + r"\s*=\s*\{(.*?)" + NL + r"\}", text, re.S)
    if not m:
        raise SystemExit("KHONG doc duoc bang %s" % name)
    return [(int(a), int(b)) for a, b in re.findall(r"\{\s*(\d+)\s*,\s*(\d+)\s*\}", m.group(1))]


def pick(text, pat, what):
    m = re.search(pat, text)
    if not m:
        raise SystemExit("KHONG trich duoc marker: %s" % what)
    return m.group(1)


def num(text, name):
    m = re.search(re.escape(name) + r"\s*=\s*(\d+)", text)
    if not m:
        raise SystemExit("KHONG doc duoc hang so %s" % name)
    return int(m.group(1))


s_mobinh = rd(os.path.join(TK, "mobinhtk.lua"))
s_xaphu = rd(os.path.join(TK, "xaphu.lua"))
s_quany = rd(os.path.join(TK, "quany.lua"))
s_quanquan = rd(os.path.join(TK, "quanquan.lua"))
s_lib = rd(os.path.join(TK, "lib_tktc.lua"))
s_timer = rd(os.path.join(SRV, "timerserver.lua"))
s_thp = rd(os.path.join(SRV, "item", "ib", "shenxingfu.lua"))	# Than Hanh Phu 6/1/1271
s_tongratrai = rd(os.path.join(TRAP, "tongratrai.lua"))
s_kimratrai = rd(os.path.join(TRAP, "kimratrai.lua"))

# ---- marker hoi thoai (neo bang chuoi ASCII, lay nguyen byte tieng Viet) ----
ANY = '([^"' + NL + ']*)'      # chuoi trong cap nhay kep, khong vuot dong
ANYL = '([^"' + NL + ']*?)'

MARK = []
# NPC bao danh: {"Ta muon dau quan cho phe Tong/go_tong","...phe kim/go_kim"}
MARK.append(("TKM_OPT_TONG", pick(s_mobinh, '"' + ANY + '/go_tong"', "dong dau quan phe Tong")))
MARK.append(("TKM_OPT_KIM", pick(s_mobinh, '"' + ANY + '/go_kim"', "dong dau quan phe Kim")))
# cau Say cua NPC bao danh co quan so 2 phe -> nhan de doc so
MARK.append(("TKM_SAY_TONG", pick(s_mobinh, '<color=fire>' + ANYL + r'"\.\.nPTong', "nhan quan so Tong")))
MARK.append(("TKM_SAY_KIM", pick(s_mobinh, '<color=pink>' + ANYL + r'"\.\.nPKim', "nhan quan so Kim")))
# Xa Phu tren map bao danh
MARK.append(("TKM_OPT_TROLAI", pick(s_xaphu, '"' + ANY + '/TownPortalFun"', "dong tro lai cho luc nay")))
MARK.append(("TKM_OPT_THANHTHI", pick(s_xaphu, '"' + ANY + '/StationFun"', "dong thanh thi da di qua")))
MARK.append(("TKM_OPT_QUATONG", pick(s_xaphu, '"' + ANY + r'/#battle_transprot\(1', "dong qua phe Tong")))
MARK.append(("TKM_OPT_QUAKIM", pick(s_xaphu, '"' + ANY + r'/#battle_transprot\(2', "dong qua phe Kim")))
# Quan Y (mua mau) - map tran
MARK.append(("TKM_OPT_MUANHANH", pick(s_quany, '"' + ANY + '/muamaunhanh"', "dong mua nhanh")))
MARK.append(("TKM_OPT_MUADUOC", pick(s_quany, '"' + ANY + '/muaquanluong"', "dong mua duoc pham")))
# Quan Nhu Quan (shop diem Tong Kim) - map bao danh
MARK.append(("TKM_OPT_CHDIEM", pick(s_quanquan, '"' + ANY + '/giaodichtongkim"', "dong cua hang diem")))
# tin toan may chu bao mo bao danh
MARK.append(("TKM_MSG_BAODANH",
             pick(s_timer, r'AddGlobalCountNews\("' + ANY + r'"\.\.TIME_BD_TK', "tin mo bao danh")))
MARK.append(("TKM_MSG_KHOIDONG",
             pick(s_timer, r'AddGlobalCountNewsEx\("' + ANY + r'"\.\.GetMissionName', "tin khoi dong tran")))
MARK.append(("TKM_MSG_SUKIENBD",
             pick(s_timer, r'(\xaeang \xeb giai \xaeo\xb9n b\xb8o danh)', "tin chat su kien bao danh")))

# Than Hanh Phu (6/1/1271): duong VAO TONG KIM KHONG CAN Chieu Thu.
# Chu game 25/08: "khong can item do than hanh phu co len map tong kim".
# Menu 2 cap: chondiadiem1() -> "Chien truong Tong Kim/tongkim"
#             tongkim()      -> "Bao danh [phe Tong]/ditongtc" | "[phe Kim ]/dikimtc"
# (menu 3 CAP: dung phu -> chondiadiem1 -> tongkim -> chon phe)
MARK.append(("TKM_OPT_THP_DI", pick(s_thp, '"' + ANY + '/chondiadiem1"', "dong dung thuat than hanh cua Than Hanh Phu")))
MARK.append(("TKM_OPT_THP_TK", pick(s_thp, '"' + ANY + '/tongkim"', "dong chien truong Tong Kim cua Than Hanh Phu")))
MARK.append(("TKM_OPT_THP_TONG", pick(s_thp, '"' + ANY + '/ditongtc"', "dong bao danh phe Tong cua Than Hanh Phu")))
MARK.append(("TKM_OPT_THP_KIM", pick(s_thp, '"' + ANY + '/dikimtc"', "dong bao danh phe Kim cua Than Hanh Phu")))

# ---- toa do ----
TONGBINH = toado(s_lib, "TONGBINH_TOADO")
KIMBINH = toado(s_lib, "KIMBINH_TOADO")
TONGRA = toado(s_tongratrai, "RANDOM_POS")
KIMRA = toado(s_kimratrai, "RANDOM_POS_KIM")

MAP_BD = num(s_lib, "MAP_BD_TC")
MAP_TK = num(s_lib, "MAP_TK_TC")
LEVEL_TK = num(s_lib, "LEVEL_ENOUGH_TK")
TIME_IN_TRAI = num(s_lib, "TIME_IN_TRAI")
TIME_DELAY_RA = num(s_lib, "TIME_DELAY_RA_TRAI")

# gio mo tran: TAB_TIME_TONG_KIM = { {13,23,...}, ... }
m = re.search(r"TAB_TIME_TONG_KIM\s*=\s*\{(.*?)" + NL + r"\}", s_lib, re.S)
GIO = []
for ln in m.group(1).splitlines():
    ln = ln.strip()
    if ln.startswith("--") or not ln.startswith("{"):
        continue
    g = re.match(r"\{\s*(\d+)\s*,\s*(\d+)", ln)
    if g:
        GIO.append((int(g.group(1)), int(g.group(2))))

TAB = chr(9)
out = []
w = out.append
w("// KTongKimTables.h - SINH TU DONG boi ReverseTools/gen_tongkim_tables.py - DUNG SUA TAY")
w("// Nguon: script song cua may chu (%s)" % SRV.replace(BS, "/"))
w("// Marker la RAW TCVN3 trich thang tu Lua (byte-for-byte) - giong KDaTauTables.h.")
w("#ifndef KTONGKIMTABLES_H")
w("#define KTONGKIMTABLES_H")
w("")
w("// ===== hang so tran (lib_tktc.lua) =====")
w("#define TK_MAP_BAODANH%s%d%s// map diem bao danh" % (TAB, MAP_BD, TAB))
w("#define TK_MAP_TRAN%s%s%d%s// map chien truong" % (TAB, TAB, MAP_TK, TAB))
w("#define TK_LEVEL_MIN%s%d%s// cap toi thieu de bao danh" % (TAB, LEVEL_TK, TAB))
w("#define TK_TIME_TRAI%s%d%s// giay toi da o hau doanh (dong ho tu nem ra tran)" % (TAB, TIME_IN_TRAI, TAB))
w("#define TK_TIME_TRAP%s%d%s// giay dau trap tu choi cho qua" % (TAB, TIME_DELAY_RA, TAB))
w("")
w("// ===== marker hoi thoai =====")
for name, s in MARK:
    w("static const char %s[] = %s;" % (name, cstr(s)))
w("")
w("// ===== khung gio mo tran (gio, phut) - theo dong ho MAY CHU =====")
w("#define TK_GIO_COUNT%s%d" % (TAB, len(GIO)))
w("static const short g_TKGio[TK_GIO_COUNT][2] = {")
for h, mi in GIO:
    w("%s{ %d, %d }," % (TAB, h, mi))
w("};")
w("")
w("// ===== toa do (don vi O; MPS = O * 32) =====")
w("struct TKPoint { short x, y; };")
w("")
w("// diem dap khi dung Tong Kim Chieu Thu (item/battles/rescript.lua)")
w("static const TKPoint g_TKDapTong = { 1529, 3196 };")
w("static const TKPoint g_TKDapKim  = { 1592, 3075 };")
w("// NPC tren map bao danh (startgame.lua:84-96)")
w("static const TKPoint g_TKNpcBdTong = { 1550, 3179 };%s// Tong Binh Bao Danh (res 62)" % TAB)
w("static const TKPoint g_TKNpcBdKim  = { 1555, 3082 };%s// Kim Binh Bao Danh (res 61)" % TAB)
w("static const TKPoint g_TKXaFuTong  = { 1535, 3153 };%s// Xa Phu phe Tong (res 235)" % TAB)
w("static const TKPoint g_TKXaFuKim   = { 1568, 3075 };%s// Xa Phu phe Kim (res 235)" % TAB)
w("static const TKPoint g_TKShopTong  = { 1546, 3158 };%s// Quan Nhu Quan Tong (res 55)" % TAB)
w("static const TKPoint g_TKShopKim   = { 1580, 3074 };%s// Quan Nhu Quan Kim (res 49)" % TAB)
w("// DIEM VONG khu Kim: giua cum diem-dap/Xa Phu va hoc NPC bao danh co MANG TUONG")
w("// (do tren maps/324_srv.fp bang ReverseTools/tk_pick_vongkim.py: duong thang")
w("// DapKim->NpcBdKim cham 5 o chan, NpcBdKim->XaPhuKim cung cham 5 o). San trong")
w("// phia nam, nhin sach toi ca DapKim / NpcBdKim / XaPhuKim / SwapKim, 5x5 thoang.")
w("static const TKPoint g_TKVongKim   = { 1566, 3084 };")
w("// hau doanh 2 phe tren map tran (TAB_PHE_TONGKIM[..][9]) - DAO theo the tran")
w("static const TKPoint g_TKHauDoanhA = { 1229, 3561 };")
w("static const TKPoint g_TKHauDoanhB = { 1689, 3074 };")
w("// Quan Y ban thuoc canh hau doanh (TKPOS_NPC_DIALOG, doi tu MPS ve O)")
w("static const TKPoint g_TKQuanYA = { 1249, 3557 };")
w("static const TKPoint g_TKQuanYB = { 1700, 3066 };")
w("// vet trap RA TRAI (addtraptongkimtrungcap) - goc + 10 o cheo")
w("static const TKPoint g_TKTrapA = { 1251, 3529 };")
w("static const TKPoint g_TKTrapB = { 1661, 3098 };")
w("#define TK_TRAP_LEN%s10%s// vet cheo dai 10 o (tam vet = goc + 5)" % (TAB, TAB))
w("")
w("// 8 diem xuat quan cua moi ben (trap/tongratrai.lua, trap/kimratrai.lua)")
w("#define TK_XQ_COUNT%s%d" % (TAB, len(TONGRA)))
w("static const TKPoint g_TKXuatQuanA[TK_XQ_COUNT] = {")
for x, y in TONGRA:
    w("%s{ %d, %d }," % (TAB, x, y))
w("};")
w("static const TKPoint g_TKXuatQuanB[TK_XQ_COUNT] = {")
for x, y in KIMRA:
    w("%s{ %d, %d }," % (TAB, x, y))
w("};")
w("")
w("// bang toa do binh doan (lib_tktc.lua) - dich di chuyen khi khong co dich quanh minh")
w("#define TK_BINHA_COUNT%s%d" % (TAB, len(TONGBINH)))
w("static const TKPoint g_TKBinhA[TK_BINHA_COUNT] = {")
for x, y in TONGBINH:
    w("%s{ %d, %d }," % (TAB, x, y))
w("};")
w("#define TK_BINHB_COUNT%s%d" % (TAB, len(KIMBINH)))
w("static const TKPoint g_TKBinhB[TK_BINHB_COUNT] = {")
for x, y in KIMBINH:
    w("%s{ %d, %d }," % (TAB, x, y))
w("};")
w("")
w("// ===== vat pham =====")
w("// Tong Kim Chieu Thu (magicscript.txt) - vao map bao danh, TRU 1 cai moi lan dung")
w("#define TK_ITEM_THU_G%s6" % TAB)
w("#define TK_ITEM_THU_D%s1" % TAB)
w("#define TK_ITEM_THU_P%s154" % TAB)
w("// binh thuoc NPC Quan Y ban bang dong 'Mua nhanh' (quany.lua: AddItem(1,2,0,5,...))")
w("#define TK_ITEM_MAU_G%s1" % TAB)
w("#define TK_ITEM_MAU_D%s2" % TAB)
w("#define TK_ITEM_MAU_P%s0" % TAB)
w("#define TK_ITEM_MAU_L%s5" % TAB)
# ---- g_TKPill: DOC THANG tu bang vat pham, khong go tay ----
# Moi vat pham dung trong tran Tong Kim deu co script trong \\script\\item\\battles\\.
# Phan loai theo dung loi cua tung script (da doc): 1 = tang cong, 2 = phong thu,
# 3 = mau / toc do. Cac mon KHONG phai thuoc thi loai tru han.
PILL_NHOM = {
	# --- 1: tang cong ---
	"pneuma_jin": 1, "pneuma_mu": 1, "pneuma_shui": 1, "pneuma_huo": 1, "pneuma_tu": 1,
	"weapon_jin": 1, "weapon_mu": 1, "weapon_shui": 1, "weapon_huo": 1, "weapon_tu": 1,
	"sj_waipuwan": 1, "sj_waiduwan": 1, "sj_waibingwan": 1,
	"sj_neipuwan": 1, "sj_neiduwan": 1, "sj_neibingwan": 1, "sj_neihuowan": 1,
	"sj_neidianwan": 1,
	# --- 2: phong thu ---
	"armor_jin": 2, "armor_mu": 2, "armor_shui": 2, "armor_huo": 2, "armor_tu": 2,
	"wardrum": 2,          # Chien co - "tang 30% khang va mau"
	"sj_gaoshanwan": 2, "sj_gaozhongwan": 2,
	"sj_pufangwan": 2, "sj_bingfangwan": 2, "sj_leifangwan": 2,
	"sj_huofangwan": 2, "sj_dufangwan": 2,
	# --- 3: mau / toc do ---
	"token": 3,            # Lenh bai - "tang 50% toc do di chuyen"
	"sj_xingjundan": 3, "sj_xiaohuandan": 3, "sj_wuhualu": 3,
	"sj_changmingwan": 3, "sj_jiapaowan": 3, "sj_feisuwan": 3,
	"sj_zaohuadan_b": 3, "sj_zaohuadan_m": 3, "sj_zaohuadan_s": 3,
}
# KHONG phai thuoc - tu dung la mat do / lam bay
PILL_BO = {
	"rescript",      # 154 Tong Kim Chieu thu = VE VAO TRAN
	"clarion",       # 157 Binh Si hieu phu = trieu hoi NPC
	"useflagitem",   # 158 Co hieu = cam co
	"dove",          # 211 Bo cau = chim dua tin
}
_mgs = os.path.join(os.path.dirname(SRV.rstrip(chr(92)+chr(47))), "settings", "item", "magicscript.txt")
PILL = []
_la = []
for _l in io.open(_mgs, "rb").read().split(b"\n"):
	_f = _l.split(b"\t")
	if len(_f) <= 9 or _f[1].strip() != b"6" or _f[2].strip() != b"1":
		continue
	_sc = _f[9].decode("latin-1")
	if "battles" not in _sc.lower():
		continue
	_ten = os.path.basename(_sc.replace("\\", "/")).lower()
	if _ten.endswith(".lua"):
		_ten = _ten[:-4]
	if _ten in PILL_BO:
		continue
	if _ten not in PILL_NHOM:
		_la.append((int(_f[3]), _ten))
		continue
	PILL.append((int(_f[3]), PILL_NHOM[_ten]))
PILL.sort()
assert PILL, "khong doc duoc mon nao trong battles/ - bang vat pham doi dang, DUNG LAI"
if _la:
	print("  [!] script battles/ CHUA PHAN LOAI (bo qua): %s" % _la)
print("  g_TKPill: %d mon (truoc day go tay 18 mon 177..194)" % len(PILL))
w("// thuoc hoat dong Tong Kim: genre 6 / detail 1, moi vien vai phut, chi dung duoc")
w("// tren map tran. DANH SACH SINH TU settings/item/magicscript.txt - lay MOI mon co")
w("// script nam trong \\script\\item\\battles\\ (tru ve vao tran / co hieu / bo cau /")
w("// binh si hieu phu - khong phai thuoc).")
w("// cot 2: 1 = tang cong, 2 = phong thu, 3 = mau / toc do (loc theo o cau hinh)")
w("#define TK_PILL_COUNT%s%d" % (TAB, len(PILL)))
w("static const short g_TKPill[TK_PILL_COUNT][2] = {")
for p, k in PILL:
	w("%s{ %d, %d }," % (TAB, p, k))
w("};")
w("")
w("// res id NPC quan quan tren chien truong (lib_tktc.lua addnpcquaitktrungcap):")
w("// Hieu Uy 632/638, Pho Tuong 634/640, Dai Tuong 635/641.")
w("// CHU Y: server dang hardcode the thuc Cuu Sat nen hien KHONG spawn NPC nao.")
w("#define TK_QUAN_COUNT%s6" % TAB)
w("static const short g_TKQuanRes[TK_QUAN_COUNT] = { 632, 638, 634, 640, 635, 641 };")
w("")
w("#endif // KTONGKIMTABLES_H")

with io.open(OUT, "w", encoding="latin-1", newline=chr(13) + NL) as f:
    f.write(NL.join(out) + NL)
print("da ghi %s (%d dong, %d marker, binhA=%d binhB=%d gio=%d)"
      % (os.path.normpath(OUT), len(out), len(MARK), len(TONGBINH), len(KIMBINH), len(GIO)))
