# -*- coding: utf-8 -*-
# [BOTNOI dot 3 06/09] Chu game (06/09 toi), 4 viec:
#   * "toi muon viet them ham nang mach cho toan bo bot"      -> H14 PB_NangMach(nCap[,nMach]) + Lua
#   * "bot cap 110 se cho mac ngua chieu da"                    -> H11 buoc 3: Chieu Da Ngoc Su Tu cap 10
#   * "bot co vu khi se cho random ti le nMagicLevel tu 7 - 8"  -> H10/H12 pb_MagicVuKhi: 6 dong cap 7-8
#   * "bot len 120 se co skill 120 full skill"                  -> H9/H13 bot_hoc120 (hocvocong.lua)
# Sua 3 tep: KPlayerBot.cpp (H9-H14), KPlayerBot.h (khai bao), ScriptFuns.cpp (dang ky Lua).
# AP SAU goi_va_botnoi_dot2_0609.py. Idempotent. Chi ASCII. Tham so 1 = goc cay (mac dinh worktree).
import io, os, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_botnoi"
SRC = os.path.join(ROOT, "Sources", "Core", "Src")

def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()

def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)

class Tep:
    def __init__(self, ten):
        self.p = os.path.join(SRC, ten)
        self.s = doc(self.p)
        self.truoc = sum(1 for c in self.s if ord(c) > 127)
        self.crlf = "\r\n" in self.s
        self.n = 0
    def ap(self, ten, cu, moi):
        if self.crlf:
            cu = cu.replace("\n", "\r\n"); moi = moi.replace("\n", "\r\n")
        if moi in self.s:
            print("  [=] %s da ap tu truoc" % ten); return
        if self.s.count(cu) != 1:
            print("LOI: neo %s khop %d cho (can 1)" % (ten, self.s.count(cu))); sys.exit(1)
        self.s = self.s.replace(cu, moi); self.n += 1
        print("  [+] %s" % ten)
    def chen_sau_dong(self, ten, chua, them):
        # chen 'them' (co the nhieu dong, ket thuc bang \n) ngay sau dong DUY NHAT chua 'chua'
        nl = "\r\n" if self.crlf else "\n"
        them = them.replace("\n", nl)
        if them in self.s:
            print("  [=] %s da ap tu truoc" % ten); return
        lines = self.s.split(nl)
        hit = [i for i, l in enumerate(lines) if chua in l]
        if len(hit) != 1:
            print("LOI: neo %s khop %d dong (can 1)" % (ten, len(hit))); sys.exit(1)
        lines.insert(hit[0] + 1, them.rstrip(nl))
        self.s = nl.join(lines); self.n += 1
        print("  [+] %s" % ten)
    def xong(self):
        if self.n:
            ghi(self.p, self.s)
        sau = sum(1 for c in self.s if ord(c) > 127)
        print("%s: %d hunk; high-byte %d -> %d %s" % (os.path.basename(self.p), self.n, self.truoc, sau,
              "OK" if self.truoc == sau else "LECH!"))

# =========================== KPlayerBot.cpp ===========================
cpp = Tep("KPlayerBot.cpp")

cpp.ap("H9a PB_Bot.nHoc120",
 '\tint          nHoc90;                      // (20/08) 1 = da goi hoc bo ky nang 90 doi nay\n',
 '\tint          nHoc90;                      // (20/08) 1 = da goi hoc bo ky nang 90 doi nay\n'
 '\tint          nHoc120;                     // [SKILL120 06/09] 1 = da hoc ky nang 120 doi nay\n')

cpp.ap("H9b khoi tao nHoc120",
 '\t\tb.nTrangBiLevel = 0;  b.nQhtTick = 0;  b.nHoc90 = 0;\n',
 '\t\tb.nTrangBiLevel = 0;  b.nQhtTick = 0;  b.nHoc90 = 0;  b.nHoc120 = 0;\n')

