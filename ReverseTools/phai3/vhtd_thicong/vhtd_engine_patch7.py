# -*- coding: utf-8 -*-
"""vhtd_engine_patch7.py [VHTD 02/09k] - dot 7 theo 11 muc chu 02/09 ~13:15 ("skill tieu dao cam van danh hut", "cac skill co vong tron nho hon
nhieu so voi client vltk", "log het cac phai moi", "luc tay ... hien 0/0", "ong mau lac nhan binh sa ... o thanh mau duoi ten nhan vat"):
 H1 KMissle.cpp  NGU NGHIA VA CHAM VLTK cho dan thoi VLTK (id >= 400: su kien 2013, Hoa Son 412-441, 3 phai >= 500). Do tren game_y.exe:
                 - ProcessCollision (0x6fb6c0): tim NPC trong BAN KINH nRange o (KSubWorld tim theo ban kinh), KHONG chia doi -> JX1 quet +-nRange/2
                   => dien tich = 1/4 ("vong tron nho hon nhieu"). Nay: dan VLTK quet +-nRange.
                 - CheckCollision (0x6fba60) CollidRange > 1: NPC con song trong ban kinh CollidRange -> DmgRange == 1 thi danh DUNG con do
                   (ProcessCollision tai NPC, range 1), nguoc lai ProcessCollision() tai dan voi ban kinh DmgRange. Thay khoi [VHTD 02/09g].
                 - LOG TOAN BO (VHLOG = AUTOLOG_IDX(launcher), chi ky nang 3 phai): [VH-MSL-NEW] [VH-MSL-TICK] [VH-COL-IN/FOUND/NONE] [VH-SCAN..]
                   [VH-DMG-TRY/HIT/SKIP] [VH-END-*] [VH-MSL-END] [VH-CL-SHOW] - server: nhan vat [AutoLog] Name; client: moi launcher.
 H2 KNpc.cpp     PaintLife: thanh khien tinh (Lac Nhan Binh Sa / Phat Y) ve NGAY TREN thanh mau duoi ten nhan vat (chinh minh), mau xanh.
 H3 KPlayer.cpp  GetEchoDamage: cam Tieu Dao (lightingdamage_p) -> 'luc tay' = sat thuong Loi theo noi luc toi da (truoc 0/0).
 H4 KProtocolProcess.cpp (client) log nhan lenh phong ky nang 3 phai: [VH-CL-CAST-DIRECT] / [VH-CL-CAST-NET].
Doc/ghi latin-1, NL theo tung tep, marker [VHTD 02/09k], chay lai = [=]. DUNG: python vhtd_engine_patch7.py [--kiem]
"""
import io, os, sys, re

KIEM = "--kiem" in sys.argv
CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
MK = "[VHTD 02/09k]"
T = "\t"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()

class F:
    def __init__(self, path):
        self.path = path; self.s = rd(path); self.orig = self.s; self.n = 0
        self.nl = "\r\n" if self.s.count("\r\n") * 2 > self.s.count("\n") else "\n"
        assert self.s[:3] != "\xef\xbb\xbf", "BOM: " + path
    def NL(self, s): return s.replace("\r\n", self.nl) if self.nl != "\r\n" else s
    def rep(self, old, new, tag, count=1):
        old = self.NL(old); new = self.NL(new)
        if tag in self.s: print("  [=] %s: %s" % (os.path.basename(self.path), tag)); return
        c = self.s.count(old)
        if c != count: raise SystemExit("neo %s: %d lan (can %d) trong %s\n%r" % (tag, c, count, self.path, old[:200]))
        self.s = self.s.replace(old, new); self.n += 1
        print("  [+] %s: %s" % (os.path.basename(self.path), tag))
    def rep_re(self, pat, fn, tag, count=1):
        if tag in self.s: print("  [=] %s: %s" % (os.path.basename(self.path), tag)); return
        ms = list(re.finditer(pat, self.s))
        if len(ms) != count: raise SystemExit("neo re %s: %d lan (can %d) trong %s" % (tag, len(ms), count, self.path))
        for m in reversed(ms):
            self.s = self.s[:m.start()] + self.NL(fn(m)) + self.s[m.end():]
        self.n += 1; print("  [+] %s: %s" % (os.path.basename(self.path), tag))
    def save(self):
        if self.s == self.orig: return
        assert "\xef\xbf\xbd" not in self.s
        assert sum(1 for c in self.s if ord(c) >= 0x80) == sum(1 for c in self.orig if ord(c) >= 0x80), "byte cao doi " + self.path
        if not KIEM: io.open(self.path, "w", encoding="latin-1", newline="").write(self.s)
        print("  => %s %s (%d hunk)" % ("KIEM" if KIEM else "ghi", self.path, self.n))

