# -*- coding: utf-8 -*-
r"""[25/08] Sua muc 9 lenh bai admin cho KHOP SU THAT sau khi bat vong sang.

Truoc: thong bao ghi "vong sang ky nang 661 cap (rank-1)" - SAI.
  661 (StateSpecialId 67 -> \spr\skill\others\lucky.spr, cot 3 = "Head") la hieu ung
  MAY MAN tren DAU, khong phai vong sang duoi chan. Ban Linux van goi 661 (match\head.lua:432)
  nen GIU LAI de dung 100% Linux, chi sua loi MO TA.
Vong sang THAT den tu bang settings\PlayerTitle.txt cot ExtSkill1 (KNpc.cpp:11016-11023):
  quan ham 1..5 -> skill 830..834 (Status136..140, cot "Foot") cap 2/4/6/8/10.

Sua: (1) chu thich, (2) thong bao ghi dung so skill + cap, (3) go het thi go ca vong sang.
Moi chuoi thay the deu la ASCII THUAN => khong dung toi byte tieng Viet TCVN3.
"""
import io, os, shutil

P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\hoatdong_admin.lua"
ENC = "latin-1"
s = io.open(P, "r", encoding=ENC, newline="").read()
NL = "\r\n" if "\r\n" in s else "\n"
goc = s

# --- 1) chu thich: noi ro 661 la gi va vong sang that o dau ---
old = "--   vong sang quan ham = skill 661 cap (rank-1)          (match\\head.lua:432)"
if old in s:
    s = s.replace(old,
        "--   skill 661 cap (rank-1) = hieu ung MAY MAN tren DAU (StateSpecialId 67 ->" + NL +
        "--     \\spr\\skill\\others\\lucky.spr, cot 3 = \"Head\"), KHONG phai vong sang." + NL +
        "--     Ban Linux van goi (match\\head.lua:432) nen GIU de dung 100% Linux." + NL +
        "--   VONG SANG THAT: bang settings\\PlayerTitle.txt cot ExtSkill1, engine tu cast" + NL +
        "--     trong KNpc::SetCurPlayerTitle (KNpc.cpp:11016-11023). Dong 288-292 (id 287-291)" + NL +
        "--     = skill 830..834 cap 2/4/6/8/10 (Status136..140, cot \"Foot\" = duoi chan);" + NL +
        "--     dong 293-296 (id 292-295) = skill 1169 cap 1 (Status81, giong id 286 THDNB)." + NL +
        "--     => chi can SetPlayerTitle la vong sang TU HIEN, khong phai cast tay.", 1)
    print("1: da sua chu thich 661")
else:
    print("1: chu thich da sua truoc do")

# --- 2) thong bao ghi DUNG so skill vong sang + dung cap ---
if "ng 661 c" in s:
    s = s.replace("ng 661 c", "ng \".. (829 + nRank) ..\" c", 1)
    s = s.replace(".. (nRank - 1) .. \")\")", ".. (nRank * 2) .. \")\")", 1)
    print("2: thong bao gio ghi skill 830..834 va cap 2/4/6/8/10")
else:
    print("2: thong bao da sua truoc do")

# --- 3) go het: go ca vong sang that ---
old3 = "\tRemoveSkillState(661)" + NL + "\tRemoveSkillState(1485)" + NL
if old3 in s:
    s = s.replace(old3,
        "\tRemoveSkillState(661)" + NL +
        "\tRemoveSkillState(1485)" + NL +
        "\t-- [FIX 25/08] go ca VONG SANG that (ExtSkill1 cua 9 dong PlayerTitle.txt moi)." + NL +
        "\t-- RemovePlayerTitle o tren da goi CastStateSkill overlook=1 nen thuong da go," + NL +
        "\t-- go lai cho chac vi menu co the cap nhieu danh hieu lien tiep." + NL +
        "\tlocal tbAura = {830, 831, 832, 833, 834, 1169}" + NL +
        "\tfor i = 1, getn(tbAura) do" + NL +
        "\t\tRemoveSkillState(tbAura[i])" + NL +
        "\tend" + NL, 1)
    print("3: go het da go ca vong sang 830-834 + 1169")
else:
    print("3: phan go het da sua truoc do")

if s != goc:
    q = P + ".truoc_vongsang_2508"
    if not os.path.isfile(q): shutil.copyfile(P, q)
    io.open(P, "w", encoding=ENC, newline="").write(s)
    print("DA GHI " + P)
else:
    print("khong co gi thay doi")
