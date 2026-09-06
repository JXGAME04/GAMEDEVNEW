--¾«Ìú¿ó
Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\lib\\awardtemplet.lua")

local tbJingTieKuangAwardList = 
{
    {szName = "Th­¬ng Long V¨n C­¬ng- Gi¸p [CÊp 2]",nRate= 3.04,tbProp={8,1,2,1,0,0}},
    {szName = "Th­¬ng Long V¨n C­¬ng- Gi¸p [CÊp 3]",nRate= 0.8,tbProp={8,1,3,1,0,0}},
    {szName = "Th­¬ng Long V¨n C­¬ng- Gi¸p [CÊp 4]",nRate= 0.16,tbProp={8,1,4,1,0,0}},
    {szName = "Th­¬ng Long V¨n C­¬ng- Êt [CÊp 2]",nRate= 3.04,tbProp={8,1,12,1,0,0}},
    {szName = "Th­¬ng Long V¨n C­¬ng- Êt [CÊp 3]",nRate= 0.8,tbProp={8,1,13,1,0,0}},
    {szName = "Th­¬ng Long V¨n C­¬ng- Êt [CÊp 4]",nRate= 0.16,tbProp={8,1,14,1,0,0}},
    {szName = "Th­¬ng Long V¨n C­¬ng- BÝnh[CÊp 2]",nRate= 3.04,tbProp={8,1,22,1,0,0}},
    {szName = "Th­¬ng Long V¨n C­¬ng- BÝnh[CÊp 3]",nRate= 0.8,tbProp={8,1,23,1,0,0}},
    {szName = "Th­¬ng Long V¨n C­¬ng- BÝnh[CÊp 4]",nRate= 0.16,tbProp={8,1,24,1,0,0}},
    {szName = "Th­¬ng Long V¨n C­¬ng- §inh [CÊp 2]",nRate= 3.04,tbProp={8,1,32,1,0,0}},
    {szName = "Th­¬ng Long V¨n C­¬ng- §inh [CÊp 3]",nRate= 0.8,tbProp={8,1,33,1,0,0}},
    {szName = "Th­¬ng Long V¨n C­¬ng- §inh [CÊp 4]",nRate= 0.16,tbProp={8,1,34,1,0,0}},
    {szName = "¢m Méc V¨n C­¬ng- Gi¸p [CÊp 2]",nRate= 3.04,tbProp={8,1,42,1,0,0}},
    {szName = "¢m Méc V¨n C­¬ng- Gi¸p [CÊp 3]",nRate= 0.8,tbProp={8,1,43,1,0,0}},
    {szName = "¢m Méc V¨n C­¬ng- Gi¸p [CÊp 4]",nRate= 0.16,tbProp={8,1,44,1,0,0}},
    {szName = "¢m Méc V¨n C­¬ng- Êt [CÊp 2]",nRate= 3.04,tbProp={8,1,52,1,0,0}},
    {szName = "¢m Méc V¨n C­¬ng- Êt [CÊp 3]",nRate= 0.8,tbProp={8,1,53,1,0,0}},
    {szName = "¢m Méc V¨n C­¬ng- Êt [CÊp 4]",nRate= 0.16,tbProp={8,1,54,1,0,0}},
    {szName = "HuyÒn Thñy V¨n C­¬ng- Gi¸p [CÊp 2]",nRate= 3.04,tbProp={8,1,62,1,0,0}},
    {szName = "HuyÒn Thñy V¨n C­¬ng- Gi¸p [CÊp 3]",nRate= 0.8,tbProp={8,1,63,1,0,0}},
    {szName = "HuyÒn Thñy V¨n C­¬ng- Gi¸p [CÊp 4]",nRate= 0.16,tbProp={8,1,64,1,0,0}},
    {szName = "HuyÒn Thñy V¨n C­¬ng- Êt [CÊp 2]",nRate= 3.04,tbProp={8,1,72,1,0,0}},
    {szName = "HuyÒn Thñy V¨n C­¬ng- Êt [CÊp 3]",nRate= 0.8,tbProp={8,1,73,1,0,0}},
    {szName = "HuyÒn Thñy V¨n C­¬ng- Êt [CÊp 4]",nRate= 0.16,tbProp={8,1,74,1,0,0}},
    {szName = "Thiªn L«i V¨n C­¬ng- Gi¸p [CÊp 2]",nRate= 3.04,tbProp={8,1,82,1,0,0}},
    {szName = "Thiªn L«i V¨n C­¬ng- Gi¸p [CÊp 3]",nRate= 0.8,tbProp={8,1,83,1,0,0}},
    {szName = "Thiªn L«i V¨n C­¬ng- Gi¸p [CÊp 4]",nRate= 0.16,tbProp={8,1,84,1,0,0}},
    {szName = "Thiªn L«i V¨n C­¬ng- Êt [CÊp 2]",nRate= 3.04,tbProp={8,1,92,1,0,0}},
    {szName = "Thiªn L«i V¨n C­¬ng- Êt [CÊp 3]",nRate= 0.8,tbProp={8,1,93,1,0,0}},
    {szName = "Thiªn L«i V¨n C­¬ng- Êt [CÊp 4]",nRate= 0.16,tbProp={8,1,94,1,0,0}},
    {szName = "Thiªn L«i V¨n C­¬ng- BÝnh[CÊp 2]",nRate= 3.04,tbProp={8,1,102,1,0,0}},
    {szName = "Thiªn L«i V¨n C­¬ng- BÝnh[CÊp 3]",nRate= 0.8,tbProp={8,1,103,1,0,0}},
    {szName = "Thiªn L«i V¨n C­¬ng- BÝnh[CÊp 4]",nRate= 0.16,tbProp={8,1,104,1,0,0}},
    {szName = "Thiªn L«i V¨n C­¬ng- §inh [CÊp 2]",nRate= 3.04,tbProp={8,1,112,1,0,0}},
    {szName = "Thiªn L«i V¨n C­¬ng- §inh [CÊp 3]",nRate= 0.8,tbProp={8,1,113,1,0,0}},
    {szName = "Thiªn L«i V¨n C­¬ng- §inh [CÊp 4]",nRate= 0.16,tbProp={8,1,114,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- Gi¸p [CÊp 2]",nRate= 3.04,tbProp={8,1,122,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- Gi¸p [CÊp 3]",nRate= 0.8,tbProp={8,1,123,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- Gi¸p [CÊp 4]",nRate= 0.16,tbProp={8,1,124,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- Êt [CÊp 2]",nRate= 3.04,tbProp={8,1,132,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- Êt [CÊp 3]",nRate= 0.8,tbProp={8,1,133,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- Êt [CÊp 4]",nRate= 0.16,tbProp={8,1,134,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- BÝnh[CÊp 2]",nRate= 3.04,tbProp={8,1,142,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- BÝnh[CÊp 3]",nRate= 0.8,tbProp={8,1,143,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- BÝnh[CÊp 4]",nRate= 0.16,tbProp={8,1,144,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- §inh [CÊp 2]",nRate= 3.04,tbProp={8,1,152,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- §inh [CÊp 3]",nRate= 0.8,tbProp={8,1,153,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- §inh [CÊp 4]",nRate= 0.16,tbProp={8,1,154,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- MËu[CÊp 2]",nRate= 3.04,tbProp={8,1,162,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- MËu[CÊp 3]",nRate= 0.8,tbProp={8,1,163,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- MËu[CÊp 4]",nRate= 0.16,tbProp={8,1,164,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- Kû [CÊp 2]",nRate= 3.04,tbProp={8,1,172,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- Kû [CÊp 3]",nRate= 0.8,tbProp={8,1,173,1,0,0}},
    {szName = "Nh­îc Thñy V¨n C­¬ng- Kû [CÊp 4]",nRate= 0.16,tbProp={8,1,174,1,0,0}},
    {szName = "Bµn Long V¨n C­¬ng- Gi¸p [CÊp 2]",nRate= 3.04,tbProp={8,1,182,1,0,0}},
    {szName = "Bµn Long V¨n C­¬ng- Gi¸p [CÊp 3]",nRate= 0.8,tbProp={8,1,183,1,0,0}},
    {szName = "Bµn Long V¨n C­¬ng- Gi¸p [CÊp 4]",nRate= 0.16,tbProp={8,1,184,1,0,0}},
    {szName = "Bµn Long V¨n C­¬ng- Êt [CÊp 2]",nRate= 3.04,tbProp={8,1,192,1,0,0}},
    {szName = "Bµn Long V¨n C­¬ng- Êt [CÊp 3]",nRate= 0.8,tbProp={8,1,193,1,0,0}},
    {szName = "Bµn Long V¨n C­¬ng- Êt [CÊp 4]",nRate= 0.16,tbProp={8,1,194,1,0,0}},
    {szName = "Kim Long V¨n C­¬ng- Gi¸p [CÊp 2]",nRate= 3.04,tbProp={8,1,202,1,0,0}},
    {szName = "Kim Long V¨n C­¬ng- Gi¸p [CÊp 3]",nRate= 0.8,tbProp={8,1,203,1,0,0}},
    {szName = "Kim Long V¨n C­¬ng- Gi¸p [CÊp 4]",nRate= 0.16,tbProp={8,1,204,1,0,0}},
    {szName = "Thanh Méc V¨n C­¬ng- Gi¸p [CÊp 2]",nRate= 3.04,tbProp={8,1,212,1,0,0}},
    {szName = "Thanh Méc V¨n C­¬ng- Gi¸p [CÊp 3]",nRate= 0.8,tbProp={8,1,213,1,0,0}},
    {szName = "Thanh Méc V¨n C­¬ng- Gi¸p [CÊp 4]",nRate= 0.16,tbProp={8,1,214,1,0,0}},
    {szName = "MÞch Thñy V¨n C­¬ng- Gi¸p [CÊp 2]",nRate= 3.04,tbProp={8,1,222,1,0,0}},
    {szName = "MÞch Thñy V¨n C­¬ng- Gi¸p [CÊp 3]",nRate= 0.8,tbProp={8,1,223,1,0,0}},
    {szName = "MÞch Thñy V¨n C­¬ng- Gi¸p [CÊp 4]",nRate= 0.16,tbProp={8,1,224,1,0,0}},
    {szName = "XÝch Háa V¨n C­¬ng- Gi¸p [CÊp 2]",nRate= 3.04,tbProp={8,1,232,1,0,0}},
    {szName = "XÝch Háa V¨n C­¬ng- Gi¸p [CÊp 3]",nRate= 0.8,tbProp={8,1,233,1,0,0}},
    {szName = "XÝch Háa V¨n C­¬ng- Gi¸p [CÊp 4]",nRate= 0.16,tbProp={8,1,234,1,0,0}},
    {szName = "L¹c L«i V¨n C­¬ng- Gi¸p [CÊp 2]",nRate= 3.04,tbProp={8,1,242,1,0,0}},
    {szName = "L¹c L«i V¨n C­¬ng- Gi¸p [CÊp 3]",nRate= 0.8,tbProp={8,1,243,1,0,0}},
    {szName = "L¹c L«i V¨n C­¬ng- Gi¸p [CÊp 4]",nRate= 0.16,tbProp={8,1,244,1,0,0}},
}

function main(nItemIndex)
	if nItemIndex <= 0 then
		return 1
	end
	if PlayerFunLib:CheckFreeBagCell(1, "default") ~= 1 then
		return 1
	end
	
	local nG, nD, nParticulType = GetItemProp(nItemIndex)
	local tbItem = {tbProp = {nG, nD, nParticulType, -1, -1, 0}}
	if PlayerFunLib:CheckItemInBag(tbItem, 1) == 1 then
        tbAwardTemplet:GiveAwardByList(tbJingTieKuangAwardList, "Sö dông Tinh ThiÕt Kho¸ng")
        AddStatData("ronglian_jingtiekuang")
    end
end

