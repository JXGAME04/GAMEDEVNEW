# -*- coding: utf-8 -*-
# [BOTNOI dot 2 06/09] Chu game tra loi 3 cau hoi cua BANGIAO_BOTNOI_NGOAI_0609.md muc 7:
#   - "thieu lam co duong quyen"            -> giu nguyen (TL khong chan cung).
#   - "ngu doc phai co noi cong"            -> H5/H6/H7: don DOC khong-vat-ly cua Ngu Doc
#                                              (63/68/71/353, eqt -2) tinh la chieu NOI; bo loai
#                                              DOCTHUAN cho Ngu Doc trong pb_PickSkill.
#   - "cho bot noi tay lai diem va tang diem lai" -> H8: pb_TayDiemBotNoi: bot noi dang mang
#                                              chi so NGOAI -> ResetBaseAttribute ve bang goc
#                                              cua game (chuyensinhdaisu.lua as[ngu hanh]),
#                                              quy = (cap-1)*5 + giu chuyen sinh (nhu
#                                              LuaSetBasePoint), roi pb_AllocAttribPoints.
# AP SAU goi_va_botnoi_bat_0609.py. Idempotent. Chi KPlayerBot.cpp (ASCII).
import io, sys
P = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_botnoi\Sources\Core\Src\KPlayerBot.cpp"

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

# ---- H5: pb_DonDocNoi + tham so bDocLaNoi cho bo quet chieu noi tay khong ----
ap("H5a pb_DonDocNoi + chu ky pb_CoChieuNoiTayKhong",
 'static int pb_CoChieuNoiTayKhong(int nNpcIdx)\n'
 '{\n'
 '\tKSkillList& sl = Npc[nNpcIdx].m_SkillList;\n',
 '// [NGUDOC-NOI 06/09] Chu game: "ngu doc phai co noi cong". Chieu tay khong cua Ngu Doc\n'
 '// (63 Doc Sa Chuong rq10, 68 U Minh Quy Luy rq30, 71 Thien Cuong Dia Sat rq60, 353 Am\n'
 '// Phong Thuc Cot rq80 - deu IsPhysical=0, eqt -2) CHI mang don DOC. Voi bDocLaNoi = 1\n'
 '// (chi Ngu Doc) thi don doc KHONG vat ly cung tinh la chieu noi. Do lai co che doc\n'
 '// (KNpc.cpp ~4940 dat trang thai, ~1487 tick): poisondamage_v = {sat thuong/tick, so\n'
 '// khung, khung/tick}; 353 cap 20 = 121 moi 10 khung trong 60..120 khung -> sut mau that.\n'
 '// Ket luan 303-DOC 30/08 "doc khong bao mon quai" chi dung voi 303 CAP 1 (8 moi 10 khung).\n'
 'static int pb_DonDocNoi(KSkill* p)\n'
 '{\n'
 '\treturn !p->IsPhysical() && pb_DonDoc(p);\n'
 '}\n'
 '\n'
 'static int pb_CoChieuNoiTayKhong(int nNpcIdx, int bDocLaNoi)\n'
 '{\n'
 '\tKSkillList& sl = Npc[nNpcIdx].m_SkillList;\n')

ap("H5b quet: doc khong-vat-ly = noi khi bDocLaNoi",
 '\t\tif (pb_DonPhepThat(p))\n'
 '\t\t\treturn 1;\n'
 '\t}\n'
 '\treturn 0;\n'
 '}\n',
 '\t\tif (pb_DonPhepThat(p))\n'
 '\t\t\treturn 1;\n'
 '\t\tif (bDocLaNoi && pb_DonDocNoi(p))   // [NGUDOC-NOI 06/09]\n'
 '\t\t\treturn 1;\n'
 '\t}\n'
 '\treturn 0;\n'
 '}\n')

