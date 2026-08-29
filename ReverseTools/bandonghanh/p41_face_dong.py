# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] Chinh vi tri + pet DONG trong khung:
1. INI: AppearanceImg -> o den trai that (40,303 260x120); Skill_1 -> o dau
   day ky nang (378,170 34x34); Ext_Skill_1..4 -> 4 o sau (buoc 38).
2. Sinh face DA FRAME: lay HUONG CHINH DIEN (huong 4/8) cua <res>_st.spr,
   crop bbox CHUNG + scale chung (khong rung), canvas 260x120 giua-day.
3. UiPet: override Breathe() -> NextFrame() ~180ms (framework tu goi Breathe
   cho moi cua so visible - WndWindow.cpp:367).
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\bandonghanh")

CR = chr(13)
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"

# ---------- 1. INI ----------
p = CL + r"\Ui\Ui3\pet_main.ini"
s = io.open(p, "r", encoding="latin-1", newline="").read()


def doi_sec(s, sec, l, t, w, h):
    i = s.find("[" + sec + "]")
    assert i >= 0, sec
    j = s.find("[", i + 1)
    khoi = s[i:j]
    moi = khoi
    import re
    moi = re.sub(r"Left=\d+", "Left=%d" % l, moi, 1)
    moi = re.sub(r"Top=\d+", "Top=%d" % t, moi, 1)
    if "Width=" in moi:
        moi = re.sub(r"Width=\d+", "Width=%d" % w, moi, 1)
    if "Height=" in moi:
        moi = re.sub(r"Height=\d+", "Height=%d" % h, moi, 1)
    return s.replace(khoi, moi, 1)


s = doi_sec(s, "AppearanceImg", 40, 303, 260, 120)
s = doi_sec(s, "Skill_1", 378, 170, 34, 34)
for k in range(4):
    s = doi_sec(s, "Ext_Skill_%d" % (k + 1), 416 + 38 * k, 170, 34, 34)
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("1. INI reposition xong")

# ---------- 2. face da frame ----------
from p15_vietsub_spr import Spr  # noqa: E402
from PIL import Image  # noqa: E402

RES = ["enemy067", "boss001", "boss011", "boss015", "boss018", "boss005",
       "boss002", "boss008", "boss012", "boss024", "boss003", "boss019",
       "boss007", "boss023", "boss013", "boss017", "boss004", "boss026",
       "boss132"]
ra_dir = os.path.join(CL, "spr", "Ui3", "pet", "face")
W, H = 260, 120


def nhom_of(res):
    i = 0
    while i < len(res) and not res[i].isdigit():
        i += 1
    return res[:i] if i else res


ok = 0
for res in RES:
    goc = os.path.join(CL, "spr", "npcres", nhom_of(res), res, res + "_st.spr")
    if not os.path.exists(goc):
        print("thieu goc:", res)
        continue
    sp = Spr(goc)
    nD = sp.dirs if sp.dirs > 0 else 8
    nF = sp.frames // nD if nD else sp.frames
    if nF <= 0:
        nF = sp.frames
        nD = 1
    huong = 0  # [do dai 8 huong] 0 = mat truoc; 4 = lung
    khung = sp.imgs[huong * nF:(huong + 1) * nF]
    if not khung:
        khung = sp.imgs[:1]
    # bbox chung
    x0 = y0 = 1 << 30
    x1 = y1 = -1
    for im in khung:
        bb = im.getbbox()
        if bb:
            x0 = min(x0, bb[0]); y0 = min(y0, bb[1])
            x1 = max(x1, bb[2]); y1 = max(y1, bb[3])
    if x1 <= x0:
        continue
    ti_le = min(W / (x1 - x0), H / (y1 - y0), 1.0)
    canvases = []
    for im in khung:
        c = im.crop((x0, y0, x1, y1))
        if ti_le < 1.0:
            c = c.resize((max(1, int(c.width * ti_le)), max(1, int(c.height * ti_le))), Image.LANCZOS)
        cv = Image.new("RGBA", (W, H), (0, 0, 0, 0))
        cv.paste(c, ((W - c.width) // 2, max(0, H - c.height - 2)), c)
        canvases.append(cv)
    sp.w, sp.h = W, H
    sp.frames = len(canvases)
    sp.dirs = 1
    if sp.itv <= 0 or sp.itv > 1000:
        sp.itv = 150
    sp.imgs = canvases
    sp.path = os.path.join(ra_dir, res + ".spr")
    if not os.path.exists(sp.path):
        open(sp.path, "wb").write(b"")
    if os.path.exists(sp.path + ".truoc_vietsub"):
        os.remove(sp.path + ".truoc_vietsub")
    sp.save()
    if os.path.exists(sp.path + ".truoc_vietsub"):
        os.remove(sp.path + ".truoc_vietsub")
    ok += 1
print("2. face dong:", ok, "res")

# ---------- 3. UiPet Breathe ----------
ph = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.h"
s = io.open(ph, "r", encoding="latin-1", newline="").read()
lf = s.replace(CR + "\n", "\n")
if "Breathe" not in lf:
    cu = "    void UpdateData();"
    assert lf.count(cu) == 1
    lf = lf.replace(cu, cu + "\n    virtual void Breathe();\t// [29/08] quay frame hinh pet", 1)
    io.open(ph, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    print("3a. UiPet.h + Breathe")
else:
    print("3a. da co")

pc = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
s = io.open(pc, "r", encoding="latin-1", newline="").read()
lf = s.replace(CR + "\n", "\n")
if "KUiPet::Breathe" not in lf:
    cu = "void KUiPet::SendOp(int nOp)"
    them = (
        "// [29/08] framework goi Breathe() cho cua so visible (WndWindow.cpp:367)\n"
        "// -> quay frame hinh pet ~180ms de dung tho nhu ngoai map\n"
        "void KUiPet::Breathe()\n"
        "{\n"
        "    static unsigned int s_uLast = 0;\n"
        "    unsigned int uNow = IR_GetCurrentTime();\n"
        "    if (uNow - s_uLast < 180)\n"
        "        return;\n"
        "    s_uLast = uNow;\n"
        "    if (m_Appearance.IsVisible())\n"
        "        m_Appearance.NextFrame();\n"
        "}\n\n")
    assert lf.count(cu) == 1
    lf = lf.replace(cu, them + cu, 1)
    io.open(pc, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    print("3b. UiPet.cpp + Breathe")
else:
    print("3b. da co")
print("XONG p41")
