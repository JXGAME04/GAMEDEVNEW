-- [LOCAL54 06/09 toi] 4 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local TK_CFG, TK_GiaiDoan, TK_TimerHien, TK_GuiThongTinPhe
-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Function: Lib Tèng Kim

Include("\\script\\lib\\lib_server.lua")
-- [CFGTKT 30/08] hai tep duoi day la LA (khong Include gi).
Include("\\script\\cauhinh\\ch_thuong.lua")
-- [CFGTK 29/08] hai tep duoi day la LA (khong Include gi).
Include("\\script\\cauhinh\\ch_lib.lua")
Include("\\script\\cauhinh\\ch_lich.lua")
-- [CFGTK 29/08] Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu) khi bo cau
-- hinh chua nap, nen kem nhat cung khong the doi hanh vi.
function TK_CFG(szKhoa, macdinh)
	if (G_CFG ~= nil) then
		return G_CFG(szKhoa, macdinh)
	end
	return macdinh
end

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

LEVEL_ENOUGH_TK 							= TK_CFG("TKT_CAP_TOI_THIEU", 80)		--®¼ng cÊp tèi thiÓu khi tham gia
MAX_PLAYER_CL 								= TK_CFG("TKT_LECH_QUANSO_TOIDA", 20)			--chªnh lÖch qu©n sè 2 bªn mÆc ®Þnh lµ 5 ng­êi
NEEDMONEY 										= TK_CFG("TKT_PHI_BAO_DANH", 20000)--tiÒn b¸o danh tham gia tèng kim
TICH_LUY_THUONG_TK_W				= TK_CFG("TKT_THUONG_TICHLUY_THANG", 1200)	--®iÓm th­ëng tÝch luü céng thªm cho phe th¾ng
TICH_LUY_THUONG_TK_L 				= TK_CFG("TKT_THUONG_TICHLUY_THUA", 300)		--®iÓm th­ëng tÝch luü céng thªm cho phe thua
TICH_LUY_THUONG_TK_H 				= TK_CFG("TKT_THUONG_TICHLUY_HOA", 600)		--®iÓm th­ëng tÝch luü céng thªm hai phe hoµ nhau
MIN_POINT_AWARD							= TK_CFG("TKT_DIEM_TOITHIEU_NHAN_THUONG", 1000)	--®iÓm tÝch luü tèi thiÓu ®Ó kÕt thóc trËn nhËn ®­îc quµ
MAX_CAMCO											=	TK_CFG("TKT_SO_CO_TOIDA", 30)		--sè cê tèi ®a hai bªn cã thÓ c¾m
FLAG_POINT 											= TK_CFG("TKT_DIEM_CAM_CO", 300) 		-- ®iÓm sè tÝch luü trong tèng kim khi c¾m cê
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
PLAYER_MS_LIMIT								= TK_CFG("TK_NGUOI_MOI_PHE", 2000) --55 ng­êi mçi phe
TIME_BD_TK											= TK_CFG("TK_PHUT_BAODANH", 1) --10 phut b¸o danh
TIME_KT_TK											= TK_CFG("TK_PHUT_TRAN", 30) -- 70 phut chiÕn ®Êu gåm 10 phót b¸o danh
TIME_NS_TK											= TK_CFG("TK_PHUT_NGUYENSOAI", 1) -- 30 phut thêi gian xuÊt hiÖn nguyªn so¸i
TIME_XOANPC_TK								= TK_CFG("TK_PHUT_XOANPC", 1) -- 1 phut xoa npc 
ID_ShopQuanYTongKim 						= 29
ID_LUU_RUONG_TONG						= 42
ID_LUU_RUONG_KIM							= 43
ITEM_CO_HIEU										= {6,1,158}	--Item cê hiÖu ®Ó tham gia thÓ thøc ®o¹t cê
RUN_SPEDD_DOW_ONFLAG				= TK_CFG("TKT_MUC_GIAM_TOCDO_GIU_CO", 60) --gi¶m tèc ®é khi gi÷ cê
TIME_IN_TRAI										= TK_CFG("TKT_GIAY_TRONG_DOANHTRAI", 90) --thêi gian tèi ®a ë trong doanh tr¹i
TIME_DELAY_RA_TRAI						= TK_CFG("TKT_GIAY_DELAY_RA_TRAI", 10) --thêi gian delay khi ra tr¹i
LIFE_NGUYEN_SOAI							= TK_CFG("TKT_MAU_NGUYENSOAI", 5000000)*STRONGBOSS_NSTK --m¸u boss nguyªn so¸i
TK_VARV_KEY = 1

