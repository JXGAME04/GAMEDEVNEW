// ============================================================================
// KMapSuKien.h - SINH TU DONG tu settings/map_type.txt cua may chu DANG CHAY
// boi ReverseTools/gen_map_sukien.py - DUNG SUA TAY.
//
// Bang nay tra loi mot cau: "ban do dang dung co phai MAP SU KIEN khong?".
// Auto WAuto dung no de DUNG cac auto TU DO khi nguoi choi dang di su kien
// (y chu game 25/08: 'da tau ma dang di tong kim - phong lang do - tin su -
// cac hoat dong bang hoi - vuot ai thi se khong chay auto da tau').
//
// nChan = 1: su kien / chien truong / pho ban -> DUNG Da Tau, dung tu di
//             chuyen, dung tu ve thanh (van danh tra + nhat do binh thuong).
// nChan = 0: ban do dac biet nhung VAN la cho cay -> khong chan gi.
//
// Da doi chieu luc sinh: 0 map su kien nao trung diem cay Da Tau
// (KDaTauSpots.h) hay map boss Sat Thu (KSatThuBossPos.h).
// ============================================================================
#ifndef KMAPSUKIEN_H
#define KMAPSUKIEN_H

struct KMapSKKhoang
{
	short		nTu;		// map dau (bao gom)
	short		nDen;		// map cuoi (bao gom)
	unsigned char	nLoai;	// chi so trong s_aTenSuKien
	unsigned char	nChan;	// 1 = dung auto tu do khi dang o day
};

static const char* const s_aTenSuKien[] = {
	"Tèng Kim (chiÕn tr­êng + ®iÓm b¸o danh)",	// 0 SONGJIN
	"Sù kiÖn Gi¸ng Sinh",	// 1 CHRISTMAS
	"Sù kiÖn N¨m Míi",	// 2 SPRING
	"Phong Háa Liªn Thµnh",	// 3 FENGHUO
	"§¹i Lao (nhµ lao)",	// 4 JIANLAO
	"DiÔn Vâ Tr­êng (l«i ®µi nhiÒu ng­êi)",	// 5 YANWUCHANG
	"L«i §µi Bang Héi",	// 6 TONGLEITAI
	"Bang Héi C«ng Thµnh ChiÕn",	// 7 TONGGONGCHENG
	"§µo Hoa §¶o",	// 8 TAOHUADAO
	"TÈy Tñy §¶o",	// 9 XISUIDAO
	"Liªn §Êu",	// 10 LEAGUEMATCH
	"Phong L¨ng §é (b·i ®¸nh b¶o)",	// 11 FENGLING
	"Phong L¨ng §é (®ß)",	// 12 FERRY
	"TÝn Sø",	// 13 XINSHI
	"§ua ThuyÒn Rång",	// 14 DRAGONBOAT
	"S¸t Thñ ThÝ LuyÖn (V­ît ¶i)",	// 15 SHILIAN
	"NhiÖm vô §ång B¹n",	// 16 TONGBAN
	"Minh NguyÖt TrÊn",	// 17 MINGYUE
	"Vâ L©m §Ö NhÊt Bang (Bang ChiÕn)",	// 18 DIYIBANG
	"Vâ L©m §¹i Héi (khu vùc)",	// 19 QUFUYONGDI
	"Vâ L©m §¹i Héi (kh¸n ®µi)",	// 20 GUANZHANCHANG
	"Vâ L©m §¹i Héi",	// 21 WULINDAHUI
	"Sù kiÖn §oan Ngä",	// 22 DUANWUJIE
	"CÊm §Þa Tr­êng Ca M«n",	// 23 JINDI
	"L«i §µi Tinh Anh Bang Héi",	// 24 TOPMANMATCH
	"Viªm §Õ B¶o Tµng",	// 25 YANDIBAOZANG
	"H¸i thuèc (nghÒ)",	// 26 CAIYAO
	"§ua ch¹y",	// 27 RACEGAME
	"Tèng Kim (cÊm thuèc PK)",	// 28 SONGJIN_NONE
	"Liªn §Êu (®¹i ®å s¸t)",	// 29 DATUSHA
	"B¸ch Nh©n L«i §µi",	// 30 BAIRENLEITAI
	"Quèc ChiÕn Tèng Kim",	// 31 GUOZHAN
	"Phã b¶n",	// 32 FUBEN
	"ThÊt Thµnh §¹i ChiÕn",	// 33 SEVENCITY
};

