# -*- coding: utf-8 -*-
"""C29 - YEU CAU CHU: them phan chinh THUONG cua boss sat thu vao cauhinh_hoatdong.lua.
Thuong khi giet 1 boss nam trong add_shashouling (lib_killlevel.lua): AddOwnExp(...)
+ 1 sat thu lenh cung cap. Them 3 khoa:
    HD3_ST_HESO_EXP  - he so % exp moi lan giet (100 = giu nguyen ban Linux)
    HD3_ST_SO_LENH   - so sat thu lenh moi lan giet (1 = ban Linux)
    HD3_ST_EXP       - bang exp theo nhom cap (nil = bang goc Linux)
Kem 1 va loi THAT cua JX1: AddOwnExp lam MAT phan exp du khi len cap
(KPlayer.cpp:2629 dat m_nExp = 0) => doi sang AddSumExp (khuon da chot 24/08 khi
doi chieu 7 item hoat dong voi ban Linux).
"""
import io, os, re, sys, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
T = "\t"


def sync(rel):
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(os.path.join(JX1, rel), dst)


# ---------- 1) cauhinh_hoatdong.lua: 3 khoa moi ----------
rel = r"script\header\cauhinh_hoatdong.lua"
p = os.path.join(JX1, rel)
d = io.open(p, encoding="latin-1", newline="").read()
NL = "\r\n" if "\r\n" in d else "\n"
if "HD3_ST_HESO_EXP" in d:
    print("(1) cauhinh: da co")
else:
    a = "HD3_ST_THUONG = nil,"
    assert d.count(a) == 1
    blk = NL.join([
        "",
        "-- (A) SAT THU - THUONG MOI LAN GIET 1 BOSS (add_shashouling):",
        "--   * He so % kinh nghiem: 100 = giu nguyen ban Linux (15.000 exp o nhom cap",
        "--     20, tang dan den 200.000 o nhom cap 90); 200 = gap doi; 50 = mot nua.",
        "HD3_ST_HESO_EXP = 100,",
        "--   * So Sat Thu lenh moi lan giet (ban Linux = 1). Lenh cung CAP va cung NGU",
        "--     HANH voi con boss vua giet; gop 5 cai cung cap thanh Sat Thu Gian.",
        "HD3_ST_SO_LENH = 1,",
        "--   * Bang kinh nghiem theo nhom cap - nil = dung so goc ban Linux. Muon doi:",
        "--     HD3_ST_EXP = {[20]=15000, [30]=20000, [40]=30000, [50]=50000,",
        "--                   [60]=60000, [70]=80000, [80]=100000, [90]=200000},",
        "HD3_ST_EXP = nil,",
    ])
    d = d.replace(a, a + blk)
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    sync(rel)
    print("(1) cauhinh: da them 3 khoa thuong sat thu")

# ---------- 2) lib_killlevel.lua: noi cau hinh + AddSumExp ----------
rel2 = r"script\task\tollgate\killer\lib_killlevel.lua"
p2 = os.path.join(JX1, rel2)
d2 = io.open(p2, encoding="latin-1", newline="").read()
NL2 = "\r\n" if "\r\n" in d2 else "\n"
if "HD3_ST_ThuongBoss" in d2:
    print("(2) lib_killlevel: da noi")
else:
    # 2a. Include cauhinh neu chua co
    if "cauhinh_hoatdong" not in d2:
        i = d2.index(NL2)
        d2 = d2[:i + len(NL2)] + 'Include("\\\\script\\\\header\\\\cauhinh_hoatdong.lua")' + T + "-- [3HD C29] noi cauhinh thuong" + NL2 + d2[i + len(NL2):]
    # 2b. ham chung
    ham = NL2.join([
        "",
        "-- [3HD C29] Thuong 1 lan giet boss sat thu - noi cauhinh_hoatdong.lua:",
        "--   HD3_ST_EXP (bang exp theo nhom cap) / HD3_ST_HESO_EXP (%) / HD3_ST_SO_LENH.",
        "-- Dung AddSumExp thay AddOwnExp: AddOwnExp dat m_nExp = 0 khi len cap",
        "-- (KPlayer.cpp:2629) => MAT phan exp du. Day la loi that cua JX1, khong phai",
        "-- khac biet voi ban Linux.",
        "function HD3_ST_ThuongBoss(nCapNhom, nExpGoc, nSeries)",
        T + "local tbExp = HD_CFG(\"HD3_ST_EXP\", nil)",
        T + "local nExp = nExpGoc",
        T + "if (tbExp ~= nil and tbExp[nCapNhom] ~= nil) then",
        T + T + "nExp = tbExp[nCapNhom]",
        T + "end",
        T + "local nHeSo = HD_CFG(\"HD3_ST_HESO_EXP\", 100)",
        T + "if (nHeSo ~= 100) then",
        T + T + "nExp = floor(nExp * nHeSo / 100)",
        T + "end",
        T + "if (nExp > 0) then",
        T + T + "AddSumExp(nExp)",
        T + "end",
        T + "local nSo = HD_CFG(\"HD3_ST_SO_LENH\", 1)",
        T + "if (nSo < 1) then nSo = 1 end",
        T + "for i = 1, nSo do",
        T + T + "AddItem(6, 1, 398, nCapNhom, nSeries, 0, 0)",
        T + "end",
        "end",
        "",
    ])
    a2 = "function add_shashouling(nvalue, series)"
    assert d2.count(a2) == 1
    d2 = d2.replace(a2, ham + a2)
    # 2c. 7 nhom 20..80: AddOwnExp roi AddItem lien nhau
    RE1 = r"\t\tAddOwnExp\((\d+)\)\r?\n\t\tAddItem\(6,1,398,(\d+),series,0, 0\)"
    cap = re.findall(RE1, d2)
    print("    nhom 20-80:", len(cap), "cap")
    assert len(cap) == 7, cap
    d2 = re.sub(RE1, lambda m: "\t\tHD3_ST_ThuongBoss(%s, %s, series)" % (m.group(2), m.group(1)), d2)
    # 2d. nhom 90: co dong AddExp_Skill_Extend chen giua - GIU NGUYEN dong do
    RE2 = (r"\t\tAddOwnExp\((\d+)\)(\r?\n[^\r\n]*)(\r?\n\t\tAddExp_Skill_Extend\([^)]*\);)"
           r"\r?\n\t\tAddItem\(6,1,398,90,series,0, 0\)")
    m90 = re.search(RE2, d2)
    assert m90, "khong khop nhom 90"
    print("    nhom 90: exp goc", m90.group(1))
    d2 = re.sub(RE2, lambda m: "%s%s\r\n\t\tHD3_ST_ThuongBoss(90, %s, series)"
                % (m.group(2), m.group(3), m.group(1)), d2)
    io.open(p2, "w", encoding="latin-1", newline="").write(d2)
    sync(rel2)
    print("(2) lib_killlevel: da noi cauhinh + doi AddOwnExp -> AddSumExp")
