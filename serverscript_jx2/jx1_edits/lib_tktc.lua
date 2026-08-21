-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Function: Lib Tèng Kim

Include("\\script\\lib\\lib_server.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\log_game\\save_log.lua")
NOW_END_SAY = "KÕt thóc ®èi tho¹i./no"
PT_CuuSat 												= 1			--Khai b¸o Id c¸c thÓ thøc tèng kim
PT_BaoVeNguyenSoai								= 2
PT_DoatCo 												= 3

Pos_Tren													= 1
Pos_Duoi													= 2

LEVEL_ENOUGH_TK 							= 80		--®¼ng cÊp tèi thiÓu khi tham gia
MAX_PLAYER_CL 								= 1			--chªnh lÖch qu©n sè 2 bªn mÆc ®Þnh lµ 5 ng­êi
NEEDMONEY 										= 20000--tiÒn b¸o danh tham gia tèng kim
TICH_LUY_THUONG_TK_W				= 1200	--®iÓm th­ëng tÝch luü céng thªm cho phe th¾ng
TICH_LUY_THUONG_TK_L 				= 300		--®iÓm th­ëng tÝch luü céng thªm cho phe thua
TICH_LUY_THUONG_TK_H 				= 600		--®iÓm th­ëng tÝch luü céng thªm hai phe hoµ nhau
MIN_POINT_AWARD							= 1000	--®iÓm tÝch luü tèi thiÓu ®Ó kÕt thóc trËn nhËn ®­îc quµ
MAX_CAMCO											=	30		--sè cê tèi ®a hai bªn cã thÓ c¾m
FLAG_POINT 											= 300 		-- ®iÓm sè tÝch luü trong tèng kim khi c¾m cê
KILL_PLAYER_POINT							= 30

ITEM_BV_POINT									= {
	{206, 200},
	{207, 220},
	{208, 240},
	{209, 260},
	{210, 300},
}

DROPRATETONGKIM 							= "\\script\\tinhnang\\tong_kim_tcap\\drop.lua"
DOSCRIPTCO 											= "\\script\\tinhnang\\tong_kim_tcap\\cotk.lua"
ONDEATHQUANTONG 						= "\\script\\tinhnang\\tong_kim_tcap\\tongtu.lua"
ONDEATHQUANKIM 							= "\\script\\tinhnang\\tong_kim_tcap\\kimtu.lua"
DOSCRIPTMOBINH 								= "\\script\\tinhnang\\tong_kim_tcap\\mobinhtk.lua"
DOSCRIPTQUANQUAN 						= "\\script\\tinhnang\\tong_kim_tcap\\quanquan.lua"
DOSCRIPTXAPHU 									= "\\script\\tinhnang\\tong_kim_tcap\\xaphu.lua"
ONDEATHQUAITK 								= "\\script\\tinhnang\\tong_kim_tcap\\binhsi.lua"
ONDEATHNSOAITK 								= "\\script\\tinhnang\\tong_kim_tcap\\nguyensoai.lua"
DOSCRIPTQUANY 									= "\\script\\tinhnang\\tong_kim_tcap\\quany.lua"
DOSCRIPTRUONG 									= "\\script\\tinhnang\\tong_kim_tcap\\ruongchua.lua"
DOSCRIPTTRINHSAT 							= "\\script\\tinhnang\\tong_kim_tcap\\trinhsat.lua"
DOSCRIPTCOSOAITONG						= "\\script\\tinhnang\\tong_kim_tcap\\cosoaitong.lua"
DOSCRIPTCOSOAIKIM							= "\\script\\tinhnang\\tong_kim_tcap\\cosoaikim.lua"

MAP_BD_TC											= 324	--b¶n ®å b¸o danh tèng kim trung cÊp
MAP_TK_TC											= 379	--b¶n ®å tèng kim trung cÊp
PLAYER_MS_LIMIT								= 2000 --55 ng­êi mçi phe
TIME_BD_TK											= 1 --10 phut b¸o danh
TIME_KT_TK											= 45 -- 70 phut chiÕn ®Êu gåm 10 phót b¸o danh
TIME_NS_TK											= 1 -- 30 phut thêi gian xuÊt hiÖn nguyªn so¸i
TIME_XOANPC_TK								= 1 -- 1 phut xoa npc 
ID_ShopQuanYTongKim 						= 29
ID_LUU_RUONG_TONG						= 42
ID_LUU_RUONG_KIM							= 43
ITEM_CO_HIEU										= {6,1,158}	--Item cê hiÖu ®Ó tham gia thÓ thøc ®o¹t cê
RUN_SPEDD_DOW_ONFLAG				= 60 --gi¶m tèc ®é khi gi÷ cê
TIME_IN_TRAI										= 90 --thêi gian tèi ®a ë trong doanh tr¹i
TIME_DELAY_RA_TRAI						= 10 --thêi gian delay khi ra tr¹i
LIFE_NGUYEN_SOAI							= 5000000*STRONGBOSS_NSTK --m¸u boss nguyªn so¸i
TK_VARV_KEY = 1

TAB_TIME_TONG_KIM = {
	{13,13,13,1},
	-- {20,50,21,2},
	{17,50,18,3},
	{20,50,21,4},
	-- {17,50,18,5},
	-- {11,50,12,4},
	-- {19,50,20,6},
	-- {14,50,15,6},
	-- {15,50,16,7},
	-- {16,50,17,8},
	{22,50,23,5},
}


