// ============================================================================
// UiTaskGuideVanTieu.h - SINH TU DONG boi ReverseTools/vantieu/gen_vantieu_chinam.py
// (ten 44 diem tu extend.lua tbBJPoints, so thuong tu award.lua, bai huong dan soan trong
// chinh script sinh) - DUNG SUA TAY. Chuoi TCVN3 tho (source-charset = execution-charset).
// Dung boi UiTaskGuide.cpp (bang F11 muc Van tieu).
// ============================================================================
#ifndef UITASKGUIDE_VANTIEU_H
#define UITASKGUIDE_VANTIEU_H

static const char* const s_szVTDiemTen[44 + 1] = {
	"",
	"L©m An Nam (181,208)",
	"L©m An B¾c (191,185)",
	"Thµnh §« (382,312)",
	"Th«n Giang T©n (439,385)",
	"T­¬ng D­¬ng (192,198)",
	"§¹o H­¬ng Th«n (203,200)",
	"D­¬ng Ch©u (223,189)",
	"§¹i Lı (204,200)",
	"Th¹ch Cæ TrÊn (209,202)",
	"Ph­îng T­êng (199,193)",
	"VÜnh L¹c TrÊn (209,201)",
	"BiÖn Kinh (211,196)",
	"Chu Tiªn TrÊn (202,194)",
	"Th«n Long TuyÒn (199,204)",
	"D­¬ng Ch©u",
	"T­êng V©n §éng",
	"TiÕn Cóc ®éng",
	"Linh Cèc ®éng",
	"L©m An",
	"L­ìng Thñy §éng",
	"Long Cung §éng",
	"NghiÖt Long §éng",
	"BiÖn Kinh",
	"Thiªn T©m ®éng",
	"Thiªn NhÉn Gi¸o",
	"Phi Thiªn ®éng",
	"ThiÕu L©m ph¸i",
	"Thµnh §«",
	"Nh¹n Th¹ch ®éng",
	"ThÇn Tiªn ®éng",
	"B¹ch V©n ®éng",
	"§éng Kinh Hoµng",
	"T­¬ng D­¬ng",
	"Kª Qu¸n ®éng",
	"Kháa Lang §éng",
	"D­¬ng Trung ®éng",
	"Ph­îng T­êng",
	"B¨ng Hµ ®éng",
	"Hoa S¬n",
	"§éng Kim Quang",
	"§¹i Lı",
	"Phông Nh·n §éng",
	"§iÓm Th­¬ng §éng",
	"V« Danh ®éng",
};

