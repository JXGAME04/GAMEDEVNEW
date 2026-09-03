# -*- coding: utf-8 -*-
"""vhtd_engine_patch10.py [VHTD 02/09q] - 6 loi "danh hut" do 6 tac tu doc log VH-* (02/09 16:00) tim ra, bang chung trong
scratchpad\\logs\\server_1443.log. TAT CA deu chi ap cho dan/ky nang THOI VLTK (kieu dan >= 400 qua VhMissleType, ky nang >= 1347) -
chieu co dien JX1 khong doi mot bit nao.

 A KMissle::CheckBeyondRegion  O DAN BI TRE: chuan hoa offset -> o chi lui/tien DUNG MOT o moi khung (if/else if). Dan VLTK nhanh
   (528 = 55 px/khung, 529 = 45, 580 = 60, 641 = 80) vuot 1 o (32 px) moi khung -> du ~0.7 o/khung, sau 6-7 khung m_nCurrentMapX/Y
   TRE 3-6 o so voi vi tri that. Map2Mps van dung (nen mps/dcell trong log nhin binh thuong) NHUNG CheckCollision/ProcessCollision
   quet quanh O CU -> quet phia SAU dan. Do: 1969 phong 103, trung 31; 48/103 (47 %) truot dung kieu nay; offset chua chuan hoa
   len toi 167936 (5 o). Vá: doi if/else if -> while (chi dan VLTK).
 B KMissleSet::Add             O DAN TAI SU DUNG GIU NGUYEN m_ulNextCalDamageTime cua dan truoc (chi dat 0 o constructor; Release
   phia server khong dat lai) -> dan moi sinh trong vong DmgInterval (17 khung = 0,94 s voi 645/646/647; 21 khung voi 527) bi
   "khoa sat thuong" ngay tu dau -> phong 4-5 lan moi trung 1 lan (dung mo ta cua chu). Vá: dat lai 0 (ca m_nHitCount) o CA HAI
   ban Add - day la khoi tao, khong doi hanh vi chieu cu.
 C KMissle::CheckCollision     nhanh VLTK goi FindNpc KHONG truyen muc tieu -> KRegion::FindNpc tra CON DAU TIEN cua o; neu con do
   la XAC thi `continue` BO LUON CA O -> muc tieu con song dung chung o voi xac khong bao gio duoc xet (do: 1969 ban 4 lan lien
   vao con 30000 mau dung tren xac, VH-COL-NONE ca 4). Vá: truyen muc tieu dang bam (hoac MAX_NPC lam moc) -> FindNpc dung nhanh
   fallback co san: uu tien muc tieu, va CON SONG hon XAC.
 D KMissle::ProcessCollision   HOI QUY BAN VA DOT 7 CUA TOI: `nRangeX = VLTK ? nRange : nRange/2` ap ca cho loi goi nRange = 1 tu
   CheckCollision (danh DUNG mot NPC) -> quet 3x3 thay vi 1 o -> dan don muc tieu danh lan con ben canh. Vá: chi bo /2 khi nRange > 1.
 E KMissle::ProcessCollision   dem XAC vao nRet/m_nHitCount (va goi ProcessDamage len xac) -> dan gioi han so muc tieu (m_nHitCount)
   het luot vi xac. Vá: dan VLTK bo qua do_death/do_revive.
 F KNpc::AttackSkill/ReplySkill/CastAutoSkillAt  auto-skill (1364 -> 1363, 1369 -> 1368) chon muc tieu chi loai do_death/do_revive,
   KHONG loai m_CurrentLife <= 0 - nan nhan vua bi don cuoi ve 0 mau (chua kip doi doing) van duoc chon -> ca loat kiem bay vao xac
   (do: 130/234 kiem 1368 bay het doi ma khong trung ai). Vá: loai luon muc tieu het mau.
Marker [VHTD 02/09q]. DUNG: python vhtd_engine_patch10.py [--kiem]
"""
import io, os, sys, re

KIEM = "--kiem" in sys.argv
CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
MK = "[VHTD 02/09q]"
T = "\t"