TAB_TIME_TONG_KIM = TK_CFG("TK_LICH", {{23,46,19,1}, {17,50,18,3}, {20,50,21,4}, {22,50,23,5}})


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


TICHLUY_GIETSOAI = TK_CFG("TKT_DIEM_GIET_NGUYENSOAI", 5000)

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
		return PT_CuuSat
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

-- [MAIL 03/09 D9] Thuong KET THUC Tong Kim gui QUA THU (chu 03/09): vat pham + xu + diem su kien vao thu, kinh nghiem
-- van cong ngay (AddSumExp). So luong giu nguyen ban cu (hoa / thang / thua). Vat pham genre 6 = 1 CHONG (nhu AddItemSL).
MAILTK_ITEMS = {
	hoa   = {{1023,10},{3846,2},{4728,2},{4844,100},{4831,20},{4850,2}},
	thang = {{1023,30},{3846,5},{4728,5},{4844,500},{4831,50},{4850,10}},
	thua  = {{1023,20},{3846,3},{4728,3},{4844,300},{4831,30},{4850,5}},
}
function ThuongTongKimQuaThu(szLoai, nRank, nXu, szTieuDe)
	Include("\\script\\mail\\mailmanager.lua")
	local tb = {}
	local tbItems = MAILTK_ITEMS[szLoai] or {}
	for i = 1, getn(tbItems) do
		tinsert(tb, {tbProp = {6, 1, tbItems[i][1], 0, 0, 0}, nCount = tbItems[i][2], nStack = 1})
	end
	tinsert(tb, {szKind = "xu", nCount = nXu})
	tinsert(tb, {szKind = "task", nTaskId = TASK_DSK, nCount = 50})
	local szND = "§¹i hiÖp th©n mÕn,<enter>TrËn Tèng Kim võa kÕt thóc, phÇn th­ëng cña ng­¬i (h¹ng "..nRank..") ®Ýnh kÌm trong th­.<enter>Tr©n träng"
	local nId = MailManager_SendRewardTemplet("tongkim", nil, szTieuDe, szND, tb, 30)
	if nId > 0 then
		Msg2Player("PhÇn th­ëng Tèng Kim ®· göi vµo hép th­ ("..getn(tb).." môc).")
	else
		Msg2Player("Göi th­ th­ëng Tèng Kim thÊt b¹i, h·y b¸o GM.")
		GhiLog("MAIL", format("TongKim %s hang %d: gui thu that bai cho %s", szLoai, nRank, GetName()))
	end
	logRutXuTK(format("***Account [%s] nhan vat [%s] nhan %d Xu qua thu (Tong Kim %s hang %d).***", GetAccount(), GetName(), nXu, szLoai, nRank))
end
function ThuongHaiPheHoaTongKim(nRank)
	AddSumExp(5000000 * nRank * 2)
	ThuongTongKimQuaThu("hoa", nRank, 2, "Th­ëng Tèng Kim - hai phe hoµ")
end
function ThuongPheThangTongKim(nRank)
	AddSumExp(15000000 * nRank * 3)
	ThuongTongKimQuaThu("thang", nRank, 5, "Th­ëng Tèng Kim - phe th¾ng")
