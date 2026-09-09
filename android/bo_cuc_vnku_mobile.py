# -*- coding: utf-8 -*-
r"""[ANDROID 10/09] HUD dien thoai theo BO CUC VNKU (chu: "ban thiet ke theo bo cuc vnku duoc khong").

Mau: D:\USVOLAM\JX1M_VNKU_DOCS\anh_mau_bo_cuc_hud.jpeg (anh chup JX1M that, 1034x584):
  - hang ICON TRON ngay duoi thanh trang thai (Nhan vat, Hanh trang, Vo cong, Bang hoi, To doi, Ngoi, Hao huu, Cai dat)
  - cot icon ben phai duoi ban do nho (Trao doi, Auto, Nhiem vu, PK, ...)
  - KHUNG CHAT NHO giua day man hinh (~41% be ngang), cum ky nang goc phai duoi, can di chuyen goc trai duoi.
Anh VNKU to gap 5/3 so voi .ini cua ho (anh_xa_cua_so.md §0.1) va bo ve cua ta KHONG co gian anh -> thu nho
san bang cong cu nay (ghi .spr nhieu khung, bang mau chung).

Sinh ra (ghi vao lop ghi de android\du_lieu_ghi_de\ va D:\jx1_android_data neu co):
  spr\uinew\uitoolscontrolbar\<ten>_m.spr   icon tron VNKU thu nho (2 khung: thuong / bam)
  spr\uinew\uiplayerbar\khung_chat_mobile.spr  khung chat thu nho, giua day, nen trong suot 1040x604
  ui\ui3\uiplayerbar.ini, uitoolscontrolbar.ini   tu ban goc cay client PC + doi anh + dat lai vi tri
Dung: python android\bo_cuc_vnku_mobile.py            (them "mock" de ghep anh dung thu tu anh chup giả lập)
Ban PC khong doi gi. Khung ve giả lập 1040x604; toa do ini tuyet doi nhu moi ini khac.
"""
import io
import os
import re
import struct
import sys

from PIL import Image

GOC = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(GOC, "..", "ReverseTools", "tongkim_chat"))
from ghi_spr import _rle_mot_khung, kiem_spr  # noqa: E402

VNKU = r"C:\Users\nguye\Downloads\NHACTAI\VNKU_ui\spr\Spr\UiNew"                   # CHI DOC
NGUON_PC = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\ui\ui3"          # ini goc
GHI_DE = os.path.join(GOC, "du_lieu_ghi_de")
DU_LIEU = r"D:\jx1_android_data"
SW, SH = 1040, 604
DX, DY = 3, 1          # chu dat KUiPlayerBar|Main=3,1 trong UiToaDo -> ini = man hinh - (3,1)

# ----------------------------------------------------------------------------- doc / ghi SPR nhieu khung
def doc_spr(path):
    """-> (W, H, [anh RGBA moi khung], [(ox, oy)])"""
    d = open(path, "rb").read()
    assert d[:3] == b"SPR", path
    W, H, cx, cy, frames, colors, dirs, itv = struct.unpack("<HHHHHHHH", d[4:20])
    pos = 32
    pal = [(d[pos + 3 * i], d[pos + 3 * i + 1], d[pos + 3 * i + 2]) for i in range(colors)]
    pos += colors * 3
    offs = [struct.unpack("<II", d[pos + 8 * i:pos + 8 * i + 8]) for i in range(frames)]
    base = pos + frames * 8
    ra, lech = [], []
    for fo, fl in offs:
        p = base + fo
        fw, fh, ox, oy = struct.unpack("<HHHH", d[p:p + 8])
        p += 8
        img = Image.new("RGBA", (fw, fh), (0, 0, 0, 0))
        px = img.load()
        x = y = 0
        end = base + fo + fl
        while y < fh and p < end:
            run, alpha = d[p], d[p + 1]
            p += 2
            if alpha == 0:
                x += run
            else:
                for _ in range(run):
                    idx = d[p]
                    p += 1
                    if x < fw:
                        r, g, b = pal[idx] if idx < len(pal) else (255, 0, 255)
                        px[x, y] = (r, g, b, 255 if alpha >= 31 else int(alpha * 255 / 31))
                    x += 1
            if x >= fw:
                x, y = 0, y + 1
        ra.append(img)
        lech.append((ox, oy))
    return W, H, ra, lech


