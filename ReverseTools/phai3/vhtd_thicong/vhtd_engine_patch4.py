# -*- coding: utf-8 -*-
"""vhtd_engine_patch4.py [VHTD 02/09g] - dot 4 Vu Hon / Tieu Dao (theo 14 muc chu giao 02/09 sang):
 H1 KMissle.cpp        CheckCollision nhanh CollidRange > 1 (dan VLTK id >= 500): NPC tim thay nam NGOAI o sat thuong quanh dan
                       (DmgRange/2) -> gay sat thuong TAI NPC do (don muc tieu: chi no; dien rong: DmgRange quanh no).
                       Log that: Tran Bien Thuy 1967 phong 87 trung 2 (86 lan 'early lasthit=0' = cham roi tan khong sat thuong).
 H2 KNpc.cpp           lightingdamage_p (cam Tieu Dao): goc = NOI LUC TOI DA (+ noi cong Loi vu khi) thay vi m_PhysicsMagic (= 0 o JX1).
 H3 KProtocolProcess   s2cDirectlyCastSkill: MapX == -1 -> Cast(nIdx, -1, chi so NPC) nhu server (dan Follow 1363 Hoa Son bam muc tieu tren client).
 H4 KNpc.cpp           DoBlurMove buoc 0: phong chieu con (2119 'Khoi' cua Tap Dap Luu Tinh 2118) tai diem xuat phat.
 H5 giao thuc          s2c_syncvhtd: so tang No/Am Luat (m_HSSp) + khien tinh (m_CurrentStaticMagicShieldP / m_nHSShieldMax) -> client;
                       CoreShell GDI_HS_SP + KUiPlayerRuntimeInfo.nShield; UiPlayerBar o icon so tang; Player_Shield thanh khien (UiHeaderControlBar).
Doc/ghi latin-1 (khong dung Edit tool), moi hunk co dau [VHTD 02/09g], chay lai = [=]. DUNG: python vhtd_engine_patch4.py [--kiem]
"""
import io, os, sys, re

KIEM = "--kiem" in sys.argv
ROOT = r"D:\GAMEDEVNEW"
SRC = os.path.join(ROOT, "Sources")
CORE = os.path.join(SRC, "Core", "Src")
S3 = os.path.join(SRC, "S3Client", "Ui")
MK = "[VHTD 02/09g]"
NL = "\r\n"
T = "\t"

def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()

def wr(p, s):
    if KIEM:
        return
    io.open(p, "w", encoding="latin-1", newline="").write(s)

def hib(s):
    return sum(1 for c in s if ord(c) >= 0x80)

class F:
    def __init__(self, path):
        self.path = path
        self.s = rd(path)
        self.orig = self.s
        self.n = 0
        # KProtocolProcess.cpp / CoreShell.h la LF; CoreShell.cpp TRON (35 CRLF / 21393 LF) -> lay theo DA SO
        self.nl = "\r\n" if self.s.count("\r\n") * 2 > self.s.count("\n") else "\n"
    def has(self, key):
        return key in self.s
    def rep(self, old, new, tag, count=1):
        if self.nl != "\r\n":
            old = old.replace("\r\n", self.nl); new = new.replace("\r\n", self.nl)
        if tag in self.s:
            print("  [=] %s: %s" % (os.path.basename(self.path), tag)); return
        if old not in self.s:
            raise SystemExit("KHONG THAY neo %s trong %s:\n%r" % (tag, self.path, old[:200]))
        if self.s.count(old) != count:
            raise SystemExit("neo %s xuat hien %d lan (can %d) trong %s" % (tag, self.s.count(old), count, self.path))
        self.s = self.s.replace(old, new)
        self.n += 1
        print("  [+] %s: %s" % (os.path.basename(self.path), tag))
    def rep_re(self, pat, fn, tag):
        if tag in self.s:
            print("  [=] %s: %s" % (os.path.basename(self.path), tag)); return
        m = re.search(pat, self.s, re.S)
        if not m:
            raise SystemExit("KHONG THAY neo regex %s trong %s" % (tag, self.path))
        self.s = self.s[:m.start()] + fn(m) + self.s[m.end():]
        self.n += 1
        print("  [+] %s: %s" % (os.path.basename(self.path), tag))
    def save(self):
        if self.s == self.orig:
            return
        if "\xef\xbf\xbd" in self.s:
            raise SystemExit("EF BF BD trong " + self.path)
        assert hib(self.s) == hib(self.orig), "byte cao doi trong " + self.path
        wr(self.path, self.s)
        print("  => ghi %s (%d hunk)%s" % (self.path, self.n, " KIEM" if KIEM else ""))