# ------------------------------------------------------------------ H1 KMissle.cpp
def h_kmissle():
    f = F(os.path.join(CORE, "KMissle.cpp")); NL = "\r\n"
    # H1a helper sau #include "KOption.h"
    helper = (
        "#include \"KOption.h\"" + NL +
        "// " + MK + " dan THOI VLTK (su kien 2013 400-411, Hoa Son 412-441, Vu Hon/Tieu Dao >= 500): ngu nghia va cham cua client VLTK game_y.exe" + NL +
        "// (ProcessCollision 0x6fb6c0: tim NPC trong BAN KINH nRange o, khong chia doi; CheckCollision 0x6fba60: CollidRange > 1 -> NPC song trong" + NL +
        "// ban kinh CollidRange, DmgRange == 1 danh dung con do, khac thi no tai dan ban kinh DmgRange). Dan JX1 (< 400) giu +-nRange/2 (can bang cu)." + NL +
        "static inline BOOL VhIsVltkMissle(int nMissleType) { return nMissleType >= 400; }" + NL +
        "// m_nMissleId = CHI SO instance trong Missle[] (KMissleSet::Add ghi de), KHONG phai id dan trong missles.txt -> lay kieu dan qua ky nang tao dan" + NL +
        "// (KSkill::Cast: GetInfoFromTabFile(m_nChildSkillId)). Khoi [VHTD 02/09g] cu so m_nMissleId >= 500 = so chi so instance -> NGAU NHIEN (goc danh 4-5 lan moi trung)." + NL +
        "static inline int VhMissleType(int nSkillId, int nLevel) { KSkill* pVhS = (KSkill*)g_SkillManager.GetSkill(nSkillId, nLevel); return pVhS ? pVhS->GetChildSkillId() : 0; }" + NL +
        "static inline BOOL VhIsNewFactionSkill(int nSkillId) { return (nSkillId >= 1363 && nSkillId <= 1384) || (nSkillId >= 1965 && nSkillId <= 1991) || (nSkillId >= 2114 && nSkillId <= 2143); }" + NL +
        "// log toan bo dan ky nang 3 phai: server = nhan vat [AutoLog] Name (AUTOLOG_IDX), client = moi launcher (g_AutoLogWho tra 1)" + NL +
        "#define VHLOG(...) do { if (VhIsNewFactionSkill(m_nSkillId)) { AUTOLOG_IDX(m_nLauncher, __VA_ARGS__); } } while (0)" + NL)
    f.rep("#include \"KOption.h\"" + NL, helper, MK + " helper")
    # H1b ProcessCollision: ban kinh
    old = T + "int nRangeX = nRange / 2;" + NL + T + "int" + T + "nRangeY = nRangeX;" + NL
    new = (T + "// " + MK + " dan VLTK: ban kinh = nRange (game_y.exe 0x6fb6c0 -> tim NPC theo ban kinh, khong chia doi); dan JX1: +-nRange/2 nhu cu" + NL +
           T + "int nRangeX = VhIsVltkMissle(VhMissleType(m_nSkillId, m_nLevel)) ? nRange : nRange / 2;" + NL + T + "int" + T + "nRangeY = nRangeX;" + NL +
           T + "VHLOG(\"[VH-SCAN-IN] msl=%d sk=%d lv=%d launcher=%d tai(r=%d,%d,%d) range=%d quet=+-%d rel=%d prefer=%d hitmax=%d clientsend=%d\", m_nMissleId, m_nSkillId, m_nLevel, nLauncherIdx, nRegionId, nMapX, nMapY, nRange, nRangeX, eRelation, nPreferIdx, m_nHitCount, (int)m_bClientSend);" + NL)
    f.rep(old, new, MK + " ban kinh ProcessCollision")
    # H1c log NPC tim thay trong ProcessCollision (sau nRet++ ngay sau COLL-NPC-FOUND)
    def fn_found(m):
        return m.group(0) + T*4 + "VHLOG(\"[VH-SCAN-NPC] msl=%d sk=%d npc=%d(id=%u kind=%u doing=%d life=%d) o(%d,%d) region=%d nret=%d\", m_nMissleId, m_nSkillId, nNpcIdx, (unsigned int)Npc[nNpcIdx].m_dwID, (unsigned int)Npc[nNpcIdx].m_Kind, (int)Npc[nNpcIdx].m_Doing, Npc[nNpcIdx].m_CurrentLife, i, j, nSearchRegion, nRet);" + NL
    f.rep_re(r"AUTOLOG_EVERY\(2000, \"\[COLL-NPC-FOUND\][^\r\n]*\r?\n\t\t\t\tnRet\+\+;\r?\n", fn_found, MK + " log NPC quet")
    # H1d log ket qua ProcessCollision
    old = T + "return nRet;" + NL + "}" + NL + NL + "int KMissle::ProcessCollision()" + NL
    new = (T + "VHLOG(\"[VH-SCAN-OUT] msl=%d sk=%d nret=%d range=%d\", m_nMissleId, m_nSkillId, nRet, nRange);" + NL + old)
    f.rep(old, new, MK + " log ket qua quet")
    # H1e CheckCollision else-branch: thay khoi [VHTD 02/09g]
    pat = (r"(\t+)if \(m_nMissleId >= 500 && \(abs\(i\) > m_nDamageRange / 2 \|\| abs\(j\) > m_nDamageRange / 2\)\)\r?\n\t+\{\r?\n\t+if \(!m_bClientSend\)\r?\n"
           r"\t+ProcessCollision\(m_nLauncher, nSearchRegion, nRMx, nRMy, m_bRangeDamage \? m_nDamageRange : 1, m_eRelation, nNpcIdx\);\r?\n\t+DoCollision\(\);\r?\n\t+return 1;\r?\n\t+\}\r?\n")
    def fn_else(m):
        t = m.group(1)
        return (t + "// " + MK + " dan VLTK (game_y.exe CheckCollision 0x6fba60): bo qua xac; DmgRange == 1 -> danh DUNG NPC nay (ProcessCollision tai NPC," + NL +
                t + "// range 1); nguoc lai ProcessCollision() tai dan voi BAN KINH DmgRange. Thay khoi [VHTD 02/09g] (danh tai NPC khi ngoai DmgRange/2)." + NL +
                t + "if (VhIsVltkMissle(VhMissleType(m_nSkillId, m_nLevel)))" + NL + t + "{" + NL +
                t + T + "if (Npc[nNpcIdx].m_Doing == do_death || Npc[nNpcIdx].m_Doing == do_revive)" + NL + t + T*2 + "continue;" + NL +
                t + T + "VHLOG(\"[VH-COL-FOUND] msl=%d sk=%d lv=%d colrange=%d dmgrange=%d npc=%d(id=%u doing=%d life=%d) o(%d,%d) dan(r=%d,%d,%d off %d,%d z=%d) life=%d/%d clientsend=%d\", m_nMissleId, m_nSkillId, m_nLevel, m_nCollideRange, m_nDamageRange, nNpcIdx, (unsigned int)Npc[nNpcIdx].m_dwID, (int)Npc[nNpcIdx].m_Doing, Npc[nNpcIdx].m_CurrentLife, i, j, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, m_nCurrentMapZ, m_nCurrentLife, m_nLifeTime, (int)m_bClientSend);" + NL +
                t + T + "if (m_nDamageRange == 1)" + NL +
                t + T*2 + "ProcessCollision(m_nLauncher, nSearchRegion, nRMx, nRMy, 1, m_eRelation, nNpcIdx);" + NL +
                t + T + "else" + NL +
                t + T*2 + "ProcessCollision();" + NL +
                t + T + "if (this->m_eMoveKind == MISSLE_MMK_Line)" + NL +
                t + T*2 + "m_nLastDoCollisionIdx = nNpcIdx;" + NL +
                t + T + "DoCollision();" + NL +
                t + T + "return 1;" + NL +
                t + "}" + NL)
    f.rep_re(pat, fn_else, MK + " CheckCollision VLTK")
    # H1f log vao CheckCollision (sau MIS-COL-ENTER) va khong thay NPC (truoc return 0 cuoi)
    def fn_colin(m):
        return m.group(0) + T + "VHLOG(\"[VH-COL-IN] msl=%d sk=%d lv=%d colrange=%d dmgrange=%d rangedmg=%d dan(r=%d,%d,%d off %d,%d z=%d) life=%d/%d follow=%d lasthit=%d\", m_nMissleId, m_nSkillId, m_nLevel, m_nCollideRange, m_nDamageRange, (int)m_bRangeDamage, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, m_nCurrentMapZ, m_nCurrentLife, m_nLifeTime, m_nFollowNpcIdx, m_nLastDoCollisionIdx);" + NL
    f.rep_re(r"\tAUTOLOG_EVERY\(1000, \"\[MIS-COL-ENTER\][^\r\n]*\r?\n", fn_colin, MK + " log vao CheckCollision")
    old = T + "return 0;" + NL + "}" + NL + NL + "inline DWORD" + T + "KMissle::GetCurrentSubWorldTime()" + NL
    new = (T + "VHLOG(\"[VH-COL-NONE] msl=%d sk=%d colrange=%d dan(r=%d,%d,%d) life=%d/%d follow=%d\", m_nMissleId, m_nSkillId, m_nCollideRange, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nCurrentLife, m_nLifeTime, m_nFollowNpcIdx);" + NL + old)
    f.rep(old, new, MK + " log khong cham")
    # H1g PrePareFly: [VH-MSL-NEW]
    old = T + "return true;" + NL + T + NL + "}" + NL + NL + "// [S5 24/08 dem]"
    new = (T + "{" + NL +
           T*2 + "int nVhTx = -1, nVhTy = -1, nVhMx = 0, nVhMy = 0; GetMpsPos(&nVhMx, &nVhMy);" + NL +
           T*2 + "if (m_nFollowNpcIdx > 0 && m_nFollowNpcIdx < MAX_NPC) Npc[m_nFollowNpcIdx].GetMpsPos(&nVhTx, &nVhTy);" + NL +
           T*2 + "VHLOG(\"[VH-MSL-NEW] msl=%d id=%d sk=%d lv=%d launcher=%d(id=%u) move=%d speed=%d life=%d..%d col=%d dmg=%d rangedmg=%d itv=%lu colvanish=%d explode=%d follow=%d(id=%u) pos(r=%d,%d,%d off %d,%d z=%d) mps(%d,%d) tgt(%d,%d) dir=%d f(%d,%d) clientsend=%d\", m_nMissleId, VhMissleType(m_nSkillId, m_nLevel), m_nSkillId, m_nLevel, m_nLauncher, (unsigned int)m_dwLauncherId, (int)m_eMoveKind, m_nSpeed, m_nStartLifeTime, m_nLifeTime, m_nCollideRange, m_nDamageRange, (int)m_bRangeDamage, m_ulDamageInterval, (int)m_bCollideVanish, (int)m_bAutoExplode, m_nFollowNpcIdx, (unsigned int)m_dwFollowNpcID, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, m_nCurrentMapZ, nVhMx, nVhMy, nVhTx, nVhTy, m_nDir, m_nXFactor, m_nYFactor, (int)m_bClientSend);" + NL +
           T + "}" + NL + old)
    f.rep(old, new, MK + " log dan moi")
    # H1h OnFly: tick + barrier + colfail + region
    def fn_tick(m):
        return (m.group(0) +
                T + "{" + NL +
                T*2 + "int nVhTx = -1, nVhTy = -1, nVhMx = 0, nVhMy = 0; GetMpsPos(&nVhMx, &nVhMy);" + NL +
                T*2 + "if (m_nFollowNpcIdx > 0 && m_nFollowNpcIdx < MAX_NPC) Npc[m_nFollowNpcIdx].GetMpsPos(&nVhTx, &nVhTy);" + NL +
                T*2 + "VHLOG(\"[VH-MSL-TICK] msl=%d sk=%d life=%d/%d st=%d pos(r=%d,%d,%d off %d,%d z=%d) mps(%d,%d) follow=%d tgt(%d,%d) dcell=%d barrier=%d lasthit=%d\", m_nMissleId, m_nSkillId, m_nCurrentLife, m_nLifeTime, (int)m_eMissleStatus, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, m_nCurrentMapZ, nVhMx, nVhMy, m_nFollowNpcIdx, nVhTx, nVhTy, (nVhTx >= 0) ? (int)(sqrt((double)((nVhTx - nVhMx) * (nVhTx - nVhMx) + (nVhTy - nVhMy) * (nVhTy - nVhMy))) / 32) : -1, (int)TestBarrier(), m_nLastDoCollisionIdx);" + NL +
                T + "}" + NL)
    f.rep_re(r"\tAUTOLOG_EVERY\(1000, \"\[MSL-BARRIER\][^\r\n]*\r?\n", fn_tick, MK + " log tick")
    f.rep_re(r"\tif \(TestBarrier\(\)\)[ \t]*\r?\n\t\{\r?\n", lambda m: m.group(0) + T*2 + "VHLOG(\"[VH-END-BARRIER] msl=%d sk=%d life=%d/%d pos(r=%d,%d,%d)\", m_nMissleId, m_nSkillId, m_nCurrentLife, m_nLifeTime, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY);" + NL, MK + " log barrier")
    f.rep_re(r"\t\t\tAUTOLOG_EVERY\(1000, \"\[MIS-FLY-COLFAIL\][^\r\n]*\r?\n", lambda m: m.group(0) + T*3 + "VHLOG(\"[VH-END-COLFAIL] msl=%d sk=%d life=%d/%d z=%d region=%d explode=%d\", m_nMissleId, m_nSkillId, m_nCurrentLife, m_nLifeTime, m_nCurrentMapZ, m_nRegionId, (int)m_bAutoExplode);" + NL, MK + " log colfail")
    f.rep_re(r"\telse//[^\r\n]*\r?\n\t\{\r?\n(\t\tDoVanish\(\);\r?\n\t\}\r?\n\}\r?\n)", lambda m: m.group(0)[:m.start(1) - m.start()] + T*2 + "VHLOG(\"[VH-END-REGION] msl=%d sk=%d life=%d/%d pos(r=%d,%d,%d)\", m_nMissleId, m_nSkillId, m_nCurrentLife, m_nLifeTime, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY);" + NL + m.group(1), MK + " log ra region")
    # H1i DoVanish: [VH-MSL-END] sau MSL-END
    f.rep_re(r"\tAUTOLOG_EVERY\(1000, \"\[MSL-END\][^\r\n]*\r?\n", lambda m: m.group(0) + T + "VHLOG(\"[VH-MSL-END] msl=%d id=%d sk=%d lv=%d st=%d life=%d/%d start=%d lasthit=%d follow=%d pos(r=%d,%d,%d z=%d) hitcount=%d\", m_nMissleId, VhMissleType(m_nSkillId, m_nLevel), m_nSkillId, m_nLevel, (int)m_eMissleStatus, m_nCurrentLife, m_nLifeTime, m_nStartLifeTime, m_nLastDoCollisionIdx, m_nFollowNpcIdx, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nCurrentMapZ, m_nHitCount);" + NL, MK + " log ket thuc")
    # H1j ProcessDamage: TRY / HIT / SKIP
    old = T*2 + "if (Npc[nNpcId].ReceiveDamage(m_nLauncher, m_nMissleSeries, m_bIsPhysical, m_bIsMelee, m_pMagicAttribsData->m_pDamageMagicAttribs, m_bUseAttackRating, m_nDoHurtP, m_nMissRate))" + NL
    new = (T*2 + "VHLOG(\"[VH-DMG-TRY] msl=%d sk=%d lv=%d launcher=%d npc=%d(id=%u doing=%d) life=%d series=%d phys=%d melee=%d ar=%d hurtp=%d miss=%d\", m_nMissleId, m_nSkillId, m_nLevel, m_nLauncher, nNpcId, (unsigned int)Npc[nNpcId].m_dwID, (int)Npc[nNpcId].m_Doing, Npc[nNpcId].m_CurrentLife, m_nMissleSeries, (int)m_bIsPhysical, (int)m_bIsMelee, (int)m_bUseAttackRating, m_nDoHurtP, m_nMissRate);" + NL +
           T*2 + "int nVhLifeTruoc = Npc[nNpcId].m_CurrentLife;" + NL + old)
    f.rep(old, new, MK + " log thu sat thuong")
    f.rep_re(r"\t\t\tAUTOLOG_EVERY\(500, \"\[E2-PDMG-HIT\][^\r\n]*\r?\n", lambda m: m.group(0) + T*3 + "VHLOG(\"[VH-DMG-HIT] msl=%d sk=%d npc=%d life %d -> %d\", m_nMissleId, m_nSkillId, nNpcId, nVhLifeTruoc, Npc[nNpcId].m_CurrentLife);" + NL, MK + " log trung")
    old = T*3 + "if (!Npc[nNpcId].IsPlayer() && pSkill->IsAura() && pSkill->IsTargetNoNpc())" + NL + T*4 + "return TRUE;" + NL
    new = (T*3 + "if (!Npc[nNpcId].IsPlayer() && pSkill->IsAura() && pSkill->IsTargetNoNpc())" + NL + T*3 + "{" + NL +
           T*4 + "VHLOG(\"[VH-DMG-SKIP] msl=%d sk=%d npc=%d ly_do=aura_khong_npc\", m_nMissleId, m_nSkillId, nNpcId);" + NL + T*4 + "return TRUE;" + NL + T*3 + "}" + NL)
    f.rep(old, new, MK + " log skip aura")
    old = (T*3 + "if(Npc[nNpcId].IsPlayer() && Npc[m_nLauncher].IsPlayer() && pSkill->IsTargetAlly() && Npc[nNpcId].m_CurrentCamp == camp_free && Npc[m_nLauncher].m_CurrentCamp == camp_free)" + NL + T*4 + "return TRUE;" + NL)
    new = (T*3 + "if(Npc[nNpcId].IsPlayer() && Npc[m_nLauncher].IsPlayer() && pSkill->IsTargetAlly() && Npc[nNpcId].m_CurrentCamp == camp_free && Npc[m_nLauncher].m_CurrentCamp == camp_free)" + NL + T*3 + "{" + NL +
           T*4 + "VHLOG(\"[VH-DMG-SKIP] msl=%d sk=%d npc=%d ly_do=dong_minh_camp_free\", m_nMissleId, m_nSkillId, nNpcId);" + NL + T*4 + "return TRUE;" + NL + T*3 + "}" + NL)
    f.rep(old, new, MK + " log skip ally")
    f.rep_re(r"\tAUTOLOG_EVERY\(1000, \"\[MIS-DMG-NOATTRIB\][^\r\n]*\r?\n", lambda m: m.group(0) + T + "VHLOG(\"[VH-DMG-IN] msl=%d sk=%d npc=%d attribs=%d\", m_nMissleId, m_nSkillId, nNpcId, (m_pMagicAttribsData ? 1 : 0));" + NL, MK + " log vao ProcessDamage")
    f.save()

