# -*- coding: utf-8 -*-
"""hs_engine_patch3.py [HOASON 01/09e] - autoreplyskill/autoattackskill/autorescueskill chuan Linux:
  Linux handler 0x080973D0 (autoreplyskill): id = (|v0| & 0xffffff) >> 8, cap = v0 & 0xff, LOAI = |v0| >> 24, cho = v2 >> 8, ti le = v2 & 0xff.
  Hoa Son 1364 Doat Menh: v0 = (1*65536 + 1363)*256 + cap  -> loai 1. JX1 cu: id = v0/256 = 66899 (>= MAX_SKILL) -> KHONG BAO GIO phong -> mat hieu ung "3 kiem bay".
  Linux ban 0x08188BB0 (KAutoSkillList::Fire(list, chu, ke_kia)): loai == 1 -> nham KE KIA (ke danh), khac -> nham chinh minh; khong co cong cap 120.
"""
import io, os, re, sys
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
KIEM = "--kiem" in sys.argv
SRC = r"D:\GAMEDEVNEW\Sources"
MARK = "[HOASON 01/09e]"
LF = chr(10); CR = chr(13); CRLF = CR + LF
class Tep:
    def __init__(self, rel):
        self.p = os.path.join(SRC, rel); self.d = io.open(self.p, "r", encoding="latin-1", newline="").read()
        self.nl = CRLF if CRLF in self.d else LF; self.n = 0; self.rel = rel
    def nx(self, s): return s.replace(CRLF, LF).replace(LF, CRLF) if self.nl == CRLF else s
    def da(self, m): return m in self.d
    def thay(self, old, new, so=1):
        old = self.nx(old); new = self.nx(new); c = self.d.count(old)
        assert c == so, "%s: neo %d lan (can %d): %r" % (self.rel, c, so, old[:90])
        self.d = self.d.replace(old, new); self.n += 1
    def ghi(self):
        if KIEM: print("  (kiem) %s: %d cho" % (self.rel, self.n)); return
        io.open(self.p, "w", encoding="latin-1", newline="").write(self.d); print("  ghi %s: %d cho" % (self.rel, self.n))

t = Tep(r"Core\Src\GameDataDef.h")
if not t.da(MARK):
    t.thay("\tint\t\t\t\tnWaitCastTime;\n\tKMagicAutoSkill(){nSkillId = nSkillLevel = nRate = dwNextCastTime = nWaitCastTime = 0;};",
           "\tint\t\t\t\tnWaitCastTime;\n\tint\t\t\t\tnType;\t\t\t\t\t\t// %s byte cao nValue[0] (Linux): 1 = nham ke danh/muc tieu, khac = nham minh\n"
           "\tKMagicAutoSkill(){nSkillId = nSkillLevel = nRate = dwNextCastTime = nWaitCastTime = nType = 0;};" % MARK)
    t.ghi()

t = Tep(r"Core\Src\KNpcAttribModify.cpp")
if not t.da(MARK):
    rx = re.compile(r"(pNpc->m_(Attack|Reply|Rescue)Skill\[i\])\.nSkillId = pMagic->nValue\[0\] / 256;(\r?\n)(\t+)\1\.nSkillLevel = pMagic->nValue\[0\] % 256;")
    n = len(rx.findall(t.d)); assert n == 3, "handler auto*: %d" % n
    t.d = rx.sub(lambda m: "%s.nSkillId = (pMagic->nValue[0] & 0xffffff) / 256;\t// %s Linux 0x080973D0: mat 24 bit thap (byte cao = loai)%s%s%s.nSkillLevel = pMagic->nValue[0] %% 256;%s%s%s.nType = pMagic->nValue[0] >> 24;"
                 % (m.group(1), MARK, m.group(3), m.group(4), m.group(1), m.group(3), m.group(4), m.group(1)), t.d)
    t.n += 3; t.ghi()

t = Tep(r"Core\Src\KNpc.h")
if not t.da(MARK):
    t.thay("\tvoid\t\t\t\tReplySkill();\n", "\tvoid\t\t\t\tReplySkill(int nLauncher = 0);\t// %s Linux Fire(list, chu, KE DANH): loai 1 nham ke danh\n" % MARK)
    t.ghi()

t = Tep(r"Core\Src\KNpc.cpp")
if not t.da(MARK):
    t.thay("void KNpc::ReplySkill()\n{\n", "void KNpc::ReplySkill(int nLauncher)\t// %s\n{\n" % MARK)
    old = ("\t\t\t\tif (g_RandPercent(m_ReplySkill[i].nRate))\n\t\t\t\t{\n"
           "\t\t\t\t\tthis->Cast(m_ReplySkill[i].nSkillId, m_ReplySkill[i].nSkillLevel);\n"
           "\t\t\t\t\tm_ReplySkill[i].dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + m_ReplySkill[i].nWaitCastTime;\n")
    new = ("\t\t\t\tif (g_RandPercent(m_ReplySkill[i].nRate))\n\t\t\t\t{\n"
           "\t\t\t\t\t// %s Linux 0x08188D0B: loai 1 -> muc tieu = ke danh (Doat Menh 1364 -> 1363 Thai Nhac Tam Thanh Phong bay ve phia ke danh),\n"
           "\t\t\t\t\t// loai khac -> chinh minh. Chieu dan (1363) can muc tieu nen Cast(m_Index, -1, nTarget) nhu AttackSkill.\n"
           "\t\t\t\t\tint nTarget = (m_ReplySkill[i].nType == 1 && nLauncher > 0 && nLauncher < MAX_NPC && Npc[nLauncher].m_Index > 0) ? nLauncher : m_Index;\n"
           "\t\t\t\t\tKSkill* pReply = (KSkill*)g_SkillManager.GetSkill(m_ReplySkill[i].nSkillId, m_ReplySkill[i].nSkillLevel);\n"
           "\t\t\t\t\tif (pReply)\n\t\t\t\t\t\tpReply->Cast(m_Index, -1, nTarget);\n"
           "\t\t\t\t\telse\n\t\t\t\t\t\tthis->Cast(m_ReplySkill[i].nSkillId, m_ReplySkill[i].nSkillLevel);\n"
           "\t\t\t\t\tm_ReplySkill[i].dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + m_ReplySkill[i].nWaitCastTime;\n") % MARK
    t.thay(old, new)
    old = ("\t\t\tif(m_Level >= LEVEL_EXPLOSIVE)\n\t\t\t{\n\t\t\t\tReplySkill();\n")
    new = ("\t\t\t// %s Linux (0x0808B4F9 goi Fire ngay, khong so cap) khong co cong cap 120: Hoa Son 1364 la chieu 90.\n"
           "\t\t\tif(m_Level > 0)\n\t\t\t{\n\t\t\t\tReplySkill(nAttacker);\n") % MARK
    t.thay(old, new)
    old = ("\t\t\tif(Npc[nAttacker].m_Level >= LEVEL_EXPLOSIVE)\n\t\t\t{\n\t\t\t\tNpc[nAttacker].AttackSkill(m_Index);\n")
    new = ("\t\t\tif(Npc[nAttacker].m_Level > 0)\t// %s bo cong cap 120 (1369 Cuu Kiem Hop Nhat la chieu 150, khong anh huong; giu dung Linux)\n\t\t\t{\n\t\t\t\tNpc[nAttacker].AttackSkill(m_Index);\n") % MARK
    t.thay(old, new)
    t.ghi()
print("XONG" + (" (chi kiem)" if KIEM else ""))