def ghi_spr_nhieu_khung(cac_khung, path, W=None, H=None):
    """Ghi nhieu anh RGBA (cung co) thanh mot SPR, bang mau CHUNG 256 mau (luon ghi du 768 byte)."""
    cac_khung = [k.convert("RGBA") for k in cac_khung]
    w, h = cac_khung[0].size
    W, H = W or w, H or h
    dai = Image.new("RGB", (w * len(cac_khung), h), (0, 0, 0))
    for i, k in enumerate(cac_khung):
        px, pd = k.load(), dai.load()
        for y in range(h):
            for x in range(w):
                r, g, b, a = px[x, y]
                pd[i * w + x, y] = (r, g, b) if a > 0 else (0, 0, 0)
    pal_img = dai.quantize(colors=256, method=Image.MEDIANCUT, dither=Image.NONE)
    pal = list(pal_img.getpalette() or [])[:768]
    pal += [0] * (768 - len(pal))
    khung_bytes = []
    for i, k in enumerate(cac_khung):
        idx = pal_img.crop((i * w, 0, (i + 1) * w, h)).load()
        khung_bytes.append(struct.pack("<HHHH", w, h, 0, 0) + _rle_mot_khung(k, idx))
    out = bytearray()
    out += struct.pack("<4sHHHHHHHH", b"SPR\x00", W, H, 0, 0, len(cac_khung), 256, 1, 1) + b"\x00" * 12
    out += bytes(pal)
    o = 0
    for kb in khung_bytes:
        out += struct.pack("<II", o, len(kb))
        o += len(kb)
    for kb in khung_bytes:
        out += kb
    os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, "wb").write(bytes(out))
    ok, msg = kiem_spr(path, bat_buoc=True)
    return msg


def thu_nho(path, cao_muc_tieu, so_khung=2):
    """Doc spr VNKU, thu nho theo chieu cao muc tieu (giu ti le), lay so_khung khung dau."""
    W, H, ks, _ = doc_spr(path)
    k = cao_muc_tieu / float(ks[0].size[1])
    w, h = int(round(ks[0].size[0] * k)), cao_muc_tieu
    return [x.resize((w, h), Image.LANCZOS) for x in ks[:so_khung]]


# ----------------------------------------------------------------------------- bo cuc
ICON_CAO = 50           # icon tron co nhan (anh 168 cao) -> 50 px, tron ~40 px nhu anh mau
ICON_HANG = ["Status", "Items", "Skills", "Faction", "Team", "Sit", "Friend", "Options"]
ANH_VNKU = {            # o -> (tep VNKU, chieu cao muc tieu, so khung)
    "Status":   ("UiToolsControlBar/nhanvat.spr", ICON_CAO, 2),
    "Items":    ("UiToolsControlBar/hanhtrang.spr", ICON_CAO, 2),
    "Skills":   ("UiToolsControlBar/vocong.spr", ICON_CAO, 2),
    "Faction":  ("UiToolsControlBar/banghoi.spr", ICON_CAO, 2),
    "Team":     ("UiToolsControlBar/todoi.spr", ICON_CAO, 2),
    "Sit":      ("UiToolsControlBar/ngoixuong.spr", ICON_CAO, 2),
    "Friend":   ("UiToolsControlBar/haohuu.spr", ICON_CAO, 2),
    "Options":  ("UiToolsControlBar/caidat.spr", ICON_CAO, 2),
    "Exchange": ("UiToolsControlBar/icon_traodoi.spr", ICON_CAO, 2),
    "Horse":    ("UiToolsControlBar/lenngua.spr", ICON_CAO, 2),
    "Run":      ("UiToolsControlBar/dichuyen.spr", ICON_CAO, 2),
    "PK":       ("UiToolsControlBar/pk.spr", 44, 3),
}
TEN_SPR = {o: os.path.splitext(os.path.basename(t[0]))[0] + "_m" for o, t in ANH_VNKU.items()}
K_KHUNG = 0.32          # khung chat: 1343x211 -> 430x68, giua day (anh mau ~41% be ngang)
GOC_W, GOC_H = 1343, 211