class F:
    def __init__(self, name):
        self.path = os.path.join(CORE, name)
        b = io.open(self.path, "rb").read()
        assert b[:3] != b"\xef\xbb\xbf", "BOM: " + self.path
        self.s = b.decode("latin-1"); self.orig = self.s; self.n = 0
        self.nl = "\r\n" if self.s.count("\r\n") * 2 > self.s.count("\n") else "\n"
    def NL(self, x): return x.replace("\r\n", self.nl) if self.nl != "\r\n" else x
    def rep(self, old, new, tag, count=1):
        old = self.NL(old); new = self.NL(new)
        if tag in self.s:
            print("  [=] %s: %s" % (os.path.basename(self.path), tag)); return
        c = self.s.count(old)
        if c != count:
            raise SystemExit("neo %s: %d lan (can %d) trong %s\n%r" % (tag, c, count, self.path, old[:200]))
        self.s = self.s.replace(old, new); self.n += 1
        print("  [+] %s: %s" % (os.path.basename(self.path), tag))
    def save(self):
        if self.s == self.orig: return
        assert "\xef\xbf\xbd" not in self.s
        assert sum(1 for c in self.s if ord(c) >= 0x80) == sum(1 for c in self.orig if ord(c) >= 0x80), "byte cao doi " + self.path
        if not KIEM: io.open(self.path, "wb").write(self.s.encode("latin-1"))
        print("  => %s %s (%d hunk)" % ("KIEM" if KIEM else "ghi", self.path, self.n))

NL = "\r\n"

