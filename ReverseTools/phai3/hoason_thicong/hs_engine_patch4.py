# -*- coding: ascii -*-
"""hs_engine_patch4.py - HOA SON dot f (02/09): ky nang TU PHONG (auto*skill) + hoi noi luc chuan Linux + mo ta client.
Doc/ghi latin-1 (giu nguyen byte TCVN3). Idempotent: anchor cu khong con + marker moi da co -> bo qua.
DUNG:  python hs_engine_patch4.py [--kiem]
Sua:  Core\\Src\\KNpc.h, KNpc.cpp, KNpcAttribModify.cpp, KMagicDesc.cpp  (marker [HOASON 02/09])
"""
import io, os, sys

ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
KIEM = "--kiem" in sys.argv
MARK = "[HOASON 02/09]"

def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()

def wr(p, s):
    if KIEM:
        return
    io.open(p, "w", encoding="latin-1", newline="").write(s)

def hib(s):
    return sum(1 for c in s if ord(c) >= 0x80)

class F:
    def __init__(self, name):
        self.p = os.path.join(ROOT, name)
        self.name = name
        self.s = rd(self.p)
        self.orig = self.s
        self.n = 0
    def rep(self, old, new, count=1, tag=""):
        c = self.s.count(old)
        if c == 0:
            if new.strip() and new.strip() in self.s:
                print("  [=] %s: da ap (%s)" % (self.name, tag))
                return
            raise SystemExit("KHONG THAY anchor trong %s (%s):\n%s" % (self.name, tag, old[:200]))
        if count == 1 and c != 1:
            raise SystemExit("anchor KHONG DUY NHAT (%d) trong %s (%s)" % (c, self.name, tag))
        self.s = self.s.replace(old, new, count)
        self.n += 1
        print("  [+] %s: %s" % (self.name, tag))
    def rep_first_after(self, anchor_fn, old, new, tag=""):
        i0 = self.s.find(anchor_fn)
        if i0 < 0:
            raise SystemExit("khong thay ham %s trong %s" % (anchor_fn, self.name))
        i = self.s.find(old, i0)
        if i < 0:
            if new.strip() in self.s[i0:]:
                print("  [=] %s: da ap (%s)" % (self.name, tag))
                return
            raise SystemExit("KHONG THAY anchor sau %s trong %s (%s)" % (anchor_fn, self.name, tag))
        self.s = self.s[:i] + new + self.s[i + len(old):]
        self.n += 1
        print("  [+] %s: %s" % (self.name, tag))
    def rep_func_body(self, sig_prefix, new_body, tag=""):
        """thay toan bo than ham (tu '{' sau signature den '}' can) bang new_body (khong gom ngoac)."""
        i = self.s.find(sig_prefix)
        if i < 0:
            raise SystemExit("khong thay ham %s trong %s" % (sig_prefix, self.name))
        j = self.s.find("{", i)
        depth = 0
        k = j
        while k < len(self.s):
            ch = self.s[k]
            if ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
                if depth == 0:
                    break
            k += 1
        old_body = self.s[j + 1:k]
        if MARK in old_body:
            print("  [=] %s: da ap (%s)" % (self.name, tag))
            return
        self.s = self.s[:j + 1] + new_body + self.s[k:]
        self.n += 1
        print("  [+] %s: %s" % (self.name, tag))
    def save(self):
        if self.s == self.orig:
            print("  (khong doi) %s" % self.name)
            return
        if hib(self.s) != hib(self.orig):
            raise SystemExit("LECH SO BYTE CAO trong %s: %d -> %d" % (self.name, hib(self.orig), hib(self.s)))
        if "\xef\xbf\xbd" in self.s:
            raise SystemExit("EF BF BD trong %s" % self.name)
        wr(self.p, self.s)
        print("  => ghi %s (%d cho%s)" % (self.name, self.n, ", KIEM - khong ghi" if KIEM else ""))

# ----------------------------------------------------------------------------------------------
# 1. KNpc.h
# ----------------------------------------------------------------------------------------------
h = F("KNpc.h")
h.rep("\tvoid\t\t\t\tRescueSkill();\r\n",
      "\tvoid\t\t\t\tRescueSkill(int nAttacker = 0);\t// [HOASON 02/09] Linux Fire(+0x1898, chu, ke danh): loai 1 nham ke danh, khac nham minh\r\n",
      tag="RescueSkill(nAttacker)")