function sj_checklastbattle()
	local n_key = GetTask(T_CHECKDATETK)
	print(format("debug by tuanln sj_checklastbattle %d %d %s", n_key, GetGlbMissionVC(TK_VARV_KEY), GetName()));
	if (n_key == GetGlbMissionVC(TK_VARV_KEY)) then
		return  1
		else
		return 0
	end
	end
	
	
TAB_PHE_TONGKIM = {
	--	1	2      3   4              5                       6             7        8             9            10  11 -- ch­a sö dông  9 10 11
	{1,"qu©n Tèng",0, M_TICHLUYA, {MAP_TK_TC,1242*32,3549*32}, {MAP_BD_TC,1543,3177}, 628, M_SOCOPHEA, {MAP_TK_TC,1229,3561}, 42, 1},
	{2,"qu©n Kim" ,6, M_TICHLUYB, {MAP_TK_TC,1688*32,3072*32}, {MAP_BD_TC,1576,3087}, 627, M_SOCOPHEB, {MAP_TK_TC,1689,3074}, 43, 2},
}

TAB_PHUONGTHUC = {
	{1,"Cõu S¸t"},
	{2,"B¶o VÖ Nguyªn So¸i"},
	{3,"§o¹t Cê"}
}

QUANHAMTK = {
	-- 1              2             3                                                                                         4  5
	{"Binh sÜ "			,0			," "																																										,100,"<color=white>"	},
	{"HiÖu óy "		,1000	,"nhËn hç trî <color=yellow>30%<color> sinh lùc tèi ®a, <color=yellow>5%<color> kh¸ng tÊt c¶ "	,120,"<color=blue>"		},
	{"Thèng lÜnh "	,3000	,"nhËn hç trî <color=yellow>40%<color> sinh lùc tèi ®a, <color=yellow>10%<color> kh¸ng tÊt c¶ "	,150,"<color=purple>"	},
	{"Phã t­íng "	,7000	,"nhËn hç trî <color=yellow>50%<color> sinh lùc tèi ®a, <color=yellow>15%<color> kh¸ng tÊt c¶ "	,180,"<color=cyan>"		},
	{"§¹i t­íng "	, 10000	,"nhËn hç trî <color=yellow>60%<color> sinh lùc tèi ®a, <color=yellow>20%<color> kh¸ng tÊt c¶ "	,200,"<color=green>"		},
	{"§¹i so¸i "	, 35000	,"nhËn hç trî <color=yellow>70%<color> sinh lùc tèi ®a, <color=yellow>25%<color> kh¸ng tÊt c¶ "	,220,"<color=metal>"		},
}


TAB_QUANHAM = { --point phong qu©n hµm khi ®¹t ®ñ ®iÓm
	{0		,1000	,100},
	{1000	,3000	,200},		
	{3000	,7000	,400},
	{7000	,10000	,600},
	{10000	,35000	,800},
	{35000	,1000000	,1000},
}

TICHLUYTK = { --point tÝch luü ®¸nh npc gi¶m 1 nöa
	5,
	100,
	200,
	400,
	1000,
}


TICHLUY_GIETSOAI = 5000

COTONG_TOADO = {
	{1313, 3433},
	{1338, 3420},
	{1348, 3441},
	{1338, 3463},
	{1320, 3459},
	{1330, 3443},
}
		
COKIM_TOADO = {
	{1557, 3178},
	{1548, 3207},
	{1563, 3223},
	{1583, 3222},
	{1595, 3207},
	{1589, 3179},
	{1572, 3195}
}

POST_DES_CAMCO = {
	{1558,3206},		--vÞ trÝ phe tèng c¾m cê ®µi so¸i phe kim
	{1332,3443}		--vÞ trÝ phe kim c¾m cê ®µi so¸i phe tèng
}

TONGBINH_TOADO = {
	{1368, 3391},
	{1369, 3387},
	{1370, 3383},
	{1376, 3388},
	{1381, 3384},
	{1383, 3374},
	{1383, 3367},
	{1391, 3365},
	{1396, 3363},
	{1394, 3355},
	{1396, 3348},
	{1405, 3346},
	{1403, 3357},
	{1406, 3363},
	{1414, 3365},
	{1422, 3372},
	{1426, 3379},
	{1418, 3384},
	{1414, 3379},
	{1412, 3396},
	{1419, 3403},
	{1421, 3412},
	{1416, 3420},
	{1409, 3415},
	{1403, 3420},
	{1400, 3426},
	{1403, 3433},
	{1400, 3442},
	{1397, 3450},
	{1398, 3457},
	{1391, 3462},
	{1383, 3459},
	{1377, 3462},
	{1381, 3472},
	{1381, 3480},
	{1376, 3483},
	{1367, 3478},
	{1366, 3478},
	{1360, 3487},
	{1359, 3492},
	{1351, 3497},
	{1344, 3499},
	{1346, 3494},
	{1351, 3487},
	{1354, 3482},
	{1300, 3410},
	{1303, 3402},
	{1308, 3396},
	{1306, 3388},
	{1311, 3383},
	{1317, 3380},
	{1319, 3371},
	{1320, 3363},
	{1329, 3361},
	{1338, 3357},
	{1344, 3352},
	{1352, 3349},
	{1348, 3340},
	{1335, 3350},
	{1352, 3340},
	{1359, 3340},
	{1360, 3329},
	{1368, 3328},
	{1374, 3331},
	{1379, 3326},
	{1385, 3320},
	{1394, 3319},
	{1404, 3318},
	{1411, 3327},
	{1407, 3336},
	{1401, 3339},
	{1393, 3332},
	{1385, 3333},
	{1388, 3350},
	{1392, 3356},
	{1400, 3355},
	{1407, 3361},
	{1403, 3366},
}