# ---- H6: pb_PhaiDocLaNoi + pb_BotNoiThat truyen co ----
ap("H6a pb_PhaiDocLaNoi",
 'static int pb_PhaiLuonCamVuKhi(int nFaction)\n'
 '{\n'
 '\treturn nFaction == 1 || nFaction == 2;   // 1 Thien Vuong, 2 Duong Mon\n'
 '}\n',
 'static int pb_PhaiLuonCamVuKhi(int nFaction)\n'
 '{\n'
 '\treturn nFaction == 1 || nFaction == 2;   // 1 Thien Vuong, 2 Duong Mon\n'
 '}\n'
 '\n'
 '// [NGUDOC-NOI 06/09] Phai ma don DOC (khong vat ly) la duong NOI: chu game "ngu doc phai\n'
 '// co noi cong". Duong Mon KHONG (van chan cung tren + 303 cap thap vo dung).\n'
 'static int pb_PhaiDocLaNoi(int nFaction)\n'
 '{\n'
 '\treturn nFaction == 3;                    // 3 Ngu Doc\n'
 '}\n')

ap("H6b pb_BotNoiThat goi quet voi co doc",
 '\tconst int nNpcIdx = Player[nIdx].m_nIndex;\n'
 '\tif (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)\n'
 '\t\treturn 0;\n'
 '\treturn pb_CoChieuNoiTayKhong(nNpcIdx);\n',
 '\tconst int nNpcIdx = Player[nIdx].m_nIndex;\n'
 '\tif (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)\n'
 '\t\treturn 0;\n'
 '\treturn pb_CoChieuNoiTayKhong(nNpcIdx,\n'
 '\t\tpb_PhaiDocLaNoi((int)Player[nIdx].m_cFaction.m_nCurFaction));   // [NGUDOC-NOI 06/09]\n')

# ---- H7: pb_PickSkill: doc = noi cho Ngu Doc; khong loai DOCTHUAN cho Ngu Doc ----
ap("H7a pb_PickSkill co bDocLaNoi",
 '\tconst int bThienNoi = pb_BotNoi(nIdx);   // [BotNoi 28/08] mot nguon su that voi duong tay-khong\n',
 '\tconst int bThienNoi = pb_BotNoi(nIdx);   // [BotNoi 28/08] mot nguon su that voi duong tay-khong\n'
 '\t// [NGUDOC-NOI 06/09] Ngu Doc: don doc khong-vat-ly LA chieu noi (63/68/71/353) - khong loai\n'
 '\t// DOCTHUAN, va duoc uu tien nhu don phep cho nua dan thien noi.\n'
 '\tconst int bDocLaNoi = pb_PhaiDocLaNoi((int)Player[nIdx].m_cFaction.m_nCurFaction);\n')

ap("H7b bNoi tinh ca doc cho Ngu Doc",
 '\t\tconst int bNoi = pb_DonPhepThat(p);\n',
 '\t\tconst int bNoi = pb_DonPhepThat(p) || (bDocLaNoi && pb_DonDocNoi(p));   // [NGUDOC-NOI 06/09]\n')

ap("H7c khong loai DOCTHUAN cho Ngu Doc",
 '\t\tif (!p->IsPhysical() && pb_DonDoc(p)\n'
 '\t\t && !pb_DonPhepThat(p) && !pb_DonVatLy(p))\n'
 '\t\t{ PB_DIAG(" %d:DOCTHUAN", id); continue; }\n',
 '\t\tif (!bDocLaNoi   // [NGUDOC-NOI 06/09] Ngu Doc: doc la chieu noi, khong loai\n'
 '\t\t && !p->IsPhysical() && pb_DonDoc(p)\n'
 '\t\t && !pb_DonPhepThat(p) && !pb_DonVatLy(p))\n'
 '\t\t{ PB_DIAG(" %d:DOCTHUAN", id); continue; }\n')