static const KMapSKKhoang s_aMapSuKien[] = {
	{ 44, 44, 0, 1 }, { 323, 331, 0, 1 }, { 344, 386, 0, 1 }, { 605, 605, 0, 1 }, { 606, 606, 0, 1 }, { 607, 607, 0, 1 },
		// SONGJIN = Tèng Kim (chiÕn tr­êng + ®iÓm b¸o danh)
	{ 582, 582, 1, 1 }, { 583, 583, 1, 1 }, { 821, 821, 1, 1 }, { 822, 822, 1, 1 },
		// CHRISTMAS = Sù kiÖn Gi¸ng Sinh
	{ 584, 584, 2, 1 }, { 585, 585, 2, 1 }, { 823, 823, 2, 1 }, { 824, 824, 2, 1 }, { 849, 849, 2, 1 }, { 850, 850, 2, 1 }, { 845, 845, 2, 1 }, { 846, 846, 2, 1 }, { 892, 892, 2, 1 }, { 893, 893, 2, 1 }, { 894, 894, 2, 1 }, { 895, 895, 2, 1 }, { 901, 901, 2, 1 }, { 926, 926, 2, 1 },
		// SPRING = Sù kiÖn N¨m Míi
	{ 516, 516, 3, 1 }, { 517, 517, 3, 1 }, { 518, 518, 3, 1 }, { 519, 519, 3, 1 }, { 580, 580, 3, 1 }, { 581, 581, 3, 1 },
		// FENGHUO = Phong Háa Liªn Thµnh
	{ 208, 208, 4, 1 },
		// JIANLAO = §¹i Lao (nhµ lao)
	{ 209, 209, 5, 1 }, { 210, 210, 5, 1 }, { 211, 211, 5, 1 },
		// YANWUCHANG = DiÔn Vâ Tr­êng (l«i ®µi nhiÒu ng­êi)
	{ 212, 220, 6, 1 },
		// TONGLEITAI = L«i §µi Bang Héi
	{ 221, 221, 7, 1 }, { 222, 222, 7, 1 }, { 223, 223, 7, 1 },
		// TONGGONGCHENG = Bang Héi C«ng Thµnh ChiÕn
	{ 235, 241, 8, 0 },
		// TAOHUADAO = §µo Hoa §¶o
	{ 242, 318, 9, 0 },
		// XISUIDAO = TÈy Tñy §¶o
	{ 334, 334, 10, 1 }, { 335, 335, 10, 1 }, { 396, 415, 10, 1 }, { 527, 538, 10, 1 }, { 540, 579, 10, 1 },
		// LEAGUEMATCH = Liªn §Êu
	{ 336, 336, 11, 1 }, { 341, 341, 11, 1 }, { 342, 342, 11, 1 },
		// FENGLING = Phong L¨ng §é (b·i ®¸nh b¶o)
	{ 337, 337, 12, 1 }, { 338, 338, 12, 1 }, { 339, 339, 12, 1 },
		// FERRY = Phong L¨ng §é (®ß)
	{ 387, 395, 13, 1 },
		// XINSHI = TÝn Sø
	{ 416, 463, 14, 1 },
		// DRAGONBOAT = §ua ThuyÒn Rång
	{ 461, 511, 15, 1 },
		// SHILIAN = S¸t Thñ ThÝ LuyÖn (V­ît ¶i)
	{ 513, 515, 16, 1 },
		// TONGBAN = NhiÖm vô §ång B¹n
	{ 520, 526, 17, 0 },
		// MINGYUE = Minh NguyÖt TrÊn
	{ 608, 613, 18, 1 },
		// DIYIBANG = Vâ L©m §Ö NhÊt Bang (Bang ChiÕn)
	{ 614, 650, 19, 1 },
		// QUFUYONGDI = Vâ L©m §¹i Héi (khu vùc)
	{ 651, 660, 20, 1 },
		// GUANZHANCHANG = Vâ L©m §¹i Héi (kh¸n ®µi)
	{ 661, 820, 21, 1 },
		// WULINDAHUI = Vâ L©m §¹i Héi
	{ 825, 825, 22, 1 }, { 826, 826, 22, 1 }, { 827, 827, 22, 1 }, { 828, 828, 22, 1 },
		// DUANWUJIE = Sù kiÖn §oan Ngä
	{ 539, 539, 23, 1 }, { 829, 834, 23, 1 },
		// JINDI = CÊm §Þa Tr­êng Ca M«n
	{ 841, 848, 24, 1 },
		// TOPMANMATCH = L«i §µi Tinh Anh Bang Héi
	{ 851, 862, 25, 1 },
		// YANDIBAOZANG = Viªm §Õ B¶o Tµng
	{ 871, 874, 26, 0 }, { 892, 892, 26, 0 },
		// CAIYAO = H¸i thuèc (nghÒ)
	{ 863, 863, 27, 1 },
		// RACEGAME = §ua ch¹y
	{ 868, 870, 28, 1 }, { 876, 878, 28, 1 }, { 886, 891, 28, 1 }, { 970, 971, 28, 1 },
		// SONGJIN_NONE = Tèng Kim (cÊm thuèc PK)
	{ 896, 896, 29, 1 },
		// DATUSHA = Liªn §Êu (®¹i ®å s¸t)
	{ 960, 960, 30, 1 },
		// BAIRENLEITAI = B¸ch Nh©n L«i §µi
	{ 898, 898, 31, 1 }, { 899, 899, 31, 1 }, { 900, 900, 31, 1 },
		// GUOZHAN = Quèc ChiÕn Tèng Kim
	{ 905, 905, 32, 1 }, { 925, 925, 32, 1 },
		// FUBEN = Phã b¶n
	{ 927, 933, 33, 1 },
		// SEVENCITY = ThÊt Thµnh §¹i ChiÕn
};
#define KMAPSK_SO	(int)(sizeof(s_aMapSuKien) / sizeof(s_aMapSuKien[0]))

// Tra chi so LOAI su kien cua ban do (dung cho s_aTenSuKien), -1 neu khong phai.
// pbChan (co the NULL) nhan 1 khi ban do nay phai DUNG cac auto tu do.
inline int KMapSK_Loai(int nMap, int* pbChan)
{
	for (int i = 0; i < KMAPSK_SO; ++i)
	{
		if (nMap >= s_aMapSuKien[i].nTu && nMap <= s_aMapSuKien[i].nDen)
		{
			if (pbChan)
				*pbChan = s_aMapSuKien[i].nChan;
			return s_aMapSuKien[i].nLoai;
		}
	}
	if (pbChan)
		*pbChan = 0;
	return -1;
}

#endif
