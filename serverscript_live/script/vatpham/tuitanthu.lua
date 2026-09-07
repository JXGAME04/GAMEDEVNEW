Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("\\script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
function main(nItemIdx)
dofile("script/item/tuitanthu.lua");
if CheckRoom(6,6) ==0 then
		Say("Xin h·y s¾p xÕp l¹i hµnh trang! Nhí ®Ó trèng 6x6 « trë lªn nhÐ!", 1, "§­îc råi./no");
		return end;
	if (GetLevel() < 95) then
		 for i = GetLevel(), 95-1 do 
		 AddOwnExp(100000000000)
		 end
	 end
	local nRand=random(1,5);
	-- Earn(1000000);---100v
	local nIndex=AddItem(6,1,2433,1,0,0) ----
	SetPlayerItemLock(nIndex, -2)
	local nIndex=AddItem(0,10,5,nRand,10,0,0,0)
	AddTimeItem(nIndex,60*60*24*7);  -- thêi gia item 7 ngµy
	SetPlayerItemLock(nIndex, -2)
	Msg2Player("Chóc mõng c¸c h¹ nhËn ®­îc Quµ T©n Thñ");
	RemoveItem(nItemIdx,1);
end
