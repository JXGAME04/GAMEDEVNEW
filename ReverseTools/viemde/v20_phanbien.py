# -*- coding: utf-8 -*-
"""VIEM DE - PHAN BIEN: soat lai toan bo truoc khi GAN VAO may chu.

Soat 8 mat, moi mat deu doi chieu voi NGUON THAT chu khong tin tri nho:
  A. Include: moi duong dan Include trong cac tep Viem De co ton tai khong
  B. Ham: moi ham goi ra ngoai co o JX1 (engine + thu vien script) khong
  C. Vat pham: moi ma 6,1,* / 4,*,* trong tep da port co dung TEN nhu ban Linux khong
  D. NPC: moi template NPC dung da khai trong npcs.txt chua
  E. Ban do: moi id ban do dung da khai trong MapList + WorldSet chua
  F. Task / TaskTemp / mission id: co dam vao ai khong
  G. Cu phap + ma hoa moi tep
  H. Tinh nhat quan cua be pho ban (cau hinh vs so ban do khai that)

In ra: [OK] / [CANH BAO] / [LOI]. Chi khi KHONG CON [LOI] moi duoc gan vao.
"""
import io
import os
import re
import sys
import subprocess
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bangtxt import tcvn2uni, uni2tcvn

spec = importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(dec2)

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
LNX = r"D:\ServerLinux\server1"
SYNCHECK = (r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto"
            r"\0064b491-1b33-4139-acb8-6928fda45bcd\scratchpad\luac\syncheck.exe")

# nhung tep thuoc dot nay
THUMUC = [
    SRV + r"\script\missions\yandibaozang",
    SRV + r"\script\missions\basemission",
    SRV + r"\script\tinhnang\viemde",
    SRV + r"\script\item\viemde",
]

LOI, CANHBAO, OK = [], [], []

# Tep CO SAN tu cac dot truoc, KHONG thuoc dot Viem De - loi cua chung khong
# phai loi cua dot nay (nhung van in ra o muc rieng de chu game biet).
NGOAI_PHAM_VI = {"lib.lua"}

# Da soat tan noi, co y CHAP NHAN - khong tinh la loi. Ghi kem ly do.
DA_SOAT = {
    "YDBZ_proceed": "npc.lua:213 - ban Linux GIONG HET (local proceed nhung goi "
                    "YDBZ_proceed). Ham nay KHONG dinh nghia o CA HAI ben; ca 26 muc "
                    "NPC deu de nil o o PROCEED nen type(nil)~='function' => NHANH CHET.",
    "BuildATeam": "readymap/include.lua:357 - thuoc luong bao danh CU, da chung minh "
                  "CHET o ca ban Linux (khong ai goi OpenMission(51)).",
    "ApplyItemDice": "he xuc xac - buoc 7, chua lam",
    "AddDiceItemInfo": "he xuc xac - buoc 7, chua lam",
    "RollItem": "he xuc xac - buoc 7, chua lam",
    "GetItemDicePlayerList": "he xuc xac - buoc 7, chua lam",
    "GetItemDiceRollInfo": "he xuc xac - buoc 7, chua lam",
    "GetItemDiceItemInfo": "he xuc xac - buoc 7, chua lam",
    "GetItemDiceState": "he xuc xac - buoc 7, chua lam",
}
DASOAT_HIT = []
NGOAI = []


def bao(muc, ma, txt):
    (LOI if ma == "LOI" else CANHBAO if ma == "CANH BAO" else OK).append((muc, txt))


def cac_tep():
    ra = []
    for t in THUMUC:
        if not os.path.isdir(t):
            continue
        for dp, dn, fn in os.walk(t):
            for f in sorted(fn):
                if f.endswith(".lua"):
                    ra.append(os.path.join(dp, f))
    return ra


def noi_dung(p):
    return io.open(p, encoding="latin-1", newline="").read()


def bo_chu_thich(s):
    return re.sub(r"--.*$", "", s)