# ------------------------------------------------------------------ H2 KNpc.cpp PaintLife
def h_knpc():
    f = F(os.path.join(CORE, "KNpc.cpp")); NL = "\r\n"
    pat = r"(g_pRepresent->DrawPrimitives\(1, &Blood, RU_T_SHADOW, FALSE\);\r?\n)(\s*)(return nHeightOffset \+ nHei;\r?\n\}\r?\n\r?\nint\tKNpc::PaintMana)"
    def fn(m):
        blk = (T + "// " + MK + " thanh khien duoi ten: khien tinh (Lac Nhan Binh Sa 2139 / Phat Y 2134 / Hoa Son) ve thanh XANH ngay tren thanh mau duoi ten (chinh minh) - chu: 'ong mau" + NL +
               T + "// thu 2 nam o thanh mau phia duoi ten nhan vat, khong phai tren ong mau'. Gia tri qua s2c_syncvhtd (dot 4). Header Player_Shield da go (UiHeaderControlBar.ini)." + NL +
               T + "if (m_Index == Player[CLIENT_PLAYER_INDEX].m_nMissleId && m_nHSShieldMax > 0 && m_CurrentStaticMagicShieldP > 0)" + NL +
               T + "{" + NL +
               T*2 + "int nVhS = m_CurrentStaticMagicShieldP * 100 / m_nHSShieldMax;" + NL +
               T*2 + "if (nVhS > 100) nVhS = 100;" + NL + T*2 + "if (nVhS < 0) nVhS = 0;" + NL +
               T*2 + "Blood.Color.Color_b.r = 70; Blood.Color.Color_b.g = 170; Blood.Color.Color_b.b = 255; Blood.Color.Color_b.a = 0;" + NL +
               T*2 + "Blood.oPosition.nX = nMpsX - nWid / 2; Blood.oPosition.nY = nMpsY; Blood.oPosition.nZ = nHeightOffset + nHei * 2 + 1;" + NL +
               T*2 + "Blood.oEndPos.nX = Blood.oPosition.nX + nWid * nVhS / 100; Blood.oEndPos.nY = nMpsY; Blood.oEndPos.nZ = nHeightOffset + nHei + 1;" + NL +
               T*2 + "g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);" + NL +
               T*2 + "Blood.Color.Color_b.r = 90; Blood.Color.Color_b.g = 90; Blood.Color.Color_b.b = 120;" + NL +
               T*2 + "Blood.oPosition.nX = Blood.oEndPos.nX; Blood.oEndPos.nX = nMpsX + nWid / 2;" + NL +
               T*2 + "g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);" + NL +
               T*2 + "return nHeightOffset + nHei * 2 + 1;" + NL +
               T + "}" + NL)
        return m.group(1) + m.group(2) + blk + T + m.group(3)
    f.rep_re(pat, fn, MK + " thanh khien duoi ten")
    f.save()

