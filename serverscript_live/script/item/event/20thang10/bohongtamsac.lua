Include("\\script\\lib\\worldlibrary.lua");
Include("\\script\\event\\20thang10\\EventLib2010.lua");
function main(nItemIdx)
	if CheckRoom(2,3) ==0 then
		Say("Xin h·y s¾p xÕp l¹i hµnh trang! Nhí ®Ó trèng 6 « trë lªn nhÐ!", 1, "§­îc råi./no");
		return end;
	local nTaskValue = GetTask(TASK_EVENT2011_1);
	if GetNumber(4,nTaskValue,1) >= 2000 then
		Talk(1,"","Nhµ ng­¬i ®· sö dông 2000 c¸i event råi ! §õng cã ng¸o n÷a !");
	return end;
Bonus(1)
RemoveItem(nItemIdx,1)
end