cpp.ap("H10a pb_MagicVuKhi",
 '// ===========================================================================\n'
 '// TRAO VU KHI NHAP MON THEO PHAI (cap 1)\n',
 '// ===========================================================================\n'
 '// [VKMAGIC 06/09] Chu game: "bot co vu khi se cho random ti le nMagicLevel tu 7 - 8".\n'
 '// Mang nMagicLevel[MAX_ITEM_MAGICLEVEL] cua ItemSet.Add -> Gen_Equipment -> Gen_MagicAttrib\n'
 '// (KItemGenerator.CPP:593): o [i] (i < 6) = CAP DONG THUOC TINH thu i (1..10, bang\n'
 '// magicattrib theo (tien/hau to, loai, he, cap)); 0 = dung sinh dong tu do. Bot truoc\n'
 '// 06/09 truyen toan 0 = vu khi TRANG. Nay 6 o = 7 hoac 8 ngau nhien tung o; 6 o sau = 0.\n'
 '// ===========================================================================\n'
 'static void pb_MagicVuKhi(int* nMagic)\n'
 '{\n'
 '\tZeroMemory(nMagic, sizeof(int) * MAX_ITEM_MAGICLEVEL);\n'
 '\tfor (int i = 0; i < MAX_ITEM_MAGICATTRIB && i < MAX_ITEM_MAGICLEVEL; i++)\n'
 '\t\tnMagic[i] = 7 + (int)g_Random(2);\n'
 '}\n'
 '\n'
 '// ===========================================================================\n'
 '// TRAO VU KHI NHAP MON THEO PHAI (cap 1)\n')

cpp.ap("H10b vu khi nhap mon co dong 7-8",
 '\tconst int nSeries = nFaction / 2;   // phai = series*2 + {0,1}\n'
 '\tint nMagic[MAX_ITEM_MAGICLEVEL];\n'
 '\tZeroMemory(nMagic, sizeof(nMagic));\n'
 '\t// (nItemNature=0, nItemGenre=0 item_equip, nSeries, nLevel=1, nLuck=0, nDetail, nParticular)\n',
 '\tconst int nSeries = nFaction / 2;   // phai = series*2 + {0,1}\n'
 '\tint nMagic[MAX_ITEM_MAGICLEVEL];\n'
 '\tpb_MagicVuKhi(nMagic);   // [VKMAGIC 06/09] 6 dong cap 7-8\n'
 '\t// (nItemNature=0, nItemGenre=0 item_equip, nSeries, nLevel=1, nLuck=0, nDetail, nParticular)\n')