# ---------------------------------------------------------------- A. Include
RE_INC = re.compile(r'Include\s*\(\s*"([^"]+)"')


def soat_include(tep):
    thieu = {}
    for p in tep:
        for i, l in enumerate(noi_dung(p).split("\r\n"), 1):
            if bo_chu_thich(l).find("Include") < 0:
                continue
            for m in RE_INC.finditer(bo_chu_thich(l)):
                d = m.group(1).replace("\\\\", "\\")
                q = SRV + (d if d.startswith("\\") else "\\" + d)
                if not os.path.isfile(q):
                    if os.path.basename(p) in NGOAI_PHAM_VI:
                        NGOAI.append("%s:%d Include %s (tep co san tu dot truoc, khong ai Include no)" % (os.path.basename(p), i, d))
                        continue
                    thieu.setdefault(d, []).append("%s:%d" % (os.path.basename(p), i))
    if thieu:
        for d, cho in sorted(thieu.items()):
            bao("A", "LOI", "Include tro toi tep KHONG CO: %s   (%s)" % (d, ", ".join(cho[:3])))
    else:
        bao("A", "OK", "moi duong dan Include deu ton tai")


# ------------------------------------------------------------------- B. Ham
LUA_CHUAN = set("""and break do else elseif end false for function if in local nil not or repeat
return then true until while print format strsub strlen strfind gsub tinsert tremove getn sort
type tonumber tostring random floor ceil abs mod max min dofile setglobal getglobal next call
rawget rawset assert error strrep strupper strlower date time sqrt strrep unpack pack iif join
foreach foreachi tinsert""".split())
RE_GOI = re.compile(r"\b([A-Za-z_][A-Za-z_0-9]*)\s*\(")
RE_CHUOI = re.compile('"[^"]*"' + "|'[^']*'")


def soat_ham(tep):
    # ham JX1 dang ky o engine
    reg = set()
    core = r"D:\GAMEDEVNEW\Sources\Core\Src"
    for f in os.listdir(core):
        if f.endswith(".cpp"):
            d = io.open(os.path.join(core, f), encoding="latin-1", errors="replace").read()
            reg |= set(re.findall(r'\{\s*"([A-Za-z_][A-Za-z_0-9]*)"', d))
    # ham dinh nghia trong CHINH cac tep nay + toan cay script JX1
    dn = set()
    for dp, dn2, fn in os.walk(SRV + r"\script"):
        for f in fn:
            if not f.endswith(".lua"):
                continue
            try:
                d = io.open(os.path.join(dp, f), encoding="latin-1", newline="").read()
            except Exception:
                continue
            dn |= set(re.findall(r"(?m)^\s*function\s+([A-Za-z_][A-Za-z_0-9.:]*)", d))
    dn = set(x.split(":")[-1].split(".")[-1] for x in dn) | set(dn)
    thieu = {}
    for p in tep:
        d = noi_dung(p)
        # ten dat bang 'local X' hoac 'X =' trong CHINH tep nay thi khong phai ham
        # thieu - vd npc.lua:152 'local id = item[..]' roi goi id(...) khi
        # type(id)=="function". Khong loc thi bao dong loat nhieu bao dong gia.
        cucbo = set(re.findall(r"(?m)\blocal\s+([A-Za-z_][A-Za-z_0-9]*)", d))
        cucbo |= set(re.findall(r"(?m)^\s*([A-Za-z_][A-Za-z_0-9]*)\s*=", d))
        # ten la TRUONG cua bang (tbX.ten / tbX:ten) cung khong phai ham toan cuc
        truong = set(re.findall(r"[.:]([A-Za-z_][A-Za-z_0-9]*)\s*\(", d))
        for i, l in enumerate(d.split("\r\n"), 1):
            l2 = RE_CHUOI.sub('""', bo_chu_thich(l))
            for m in RE_GOI.finditer(l2):
                g = m.group(1)
                if g in LUA_CHUAN or g in reg or g in dn or g in cucbo or g in truong:
                    continue
                thieu.setdefault(g, []).append("%s:%d" % (os.path.basename(p), i))
    conlai = {}
    for g, cho in thieu.items():
        if g in DA_SOAT:
            DASOAT_HIT.append("%-22s %s  (%s)" % (g, DA_SOAT[g], ", ".join(sorted(set(cho))[:2])))
        else:
            conlai[g] = cho
    if conlai:
        for g, cho in sorted(conlai.items(), key=lambda x: -len(x[1])):
            bao("B", "LOI", "ham CHUA CO va CHUA SOAT: %-24s (%s)" % (g, ", ".join(sorted(set(cho))[:3])))
    else:
        bao("B", "OK", "moi ham goi ra deu co, tru %d ham da soat va chap nhan" % len(DASOAT_HIT))


