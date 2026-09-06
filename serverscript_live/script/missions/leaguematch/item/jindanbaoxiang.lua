Include("\\script\\lib\\awardtemplet.lua")
local tbItem = 
{
	{szName="Kim §¬n Ch©n Kinh", tbProp={6, 1, 2226, 1, 0, 0}, nExpiredTime = 10080},
	{szName="Hång ¶nh B¶o R­¬ng", tbProp={6, 1, 2227, 1, 0, 0} , nBindState = -2},
}
function main()
	
	
	if  CountFreeRoomByWH(1, 1, 1) < 1 then
		Talk(1, "", format("§Ó b¶o ®¶m tµi s¶n cña ®¹i hiÖp, xin h·y ®Ó trèng %d %dx%d hµnh trang", 1, 1, 1))
		return 1
	end
	
	
	tbAwardTemplet:GiveAwardByList(tbItem, "jindanbaoxiang")
	
end