end
function ThuongPheThuaTongKim(nRank)
	AddSumExp(10000000 * nRank * 1)
	ThuongTongKimQuaThu("thua", nRank, 2, "Th­ëng Tèng Kim - phe thua")
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
			break	--[LUA54] truoc la 'i = 21': Lua 4 thoat vong, 5.4 gan khong tac dung
		end
	end
	--check diem tich luy co kha nang vao bang ko? va vi tri thu may?
	for i=1,10 do
		--Msg2Player(format("%d",GetMissionV(i+10)))
		if(nTotalAccum > GetMissionV(i)) then
			nRankAcc = i
			break	--[LUA54] truoc la 'i = 11': Lua 4 thoat vong, 5.4 gan khong tac dung
		end
		if(nData == i+10 and nRankAcc < 0) then
			nRankAcc = i
			break	--[LUA54] truoc la 'i = 11': Lua 4 thoat vong, 5.4 gan khong tac dung
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

-- ================================================================ [TKDIEM 04/09] BANG DIEM "Tong VS Kim" giua-tren man hinh
-- Chu 04/09: "mo ban 2.0 lay cai hien so diem nhu tren hinh gan vao tong kim cua du an". Client Game.exe co lop
-- KUiTongKimScore (mo tu KUiHuaihehepan cua 2.0: hai nhan diem + thanh can bang % = tong*100/(tong+kim)).
-- Kenh: UpdateBattleBox(MS_TONGKIM, nTong, nKim, nDiemToi, nKind) -> S2C_BATTLE_BOX "tong|kim|diem" (5 tham so,
-- co san trong ScriptFuns.cpp, khong doi giao thuc). nKind 6 = cap nhat (hien bang), 9 = an bang.
-- Diem phe = GetMissionV(M_TICHLUYA) (Tong) / GetMissionV(M_TICHLUYB) (Kim) - dung bien cac script giet dang cong.
-- PHAI goi trong ngu canh co PlayerIndex + SubWorldIndex cua tran (tongtu/kimtu/trap ra trai/task03 deu co).
TKDIEM_KIND_CAPNHAT = 6
TKDIEM_KIND_AN      = 9
TKDIEM_MAX_O        = 600	-- MAX_PLAYER_MISSION (KMission.h)
TKDIEM_KHUNG_GIUA_2LAN = 18	-- [TKDIEM 05/09] toi thieu 18 khung (1 giay; GetGameTime script JX1 = frame 18/s) giua hai lan gui ca phe
TKDIEM_KHUNG_GUI_CUOI  = 0

-- gui cho CHINH nguoi choi PlayerIndex hien tai
function TK_GuiDiemChoToi(nKind)
	local nIdx = PIdx2MSDIdx(MS_TONGKIM, PlayerIndex)
	if nIdx == nil or nIdx <= 0 then
		return 0
	end
	UpdateBattleBox(MS_TONGKIM, GetMissionV(M_TICHLUYA), GetMissionV(M_TICHLUYB), GetPMParam(MS_TONGKIM, nIdx, 6), nKind or TKDIEM_KIND_CAPNHAT)
	return 1
end

-- gui cho MOI nguoi con trong tran. O du lieu mission khong lien tuc va co the con chi so cu cua nguoi da roi,
-- nen chi nhan o ma chinh nguoi choi do tra nguoc ve dung o (PIdx2MSDIdx == i). UpdateBattleBox tu bo qua
-- nguoi da mat ket noi (m_nNetConnectIdx == -1).
function TK_GuiDiemPhe(nKind)
	if nKind ~= TKDIEM_KIND_AN then
		TK_GuiThongTinPhe()	-- [TKINFO 05/09] cua so thong tin tran (tu tiet che 3 s)
	end
	-- [TKDIEM 05/09] chu 20:40 "fix xong thay game lag hon nhieu": truoc day MOI lan giet gui 1 goi cho TUNG nguoi
	-- (toi 600 goi) -> Tong Kim dong, hang chuc lan giet/giay = hang nghin goi/giay + client cap nhat bang lien tuc.
	-- Nay toi da 1 lan / 1 giay (05/09b, truoc 2 giay; lan giet ke tiep se gui tiep); lenh AN (kind 9) khong bi han che.
	if nKind ~= TKDIEM_KIND_AN then
		local nKhung = GetGameTime()
		if nKhung - TKDIEM_KHUNG_GUI_CUOI < TKDIEM_KHUNG_GIUA_2LAN then
			return 0
		end
		TKDIEM_KHUNG_GUI_CUOI = nKhung
	end
	local nTong = GetMissionV(M_TICHLUYA)
	local nKim = GetMissionV(M_TICHLUYB)
	-- [TKDIEM 05/09b] MOT loi goi C++ duyet thang danh sach tran (UpdateBattleBoxAll, ScriptFuns.cpp) thay vong 600 o
	-- (2.400 loi goi Lua-C, ~90 ms moi lan - ban giao 13f). CoreServer cu chua co ham -> roi ve vong cu, khong loi script.
	if UpdateBattleBoxAll then
		return UpdateBattleBoxAll(MS_TONGKIM, nTong, nKim, nKind or TKDIEM_KIND_CAPNHAT)
	end
	local nCu = PlayerIndex
	local nSo = 0
	for i = 1, TKDIEM_MAX_O do
		local nIdx = MSDIdx2PIdx(MS_TONGKIM, i)
		if nIdx ~= nil and nIdx > 0 and PIdx2MSDIdx(MS_TONGKIM, nIdx) == i then
			PlayerIndex = nIdx
			UpdateBattleBox(MS_TONGKIM, nTong, nKim, GetPMParam(MS_TONGKIM, i, 6), nKind or TKDIEM_KIND_CAPNHAT)
			nSo = nSo + 1
		end
	end
	PlayerIndex = nCu
	return nSo