# ------------------------------------------------------------------ H1 KMissle.cpp
def h1():
    f = F(os.path.join(CORE, "KMissle.cpp"))
    old = (T*4 + "nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nLauncher, m_eRelation);" + NL +
           T*4 + "if (nNpcIdx > 0)" + NL +
           T*4 + "{" + NL +
           T*5 + "ProcessCollision();//")
    new = (T*4 + "nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nLauncher, m_eRelation);" + NL +
           T*4 + "if (nNpcIdx > 0)" + NL +
           T*4 + "{" + NL +
           T*5 + "// " + MK + " dan VLTK (id >= 500, Vu Hon/Tieu Dao: CollidRange = DmgRange 2..4, du lieu chep nguyen client VLTK): NPC lam dan" + NL +
           T*5 + "// 'cham' co the nam CACH dan toi CollidRange o, nhung ProcessCollision() chi quet DmgRange/2 o quanh DAN -> khong ai" + NL +
           T*5 + "// bi sat thuong ma dan van DoCollision/tan (ColVanish 1). Do that (jx_auto_server.log 02/09): Tran Bien Thuy 1967 phong 87," + NL +
           T*5 + "// trung 2; 86 lan S4-MSL-END 'early lasthit=0'. Nay: NPC nam ngoai o quanh dan -> gay sat thuong TAI NPC do" + NL +
           T*5 + "// (don muc tieu: chi no; dien rong: DmgRange quanh no). Dan cu (< 500) giu nguyen." + NL +
           T*5 + "if (m_nMissleId >= 500 && (abs(i) > m_nDamageRange / 2 || abs(j) > m_nDamageRange / 2))" + NL +
           T*5 + "{" + NL +
           T*6 + "if (!m_bClientSend)" + NL +
           T*7 + "ProcessCollision(m_nLauncher, nSearchRegion, nRMx, nRMy, m_bRangeDamage ? m_nDamageRange : 1, m_eRelation, nNpcIdx);" + NL +
           T*6 + "DoCollision();" + NL +
           T*6 + "return 1;" + NL +
           T*5 + "}" + NL +
           T*5 + "ProcessCollision();//")
    f.rep(old, new, MK + " dan VLTK")
    f.save()