#define VT_CN_TIEU           "<color=yellow>Long M«n Tiªu Côc - vËn tiªu c¸ nh©n (1 ®Õn 9 sao)<color>"
#define VT_CN_DK             "§iÒu kiÖn: cÊp 90 trë lªn (hoÆc ®· chuyÓn sinh) vµ ®· gia nhËp m«n ph¸i. Giê më: 10:00 ®Õn 23:00 h»ng ngµy. Mçi ngµy nhËn tèi ®a 3 nhiÖm vô; lµm míi nhiÖm vô ngÉu nhiªn miÔn phİ 5 lÇn, hÕt l­ît th× dïng Ho¸n Tiªu ChØ."
#define VT_CN_B1             "B­íc 1 - NhËn nhiÖm vô: ®Õn <color=yellow>Long M«n trÊn<color> gÆp <color=yellow>¤ng chñ Tiªu côc Lôc Tam C©n<color> chän NhËn nhiÖm vô ¸p Tiªu. Cã 3 lo¹i: 1 sao (dÔ nhÊt); ngÉu nhiªn 2 ®Õn 9 sao (xem tuyÕn råi míi nhËn, ®­îc lµm míi); cao cÊp (nép ¸p tiªu ñy nhiÖm tr¹ng cao cÊp ®Ó nhËn 7, 8 hoÆc 9 sao theo tØ lÖ 60, 30, 10 phÇn tr¨m)."
#define VT_CN_B2             "B­íc 2 - XuÊt ph¸t: nhËn xong ng­¬i ®­îc ®­a ngay tíi <color=yellow>Long M«n Tiªu S­ <color> ë ®iÓm ®Çu tuyÕn. Nãi chuyÖn víi Tiªu S­ chän B¾t ®Çu ¸p Tiªu, Tiªu Xa xuÊt hiÖn c¹nh ng­¬i vµ b¸m theo. §ång hå 30 phót ch¹y tõ lóc nµy."
#define VT_CN_B3             "B­íc 3 - Hé tèng: ®i bé (c­ìi ngùa ®­îc) dÉn Tiªu Xa tíi Tiªu S­ ë ®iÓm cuèi tr­íc khi hÕt 30 phót. Xe chØ b¸m theo khi ng­¬i ë gÇn (kho¶ng 16 «); xa qu¸ xe ®øng l¹i, sau 5 phót kh«ng ai quay l¹i th× xe biÕn mÊt. Xe theo ®­îc qua cöa b¶n ®å nh­ng kh«ng theo phï vÒ thµnh, thÇn hµnh phï, vµ mÊt nÕu ng­¬i tho¸t game qu¸ 5 phót."
#define VT_CN_B3B            "Xe cã m¸u vµ bŞ ng­êi kh¸c ®¸nh ph¸. Xe vì th× r¬i <color=yellow>Tiªu Kú <color> (chØ chñ xe nhÆt ®­îc, cßn 10 phót) vµ <color=yellow>T¸n L¹c Tiªu VËt<color> (3 phót ®Çu chØ chñ xe vµ tæ ®éi nhÆt, sau ®ã ai ®ñ cÊp 90 còng nhÆt ®­îc, mçi ng­êi 5 lÇn mét ngµy). Mang Tiªu Kú vÒ Tiªu S­ vÉn nhËn mét phÇn th­ëng; mÊt c¶ xe lÉn kú th× chän KÕt thóc nhiÖm vô ®Ó nhËn 100 v¹n kinh nghiÖm."
#define VT_CN_B4             "B­íc 4 - Tr¶ hµng: ®­a xe tíi s¸t <color=yellow>Tiªu S­ ®iÓm cuèi<color> råi chän NhËn phÇn th­ëng nhiÖm vô ¸p Tiªu. Th­ëng bèc theo cÊp sao: kinh nghiÖm hoÆc vËt phÈm quı (B¸ch Niªn Huy Hoµng qu¶, Qu¶ Huy Hoµng, HuyÒn Kim Kho¸ng Th¹ch, Hé M¹ch §¬n, Ch©n Nguyªn §an, B¹o Né LÖnh Phï, Ngò Hµnh Nguyªn Th¹ch, M¶nh c«ng hu©n ch­¬ng Tèng Kim, NhÊt Kû Cµn Kh«n Phï hiÕm), kÌm 2 <color=yellow>Hé Tiªu LÖnh<color>."
#define VT_CN_THUONG         "Sè lÇn bèc th­ëng khi giao xe: 1 sao 1 lÇn, 2 sao 5 lÇn, 3 sao 5 lÇn, 4 sao 7 lÇn, 5 sao 9 lÇn, 6 sao 10 lÇn, 7 sao 10 lÇn, 8 sao 20 lÇn, 9 sao 30 lÇn. Mçi lÇn bèc tróng kinh nghiÖm ®­îc 1288 v¹n (1 sao), 1288 v¹n (2 sao), 2188 v¹n (3 sao), 2888 v¹n (4 sao), 4888 v¹n (5 sao), 4888 v¹n (6 sao), 5888 v¹n (7 sao), 6888 v¹n (8 sao), 8888 v¹n (9 sao); bèc tróng vËt phÈm th× nhËn vËt phÈm."
#define VT_CN_SHOP           "Cöa hµng Tiªu côc (ë ¤ng chñ vµ Tiªu S­) ®æi Hé Tiªu LÖnh lÊy ®¹o cô dïng khi ë gÇn xe: Kho¸i M· Gia Tiªn (t¨ng tèc xe 15 gi©y), Kiªn BÊt Kh¶ Táa (håi 10 phÇn tr¨m m¸u xe), Tiªu Xa Di VŞ (kĞo xe vÒ chç m×nh ®øng); mçi mãn chê 3 phót míi dïng l¹i. L¹c xe th× gÆp ¤ng chñ hoÆc Tiªu S­ chän TruyÒn tèng ®Õn vŞ trİ cña Tiªu Xa (1 v¹n l­îng, c¸ch nhau 30 gi©y)."
#define VT_CN_LUUY           "Tõ bá nhiÖm vô chØ ®­îc khi xe ch­a xuÊt ph¸t vµ nhiÖm vô d­íi 7 sao. §ang nhËn vËn tiªu c¸ nh©n th× kh«ng nhËn ®­îc ¸p Tiªu Bang vµ ng­îc l¹i."
#define VT_CN_TT_NONE        "Tr¹ng th¸i: <color=yellow>ch­a nhËn nhiÖm vô <color>."
#define VT_CN_TT_FMT         "Tr¹ng th¸i: ®ang nhËn nhiÖm vô <color=yellow>%d sao<color>, tuyÕn <color=yellow>%s<color> tíi <color=yellow>%s<color>, xe %s."
#define VT_CN_XE_CHUA        "ch­a xuÊt ph¸t"
#define VT_CN_XE_CON_FMT     "®· xuÊt ph¸t, cßn kho¶ng %d phót"
#define VT_CN_XE_HET         "®· xuÊt ph¸t vµ ®· hÕt giê"
#define VT_CN_NGAY_FMT       "H«m nay: ®· nhËn <color=yellow>%d/3<color> nhiÖm vô, lµm míi miÔn phİ <color=yellow>%d/5<color>."
#define VT_CN_HINT           "<color=Cyan>NhÊp vµo dßng nhiÖm vô ®Ó tù ®i: ch­a nhËn th× tíi ¤ng chñ Tiªu côc ë Long M«n trÊn, ®· nhËn th× tíi Tiªu S­ ®iÓm ®Çu (®ang ë thµnh hay th«n th× ch¹y ra Xa Phu ®i xe, ®ang ë map luyÖn c«ng th× dïng phï vÒ thµnh tr­íc); xe ®· xuÊt ph¸t th× tíi Tiªu S­ ®iÓm cuèi nÕu cïng b¶n ®å. Kh«ng dÉn ®­êng khi ®ang ë map sù kiÖn.<color>"
#define VT_CN_TRACE_TITLE    "<color=yellow>VËn tiªu c¸ nh©n<color>"
#define VT_CN_BRIEF_NONE     "Ch­a nhËn nhiÖm vô ¸p Tiªu."
#define VT_CN_BRIEF_FMT      "%d sao: %s tíi %s (%s)"
#define VT_BH_TIEU           "<color=yellow>Long M«n Tiªu Côc - ¸p Tiªu Bang (10 sao)<color>"
#define VT_BH_DK             "Thêi gian: <color=yellow>thø b¶y vµ chñ nhËt, 12:00 ®Õn 23:00<color>. §iÒu kiÖn bang: ®ang chiÕm gi÷ thµnh sau C«ng thµnh chiÕn; sè l­ît trong tuÇn b»ng (sè thµnh chiÕm trong tuÇn + 1) chia 2. Ng­êi nhËn xe: bang chñ hoÆc tr­ëng l·o, cÊp 90 trë lªn (hoÆc chuyÓn sinh), vµo bang ®ñ 7 ngµy, kh«ng ®ang lµm vËn tiªu c¸ nh©n."
#define VT_BH_B1             "B­íc 1 - Bang chñ më: gÆp <color=yellow>TiÕp DÉn ¸p Tiªu Bang<color> t¹i thµnh bang ®ang chiÕm (cã ë D­¬ng Ch©u, L©m An, BiÖn Kinh, Thµnh §«, T­¬ng D­¬ng, Ph­îng T­êng, §¹i Lı) chän Më ¸p Tiªu Bang. Mçi tuÇn më mét lÇn."
#define VT_BH_B2             "B­íc 2 - NhËn xe: bang chñ hoÆc tr­ëng l·o gÆp TiÕp DÉn chän NhËn ¸p Tiªu Bang. <color=yellow>Tiªu Xa Bang 10 sao<color> xuÊt hiÖn c¹nh ng­êi nhËn, loa toµn m¸y chñ b¸o bang, ng­êi nhËn vµ ®iÓm ®Çu. C¶ bang chØ gi÷ mét xe mét lóc, lÇn nhËn tiÕp theo c¸ch 30 phót."
#define VT_BH_B3             "B­íc 3 - C¶ bang hé tèng: ng­êi nhËn dÉn xe ®i bé tíi <color=yellow>NhËn Hµng ¸p Tiªu Bang<color> ë hang ®éng ®iÓm cuèi (tuyÕn chän ngÉu nhiªn trong 3 ®Õn 4 hang quanh thµnh) trong 30 phót. Thµnh viªn bang ®øng gÇn xe (trong 25 «) ë tr¹ng th¸i chiÕn ®Êu mçi phót nhËn <color=yellow>400 v¹n kinh nghiÖm<color> vµ mét ®iÓm b¸m xe."
#define VT_BH_B3B            "Xe bŞ bang kh¸c ph¸ th× r¬i <color=yellow>R­¬ng Tiªu VËt Bang<color>: 3 phót ®Çu chØ bang ph¸ xe vµ bang chñ xe nhÆt ®­îc, sau ®ã bang nµo còng nhÆt ®­îc (cÇn vµo bang 7 ngµy). R­¬ng nép cho bang chñ t¹i TiÕp DÉn ®Ó bang chñ ®æi th­ëng nhãm cho bang."
#define VT_BH_B4             "B­íc 4 - Giao xe: ®­a xe tíi s¸t NhËn Hµng råi ng­êi nhËn chän Giao Tiªu Xa Bang: nhËn ngay <color=yellow>8 R­¬ng Tiªu VËt Bang<color> (h¹n dïng ®Õn hÕt tuÇn). Sau khi chuyÕn kÕt thóc, thµnh viªn ®· b¸m xe gÆp NhËn Hµng chän NhËn phÇn th­ëng hé tiªu c¸ nh©n: d­íi 5 ®iÓm nhËn CÈm nang thay ®æi trêi ®Êt, 5 ®Õn 11 ®iÓm Th­ëng TËn Trung, 12 ®iÓm trë lªn Th­ëng Trung Thµnh (më ra kinh nghiÖm, Hé M¹ch §¬n, Ch©n Nguyªn §¬n, HuyÕt Long §»ng, Tói D­îc PhÈm...)."
#define VT_BH_LUUY           "T¹i TiÕp DÉn cßn cã Xem th«ng tin Tiªu Xa, TruyÒn tèng ®Õn Tiªu Xa (chê 20 gi©y, t¨ng dÇn tíi 60) vµ Tõ bá. Ng­êi ®ang gi÷ xe kh«ng rêi bang hoÆc bŞ khai trõ ®­îc; bang ®· më ¸p tiªu trong tuÇn th× kh«ng ®æi bang chñ."
#define VT_BH_TT_NONE        "Tr¹ng th¸i: <color=yellow>ng­¬i ch­a gi÷ xe bang<color>."
#define VT_BH_TT_FMT         "Tr¹ng th¸i: ®ang gi÷ Tiªu Xa Bang tuyÕn <color=yellow>%s<color> tíi <color=yellow>%s<color>, %s."
#define VT_BH_DIEM_FMT       "§iÓm b¸m xe tuÇn nµy (4 l­ît): %d, %d, %d, %d."
#define VT_BH_HINT           "<color=Cyan>NhÊp vµo dßng nhiÖm vô ®Ó tù ®i tíi TiÕp DÉn ¸p Tiªu Bang ë thµnh bang ®ang chiÕm (®ang ë thµnh hay th«n th× ch¹y ra Xa Phu ®i xe, ®ang ë map luyÖn c«ng th× dïng phï vÒ thµnh tr­íc); ®ang gi÷ xe th× ®i tíi NhËn Hµng ®iÓm cuèi nÕu cïng b¶n ®å. Kh«ng dÉn ®­êng khi ®ang ë map sù kiÖn.<color>"
#define VT_BH_TRACE_TITLE    "<color=yellow>¸p Tiªu Bang<color>"
#define VT_BH_BRIEF_NONE     "Ch­a gi÷ xe bang."
#define VT_BH_BRIEF_FMT      "%s tíi %s (%s)"

#endif
