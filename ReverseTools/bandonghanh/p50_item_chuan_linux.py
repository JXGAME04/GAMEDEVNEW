# -*- coding: utf-8 -*-
r"""[PETSYS 29/08 chieu] LAM LAI CHUAN - moi so lieu tu NGUON, khong doan:
A. head.lua + PLOG guard  -> het "attempt to call global PLOG (nil)" trong
   state item script (ScriptError.log 11:41) = goc "item trang bi loi script"
   va "bi kip dung khong tru".
B. 6 item pet: Ten/Anh/Intro lay NGUYEN VAN tu bang LINUX
   settings\item\004\magicscript.txt (id JX1 = id Linux + 1421):
     3453 Thiep      -> 4874   \spr\item\lottery\wulincaiquan_red.spr
     3454 Thuoc      -> 4875   \spr\item\medecine\fuyuanlu_small.spr
     3455 Trai tao   -> 4876   \spr\item\vietnam\christmas2009\hongpingguo.spr
     3456 Mia        -> 4877   \spr\item\script\...\jipinbujuan.spr
     3457 Bap        -> 4878   \spr\item\script\yumi.spr
     3458 Khoai lang -> 4879   \spr\item\script\hongshu.spr
C. Bi kip 4880: theo bang VLTK id 4808 "Bi kiep ban dong hanh" +
   \spr\item\questkey\obj_item_lection02.spr
D. 6 trang bi 4881..4886: KHONG co trong ban Linux/VLTK (da quet: 0 dong
   equip 'dong hanh') -> dung ANH TRANG BI THAT co san trong bang JX1.
E. Anh nao client JX1 thieu -> rut tu pak VLTK ve loose.
F. Thuoc stack 200 (chu: "cho nhan 200 vien").
"""
import io
import os
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump as P  # noqa: E402
import ucl  # noqa: E402

CR = chr(13)
BS = chr(92)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
VL = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"
LX = r"D:\ServerLinux\server1\settings\item\004\magicscript.txt"
VLI = r"D:\GAMEDEVNEW\ReverseTools\bandonghanh\ra_pet\vltk_item\004.txt"


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


def bang(p):
    return [d.split("\t") for d in doc_lf(p).split("\n") if d.strip()]


# ---------- A. PLOG vao head ----------
p = SV + r"\script\petsys\head.lua"
lf = doc_lf(p)
if "function PLOG" in lf:
    print("A. da co PLOG")
else:
    them = "\n".join([
        "-- [29/08] PLOG phai co o MOI state petsys (item script chay state rieng;",
        "-- thieu -> 'attempt to call global PLOG' lam dut ca ham dung item)",
        "if (PLOG == nil) then",
        "\tfunction PLOG(sz)",
        '\t\tappendto("petops.log")',
        '\t\twrite("    " .. sz .. "' + BS + 'n")',
        "\t\twriteto()",
        "\tend",
        "end",
        "",
    ])
    ghi_crlf(p, them + lf)
    print("A. VA PLOG guard vao head.lua")

# ---------- B/C. anh + ten + intro tu nguon ----------
lx = {c[3]: c for c in bang(LX) if len(c) > 9}
vli = {c[3]: c for c in bang(VLI) if len(c) > 9}
MAP = {}
for idl in ("3453", "3454", "3455", "3456", "3457", "3458"):
    c = lx.get(idl)
    assert c, "thieu Linux " + idl
    MAP[str(int(idl) + 1421)] = (c[0], c[4], c[8])
cb = vli.get("4808")
assert cb, "thieu VLTK 4808"
MAP["4880"] = (cb[0], cb[4], cb[8])

# D. anh trang bi that trong bang JX1 (tim item equip pho thong)
jx = bang(SV + r"\settings\item\magicscript.txt")
hdj = jx[0]


def tim_anh_jx(tu_khoa):
    for c in jx[1:]:
        if len(c) > 4 and tu_khoa in c[4].lower() and c[4].lower().endswith(".spr"):
            return c[4]
    return None


EQ = [("weapon", "sword"), ("hat", "helmet"), ("armor", "cloth"),
      ("belt", "yaodai"), ("boot", "shoe"), ("necklace", "amulet")]
anh_eq = []
for a, b in EQ:
    anh_eq.append(tim_anh_jx(a) or tim_anh_jx(b) or r"\spr\item\questkey\random_taskbook.spr")
TEN_EQ = [c[0] for c in (lx.get("3453"),)] and None  # khong dung ten Linux cho equip

# ---------- ap vao 2 bang JX1 ----------
for pth in (SV + r"\settings\item\magicscript.txt", CL + r"\settings\item\magicscript.txt"):
    rows = bang(pth)
    doi = 0
    for c in rows:
        if len(c) < 13:
            continue
        pid = c[3]
        if pid in MAP:
            ten, anh, intro = MAP[pid]
            c[0], c[4], c[8] = ten, anh, intro
            if pid == "4875":
                c[12] = "200"          # thuoc: stack 200
            doi += 1
        elif pid in ("4881", "4882", "4883", "4884", "4885", "4886"):
            k = int(pid) - 4881
            c[4] = anh_eq[k]
            doi += 1
    ghi_crlf(pth, "\n".join("\t".join(c) for c in rows) + "\n")
    print("B/C/D. cap nhat", doi, "dong:", pth[:2])

# ---------- E. rut anh thieu tu VLTK ----------
THU_TU = ["1024.pak", "serverlist.pak", "slistcache.pak"] + \
    ["updatejx%02d.pak" % i for i in range(17, 0, -1)] + \
    ["updatejx07_jxf06.pak", "update02.pak", "update.pak", "update04.pak",
     "update03.pak", "update01.pak", "skills.pak", "spr.pak", "resource.pak"]


def blob(f, e):
    u, off, size, cf = e
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    if fl == 0x20:
        fl = 1
    fl &= 0x0F
    f.seek(off)
    raw = f.read(cs if cs else size)
    try:
        if fl == 0 or cs in (0, size):
            return raw[:size] if size > 0 else raw
        if fl == 1:
            return ucl.nrv2b_decompress_8(raw, size)
        import bz2
        import zlib
        return bz2.decompress(raw) if fl == 2 else zlib.decompress(raw)
    except Exception:
        return b""


can = [v[1] for v in MAP.values()] + anh_eq
thieu_loose = [a for a in can if not os.path.exists(CL + a)]
# loc anh co trong pak JX1
con = []
for a in thieu_loose:
    uid = P.name2id(a)
    co = False
    import glob
    for pak in glob.glob(CL + r"\*.pak"):
        try:
            f, es = P.entries(pak)
        except Exception:
            continue
        if any(e[0] == uid for e in es):
            co = True
        f.close()
        if co:
            break
    if not co:
        con.append(a)
print("E. anh thieu loose:", len(thieu_loose), "| khong co ca trong pak JX1:", len(con))
ids = {P.name2id(a): a for a in con}
rut = 0
for pak in THU_TU:
    p2 = os.path.join(VL, pak)
    if not os.path.exists(p2) or not ids:
        continue
    f, es = P.entries(p2)
    for e in es:
        if e[0] in ids:
            d = blob(f, e)
            duong = ids.pop(e[0])
            ra = CL + duong
            os.makedirs(os.path.dirname(ra), exist_ok=True)
            open(ra, "wb").write(d)
            rut += 1
    f.close()
print("E. rut tu VLTK:", rut, "| van thieu:", list(ids.values()))
print("XONG p50")