# ------------------------------------------------------------------ H2 KNpc.cpp lightingdamage_p
def h2():
    f = F(os.path.join(CORE, "KNpc.cpp"))
    old = (T*2 + "int nLMin = m_PhysicsMagic.nValue[0] + m_CurrentLightMagic.nValue[0] * (MAX_PERCENT + m_nHSAddLightMagicP) / MAX_PERCENT;" + NL +
           T*2 + "int nLMax = m_PhysicsMagic.nValue[2] + m_CurrentLightMagic.nValue[2] * (MAX_PERCENT + m_nHSAddLightMagicP) / MAX_PERCENT;" + NL)
    new = (T*2 + "// " + MK + " goc Loi % = NOI LUC TOI DA (+ noi cong co ban + noi cong Loi vu khi neu co) x (100 + addlightingmagic_p)/100." + NL +
           T*2 + "// Ban cu chi m_PhysicsMagic (KPlayer.cpp nMagicBase = 0) + m_CurrentLightMagic (Moc Cam P3 khong co dong noi cong) -> luon 0" + NL +
           T*2 + "// ('tang diem khong thay dame'). Tieu Dao VLTK thiet ke quanh noi luc toi da (ho thuan 2139/2134 = x lan noi luc toi da);" + NL +
           T*2 + "// GIA DINH - chua doi chieu duoc nhi phin game_y.exe (bang nhay), chu quyet neu muon he so khac." + NL +
           T*2 + "int nLMin = (int)((__int64)(m_CurrentManaMax + m_PhysicsMagic.nValue[0] + m_CurrentLightMagic.nValue[0]) * (MAX_PERCENT + m_nHSAddLightMagicP) / MAX_PERCENT);" + NL +
           T*2 + "int nLMax = (int)((__int64)(m_CurrentManaMax + m_PhysicsMagic.nValue[2] + m_CurrentLightMagic.nValue[2]) * (MAX_PERCENT + m_nHSAddLightMagicP) / MAX_PERCENT);" + NL)
    f.rep(old, new, MK + " goc Loi %")

    # H4 DoBlurMove: phong chieu con tai diem xuat phat (truoc 'm_Doing = do_blurmove; break; case 1:')
    old4 = (T*2 + "m_Doing = do_blurmove;" + NL + T*2 + "break;" + NL + T + "case 1:" + NL)
    new4 = (T*2 + "// " + MK + " Tap Dap Luu Tinh 2118: chieu con 2119 ('Khoi' - dan 638 dung yen, khong thuoc tinh) phong tai diem xuat phat huong" + NL +
            T*2 + "// diem den, ca client (ve hieu ung) lan server (vo hai). Cac chieu MoveWithBlur cu (710/732/995) ChildSkillId = 0 -> khong doi." + NL +
            T*2 + "if (pSkill->GetChildSkillId() > 0 && pSkill->GetChildSkillId() < MAX_SKILL)" + NL +
            T*2 + "{" + NL +
            T*3 + "KSkill* pChildBlur = (KSkill*)g_SkillManager.GetSkill(pSkill->GetChildSkillId(), pSkill->GetSkillLevel());" + NL +
            T*3 + "if (pChildBlur)" + NL +
            T*4 + "pChildBlur->Cast(m_Index, m_DesX, m_DesY);" + NL +
            T*2 + "}" + NL +
            T*2 + "m_Doing = do_blurmove;" + NL + T*2 + "break;" + NL + T + "case 1:" + NL)
    f.rep(old4, new4, MK + " Tap Dap Luu Tinh")

    # H5 sync: HS_SpAdd / HS_SpCost -> HS_SyncSp; client khong tu sua so tang (chi nhan dong bo)
    oldA = (T + "for (int i = 0; i < MAX_HS_SP; i++)" + NL + T + "{" + NL + T*2 + "if (m_HSSp[i].nKey != nKey) continue;" + NL +
            T*2 + "m_HSSp[i].nCount += nAdd;" + NL +
            T*2 + "if (m_HSSp[i].nMax > 0 && m_HSSp[i].nCount > m_HSSp[i].nMax) m_HSSp[i].nCount = m_HSSp[i].nMax;" + NL +
            T*2 + "if (m_HSSp[i].nCount < 0) m_HSSp[i].nCount = 0;" + NL +
            T*2 + "return;" + NL)
    newA = ("#ifndef _SERVER" + NL + T + "return;" + T + "// " + MK + " client: so tang chi nhan tu s2c_syncvhtd (khong tu cong khi Cast/ReCalc de khoi lech server)" + NL + "#endif" + NL +
            T + "for (int i = 0; i < MAX_HS_SP; i++)" + NL + T + "{" + NL + T*2 + "if (m_HSSp[i].nKey != nKey) continue;" + NL +
            T*2 + "int nTruoc = m_HSSp[i].nCount;" + T + "// " + MK + NL +
            T*2 + "m_HSSp[i].nCount += nAdd;" + NL +
            T*2 + "if (m_HSSp[i].nMax > 0 && m_HSSp[i].nCount > m_HSSp[i].nMax) m_HSSp[i].nCount = m_HSSp[i].nMax;" + NL +
            T*2 + "if (m_HSSp[i].nCount < 0) m_HSSp[i].nCount = 0;" + NL +
            T*2 + "if (m_HSSp[i].nCount != nTruoc) HS_SyncSp(nKey);" + T + "// " + MK + NL +
            T*2 + "return;" + NL)
    f.rep(oldA, newA, MK + " HS_SpAdd sync")
    oldC = (T*2 + "if (m_HSSp[i].nCount < nCost) return FALSE;" + NL + T*2 + "m_HSSp[i].nCount -= nCost;" + NL + T*2 + "return TRUE;" + NL)
    newC = (T*2 + "if (m_HSSp[i].nCount < nCost) return FALSE;" + NL + T*2 + "m_HSSp[i].nCount -= nCost;" + NL +
            T*2 + "HS_SyncSp(nKey);" + T + "// " + MK + NL + T*2 + "return TRUE;" + NL)
    f.rep(oldC, newC, MK + " HS_SpCost sync")
    # than ham HS_SyncSp / HS_SyncShield sau HS_ResetVhtd
    oldR = (T + "m_nHSAddLightMagicP = 0; m_nHSMeleeReturnResP = 0; m_nHSUnravel = 0;" + NL + "}" + NL)
    newR = (T + "m_nHSAddLightMagicP = 0; m_nHSMeleeReturnResP = 0; m_nHSUnravel = 0;" + NL + "}" + NL + NL +
            "// " + MK + " gui so tang No/Am Luat (m_HSSp[nKey]) toi client cua CHINH NGUOI CHOI (UiPlayerBar ve o icon + chu so)" + NL +
            "void KNpc::HS_SyncSp(int nKey)" + NL + "{" + NL +
            "#ifdef _SERVER" + NL +
            T + "if (m_Kind != kind_player || m_nPlayerIdx <= 0 || m_nPlayerIdx >= MAX_PLAYER || !g_pServer)" + NL + T*2 + "return;" + NL +
            T + "if (Player[m_nPlayerIdx].m_nNetConnectIdx == -1)" + NL + T*2 + "return;" + NL +
            T + "for (int i = 0; i < MAX_HS_SP; i++)" + NL + T + "{" + NL +
            T*2 + "if (m_HSSp[i].nKey != nKey) continue;" + NL +
            T*2 + "S2C_SYNC_VHTD sSync;" + NL +
            T*2 + "sSync.ProtocolType = s2c_syncvhtd;" + NL +
            T*2 + "sSync.btKind = 0;" + NL +
            T*2 + "sSync.wKey = (WORD)nKey;" + NL +
            T*2 + "sSync.nV1 = m_HSSp[i].nCount;" + NL +
            T*2 + "sSync.nV2 = m_HSSp[i].nMax;" + NL +
            T*2 + "g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sSync, sizeof(sSync));" + NL +
            T*2 + "return;" + NL + T + "}" + NL +
            "#endif" + NL + "}" + NL + NL +
            "// " + MK + " gui khien tinh (staticmagicshield_p: hien tai / toi da) toi client cua chinh minh - thanh 'ong mau thu 2' (Player_Shield)" + NL +
            "void KNpc::HS_SyncShield()" + NL + "{" + NL +
            "#ifdef _SERVER" + NL +
            T + "if (m_Kind != kind_player || m_nPlayerIdx <= 0 || m_nPlayerIdx >= MAX_PLAYER || !g_pServer)" + NL + T*2 + "return;" + NL +
            T + "if (Player[m_nPlayerIdx].m_nNetConnectIdx == -1)" + NL + T*2 + "return;" + NL +
            T + "S2C_SYNC_VHTD sSync;" + NL +
            T + "sSync.ProtocolType = s2c_syncvhtd;" + NL +
            T + "sSync.btKind = 1;" + NL +
            T + "sSync.wKey = 0;" + NL +
            T + "sSync.nV1 = m_CurrentStaticMagicShieldP;" + NL +
            T + "sSync.nV2 = m_nHSShieldMax;" + NL +
            T + "g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sSync, sizeof(sSync));" + NL +
            "#endif" + NL + "}" + NL)
    f.rep(oldR, newR, MK + " HS_SyncSp/HS_SyncShield")
    # khoi tao
    oldI = (T + "m_CurrentStaticMagicShieldP = 0;" + NL + T + "m_CurrentExpEnhance = 0;" + NL)
    newI = (T + "m_CurrentStaticMagicShieldP = 0;" + NL + T + "m_nHSShieldMax = 0;" + T + "// " + MK + NL + T + "m_CurrentExpEnhance = 0;" + NL)
    f.rep(oldI, newI, MK + " khoi tao m_nHSShieldMax")
    # CalcDamage: khien tinh hap thu / vo -> dong bo
    oldD1 = (T*4 + "m_CurrentStaticMagicShieldP -= nDamage;" + NL)
    newD1 = (T*4 + "m_CurrentStaticMagicShieldP -= nDamage;" + NL + T*4 + "HS_SyncShield();" + T + "// " + MK + NL)
    f.rep(oldD1, newD1, MK + " khien hap thu -> sync")
    oldD2 = (T*3 + "nDamage -= m_CurrentStaticMagicShieldP;" + NL + T*3 + "m_CurrentStaticMagicShieldP = 0;" + NL)
    newD2 = (T*3 + "nDamage -= m_CurrentStaticMagicShieldP;" + NL + T*3 + "m_CurrentStaticMagicShieldP = 0;" + NL + T*3 + "HS_SyncShield();" + T + "// " + MK + NL)
    f.rep(oldD2, newD2, MK + " khien vo -> sync")
    f.save()