h.rep("\tvoid\t\t\t\tDeathSkill();\r\n",
      "\tvoid\t\t\t\tDeathSkill();\r\n"
      "\tvoid\t\t\t\tCastAutoSkillAt(int nSkillId, int nSkillLevel, int nTarget);\t// [HOASON 02/09] tu phong nham muc tieu: phat s2c_castskilldirectly (client ve dan) roi Cast(m_Index, -1, nTarget)\r\n",
      tag="decl CastAutoSkillAt")
h.save()

# ----------------------------------------------------------------------------------------------
# 2. KNpcAttribModify.cpp - 4 handler auto*skill dung chung HS_AutoSkillModify (Linux 0x08189000)
# ----------------------------------------------------------------------------------------------
m = F("KNpcAttribModify.cpp")
HELPER = (
"// [HOASON 02/09] AUTO SKILL (autodeath/autoattack/autoreply/autorescueskill) CHUAN LINUX 0x08189000 (danh sach +0x18bc/+0x1874/+0x1850/+0x1898):\r\n"
"// khoa = (|v0| & 0xffffff) = id*256 + cap (byte cao cua |v0| = LOAI nham: 1 = ke danh/muc tieu, khac = chinh minh);\r\n"
"// ty le = v2 & 0xff, hoi chieu = v2 >> 8 (khung). v2 > 0 (ap trang thai): cung khoa -> CONG DON ty le, chua co -> them muc.\r\n"
"// v2 < 0 (go trang thai - gia tri bi dao dau o KStateNode): TRU ty le, ve 0 -> XOA muc. Ban cu: (a) go trang thai tao muc rac\r\n"
"// id 64172 (so am & 0xffffff) chiem cho, (b) muc them lan dau KHONG BAO GIO bi go -> buff het han van tu phong, buff lai -> 2 muc.\r\n"
"static void HS_AutoSkillModify(KMagicAutoSkill* pList, KNpc* pNpc, KMagicAttrib* pMagic)\r\n"
"{\r\n"
"\tif (!pList || !pNpc || !pMagic || pMagic->nValue[0] == 0)\r\n"
"\t\treturn;\r\n"
"\tint nV0 = pMagic->nValue[0];\r\n"
"\tif (nV0 < 0)\r\n"
"\t\tnV0 = -nV0;\r\n"
"\tint nKey = nV0 & 0xffffff;\r\n"
"\tint nSkillId = nKey / 256;\r\n"
"\tint nLevel = nKey % 256;\r\n"
"\tint nType = nV0 >> 24;\r\n"
"\tint nV2 = pMagic->nValue[2];\r\n"
"\tint nRate, nWait;\r\n"
"\tif (nV2 > 0)\r\n"
"\t{\r\n"
"\t\tnRate = nV2 % 256;\r\n"
"\t\tnWait = nV2 / 256;\r\n"
"\t}\r\n"
"\telse\r\n"
"\t{\r\n"
"\t\tnRate = -((-nV2) % 256);\r\n"
"\t\tnWait = (-nV2) / 256;\r\n"
"\t}\r\n"
"\tif (nSkillId <= 0)\r\n"
"\t\treturn;\r\n"
"\tint nFound = -1, nFree = -1;\r\n"
"\tfor (int i = 0; i < MAX_AUTOSKILL; i++)\r\n"
"\t{\r\n"
"\t\tif (pList[i].nSkillId == nSkillId && pList[i].nSkillLevel == nLevel)\r\n"
"\t\t{\r\n"
"\t\t\tnFound = i;\r\n"
"\t\t\tbreak;\r\n"
"\t\t}\r\n"
"\t\tif (nFree < 0 && pList[i].nSkillId <= 0)\r\n"
"\t\t\tnFree = i;\r\n"
"\t}\r\n"
"\tif (nFound >= 0)\r\n"
"\t{\r\n"
"\t\tpList[nFound].nRate += nRate;\r\n"
"\t\tif (pList[nFound].nRate <= 0)\r\n"
"\t\t{\r\n"
"\t\t\tmemset(&pList[nFound], 0, sizeof(KMagicAutoSkill));\t// Linux xoa nut khi ty le ve 0\r\n"
"\t\t\treturn;\r\n"
"\t\t}\r\n"
"\t\tif (nV2 > 0)\r\n"
"\t\t{\r\n"
"\t\t\tpList[nFound].nType = nType;\r\n"
"\t\t\tpList[nFound].nWaitCastTime = nWait;\r\n"
"\t\t}\r\n"
"\t\treturn;\r\n"
"\t}\r\n"
"\tif (nV2 <= 0 || nFree < 0)\r\n"
"\t\treturn;\t\t// go ma khong co muc (Linux tao nut ty le am - vo tac dung) hoac het cho\r\n"
"\tpList[nFree].nSkillId = nSkillId;\r\n"
"\tpList[nFree].nSkillLevel = nLevel;\r\n"
"\tpList[nFree].nType = nType;\r\n"
"\tpList[nFree].nRate = nRate;\r\n"
"\tpList[nFree].nWaitCastTime = nWait;\r\n"
"\tpList[nFree].dwNextCastTime = SubWorld[pNpc->m_SubWorldIndex].m_dwCurrentTime;\r\n"
"}\r\n"
"\r\n")
sig_death = "void KNpcAttribModify::AutoDeathSkill( KNpc* pNpc, void* pData )"
if HELPER not in m.s:
    m.rep(sig_death, HELPER + sig_death, tag="helper HS_AutoSkillModify")
