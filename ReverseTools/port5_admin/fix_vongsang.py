# -*- coding: utf-8 -*-
r"""[25/08] BAT VONG SANG cho 9 danh hieu Bang Chien (dong id 287-295).

GOC: hom qua them 9 dong vao settings\PlayerTitle.txt nhung de MOI cot phu = 0,
trong do co ExtSkill1/ExtSkillLevel1 - DUONG DUY NHAT engine tao vong sang:
    KNpc::SetCurPlayerTitle (KNpc.cpp:11016-11023)
        pTab.GetInteger(id+1, "ExtSkill1"/"ExtSkillLevel1")
        -> g_SkillManager.GetSkill(...)->CastStateSkill(...)
Skill do co StateSpecialId, tra bang \settings\npcres\state_magic_table_name.txt
DONG = StateSpecialId + 1 -> file .spr ve duoi chan (cot 3 = "Foot").

KHONG THIEU ANH: chinh bang nay da co 15 dong khac dung san cac skill do.
Quy uoc SAN CO cua ban nay (chep y nguyen, khong tu che):
    quan ham chien truong 5 cap (id 198-202) : 830/831/832/833/834  level 2/4/6/8/10
      830 -> Status136 title_new_zw.spr  "Tong Kim Chieu Vo Hieu uy"
      831 -> Status137 title_new_yl.spr  "Tong Kim Vu Lam Trung Lang Tuong"
      832 -> Status138 title_new_bj.spr  "Tong Kim Phieu Ky Dai Tuong Quan"
      833 -> Status139 title_new_ab.spr  "Tong Kim An Bang Dai Tuong Quan"
      834 -> Status140 title_new_dg.spr  "Tong Kim Dinh Quoc Nguyen Soai"
    danh hieu bang/giai dau (id 286 "Quan Quan THDNB") : 1169 level 1
      1169 -> Status81  title_yl.spr     "Vong tron Vo Lam Dai Hoi 1"

CACH LAM: chep NGUYEN dong mau (moi cot tru TitleName/TitleId) => chac chan nhat quan.
"""
import io, os, shutil

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\settings"
ENC = "latin-1"
NL = "\r\n"

# id moi -> id dong MAU de chep cac cot phu
MAU = {287: 198, 288: 199, 289: 200, 290: 201, 291: 202,
       292: 286, 293: 286, 294: 286, 295: 286}

def rd(p): return io.open(p, "r", encoding=ENC, newline="").read()
def wr(p, s): io.open(p, "w", encoding=ENC, newline="").write(s)

for root, tag in ((SRV, "server"), (CLI, "client")):
    p = os.path.join(root, "PlayerTitle.txt")
    s = rd(p)
    lines = s.split(NL)
    while lines and lines[-1].strip() == "":
        lines.pop()
    head = lines[0].split("\t")
    iName, iId = head.index("TitleName"), head.index("TitleId")
    iSk, iLv = head.index("ExtSkill1"), head.index("ExtSkillLevel1")

    # ban do id -> chi so dong
    pos = {}
    for i, l in enumerate(lines):
        if i == 0: continue
        c = l.split("\t")
        if len(c) > iId:
            try: pos[int(c[iId])] = i
            except ValueError: pass

    doi = 0
    for newid, srcid in sorted(MAU.items()):
        if newid not in pos or srcid not in pos:
            print("%-7s: THIEU id %d hoac mau %d - BO QUA" % (tag, newid, srcid)); continue
        cur = lines[pos[newid]].split("\t")
        src = lines[pos[srcid]].split("\t")
        if (cur[iSk] or "0") != "0":
            continue                      # da bat truoc do
        ten, idv = cur[iName], cur[iId]   # GIU ten + id cua dong moi
        moi = list(src)
        while len(moi) < len(head): moi.append("0")
        moi[iName], moi[iId] = ten, idv
        lines[pos[newid]] = "\t".join(moi)
        doi += 1

    if doi == 0:
        print("%-7s: khong co gi de doi (da bat truoc do)" % tag); continue

    q = p + ".truoc_vongsang_2508"
    if not os.path.isfile(q): shutil.copyfile(p, q)
    wr(p, NL.join(lines) + NL)

    # kiem lai: bat bien dong = id + 1, va 9 dong deu co skill
    chk = rd(p).split(NL)
    bad = 0; ok = 0
    for i, l in enumerate(chk):
        if i == 0 or not l.strip(): continue
        c = l.split("\t")
        if len(c) > iId:
            try:
                v = int(c[iId])
                if v + 1 != i + 1: bad += 1
                if 287 <= v <= 295 and (c[iSk] or "0") != "0": ok += 1
            except ValueError: pass
    print("%-7s: bat vong sang cho %d dong | 287-295 co skill: %d/9 | dong LECH bat bien: %d"
          % (tag, doi, ok, bad))

print("\nBANG CUOI (doc lai tu server):")
L = rd(os.path.join(SRV, "PlayerTitle.txt")).split(NL)
h = L[0].split("\t"); iId = h.index("TitleId"); iSk = h.index("ExtSkill1"); iLv = h.index("ExtSkillLevel1")
for l in L[1:]:
    c = l.split("\t")
    if len(c) > iLv:
        try: v = int(c[iId])
        except ValueError: continue
        if 287 <= v <= 295:
            print("   id=%-4s ExtSkill1=%-5s level=%-3s" % (c[iId], c[iSk], c[iLv]))
print("XONG")