# ------------------------------------------------------------------ KNpc.h
def h_knpc_h():
    f = F(os.path.join(CORE, "KNpc.h"))
    old = (T + "int" + T*5 + "m_nHSUnravel;" + T*3 + "// [VHTD 02/09] unravel_effect (chua co co che)" + NL)
    new = (old + T + "int" + T*5 + "m_nHSShieldMax;" + T*2 + "// " + MK + " khien tinh toi da luc ap (client: thanh Player_Shield)" + NL)
    f.rep(old, new, MK + " m_nHSShieldMax")
    old2 = (T + "void" + T*4 + "HS_ResetVhtd();" + T*5 + "// [VHTD 02/09] reset truong tinh lai thuoc tinh (khong reset so tang)" + NL)
    new2 = (old2 + T + "void" + T*4 + "HS_SyncSp(int nKey);" + T*4 + "// " + MK + " server: gui so tang toi client cua chinh minh (client: rong)" + NL +
            T + "void" + T*4 + "HS_SyncShield();" + T*4 + "// " + MK + " server: gui khien tinh hien tai/toi da (client: rong)" + NL)
    f.rep(old2, new2, MK + " HS_Sync decl")
    f.save()

# ------------------------------------------------------------------ KNpcAttribModify.cpp
def h_attrib():
    f = F(os.path.join(CORE, "KNpcAttribModify.cpp"))
    old = (T + "if (pMagic->nValue[0] > 0)" + NL +
           T*2 + "pNpc->m_CurrentStaticMagicShieldP += (int)((__int64)pNpc->m_CurrentManaMax * pMagic->nValue[0] / MAX_PERCENT);" + NL +
           T + "else" + NL +
           T*2 + "pNpc->m_CurrentStaticMagicShieldP = 0;" + NL + "}" + NL)
    new = ("#ifndef _SERVER" + NL + T + "return;" + T + "// " + MK + " client: gia tri khien chi nhan tu s2c_syncvhtd" + NL + "#endif" + NL +
           T + "if (pMagic->nValue[0] > 0)" + NL +
           T*2 + "pNpc->m_CurrentStaticMagicShieldP += (int)((__int64)pNpc->m_CurrentManaMax * pMagic->nValue[0] / MAX_PERCENT);" + NL +
           T + "else" + NL +
           T*2 + "pNpc->m_CurrentStaticMagicShieldP = 0;" + NL +
           T + "pNpc->m_nHSShieldMax = pNpc->m_CurrentStaticMagicShieldP;" + T + "// " + MK + " tran thanh khien = be luc ap (0 khi go)" + NL +
           T + "pNpc->HS_SyncShield();" + NL + "}" + NL)
    f.rep(old, new, MK + " StaticMagicShieldP sync")
    # SpecialPointBase: client khong tu dat; server sync sau khi dat
    oldB = (T + "KMagicAttrib* pMagic = (KMagicAttrib *)pData;" + NL + T + "if (pMagic->nValue[0] <= 0)" + NL + T*2 + "return;" + NL +
            T + "int nFree = -1;" + NL)
    newB = (T + "KMagicAttrib* pMagic = (KMagicAttrib *)pData;" + NL + T + "if (pMagic->nValue[0] <= 0)" + NL + T*2 + "return;" + NL +
            "#ifndef _SERVER" + NL + T + "return;" + T + "// " + MK + " client: khoa/tran chi nhan tu s2c_syncvhtd" + NL + "#endif" + NL +
            T + "int nFree = -1;" + NL)
    f.rep(oldB, newB, MK + " SpecialPointBase client")
    oldB2 = (T*3 + "if (pNpc->m_HSSp[i].nCount > pNpc->m_HSSp[i].nMax) pNpc->m_HSSp[i].nCount = pNpc->m_HSSp[i].nMax;" + NL + T*3 + "return;" + NL)
    newB2 = (T*3 + "if (pNpc->m_HSSp[i].nCount > pNpc->m_HSSp[i].nMax) pNpc->m_HSSp[i].nCount = pNpc->m_HSSp[i].nMax;" + NL +
             T*3 + "pNpc->HS_SyncSp(pMagic->nValue[0]);" + T + "// " + MK + NL + T*3 + "return;" + NL)
    f.rep(oldB2, newB2, MK + " SpecialPointBase sync 1")
    oldB3 = (T*2 + "pNpc->m_HSSp[nFree].nMax = pMagic->nValue[2];" + NL + T*2 + "pNpc->m_HSSp[nFree].nCount = 0;" + NL + T + "}" + NL)
    newB3 = (T*2 + "pNpc->m_HSSp[nFree].nMax = pMagic->nValue[2];" + NL + T*2 + "pNpc->m_HSSp[nFree].nCount = 0;" + NL +
             T*2 + "pNpc->HS_SyncSp(pMagic->nValue[0]);" + T + "// " + MK + NL + T + "}" + NL)
    f.rep(oldB3, newB3, MK + " SpecialPointBase sync 2")
    f.save()