# ------------------------------------------------------------------ H3 KPlayer.cpp GetEchoDamage
def h_kplayer():
    f = F(os.path.join(CORE, "KPlayer.cpp")); NL = "\r\n"
    old = T + "// Calc lighting damage[12]" + NL + T + "if (magic_lightingdamage_v == pMagicData->nAttribType)" + NL
    new = (T + "// " + MK + " cam Tieu Dao (lightingdamage_p o [12]): 'luc tay' = sat thuong Loi theo NOI LUC TOI DA (cong thuc KNpc::AppendSkillEffect dot 4);" + NL +
           T + "// truoc chi cong sat thuong vat ly -> bang trang thai hien 0/0 khi dat ky nang cam (chu 02/09)." + NL +
           T + "if (magic_lightingdamage_p == pMagicData->nAttribType)" + NL + T + "{" + NL +
           T*2 + "int nVhMin = (int)((__int64)(Npc[m_nMissleId].m_CurrentManaMax + Npc[m_nMissleId].m_PhysicsMagic.nValue[0] + Npc[m_nMissleId].m_CurrentLightMagic.nValue[0]) * (100 + Npc[m_nMissleId].m_nHSAddLightMagicP) / 100);" + NL +
           T*2 + "int nVhMax = (int)((__int64)(Npc[m_nMissleId].m_CurrentManaMax + Npc[m_nMissleId].m_PhysicsMagic.nValue[2] + Npc[m_nMissleId].m_CurrentLightMagic.nValue[2]) * (100 + Npc[m_nMissleId].m_nHSAddLightMagicP) / 100);" + NL +
           T*2 + "if (nVhMax < nVhMin) nVhMax = nVhMin;" + NL +
           T*2 + "*nMin += (int)((__int64)nVhMin * pMagicData->nValue[0] / 100 * (100 + nAddDamageP) / 100);" + NL +
           T*2 + "*nMax += (int)((__int64)nVhMax * pMagicData->nValue[0] / 100 * (100 + nAddDamageP) / 100);" + NL +
           T + "}" + NL + old)
    f.rep(old, new, MK + " luc tay Loi")
    f.save()

