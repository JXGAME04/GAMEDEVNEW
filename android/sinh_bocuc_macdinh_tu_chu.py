# -*- coding: utf-8 -*-
r"""[SUAGD 13/09] Lay bo cuc chu DANG DUNG (tep nguoi choi tren may ao) lam bo cuc MAC DINH cua game cho ca hai ho khung.
Chu 13/09: "tam thoi luu toa do hien tai lam mac dinh va toi muon ban xoa di cac icon toi da an".

- Nguon: tep nguoi choi (mac dinh: tep theo nhan vat moi nhat trong D:\jx1_android_data\userdata, khong co thi uitoado.ini).
- Giu cac dong "Goc.<lop>=<muc>" cua tep mac dinh dang co (KhoaCuaCha can chung de neo con theo cha), GhiTepVao cua game
  khong ghi cac dong nay.
- O da giau (truong Co bit 1) GIU NGUYEN co giau trong mac dinh -> nguoi choi nao cung khong thay. Tu 13/09 e cac khoa
  nay VAN o trong ui\uitoado_danhsach.ini nhung trinh chinh cat di (khong ve, khong chon) cho den khi bam "O da giau".
- HIEN_LAI: khoa bi giau trong tep nguon nhung chu muon HIEN (13/09 e: "mini skill goc phai" = nut Auto hai kiem cheo
  KUiPlayerBar|AutoPlay, chu da keo len goc phai tren 1006,168 luc no con giau) -> go bit giau khi sinh.
- Ghi: android\du_lieu_ghi_de\ui\uitoado_macdinh_rong.ini (ho dien thoai) + uitoado_macdinh.ini (ho may tinh bang)
  + ban sao vao D:\jx1_android_data\ui\. Bo cuc neo (ManHinh=W,H) tu doi cho khung khac.

Dung:  python android\sinh_bocuc_macdinh_tu_chu.py [tep nguon]
"""
import glob
import io
import os
import sys

GOC = os.path.dirname(os.path.abspath(__file__))
LOP = os.path.join(GOC, "du_lieu_ghi_de", "ui")
MAYAO = r"D:\jx1_android_data"
HIEN_LAI = []		# [SUAGD 13/09 e] khoa giau trong tep nguon nhung mac dinh phai HIEN (chu 13/09: nut Auto "du thua" -> de trong)


def nguon_mac_dinh():
    if len(sys.argv) > 1:
        return sys.argv[1]
    cac = glob.glob(os.path.join(MAYAO, "userdata", "uitoado_*.ini"))
    cac = [p for p in cac if "sao_luu" not in p.lower()]
    cac.sort(key=os.path.getmtime, reverse=True)
    if cac:
        return cac[0]
    return os.path.join(MAYAO, "userdata", "uitoado.ini")


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


src = nguon_mac_dinh()
s = doc(src)
nl = "\r\n" if "\r\n" in s else "\n"
dong = s.replace("\r\n", "\n").split("\n")
muc = [l for l in dong if l and not l.startswith(";") and not l.startswith("[") and not l.startswith("ManHinh") and not l.startswith("Goc.")]
manhinh = next((l for l in dong if l.startswith("ManHinh")), "ManHinh=1040,604")


def hien_lai(l):
    k, v = l.split("=", 1)
    t = v.split(",")
    if k in HIEN_LAI and len(t) >= 4 and int(t[3]) & 1:
        t[3] = str(int(t[3]) & ~1)
        print("hien lai:", k, "->", ",".join(t))
        return k + "=" + ",".join(t)
    return l


muc = [hien_lai(l) for l in muc]
an = [l.split("=")[0] for l in muc if len(l.split("=")[1].split(",")) >= 4 and int(l.split("=")[1].split(",")[3]) & 1]

# dong Goc. tu tep mac dinh dang co
goc = []
for ten in ("uitoado_macdinh_rong.ini", "uitoado_macdinh.ini"):
    p = os.path.join(LOP, ten)
    if os.path.isfile(p):
        for l in doc(p).replace("\r\n", "\n").split("\n"):
            if l.startswith("Goc.") and l not in goc:
                goc.append(l)
if not goc:
    goc = ["Goc.KSysMsgCentrePad=SysRoom", "Goc.KUiMiniMap=MiniMap"]

dau = [
    "; [SUAGD 13/09] Bo cuc MAC DINH cua game = bo cuc chu dang dung, lay tu %s (sinh boi android/sinh_bocuc_macdinh_tu_chu.py)." % os.path.basename(src),
    "; Moi dong: <lop>|<muc> = Left,Top,TiLe,Co,NeoX,NeoY (tuong doi cha; toa do trong VUNG AN TOAN cua khung ManHinh; Co bit 1 = giau).",
    "; O da giau theo y chu: " + (", ".join(an) if an else "(khong)"),
    "[Pos]",
] + goc + [manhinh] + muc
txt = "\r\n".join(dau) + "\r\n"
for goc_dir in (LOP, os.path.join(MAYAO, "ui")):
    for ten in ("uitoado_macdinh_rong.ini", "uitoado_macdinh.ini"):
        p = os.path.join(goc_dir, ten)
        os.makedirs(goc_dir, exist_ok=True)
        io.open(p, "w", encoding="latin-1", newline="").write(txt)
        print("da ghi:", p, "(%d muc)" % len(muc))
print("nguon:", src, "|", manhinh)
print("o da giau (trinh chinh cat di, bam 'O da giau' moi thay):", ", ".join(an) if an else "(khong)")
