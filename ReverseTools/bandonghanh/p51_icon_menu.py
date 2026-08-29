# -*- coding: utf-8 -*-
r"""[PETSYS 29/08 chieu] tiep p50:
1. 6 o trang bi pet dung ANH TRANG BI THAT cua JX1 (tra tu bang
   meleeweapon/helm/armor/belt/boot/amulet - dong dau moi bang).
2. Icon 18 skill 1670..1687 dang 36x36 (tran o) -> sinh ban 24x24 (bang
   icon aura dang vua o) vao \spr\Ui3\pet\extskill\, sua cot SkillIcon
   cua 18 dong (server+client) tro vao ban moi. KHONG dung icon goc de
   khong dung toi skill khac cung dung anh do.
3. Menu admin PET: bo muc thua, them muc can (200 thuoc, 5 thiep, xoa het
   ky nang, xoa het trang bi).
"""
import io
import os
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\bandonghanh")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")

CR = chr(13)
BS = chr(92)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---------- 1. anh trang bi ----------
def anh_dau(ten_bang):
    p = os.path.join(SV, "settings", "item", ten_bang)
    for d in doc_lf(p).split("\n")[1:]:
        c = d.split("\t")
        if len(c) > 4 and c[4].strip().lower().endswith(".spr"):
            return c[4].strip()
    return None


ANH6 = [anh_dau("meleeweapon.txt"), anh_dau("helm.txt"), anh_dau("armor.txt"),
        anh_dau("belt.txt"), anh_dau("boot.txt"), anh_dau("amulet.txt")]
assert all(ANH6), ANH6
for pth in (SV + r"\settings\item\magicscript.txt", CL + r"\settings\item\magicscript.txt"):
    rows = [d.split("\t") for d in doc_lf(pth).split("\n") if d.strip()]
    n = 0
    for c in rows:
        if len(c) > 4 and c[3] in [str(x) for x in range(4881, 4887)]:
            c[4] = ANH6[int(c[3]) - 4881]
            n += 1
    ghi_crlf(pth, "\n".join("\t".join(c) for c in rows) + "\n")
    print("1. anh trang bi", n, "dong:", pth[:2])
print("   ", [a.split(BS)[-1] for a in ANH6])

# ---------- 2. icon 24x24 ----------
import p15_vietsub_spr as p15  # noqa: E402
from PIL import Image  # noqa: E402

ra_dir = os.path.join(CL, "spr", "Ui3", "pet", "extskill")
os.makedirs(ra_dir, exist_ok=True)
rows_sv = [d.split("\t") for d in doc_lf(SV + r"\settings\skills.txt").split("\n") if d.strip()]
doi = {}
for c in rows_sv:
    if len(c) > 5 and c[2].strip().isdigit() and 1670 <= int(c[2]) <= 1687:
        goc = c[5].strip()
        ten = goc.split(BS)[-1]
        src = CL + goc
        if not os.path.exists(src):
            print("   !! thieu icon goc:", ten)
            continue
        sp = p15.Spr(src)
        im = sp.imgs[0]
        bb = im.getbbox()
        if bb:
            im = im.crop(bb)
        im = im.resize((24, 24), Image.LANCZOS)
        cv = Image.new("RGBA", (24, 24), (0, 0, 0, 0))
        cv.paste(im, (0, 0), im)
        sp.w = sp.h = 24
        sp.frames = 1
        sp.dirs = 1
        sp.imgs = [cv]
        sp.path = os.path.join(ra_dir, ten)
        if not os.path.exists(sp.path):
            open(sp.path, "wb").write(b"")
        bak = sp.path + ".truoc_vietsub"
        if os.path.exists(bak):
            os.remove(bak)
        sp.save()
        if os.path.exists(bak):
            os.remove(bak)
        doi[goc] = BS + "spr" + BS + "Ui3" + BS + "pet" + BS + "extskill" + BS + ten
print("2. sinh", len(doi), "icon 24x24")
for pth in (SV + r"\settings\skills.txt", CL + r"\settings\skills.txt"):
    rows = [d.split("\t") for d in doc_lf(pth).split("\n") if d.strip()]
    n = 0
    for c in rows:
        if len(c) > 5 and c[2].strip().isdigit() and 1670 <= int(c[2]) <= 1687:
            if c[5].strip() in doi:
                c[5] = doi[c[5].strip()]
                n += 1
    ghi_crlf(pth, "\n".join("\t".join(c) for c in rows) + "\n")
    print("2. skills.txt tro icon moi:", n, "dong:", pth[:2])

