Include("\\script\\lib\\worldlibrary.lua");
Include("\\script\\event\\eventnoel\\EventLibNoel.lua");
function main(nItemIdx)
	if CheckRoom(2,3) ==0 then
		Say("Xin h·y s¾p xÕp l¹i hµnh trang! Nhí ®Ó trèng 6 « trë lªn nhÐ!", 1, "§­îc råi./no");
		return end;
	if GetTask(TASK_EVENT2011_3) >= 2000 then
		Talk(1,"","Nhµ ng­¬i ®· sö dông 2000 c¸i event råi ! §õng cã ng¸o n÷a !");
	return end;
Bonus(3);
RemoveItem(nItemIdx,1)
end