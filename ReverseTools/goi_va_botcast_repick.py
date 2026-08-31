# -*- coding: utf-8 -*-
# [CAST-LECH 30/08] Do that: 82.971 cu "BI TU CHOI"/ngay den tu VEN VEN 2 bot he kiem
# (Nga My NguyenAnh343, Vo Dang VuTai275) - boc trung "duong QUYEN" (PB_WPN_NONE,
# tay khong) trong pool {kiem, NONE} nhung b.nAtkSkill VAN GIU chieu kiem cu
# (328/368 eqt=0): chieu chi duoc chon lai khi DOI CAP, con nhanh phat-lai vu khi
# chi reset khi NHAN DUOC vu khi (boc trung NONE -> khong reset) -> cast lech vu khi
# bi CanCastSkill tu choi VINH VIEN = "bot khong danh duoc npc".
# Va: ngay tai cho tham do CanCastSkill == 0, neu ly do la CHIEU LECH VU KHI dang cam
# (eqt != -2 va != nhom vu khi hien tai, cung quy uoc voi pb_PickSkill) -> ep
# b.nAtkSkill = 0 de nhip sau chon lai theo vu khi hien tai; tu choi vi ly do khac
# (mana, the cuoi ngua...) giu nguyen duong cu. Kem gac khong SendCommand chieu 0.
# Chi KPlayerBot.cpp (server-only).
import io, sys
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerBot.cpp"

s = io.open(P, "r", encoding="latin-1", newline="").read()
truoc = sum(1 for c in s if ord(c) > 127)
crlf = "\r\n" in s
n = 0

def ap(ten, cu, moi):
    global s, n
    if crlf:
        cu = cu.replace("\n", "\r\n"); moi = moi.replace("\n", "\r\n")
    if moi in s:
        print("  [=] %s da ap tu truoc" % ten); return
    if s.count(cu) != 1:
        print("LOI: neo %s khop %d cho (can 1)" % (ten, s.count(cu))); sys.exit(1)
    s = s.replace(cu, moi); n += 1
    print("  [+] %s" % ten)

ap("H1 chon lai chieu khi lech vu khi",
 '\t\t\t\tint nP1 = -1, nP2 = t;\n'
 '\t\t\t\tif (pDo->CanCastSkill(nNpcIdx, nP1, nP2) == 0\n'
 '\t\t\t\t && now - b.nUongTick >= (unsigned int)(GAME_FPS * 5))\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tpb_Log("[BotCast] %s chieu %d cap %d BI TU CHOI (eqt=%d,"\n'
 '\t\t\t\t\t       " vukhi detail=%d parti=%d, mana=%d/%d)\\n",\n'
 '\t\t\t\t\t       Player[nIdx].m_PlayerName, b.nAtkSkill, b.nAtkSkillLv,\n'
 '\t\t\t\t\t       pDo->GetEquipLimit(),\n'
 '\t\t\t\t\t       Player[nIdx].m_ItemList.GetWeaponType(),\n'
 '\t\t\t\t\t       Player[nIdx].m_ItemList.GetWeaponParticular(),\n'
 '\t\t\t\t\t       (int)Npc[nNpcIdx].m_CurrentMana, (int)Npc[nNpcIdx].m_CurrentManaMax);\n'
 '\t\t\t\t}\n',
 '\t\t\t\tint nP1 = -1, nP2 = t;\n'
 '\t\t\t\tif (pDo->CanCastSkill(nNpcIdx, nP1, nP2) == 0)\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\t// [CAST-LECH 30/08] 83k cu tu choi/ngay = bot he kiem boc trung\n'
 '\t\t\t\t\t// "duong quyen" (tay khong) nhung van giu chieu kiem cu: chieu chi\n'
 '\t\t\t\t\t// duoc chon lai khi DOI CAP, nhanh phat-lai vu khi boc trung NONE\n'
 '\t\t\t\t\t// cung khong reset -> ket vinh vien. Chieu LECH vu khi dang cam\n'
 '\t\t\t\t\t// (quy uoc eqt y het pb_PickSkill) -> ep chon lai ngay; tu choi vi\n'
 '\t\t\t\t\t// ly do khac (mana, the cuoi...) giu nguyen.\n'
 '\t\t\t\t\tconst int eqC = pDo->GetEquipLimit();\n'
 '\t\t\t\t\tint nWantC = -1;\n'
 '\t\t\t\t\t{\n'
 '\t\t\t\t\t\tconst int nWpnC = Player[nIdx].m_ItemList.GetEquipment(itempart_weapon);\n'
 '\t\t\t\t\t\tif (nWpnC > 0)\n'
 '\t\t\t\t\t\t{\n'
 '\t\t\t\t\t\t\tconst int nDetC = Item[nWpnC].GetDetailType();\n'
 '\t\t\t\t\t\t\tconst int nParC = Item[nWpnC].GetParticular();\n'
 '\t\t\t\t\t\t\tnWantC = (nParC == HAND_PARTICULAR) ? -1\n'
 '\t\t\t\t\t\t\t       : ((nDetC == 1) ? (nParC + 100) : nParC);\n'
 '\t\t\t\t\t\t}\n'
 '\t\t\t\t\t}\n'
 '\t\t\t\t\tif (eqC != -2 && eqC != nWantC)\n'
 '\t\t\t\t\t{\n'
 '\t\t\t\t\t\tpb_Log("[BotCast] %s chieu %d LECH vu khi (eqt=%d want=%d)"\n'
 '\t\t\t\t\t\t       " -> chon lai chieu theo vu khi hien tai\\n",\n'
 '\t\t\t\t\t\t       Player[nIdx].m_PlayerName, b.nAtkSkill, eqC, nWantC);\n'
 '\t\t\t\t\t\tb.nAtkSkill = 0;\n'
 '\t\t\t\t\t}\n'
 '\t\t\t\t\telse if (now - b.nUongTick >= (unsigned int)(GAME_FPS * 5))\n'
 '\t\t\t\t\t{\n'
 '\t\t\t\t\t\tpb_Log("[BotCast] %s chieu %d cap %d BI TU CHOI (eqt=%d,"\n'
 '\t\t\t\t\t\t       " vukhi detail=%d parti=%d, mana=%d/%d)\\n",\n'
 '\t\t\t\t\t\t       Player[nIdx].m_PlayerName, b.nAtkSkill, b.nAtkSkillLv,\n'
 '\t\t\t\t\t\t       pDo->GetEquipLimit(),\n'
 '\t\t\t\t\t\t       Player[nIdx].m_ItemList.GetWeaponType(),\n'
 '\t\t\t\t\t\t       Player[nIdx].m_ItemList.GetWeaponParticular(),\n'
 '\t\t\t\t\t\t       (int)Npc[nNpcIdx].m_CurrentMana, (int)Npc[nNpcIdx].m_CurrentManaMax);\n'
 '\t\t\t\t\t}\n'
 '\t\t\t\t}\n')

ap("H2 khong phat chieu 0",
 '\t\tNpc[nNpcIdx].SendCommand(do_skill, b.nAtkSkill, -1, t);\n',
 '\t\tif (b.nAtkSkill > 0)   // [CAST-LECH] vua bi ep chon lai -> nhip sau phat chieu moi\n'
 '\t\t\tNpc[nNpcIdx].SendCommand(do_skill, b.nAtkSkill, -1, t);\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