# --------------------------------------------------------------- C. Vat pham
def bang_item():
    ra = {}
    p = SRV + r"\settings\item\magicscript.txt"
    for l in io.open(p, encoding="latin-1", newline="").read().split("\r\n"):
        c = l.split("\t")
        if len(c) < 4:
            continue
        ra[(c[1], c[2], c[3])] = tcvn2uni(c[0])
    p = SRV + r"\settings\item\questkey.txt"
    rows = [l.split("\t") for l in io.open(p, encoding="latin-1", newline="").read().split("\r\n")]
    ix = {n: i for i, n in enumerate(rows[0])}
    for c in rows[1:]:
        if len(c) < 3:
            continue
        ra[(c[ix["Genre"]], c[ix["DetailType"]], "*")] = tcvn2uni(c[ix["Name"]])
    return ra


def bang_item_lnx():
    ra = {}
    for r in open(LNX + r"\settings\item\004\magicscript.txt", "rb").read().split(b"\n")[1:]:
        c = dec2.decline2(r.rstrip(b"\r")).split("\t")
        if len(c) < 4:
            continue
        ra[(c[1], c[2], c[3])] = c[0].strip()
    return ra


RE_TB3 = re.compile(r"\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*[,}]")


def soat_vatpham(tep):
    jx1 = bang_item()
    n_khong = 0
    n_co = 0
    for p in tep:
        for i, l in enumerate(noi_dung(p).split("\r\n"), 1):
            l2 = bo_chu_thich(l)
            for m in RE_TB3.finditer(l2):
                g, d, pp = m.group(1), m.group(2), m.group(3)
                if g != "6" or d not in ("0", "1"):
                    continue
                if (g, d, pp) in jx1:
                    n_co += 1
                else:
                    n_khong += 1
                    bao("C", "LOI", "ma vat pham %s,%s,%s KHONG CO trong bang item JX1  (%s:%d)"
                        % (g, d, pp, os.path.basename(p), i))
    if n_khong == 0:
        bao("C", "OK", "%d cho dung ma vat pham, tat ca deu co trong bang item JX1" % n_co)


# ------------------------------------------------------------------- D. NPC
def soat_npc(tep):
    rows = [l.split("\t") for l in io.open(SRV + r"\settings\npcs.txt",
                                           encoding="latin-1", newline="").read().split("\r\n") if l]
    ids = set()
    for i, c in enumerate(rows):
        if i == 0 or not c:
            continue
        if not tcvn2uni(c[0]).startswith("<<"):
            ids.add(i - 1)          # id = so dong du lieu - 1
    RE_NPC = re.compile(r"\{\s*(?:nil|[A-Za-z_][\w.]*)\s*,\s*(\d{2,5})\s*,\s*\"")
    RE_ADD = re.compile(r"(?:AddNpcEx|HD3_AddNpc|AddNpc)\s*\(\s*(\d+)")
    RE_TPL = re.compile(r"YDBZ_NPC_TPL\s*=\s*(\d+)")
    dung = {}
    for p in tep:
        for i, l in enumerate(noi_dung(p).split("\r\n"), 1):
            l2 = bo_chu_thich(l)
            for rx in (RE_NPC, RE_ADD, RE_TPL):
                for m in rx.finditer(l2):
                    dung.setdefault(int(m.group(1)), []).append("%s:%d" % (os.path.basename(p), i))
    thieu = [k for k in sorted(dung) if k not in ids]
    if thieu:
        for k in thieu:
            bao("D", "LOI", "NPC template %d CHUA KHAI trong npcs.txt  (%s)"
                % (k, ", ".join(sorted(set(dung[k]))[:2])))
    else:
        bao("D", "OK", "%d template NPC dung, tat ca da khai" % len(dung))