cpp.ap("H11 ngua Chieu Da tu cap 110",
 '\t// ---- 3. ngua Tuc Suong cap 10 (horse.txt dong 30-31: detail 10 = equip_horse,\n'
 '\t//         particular 2; cot cap co ban 9 va 10 - chu game dan lay cap 10) ----\n'
 '\t{\n'
 '\t\tconst int nNgua = Player[nIdx].m_ItemList.GetEquipment(itempart_horse);\n'
 '\t\tconst bool bDaCo = (nNgua > 0 && Item[nNgua].GetDetailType() == equip_horse\n'
 '\t\t                 && Item[nNgua].GetParticular() == 2\n'
 '\t\t                 && Item[nNgua].GetLevel() >= 10);\n'
 '\t\tif (!bDaCo)\n'
 '\t\t{\n'
 '\t\t\tint nMagic[MAX_ITEM_MAGICLEVEL];\n'
 '\t\t\tZeroMemory(nMagic, sizeof(nMagic));\n'
 '\t\t\tconst int nNew = ItemSet.Add(0, 0, Npc[nNpcIdx].m_Series, 10, 0,\n'
 '\t\t\t                             equip_horse, 2, nMagic,\n'
 '\t\t\t                             g_SubWorldSet.GetGameVersion(), 0);\n'
 '\t\t\tif (nNew > 0 && pb_MacVaoNguoi(nIdx, nNew, -1))\n'
 '\t\t\t\tpb_Log("[BotTrangBi] %s cap %d cuoi ngua Tuc Suong cap 10\\n",\n'
 '\t\t\t\t       Player[nIdx].m_PlayerName, nLevel);\n'
 '\t\t}\n'
 '\t}\n',
 '\t// ---- 3. ngua: duoi cap 110 Tuc Suong cap 10 (horse.txt dong 30-31: detail 10 =\n'
 '\t//         equip_horse, particular 2). [NGUA110 06/09] chu game: "bot cap 110 se cho\n'
 '\t//         mac ngua chieu da" -> tu cap 110 Chieu Da Ngoc Su Tu cap 10 (horse.txt\n'
 '\t//         dong 61: particular 5, cap 10; dong 130 ban Hoang Kim particular 12 khong\n'
 '\t//         dung). Ngua cu (khong phai Hoang Kim) thao huy nhu vu khi buoc 4; kiem\n'
 '\t//         CanEquip mon moi TRUOC khi huy mon cu. ----\n'
 '\t{\n'
 '\t\tconst int  nNguaParti = (nLevel >= 110) ? 5 : 2;\n'
 '\t\tconst int  nNgua = Player[nIdx].m_ItemList.GetEquipment(itempart_horse);\n'
 '\t\tconst bool bDaCo = (nNgua > 0 && Item[nNgua].GetDetailType() == equip_horse\n'
 '\t\t                 && Item[nNgua].GetParticular() == nNguaParti\n'
 '\t\t                 && Item[nNgua].GetLevel() >= 10);\n'
 '\t\tconst bool bHoangKim = (nNgua > 0 && Item[nNgua].GetGoldId() != 0);\n'
 '\t\tif (!bDaCo && !bHoangKim)\n'
 '\t\t{\n'
 '\t\t\tint nMagic[MAX_ITEM_MAGICLEVEL];\n'
 '\t\t\tZeroMemory(nMagic, sizeof(nMagic));\n'
 '\t\t\tconst int nNew = ItemSet.Add(0, 0, Npc[nNpcIdx].m_Series, 10, 0,\n'
 '\t\t\t                             equip_horse, nNguaParti, nMagic,\n'
 '\t\t\t                             g_SubWorldSet.GetGameVersion(), 0);\n'
 '\t\t\tif (nNew > 0 && !Player[nIdx].m_ItemList.CanEquip(nNew, -1))\n'
 '\t\t\t\tItemSet.Remove(nNew);\n'
 '\t\t\telse if (nNew > 0)\n'
 '\t\t\t{\n'
 '\t\t\t\tif (nNgua > 0)\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tif (Npc[nNpcIdx].m_bRideHorse)\n'
 '\t\t\t\t\t\tPlayer[nIdx].CheckRideHorse(TRUE);   // dang cuoi -> xuong ngua truoc khi thao\n'
 '\t\t\t\t\tPlayer[nIdx].m_ItemList.RemoveItemIdx(nNgua, Item[nNgua].GetStackNum());\n'
 '\t\t\t\t}\n'
 '\t\t\t\tif (pb_MacVaoNguoi(nIdx, nNew, -1))\n'
 '\t\t\t\t\tpb_Log("[BotTrangBi] %s cap %d cuoi ngua %s cap 10\\n",\n'
 '\t\t\t\t\t       Player[nIdx].m_PlayerName, nLevel,\n'
 '\t\t\t\t\t       (nNguaParti == 5) ? "Chieu Da Ngoc Su Tu" : "Tuc Suong");\n'
 '\t\t\t}\n'
 '\t\t}\n'
 '\t}\n')

