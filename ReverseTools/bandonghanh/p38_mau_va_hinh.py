# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] 2 fix sau nghiem thu dot 1:
A. MAU PET = 0: template pet (566 gubo...) la NPC thoai LifeMax=0 ->
   sPetSummon set LifeMax/CurrentLife theo ATTRIB Sinh luc (o 5122, dang 250).
B. HINH PET khong hien trong khung: spr npcres co offset tam-chan lon ->
   KWndImage ve bay ra ngoai control. Sinh spr TINH 1 frame offset (0,0),
   thu nho vua khung 120x200 (bo bot dong/cot theo buoc), dat
   \spr\Ui3\pet\face\<res>.spr; C sPetResPath doi sang path nay.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\bandonghanh")

CR = chr(13)
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"

# ---------- A. mau pet ----------
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
lf = s.replace(CR + "\n", "\n")
dau = "// [29/08] mau pet"
if dau in lf:
    print("A. da co")
else:
    cu = "\tpNpc->m_CurrentLife = pNpc->m_LifeMax;\n"
    moi = ("\t// [29/08] mau pet = ATTRIB Sinh luc (o 5122) - template 566.. la NPC\n"
           "\t// thoai LifeMax=0 nen thanh mau tren dau pet hien 0\n"
           "\t{\n"
           "\t\tint nHp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 4);\n"
           "\t\tif (nHp > 0)\n"
           "\t\t\tpNpc->m_LifeMax = nHp;\n"
           "\t\tint nMp = sPetG(nPlayerIdx, PET_TV_ATTRIB0 + 5);\n"
           "\t\tif (nMp > 0)\n"
           "\t\t\tpNpc->m_ManaMax = nMp;\n"
           "\t\tpNpc->m_CurrentMana = pNpc->m_ManaMax;\n"
           "\t}\n"
           "\tpNpc->m_CurrentLife = pNpc->m_LifeMax;\n")
    assert lf.count(cu) == 1, lf.count(cu)
    lf = lf.replace(cu, moi, 1)
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    print("A. VA mau pet theo attrib")

# ---------- B1. C doi path face ----------
p = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
lf = s.replace(CR + "\n", "\n")
if "pet\\\\face" in lf:
    print("B1. da co")
else:
    cu = ('    _snprintf(szOut, nOutLen - 1, "\\\\spr\\\\npcres\\\\%s\\\\%s\\\\%s_st01.spr",\n')
    i = lf.find('_snprintf(szOut')
    assert i > 0
    dong_dau = lf.rfind("\n", 0, i) + 1
    dong_cuoi = lf.find(";", i) + 1
    khoi_cu = lf[dong_dau:dong_cuoi]
    print("khoi cu:", repr(khoi_cu[:120]))
    khoi_moi = ('    // [29/08] dung spr TINH offset-0 sinh rieng cho khung (spr npcres\n'
                '    // goc co offset tam-chan lon -> KWndImage ve lech ra ngoai control)\n'
                '    _snprintf(szOut, nOutLen - 1, "\\\\spr\\\\Ui3\\\\pet\\\\face\\\\%s.spr", szRes);')
    lf = lf[:dong_dau] + khoi_moi + lf[dong_cuoi:]
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    print("B1. VA path face")

# ---------- B2. sinh spr face 21 res (API that cua p15: imgs=PIL, save->path) ----------
from p15_vietsub_spr import Spr  # noqa: E402
from PIL import Image  # noqa: E402

RES = ["enemy067", "boss001", "boss011", "boss015", "boss018", "boss005",
       "boss002", "boss008", "boss012", "boss024", "boss003", "boss019",
       "boss007", "boss023", "boss013", "boss017", "boss004", "boss037",
       "boss026", "boss132"]
ra_dir = os.path.join(CL, "spr", "Ui3", "pet", "face")
os.makedirs(ra_dir, exist_ok=True)
W, H = 120, 200


def nhom_of(res):
    i = 0
    while i < len(res) and not res[i].isdigit():
        i += 1
    return res[:i] if i else res


ok = thieu = 0
for res in RES:
    goc = os.path.join(CL, "spr", "npcres", nhom_of(res), res, res + "_st.spr")
    if not os.path.exists(goc):
        goc = os.path.join(CL, "spr", "npcres", nhom_of(res), res, res + "_st01.spr")
    if not os.path.exists(goc):
        print("thieu goc:", res)
        thieu += 1
        continue
    sp = Spr(goc)
    img = sp.imgs[0]
    bb = img.getbbox()
    if bb:
        img = img.crop(bb)
    ti_le = min(W / img.width, H / img.height, 1.0)
    if ti_le < 1.0:
        img = img.resize((max(1, int(img.width * ti_le)),
                          max(1, int(img.height * ti_le))), Image.LANCZOS)
    canvas = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    canvas.paste(img, ((W - img.width) // 2, max(0, H - img.height - 6)), img)
    sp.w, sp.h = W, H
    sp.frames = 1
    sp.dirs = 1
    sp.imgs = [canvas]
    sp.path = os.path.join(ra_dir, res + ".spr")
    if not os.path.exists(sp.path):
        open(sp.path, "wb").write(b"")  # save() doc path de backup
    if os.path.exists(sp.path + ".truoc_vietsub"):
        os.remove(sp.path + ".truoc_vietsub")
    sp.save()
    if os.path.exists(sp.path + ".truoc_vietsub"):
        os.remove(sp.path + ".truoc_vietsub")
    ok += 1
print("B2. sinh face:", ok, "thieu:", thieu)
print("XONG p38")
