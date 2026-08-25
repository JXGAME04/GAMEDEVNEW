# -*- coding: utf-8 -*-
r"""[25/08] VA "danh hieu Bang Chien khong hien tren nhan vat".

GOC (da lan tung buoc trong engine):
  Script Bang Chien dung HE DANH HIEU JX2: Title_AddTitle / Title_ActiveTitle
  (KJx2Title.cpp) - he nay CHI luu vao bang rieng trong DLL va ap skill phu tu
  bang settings\playertitle_jx2.txt. No KHONG HE dat Npc[].m_btPlayerTitle.
  Ma client ve danh hieu dua vao DUNG bien do:
      KNpc.cpp:5647  PlayerSync.PlayerTitle = (DWORD)m_btPlayerTitle
      KNpcRes.cpp:638 dung m_btPlayerTitle de tinh cho ve
  Duong dat bien do la HE JX1: SetPlayerTitle(id, time, overlook)
      -> ScriptFuns.cpp:10702 -> KNpc::SetCurPlayerTitle (KNpc.cpp:11006)
      -> m_btPlayerTitle = id, va doc bang settings\PlayerTitle.txt DONG = id + 1
         de lay ExtSkill1/ExtSkillLevel1 roi CastStateSkill (hieu ung di kem).
  => Hai he SONG SONG, KHONG noi voi nhau; danh hieu Bang Chien khong bao gio hien.

  Nang hon: id cua hai bang MANG NGHIA KHAC NHAU
      id 105: bang JX2 = "Vo Lam De Nhat Bang" | bang JX1 = "Hieu uy"
      id 199: bang JX2 = "Cao Cap De Nhat Bang"| bang JX1 = "Vu Lam Trung Lang Tuong"
  => KHONG duoc goi thang SetPlayerTitle(105) - se hien SAI TEN.

VA: them 9 dong MOI vao settings\PlayerTitle.txt (ca server lan client) mang dung ten
lay tu bang JX2, roi cau noi id JX2 -> id JX1 moi khi goi SetPlayerTitle.
Bang tra theo DONG = id + 1 nen phai them LIEN TIEP tu id 287.
"""
import io, os, shutil

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\settings"
ENC = "latin-1"
NL = "\r\n"

def rd(p): return io.open(p, "r", encoding=ENC, newline="").read()
def wr(p, s): io.open(p, "w", encoding=ENC, newline="").write(s)
def bak(p, t):
    q = p + ".truoc_" + t
    if not os.path.isfile(q): shutil.copyfile(p, q)

# ---- lay TEN nguyen byte tu bang JX2 ----
jx2 = rd(os.path.join(SRV, "playertitle_jx2.txt")).split(NL)
ten_jx2 = {}
for l in jx2[1:]:
    c = l.split("\t")
    if len(c) > 1:
        try: ten_jx2[int(c[1])] = c[0]
        except ValueError: pass

# id JX2 -> id JX1 moi (lien tiep tu 287)
MAP = [(100, 287), (101, 288), (102, 289), (103, 290), (104, 291),
       (105, 292), (106, 293), (199, 294), (3000, 295)]

for root, tag in ((SRV, "server"), (CLI, "client")):
    p = os.path.join(root, "PlayerTitle.txt")
    s = rd(p)
    lines = s.split(NL)
    while lines and lines[-1].strip() == "":
        lines.pop()
    ncol = len(lines[0].split("\t"))
    # da them chua?
    cur_max = 0
    for l in lines[1:]:
        c = l.split("\t")
        if len(c) > 1:
            try: cur_max = max(cur_max, int(c[1]))
            except ValueError: pass
    if cur_max >= 295:
        print("%-7s: da them truoc do (idmax=%d)" % (tag, cur_max)); continue
    assert cur_max == 286, "%s: idmax = %d (mong 286)" % (tag, cur_max)

    them = 0
    for jx2id, newid in MAP:
        ten = ten_jx2.get(jx2id, "")
        assert ten, "khong co ten cho id JX2 %d" % jx2id
        c = [""] * ncol
        c[0] = ten          # TitleName (nguyen byte TCVN3 tu bang JX2)
        c[1] = str(newid)   # TitleId
        for k in range(2, ncol):
            c[k] = "0"
        lines.append("\t".join(c))
        them += 1
    bak(p, "themdanhhieu_2508")
    wr(p, NL.join(lines) + NL)
    # kiem bat bien: dong (1-based) == id + 1
    chk = rd(p).split(NL)
    bad = 0
    for i, l in enumerate(chk):
        if i == 0 or not l.strip(): continue
        c = l.split("\t")
        if len(c) > 1:
            try:
                if int(c[1]) + 1 != i + 1: bad += 1
            except ValueError: pass
    print("%-7s: them %d dong (nay %d dong, idmax=295) | dong LECH bat bien: %d"
          % (tag, them, len([x for x in chk if x.strip()]), bad))

print("\nBAN DO id JX2 -> id JX1 moi:")
for a, b in MAP:
    print("   %-5d -> %-4d  %s" % (a, b, repr(ten_jx2.get(a, ""))[:44]))
print("XONG")