cpp.ap("H12a vu khi cap 10: dong 7-8 + sinh lai vu khi trang",
 '\tif (nLevel >= 81)\n'
 '\t{\n'
 '\t\tconst int nW = Player[nIdx].m_ItemList.GetEquipment(itempart_weapon);\n'
 '\t\tif (nW > 0 && Item[nW].GetGoldId() == 0 && Item[nW].GetLevel() < 10)\n'
 '\t\t{\n'
 '\t\t\tint nMagic[MAX_ITEM_MAGICLEVEL];\n'
 '\t\t\tZeroMemory(nMagic, sizeof(nMagic));\n'
 '\t\t\tconst int nNew = ItemSet.Add(0, 0, Item[nW].GetSeries(), 10, 0,\n',
 '\t// [VKMAGIC 06/09] chu game: "bot co vu khi se cho random ti le nMagicLevel tu 7 - 8":\n'
 '\t// vu khi cap 10 sinh moi mang 6 dong cap 7-8 (pb_MagicVuKhi); vu khi cap 10 CU chua co\n'
 '\t// dong (GetTotalMagicLevel() == 0 = moi vu khi bot sinh truoc 06/09) duoc sinh lai MOT\n'
 '\t// LAN cung loai/he/cap, co dong 7-8 (sau do tong dong > 0 -> khong lam lai).\n'
 '\tif (nLevel >= 81)\n'
 '\t{\n'
 '\t\tconst int nW = Player[nIdx].m_ItemList.GetEquipment(itempart_weapon);\n'
 '\t\tif (nW > 0 && Item[nW].GetGoldId() == 0\n'
 '\t\t && (Item[nW].GetLevel() < 10 || Item[nW].GetTotalMagicLevel() <= 0))\n'
 '\t\t{\n'
 '\t\t\tconst int nCapVk = (Item[nW].GetLevel() < 10) ? 10 : Item[nW].GetLevel();\n'
 '\t\t\tint nMagic[MAX_ITEM_MAGICLEVEL];\n'
 '\t\t\tpb_MagicVuKhi(nMagic);\n'
 '\t\t\tconst int nNew = ItemSet.Add(0, 0, Item[nW].GetSeries(), nCapVk, 0,\n')

cpp.ap("H12b log vu khi moi",
 '\t\t\t\t\tpb_Log("[BotTrangBi] %s cap %d len vu khi cap 10 (detail %d parti %d)\\n",\n'
 '\t\t\t\t\t       Player[nIdx].m_PlayerName, nLevel,\n'
 '\t\t\t\t\t       Item[nNew].GetDetailType(), Item[nNew].GetParticular());\n',
 '\t\t\t\t\tpb_Log("[BotTrangBi] %s cap %d len vu khi cap %d (detail %d parti %d,"\n'
 '\t\t\t\t\t       " dong 7-8 tong %d)\\n",\n'
 '\t\t\t\t\t       Player[nIdx].m_PlayerName, nLevel, Item[nNew].GetLevel(),\n'
 '\t\t\t\t\t       Item[nNew].GetDetailType(), Item[nNew].GetParticular(),\n'
 '\t\t\t\t\t       Item[nNew].GetTotalMagicLevel());\n')

cpp.ap("H13 hoc ky nang 120",
 '\t\tb.nAtkSkill = 0;   // chon lai chieu danh - co the co chieu 90 manh hon\n'
 '\t}\n'
 '\n'
 '\t// ---- 2. bo Kim Phong: mon nao chua mac ma du dieu kien thi mac ----\n',
 '\t\tb.nAtkSkill = 0;   // chon lai chieu danh - co the co chieu 90 manh hon\n'
 '\t}\n'
 '\n'
 '\t// ---- 1c. [SKILL120 06/09] chu game: "bot len 120 se co skill 120 full skill" ->\n'
 '\t// bot_hoc120(nCurFac) trong hocvocong.lua: AddMagic(SKILL120AR[nCurFac], 20) = chieu\n'
 '\t// 120 cua phai (709..717 / 1365 / 1984 / 2127, deu bi dong-tu buff) o cap 20 = max\n'
 '\t// (skills.txt MaxLevel 20) - dung dong show_kynang90 cap cho nguoi choi. Khong kem\n'
 '\t// 210 khinh cong / SKILL150_ARRAY. Goi lai sau nap-bot-cu vo hai (KSkillList::Add\n'
 '\t// chi nang, khong ha). ----\n'
 '\tif (nLevel >= 120 && !b.nHoc120 && b.nFaction >= 0)\n'
 '\t{\n'
 '\t\tb.nHoc120 = 1;\n'
 '\t\tPlayer[nIdx].ExecuteScript((char*)"\\\\script\\\\global\\\\hocvocong.lua",\n'
 '\t\t                           (char*)"bot_hoc120",\n'
 '\t\t                           (int)Player[nIdx].m_cFaction.m_nCurFaction + 1,\n'
 '\t\t                           false);\n'
 '\t\tpb_Log("[BotSkill120] %s cap %d hoc ky nang 120 cap 20 (phai %d)\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, nLevel,\n'
 '\t\t       (int)Player[nIdx].m_cFaction.m_nCurFaction + 1);\n'
 '\t\tb.nAtkSkill = 0;\n'
 '\t}\n'
 '\n'
 '\t// ---- 2. bo Kim Phong: mon nao chua mac ma du dieu kien thi mac ----\n')