# ------------------------------------------------------------------ H4 KProtocolProcess.cpp (client)
def h_proto():
    f = F(os.path.join(CORE, "KProtocolProcess.cpp")); NL = "\r\n"
    cond = "((nSkillID >= 1363 && nSkillID <= 1384) || (nSkillID >= 1965 && nSkillID <= 1991) || (nSkillID >= 2114 && nSkillID <= 2143))"
    old = T + "int nIdx = NpcSet.SearchID(dwNpcID);" + NL + T + NL + T + "//_ASSERT (nSkillID > 0 && nSkillLevel > 0);" + NL
    new = (T + "int nIdx = NpcSet.SearchID(dwNpcID);" + NL +
           T + "if " + cond + "\t// " + MK + " log nhan lenh phong ky nang 3 phai (client)" + NL +
           T*2 + "AUTOLOG(\"[VH-CL-CAST-DIRECT] npc=%u idx=%d skill=%d lv=%d mps=(%d,%d) tgt=%d me=%d t=%u\", dwNpcID, nIdx, nSkillID, nSkillLevel, (int)MapX, (int)MapY, nVhtdTargetIdx, Player[CLIENT_PLAYER_INDEX].m_nMissleId, SubWorld[0].m_dwCurrentTime);" + NL +
           T + NL + T + "//_ASSERT (nSkillID > 0 && nSkillLevel > 0);" + NL)
    f.rep(old, new, MK + " log cast direct")
    old = T + "nSkillEnChance = *(int *)&pMsg[21];" + NL
    new = (old + T + "if " + cond + "\t// " + MK + NL +
           T*2 + "AUTOLOG(\"[VH-CL-CAST-NET] npc=%u idx=%d skill=%d lv=%d map=(%d,%d) ench=%d me=%d t=%u\", dwNpcID, NpcSet.SearchID(dwNpcID), nSkillID, nSkillLevel, MapX, MapY, nSkillEnChance, Player[CLIENT_PLAYER_INDEX].m_nMissleId, SubWorld[0].m_dwCurrentTime);" + NL)
    f.rep(old, new, MK + " log cast net")
    f.save()

if __name__ == "__main__":
    print("vhtd_engine_patch7 %s%s" % (MK, " (KIEM)" if KIEM else ""))
    h_kmissle(); h_knpc(); h_kplayer(); h_proto()
    print("XONG.")
