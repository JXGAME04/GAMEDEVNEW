# -*- coding: utf-8 -*-
"""C26 - SOAT TOAN BO DOI THOAI + THONG BAO cua 3 hoat dong, DOI CHIEU BAN LINUX.
Bat 3 loai loi:
  (A) THIEU/LECH so voi Linux: tep port khac tep goc (so loi goi thoai, so chuoi).
  (B) VO TRAN GOI 511B: JX1 gioi han moi goi thoai 511 byte (bug "<#>M" cua NPC
      769). Tinh tong byte cua tung goi Describe/Say (giai ContentList[i] trong
      chinh tep) roi bao goi nao >= 500B.
  (C) THE <color...> DINH SAT chu co dau: TEncodeText nuot dau '<' (bug "<co lor>").
Xuat bao cao ra man hinh + tep audit\\C26_soat_thoai.md
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
LNX = r"D:\ServerLinux\server1"
OUT = r"D:\GAMEDEVNEW\ReverseTools\port_3hd\audit\C26_soat_thoai.md"

# cac thu muc/tep thuoc 3 hoat dong (duong TUONG DOI tu goc server)
THUMUC = [
    r"script\task\tollgate\killer",
    r"script\missions\fengling_ferry",
    r"script\missions\challengeoftime",
    r"script\vng_feature\challengeoftime",
]
TEPLE = [
    r"script\task\tollgate\killbosshead.lua",
    r"settings\trigger_challengeoftime.lua",
    r"settings\trigger_include.lua",
]

HAM_THOAI = ("Describe", "Say", "SayEx", "Talk")
HAM_TB = ("Msg2Player", "Msg2SubWorld", "AddGlobalNews", "AddGlobalCountNews",
          "AddLocalCountNews", "PutMessage", "MsgToGlobal")
TRAN = 511


def liet_ke():
    tep = []
    for t in THUMUC:
        d = os.path.join(JX1, t)
        if not os.path.isdir(d):
            continue
        for f in sorted(os.listdir(d)):
            if f.endswith(".lua"):
                tep.append(os.path.join(t, f))
        # 1 cap con (vd challengeoftime\item)
        for f in sorted(os.listdir(d)):
            sub = os.path.join(d, f)
            if os.path.isdir(sub):
                for g in sorted(os.listdir(sub)):
                    if g.endswith(".lua"):
                        tep.append(os.path.join(t, f, g))
    for t in TEPLE:
        if os.path.exists(os.path.join(JX1, t)):
            tep.append(t)
    return tep


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def bang_content(txt):
    """giai bang ContentList = { "..", .. } trong chinh tep -> {chi so: chuoi}"""
    out = {}
    m = re.search(r"ContentList\s*=\s*\{(.*?)\n\}", txt, re.S)
    if not m:
        return out
    i = 0
    for line in m.group(1).split("\n"):
        s = line.strip()
        if not s.startswith('"'):
            continue
        i += 1
        j = s.rfind('"')
        out[i] = s[1:j] if j > 0 else s
    return out


def goi_thoai(txt):
    """tra list (ham, dong, danh sach doi so tho)"""
    ra = []
    for i, line in enumerate(txt.split("\n"), 1):
        s = line.lstrip()
        if s.startswith("--"):
            continue
        for m in re.finditer(r"\b(" + "|".join(HAM_THOAI) + r")\s*\(", s):
            ra.append((m.group(1), i, s[m.end():]))
    return ra


def do_goi(args, cl):
    """uoc luong so byte cua mot goi thoai: cong do dai moi chuoi + moi ContentList[i]"""
    n = 0
    for lit in re.findall(r'"([^"]*)"', args):
        n += len(lit.encode("latin-1"))
    for idx in re.findall(r"ContentList\[(\d+)\]", args):
        n += len(cl.get(int(idx), "").encode("latin-1"))
    return n


def dem_tb(txt):
    d = {}
    for line in txt.split("\n"):
        s = line.lstrip()
        if s.startswith("--"):
            continue
        for h in HAM_TB:
            d[h] = d.get(h, 0) + len(re.findall(r"\b" + h + r"\s*\(", s))
    return {k: v for k, v in d.items() if v}


def loi_color(txt):
    """the <color...> dung ngay sau byte cao (TCVN3) - se bi nuot dau '<'"""
    ra = []
    for i, line in enumerate(txt.split("\n"), 1):
        s = line.lstrip()
        if s.startswith("--"):
            continue
        for m in re.finditer(r"(.)<color", line):
            if ord(m.group(1)) > 0x80:
                ra.append((i, line.strip()[:70]))
                break
    return ra


bao = []
bao.append("# C26 - Soat doi thoai + thong bao 3 hoat dong (doi chieu ban Linux)\n")
tong_thieu = tong_vo = tong_color = 0
rows = []
for rel in liet_ke():
    pj = os.path.join(JX1, rel)
    pl = os.path.join(LNX, rel)
    tj = doc(pj)
    co_linux = os.path.exists(pl)
    tl = doc(pl) if co_linux else ""
    clj = bang_content(tj)
    cll = bang_content(tl) if co_linux else {}
    gj = goi_thoai(tj)
    gl = goi_thoai(tl) if co_linux else []
    tbj = dem_tb(tj)
    tbl = dem_tb(tl) if co_linux else {}
    # (B) vo tran
    vo = []
    for (h, dong, args) in gj:
        n = do_goi(args, clj)
        if n >= 500:
            vo.append((dong, h, n))
    # (C) color
    lc = loi_color(tj)
    # (A) lech so luong
    lech = []
    if co_linux:
        if len(gj) != len(gl):
            lech.append("goi thoai %d/%d" % (len(gj), len(gl)))
        for k in set(list(tbj) + list(tbl)):
            if tbj.get(k, 0) != tbl.get(k, 0):
                lech.append("%s %d/%d" % (k, tbj.get(k, 0), tbl.get(k, 0)))
        if len(clj) != len(cll):
            lech.append("ContentList %d/%d" % (len(clj), len(cll)))
    tong_thieu += len(lech)
    tong_vo += len(vo)
    tong_color += len(lc)
    if lech or vo or lc or not co_linux:
        rows.append((rel, co_linux, lech, vo, lc, len(tj), len(tl)))

print("=" * 70)
print("Da soat %d tep." % len(liet_ke()))
print("Lech so voi Linux: %d muc | Goi >=500B: %d | The <color> dinh chu: %d"
      % (tong_thieu, tong_vo, tong_color))
print("=" * 70)
for (rel, co, lech, vo, lc, sj, sl) in rows:
    print("\n### %s" % rel)
    if not co:
        print("   (KHONG co ban Linux tuong ung - tep rieng cua JX1)")
    else:
        print("   byte: JX1 %d / Linux %d %s" % (sj, sl, "(KHOP)" if sj == sl else "<-- KHAC"))
    for x in lech:
        print("   [A] LECH:", x)
    for (dong, h, n) in vo:
        print("   [B] dong %d %s(...) = %dB %s" % (dong, h, n, ">>> VO TRAN 511" if n >= TRAN else "(sat tran)"))
    for (dong, s) in lc[:4]:
        print("   [C] dong %d the <color> dinh chu co dau: %s" % (dong, s[:60]))

os.makedirs(os.path.dirname(OUT), exist_ok=True)
with io.open(OUT, "w", encoding="utf-8") as f:
    f.write("# C26 - Soat doi thoai + thong bao 3 hoat dong (doi chieu Linux)\n\n")
    f.write("Tep soat: %d | Lech: %d | Goi >=500B: %d | Loi <color>: %d\n\n"
            % (len(liet_ke()), tong_thieu, tong_vo, tong_color))
    for (rel, co, lech, vo, lc, sj, sl) in rows:
        f.write("## %s\n" % rel)
        f.write("- byte JX1 %d / Linux %d%s\n" % (sj, sl, "" if sj == sl else "  **KHAC**"))
        for x in lech:
            f.write("- [A] LECH: %s\n" % x)
        for (dong, h, n) in vo:
            f.write("- [B] dong %d `%s(...)` = **%dB**\n" % (dong, h, n))
        for (dong, s) in lc[:6]:
            f.write("- [C] dong %d the `<color>` dinh chu co dau\n" % dong)
        f.write("\n")
print("\nda ghi", OUT)