# ------------------------------------------------------------------ KPlayer.cpp (UpdataCurData: client giu khien dong bo)
def h_kplayer():
    f = F(os.path.join(CORE, "KPlayer.cpp"))
    old = (T + "Npc[m_nIndex].m_CurrentStaticMagicShieldP = 0;" + NL + T + "Npc[m_nIndex].m_CurrentReturnResPercent = 0;" + NL)
    new = ("#ifdef _SERVER" + NL + T + "Npc[m_nIndex].m_CurrentStaticMagicShieldP = 0;" + T + "// " + MK + " client: gia tri khien chi tu s2c_syncvhtd (khong xoa khi tinh lai)" + NL +
           T + "Npc[m_nIndex].m_nHSShieldMax = 0;" + NL + "#endif" + NL +
           T + "Npc[m_nIndex].m_CurrentReturnResPercent = 0;" + NL)
    f.rep(old, new, MK + " UpdataCurData khien")
    f.save()

# ------------------------------------------------------------------ H3 + H5 KProtocolProcess.cpp
def h_proto_process():
    f = F(os.path.join(CORE, "KProtocolProcess.cpp"))
    old3 = (T + "if (MapX == -1)" + NL + T + "{" + NL +
            T*2 + "if (MapY < 0 ) return;" + NL +
            T*2 + "MapY = NpcSet.SearchID(MapY);" + NL +
            T*2 + "if (MapY == 0)" + T + "return;" + NL +
            T*2 + "if (Npc[MapY].m_RegionIndex < 0)" + NL + T*3 + "return;" + NL +
            T*2 + "int nX, nY;" + NL)
    new3 = (T + "int nVhtdTargetIdx = 0;" + T + "// " + MK + " chi so NPC muc tieu (MapX == -1): truyen thang cho Cast nhu server -> dan Follow (1363) bam muc tieu" + NL +
            T + "if (MapX == -1)" + NL + T + "{" + NL +
            T*2 + "if (MapY < 0 ) return;" + NL +
            T*2 + "MapY = NpcSet.SearchID(MapY);" + NL +
            T*2 + "if (MapY == 0)" + T + "return;" + NL +
            T*2 + "if (Npc[MapY].m_RegionIndex < 0)" + NL + T*3 + "return;" + NL +
            T*2 + "nVhtdTargetIdx = (int)MapY;" + NL +
            T*2 + "int nX, nY;" + NL)
    f.rep(old3, new3, MK + " castdirectly target idx")
    oldC = ("    pOrdinSkill->Cast(nIdx, MapX, MapY);" + NL)
    newC = ("    if (nVhtdTargetIdx > 0)" + NL +
            "        pOrdinSkill->Cast(nIdx, -1, nVhtdTargetIdx);" + T + "// " + MK + " nhu KNpc::Cast(int,int)/CastAutoSkillAt tren server (KSkill::Cast nhan -1 + chi so)" + NL +
            "    else" + NL +
            "        pOrdinSkill->Cast(nIdx, MapX, MapY);" + NL)
    f.rep(oldC, newC, MK + " castdirectly Cast")
    # H5 dang ky + than ham
    oldReg = (T + "ProcessFunc[s2c_reduceskillcd] = &KProtocolProcess::s2cReduceSkillCD;" + T + "// [HOASON 01/09b]" + NL)
    newReg = (oldReg + T + "ProcessFunc[s2c_syncvhtd] = &KProtocolProcess::s2cSyncVhtd;" + T + "// " + MK + NL)
    f.rep(oldReg, newReg, MK + " ProcessFunc")
    oldImpl = (T*2 + "Npc[nNpc].m_SkillList.ReduceCoolDown((int)pSync->m_wSkillId, (int)pSync->m_wFrames);" + NL + "}" + NL + "#endif" + NL)
    newImpl = (T*2 + "Npc[nNpc].m_SkillList.ReduceCoolDown((int)pSync->m_wSkillId, (int)pSync->m_wFrames);" + NL + "}" + NL + NL +
               "// " + MK + " so tang No/Am Luat (btKind 0) + khien tinh (btKind 1) cua CHINH MINH - UiPlayerBar / Player_Shield doc qua GDI" + NL +
               "void KProtocolProcess::s2cSyncVhtd(BYTE* pMsg)" + NL + "{" + NL +
               T + "S2C_SYNC_VHTD* pSync = (S2C_SYNC_VHTD*)pMsg;" + NL +
               T + "if (pSync->ProtocolType != s2c_syncvhtd)" + NL + T*2 + "return;" + NL +
               T + "int nNpc = Player[CLIENT_PLAYER_INDEX].m_nIndex;" + NL +
               T + "if (nNpc <= 0 || nNpc >= MAX_NPC)" + NL + T*2 + "return;" + NL +
               T + "if (pSync->btKind == 0)" + NL + T + "{" + NL +
               T*2 + "int nFree = -1;" + NL +
               T*2 + "for (int i = 0; i < MAX_HS_SP; i++)" + NL + T*2 + "{" + NL +
               T*3 + "if (Npc[nNpc].m_HSSp[i].nKey == (int)pSync->wKey)" + NL + T*3 + "{" + NL +
               T*4 + "Npc[nNpc].m_HSSp[i].nCount = pSync->nV1;" + NL +
               T*4 + "Npc[nNpc].m_HSSp[i].nMax = pSync->nV2;" + NL +
               T*4 + "return;" + NL + T*3 + "}" + NL +
               T*3 + "if (nFree < 0 && Npc[nNpc].m_HSSp[i].nKey == 0) nFree = i;" + NL +
               T*2 + "}" + NL +
               T*2 + "if (nFree >= 0)" + NL + T*2 + "{" + NL +
               T*3 + "Npc[nNpc].m_HSSp[nFree].nKey = (int)pSync->wKey;" + NL +
               T*3 + "Npc[nNpc].m_HSSp[nFree].nCount = pSync->nV1;" + NL +
               T*3 + "Npc[nNpc].m_HSSp[nFree].nMax = pSync->nV2;" + NL +
               T*2 + "}" + NL + T + "}" + NL +
               T + "else if (pSync->btKind == 1)" + NL + T + "{" + NL +
               T*2 + "Npc[nNpc].m_CurrentStaticMagicShieldP = pSync->nV1;" + NL +
               T*2 + "Npc[nNpc].m_nHSShieldMax = pSync->nV2;" + NL +
               T + "}" + NL + "}" + NL + "#endif" + NL)
    f.rep(oldImpl, newImpl, MK + " s2cSyncVhtd")
    f.save()