def h_kmissle():
    f = F("KMissle.cpp")
    # ---- A: CheckBeyondRegion
    old = (T + "if (nNewXOffset < 0)" + NL + T + "{" + NL + T*2 + "nNewMapX--;" + NL + T*2 + "nNewXOffset += CellWidth;" + NL + T + "}" + NL +
           T + "else if (nNewXOffset > CellWidth)" + NL + T + "{" + NL + T*2 + "nNewMapX++;" + NL + T*2 + "nNewXOffset -= CellWidth;" + NL + T + "}" + NL + T + NL +
           T + "if (nNewYOffset < 0)" + NL + T + "{" + NL + T*2 + "nNewMapY--;" + NL + T*2 + "nNewYOffset += CellHeight;" + NL + T + "}" + NL +
           T + "else if (nNewYOffset > CellHeight)" + NL + T + "{" + NL + T*2 + "nNewMapY++;" + NL + T*2 + "nNewYOffset -= CellHeight;" + NL + T + "}" + NL)
    new = (T + "// " + MK + " O DAN BI TRE: khoi if/else if duoi chi lui/tien DUNG MOT o moi khung. Dan VLTK nhanh (528 = 55 px/khung," + NL +
           T + "// 529 = 45, 580 = 60, 641 = 80) vuot 1 o (32 px) moi khung -> du ~0,7 o/khung don lai, sau 6-7 khung m_nCurrentMapX/Y TRE 3-6 o" + NL +
           T + "// so voi vi tri that. Map2Mps van ra dung mps (log nhin binh thuong) nhung CheckCollision/ProcessCollision quet quanh O CU" + NL +
           T + "// -> quet phia SAU dan, muc tieu ngay truoc mui dan khong bao gio thay (do 02/09: 1969 phong 103, 48 lan truot kieu nay," + NL +
           T + "// offset chua chuan hoa toi 167936 = 5 o). Chieu co dien JX1 <= 1 o/khung nen while chay dung 1 vong = y het cu." + NL +
           T + "BOOL bVhCell = VhIsVltkMissle(VhMissleType(m_nSkillId, m_nLevel));" + NL +
           T + "if (bVhCell)" + NL +
           T + "{" + NL +
           T*2 + "while (nNewXOffset < 0)" + NL + T*2 + "{" + NL + T*3 + "nNewMapX--;" + NL + T*3 + "nNewXOffset += CellWidth;" + NL + T*2 + "}" + NL +
           T*2 + "while (nNewXOffset > CellWidth)" + NL + T*2 + "{" + NL + T*3 + "nNewMapX++;" + NL + T*3 + "nNewXOffset -= CellWidth;" + NL + T*2 + "}" + NL +
           T*2 + "while (nNewYOffset < 0)" + NL + T*2 + "{" + NL + T*3 + "nNewMapY--;" + NL + T*3 + "nNewYOffset += CellHeight;" + NL + T*2 + "}" + NL +
           T*2 + "while (nNewYOffset > CellHeight)" + NL + T*2 + "{" + NL + T*3 + "nNewMapY++;" + NL + T*3 + "nNewYOffset -= CellHeight;" + NL + T*2 + "}" + NL +
           T + "}" + NL +
           T + "else" + NL +
           T + "{" + NL +
           T*2 + "if (nNewXOffset < 0)" + NL + T*2 + "{" + NL + T*3 + "nNewMapX--;" + NL + T*3 + "nNewXOffset += CellWidth;" + NL + T*2 + "}" + NL +
           T*2 + "else if (nNewXOffset > CellWidth)" + NL + T*2 + "{" + NL + T*3 + "nNewMapX++;" + NL + T*3 + "nNewXOffset -= CellWidth;" + NL + T*2 + "}" + NL + T + NL +
           T*2 + "if (nNewYOffset < 0)" + NL + T*2 + "{" + NL + T*3 + "nNewMapY--;" + NL + T*3 + "nNewYOffset += CellHeight;" + NL + T*2 + "}" + NL +
           T*2 + "else if (nNewYOffset > CellHeight)" + NL + T*2 + "{" + NL + T*3 + "nNewMapY++;" + NL + T*3 + "nNewYOffset -= CellHeight;" + NL + T*2 + "}" + NL +
           T + "}" + NL)
    f.rep(old, new, MK + " o dan bi tre")
    # ---- C: FindNpc voi moc uu tien (nhanh VLTK)
    old = (T*4 + "nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nLauncher, m_eRelation);" + NL +
           T*4 + "if (nNpcIdx > 0)" + NL)
    new = (T*4 + "// " + MK + " XAC CHE MUC TIEU: FindNpc khong co moc uu tien tra CON DAU TIEN cua o; neu do la xac thi khoi VLTK duoi" + NL +
           T*4 + "// `continue` bo LUON CA O -> muc tieu con song dung chung o khong bao gio duoc xet (do 02/09: 1969 ban 4 lan vao con" + NL +
           T*4 + "// 30000 mau dung tren xac, ca 4 lan VH-COL-NONE). Truyen muc tieu dang bam (hoac MAX_NPC lam moc khong bao gio trung)" + NL +
           T*4 + "// de FindNpc dung nhanh fallback co san: uu tien muc tieu, va CON SONG hon XAC. Dan co dien truyen 0 = y nguyen." + NL +
           T*4 + "int nVhPrefer = VhIsVltkMissle(VhMissleType(m_nSkillId, m_nLevel)) ? ((m_nFollowNpcIdx > 0 && m_nFollowNpcIdx < MAX_NPC) ? m_nFollowNpcIdx : MAX_NPC) : 0;" + NL +
           T*4 + "nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nLauncher, m_eRelation, nVhPrefer);" + NL +
           T*4 + "if (nNpcIdx > 0)" + NL)
    f.rep(old, new, MK + " xac che muc tieu")
    # ---- D: hoi quy ban va dot 7 (nRange == 1 phai quet dung 1 o)
    old = (T + "// [VHTD 02/09k] dan VLTK: ban kinh = nRange (game_y.exe 0x6fb6c0 -> tim NPC theo ban kinh, khong chia doi); dan JX1: +-nRange/2 nhu cu" + NL +
           T + "int nRangeX = VhIsVltkMissle(VhMissleType(m_nSkillId, m_nLevel)) ? nRange : nRange / 2;" + NL)
    new = (T + "// [VHTD 02/09k] dan VLTK: ban kinh = nRange (game_y.exe 0x6fb6c0 -> tim NPC theo ban kinh, khong chia doi); dan JX1: +-nRange/2 nhu cu" + NL +
           T + "// " + MK + " SUA HOI QUY dot 7: khoi tren ap ca cho loi goi nRange = 1 tu CheckCollision (danh DUNG mot NPC da chon)" + NL +
           T + "// -> quet 3x3 thay vi 1 o, dan don muc tieu danh lan con dung canh. Chi bo /2 khi nRange > 1 (dan dien rong)." + NL +
           T + "int nRangeX = (VhIsVltkMissle(VhMissleType(m_nSkillId, m_nLevel)) && nRange > 1) ? nRange : nRange / 2;" + NL)
    f.rep(old, new, MK + " nRange 1 quet 1 o")
    # ---- E: bo qua xac trong vong quet sat thuong (dan VLTK)
    old = (T*3 + "int nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, nLauncherIdx, eRelation, nPreferIdx);" + NL +
           T*3 + "if (nNpcIdx > 0)" + T + NL)
    new = (T*3 + "int nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, nLauncherIdx, eRelation, nPreferIdx);" + NL +
           T*3 + "// " + MK + " dan VLTK: XAC khong duoc tinh vao nRet / m_nHitCount (dan gioi han so muc tieu se het luot vi xac)" + NL +
           T*3 + "if (nNpcIdx > 0 && VhIsVltkMissle(VhMissleType(m_nSkillId, m_nLevel)) &&" + NL +
           T*4 + "(Npc[nNpcIdx].m_Doing == do_death || Npc[nNpcIdx].m_Doing == do_revive))" + NL +
           T*3 + "{" + NL +
           T*4 + "VHLOG(\"[VH-SCAN-XAC] msl=%d sk=%d bo qua xac npc=%d(id=%u doing=%d) o(%d,%d)\", m_nMissleId, m_nSkillId, nNpcIdx, (unsigned int)Npc[nNpcIdx].m_dwID, (int)Npc[nNpcIdx].m_Doing, i, j);" + NL +
           T*4 + "continue;" + NL +
           T*3 + "}" + NL +
           T*3 + "if (nNpcIdx > 0)" + T + NL)
    f.rep(old, new, MK + " bo qua xac khi quet")
    f.save()

