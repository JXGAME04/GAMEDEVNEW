Include("\\script\\header\\cauhinh_hoatdong.lua")	-- [HD CONFIG 24/08] radius bua chinh duoc
Include("\\script\\missions\\tongcastle\\tongcastle.lua")
Include("\\script\\lib\\common.lua")
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\dailogsys\\dailogsay.lua")

local tbUseMapInfo = {
	[981] = {4, 37},
	[984] = {7, 176},
}

local tbGuardType = {
	[1] = {nNpcId = 1908, szName = "B¨ng §«ng Thñ VÖ", nNpcParam = 4,},
	[2] = {nNpcId = 1909, szName = "HuyÔn Vùng Thñ VÖ", nNpcParam = 5,},
	[3] = {nNpcId = 1910, szName = "Tõ Hµng Thñ VÖ", nNpcParam = 6,},
	[4] = {nNpcId = 1911, szName = "Háa T­êng Thñ VÖ", nNpcParam = 7,},
}

function CheckNearTreeNpc()
	local tbNpc, nCount = GetAroundNpcList(HD_CFG("TC_BANKINH_BUA", 15))
	for i = 1, nCount do 
		local nNpcParam = GetNpcParam(tbNpc[i], 1)
		local nTmpCamp = GetTmpCamp(tbNpc[i])
		if  nNpcParam >= 1 and nNpcParam <= 3 and nTmpCamp == 1 then
			return 1
		end
	end
end

function CallGuard(nType, nItemIndex)
	-- [TONGCASTLE 23/08] kho Thu Ve (GuardIsLimit/RegANpc) song trong state chu -> chuyen ca luong sang do
	DynamicExecuteByPlayer(PlayerIndex, "\\script\\missions\\tongcastle\\tongcastle.lua", "TongCastle:JX1_CallGuard", nType, nItemIndex)
end

function main(nItemIndex)
	local szTitle = "ChänThñ VÖ mµ ng­¬i muèn triÖu håi"
	local tbOpt = 
	{
		{"TriÖu håi B¨ng §«ng Thñ VÖ", CallGuard, {1, nItemIndex}},
		{"TriÖu håi HuyÔn Vùng Thñ VÖ", CallGuard, {2, nItemIndex}},
		--{"ÕÙ»½´Èº½ÊØÎÀ", CallGuard, {3, nItemIndex}},
		{"TriÖu håi Háa T­êng Thñ VÖ", CallGuard, {4, nItemIndex}},
		{" KÕt thóc ®èi tho¹i!"}
	}
	CreateNewSayEx(szTitle, tbOpt)
	return 1
end