def h_proto_h():
    f = F(os.path.join(CORE, "KProtocolProcess.h"))
    old = (T + "void" + T + "s2cReduceSkillCD(BYTE* pMsg);" + T + "// [HOASON 01/09b]" + NL)
    new = (old + T + "void" + T + "s2cSyncVhtd(BYTE* pMsg);" + T + "// " + MK + NL)
    f.rep(old, new, MK + " decl")
    f.save()

def h_proto_def():
    f = F(os.path.join(ROOT, "Headers", "KProtocolDef.h"))
    old = (T + "s2c_reduceskillcd,")
    m = re.search(re.escape(T + "s2c_reduceskillcd,") + r"[^\r\n]*\r?\n", f.s)
    if not m:
        raise SystemExit("khong thay s2c_reduceskillcd trong KProtocolDef.h")
    line = m.group(0)
    new = (line + T + "s2c_syncvhtd," + T*3 + "// " + MK + " dong bo so tang No/Am Luat + khien tinh toi client cua chinh minh (S2C_SYNC_VHTD)" + NL)
    f.rep(line, new, MK + " enum s2c_syncvhtd")
    f.save()

def h_proto_cpp():
    f = F(os.path.join(CORE, "KProtocol.cpp"))
    old = (T + "sizeof(S2C_REDUCE_SKILL_CD),")
    m = re.search(re.escape(old) + r"[^\r\n]*\r?\n", f.s)
    if not m:
        raise SystemExit("khong thay sizeof(S2C_REDUCE_SKILL_CD) trong KProtocol.cpp")
    line = m.group(0)
    new = (line + T + "sizeof(S2C_SYNC_VHTD)," + T*3 + "// s2c_syncvhtd " + MK + NL)
    f.rep(line, new, MK + " size s2c_syncvhtd")
    f.save()

def h_proto_struct():
    f = F(os.path.join(CORE, "KProtocol.h"))
    old = ("} S2C_REDUCE_SKILL_CD;" + NL)
    new = (old + NL +
           "// " + MK + " dong bo Vu Hon/Tieu Dao toi client cua NGUOI CHOI: btKind 0 = so tang (wKey = id ky nang khoa 1976 No / 2116 Am Luat," + NL +
           "// nV1 = tang hien co, nV2 = tran); btKind 1 = khien tinh staticmagicshield_p (nV1 = con lai, nV2 = toi da luc ap)." + NL +
           "typedef struct" + NL + "{" + NL +
           T + "BYTE" + T + "ProtocolType;" + T*2 + "// s2c_syncvhtd" + NL +
           T + "BYTE" + T + "btKind;" + NL +
           T + "WORD" + T + "wKey;" + NL +
           T + "int" + T*2 + "nV1;" + NL +
           T + "int" + T*2 + "nV2;" + NL +
           "} S2C_SYNC_VHTD;" + NL)
    f.rep(old, new, MK + " S2C_SYNC_VHTD")
    f.save()