# ---- H8: tay diem bot noi ----
ap("H8a pb_TayDiemBotNoi (truoc TRANG BI THEO CAP)",
 '// ===========================================================================\n'
 '// TRANG BI THEO CAP (chu game 19/08 chieu): tieu not tiem nang ton dong, mac bo\n',
 '// ===========================================================================\n'
 '// [TAYDIEM 06/09] TAY DIEM BOT NOI - chu game: "cho bot noi tay lai diem va tang diem lai".\n'
 '// Bot noi dang mang chi so NGOAI (diem da tieu theo mau SM 50% khi con cam vu khi).\n'
 '// Bang goc = cua chinh game: chuyensinhdaisu.lua:122 / lenhbaitanthu.lua:212 as[ngu hanh]\n'
 '// = {Suc manh, Sinh khi, Than phap, Noi cong} (thu tu tham so SetBasePoint: LuaSetBasePoint\n'
 '// ScriptFuns.cpp:10680 gan m_nStrength=1, m_nVitality=2, m_nDexterity=3, m_nEngergy=4).\n'
 '// Quy sau tay = (cap-1)*5 + diem giu chuyen sinh, y het LuaSetBasePoint (ScriptFuns.cpp:10695).\n'
 '// Dat chi so qua ResetBaseAttribute (KPlayer.cpp:4542 -> ResetBase*: dat tuyet doi +\n'
 '// UpdataCurData + dong bo), roi pb_AllocAttribPoints chia lai (tay khong -> mau NOI).\n'
 '// Nhan biet "dang mang chi so ngoai" bang TY LE chu khong so bang tuyet doi (chia tung\n'
 '// cap 5 diem lam tron khac chia mot cuc): phai thuong SM >= 35% quy da tieu (mau noi 20%,\n'
 '// ngoai 50%); Vo Dang NC < 50% (mau noi 70%, ngoai 0%). Sau khi tay, ty le ve dung mau\n'
 '// -> khong lam lai moi lan restart / len cap.\n'
 '// ===========================================================================\n'
 'static const int s_nTayDiemGoc[series_num][4] = {   // {SM, SK, TP, NC}\n'
 '\t{ 35, 25, 25, 15 },   // 0 Kim\n'
 '\t{ 20, 35, 20, 25 },   // 1 Moc\n'
 '\t{ 25, 25, 25, 25 },   // 2 Thuy\n'
 '\t{ 30, 20, 30, 20 },   // 3 Hoa\n'
 '\t{ 20, 15, 25, 40 },   // 4 Tho\n'
 '};\n'
 '\n'
 'static void pb_TayDiemBotNoi(int nIdx, int nNpcIdx, PB_Bot& b)\n'
 '{\n'
 '\tconst int nSeries = Npc[nNpcIdx].m_Series;\n'
 '\tif (nSeries < 0 || nSeries >= series_num)\n'
 '\t\treturn;\n'
 '\tconst int* g = s_nTayDiemGoc[nSeries];\n'
 '\t// diem da tieu = chi so goc hien tai - bang goc (kep 0 neu nhan vat mau thap hon bang goc)\n'
 '\tint nSM = Player[nIdx].m_nStrength  - g[0];  if (nSM < 0) nSM = 0;\n'
 '\tint nSK = Player[nIdx].m_nVitality  - g[1];  if (nSK < 0) nSK = 0;\n'
 '\tint nTP = Player[nIdx].m_nDexterity - g[2];  if (nTP < 0) nTP = 0;\n'
 '\tint nNC = Player[nIdx].m_nEngergy   - g[3];  if (nNC < 0) nNC = 0;\n'
 '\tconst int nTieu = nSM + nSK + nTP + nNC;\n'
 '\tif (nTieu <= 0)\n'
 '\t\treturn;\n'
 '\tint bNgoai;\n'
 '\tif (b.nFaction == 8)                       // Vo Dang noi: 70% NC\n'
 '\t\tbNgoai = (nNC * 100 / nTieu < 50);\n'
 '\telse                                       // noi cac phai con lai: 20% SM\n'
 '\t\tbNgoai = (nSM * 100 / nTieu >= 35);\n'
 '\tif (!bNgoai)\n'
 '\t\treturn;\n'
 '\n'
 '\tconst int nQuy = (Npc[nNpcIdx].m_Level - 1) * 5\n'
 '\t               + Player[nIdx].m_cReBorn.GetReBornKeepQpiont();\n'
 '\tPLAYER_ADD_BASE_ATTRIBUTE_COMMAND cmd;\n'
 '\tcmd.ProtocolType = c2s_playeraddbaseattribute;\n'
 '\tcmd.m_btAttribute = ATTRIBUTE_STRENGTH;   cmd.m_nAddNo = g[0];  Player[nIdx].ResetBaseAttribute((BYTE*)&cmd);\n'
 '\tcmd.m_btAttribute = ATTRIBUTE_VITALITY;   cmd.m_nAddNo = g[1];  Player[nIdx].ResetBaseAttribute((BYTE*)&cmd);\n'
 '\tcmd.m_btAttribute = ATTRIBUTE_DEXTERITY;  cmd.m_nAddNo = g[2];  Player[nIdx].ResetBaseAttribute((BYTE*)&cmd);\n'
 '\tcmd.m_btAttribute = ATTRIBUTE_ENGERGY;    cmd.m_nAddNo = g[3];  Player[nIdx].ResetBaseAttribute((BYTE*)&cmd);\n'
 '\tPlayer[nIdx].m_nAttributePoint = nQuy;    // y het LuaSetBasePoint\n'
 '\tpb_Log("[BotTayDiem] %s phai %s cap %d: tay diem (da tieu SM=%d SK=%d TP=%d NC=%d)"\n'
 '\t       " -> ve goc he %d, chia lai %d diem theo duong NOI\\n",\n'
 '\t       Player[nIdx].m_PlayerName,\n'
 '\t       (b.nFaction >= 0 && b.nFaction < MAX_FACTION) ? s_facNpc[b.nFaction].szTen : "?",\n'
 '\t       (int)Npc[nNpcIdx].m_Level, nSM, nSK, nTP, nNC, nSeries, nQuy);\n'
 '\tpb_AllocAttribPoints(nIdx, b.nFaction);\n'
 '\tb.nAtkSkill = 0;   // tran mana/HP doi -> chon lai chieu (bo loc COST cua pb_PickSkill)\n'
 '}\n'
 '\n'
 '// ===========================================================================\n'
 '// TRANG BI THEO CAP (chu game 19/08 chieu): tieu not tiem nang ton dong, mac bo\n')

