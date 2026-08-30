# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] Chu: "di chuot vao trang bi khong hien thong tin thuoc
tinh". O trang bi dang la KWndImage (chi ve anh, khong co tooltip).
Client co san lop KMouseOver (Ui\Elem\MouseHover.h, g_MouseOver) - khuon
dung o UiPlayerBar.cpp:2429.

Lam:
- UiPet bat WM_MOUSEMOVE: xac dinh chuot dang o o trang bi nao (theo toa do
  10 o trong INI), roi SetMouseHoverInfo + SetMouseHoverTitle voi:
    <ten mon>  (doc tu bang item theo id o task 5143+i)
    3 dong thuoc tinh: <ten attrib> + <gia tri> (gia tri o task 5170+...)
- Ten attrib lay tu bang settings\petsys\attribname.txt (sinh tu
  KMagicAttrib.h + ten Viet), de client khong phai doc enum.
"""
import io
import os
import re
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

CR = chr(13)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"

# ---------- 1. bang ten thuoc tinh (chi cac ma dung trong 20 mon) ----------
ma_dung = set()
for d in io.open(SV + r"\settings\petsys\equipattrib.txt", "r", encoding="latin-1").read().replace(CR, "").split("\n")[1:]:
    c = d.split("\t")
    for x in c[1:4]:
        if x.strip().isdigit() and int(x) > 0:
            ma_dung.add(int(x))
s = io.open(r"D:\GAMEDEVNEW\Sources\Core\Src\KMagicAttrib.h", "r", encoding="latin-1").read()
i = s.find("enum")
names = re.findall(r"(magic_\w+)", s[i:s.find("};", i)])
TEN_VN = {
    "anti_block_rate": vn("Xuyên đỡ đòn"),
    "block_rate": vn("Tỷ lệ đỡ đòn"),
    "addskilldamage": vn("Sát thương chiêu"),
    "lifemax": vn("Sinh lực tối đa"),
    "manamax": vn("Nội lực tối đa"),
    "enhancehit_rate": vn("Chính xác"),
    "skill_enhance": vn("Uy lực kỹ năng"),
    "allres": vn("Kháng tất cả"),
    "hitrecover": vn("Hồi phục"),
    "damage": vn("Sát thương"),
    "defend": vn("Phòng ngự"),
    "speed": vn("Tốc độ"),
}
ra = ["Ma\tTen"]
for ma in sorted(ma_dung):
    ten = names[ma] if ma < len(names) else "attrib_%d" % ma
    goc = ten.replace("magic_", "")
    hien = None
    for k, v in TEN_VN.items():
        if k in goc:
            hien = v
            break
    if hien is None:
        hien = goc.replace("_", " ")
    ra.append("%d\t%s" % (ma, hien))
io.open(SV + r"\settings\petsys\attribname.txt", "w", encoding="latin-1",
        newline="").write(("\n".join(ra) + "\n").replace("\n", CR + "\n"))
# client can ban sao
io.open(CL + r"\settings\petsys\attribname.txt", "w", encoding="latin-1",
        newline="").write(("\n".join(ra) + "\n").replace("\n", CR + "\n")) if os.path.isdir(CL + r"\settings\petsys") else None
os.makedirs(CL + r"\settings\petsys", exist_ok=True)
io.open(CL + r"\settings\petsys\attribname.txt", "w", encoding="latin-1",
        newline="").write(("\n".join(ra) + "\n").replace("\n", CR + "\n"))
io.open(CL + r"\settings\petsys\equipattrib.txt", "w", encoding="latin-1", newline="").write(
    io.open(SV + r"\settings\petsys\equipattrib.txt", "r", encoding="latin-1", newline="").read())
print("1. attribname.txt:", len(ra) - 1, "ma | da chep sang client kem equipattrib")

# ---------- 2. UiPet: tooltip ----------
p = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
lf = io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")
if "sPetEquipTip" in lf:
    print("2. da co")
else:
    # ham dung chuoi tooltip
    neo = "void KUiPet::UpdateData()"
    them = """// [30/08] Tooltip o trang bi: ten mon + 3 thuoc tinh (ma tu
// settings\\petsys\\equipattrib.txt, ten tu attribname.txt, gia tri tu
// task 5170 + slot*3). Dung g_MouseOver nhu UiPlayerBar.cpp:2429.
static KTabFile s_AttNameTab;
static int      s_bAttNameLoaded = 0;

static void sPetAttName(int nMa, char* szOut, int nOutLen)
{
	szOut[0] = 0;
	if (!s_bAttNameLoaded)
	{
		s_bAttNameLoaded = 1;
		s_AttNameTab.Load((LPSTR)"\\\\settings\\\\petsys\\\\attribname.txt");
	}
	char szNum[16];
	int nRow = s_AttNameTab.GetHeight();
	for (int r = 2; r <= nRow; r++)
	{
		s_AttNameTab.GetString(r, 1, (LPSTR)"", szNum, sizeof(szNum));
		if (atoi(szNum) == nMa)
		{
			s_AttNameTab.GetString(r, 2, (LPSTR)"", szOut, nOutLen);
			return;
		}
	}
	_snprintf(szOut, nOutLen - 1, "thuoc tinh %d", nMa);
}

static void sPetEquipTip(int nSlot, char* szOut, int nOutLen)
{
	szOut[0] = 0;
	int nId = sPetTV(5143 + nSlot);
	if (nId < 4907 || nId > 4926)
		return;
	char szTen[64];
	szTen[0] = 0;
	sPetItemName(nId, szTen, sizeof(szTen));
	int nLen = _snprintf(szOut, nOutLen - 1, "%s", szTen);
	if (nLen < 0) nLen = 0;
	// 3 thuoc tinh
	static KTabFile s_EqAttTab;
	static int s_bLoaded = 0;
	if (!s_bLoaded)
	{
		s_bLoaded = 1;
		s_EqAttTab.Load((LPSTR)"\\\\settings\\\\petsys\\\\equipattrib.txt");
	}
	char szNum[16];
	int nRow = s_EqAttTab.GetHeight();
	for (int r = 2; r <= nRow; r++)
	{
		s_EqAttTab.GetString(r, 1, (LPSTR)"", szNum, sizeof(szNum));
		if (atoi(szNum) != nId) continue;
		for (int c = 0; c < 3; c++)
		{
			s_EqAttTab.GetString(r, 2 + c, (LPSTR)"", szNum, sizeof(szNum));
			int nMa = atoi(szNum);
			int nVal = sPetTV(5170 + nSlot * 3 + c);
			if (nMa > 0 && nVal > 0 && nLen < nOutLen - 40)
			{
				char szTenAtt[48];
				sPetAttName(nMa, szTenAtt, sizeof(szTenAtt));
				nLen += _snprintf(szOut + nLen, nOutLen - nLen - 1,
					"\\n%s +%d", szTenAtt, nVal);
			}
		}
		break;
	}
}

void KUiPet::UpdateData()"""
    lf = lf.replace(neo, them, 1)
    print("2a. them ham tooltip")
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
print("XONG p80 (phan bat chuot lam o buoc sau)")