# ------------------------------------------------------------------ CoreShell / GameDataDef
def h_coreshell():
    f = F(os.path.join(CORE, "CoreShell.h"))
    old = (T + "GDI_FUSION_INFO," + NL + NL + "};" + NL)
    new = (T + "GDI_FUSION_INFO," + NL + NL +
           T + "// " + MK + " so tang No (1976) / Am Luat (2116) cua chinh minh: uParam = KHsSpView[nParam], tra so muc co tran > 0. Dat CUOI enum." + NL +
           T + "GDI_HS_SP," + NL + NL + "};" + NL)
    f.rep(old, new, MK + " GDI_HS_SP")
    f.save()
    g = F(os.path.join(CORE, "GameDataDef.h"))
    old2 = ("struct KStateTempNode" + NL + "{" + NL + T + "int" + T*2 + "nSkillId;" + NL + T + "int" + T*2 + "nLeftTime;" + NL + "};" + NL)
    new2 = (old2 + NL + "// " + MK + " o 'tang' No/Am Luat (GDI_HS_SP)" + NL +
            "struct KHsSpView" + NL + "{" + NL + T + "int" + T*2 + "nKey;" + NL + T + "int" + T*2 + "nCount;" + NL + T + "int" + T*2 + "nMax;" + NL + "};" + NL)
    g.rep(old2, new2, MK + " KHsSpView")
    old3 = (T + "unsigned short" + T + "wReserved;" + T + "//")
    m = re.search(re.escape(old3) + r"[^\r\n]*\r?\n\};\r?\n", g.s)
    if not m:
        raise SystemExit("khong thay wReserved trong KUiPlayerRuntimeInfo")
    blk = m.group(0)
    new3 = blk.replace("};" + g.nl, T + "int" + T*2 + "nShield;" + T*2 + "// " + MK + " khien tinh con lai (0 = khong co)" + g.nl +
                       T + "int" + T*2 + "nShieldFull;" + T + "// " + MK + " khien tinh toi da luc ap" + g.nl + "};" + g.nl)
    assert new3 != blk
    g.rep(blk, new3, MK + " nShield")
    g.save()
    c = F(os.path.join(CORE, "CoreShell.cpp"))
    old4 = (T*3 + "pInfo->wReserved = 0;" + NL + T*2 + "}" + NL + T*2 + "break;" + NL)
    new4 = (T*3 + "pInfo->wReserved = 0;" + NL +
            T*3 + "pInfo->nShield = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStaticMagicShieldP;" + T + "// " + MK + NL +
            T*3 + "pInfo->nShieldFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nHSShieldMax;" + NL +
            T*2 + "}" + NL + T*2 + "break;" + NL)
    c.rep(old4, new4, MK + " RT_INFO nShield")
    old5 = (T + "case GDI_EXBOX_ID:")
    new5 = (T + "case GDI_HS_SP:" + T + "// " + MK + " uParam = KHsSpView[nParam]; tra so muc No/Am Luat co tran > 0" + NL +
            T*2 + "nRet = 0;" + NL + T*2 + "{" + NL +
            T*3 + "int nMe = Player[CLIENT_PLAYER_INDEX].m_nIndex;" + NL +
            T*3 + "if (nMe > 0 && nMe < MAX_NPC)" + NL + T*3 + "{" + NL +
            T*4 + "KHsSpView* pV = (KHsSpView*)uParam;" + NL +
            T*4 + "for (int i = 0; i < MAX_HS_SP; i++)" + NL + T*4 + "{" + NL +
            T*5 + "if (Npc[nMe].m_HSSp[i].nKey <= 0 || Npc[nMe].m_HSSp[i].nMax <= 0) continue;" + NL +
            T*5 + "if (pV && nRet < nParam)" + NL + T*5 + "{" + NL +
            T*6 + "pV[nRet].nKey = Npc[nMe].m_HSSp[i].nKey;" + NL +
            T*6 + "pV[nRet].nCount = Npc[nMe].m_HSSp[i].nCount;" + NL +
            T*6 + "pV[nRet].nMax = Npc[nMe].m_HSSp[i].nMax;" + NL +
            T*5 + "}" + NL + T*5 + "nRet++;" + NL +
            T*4 + "}" + NL + T*3 + "}" + NL + T*2 + "}" + NL + T*2 + "break;" + NL +
            T + "case GDI_EXBOX_ID:")
    c.rep(old5, new5, MK + " GDI_HS_SP case")
    c.save()