else:
    print("  [=] KNpcAttribModify.cpp: helper da co")
for fn, lst in (("AutoDeathSkill", "m_DeathSkill"), ("AutoAttackSkill", "m_AttackSkill"),
                ("AutoReplySkill", "m_ReplySkill"), ("AutoRescueSkill", "m_RescueSkill")):
    m.rep_func_body("void KNpcAttribModify::%s( KNpc* pNpc, void* pData )" % fn,
                    "\r\n\tHS_AutoSkillModify(pNpc->%s, pNpc, (KMagicAttrib *)pData);\t// %s Linux 0x08189000 (cong don / go dung khoa)\r\n" % (lst, MARK),
                    tag=fn)
m.save()

# ----------------------------------------------------------------------------------------------
# 3. KMagicDesc.cpp - #lA- : bo byte LOAI
# ----------------------------------------------------------------------------------------------
d = F("KMagicDesc.cpp")
d.rep("\t\t\tcase 'A':\r\n\t\t\t\tnValue = (int)(pAttrib->nValue[0] / 256);\r\n",
      "\t\t\tcase 'A':\r\n\t\t\t\tnValue = (int)((pAttrib->nValue[0] & 0xffffff) / 256);\t// [HOASON 02/09] bo byte LOAI (bit 24-31, Linux 0x080973D0): 1364 autoreplyskill v0 = (1*65536+1363)*256+cap -> truoc ra id 66899, ten ky nang trong\r\n",
      tag="#lA- mask 24 bit")
d.save()

# ----------------------------------------------------------------------------------------------
# 4. KNpc.cpp
# ----------------------------------------------------------------------------------------------
c = F("KNpc.cpp")

# 4.1 Reply/Rescue/Attack + CastAutoSkillAt (thay tron tu 'void KNpc::ReplySkill' den truoc 'void KNpc::DeathSkill()')
i0 = c.s.find("void KNpc::ReplySkill(int nLauncher)")
i1 = c.s.find("void KNpc::DeathSkill()")
if i0 < 0 or i1 < 0 or i1 < i0:
    raise SystemExit("khong thay khoi ReplySkill..DeathSkill")
old_block = c.s[i0:i1]
if MARK in old_block:
    print("  [=] KNpc.cpp: khoi Reply/Rescue/Attack da ap")
