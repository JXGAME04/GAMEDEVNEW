// ===== "NHUNG NOI DA DI QUA" cua Xa Phu - SINH TU DONG boi
// ReverseTools/gen_waypoint_tables.py tu bin\server\settings\WayPoint.txt. DUNG SUA TAY.
// Xa Phu co HAI danh sach (script/global/npcchucnang/xaphu.lua):
//   "Nhung noi da di qua"       -> WayPoint.txt (bang nay: dong / bai luyen cong)
//   "Nhung thanh thi da di qua" -> Station.txt  (bang g_aDTSapTown trong CoreShell.cpp)
// Mot map co the co nhieu diem trung ten - chi giu ten cua diem dau tien vi menu
// chon theo TEN. Ten giu nguyen byte TCVN3 nhu trong tep goc.
#ifndef KWAYPOINTTABLES_H
#define KWAYPOINTTABLES_H

struct WayPointRow { int nMapId; const char* szMenu; };
#define WAYPOINT_COUNT	81
static const WayPointRow g_aWayPoint[WAYPOINT_COUNT] =
{
	{ 2   , "Hoa s¬n" },
	{ 3   , "KiÕm C¸c Thôc §¹o" },
	{ 4   , "Kim Quang ®éng" },
	{ 5   , "Kinh Hoµng ®éng" },
	{ 6   , "Táa V©n ®éng" },
	{ 7   , "L¨ng TÇn Thñy Hoµng" },
	{ 8   , "tÇng 1 TÇn L¨ng" },
	{ 9   , "Nh¹n Th¹ch ®éng" },
	{ 10  , "Nh¹n Th¹ch ®éng" },
	{ 12  , "HËu ViÖn TÝn T­íng Tù" },
	{ 14  , "M·nh Hæ ®éng" },
	{ 19  , "Kinh Hoµng ®éng" },
	{ 20  , "Phï Dung ®éng" },
	{ 21  , "Thanh Thµnh s¬n" },
	{ 22  , "B¹ch V©n ®éng" },
	{ 23  , "ThÇn Tiªn ®éng" },
	{ 24  , "H­ëng Thñy ®éng" },
	{ 26  , "Tróc T¬ ®éng" },
	{ 38  , "ThiÕt Th¸p mª cung" },
	{ 41  , "Phôc Ng­u s¬n  B¾c" },
	{ 42  , "Thiªn T©m ®éng" },
	{ 43  , "KiÕm C¸c Thôc §¹o" },
	{ 53  , "Vò L¨ng ®éng" },
	{ 56  , "Hoµnh s¬n ph¸i" },
	{ 65  , "Thiªn V­¬ng ®éng" },
	{ 66  , "D­íi ®¸y §éng §×nh Hå" },
	{ 69  , "Thanh Loa ®éng" },
	{ 70  , "Vò L¨ng s¬n" },
	{ 71  , "B¹ch Thñy ®éng" },
	{ 72  , "§¹i Tï ®éng" },
	{ 73  , "Phôc L­u ®éng" },
	{ 74  , "Miªu LÜnh" },
	{ 75  , "Kho¶ Lang ®éng" },
	{ 76  , "S¬n B¶o ®éng" },
	{ 77  , "YÕn Tö ®éng" },
	{ 79  , "Nha m«n mËt ®¹o T­¬ng D­¬ng" },
	{ 82  , "Mª cung ®¸y giÕng" },
	{ 83  , "Háa Lang ®éng" },
	{ 90  , "Kª Qu¸n ®éng" },
	{ 91  , "Kª Qu¸n ®éng" },
	{ 92  , "TiÕn Cóc ®éng" },
	{ 93  , "TiÕn Cóc ®éng" },
	{ 94  , "Linh Cèc ®éng" },
	{ 99  , "B¨ng Hµ ®éng" },
	{ 100 , "Phi Thiªn ®éng" },
	{ 101 , "D­¬ng Trung ®éng" },
	{ 114 , "108 La H¸n trËn" },
	{ 120 , "TÇng 5 T­êng V©n ®éng" },
	{ 121 , "Cæ D­¬ng ®éng" },
	{ 122 , "L·o Hæ ®éng" },
	{ 123 , "L·o Hæ ®éng" },
	{ 124 , "C¸n Viªn ®éng" },
	{ 136 , "Sa M¹c Mª Cung" },
	{ 140 , "TuyÕt B¸o ®éng" },
	{ 143 , "TÇng 3 D­îc V­¬ng ®éng" },
	{ 144 , "TÇng 4 D­îc V­¬ng ®éng" },
	{ 153 , "V« Danh ®éng" },
	{ 164 , "Thiªn TÇm th¸p tÇng 1" },
	{ 167 , "§iÓm Th­¬ng s¬n" },
	{ 174 , "Thanh Khª ®éng" },
	{ 179 , "La Tiªu s¬n" },
	{ 193 , "Vò Di s¬n" },
	{ 195 , "Nh¹n §·ng s¬n" },
	{ 198 , "Thanh Khª ®éng" },
	{ 199 , "Vò L¨ng ®éng" },
	{ 200 , "Cæ D­¬ng ®éng" },
	{ 201 , "B¨ng Hµ ®éng" },
	{ 202 , "Phï Dung ®éng" },
	{ 203 , "V« Danh ®éng" },
	{ 204 , "Phi Thiªn ®éng" },
	{ 205 , "D­¬ng Trung ®éng" },
	{ 206 , "TÇn L¨ng tÇng 2" },
	{ 207 , "TÇn L¨ng tÇng 3" },
	{ 319 , "L©m Du Quan" },
	{ 320 , "Tr­êng B¹ch s¬n" },
	{ 321 , "Tr­êng B¹ch s¬n Nam" },
	{ 322 , "Tr­êng B¹ch s¬n B¾c" },
	{ 332 , "Thiªn Long Tù" },
	{ 333 , "Hoa S¬n ph¸i" },
	{ 336 , "Phong L¨ng ®é" },
	{ 340 , "M¹c Cao QuËt" },
};

// map nay co trong menu "Nhung noi da di qua"? tra ten muc, NULL = khong co
static const char* WayPointMenu(int nMapId)
{
	for (int i = 0; i < WAYPOINT_COUNT; ++i)
		if (g_aWayPoint[i].nMapId == nMapId)
			return g_aWayPoint[i].szMenu;
	return NULL;
}

#endif // KWAYPOINTTABLES_H