# ------------------------------------------------------------------ UI (S3Client)
def h_ui():
    h = F(os.path.join(S3, "UiShell.h"))
    old = ("class Player_Mana : public KWndImageTextButton" + NL + "{" + NL + "public:" + NL +
           T + "DECLARE_COMCLASS(Player_Mana)" + NL + T + "void UpdateData();" + NL + T + "void OnButtonClick();" + NL +
           T + "static bool m_bText;" + NL + "};" + NL)
    new = (old + NL + "// " + MK + " thanh khien tinh (staticmagicshield_p: Tieu Dao Vu 2139, Phat Y_Thuan 2134...) - 'ong mau thu 2' de len thanh sinh luc" + NL +
           "class Player_Shield : public KWndImageTextButton" + NL + "{" + NL + "public:" + NL +
           T + "DECLARE_COMCLASS(Player_Shield)" + NL + T + "void UpdateData();" + NL + T + "void OnButtonClick();" + NL + "};" + NL)
    h.rep(old, new, MK + " Player_Shield decl")
    h.save()
    c = F(os.path.join(S3, "UiShell.cpp"))
    oldR = (T + "Player_Mana::RegisterSelfClass();" + NL)
    newR = (oldR + T + "Player_Shield::RegisterSelfClass();" + T + "// " + MK + NL)
    c.rep(oldR, newR, MK + " RegisterSelfClass")
    oldI = ("IMPLEMENT_COMCLASS(Player_Stamina)" + NL)
    newI = ("// " + MK + " thanh khien tinh: chi hien khi con khien (UiHeaderControlBar.ini [Shield] ClassType=Player_Shield)" + NL +
            "IMPLEMENT_COMCLASS(Player_Shield)" + NL +
            "void Player_Shield::UpdateData()" + NL + "{" + NL +
            T + "if (g_pCoreShell)" + NL + T + "{" + NL +
            T*2 + "KUiPlayerRuntimeInfo" + T + "Info;" + NL +
            T*2 + "memset(&Info, 0, sizeof(KUiPlayerRuntimeInfo));" + NL +
            T*2 + "g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (int)&Info, 0);" + NL +
            T*2 + "if (Info.nShield <= 0 || Info.nShieldFull <= 0)" + NL + T*2 + "{" + NL +
            T*3 + "Hide();" + NL + T*3 + "return;" + NL + T*2 + "}" + NL +
            T*2 + "if (Info.nShield > Info.nShieldFull)" + NL + T*3 + "Info.nShieldFull = Info.nShield;" + NL +
            T*2 + "Show();" + NL +
            T*2 + "Set2IntValue(Info.nShield, Info.nShieldFull);" + NL +
            T*2 + "SetText(NULL, 0);" + NL +
            T + "}" + NL + "}" + NL + NL +
            "void Player_Shield::OnButtonClick()" + NL + "{" + NL + T + "UpdateData();" + NL + "}" + NL + NL +
            "IMPLEMENT_COMCLASS(Player_Stamina)" + NL)
    c.rep(oldI, newI, MK + " Player_Shield impl")
    c.save()
    p = F(os.path.join(S3, "UiCase", "UiPlayerBar.cpp"))
    oldP = (T*3 + "free(pNode);" + NL + T*3 + "pNode = NULL;" + NL + NL + T*2 + "}" + NL +
            T*2 + "m_StateImg[i].Hide();" + NL + T*2 + "m_StateLife[i].Hide();" + NL + T + "}" + NL + "}" + NL)
    newP = (T*3 + "free(pNode);" + NL + T*3 + "pNode = NULL;" + NL + NL + T*2 + "}" + NL +
            T*2 + "m_StateImg[i].Hide();" + NL + T*2 + "m_StateLife[i].Hide();" + NL + T + "}" + NL +
            T + "// " + MK + " o icon TANG No (1976) / Am Luat (2116) ngay sau cac trang thai: anh = StatePos.ini [id khoa], chu so = tang hien co," + NL +
            T + "// tooltip 'ten / mo ta / tang/tran'. Du lieu tu s2c_syncvhtd (GDI_HS_SP)." + NL +
            T + "{" + NL +
            T*2 + "KHsSpView sp[MAX_HS_SP];" + NL +
            T*2 + "int nSp = g_pCoreShell->GetGameData(GDI_HS_SP, (unsigned int)sp, MAX_HS_SP);" + NL +
            T*2 + "if (nSp > MAX_HS_SP) nSp = MAX_HS_SP;" + NL +
            T*2 + "int nBase = nCount;" + NL +
            T*2 + "if (nBase < 0) nBase = 0;" + NL +
            T*2 + "for (int k = 0; k < nSp && nBase + k < MAX_BUTTON_STATE - 5; k++)" + NL + T*2 + "{" + NL +
            T*3 + "int s = nBase + k;" + NL +
            T*3 + "int nKey = sp[k].nKey;" + NL +
            T*3 + "BOOL bCo = (ms_pStateList && nKey > 0 && nKey < MAX_SKILL);" + NL +
            T*3 + "if (bCo && ms_pStateList[nKey].szImage[0])" + NL +
            T*4 + "m_StateImg[s].SetImage(ISI_T_SPR, ms_pStateList[nKey].szImage, true);" + NL +
            T*3 + "else" + NL +
            T*4 + "m_StateImg[s].SetImage(ISI_T_SPR, \"\\\\spr\\\\item\\\\medecine\\\\obj-potion-15.spr\", true);" + NL +
            T*3 + "m_StateImg[s].Show();" + NL +
            T*3 + "char szSpTip[160] = \"\";" + NL +
            T*3 + "sprintf(szSpTip, \"%s \\n%s \\n %d/%d\", bCo ? ms_pStateList[nKey].szName : \"\", bCo ? ms_pStateList[nKey].szDesc : \"\", sp[k].nCount, sp[k].nMax);" + NL +
            T*3 + "m_StateImg[s].SetToolTipInfo(szSpTip, strlen(szSpTip));" + NL +
            T*3 + "char szSpNum[16];" + NL +
            T*3 + "sprintf(szSpNum, \"%d\", sp[k].nCount);" + NL +
            T*3 + "m_StateLife[s].SetText(szSpNum);" + NL +
            T*3 + "m_StateLife[s].Show();" + NL +
            T*2 + "}" + NL + T + "}" + NL + "}" + NL)
    p.rep(oldP, newP, MK + " o icon tang")
    p.save()

if __name__ == "__main__":
    print("vhtd_engine_patch4 %s%s" % (MK, " (KIEM)" if KIEM else ""))
    h1(); h2(); h_knpc_h(); h_attrib(); h_kplayer(); h_proto_process(); h_proto_h(); h_proto_def(); h_proto_cpp(); h_proto_struct(); h_coreshell(); h_ui()
    print("XONG.")