else:
    NEW_BLOCK = (
"// [HOASON 02/09] TU PHONG NHAM MUC TIEU (autoreply loai 1 -> ke danh; autoattack loai 0 -> nan nhan).\r\n"
"// Linux Fire 0x08188BB0: sau Cast (0x080EA920) PHAT GOI skillcast 0x85 (0x0807A870) cho ca vung. JX1 dot e chi Cast tren may\r\n"
"// chu: JX1 KHONG dong bo tung vien dan, client chi tu mo phong dan khi nhan s2c_skillcast/s2c_castskilldirectly => nguoi choi\r\n"
"// thay muc tieu mat mau (log S4-MSL-HIT 1363) ma KHONG THAY 3 kiem bay => 'skill chua co tac dung'. Nay phat s2c_castskilldirectly\r\n"
"// (client s2cDirectlyCastSkill: nMpsX=-1, nMpsY=ID muc tieu -> Cast tai vi tri muc tieu, ap dung ca cho chinh nguoi choi).\r\n"
"void KNpc::CastAutoSkillAt(int nSkillId, int nSkillLevel, int nTarget)\r\n"
"{\r\n"
"\tif (!m_Index || m_RegionIndex < 0)\r\n"
"\t\treturn;\r\n"
"\tif (nSkillId <= 0 || nSkillId >= MAX_SKILL || nSkillLevel <= 0 || nSkillLevel >= MAX_SKILLLEVEL)\r\n"
"\t\treturn;\r\n"
"\tif (nTarget <= 0 || nTarget >= MAX_NPC || Npc[nTarget].m_Index <= 0 || Npc[nTarget].m_SubWorldIndex != m_SubWorldIndex)\r\n"
"\t\treturn;\r\n"
"\tKSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(nSkillId, nSkillLevel);\r\n"
"\tif (!pSkill)\r\n"
"\t\treturn;\r\n"
"\tNPC_SKILL_SYNC SkillCmd;\r\n"
"\tSkillCmd.ProtocolType = s2c_castskilldirectly;\r\n"
"\tSkillCmd.ID = m_dwID;\r\n"
"\tSkillCmd.nSkillID = nSkillId;\r\n"
"\tSkillCmd.nSkillLevel = nSkillLevel;\r\n"
"\tSkillCmd.nMpsX = -1;\r\n"
"\tSkillCmd.nMpsY = Npc[nTarget].m_dwID;\r\n"
"\tstatic const POINT POff[8] = { {0, 32}, {-16, 32}, {-16, 0}, {-16, -32}, {0, -32}, {16, -32}, {16, 0}, {16, 32} };\r\n"
"\tint nMaxCount = MAX_BROADCAST_COUNT;\r\n"
"\tCURREGION.BroadCast(&SkillCmd, sizeof(SkillCmd), nMaxCount, m_MapX, m_MapY);\r\n"
"\tfor (int i = 0; i < 8; i++)\r\n"
"\t{\r\n"
"\t\tif (CONREGIONIDX(i) == -1)\r\n"
"\t\t\tcontinue;\r\n"
"\t\tCONREGION(i).BroadCast(&SkillCmd, sizeof(SkillCmd), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);\r\n"
"\t}\r\n"
"\tpSkill->Cast(m_Index, -1, nTarget);\r\n"
"}\r\n"
"\r\n"
"// [HOASON 02/09] autoreplyskill: Linux Fire(+0x1850, chu, KE DANH) tai ReceiveDamage 0x0808B4F9 (mot lan moi don, quan he DICH).\r\n"
"// loai 1 -> muc tieu = ke danh (1364 Doat Menh -> 1363 Thai Nhac Tam Thanh Phong bay ve ke danh), khac -> chinh minh.\r\n"
"// ty le: rate > Random(100); hoi chieu: NextCast = now + wait (khung).\r\n"
"void KNpc::ReplySkill(int nLauncher)\r\n"
"{\r\n"
"\tif (!m_Index)\r\n"
"\t\treturn;\r\n"
"\tif (m_Doing == do_death || m_Doing == do_revive)\r\n"
"\t\treturn;\r\n"
"\tfor (int i = 0; i < MAX_AUTOSKILL; i ++)\r\n"
"\t{\r\n"
"\t\tKMagicAutoSkill& rA = m_ReplySkill[i];\r\n"
"\t\tif (rA.nSkillId <= 0 || rA.nSkillId >= MAX_SKILL || rA.nSkillLevel <= 0 || rA.nSkillLevel >= MAX_SKILLLEVEL)\r\n"
"\t\t\tcontinue;\r\n"
"\t\tif (rA.dwNextCastTime >= SubWorld[m_SubWorldIndex].m_dwCurrentTime)\r\n"
"\t\t\tcontinue;\r\n"
"\t\tif (!g_RandPercent(rA.nRate))\r\n"
"\t\t\tcontinue;\r\n"
"\t\tif (rA.nType == 1 && nLauncher > 0 && nLauncher < MAX_NPC && Npc[nLauncher].m_Index > 0)\r\n"
"\t\t\tCastAutoSkillAt(rA.nSkillId, rA.nSkillLevel, nLauncher);\r\n"
"\t\telse\r\n"
"\t\t\tthis->Cast(rA.nSkillId, rA.nSkillLevel);\t// nham minh (Cast(id,cap) da phat s2c_castskilldirectly)\r\n"
"\t\trA.dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + rA.nWaitCastTime;\r\n"
"\t}\r\n"
"}\r\n"
"\r\n"
"// [HOASON 02/09] autorescueskill: Linux Fire(+0x1898, chu, ke danh) khi mau VUOT XUONG duoi 25% max (BeHurt 0x0808A003,\r\n"
"// ReceiveDamage 0x0808B0E3). loai 1 -> nham ke danh, khac -> chinh minh (1365 Tu Ha Kiem Khi -> 1366 loai 0).\r\n"
"void KNpc::RescueSkill(int nAttacker)\r\n"
"{\r\n"
"\tif (!m_Index)\r\n"
"\t\treturn;\r\n"
"\tif (m_Doing == do_death || m_Doing == do_revive)\r\n"
"\t\treturn;\r\n"
"\tfor (int i = 0; i < MAX_AUTOSKILL; i ++)\r\n"
"\t{\r\n"
"\t\tKMagicAutoSkill& rA = m_RescueSkill[i];\r\n"
"\t\tif (rA.nSkillId <= 0 || rA.nSkillId >= MAX_SKILL || rA.nSkillLevel <= 0 || rA.nSkillLevel >= MAX_SKILLLEVEL)\r\n"
"\t\t\tcontinue;\r\n"
"\t\tif (rA.dwNextCastTime >= SubWorld[m_SubWorldIndex].m_dwCurrentTime)\r\n"
"\t\t\tcontinue;\r\n"
"\t\tif (!g_RandPercent(rA.nRate))\r\n"
"\t\t\tcontinue;\r\n"
"\t\tif (rA.nType == 1 && nAttacker > 0 && nAttacker < MAX_NPC && Npc[nAttacker].m_Index > 0)\r\n"
"\t\t\tCastAutoSkillAt(rA.nSkillId, rA.nSkillLevel, nAttacker);\r\n"
"\t\telse\r\n"
"\t\t\tthis->Cast(rA.nSkillId, rA.nSkillLevel);\r\n"
"\t\trA.dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + rA.nWaitCastTime;\r\n"
"\t}\r\n"
"}\r\n"
"\r\n"
"// [HOASON 02/09] autoattackskill: Linux Fire(+0x1874 cua KE DANH, nan nhan, ke danh) tai ReceiveDamage 0x0808B1D3.\r\n"
"// loai 1 -> nham chinh minh (ke danh), khac -> nham nan nhan (1369 Cuu Kiem Hop Nhat -> 1368 Doc Co Cuu Kiem, loai 0).\r\n"
"void KNpc::AttackSkill(int nLauncher)\r\n"
"{\r\n"
"\tif (!m_Index || nLauncher <= 0 || nLauncher >= MAX_NPC || !Npc[nLauncher].m_Index)\r\n"
"\t\treturn;\r\n"
"\tif (m_Doing == do_death || m_Doing == do_revive)\r\n"
"\t\treturn;\r\n"
"\tif (Npc[nLauncher].m_Doing == do_death || Npc[nLauncher].m_Doing == do_revive)\r\n"
"\t\treturn;\r\n"
"\tfor (int i = 0; i < MAX_AUTOSKILL; i ++)\r\n"
"\t{\r\n"
"\t\tKMagicAutoSkill& rA = m_AttackSkill[i];\r\n"
"\t\tif (rA.nSkillId <= 0 || rA.nSkillId >= MAX_SKILL || rA.nSkillLevel <= 0 || rA.nSkillLevel >= MAX_SKILLLEVEL)\r\n"
"\t\t\tcontinue;\r\n"
"\t\tif (rA.dwNextCastTime >= SubWorld[m_SubWorldIndex].m_dwCurrentTime)\r\n"
"\t\t\tcontinue;\r\n"
"\t\tif (!g_RandPercent(rA.nRate))\r\n"
"\t\t\tcontinue;\r\n"
"\t\tif (rA.nType == 1)\r\n"
"\t\t\tthis->Cast(rA.nSkillId, rA.nSkillLevel);\r\n"
"\t\telse\r\n"
"\t\t\tCastAutoSkillAt(rA.nSkillId, rA.nSkillLevel, nLauncher);\r\n"
"\t\trA.dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + rA.nWaitCastTime;\r\n"
"\t}\r\n"
"}\r\n"
"\r\n")
    c.s = c.s[:i0] + NEW_BLOCK + c.s[i1:]
    c.n += 1
    print("  [+] KNpc.cpp: CastAutoSkillAt + ReplySkill/RescueSkill/AttackSkill")

