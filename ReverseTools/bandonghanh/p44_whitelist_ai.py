# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] (a) whitelist \script\petsys\ vao sIsJx2ItemScript -> item
pet tu tru khi dung (ngu nghia Linux: main tra ~=1 la engine tru 1);
(b) AI het "toan nhay": tick 0.5s + keo-ve nang nguong 1500."""
import io

CR = chr(13)
BS = chr(92)

p = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemList.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
lf = s.replace(CR + "\n", "\n")
vung = lf[lf.find("szJx2[] = {"):lf.find("szJx2[] = {") + 1200]
if "petsys" in vung:
    print("1. da co")
else:
    cu = "\t\t" + '"' + BS*2 + "script" + BS*2 + "item" + BS*2 + "messenger" + BS*2 + '",'
    moi = ("\t\t" + '"' + BS*2 + "script" + BS*2 + "petsys" + BS*2 + '",'
           + "\t\t\t\t\t\t// [PET 29/08] Thiep/trai/Bi kip - tra ~=1 la engine tru 1 (Linux)\n" + cu)
    assert lf.count(cu) == 1, lf.count(cu)
    lf = lf.replace(cu, moi, 1)
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    print("1. whitelist petsys")

p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
lf = s.replace(CR + "\n", "\n")
sua = 0
if "m_AIMAXTime = 9" not in lf:
    cu = "\tpNpc->m_RunSpeed = pOwner->m_RunSpeed * 5 / 4;\n"
    moi = cu + "\t// tick AI ~0.5s (template npc thoai tick cham -> canh 'toan nhay' thay vi chay)\n\tpNpc->m_AIMAXTime = 9;\n"
    assert lf.count(cu) == 1, lf.count(cu)
    lf = lf.replace(cu, moi, 1)
    sua += 1
if "nDis > 800" in lf:
    lf = lf.replace("\tif (nDis > 800)\n", "\tif (nDis > 1500)\n", 1)
    lf = lf.replace("// [29/08] bo xa 25 o la keo ve ngay",
                    "// [29/08] bo xa ~47 o moi keo ve; gan hon thi CHAY (do_run)", 1)
    sua += 1
if sua:
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
print("2. AI sua", sua, "cho")
print("XONG p44")
