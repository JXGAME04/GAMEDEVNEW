# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] Anh trang bi HIEN SAI trong 10 o - do dung:
o task 5143..5152 CON GIU id cua he TU CHE cu (4881..4887) ma nhung id do
tren JX1 la item khac (Tinh Ngoc / Thien Tinh Ngoc / Vuong Thiet Tuong
Lenh Phu...) -> tra bang ra dung anh cua chung.
Anh trong file la DUNG (da xuat PNG kiem: 10 icon Bich Huyet).
VA:
1. C (UiPet): chi ve khi id nam trong DAI trang bi pet 4907..4926,
   ngoai dai coi nhu o trong (chan rac cu + rac tuong lai).
2. Server: khi doc o, bo qua id ngoai dai (PETEQUIP_DEF khong co).
3. Admin: them lai muc "Thao het trang bi" de don o rac.
"""
import io

CR = chr(13)
BS = chr(92)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---- 1. UiPet loc dai ----
p = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
lf = doc_lf(p)
cu = """        int nP = sPetTV(5143 + i);
        char szImg[128];
        szImg[0] = 0;
        if (nP > 0)
            sPetItemImg(nP, szImg, sizeof(szImg));"""
moi = """        int nP = sPetTV(5143 + i);
        char szImg[128];
        szImg[0] = 0;
        // [30/08] CHI ve id trong DAI trang bi Dong Hanh (4907..4926 - bang
        // goc VLTK da port). O con giu id he tu che cu (4881..) thi bo qua,
        // neu khong se ve nham anh cua item JX1 khac trung id.
        if (nP >= 4907 && nP <= 4926)
            sPetItemImg(nP, szImg, sizeof(szImg));"""
if "CHI ve id trong DAI" in lf:
    print("1. da co")
else:
    assert lf.count(cu) == 1, lf.count(cu)
    ghi_crlf(p, lf.replace(cu, moi, 1))
    print("1. UiPet loc dai 4907..4926")

# ---- 2. admin: them muc thao het + don rac ----
p = SV + r"\script\item\bdh_admin.lua"
lf = doc_lf(p)
if "BDH_P_XoaTB" not in lf.split("function BDH_P_Root")[1].split("end")[0]:
    i = lf.find('\t"Dong bo client/BDH_P_DongBo",')
    assert i > 0
    lf = lf[:i] + '\t"Thao het trang bi/BDH_P_XoaTB",\n' + lf[i:]
    print("2a. them muc menu thao trang bi")
# ham XoaTB: don ca o rac
i = lf.find("function BDH_P_XoaTB()")
if i > 0:
    j = lf.find("\nend", i)
    moi2 = '''function BDH_P_XoaTB()
	local i
	for i = 0, 9 do
		SetTask(5143 + i, 0)
	end
	SetTask(5163, 0)
	Msg2Player("Da thao het trang bi pet (don ca o rac he cu)")'''
    lf = lf[:i] + moi2 + lf[j:]
else:
    them = '''function BDH_P_XoaTB()
	local i
	for i = 0, 9 do
		SetTask(5143 + i, 0)
	end
	SetTask(5163, 0)
	Msg2Player("Da thao het trang bi pet (don ca o rac he cu)")
end

'''
    neo = "function BDH_P_CapKN()"
    lf = lf.replace(neo, them + neo, 1)
ghi_crlf(p, lf)
print("2b. ham BDH_P_XoaTB don 10 o + 5163")

# ---- 3. petequip.lua: bo qua id ngoai bang ----
p = SV + r"\script\petsys\petequip.lua"
lf = doc_lf(p)
if "-- bo qua id ngoai bang" not in lf:
    lf = lf.replace(
        "\t\tlocal nId = GetTask(PETEQUIP_O_DAU + i)\n\t\tlocal tb = PETEQUIP_DEF[nId]\n",
        "\t\tlocal nId = GetTask(PETEQUIP_O_DAU + i)\n\t\tlocal tb = PETEQUIP_DEF[nId]\t-- bo qua id ngoai bang\n", 1)
    ghi_crlf(p, lf)
    print("3. petequip.lua ghi chu bo qua id la")
print("XONG p66")