# 4.2 DeathSkill: loi go m_ReplySkill -> m_DeathSkill
c.rep("if (m_DeathSkill[i].nSkillId > 0 && m_ReplySkill[i].nSkillId < MAX_SKILL && ",
      "if (m_DeathSkill[i].nSkillId > 0 && m_DeathSkill[i].nSkillId < MAX_SKILL && \t// [HOASON 02/09] loi go cu: kiem m_ReplySkill\r\n\t\t\t",
      tag="DeathSkill typo")

# 4.3 CalcDamage: go khoi tu phong cu
OLD_CALC = (
"\t\tif(!bReturn)\r\n"
"\t\t{\r\n"
"\t\t\t// [HOASON 01/09e] Linux (0x0808B4F9 goi Fire ngay, khong so cap) khong co cong cap 120: Hoa Son 1364 la chieu 90.\r\n"
"\t\t\tif(m_Level > 0)\r\n"
"\t\t\t{\r\n"
"\t\t\t\tReplySkill(nAttacker);\r\n"
"\t\t\t\tif (m_CurrentLife < (m_CurrentLifeMax * LIFE_EXPLOSIVE / MAX_PERCENT))\r\n"
"\t\t\t\t\tRescueSkill();\r\n"
"\t\t\t}\r\n"
"\t\t\t\r\n"
"\t\t\tif(Npc[nAttacker].m_Level > 0)\t// [HOASON 01/09e] bo cong cap 120 (1369 Cuu Kiem Hop Nhat la chieu 150, khong anh huong; giu dung Linux)\r\n"
"\t\t\t{\r\n"
"\t\t\t\tNpc[nAttacker].AttackSkill(m_Index);\r\n"
"\t\t\t}\r\n"
"\t\t}\r\n")
NEW_CALC = (
"\t\t// [HOASON 02/09] Tu phong (autoreply/autoattack) DA CHUYEN sang ReceiveDamage: mot lan moi DON TRUNG, chi quan he DICH,\r\n"
"\t\t// sau khi tru mau (Linux 0x0808AACF-0x0808B1D8). Truoc day nam o day => moi he lanh/hoa/loi/doc va moi NHIP DOC deu tung\r\n"
"\t\t// them mot lan, ca don khong sat thuong. Cuu nguy (autorescue) xet tai khe tru mau ben duoi (Linux BeHurt 0x0808A003).\r\n")
c.rep(OLD_CALC, NEW_CALC, tag="CalcDamage bo khoi tu phong cu")

