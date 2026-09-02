# -*- coding: utf-8 -*-
# [HOASON 01/09e] chinh lai: chi nham ke danh khi loai == 1; loai khac giu nguyen duong cu this->Cast (tu buff) - tranh doi hanh vi 9 phai kia
import io
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KNpc.cpp"
d = io.open(p, "r", encoding="latin-1", newline="").read()
old = ("\t\t\t\t\tint nTarget = (m_ReplySkill[i].nType == 1 && nLauncher > 0 && nLauncher < MAX_NPC && Npc[nLauncher].m_Index > 0) ? nLauncher : m_Index;\r\n"
       "\t\t\t\t\tKSkill* pReply = (KSkill*)g_SkillManager.GetSkill(m_ReplySkill[i].nSkillId, m_ReplySkill[i].nSkillLevel);\r\n"
       "\t\t\t\t\tif (pReply)\r\n\t\t\t\t\t\tpReply->Cast(m_Index, -1, nTarget);\r\n"
       "\t\t\t\t\telse\r\n\t\t\t\t\t\tthis->Cast(m_ReplySkill[i].nSkillId, m_ReplySkill[i].nSkillLevel);\r\n")
new = ("\t\t\t\t\tKSkill* pReply = (m_ReplySkill[i].nType == 1) ? (KSkill*)g_SkillManager.GetSkill(m_ReplySkill[i].nSkillId, m_ReplySkill[i].nSkillLevel) : NULL;\r\n"
       "\t\t\t\t\tif (pReply && nLauncher > 0 && nLauncher < MAX_NPC && Npc[nLauncher].m_Index > 0)\r\n"
       "\t\t\t\t\t\tpReply->Cast(m_Index, -1, nLauncher);\t// loai 1: bay ve phia ke danh (Linux)\r\n"
       "\t\t\t\t\telse\r\n\t\t\t\t\t\tthis->Cast(m_ReplySkill[i].nSkillId, m_ReplySkill[i].nSkillLevel);\t// loai khac: nhu cu (tu buff len minh)\r\n")
assert d.count(old) == 1, d.count(old)
d = d.replace(old, new)
io.open(p, "w", encoding="latin-1", newline="").write(d)
print("KNpc.cpp ReplySkill: chinh lai loai != 1 giu duong cu")