def h_kmissleset():
    p = os.path.join(CORE, "KMissleSet.cpp")
    b = io.open(p, "rb").read(); assert b[:3] != b"\xef\xbb\xbf"
    s = b.decode("latin-1"); orig = s
    if MK in s:
        print("  [=] KMissleSet.cpp: " + MK); return
    old = T + "Missle[nFreeIndex].m_nLastDoCollisionIdx = 0;" + NL
    c = s.count(old)
    if c != 2: raise SystemExit("neo KMissleSet: %d lan (can 2)" % c)
    new = (T + "Missle[nFreeIndex].m_nLastDoCollisionIdx = 0;" + NL +
           T + "// " + MK + " O DAN TAI SU DUNG: m_ulNextCalDamageTime / m_nHitCount chi duoc dat 0 o constructor; KMissle::Release phia server" + NL +
           T + "// khong dat lai -> dan moi sinh trong vong DmgInterval cua dan truoc (17 khung = 0,94 s voi 645/646/647; 21 khung voi 527)" + NL +
           T + "// bi khoa sat thuong ngay tu khung dau => \"phong 4-5 lan moi trung 1 lan\". Day la KHOI TAO, khong doi hanh vi chieu cu." + NL +
           "#ifdef _SERVER" + T + "// m_ulNextCalDamageTime chi co o ban server (KMissle.h:165)" + NL +
           T + "Missle[nFreeIndex].m_ulNextCalDamageTime = 0;" + NL + "#endif" + NL +
           T + "Missle[nFreeIndex].m_nHitCount = 0;" + NL)
    s = s.replace(old, new)
    nb = s.encode("latin-1")
    assert sum(1 for ch in nb if ch >= 0x80) == sum(1 for ch in b if ch >= 0x80)
    print("  [+] KMissleSet.cpp: %s dat lai bo dem sat thuong (2 cho)" % MK)
    if not KIEM: io.open(p, "wb").write(nb)
    print("  => %s %s" % ("KIEM" if KIEM else "ghi", p))