# 4.4 CalcDamage: cuu nguy khi mau vuot xuong 25%
c.rep("\tm_CurrentLife -= nDamage;\r\n\tnRealDamage += nDamage;\r\n\tif (m_CurrentLife <= 0)\r\n",
      "\tm_CurrentLife -= nDamage;\r\n\tnRealDamage += nDamage;\r\n"
      "\t// [HOASON 02/09] autorescueskill CHUAN LINUX (BeHurt 0x0808A003-0x0808A01A): chi ban khi DON NAY dua mau tu >= 25% max\r\n"
      "\t// xuong < 25% max va con song (Lua 1365: 'Sinh menh thap hon 25% trong chop mat'). Ban cu: moi don khi mau dang < 25%.\r\n"
      "\tif (nDamage > 0 && m_CurrentLife > 0 && m_Level > 0)\r\n"
      "\t{\r\n"
      "\t\tint nHSNguong = m_CurrentLifeMax / 4;\r\n"
      "\t\tif (m_CurrentLife < nHSNguong && m_CurrentLife + nDamage >= nHSNguong)\r\n"
      "\t\t\tRescueSkill(nAttacker);\r\n"
      "\t}\r\n"
      "\tif (m_CurrentLife <= 0)\r\n",
      tag="CalcDamage cuu nguy 25%")

