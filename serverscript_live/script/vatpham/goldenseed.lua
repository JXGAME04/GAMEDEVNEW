Include("\\script\\event\\event_huyhoang_dungdb\\lib_huyhoang.lua")
Include("\\script\\lib\\lib_task.lua");

function main(nItemIdx)
	
	--dofile("script/item/goldenseed.lua")
	
	local nUsed = GetTask(T_USEHATHKIM)
	
	if (nUsed >= MAX_QHK_PER_DAY) then
		Talk(1,"","H«m nay c¸c h¹ ®· sö dông "..nUsed.." qu¶ Hoµng Kim råi, c«ng lùc t¨ng qu¸ nhanh còng sÏ bÞ ph¶n t¸c dông ®ã. §îi ngµy mai råi h·y dïng nhÐ.")
		return 
	end
	if (GetLevel() < 120) then
		Talk(1,"","§¼ng cÊp 120 trë lªn míi ®­îc sö dông Qu¶ Hoµng Kim.")
		return 
	end
    SetTask(T_USEHATHKIM,nUsed+1)
    AddSumExp(EXP_QHK)
	Earn(TIENVAN_QHK)
    RemoveItem(nItemIdx,1)
	nUsed = GetTask(T_USEHATHKIM)
	Msg2SubWorld("<color=yellow>Chóc mõng <color=blue>"..GetName().." <color>¨n qu¶ hoµng kim nhËn ®­îc 50 v¹n l­îng.")
	Talk(1,"","H«m nay <sex> ®· sö dông "..nUsed.." qu¶ Hoµng Kim")
end
