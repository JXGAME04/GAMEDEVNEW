--Author: Fong KiÒu
--Date: 2021
--Funtion: Xa phu Thµnh §«

Include("\\Script\\Global\\station.lua")
Include("\\script\\lib\\lib_server.lua")
function main(sel)
local nCurTime = tonumber(GetLocalDate("%y%m%d%H%M"))
if(nCurTime < SERVER_OPEN) then
Talk(1,"","§óng 19h míi Open Server .")
return 1
end
	if (GetLevel() >= 10) then
		Say("C¸c h¹ muèn ®i ®©u?", 10, 
		"Nh÷ng n¬i ®· ®i qua/WayPointFun", 
		"Nh÷ng thµnh thÞ ®· ®i qua/StationFun",
		"Trë l¹i ®Þa ®iÓm cò/TownPortalFun", 
		"§i ®¶o tÈy tuû/didaotaytuy",
		"§Õn n¬i lµm nhiÖm vô d· tÈu/godatau",
		"§i KiÕm M«n Quan/GoMap995",
		"§i Hoa S¬n c¶nh kü tr­êng(1x)/go_HSBattle",
		"§i ChiÕn tr­êng c«ng thµnh/GoCityWar",
		"Len ban do luyen cong (20 - 90)/LuyenCongFun",
		"Kh«ng ®i/OnCancel")
	else		
		Say("Giang hå hiÓm ¸c ®¹i hiÖp tu luyÖn thªm råi h·y ra ngoµi", 0)
	end

end