KIMBINH_TOADO = {
	{1434, 3331},
	{1436, 3320},
	{1437, 3303},
	{1444, 3302},
	{1447, 3314},
	{1444, 3326},
	{1441, 3335},
	{1441, 3344},
	{1444, 3353},
	{1451, 3354},
	{1453, 3345},
	{1452, 3336},
	{1458, 3333},
	{1463, 3338},
	{1471, 3344},
	{1477, 3342},
	{1475, 3332},
	{1470, 3323},
	{1464, 3318},
	{1461, 3308},
	{1459, 3300},
	{1451, 3291},
	{1450, 3281},
	{1453, 3271},
	{1458, 3269},
	{1466, 3273},
	{1476, 3275},
	{1480, 3281},
	{1478, 3290},
	{1477, 3297},
	{1482, 3299},
	{1491, 3297},
	{1494, 3306},
	{1491, 3314},
	{1485, 3323},
	{1483, 3332},
	{1485, 3339},
	{1493, 3340},
	{1497, 3333},
	{1497, 3323},
	{1499, 3312},
	{1506, 3304},
	{1514, 3306},
	{1519, 3313},
	{1517, 3323},
	{1512, 3331},
	{1510, 3336},
	{1518, 3334},
	{1523, 3328},
	{1525, 3318},
	{1530, 3311},
	{1534, 3320},
	{1539, 3324},
	{1542, 3316},
	{1544, 3310},
	{1549, 3317},
	{1552, 3321},
	{1557, 3318},
	{1558, 3309},
	{1559, 3303},
	{1563, 3309},
	{1568, 3312},
	{1571, 3306},
	{1571, 3296},
	{1578, 3298},
	{1581, 3304},
	{1587, 3300},
	{1590, 3295},
	{1588, 3288},
	{1585, 3282},
	{1587, 3275},
	{1592, 3272},
	{1598, 3277},
	{1602, 3282},
	{1607, 3279},
	{1607, 3270},
	{1606, 3261},
	{1607, 3254},
	{1613, 3253},
	{1619, 3253},
	{1620, 3249},
	{1617, 3241},
	{1614, 3236},
	{1606, 3243},
	{1609, 3248},
	{1609, 3239},
	{1539, 3165},
	{1535, 3157},
	{1539, 3152},
	{1540, 3144},
	{1535, 3140},
	{1530, 3145},
	{1527, 3153},
	{1523, 3158},
	{1518, 3159},
	{1516, 3154},
	{1518, 3146},
	{1512, 3142},
	{1507, 3147},
	{1507, 3154},
	{1509, 3161},
	{1509, 3169},
	{1505, 3172},
	{1500, 3169},
	{1496, 3177},
	{1496, 3186},
	{1492, 3190},
	{1487, 3185},
	{1482, 3190},
	{1485, 3197},
	{1488, 3201},
	{1487, 3207},
	{1482, 3207},
	{1477, 3202},
	{1472, 3204},
	{1469, 3213},
	{1474, 3217},
	{1477, 3223},
	{1474, 3229},
	{1469, 3228},
	{1462, 3229},
	{1461, 3238},
	{1465, 3245},
	{1470, 3252},
	{1472, 3257},
	{1477, 3260},
	{1485, 3265},
	{1490, 3272},
	{1493, 3278},
	{1497, 3283},
	{1501, 3279},
	{1502, 3271},
	{1505, 3265},
	{1511, 3265},
	{1513, 3261},
	{1514, 3256},
	{1521, 3257},
	{1524, 3257},
	{1523, 3249},
}

TONGHIEUUY_TOADO ={
	{1403, 3355},
	{1417, 3384},
	{1397, 3431},
	{1374, 3474},
	{1343, 3467},
	{1347, 3431},
	{1370, 3387}
}

TONGTLINH_TOADO ={
	{1351, 3420},
	{1314, 3433},
	{1351, 3445}
}

KIMHIEUUY_TOADO ={
	{1539, 3208},
	{1551, 3184},
	{1579, 3174},
	{1585, 3217},
	{1584, 3292},
	{1533, 3320},
	{1476, 3217}
}

KIMTLINH_TOADO ={
	{1554, 3177},
	{1580, 3180},
	{1568, 3198}
}

KIMPTUONG_TOADO = {
	{1559, 3180},
	{1580, 3185},
}

KIMDTUONG_TOADO = {
	{1570, 3198},
}

TONGPTUONG_TOADO = {
	{1345, 3430},
	{1340, 3456}
}

TONGDTUONG_TOADO = {
	{1331, 3444}
}


function RandPThucTongKim2()
	local nRandomPT = random(1,1000)
	if(nRandomPT < 300) then
		return PT_CuuSat
	elseif(nRandomPT >= 300 and nRandomPT <= 600) then
		return PT_BaoVeNguyenSoai
	else
		return PT_BaoVeNguyenSoai
	end
end

function RandPThucTongKim1()
	return PT_BaoVeNguyenSoai
end

function RandPThucTongKim()
	local nYr,nMo,nDy,nHr,nMi,nSe,nDyfW = GetTimeNow()	
	if(nHr == 22) then
		return PT_CuuSat
	else
		return PT_BaoVeNguyenSoai
	end
end

function RandPosTK() --NgÉu nhiªn vÞ trÝ 2 phe Tèng vµ Kim tr¶ vÒ 1 lµ bªn trªn 2 lµ bªn d­íi
	local nRand = random(1, 1000)
	if(nRand < 500) then
		return Pos_Tren
	else
		return Pos_Duoi
	end
end