# ---------------------------------------------------------------- E. Ban do
def soat_bando(tep):
    ml = io.open(SRV + r"\settings\MapList.ini", encoding="latin-1", newline="").read()
    ws = io.open(SRV + r"\maps\WorldSet_GameServer.ini", encoding="latin-1", newline="").read()
    khai = set(int(x) for x in re.findall(r"(?m)^(\d+)\s*=", ml))
    nap = set(int(x) for x in re.findall(r"(?m)^World\d+\s*=\s*(\d+)", ws))
    can = set()
    for p in tep:
        d = noi_dung(p)
        for m in re.finditer(r"YDBZ_MAP_MAP\s*=\s*\{([^}]*)\}", d):
            can |= set(int(x) for x in re.findall(r"\d+", m.group(1)))
        for m in re.finditer(r"DUNGEON_POOL\[(\d+)\]\s*=\s*\{([^}]*)\}", d):
            can.add(int(m.group(1)))
            can |= set(int(x) for x in re.findall(r"\d+", m.group(2)))
        for m in re.finditer(r"nTemplateMapId\s*=\s*(\d+)|YDBZ_NEO_MAPID\s*=\s*(\d+)", d):
            can.add(int(m.group(1) or m.group(2)))
    x1 = [m for m in sorted(can) if m not in khai]
    x2 = [m for m in sorted(can) if m not in nap]
    if x1:
        bao("E", "LOI", "ban do CHUA khai trong MapList.ini: %s" % x1)
    if x2:
        bao("E", "LOI", "ban do CHUA nap trong WorldSet_GameServer.ini: %s" % x2)
    if not x1 and not x2:
        bao("E", "OK", "%d ban do dung, deu da khai va da nap" % len(can))
    # du lieu ban do co that khong
    thieu = []
    for m in sorted(can):
        mm = re.search(r"(?m)^%d\s*=\s*(.+)$" % m, ml)
        if not mm:
            continue
        thu = SRV + "\\maps\\" + mm.group(1).strip()
        if not os.path.isdir(thu):
            thieu.append((m, mm.group(1).strip()))
    if thieu:
        for m, d in thieu[:5]:
            bao("E", "LOI", "ban do %d tro toi thu muc KHONG CO: maps\\%s" % (m, d))
    else:
        bao("E", "OK", "moi ban do deu co thu muc du lieu tren dia")


# ------------------------------------------------------------- F. task / mission
def soat_task(tep):
    dung = {}
    for p in tep:
        for i, l in enumerate(noi_dung(p).split("\r\n"), 1):
            l2 = bo_chu_thich(l)
            for m in re.finditer(r"\b(SetTask|GetTask)\s*\(\s*(\d+)", l2):
                dung.setdefault(int(m.group(2)), []).append("%s:%d" % (os.path.basename(p), i))
    # ai khac dung
    khac = {}
    for dp, dn, fn in os.walk(SRV + r"\script"):
        if "yandibaozang" in dp or "\\viemde" in dp or "basemission" in dp:
            continue
        for f in fn:
            if not f.endswith(".lua") or ".truoc_" in f:
                continue
            try:
                d = io.open(os.path.join(dp, f), encoding="latin-1", newline="").read()
            except Exception:
                continue
            for m in re.finditer(r"\b(?:SetTask|GetTask)\s*\(\s*(\d+)", d):
                khac.setdefault(int(m.group(1)), set()).add(f)
    dam = [k for k in sorted(dung) if k in khac]
    if dam:
        for k in dam:
            bao("F", "CANH BAO", "task %d dung chung voi: %s" % (k, ", ".join(sorted(khac[k])[:3])))
    else:
        bao("F", "OK", "%d task id dung, khong dam vao tinh nang khac" % len(dung))


