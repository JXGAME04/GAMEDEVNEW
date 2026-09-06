Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\activitysys\\playerfunlib.lua")

function main(nItemIndex)
	if PlayerFunLib:CheckFreeBagCell(1, "default") ~= 1 then
		return 1
	end

	local tbItem = 
	{
		{szName="Kho¸i M· Gia Tiªn",tbProp={6,1,4775,1,0,0},nCount=1,nRate=30, nBindState=-2},
		{szName="ThiÕt Xa MËt Bè",tbProp={6,1,4777,1,0,0},nCount=1,nRate=30, nBindState=-2},
		{szName="Kiªn BÊt Kh¶ Táa",tbProp={6,1,4776,1,0,0},nCount=1,nRate=30, nBindState=-2},
		{szName="Ho¸n Tiªu ChØ",tbProp={6,1,4772,1,0,0},nCount=1,nRate=10, nBindState=-2},
	}
	tbAwardTemplet:Give(tbItem, 1, {"LongMenBiaoJu", "BiaoJuLibao"})
end