function addnpcquaitktrungcap()
	local nViTri = GetMissionV(M_VITRI_TRENDUOI)
	local TONGBINH_POS = nil
	local KIMBINH_POS = nil
	local TONGHIEUUY_POS = nil
	local KIMHIEUUY_POS = nil
	local TONGTLINH_POS = nil
	local KIMTLINH_POS = nil
	local TONGPTUONG_POS = nil
	local KIMPTUONG_POS = nil
	local TONGDTUONG_POS = nil	
	local KIMDTUONG_POS = nil
	if(nViTri == 1) then	--ho¸n ®æi to¹ ®é trªn d­íi 2 phe
		TONGBINH_POS = TONGBINH_TOADO
		KIMBINH_POS = KIMBINH_TOADO
		TONGHIEUUY_POS = TONGHIEUUY_TOADO
		KIMHIEUUY_POS = KIMHIEUUY_TOADO		
		TONGTLINH_POS = TONGTLINH_TOADO
		KIMTLINH_POS = KIMTLINH_TOADO
		TONGPTUONG_POS = TONGPTUONG_TOADO
		KIMPTUONG_POS = KIMPTUONG_TOADO
		TONGDTUONG_POS = TONGDTUONG_TOADO
		KIMDTUONG_POS = KIMDTUONG_TOADO
	else
		TONGBINH_POS = KIMBINH_TOADO
		KIMBINH_POS = TONGBINH_TOADO
		TONGHIEUUY_POS = KIMHIEUUY_TOADO
		KIMHIEUUY_POS = TONGHIEUUY_TOADO		
		TONGTLINH_POS = KIMTLINH_TOADO
		KIMTLINH_POS = 	TONGTLINH_TOADO
		TONGPTUONG_POS = KIMPTUONG_TOADO
		KIMPTUONG_POS = TONGPTUONG_TOADO
		TONGDTUONG_POS = KIMDTUONG_TOADO
		KIMDTUONG_POS = TONGDTUONG_TOADO
	end
	
	local nNpcIdx
--	tong binh
	for i=1,getn(TONGBINH_POS) do
		nNpcIdx = AddNpcEx3({631},100,{0,1,2,3,4},MAP_TK_TC,TONGBINH_POS[i][1]*32,TONGBINH_POS[i][2]*32,DROPRATETONGKIM,ONDEATHQUAITK,nil,1,0,36000,nil,500,50,100,200,2320,20)
		AddMSNpc(MS_TONGKIM,nNpcIdx,1)
		SetNpcValue(nNpcIdx,1)		
	end	
	
	for i=1,getn(TONGHIEUUY_POS) do
		nNpcIdx = AddNpcEx3({632},100,{0,1,2,3,4},MAP_TK_TC,TONGHIEUUY_POS[i][1]*32,TONGHIEUUY_POS[i][2]*32,DROPRATETONGKIM,ONDEATHQUAITK,nil,1,0,70000,nil,800,100,200,300,4320,30)
		AddMSNpc(MS_TONGKIM,nNpcIdx,1)	
		SetNpcValue(nNpcIdx,2)		
	end	
	
	for i=1,getn(TONGTLINH_POS) do	
		nNpcIdx = AddNpcEx3({633},100,{0,1,2,3,4},MAP_TK_TC,TONGTLINH_POS[i][1]*32,TONGTLINH_POS[i][2]*32,DROPRATETONGKIM,ONDEATHQUAITK,nil,1,0,150000,nil,1500,180,350,500,7560,40)
		AddMSNpc(MS_TONGKIM,nNpcIdx,1)
		SetNpcValue(nNpcIdx,3)				
	end	
	
	for i=1,getn(TONGPTUONG_POS) do
		nNpcIdx = AddNpcEx3({634},100,{0,1,2,3,4},MAP_TK_TC,TONGPTUONG_POS[i][1]*32,TONGPTUONG_POS[i][2]*32,DROPRATETONGKIM,ONDEATHQUAITK,nil,1,0,250000,nil,1800,200,400,580,7560,50)
		AddMSNpc(MS_TONGKIM,nNpcIdx,1)
		SetNpcValue(nNpcIdx,4)			
	end	
	
	for i=1,getn(TONGDTUONG_POS) do	
		nNpcIdx = AddNpcEx3({635},100,{0,1,2,3,4},MAP_TK_TC,TONGDTUONG_POS[i][1]*32,TONGDTUONG_POS[i][2]*32,DROPRATETONGKIM,ONDEATHQUAITK,nil,1,0,350000,nil,2200,230,450,670,7560,60)
		AddMSNpc(MS_TONGKIM,nNpcIdx,1)
		SetNpcValue(nNpcIdx,5)	
	end		
	
