Include("\\script\\lib\\worldlibrary.lua");
Include("\\script\\event\\30thang4\\eventlib.lua");
FREECELL_EVENT = 6
function main(nItemIdx)
	if CalcFreeItemCellCount() < FREECELL_EVENT then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ "..FREECELL_EVENT.." « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return
	end	
	local nTaskValue = GetTask(TASK_EVENT3004_2);
	if nTaskValue >= 2000 then
		Talk(1,"","Nhµ ng­¬i ®· sö dông 2000 c¸i event råi ! §õng cã ng¸o n÷a !");
	return end;
Bonus(2);
RemoveItem(nItemIdx,1)
end