def h_knpc():
    f = F("KNpc.cpp")
    # F1: ReplySkill / AttackSkill loai muc tieu het mau
    old = (T + "if (rA.nType == 1 && nLauncher > 0 && nLauncher < MAX_NPC && Npc[nLauncher].m_Index > 0)" + NL +
           T*3 + "CastAutoSkillAt(rA.nSkillId, rA.nSkillLevel, nLauncher);" + NL)
    new = (T + "// " + MK + " khong phan don vao XAC: nan nhan vua bi don cuoi ve 0 mau van con m_Doing != do_death mot vai khung" + NL +
           T*2 + "if (rA.nType == 1 && nLauncher > 0 && nLauncher < MAX_NPC && Npc[nLauncher].m_Index > 0 && Npc[nLauncher].m_CurrentLife > 0)" + NL +
           T*3 + "CastAutoSkillAt(rA.nSkillId, rA.nSkillLevel, nLauncher);" + NL)
    # neo that co 2 tab dau (trong for) - kiem lai bang regex de chac chan
    m = re.search(r"\r\n(\t+)if \(rA\.nType == 1 && nLauncher > 0 && nLauncher < MAX_NPC && Npc\[nLauncher\]\.m_Index > 0\)\r\n", f.s)
    if not m: raise SystemExit("neo ReplySkill khong thay")
    tabs = m.group(1)
    old = "\r\n" + tabs + "if (rA.nType == 1 && nLauncher > 0 && nLauncher < MAX_NPC && Npc[nLauncher].m_Index > 0)\r\n"
    new = ("\r\n" + tabs + "// " + MK + " khong phan don vao XAC: nan nhan vua bi don cuoi ve 0 mau van con m_Doing != do_death mot vai khung" + NL +
           tabs + "if (rA.nType == 1 && nLauncher > 0 && nLauncher < MAX_NPC && Npc[nLauncher].m_Index > 0 && Npc[nLauncher].m_CurrentLife > 0)\r\n")
    f.rep(old, new, MK + " ReplySkill bo xac")
    # AttackSkill: chan o dau ham (them dieu kien het mau cho ca hai ben)
    old = (T + "if (Npc[nLauncher].m_Doing == do_death || Npc[nLauncher].m_Doing == do_revive)" + NL + T*2 + "return;" + NL)
    new = (T + "if (Npc[nLauncher].m_Doing == do_death || Npc[nLauncher].m_Doing == do_revive)" + NL + T*2 + "return;" + NL +
           T + "// " + MK + " nan nhan het mau (chua kip doi sang do_death) khong duoc chon lam muc tieu auto-skill:" + NL +
           T + "// do 02/09 (log VH-*): 130/234 kiem 1368 bay het doi ma khong trung ai vi duoi theo xac." + NL +
           T + "if (m_CurrentLife <= 0 || Npc[nLauncher].m_CurrentLife <= 0)" + NL + T*2 + "return;" + NL)
    f.rep(old, new, MK + " AttackSkill bo xac")
    # CastAutoSkillAt: chan cuoi cung
    old = (T + "if (nTarget <= 0 || nTarget >= MAX_NPC || Npc[nTarget].m_Index <= 0 || Npc[nTarget].m_SubWorldIndex != m_SubWorldIndex)" + NL + T*2 + "return;" + NL)
    new = (T + "if (nTarget <= 0 || nTarget >= MAX_NPC || Npc[nTarget].m_Index <= 0 || Npc[nTarget].m_SubWorldIndex != m_SubWorldIndex)" + NL + T*2 + "return;" + NL +
           T + "// " + MK + " chot cuoi: khong phong auto-skill vao muc tieu da chet / dang hoi sinh / het mau" + NL +
           T + "if (Npc[nTarget].m_CurrentLife <= 0 || Npc[nTarget].m_Doing == do_death || Npc[nTarget].m_Doing == do_revive)" + NL + T*2 + "return;" + NL)
    f.rep(old, new, MK + " CastAutoSkillAt bo xac")
    f.save()

if __name__ == "__main__":
    print("vhtd_engine_patch10 %s%s" % (MK, " (KIEM)" if KIEM else ""))
    h_kmissle(); h_kmissleset(); h_knpc()
    print("XONG.")