# 4.5 ReceiveDamage: co trung don + ghi ket qua 5 he
RD = "BOOL KNpc::ReceiveDamage(int nLauncher, int nMissleSeries, BOOL bIsPhysical, BOOL bIsMelee, void *pData, BOOL bUseAR, int nDoHurtP, int nMissRate)"
c.rep_first_after(RD, "\tKMagicAttrib *pTemp = NULL;\r\n\r\n\tpTemp = (KMagicAttrib *)pData;\r\n\tint nAr = pTemp->nValue[0]; //attackrating[0]",
      "\tKMagicAttrib *pTemp = NULL;\r\n"
      "\tBOOL bHSTrung = FALSE;\t// [HOASON 02/09] co it nhat mot he gay sat thuong (Linux 0x0808AAC7: ket qua BeHurt >= 0) -> moi tung tu phong\r\n"
      "\r\n\tpTemp = (KMagicAttrib *)pData;\r\n\tint nAr = pTemp->nValue[0]; //attackrating[0]",
      tag="ReceiveDamage bHSTrung")
c.rep_first_after(RD, "\tCalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_physics, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, nStolenLifeP, nStolenManaP, nStolenStaminaP, bIsDS, FALSE, nTotalAvg);",
      "\tbHSTrung |= CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_physics, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, nStolenLifeP, nStolenManaP, nStolenStaminaP, bIsDS, FALSE, nTotalAvg);\t// [HOASON 02/09]",
      tag="RD physics")
c.rep_first_after(RD, "\tif (CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_cold, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, FALSE, nTotalAvg))\r\n",
      "\tBOOL bHSCold = CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_cold, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, FALSE, nTotalAvg);\t// [HOASON 02/09]\r\n"
      "\tbHSTrung |= bHSCold;\r\n"
      "\tif (bHSCold)\r\n",
      tag="RD cold")
c.rep_first_after(RD, "\tCalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_fire, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, FALSE, nTotalAvg);",
      "\tbHSTrung |= CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_fire, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, FALSE, nTotalAvg);\t// [HOASON 02/09]",
      tag="RD fire")
c.rep_first_after(RD, "\tCalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_light, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, FALSE, nTotalAvg);",
      "\tbHSTrung |= CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_light, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, FALSE, nTotalAvg);\t// [HOASON 02/09]",
      tag="RD light")
c.rep_first_after(RD, "\tif (CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_poison, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE))\r\n",
      "\tBOOL bHSPoison = CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_poison, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE);\t// [HOASON 02/09]\r\n"
      "\tbHSTrung |= bHSPoison;\r\n"
      "\tif (bHSPoison)\r\n",
      tag="RD poison")
# 4.6 khe chi tu: cuu nguy khi mau vuot xuong 25%
c.rep_first_after(RD, "\t\t\tm_CurrentLife -= nFSDamage;\t// 0x0808B0F8 ghi thang mau\r\n",
      "\t\t\tm_CurrentLife -= nFSDamage;\t// 0x0808B0F8 ghi thang mau\r\n"
      "\t\t\t// [HOASON 02/09] cuu nguy khi khe chi tu dua mau xuong duoi 25% max (Linux ReceiveDamage 0x0808B0E3-0x0808B13A)\r\n"
      "\t\t\tif (m_CurrentLife > 0 && m_Level > 0 && m_CurrentLife < m_CurrentLifeMax / 4 && m_CurrentLife + nFSDamage >= m_CurrentLifeMax / 4)\r\n"
      "\t\t\t\tRescueSkill(nLauncher);\r\n",
      tag="RD chi tu cuu nguy")