cpp.ap("H14 PB_NangMach + LuaPB_NangMach",
 'int LuaPB_SaveAll(Lua_State* L)\n'
 '{\n'
 '\tLua_PushNumber(L, PB_SaveAll());\n'
 '\treturn 1;\n'
 '}\n',
 'int LuaPB_SaveAll(Lua_State* L)\n'
 '{\n'
 '\tLua_PushNumber(L, PB_SaveAll());\n'
 '\treturn 1;\n'
 '}\n'
 '\n'
 '// ===========================================================================\n'
 '// [NANGMACH 06/09] NANG KINH MACH CHO TOAN BO BOT - chu game: "toi muon viet them ham\n'
 '// nang mach cho toan bo bot". Lenh GM/Lua PB_NangMach(nCap [, nMach]):\n'
 '//   nCap  0..32 (MAX_MERIDIAN_LEVEL) - cap dat cho mach (moi huyet toi cap do)\n'
 '//   nMach 0 = ca 12 mach (MAX_MERIDIAN), 1..12 = mot mach (meridian.txt: 1 Doc, 2 Nham,\n'
 '//         3 Xung, 4 Dai, 5-12 con lai)\n'
 '// Di dung duong SetMeridian cua Lua (ScriptFuns.cpp:315 LuaSetPlayerMeridianValue):\n'
 '// setMeridian (ma mach dem tu 0) roi ApplyMaridianToNPC / RemoveMaridianFromNPC (dem\n'
 '// tu 1 - [KM 27/08b]) + UpdataCurData. Cap mach nam trong blob role (KPlayerDBFuns.cpp\n'
 '// :1088 szStringduphong2) -> ben qua restart sau khi luu (goi PB_SaveAll); KPlayer.cpp\n'
 '// :3103 ap lai luc dang nhap. Khong gui s2c_syncmeridian (bot khong co client).\n'
 '// Tra so bot co it nhat mot mach doi. Menu lenh bai: simcity_admin.lua PB_MachMenu.\n'
 '// ===========================================================================\n'
 'static int pb_DatMach(int nIdx, int nMach0, int nCap)\n'
 '{\n'
 '\tif (nMach0 < 0 || nMach0 >= MAX_MERIDIAN || nCap < 0 || nCap > MAX_MERIDIAN_LEVEL)\n'
 '\t\treturn 0;\n'
 '\tconst int nNpcIdx = Player[nIdx].m_nIndex;\n'
 '\tif (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)\n'
 '\t\treturn 0;\n'
 '\tconst int nDiff = Player[nIdx].m_cMeridian.setMeridian(nMach0, nCap);\n'
 '\tif (nDiff == 0)\n'
 '\t\treturn 0;\n'
 '\tif (nDiff > 0)\n'
 '\t\tMeridianManager.ApplyMaridianToNPC(&Npc[nNpcIdx], nMach0 + 1, nCap, nDiff);\n'
 '\telse\n'
 '\t\tMeridianManager.RemoveMaridianFromNPC(&Npc[nNpcIdx], nMach0 + 1, nCap, nDiff);\n'
 '\treturn 1;\n'
 '}\n'
 '\n'
 'int PB_NangMach(int nCap, int nMach)\n'
 '{\n'
 '\tif (nCap < 0) nCap = 0;\n'
 '\tif (nCap > MAX_MERIDIAN_LEVEL) nCap = MAX_MERIDIAN_LEVEL;\n'
 '\tif (nMach < 0 || nMach > MAX_MERIDIAN)\n'
 '\t\treturn 0;\n'
 '\tint nBot = 0;\n'
 '\tfor (int i = 0; i < s_botCount; i++)\n'
 '\t{\n'
 '\t\tPB_Bot& b = s_bots[i];\n'
 '\t\tconst int nIdx = b.nPlayerIdx;\n'
 '\t\tif (nIdx <= 0 || nIdx >= MAX_PLAYER || Player[nIdx].m_nIndex <= 0)\n'
 '\t\t\tcontinue;\n'
 '\t\tint nDoi = 0;\n'
 '\t\tif (nMach == 0)\n'
 '\t\t{\n'
 '\t\t\tfor (int m = 0; m < MAX_MERIDIAN; m++)\n'
 '\t\t\t\tnDoi += pb_DatMach(nIdx, m, nCap);\n'
 '\t\t}\n'
 '\t\telse\n'
 '\t\t\tnDoi = pb_DatMach(nIdx, nMach - 1, nCap);\n'
 '\t\tif (nDoi > 0)\n'
 '\t\t{\n'
 '\t\t\tPlayer[nIdx].UpdataCurData();\n'
 '\t\t\tnBot++;\n'
 '\t\t}\n'
 '\t}\n'
 '\tpb_Log("[BotMach] nang mach %d (0 = ca 12) len cap %d: %d/%d bot doi\\n",\n'
 '\t       nMach, nCap, nBot, s_botCount);\n'
 '\tif (nBot > 0)\n'
 '\t\tPB_SaveAll();               // ghi blob de ben qua restart\n'
 '\treturn nBot;\n'
 '}\n'
 '\n'
 'int LuaPB_NangMach(Lua_State* L)\n'
 '{\n'
 '\tconst int nCap  = (Lua_GetTopIndex(L) >= 1) ? (int)Lua_ValueToNumber(L, 1) : 0;\n'
 '\tconst int nMach = (Lua_GetTopIndex(L) >= 2) ? (int)Lua_ValueToNumber(L, 2) : 0;\n'
 '\tLua_PushNumber(L, PB_NangMach(nCap, nMach));\n'
 '\treturn 1;\n'
 '}\n')
cpp.xong()

# =========================== KPlayerBot.h ===========================
h = Tep("KPlayerBot.h")
h.chen_sau_dong("Hh khai bao LuaPB_NangMach", "LuaPB_SaveAll(Lua_State* L);",
 'int  LuaPB_NangMach(Lua_State* L);          // [NANGMACH 06/09] (nCap[,nMach]) -> so bot doi mach\n'
 'int  PB_NangMach(int nCap, int nMach);      // [NANGMACH 06/09] nang kinh mach toan bo bot\n')
h.xong()

# =========================== ScriptFuns.cpp ===========================
sf = Tep("ScriptFuns.cpp")
sf.ap("Hs1 extern LuaPB_NangMach",
 'extern int LuaPB_SaveAll(Lua_State* L);\n',
 'extern int LuaPB_SaveAll(Lua_State* L);\n'
 'extern int LuaPB_NangMach(Lua_State* L);\t// [NANGMACH 06/09]\n')
sf.chen_sau_dong("Hs2 dang ky PB_NangMach", '{"PB_SaveAll",',
 '\t{"PB_NangMach",\t\tLuaPB_NangMach},\t// [NANGMACH 06/09] (nCap[,nMach]) nang kinh mach toan bo bot, ghi blob\n')
sf.xong()