--	kim binh	
	for i=1,getn(KIMBINH_POS) do
		nNpcIdx = AddNpcEx3({637},100,{0,1,2,3,4},MAP_TK_TC,KIMBINH_POS[i][1]*32,KIMBINH_POS[i][2]*32,DROPRATETONGKIM,ONDEATHQUAITK,nil,2,0,36000,nil,500,50,100,200,2320,20)
		AddMSNpc(MS_TONGKIM,nNpcIdx,2)
		SetNpcValue(nNpcIdx,1)		
	end	
	
	for i=1,getn(KIMHIEUUY_POS) do	
		nNpcIdx = AddNpcEx3({638},100,{0,1,2,3,4},MAP_TK_TC,KIMHIEUUY_POS[i][1]*32,KIMHIEUUY_POS[i][2]*32,DROPRATETONGKIM,ONDEATHQUAITK,nil,2,0,70000,nil,800,100,200,300,4320,30)
		AddMSNpc(MS_TONGKIM,nNpcIdx,2)
		SetNpcValue(nNpcIdx,2)			
	end

	for i=1,getn(KIMTLINH_POS) do
		nNpcIdx = AddNpcEx3({639},100,{0,1,2,3,4},MAP_TK_TC,KIMTLINH_POS[i][1]*32,KIMTLINH_POS[i][2]*32,DROPRATETONGKIM,ONDEATHQUAITK,nil,2,0,150000,nil,1500,180,350,500,7560,40)
		AddMSNpc(MS_TONGKIM,nNpcIdx,2)
		SetNpcValue(nNpcIdx,3)	
	end

	for i=1,getn(KIMPTUONG_POS) do
		nNpcIdx = AddNpcEx3({640},100,{0,1,2,3,4},MAP_TK_TC,KIMPTUONG_POS[i][1]*32,KIMPTUONG_POS[i][2]*32,DROPRATETONGKIM,ONDEATHQUAITK,nil,2,0,250000,nil,1800,200,400,580,7560,50)
		AddMSNpc(MS_TONGKIM,nNpcIdx,2)
		SetNpcValue(nNpcIdx,4)	
	end
	
	for i=1,getn(KIMDTUONG_POS) do
		nNpcIdx = AddNpcEx3({641},100,{0,1,2,3,4},MAP_TK_TC,KIMDTUONG_POS[i][1]*32,KIMDTUONG_POS[i][2]*32,DROPRATETONGKIM,ONDEATHQUAITK,nil,2,0,350000,nil,2200,230,450,670,7560,60)
		AddMSNpc(MS_TONGKIM,nNpcIdx,2)
		SetNpcValue(nNpcIdx,5)	
	end	
end

TKPOS_NPC_DIALOG ={ 
	--1 STT -- 2 XQu©n Y -- 3 Y Qu©n Y -- 4 X R­¬ng -- 5 Y R­¬ng -- 6 X Trinh s¸t -- 7 Y Trinh s¸t
	{1, 1249*32, 3557*32, 1219*32, 3574*32, 1217*32,3550*32},
	{2, 1700*32, 3066*32, 1705*32, 3085*32, 1688*32,3047*32}
}

TKPOS_GO_HDOANH = {
	--1 STT -- 2 X HauDoanh -- 3 Y HauDoanh -- 4 X PhucSinh -- 5 Y PhucSinh
	{1, 1229, 3561, 1242*32, 3549*32},
	{2, 1689, 3074, 1688*32, 3072*32}
}

function addnpctongkimtrungcap(nViTriTong)

		local nNpcIdx;
		local nViTriKim = 0
	if(nViTriTong == 1) then
		nViTriKim = 2
	else
		nViTriKim = 1
	end
	
	local nNpcIdx
	--nNpcIdx = AddNpcEx1({62},1,nil,38,1549*32,3178*32,nil,DOSCRIPTMOBINH,"Tèng mé binh",6) 	SetNpcValue(nNpcIdx,1)-- tong mo binh
	--nNpcIdx = AddNpcEx1({61},1,nil,38,1567*32,3075*32,nil,DOSCRIPTMOBINH,"Kim mé binh",6)	SetNpcValue(nNpcIdx,2)-- kim mo binh	
	
	local xTQuanY = TKPOS_NPC_DIALOG[nViTriTong][2]
	local yTQuanY = TKPOS_NPC_DIALOG[nViTriTong][3]
	local xKQuanY = TKPOS_NPC_DIALOG[nViTriKim][2]
	local yKQuanY = TKPOS_NPC_DIALOG[nViTriKim][3]	
		
	nNpcIdx = AddNpcEx1({203},1,nil,MAP_TK_TC,xTQuanY,yTQuanY,nil,DOSCRIPTQUANY,"Tèng Qu©n Y",6) SetNpcValue(nNpcIdx,ID_ShopQuanYTongKim)-- tong quan y
	AddMSNpc(MS_TONGKIM,nNpcIdx,1)
	nNpcIdx = AddNpcEx1({203},1,nil,MAP_TK_TC,xKQuanY,yKQuanY,nil,DOSCRIPTQUANY,"Kim Qu©n Y",6) SetNpcValue(nNpcIdx,ID_ShopQuanYTongKim)-- kim quan y
	AddMSNpc(MS_TONGKIM,nNpcIdx,2)

	local xTRuong = TKPOS_NPC_DIALOG[nViTriTong][4]
	local yTRuong = TKPOS_NPC_DIALOG[nViTriTong][5]
	local xKRuong = TKPOS_NPC_DIALOG[nViTriKim][4]
	local yKRuong = TKPOS_NPC_DIALOG[nViTriKim][5]	
	nNpcIdx = AddNpcEx1({625},1,nil,MAP_TK_TC,xTRuong,yTRuong,nil,DOSCRIPTRUONG,nil,6) --SetNpcValue(nNpcIdx,ID_LUU_RUONG_TONG)	-- ruong tong
	AddMSNpc(MS_TONGKIM,nNpcIdx,1)
	nNpcIdx = AddNpcEx1({625},1,nil,MAP_TK_TC,xKRuong,yKRuong,nil,DOSCRIPTRUONG,nil,6) --SetNpcValue(nNpcIdx,ID_LUU_RUONG_KIM)  -- ruong kim
	AddMSNpc(MS_TONGKIM,nNpcIdx,2)
	
	--nNpcIdx = AddNpcEx1({625},1,nil,MAP_TK_TC,1219*32,3574*32,nil,DOSCRIPTRUONG,nil,6) --SetNpcValue(nNpcIdx,ID_LUU_RUONG_TONG)	-- ruong tong
	--nNpcIdx = AddNpcEx1({625},1,nil,MAP_TK_TC,1705*32,3085*32,nil,DOSCRIPTRUONG,nil,6) --SetNpcValue(nNpcIdx,ID_LUU_RUONG_KIM)  -- ruong kim	

	local xTTSat = TKPOS_NPC_DIALOG[nViTriTong][6]
	local yTTSat = TKPOS_NPC_DIALOG[nViTriTong][7]
	local xKTSat = TKPOS_NPC_DIALOG[nViTriKim][6]
	local yKTSat = TKPOS_NPC_DIALOG[nViTriKim][7]	
	nNpcIdx = AddNpcEx1({55},1,nil,MAP_TK_TC,xTTSat,yTTSat,nil,DOSCRIPTTRINHSAT,"Tèng Trinh S¸t",6) SetNpcValue(nNpcIdx,1)-- tong 
	AddMSNpc(MS_TONGKIM,nNpcIdx,1)
	nNpcIdx = AddNpcEx1({49},1,nil,MAP_TK_TC,xKTSat,yKTSat,nil,DOSCRIPTTRINHSAT,"Kim Trinh S¸t",6) SetNpcValue(nNpcIdx,2)-- kim
	AddMSNpc(MS_TONGKIM,nNpcIdx,2)
	
	--nNpcIdx = AddNpcEx1({235},1,nil,38,1537*32,3161*32,nil,DOSCRIPTXAPHU,"Xa phu",6) SetNpcValue(nNpcIdx,1)	-- xa phu tong
	--nNpcIdx = AddNpcEx1({235},1,nil,38,1582*32,3075*32,nil,DOSCRIPTXAPHU,"Xa phu",6) SetNpcValue(nNpcIdx,2)	-- xa phu kim
	
	nNpcIdx = AddNpcEx1({630},1,nil,MAP_TK_TC,50336,102560,nil,DOSCRIPTCOSOAITONG,"",6) -- Cê So¸i Tèng
	AddMSNpc(MS_TONGKIM,nNpcIdx,1)
	nNpcIdx = AddNpcEx1({630},1,nil,MAP_TK_TC,42432,110144,nil,DOSCRIPTCOSOAIKIM,"",6)	--Cê So¸i Kim
	AddMSNpc(MS_TONGKIM,nNpcIdx,2)





	
	print("===>Add Npc chuc nang Tong Kim Trung Cap <===")
