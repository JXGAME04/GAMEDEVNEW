Include("\\script\\lib\\worldlibrary.lua");
Include("\\script\\event\\30thang4\\eventlib.lua");
function main(nItemIdx)
local nRan = random(1,3);
local nID = 0;
local nName = 0;
	if (nRan == 1) then
		nID = TAB_NEED[1][2];
		nName = TAB_NEED[1][3];
	elseif (nRan == 2) then
		nID = TAB_NEED[2][2];
		nName = TAB_NEED[2][3];
	elseif (nRan == 3) then
		nID = TAB_NEED[3][2];
		nName = TAB_NEED[3][3];
	end
	AddItem(6,1,nID,0,0,0,0);
	Msg2Player("§¹i hiÖp nhËn ®­îc mét <color=yellow>"..nName.."<color>.");
	RemoveItem(nItemIdx,1);
end