# 4.7 tu phong sau tru mau, truoc khe choang
c.rep_first_after(RD, "\tpTemp++; //stun[14]\r\n",
      "\t// [HOASON 02/09] TU PHONG CHUAN LINUX ReceiveDamage 0x0808AACF-0x0808B1D8: sau khi tru mau, CHI khi quan he la DICH\r\n"
      "\t// ([ebp+0x24] & 0xC == 8) va don co sat thuong: (1) NAN NHAN ban autoreplyskill (+0x1850) nham ke danh/minh,\r\n"
      "\t// (2) KE DANH ban autoattackskill (+0x1874) nham nan nhan/minh. Moi don MOT lan. Hai bang cong 'khong ban lai'\r\n"
      "\t// (0x8fc62a0/0x8fc4360) cua Linux khong bao gio duoc dien (vector 0x8fbfe04/0x8fbfe10 khong co noi ghi) -> luon mo.\r\n"
      "\tif (bHSTrung && nLauncher > 0 && nLauncher < MAX_NPC && Npc[nLauncher].m_Index > 0\r\n"
      "\t\t&& (NpcSet.GetRelation(nLauncher, m_Index) & (relation_ally | relation_enemy)) == relation_enemy)\r\n"
      "\t{\r\n"
      "\t\tif (m_Level > 0)\r\n"
      "\t\t\tReplySkill(nLauncher);\r\n"
      "\t\tif (Npc[nLauncher].m_Level > 0)\r\n"
      "\t\t\tNpc[nLauncher].AttackSkill(m_Index);\r\n"
      "\t}\r\n"
      "\r\n"
      "\tpTemp++; //stun[14]\r\n",
      tag="RD tu phong")

# 4.8 hoi sinh luc / noi luc chuan Linux
c.rep("\t\t\tm_CurrentLife += m_CurrentLifeReplenish + (m_CurrentLifeReplenish * m_CurrentLifeReplenishPercent / MAX_PERCENT);\r\n",
      "\t\t\t// [HOASON 02/09] CHUAN LINUX 0x0808B65F-0x0808B6BD (log 'AddLife: %d * %d%% = %d'): % hoi (lifereplenish_p) CHI nhan\r\n"
      "\t\t\t// khi tong hoi > 0; hoi AM (rut mau) cong thang, khong nhan %.\r\n"
      "\t\t\tif (m_CurrentLifeReplenish > 0)\r\n"
      "\t\t\t\tm_CurrentLife += m_CurrentLifeReplenish * (MAX_PERCENT + m_CurrentLifeReplenishPercent) / MAX_PERCENT;\r\n"
      "\t\t\telse\r\n"
      "\t\t\t\tm_CurrentLife += m_CurrentLifeReplenish;\r\n",
      tag="tick hoi sinh luc")
c.rep("\t\t\t// [PF 31/08k] manareplenish_p (254): hoi noi luc theo %, doi xung voi ve % cua sinh luc o tren\r\n"
      "\t\t\tm_CurrentMana += m_CurrentManaReplenish + (m_CurrentManaReplenish * m_CurrentManaReplenishPercent / MAX_PERCENT);\r\n",
      "\t\t\t// [HOASON 02/09] CHUAN LINUX 0x0808B6FA-0x0808B721: noi luc += (hoi goc + tong manareplenish_v) KHONG nhan %.\r\n"
      "\t\t\t// manareplenish_p ([0x119c]) o Linux CHI nhan vao thuoc hoi noi luc theo thoi gian (0x0808B826 'AddManaState') - xem m_ManaState.\r\n"
      "\t\t\t// Ban cu R + R*p/100: Kiem Tong Hoa Son (1349/1364/1369: v = -10000, p = -200) ra (goc-10000)*(-1) = +9950/nhip -> DAY noi luc,\r\n"
      "\t\t\t// nguoc voi Linux (rut ve 0 - Kiem Tong khong dung noi luc). Cac phai khac p = 0 -> khong doi.\r\n"
      "\t\t\tm_CurrentMana += m_CurrentManaReplenish;\r\n",
      tag="tick hoi noi luc")
c.rep("\t\t\tm_CurrentMana += m_ManaState.nValue[0];\r\n",
      "\t\t\t// [HOASON 02/09] Linux 0x0808B826 ('AddManaState: %d * %d%% = %d'): thuoc hoi noi luc theo thoi gian nhan manareplenish_p\r\n"
      "\t\t\tm_CurrentMana += m_ManaState.nValue[0] * (MAX_PERCENT + m_CurrentManaReplenishPercent) / MAX_PERCENT;\r\n",
      tag="tick thuoc noi luc")
c.save()
print("XONG%s." % (" (KIEM)" if KIEM else ""))