end

function addtraptongkimtrungcap()
	AddTrapEx2(MAP_TK_TC,1251,3529,10,"\\script\\maps\\tongkim\\trap\\tongratrai.lua")
	AddTrapEx2(MAP_TK_TC,1289,3480,10,"\\script\\maps\\tongkim\\trap\\tongvaotrai.lua")
	AddTrapEx2(MAP_TK_TC,1661,3098,10,"\\script\\maps\\tongkim\\trap\\kimratrai.lua")
	AddTrapEx2(MAP_TK_TC,1591,3162,10,"\\script\\maps\\tongkim\\trap\\kimvaotrai.lua")
	
	AddTrapEx1(MAP_TK_TC,1299,3420,18,"\\script\\maps\\tongkim\\trap\\tong_chancong.lua")
	AddTrapEx1(MAP_TK_TC,1342,3483,18,"\\script\\maps\\tongkim\\trap\\tong_chancong.lua")
	AddTrapEx2(MAP_TK_TC,1355,3403,18,"\\script\\maps\\tongkim\\trap\\tong_chancong.lua")
	
	AddTrapEx1(MAP_TK_TC,1537,3169,18,"\\script\\maps\\tongkim\\trap\\kim_chancong.lua")
	AddTrapEx1(MAP_TK_TC,1605,3233,18,"\\script\\maps\\tongkim\\trap\\kim_chancong.lua")
	AddTrapEx2(MAP_TK_TC,1525,3241,18,"\\script\\maps\\tongkim\\trap\\kim_chancong.lua")
	
	AddTrapEx2(MAP_TK_TC, POST_DES_CAMCO[1][1], POST_DES_CAMCO[1][2], 18, "\\script\\maps\\tongkim\\trap\\pos_cotong.lua") --add trap phe tèng ch¹y qua mang cê sÏ hoµn thµnh
	AddTrapEx2(MAP_TK_TC, POST_DES_CAMCO[2][1], POST_DES_CAMCO[2][2], 18, "\\script\\maps\\tongkim\\trap\\pos_cokim.lua") --add trap phe kim ch¹y qua mang cê sÏ hoµn thµnh
	print("===>Add TRAP Tong Kim Trung Cap <===")
end

function addnpccotongkim()
	local nNpcIndex
	local nRand = random(1,6)
	nNpcIndex = AddNpcEx1({TAB_PHE_TONGKIM[2][7]},1,{0},MAP_TK_TC,COKIM_TOADO[nRand][1]*32,COKIM_TOADO[nRand][2]*32,nil,DOSCRIPTCO,nil,6)
	AddMSNpc(MS_TONGKIM,nNpcIndex)
	SetNpcValue(nNpcIndex, 2)
	
	nNpcIndex = AddNpcEx1({TAB_PHE_TONGKIM[1][7]},1,{0},MAP_TK_TC,COTONG_TOADO[nRand][1]*32,COTONG_TOADO[nRand][2]*32,nil,DOSCRIPTCO,nil,6)
	AddMSNpc(MS_TONGKIM,nNpcIndex)
	SetNpcValue(nNpcIndex, 1)
end

function addobjtongkimtrungcap()
end