# ------------------------------------------------------- G. cu phap + ma hoa
def soat_cuphap(tep):
    loi = []
    for p in tep:
        try:
            r = subprocess.run([SYNCHECK, p], capture_output=True, timeout=60)
            out = (r.stdout or b"").decode("latin-1", "replace")
        except Exception as e:
            loi.append((p, str(e)))
            continue
        if "LOI CU PHAP" in out:
            loi.append((p, out.strip().split("\n")[0][:80]))
    if loi:
        for p, m in loi:
            bao("G", "LOI", "cu phap: %s -> %s" % (os.path.basename(p), m))
    else:
        bao("G", "OK", "%d tep .lua: 0 loi cu phap" % len(tep))
    xau = []
    for p in tep:
        d = open(p, "rb").read()
        if d.count(b"\xef\xbf\xbd"):
            xau.append((p, "co FFFD"))
        if d.count(b"\n") - d.count(b"\r\n") != 0:
            xau.append((p, "co dong LF le"))
    if xau:
        for p, m in xau:
            bao("G", "LOI", "ma hoa: %s -> %s" % (os.path.basename(p), m))
    else:
        bao("G", "OK", "%d tep: FFFD=0, khong co dong LF le" % len(tep))


# ------------------------------------------------------------------ H. be pho ban
def soat_be(tep):
    cfg = io.open(SRV + r"\script\header\cauhinh_hoatdong.lua", encoding="latin-1", newline="").read()
    m = re.search(r"YDBZ_PHONG_TOIDA\s*=\s*(\d+)", cfg)
    n_cfg = int(m.group(1)) if m else None
    n_be = None
    for p in tep:
        mm = re.search(r"DUNGEON_POOL\[\d+\]\s*=\s*\{([^}]*)\}", noi_dung(p))
        if mm:
            n_be = len(re.findall(r"\d+", mm.group(1)))
    if n_cfg is None or n_be is None:
        bao("H", "CANH BAO", "khong doc duoc so phong (cfg=%s, be=%s)" % (n_cfg, n_be))
    elif n_cfg != n_be:
        bao("H", "LOI", "cau hinh cho %d phong nhung be chi khai %d ban do" % (n_cfg, n_be))
    else:
        bao("H", "OK", "cau hinh %d phong = %d ban do trong be (khop)" % (n_cfg, n_be))


def main():
    tep = cac_tep()
    print("PHAN BIEN VIEM DE - soat %d tep .lua" % len(tep))
    print("=" * 78)
    soat_include(tep)
    soat_ham(tep)
    soat_vatpham(tep)
    soat_npc(tep)
    soat_bando(tep)
    soat_task(tep)
    soat_cuphap(tep)
    soat_be(tep)
    for ten, ds in (("LOI", LOI), ("CANH BAO", CANHBAO), ("OK", OK)):
        if not ds:
            continue
        print()
        print("---- %s (%d) ----" % (ten, len(ds)))
        for muc, t in ds:
            print("  [%s] %s" % (muc, t))
    if DASOAT_HIT:
        print()
        print("---- DA SOAT, CO Y CHAP NHAN (%d) ----" % len(DASOAT_HIT))
        for t in sorted(DASOAT_HIT): print("  ", t)
    if NGOAI:
        print()
        print("---- NGOAI PHAM VI DOT NAY (%d) ----" % len(NGOAI))
        for t in sorted(set(NGOAI)): print("  ", t)
    print()
    print("=" * 78)
    print("KET LUAN: %d LOI, %d CANH BAO, %d OK" % (len(LOI), len(CANHBAO), len(OK)))
    print("  -> %s" % ("CHUA duoc gan vao - phai het LOI truoc" if LOI else "khong con LOI"))


main()
