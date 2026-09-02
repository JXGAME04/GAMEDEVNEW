Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
--增加华山trap点
--

Include("\\script\\lib\\getrectangle_point.lua") --获得矩形点

--演武场Trap点
function add_trap_huashanyanwu()
	local tbpoint =
	{
		tbtoppoint={1488,2966},
		nleftstep = 14,
		nrightstep = 3,
	}
	local nMapID = 987
	local szScriptfile1 = "\\script\\global\\huashan2013\\trap\\yanwuchang_trap1.lua"
	local szScriptfile2 = "\\script\\global\\huashan2013\\trap\\yanwuchang_trap2.lua"
    local szScriptfile3 = "\\script\\global\\huashan2013\\trap\\huashanpai2huashan_trap.lua"

	--演武场trap点1
	local tballpoint = getRectanglePoint(tbpoint)
	for nx,tbp in tballpoint do
		AddMapTrap(nMapID,floor(tbp[1]*32),floor(tbp[2]*32),szScriptfile1)
	end

	--演武场trap点2
	local tballpoint2 = {
		[1] = {1474,2996}, [2] = {1473,2997},
		[3] = {1475,2997}, [4] = {1474,2998},
		[5] = {1476,2998}, [6] = {1475,2999},
		[7] = {1477,2999}, [8] = {1476,3000},
		[9] = {1478,3000}, [10] = {1477,3001},
		[11] = {1479,3001},[12] = {1478,3002},
	}
	for nx,tbp in tballpoint2 do
		AddMapTrap(nMapID,floor(tbp[1]*32),floor(tbp[2]*32),szScriptfile2)
	end


    local tballpoint3 = {
		[1] = {1230,3349}, [2] = {1230,3351},
		[3] = {1231,3352}, [4] = {1232,3353},
		[5] = {1233,3354}, [6] = {1234,3355},
		[7] = {1235,3355}, [8] = {1235,3356},
		[9] = {1237,3358}, [10] = {1238,3359},
		[11] = {1239,3360},[12] = {1239,3361},
	}
	for nx,tbp in tballpoint3 do
		AddMapTrap(nMapID,floor(tbp[1]*32),floor(tbp[2]*32),szScriptfile3)
    end
end

--瀑布Trap点
function add_trap_huashanpubu()
	local tbpoint =
	{
		tbtoppoint={1480,2835},
		nleftstep = 8,
		nrightstep = 4,
	}
	local nMapID = 987
	local szScriptfile = "\\script\\global\\huashan2013\\trap\\pubu_trap.lua"

	--瀑布trap点
	local tballpoint = getRectanglePoint(tbpoint)
	for nx,tbp in tballpoint do
		AddMapTrap(nMapID,floor(tbp[1]*32),floor(tbp[2]*32),szScriptfile)
	end
end
