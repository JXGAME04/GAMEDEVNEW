Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\activitysys\\playerfunlib.lua")

function main(nItemIndex)
	if PlayerFunLib:CheckFreeBagCell(1, "default") ~= 1 then
		return 1
	end

	local tbItem = 
	{
		{szName="Th­¬ng Long V¨n Tinh- Gi¸p [CÊp 1]",tbProp={6,1,4178,1,0,0},nCount=1,nRate=4},
		{szName="Th­¬ng Long V¨n Tinh- Êt [CÊp 1]",tbProp={6,1,4188,1,0,0},nCount=1,nRate=4},
		{szName="Th­¬ng Long V¨n Tinh- BÝnh[CÊp 1]",tbProp={6,1,4198,1,0,0},nCount=1,nRate=4},
		{szName="Th­¬ng Long V¨n Tinh- §inh [CÊp 1]",tbProp={6,1,4208,1,0,0},nCount=1,nRate=4},
		{szName="¢m Méc V¨n Tinh- Gi¸p [CÊp 1]",tbProp={6,1,4218,1,0,0},nCount=1,nRate=4},
		{szName="¢m Méc V¨n Tinh- Êt [CÊp 1]",tbProp={6,1,4228,1,0,0},nCount=1,nRate=4},
		{szName="HuyÒn Thñy V¨n Tinh- Gi¸p [CÊp 1]",tbProp={6,1,4238,1,0,0},nCount=1,nRate=4},
		{szName="HuyÒn Thñy V¨n Tinh- Êt [CÊp 1]",tbProp={6,1,4248,1,0,0},nCount=1,nRate=4},
		{szName="Thiªn L«i V¨n Tinh- Gi¸p [CÊp 1]",tbProp={6,1,4258,1,0,0},nCount=1,nRate=4},
		{szName="Thiªn L«i V¨n Tinh- Êt [CÊp 1]",tbProp={6,1,4268,1,0,0},nCount=1,nRate=4},
		{szName="Thiªn L«i V¨n Tinh- BÝnh[CÊp 1]",tbProp={6,1,4278,1,0,0},nCount=1,nRate=4},
		{szName="Thiªn L«i V¨n Tinh- §inh [CÊp 1]",tbProp={6,1,4288,1,0,0},nCount=1,nRate=4},
		{szName="Nh­îc Thñy V¨n Tinh- Gi¸p [CÊp 1]",tbProp={6,1,4298,1,0,0},nCount=1,nRate=4},
		{szName="Nh­îc Thñy V¨n Tinh- Êt [CÊp 1]",tbProp={6,1,4308,1,0,0},nCount=1,nRate=4},
		{szName="Nh­îc Thñy V¨n Tinh- BÝnh[CÊp 1]",tbProp={6,1,4318,1,0,0},nCount=1,nRate=4},
		{szName="Nh­îc Thñy V¨n Tinh- §inh [CÊp 1]",tbProp={6,1,4328,1,0,0},nCount=1,nRate=4},
		{szName="Nh­îc Thñy V¨n Tinh- MËu[CÊp 1]",tbProp={6,1,4338,1,0,0},nCount=1,nRate=4},
		{szName="Nh­îc Thñy V¨n Tinh- Kû [CÊp 1]",tbProp={6,1,4348,1,0,0},nCount=1,nRate=4},
		{szName="Bµn Long V¨n Tinh #Gi¸p[cÊp 1 ]",tbProp={6,1,4358,1,0,0},nCount=1,nRate=4},
		{szName="Bµn Long V¨n Tinh #Êt [cÊp 1 ]",tbProp={6,1,4368,1,0,0},nCount=1,nRate=4},
		{szName="Kim Long V¨n Tinh- Gi¸p [CÊp 1]",tbProp={6,1,4378,1,0,0},nCount=1,nRate=4},
		{szName="Thanh Méc V¨n Tinh- Gi¸p [CÊp 1]",tbProp={6,1,4388,1,0,0},nCount=1,nRate=4},
		{szName="MÞch Thñy V¨n Tinh #Gi¸p[cÊp 1 ]",tbProp={6,1,4398,1,0,0},nCount=1,nRate=4},
		{szName="XÝch Háa V¨n Tinh- Gi¸p [CÊp 1]",tbProp={6,1,4408,1,0,0},nCount=1,nRate=4},
		{szName="L¹c L«i V¨n Tinh #Gi¸p[cÊp 1 ]",tbProp={6,1,4418,1,0,0},nCount=1,nRate=4},
	}
	tbAwardTemplet:Give(tbItem, 1, {"LongMenBiaoJu", "BiaoJuLibao"})
end