# ---------- 3. menu admin gon ----------
p = SV + r"\script\item\bdh_admin.lua"
lf = doc_lf(p)
i = lf.find("function BDH_P_Root()")
j = lf.find("\nend", i)
assert i > 0 and j > i
moi = '''function BDH_P_Root()
	SayEx({format("Pet: co=%d goi=%d cap=%d | diem %d/%d/%d/%d | xu %d",
		PET_IsCreate(), PET_IsSummon(), PET_GetLevel(),
		PET_GetUpgradePoint(), PET_GetGrownPoint(), PET_GetTamePoint(),
		PET_GetXiuzhenPoint(), GetTask(251)),
	"Cap 5 Thiep + 200 Thuoc + 20 moi loai trai/BDH_P_CapDo",
	"Cap 5 Bi kip + 6 trang bi/BDH_P_CapKN",
	"Bon diem = 9999 + 2000 chan nguyen + 500 xu/BDH_P_Diem",
	"Tao pet nhanh (khong can thiep)/BDH_P_TaoNhanh",
	"Xoa het ky nang da hoc/BDH_P_XoaKN",
	"Thao het trang bi/BDH_P_XoaTB",
	"Dong bo diem xuong client/BDH_P_DongBo",
	"Ket thuc doi thoai./no"})'''
lf = lf[:i] + moi + lf[j:]
# gop cac ham cap do
them = '''
function BDH_P_CapKN()
	local i
	for i = 1, 5 do
		AddItem(6, 1, 4880, 1, 0, 0)
	end
	for i = 4881, 4886 do
		AddItem(6, 1, i, 1, 0, 0)
	end
	Msg2Player("Da cap 5 Bi kip + 6 trang bi")
end

function BDH_P_XoaKN()
	local i
	for i = 0, 3 do
		SetTask(5139 + i, 0)
	end
	Msg2Player("Da xoa het ky nang pet")
end

function BDH_P_XoaTB()
	local i
	for i = 0, 5 do
		SetTask(5143 + i, 0)
		SetTask(5150 + i, 0)
	end
	SetTask(5157, 0)
	SetTask(5158, 0)
	Msg2Player("Da thao het trang bi pet")
end
'''
if "BDH_P_CapKN" not in lf:
    neo = "function BDH_P_CapDo()"
    assert lf.count(neo) == 1
    lf = lf.replace(neo, them.lstrip("\n") + "\n" + neo, 1)
# CapDo: 5 thiep + 200 thuoc + 20 moi trai
i2 = lf.find("function BDH_P_CapDo()")
j2 = lf.find("\nend", i2)
moi2 = '''function BDH_P_CapDo()
	local i
	for i = 1, 5 do
		AddItem(6, 1, 4874, 1, 0, 0)
	end
	for i = 1, 200 do
		AddItem(6, 1, 4875, 1, 0, 0)
	end
	for i = 1, 20 do
		AddItem(6, 1, 4876, 1, 0, 0)
		AddItem(6, 1, 4877, 1, 0, 0)
		AddItem(6, 1, 4878, 1, 0, 0)
		AddItem(6, 1, 4879, 1, 0, 0)
	end
	Msg2Player("Da cap 5 Thiep + 200 Thuoc + 20 moi loai trai")'''
lf = lf[:i2] + moi2 + lf[j2:]
# Diem: them chan nguyen + xu
i3 = lf.find("function BDH_P_Diem()")
j3 = lf.find("\nend", i3)
moi3 = '''function BDH_P_Diem()
	PET_SetUpgradePoint(9999)
	PET_SetGrownPoint(9999)
	PET_SetTamePoint(9999)
	PET_SetXiuzhenPoint(9999)
	SetTask(362, GetTask(362) + 2000)
	SetTask(251, GetTask(251) + 500)
	Msg2Player("Da cap 4 diem 9999 + 2000 chan nguyen + 500 xu")'''
if i3 > 0:
    lf = lf[:i3] + moi3 + lf[j3:]
ghi_crlf(p, lf)
print("3. menu admin gon lai")
print("XONG p51")
