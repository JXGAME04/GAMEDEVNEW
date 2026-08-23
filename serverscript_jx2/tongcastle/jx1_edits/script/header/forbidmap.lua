--Author: Fong KiÒu

SJMAPS = {44,326,327,328,329,330,331,344,345,346,347,348,349,350,351,352,353,354,355,357,358,359,360,361,362,363,364,365,366,367,368,369,370,371,372,373,374,863}
BWMAPS = {242,243,244,245,246,247,248,396,397,398,399,400,401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,527,528,529,530,531,532,533,534,535,536,537,538,864,865,866,867}
ZQMAPS = {520,521,522,523,524,525,526,582,583,584,585,825,826,863}
FHMAPS = {516,517,518,519,580,581,605,606,607,608,609,610,611,612,613}
YANDIBAOZANG = {851,852,853,854,855,856,857,858,859,860,861,862,871,872,873,874}
TONGMAPS = {586,587,588,589,590,591,593,594,595,596,597}
NEWPRACTICEMAPS = {917,918,919,920,921,922,923,924}
YANDIBAOZANG = {851,852,853,854,855,856,857,858,859,860,861,862,871,872,873,874,892,893,894,895,896,901}
TRAINMAPS = {1,11,162,37,78,80,176,20,121,53,54,174,2,101,99,100,153,21,70,90,92,41,122,125,163,9,56,319,123,224,320,321,322,75,225,226,227,92,124,336,340}

function checkActMaps(nMapId)
	
	if nMapId >=906 and nMapId <= 916 then
		return 1
	end

	for i=1,getn(YANDIBAOZANG) do
		if(nMapId == YANDIBAOZANG[i]) then return 1 end
	end
	return 0
end

function checkYDBZMaps(mapid)
	for i=1,getn(YANDIBAOZANG) do
		if(mapid == YANDIBAOZANG[i]) then return 1 end
	end
	return 0
end

function checkSJMaps(mapid)
	for i=1,getn(SJMAPS) do
		if(mapid == SJMAPS[i]) then return 1 end
	end
	if (mapid >= 375 and mapid <= 386) or (mapid >= 387 and mapid <= 395) then
		return 1
	end
	return 0
end

function checkBWMaps(mapid)
	if (mapid >= 540 and mapid <= 579) then
		return 1
	end
	for i=1,getn(BWMAPS) do
		if(mapid == BWMAPS[i]) then return 1 end
	end
	return 0
end

function checkZQMaps(mapid)
	for i=1,getn(ZQMAPS) do
		if(mapid == ZQMAPS[i]) then return 1 end
	end
	return 0
end

function checkXSMaps(mapid)
	if (mapid >= 387 and mapid <= 394) then
		return 1
	end
	return 0
end

function checkHFMaps(mapid)
	for i = 1, getn(FHMAPS) do
		if(mapid == FHMAPS[i]) then return 1 end
	end
	return 0
end

function checkTONGMaps(mapid)
	for i = 1, getn(TONGMAPS) do
		if(mapid == TONGMAPS[i]) then return 1 end
	end
	return 0
end

function checkNEWPRACTICEMaps(mapid)
	for i = 1, getn(NEWPRACTICEMAPS) do
		if(mapid == NEWPRACTICEMAPS[i]) then return 1 end
	end
	return 0
end

function checkTRAINMaps(mapid)
	for i = 1, getn(TRAINMAPS) do
		if(mapid == TRAINMAPS[i]) then return 1 end
	end
	return 0
end

function CheckAllMaps(mapid)
	-- [TONGCASTLE 23/08 phan bien F15] cam moi item dich chuyen trong Thanh Bao 984
	-- (DisabledUseTownP la no-op tren JX1; Linux cam qua forbiditem TRANSFER)
	if mapid == 984 then
		return 1
	end
	if checkSJMaps(mapid) == 1 then
		return 1
	end
	if checkBWMaps(mapid) == 1 then
		return 1
	end
	if checkZQMaps(mapid) == 1 then
		return 1
	end
	if checkXSMaps(mapid) == 1 then
		return 1
	end
	if checkHFMaps(mapid) == 1 then
		return 1
	end
	if checkActMaps(mapid) == 1 then
		return 1
	end
	return 0
end