function AddRunSpeed()
	local ntime = 60*60*18 --60 phót thêi gian ng­êi gi÷ cê bÞ gi¶m tèc ®é
	local overlook = 0
	AddSkillState(656,RUN_SPEDD_DOW_ONFLAG,1, ntime, overlook)--overlook = 0 not save 1 delete -1 save
end

function RestoreRunSpeed()
	local ntime = 1
	local overlook = 1
	AddSkillState(656,RUN_SPEDD_DOW_ONFLAG,1,ntime, overlook) --overlook = 0 not save 1 delete -1 save
end

function ThuongHaiPheHoaTongKim(nRank)
	local nNum = nRank*2
	local nExp = 5000000
	AddSumExp(nExp*nNum)
	AddItemSL(1023,10,0)
	AddItemSL(3846,2,0)
	AddItemSL(4728,2,0)
	AddItemSL(4844,100,0)
	AddItemSL(4831,20,0) -- huy chuong tong kim
	AddItemSL(4850,2,0) -- ruong trang bi xanh
	SetTask(TASK_DSK, GetTask(TASK_DSK) + 50)
	Msg2Player("Chóc mõng b¹n ®· nhËn ®­îc 50 ®iÓm sù kiÖn trong ho¹t ®éng tèng kim .")
	SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + 2)
	Msg2Player("Chóc mõng b¹n ®· nhËn ®­îc 2 Xu trong ho¹t ®éng tèng kim .")
	logRutXuTK(format("***Account [%s] nh©n vËt [%s] nhËn ®­îc 2 Xu t¹i ho¹t ®éng tèng kim hoµ.***", GetAccount(), GetName()))
end


function ThuongPheThangTongKim(nRank)
	local nNum = nRank*3
	local nExp = 15000000
	AddSumExp(nExp*nNum)
	AddItemSL(1023,30,0)
	AddItemSL(3846,5,0)
	AddItemSL(4728,5,0)
	AddItemSL(4844,500,0)
	AddItemSL(4831,50,0) -- huy chuong tong kim
	AddItemSL(4850,10,0) -- ruong trang bi xanh
	SetTask(TASK_DSK, GetTask(TASK_DSK) + 50)
	Msg2Player("Chóc mõng b¹n ®· nhËn ®­îc 50 ®iÓm sù kiÖn trong ho¹t ®éng tèng kim .")
	SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + 5)
	Msg2Player("Chóc mõng b¹n ®· nhËn ®­îc 5  Xu trong ho¹t ®éng tèng kim .")
	logRutXuTK(format("***Account [%s] nh©n vËt [%s] nhËn ®­îc 5 Xu t¹i ho¹t ®éng tèng kim dµnh th¾ng lîi.***", GetAccount(), GetName()))
end

function ThuongPheThuaTongKim(nRank)
	local nNum = nRank*1
	Msg2Player("Kh¸ch quan ®· nhËn ®­îc "..nNum.."  Xu.")
	local nExp = 10000000
	AddSumExp(nExp*nNum)
	AddItemSL(1023,20,0)
	AddItemSL(3846,3,0)
	AddItemSL(4728,3,0)
	AddItemSL(4844,300,0)
	AddItemSL(4831,30,0) -- huy chuong tong kim
	AddItemSL(4850,5,0) -- ruong trang bi xanh
	SetTask(TASK_DSK, GetTask(TASK_DSK) + 50)
	Msg2Player("Chóc mõng b¹n ®· nhËn ®­îc 50 ®iÓm sù kiÖn trong ho¹t ®éng tèng kim .")
	SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + 2)
	Msg2Player("Chóc mõng b¹n ®· nhËn ®­îc 2 Xu trong ho¹t ®éng tèng kim .")
	logRutXuTK(format("***Account [%s] nh©n vËt [%s] nhËn ®­îc 2 Xu t¹i ho¹t ®éng tèng kim dµnh th¾ng lîi.***", GetAccount(), GetName()))
end

function reset_savetask_tongkim_tk()
	
	SetTask(T_SAVE_TK_KILLPLAYER, 0)
	SetTask(T_SAVE_TK_KILLNPC, 0)
	SetTask(T_SAVE_TK_DEATH, 0)
	SetTask(T_SAVE_TK_MAXLT, 0)
	SetTask(T_SAVE_TK_POINT, 0) --hÕt trËn th× bá ®iÓm l­u trËn tr­íc ®ã	
	SetTask(T_SAVE_TK_CURLT, 0)
	SetTask(T_SAVE_TK_NUMLAG, 0)
	SetTask(T_SAVE_TK_RANK, 0) --hÕt trËn th× bá danh hiÖu trËn tr­íc ®ã
	SetTask(T_SAVE_TK_NHATBV, 0)
	SetTask(T_SAVE_TK_POINTKILLPL, 0)
	SetTask(T_SAVE_TK_POINTKILLNPC, 0)
	SetTask(T_SAVE_TK_POINTLT, 0)
	SetTask(T_SAVE_TK_POINTBV, 0)
	SetTask(T_SAVE_TK_POINTFLAG, 0)	
end


function AdminTestTK()
	SayEx({ADMIN_SUPPORT_TEXT, 
	"B¸o danh bªn Tèng trung cÊp/ditongtc",
	"B¸o danh bªn Kim trung cÊp/dikimtc",
	"HËu doanh phe Tèng/haudoanhphetong",
	"HËu doanh phe Kim/haudoanhphekim",	
	"§µi so¸i phe Tèng/daisoaiphetong",
	"§µi so¸i phe Kim/daisoaiphekim",
	-- "T¾t TK /CloseMissonTK",
	NOW_END_SAY})