def bo_cuc(co_icon):
    """Toa do MAN HINH. co_icon[o] = (w, h) cua anh da thu nho (de can giua theo tam)."""
    ra = {}
    # 1. hang icon tron duoi thanh trang thai: tam x 400..792 buoc 56 (nhan "Hanh trang" rong 73 nen buoc 50
    #    cua anh mau bi de chu), tam y 72; ket thuc truoc ban do nho (x 854)
    for i, o in enumerate(ICON_HANG):
        w, h = co_icon[o]
        ra[o] = (400 + 56 * i - w // 2, 72 - h // 2)
    # 2. luoi 3x2 ben phai, DUOI nut "Sua giao dien" (y 172..200) va TREN cum ky nang (y ~320):
    #    Trao doi, Len ngua, Chay / PK, Auto, Ghi hinh   (o 46, buoc 50)
    LUOI = ["Exchange", "Horse", "Run", "PK", "AutoPlay", "Rec"]
    for i, o in enumerate(LUOI):
        w, h = co_icon.get(o, (28, 28))
        cx, cy = 868 + 50 * (i % 3) + 23, 215 + 50 * (i // 3) + 23
        ra[o] = (cx - w // 2, cy - h // 2)
    # 2b. tui hanh trang mo rong + an chat: canh T/P o day (duoi cum ky nang, x 830..900)
    ra["ItemEx"] = (834, SH - 38)
    ra["HideChat"] = (868, SH - 38)
    # 3. khung chat giua day
    w, h = int(round(GOC_W * K_KHUNG)), int(round(GOC_H * K_KHUNG))
    L, T = (SW - w) // 2, SH - h
    ra["khung"] = (L, T, w, h)
    X = lambda a: L + int(round(a * K_KHUNG))  # noqa: E731
    Y = lambda a: T + int(round(a * K_KHUNG))  # noqa: E731
    gy = (Y(100) + Y(175)) // 2
    ra["ChannelBtn"] = ((X(185) + X(252)) // 2 - 10, gy - 10)
    ra["InputEdit"] = (X(303) + 4, gy - 9, X(1009) - X(303) - 8, 18)
    ra["Face"] = ((X(1029) + X(1097)) // 2 - 11, gy - 11)
    ra["SendBtn"] = ((X(1124) + X(1192)) // 2 - 11, gy - 11)
    # 4. ky nang trai/phai (T/P) ngay ben phai khung chat
    ra["ImediaLeftSkill"] = (L + w + 10, SH - 42)
    ra["ImediaRightSkill"] = (L + w + 50, SH - 42)
    # 5. vat pham nhanh 1..9: mot hang goc trai duoi (36 px, buoc 33) - vung can di chuyen chi hien khi cham
    for i in range(9):
        ra["Item_%d" % i] = (2 + 33 * i, SH - 42)
    return ra


def anh_khung():
    W, H, ks, _ = doc_spr(os.path.join(VNKU, "UiPlayerBar", "khung_chat_new.spr"))
    L, T, w, h = bo_cuc({o: (0, 0) for o in ICON_HANG})["khung"]
    nen = Image.new("RGBA", (SW, SH), (0, 0, 0, 0))
    nen.alpha_composite(ks[0].resize((w, h), Image.LANCZOS), (L, T))
    return nen


# ----------------------------------------------------------------------------- ini
def dat_khoa(s, muc, khoa, gia_tri):
    ra, pos, n = [], 0, 0
    for m in re.finditer(r"^\[" + re.escape(muc) + r"\][ \t]*\r?\n", s, re.M):
        dau = m.end()
        m2 = re.search(r"^\[", s[dau:], re.M)
        cuoi = dau + m2.start() if m2 else len(s)
        than = s[dau:cuoi]
        mk = re.search(r"^" + re.escape(khoa) + r"=.*$", than, re.M)
        if mk:
            than = than[:mk.start()] + "%s=%s" % (khoa, gia_tri) + than[mk.end():]
        else:
            than_r = than.rstrip("\r\n")
            than = than_r + "\r\n%s=%s" % (khoa, gia_tri) + than[len(than_r):]
        ra.append(s[pos:dau] + than)
        pos, n = cuoi, n + 1
    ra.append(s[pos:])
    if n == 0:
        raise SystemExit("khong co muc [%s]" % muc)
    return "".join(ra)


def dat_vitri(s, muc, o, co=None):
    s = dat_khoa(s, muc, "Left", o[0] - DX)
    s = dat_khoa(s, muc, "Top", o[1] - DY)
    if len(o) == 4:
        co = (o[2], o[3])
    if co:
        s = dat_khoa(s, muc, "Width", co[0])
        s = dat_khoa(s, muc, "Height", co[1])
    return s


def dat_anh_vnku(s, muc, so_khung):
    s = dat_khoa(s, muc, "Image", r"\Spr\UiNew\UiToolsControlBar\%s.spr" % TEN_SPR[muc])
    s = dat_khoa(s, muc, "Up", 0)
    s = dat_khoa(s, muc, "Down", 1)
    s = dat_khoa(s, muc, "Over", 1 if so_khung >= 2 else 0)
    s = dat_khoa(s, muc, "OverFrame", 1 if so_khung >= 2 else 0)
    return s


def ghi_moi_noi(rel, du_lieu):
    for goc in [GHI_DE] + ([DU_LIEU] if os.path.isdir(DU_LIEU) else []):
        p = os.path.join(goc, rel)
        os.makedirs(os.path.dirname(p), exist_ok=True)
        io.open(p, "wb").write(du_lieu)
        print("  ghi", p)


def main(mock=False):
    # ---- icon VNKU thu nho
    co_icon, anh_icon = {}, {}
    for o, (tep, cao, so) in ANH_VNKU.items():
        ks = thu_nho(os.path.join(VNKU, tep.replace("/", os.sep)), cao, so)
        co_icon[o], anh_icon[o] = ks[0].size, ks[0]
        tmp = os.path.join(GOC, "_tmp.spr")
        print("%-9s %-18s %dx%d x%d  %s" % (o, TEN_SPR[o], ks[0].size[0], ks[0].size[1], len(ks), ghi_spr_nhieu_khung(ks, tmp)))
        ghi_moi_noi(os.path.join("spr", "uinew", "uitoolscontrolbar", TEN_SPR[o] + ".spr"), open(tmp, "rb").read())
        os.remove(tmp)
    bc = bo_cuc(co_icon)
    # ---- khung chat
    tmp = os.path.join(GOC, "_tmp.spr")
    print(ghi_spr_nhieu_khung([anh_khung()], tmp))
    ghi_moi_noi(os.path.join("spr", "uinew", "uiplayerbar", "khung_chat_mobile.spr"), open(tmp, "rb").read())
    os.remove(tmp)
    # ---- uiplayerbar.ini
    s = io.open(os.path.join(NGUON_PC, "uiplayerbar.ini"), encoding="latin-1", newline="").read()
    s = s.replace("; Giao dien man hinh\r\n", "; Giao dien man hinh\r\n; [ANDROID 10/09] HUD theo bo cuc VNKU: icon tron "
                  "VNKU thu nho, khung chat nho giua day, vat pham hang trai duoi (android\\bo_cuc_vnku_mobile.py).\r\n", 1)
    s = dat_khoa(s, "Main", "Image", r"\Spr\UiNew\UiPlayerBar\khung_chat_mobile.spr")
    s = dat_khoa(s, "Main", "Image1024", r"\Spr\UiNew\UiPlayerBar\khung_chat_mobile.spr")
    for o in ["Status", "Items", "Skills", "Faction", "Team", "Friend", "Options"]:
        s = dat_anh_vnku(s, o, ANH_VNKU[o][2])
        s = dat_vitri(s, o, bc[o], co_icon[o])
    for o in ["AutoPlay", "ItemEx", "HideChat", "ChannelBtn", "Face", "SendBtn", "ImediaLeftSkill", "ImediaRightSkill",
              "InputEdit"] + ["Item_%d" % i for i in range(9)]:
        s = dat_vitri(s, o, bc[o])
    ghi_moi_noi(os.path.join("ui", "ui3", "uiplayerbar.ini"), s.encode("latin-1"))
    # ---- uitoolscontrolbar.ini
    s = io.open(os.path.join(NGUON_PC, "uitoolscontrolbar.ini"), encoding="latin-1", newline="").read()
    s = s.replace("; Ui Designed Muaroilangtham\r\n", "; Ui Designed Muaroilangtham\r\n; [ANDROID 10/09] HUD theo bo cuc VNKU: "
                  "Ngoi vao hang icon tron; Trao doi/Len ngua/Chay/PK vao luoi ben phai (android\\bo_cuc_vnku_mobile.py).\r\n", 1)
    for khoa, gt in [("Left", 0), ("Top", 0), ("Width", SW), ("Height", SH)]:
        s = dat_khoa(s, "Main", khoa, gt)
    for o in ["Sit", "Exchange", "Horse", "Run", "PK"]:
        s = dat_anh_vnku(s, o, ANH_VNKU[o][2])
        s = dat_vitri(s, o, bc[o], co_icon[o])
    s = dat_vitri(s, "Rec", bc["Rec"])
    ghi_moi_noi(os.path.join("ui", "ui3", "uitoolscontrolbar.ini"), s.encode("latin-1"))
    print("xong. khung chat %s; hang icon tam y=72 x=400..; luoi phai tu (868,215); vat pham y=%d" % (bc["khung"], bc["Item_0"][1]))
    if mock:
        dung_thu(bc, anh_icon)


def dung_thu(bc, anh_icon):
    """Ghep anh dung thu len anh chup giả lập (chi de xem, khong vao game)."""
    S = "C:/Users/nguye/AppData/Local/Temp/claude/D--GAMEDEVNEW--claude-worktrees-quirky-joliot-3a4986/12982956-8ae5-4aa1-b223-755019e04ac8/scratchpad/"
    mh = Image.open(S + "man_hinh_1103.png").convert("RGBA")
    anh = mh.copy()
    nen = mh.getpixel((300, 400))
    mh.paste(Image.new("RGBA", (SW, SH - 496), nen), (0, 496))                  # xoa thanh cu
    mh.paste(Image.new("RGBA", (520, 40), nen), (440, 500))                       # xoa 6 nut cong cu cu
    mh.alpha_composite(anh_khung(), (0, 0))

    def cat(l, t, w, h):
        return anh.crop((l + 3, t + 1, l + 3 + w, t + 1 + h))

    for o, im in anh_icon.items():
        mh.alpha_composite(im, bc[o])
    CU = {"ItemEx": (523, 559), "AutoPlay": (738, 559), "HideChat": (647, 559), "Rec": (692, 508)}
    for o, (l, t) in CU.items():
        mh.alpha_composite(cat(l, t, 28, 28), bc[o])
    for i in range(9):
        mh.alpha_composite(cat(11 + 38 * i, 545, 36, 36), bc["Item_%d" % i])
    mh.alpha_composite(cat(372, 529, 36, 36), bc["ImediaLeftSkill"])
    mh.alpha_composite(cat(410, 529, 36, 36), bc["ImediaRightSkill"])
    mh.alpha_composite(cat(281, 522, 22, 22), bc["Face"])
    mh.alpha_composite(cat(304, 522, 22, 22), bc["SendBtn"])
    mh.alpha_composite(cat(1, 520, 20, 20), bc["ChannelBtn"])
    x, y, w, h = bc["InputEdit"]
    mh.paste(Image.new("RGBA", (w, h), (25, 31, 11, 200)), (x, y))
    # che cac icon cu con lai o thanh duoi cu (461..766, 559) bang nen
    mh.save(S + "mock_vnku.png")
    print("  anh dung thu:", S + "mock_vnku.png")


if __name__ == "__main__":
    main(mock=("mock" in sys.argv[1:]))
