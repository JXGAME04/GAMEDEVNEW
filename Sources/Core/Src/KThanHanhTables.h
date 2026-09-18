// ===== MENU THAN HANH PHU (6/1/1271) - SINH TU DONG boi ReverseTools/gen_thanhanh_tables.py
// tu bin\server\script\vatpham\ib\shenxingfu.lua. DUNG SUA TAY - script doi thi sinh lai.
// Duong menu toi bai luyen cong: "Su dung thuat than hanh..." (TKM_OPT_THP_DI) -> THPM_LUYEN
//   -> THPM_LV90 (cap 90) | THPM_LV2080 -> THPM_CAP + so cap (20..80) -> ten bai (bang duoi).
// Ten giu nguyen byte TCVN3 nhu script (strstr len thoai). Chi N dong dau ma gopos_step2lvNN liet ke.
#ifndef KTHANHANHTABLES_H
#define KTHANHANHTABLES_H

struct THPBaiRow { int nMapId; int nCap; const char* szTen; };
#define THP_BAI_COUNT	31
static const THPBaiRow g_aTHPBai[THP_BAI_COUNT] =
{
	{ 75  , 90, "Kho¶ Lang ®éng" },
	{ 93  , 90, "TiÕn Cóc §éng MËt Cung" },
	{ 124 , 90, "C¸n Viªn §éng Mª Cung" },
	{ 144 , 90, "D­îc V­¬ng ®éng tÇng 4" },
	{ 152 , 90, "TuyÕt B¸o §éng TÇng 8" },
	{ 225 , 90, "Sa M¹c Mª Cung 1" },
	{ 226 , 90, "Sa M¹c Mª Cung 2" },
	{ 227 , 90, "Sa M¹c Mª Cung 3" },
	{ 321 , 90, "Tr­êng B¹ch S¬n Nam" },
	{ 322 , 90, "Tr­êng B¹ch S¬n B¾c" },
	{ 336 , 90, "Phong L¨ng ®é" },
	{ 340 , 90, "M¹c Cao QuËt" },
	{ 875 , 90, "H¾c Sa ®éng" },
	{ 181 , 80, "L­ìng Thñy §éng" },
	{ 198 , 80, "Thanh Khª §éng" },
	{ 224 , 80, "Sa M¹c ®Þa biÓu" },
	{ 320 , 80, "Ch©n nói Tr­êng B¹ch" },
	{ 123 , 70, "L·o Hæ §éng" },
	{ 206 , 70, "TÇn L¨ng tÇng 2" },
	{ 319 , 70, "L©m Du Quan" },
	{ 56  , 60, "Hoµnh S¬n Ph¸i" },
	{ 79  , 60, "T­¬ng D­¬ng Nha M«n MËt §¹o" },
	{ 166 , 60, "Thiªn T©m Th¸p tÇng 3" },
	{ 164 , 50, "Thiªn T©m Th¸p" },
	{ 182 , 50, "NghiÖt Long §éng" },
	{ 21  , 40, "Thanh Thµnh S¬n" },
	{ 167 , 40, "§iÓm Th­¬ng S¬n" },
	{ 170 , 30, "Thæ PhØ §éng" },
	{ 193 , 30, "Vò Di S¬n" },
	{ 7   , 20, "TÇn L¨ng tÇng 1" },
	{ 19  , 20, "KiÕm C¸c T©y Nam" },
};
static const char THPM_LUYEN[]  = "B¶n ®å luyÖn c«ng tõ 20 ®Õn 90";	// chondiadiem1 -> gotoluyencong
static const char THPM_LV90[]   = "B¶n ®å luyÖn c«ng 90 trë lªn.";	// gotoluyencong -> gopos_step2lv90
static const char THPM_LV2080[] = "B¶n ®å LuyÖn c«ng 20 ®Õn 80.";	// gotoluyencong -> luyencongtanthu
static const char THPM_CAP[]    = "Di chuyÓn ®Õn b¶n ®å luyÖn c«ng cÊp ";	// luyencongtanthu: + "20".."80" -> gopos_step2lvNN

#endif // KTHANHANHTABLES_H
