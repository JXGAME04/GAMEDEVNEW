# -*- coding: utf-8 -*-
# goi_va_bot_tranphai60_0709.py - chủ game 07/09: "cho bot nâng max skill trấn phái cấp 60".
#
# Bộ trấn phái 60 = chiêu mốc 60 có MaxLevel > 20 trong skills.txt (mỗi phái đúng một chiêu, trùng
# chú thích "tran phai" trong SKILLNORMAL của factionhead.lua): 273 Như Lai Thiên Diệp, 36 Thiên Vương
# Chiến Ý, 48 Tâm Nhãn, 75 Ngũ Độc Kỳ Kinh, 252 Phật Pháp Vô Biên, 114 Băng Cốt Tuyết Tâm, 130 Tẩy Điệp
# Cuồng Vũ, 150 Thiên Ma Giải Thể, 166 Thái Cực Thần Công, 275 Sương Ngạo Côn Lôn.
# Vòng "nâng full theo cấp" trong pb_Fight (KPlayerBot.cpp) trước đây bỏ qua mọi chiêu bị động (style 3)
# và kẹp 20 -> 7/10 chiêu trấn phái (bị động) nằm ở cấp 0, 3 chiêu còn lại kẹt 20. Nay: chiêu trấn phái
# được nâng tới MaxLevel (30) kể cả bị động; các chiêu khác giữ nguyên luật cũ.
# Chỉ chèn ASCII, đọc/ghi latin-1, idempotent (dấu mốc "[TRANPHAI60 07/09]").
import io, os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
P = os.path.join(ROOT, 'Sources', 'Core', 'Src', 'KPlayerBot.cpp')
NL = '\r\n'


def L(*lines):
    return NL.join(lines) + NL


s = io.open(P, 'r', encoding='latin-1', newline='').read()
cao = sum(1 for c in s if ord(c) >= 128)
if '[TRANPHAI60 07/09]' in s:
    print('KPlayerBot.cpp: da va, bo qua')
    raise SystemExit(0)

old = L('\t\t\t\tif (p2->GetSkillStyle() == SKILL_SS_PassivityNpcState)',
        '\t\t\t\t\tcontinue;',
        '\t\t\t\t// khong nang chieu khac ngu hanh (chieu phai khac thua ke tu mau)',
        '\t\t\t\t{',
        '\t\t\t\t\tconst int nSr2 = p2->GetSkillSeries();',
        '\t\t\t\t\tif (nSr2 >= 0 && nSr2 < series_num && nSr2 != Npc[nNpcIdx].m_Series)',
        '\t\t\t\t\t\tcontinue;',
        '\t\t\t\t}',
        '\t\t\t\tconst int rq2 = p2->GetSkillReqLevel();',
        '\t\t\t\tif (nLvBot < rq2)',
        '\t\t\t\t\tcontinue;                    // chua du cap mo ky nang nay',
        '\t\t\t\tint nMuon = (rq2 >= 80) ? 1 : nLvBot;',
        '\t\t\t\tif (nMuon > 20) nMuon = 20;',
        '\t\t\t\tif (sl2.m_Skills[q].SkillLevel < nMuon)',
        '\t\t\t\t{',
        '\t\t\t\t\tsl2.Add(id2, nMuon);',
        '\t\t\t\t\tnNang++;',
        '\t\t\t\t}')
new = L('\t\t\t\t// [TRANPHAI60 07/09] chu game: "cho bot nang max skill tran phai cap 60". Bo tran phai =',
        '\t\t\t\t// chieu moc 60 co MaxLevel > 20 (skills.txt; moi phai dung 1 chieu, trung chu thich "tran phai"',
        '\t\t\t\t// trong SKILLNORMAL factionhead.lua: 273/36/48/75/252/114/130/150/166/275). Chieu nay duoc nang',
        '\t\t\t\t// toi MaxLevel (30) va KHONG bi bo qua du la bi dong (style 3): KSkillList::Add/IncreaseLevel',
        '\t\t\t\t// tu Cast bi dong nen hieu ung ap ngay. Do 07/09: 7/10 chieu tran phai (bi dong) nam o cap 0,',
        '\t\t\t\t// 3 chieu con lai ket 20 tren toan bo 1.000 bot.',
        '\t\t\t\tconst int rq2 = p2->GetSkillReqLevel();',
        '\t\t\t\tconst int nMaxSk = (int)g_SkillManager.GetSkillMaxLevel(id2);',
        '\t\t\t\tconst int bTranPhai = (rq2 == 60 && nMaxSk > 20);',
        '\t\t\t\tif (p2->GetSkillStyle() == SKILL_SS_PassivityNpcState && !bTranPhai)',
        '\t\t\t\t\tcontinue;',
        '\t\t\t\t// khong nang chieu khac ngu hanh (chieu phai khac thua ke tu mau)',
        '\t\t\t\t{',
        '\t\t\t\t\tconst int nSr2 = p2->GetSkillSeries();',
        '\t\t\t\t\tif (nSr2 >= 0 && nSr2 < series_num && nSr2 != Npc[nNpcIdx].m_Series)',
        '\t\t\t\t\t\tcontinue;',
        '\t\t\t\t}',
        '\t\t\t\tif (nLvBot < rq2)',
        '\t\t\t\t\tcontinue;                    // chua du cap mo ky nang nay',
        '\t\t\t\tint nMuon = bTranPhai ? nMaxSk : ((rq2 >= 80) ? 1 : nLvBot);',
        '\t\t\t\tif (!bTranPhai && nMuon > 20) nMuon = 20;',
        '\t\t\t\tif (sl2.m_Skills[q].SkillLevel < nMuon)',
        '\t\t\t\t{',
        '\t\t\t\t\tconst int nCuLv = sl2.m_Skills[q].SkillLevel;',
        '\t\t\t\t\tsl2.Add(id2, nMuon);',
        '\t\t\t\t\tnNang++;',
        '\t\t\t\t\tif (bTranPhai)',
        '\t\t\t\t\t\tpb_Log("[BotTranPhai] %s cap %d: %s (id %d) %d -> %d (max)\\n",',
        '\t\t\t\t\t\t       Player[nIdx].m_PlayerName, nLvBot, p2->GetSkillName(), id2, nCuLv,',
        '\t\t\t\t\t\t       sl2.m_Skills[q].SkillLevel);',
        '\t\t\t\t}')
if s.count(old) != 1:
    raise SystemExit('NEO thay %d lan - KHONG GHI' % s.count(old))
s2 = s.replace(old, new)
if sum(1 for c in s2 if ord(c) >= 128) != cao:
    raise SystemExit('SO BYTE CAO DOI - KHONG GHI')
io.open(P, 'w', encoding='latin-1', newline='').write(s2)
print('KPlayerBot.cpp: da va 1 hunk (byte cao %d giu nguyen)' % cao)