end

function CloseMissonTK()

	CloseMission(MS_TONGKIM)	--dong mission
	--Msg2Player("IsMission(MS_PLANGDO)"..IsMission(MS_PLANGDO))
	print("===>Tong Kim: tat<===")
end

function daisoaiphetong()
	NewWorld(MAP_TK_TC,1333,3442) SetFightState(1)
end

function daisoaiphekim()
	NewWorld(MAP_TK_TC,1571,3196) SetFightState(1)
end

function haudoanhphetong()
	NewWorld(MAP_TK_TC,1229,3561) SetFightState(0)
end

function haudoanhphekim()
	NewWorld(MAP_TK_TC,1689,3074) SetFightState(0)
end

function ditongtc()
	battle_transprot(1,MAP_BD_TC)
end

function dikimtc()
	battle_transprot(2,MAP_BD_TC)
end

function battle_transprot(nSel, mapid)
	local tbsongjin_pos = {1541, 3178}
	local szstr = "phe Tèng (T)"
	if (nSel == 2) then
		tbsongjin_pos = {1570, 3085}
		szstr = "phe Kim (K)"
	end
	if (mapid >= 323 and mapid <= 325) then
		NewWorld( mapid, tbsongjin_pos[1], tbsongjin_pos[2]) SetFightState(0)
		-- KickOutSelf()
	--	SetTask(T_CHECKPHETK, 0) --hÕt trËn set phe tham gia 0
	--	SetTask(T_CHECKDATETK, 0)--hÕt trËn set thêi gian tham gia 0
		Msg2Player( "Vµo ®iÓm b¸o danh "..szstr)
	end
end

function UpdatePlayerCountMSTK() --update player dang tham gia tong kim
	local oldPlayerIdx = PlayerIndex
	local nPTong = GetMSPlayerCount(MS_TONGKIM,1)
	local nPKim  = GetMSPlayerCount(MS_TONGKIM,2)
	for i=dataindex,GetMSPlayerCount(MS_TONGKIM) do
		if(GetPMParam(MS_TONGKIM, dataindex, 0) == 1) then		
			PlayerIndex = MSDIdx2PIdx(MS_TONGKIM, dataindex)
			--UpdateBattleBox(MS_TONGKIM,2,nPTong,nPKim)
		end
	end		
	PlayerIndex = oldPlayerIdx
end

function updatetopten(nTotalAccum,nDesPlayerData)
	local i
	local nRankAcc = 0
	local nData = 0	
	--tim xem co trong bang chua?
	for i=11,20 do
		if(nDesPlayerData == GetMissionV(i)) then
			nData = i
			i = 21 --break
		end
	end
	--check diem tich luy co kha nang vao bang ko? va vi tri thu may?
	for i=1,10 do
		--Msg2Player(format("%d",GetMissionV(i+10)))
		if(nTotalAccum > GetMissionV(i)) then
			nRankAcc = i
			i = 11 --break
		end
		if(nData == i+10 and nRankAcc < 0) then
			nRankAcc = i
			i = 11 --break
		end
	end
	--Msg2Player(format("updatetopten %d %d %d", nTotalAccum,nDesPlayerData,nRankAcc))
	if(nRankAcc <= 0) then --neu ko du diem de vao bang thi ngung
		return 0
	 end
	--du diem vao bang~ thi` sap xep lai bang~
	if(nRankAcc == 10 or nData-10 == nRankAcc) then	--cuoi' bang~ hoac trung vi tri'
		SetMission(nRankAcc,nTotalAccum)	--chen data moi vao
		SetMission(nRankAcc+10,nDesPlayerData)
		return 1 
	end

	local RankTab = {}
	local DataTab = {}
	local count = 0
	--sao luu data cu~
	if(nData == 0) then
		for i=nRankAcc,9 do
			count = count + 1
			RankTab[count] = GetMissionV(i)
			DataTab[count] = GetMissionV(i+10)
		end
	else
		for i=nRankAcc,nData-11 do		-- 5 15 4
			count = count + 1
			RankTab[count] = GetMissionV(i)
			DataTab[count] = GetMissionV(i+10)
		end
		for i=nData-9,10 do			-- 15 5 10
			count = count + 1
			RankTab[count] = GetMissionV(i)
			DataTab[count] = GetMissionV(i+10)
		end
	end
	SetMission(nRankAcc,nTotalAccum)	--chen data moi vao
	SetMission(nRankAcc+10,nDesPlayerData)	--chen data moi vao
	for i=nRankAcc+1,10 do
		SetMission(i,RankTab[i-nRankAcc])	--di chuyen data cu~ xuong 1 bac
		SetMission(i+10,DataTab[i-nRankAcc])	--di chuyen data cu~ xuong 1 bac
	end
	--for i=0,9 do
		--	Msg2Player("Rank "..(i+1).." Point:"..GetMissionV(i).." PDataIdx:"..GetMissionV(i+10))
	--end
	return 1
end

-- 0: online
-- 1: chien dau
-- 2: giet player
-- 3: giet npc
-- 4: tu vong
-- 5: lien tram cao nhat
-- 6: tich luy tam
-- 7: lien tram hien tai
-- 8: dang giu co
-- 9: cam co
-- 10: xep hang
-- 11: quan ham
-- 12: nhÆt b¸u vËt
-- 13: tÝch luü giÕt player
-- 14: tÝch luü giÕt npc
-- 15: tÝch luü liªn tr¶m
-- 16: tÝch luü nhÆt b¶o vËt
-- 17: tÝch luü nhÆt cê
