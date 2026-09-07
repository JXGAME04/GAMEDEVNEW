// ============================================================================
// KVanTieuMsg.h - SINH TU DONG boi ReverseTools/vantieu/gen_vantieu_chinam.py - DUNG SUA TAY.
// Thong bao [Chi nam] cua bo dan duong van tieu (CoreShell.cpp TG_VanTieu*) + ten NPC.
// Chuoi TCVN3 tho (Core.vcxproj: source-charset = execution-charset windows-1258).
// ============================================================================
#ifndef KVANTIEU_MSG_H
#define KVANTIEU_MSG_H

// ben Xa Phu (settings\Station.txt cot DESC): map -> nhan trong thoai 'thanh thi da di qua'
#define VT_XAPHU_SO 16
static const short s_nVTXaPhuMap[VT_XAPHU_SO] = { 1, 11, 162, 37, 78, 80, 176, 20, 121, 53, 54, 174, 101, 99, 100, 153 };
static const char* const s_szVTXaPhuTen[VT_XAPHU_SO] = {
	"Ph­îng T­êng Phñ",
	"Thµnh §« Phñ",
	"Phñ §¹i Lý",
	"BiÖn Kinh Phñ",
	"T­¬ng D­¬ng Phñ",
	"D­¬ng Ch©u Phñ",
	"L©m An Phñ",
	"Th«n Giang T©n",
	"TrÊn Long M«n",
	"Ba L¨ng huyÖn",
	"Nam Nh¹c trÊn",
	"Th«n Long TuyÒn",
	"Th«n §¹o H­¬ng",
	"TrÊn VÜnh L¹c",
	"TrÊn Chu Tiªn",
	"TrÊn Th¹ch Cæ",
};

#define VT_MSG_HUY               "<color=Cyan>[ChØ nam] §· hñy dÉn ®­êng vËn tiªu."
#define VT_MSG_DT                "<color=Yellow>[ChØ nam] Auto D· TÈu ®ang ch¹y - ®Ó auto tù lo viÖc di chuyÓn."
#define VT_MSG_LAU               "<color=Yellow>[ChØ nam] §i qu¸ l©u - dõng dÉn ®­êng."
#define VT_MSG_SUKIEN_FMT        "<color=Yellow>[ChØ nam] §ang ë map sù kiÖn (%s) - ChØ nam kh«ng tù di chuyÓn ë ®©y."
#define VT_MSG_CN_XUATPHAT       "<color=Yellow>[ChØ nam] Xe ®· xuÊt ph¸t - h·y hé tèng xe ®i bé tíi Tiªu S­ ®iÓm cuèi (kh«ng dÉn ®­êng qua b¶n ®å)."
#define VT_MSG_TOI_NPC_FMT       "<color=Cyan>[ChØ nam] §ang ch¹y tíi %s - bÊm l¹i vµo dßng nhiÖm vô ®Ó hñy."
#define VT_MSG_DEN_NPC_FMT       "<color=Cyan>[ChØ nam] §· tíi %s - h·y chän môc trong khung tho¹i."
#define VT_MSG_PHU               "<color=Cyan>[ChØ nam] §ang ë map luyÖn c«ng - dïng phï vÒ thµnh, råi ra Xa Phu ®i tiÕp..."
#define VT_MSG_KHONGPHU          "<color=Yellow>[ChØ nam] Kh«ng cã phï vÒ thµnh trong tói - h·y tù vÒ thµnh råi bÊm l¹i."
#define VT_MSG_XAPHU_DI_FMT      "<color=Cyan>[ChØ nam] §ang ch¹y tíi Xa Phu ®Ó ®i %s (tèn tiÒn xe) - bÊm l¹i ®Ó hñy."
#define VT_MSG_XAPHU_KHONGCO_FMT "<color=Yellow>[ChØ nam] Tho¹i Xa Phu kh«ng cã môc %s - h·y tù chän trong khung tho¹i."
#define VT_MSG_XAPHU_CHUADI      "<color=Yellow>[ChØ nam] Ch­a lªn ®­îc b¶n ®å ®Ých (thiÕu tiÒn xe?) - dõng dÉn ®­êng."
#define VT_MSG_XAPHU_KHONGBEN    "<color=Yellow>[ChØ nam] N¬i cÇn ®Õn kh«ng cã bÕn Xa Phu - h·y tù ®i bé."
#define VT_MSG_BANG_KHONGBANG    "<color=Yellow>[ChØ nam] Ng­¬i ch­a vµo bang - TiÕp DÉn ¸p Tiªu Bang chØ tiÕp bang chiÕm thµnh."
#define VT_MSG_BANG_KHONGTHANH   "<color=Yellow>[ChØ nam] Bang cña ng­¬i ch­a chiÕm thµnh nµo - ch­a cã TiÕp DÉn nµo nhËn xe."
#define VT_MSG_BH_XE             "<color=Yellow>[ChØ nam] §ang gi÷ xe bang - xe ph¶i ®i bé; chØ dÉn ®­êng tíi NhËn Hµng khi ng­¬i ®ang ë ®óng b¶n ®å ®iÓm cuèi."
#define VT_MSG_KHONG_NPC         "<color=Yellow>[ChØ nam] Kh«ng thÊy NPC ë vÞ trÝ quy ®Þnh - cã thÓ m¸y chñ ch­a sinh NPC vËn tiªu."
#define VT_MENU_THANHTHI         "thµnh thÞ ®· ®i qua"
#define VT_NPC_CHUONGQUY         "¤ng chñ Tiªu côc Lôc Tam C©n"
#define VT_NPC_TIEUSU            "Long M«n Tiªu S­"
#define VT_NPC_TIEPDAN           "TiÕp DÉn ¸p Tiªu Bang"
#define VT_NPC_NHANHANG          "NhËn Hµng ¸p Tiªu Bang"

#endif