ap("H8b goi tay diem sau buoc 3b",
 '\t\t\tpb_Log("[BotNoi] %s phai %s cap %d: thao vu khi (duong NOI CONG danh tay khong)\\n",\n'
 '\t\t\t       Player[nIdx].m_PlayerName,\n'
 '\t\t\t       (b.nFaction >= 0 && b.nFaction < MAX_FACTION) ? s_facNpc[b.nFaction].szTen : "?",\n'
 '\t\t\t       nLevel);\n'
 '\t\t}\n'
 '\t}\n'
 '\n'
 '\t// ---- 4. dat cap 81: doi vu khi len CAP 10 cung loai dang cam ----\n',
 '\t\t\tpb_Log("[BotNoi] %s phai %s cap %d: thao vu khi (duong NOI CONG danh tay khong)\\n",\n'
 '\t\t\t       Player[nIdx].m_PlayerName,\n'
 '\t\t\t       (b.nFaction >= 0 && b.nFaction < MAX_FACTION) ? s_facNpc[b.nFaction].szTen : "?",\n'
 '\t\t\t       nLevel);\n'
 '\t\t}\n'
 '\t\t// ---- 3c. [TAYDIEM 06/09] bot noi (da tay khong) con mang chi so NGOAI -> tay\n'
 '\t\t// diem ve bang goc cua game roi chia lai theo mau NOI (xem pb_TayDiemBotNoi) ----\n'
 '\t\tif (Player[nIdx].m_ItemList.GetEquipment(itempart_weapon) <= 0)\n'
 '\t\t\tpb_TayDiemBotNoi(nIdx, nNpcIdx, b);\n'
 '\t}\n'
 '\n'
 '\t// ---- 4. dat cap 81: doi vu khi len CAP 10 cung loai dang cam ----\n')

if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
sau = sum(1 for c in s if ord(c) > 127)
print("da ap %d hunk; high-byte truoc=%d sau=%d %s" % (n, truoc, sau, "OK" if truoc == sau else "LECH!"))