end

-- ================================================================ [TKINFO 05/09] CUA SO 'THONG TIN TRAN' kieu Lien Dau Bang 2.0
-- Client KUiTongKimInfo: giai doan, giay con lai (client tu dem lui), top 5 tich luy (hang/ten/phe/diem), nut xem Chien Bao.
-- Kenh S2C_BATTLE_BOX kind 7 (dau) + 8 (hang), kind 9 (an) dung chung voi bang diem. C++: UpdateBattleInfo / UpdateBattleInfoAll
-- (ScriptFuns.cpp) doc m_MissionLadder (top theo tham so ladder = 6 tich luy, xep san trong C++) + timer con lai.
-- Cu phap trung tinh Lua 4 / 5.4. CoreServer cu chua co ham -> bo qua, khong loi.
TKINFO_KHUNG_GIUA_2LAN = 54	-- 3 giay giua hai lan phat ca tran
TKINFO_KHUNG_GUI_CUOI  = 0
TKINFO_SO_HANG         = 5

-- 1 = bao danh (timer 1 con chay), 2 = chien dau
function TK_GiaiDoan()
	local nBD = GetMSRestTime(MS_TONGKIM, 1)
	if nBD ~= nil and nBD > 0 then
		return 1
	end
	return 2
end

-- timer dem lui hien tren cua so: bao danh -> timer 1, chien dau -> timer 3 (ca tran)
function TK_TimerHien(nPha)
	if nPha == 1 then
		return 1
	end
	return 3
end

-- gui cho CHINH PlayerIndex hien tai (ra khoi hau doanh / vao tran)
function TK_GuiThongTinChoToi()
	if UpdateBattleInfo == nil then
		return 0
	end
	local nPha = TK_GiaiDoan()
	return UpdateBattleInfo(MS_TONGKIM, nPha, TK_TimerHien(nPha), GetMissionV(M_TICHLUYA), GetMissionV(M_TICHLUYB), TKINFO_SO_HANG)
end

-- gui ca tran, tu tiet che TKINFO_KHUNG_GIUA_2LAN (goi tu TK_GuiDiemPhe moi lan giet)
function TK_GuiThongTinPhe()
	if UpdateBattleInfoAll == nil then
		return 0
	end
	local nKhung = GetGameTime()
	if nKhung - TKINFO_KHUNG_GUI_CUOI < TKINFO_KHUNG_GIUA_2LAN then
		return 0
	end
	TKINFO_KHUNG_GUI_CUOI = nKhung
	local nPha = TK_GiaiDoan()
	return UpdateBattleInfoAll(MS_TONGKIM, nPha, TK_TimerHien(nPha), GetMissionV(M_TICHLUYA), GetMissionV(M_TICHLUYB), TKINFO_SO_HANG)
end
