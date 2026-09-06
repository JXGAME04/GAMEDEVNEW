Include("\\script\\event\\event_huyhoang_dungdb\\lib_huyhoang.lua")
Include("\\script\\lib\\lib_task.lua")

function main(nItemIdx)
	
	-- dofile("script/item/huihuangzhiguo2.lua")
	
	if (GetLevel() < 80) then
		Talk(1,"","B¹n chØ cã thÓ ¨n qu¶ Huy hoµng S¬ cÊp.")
		return
	elseif (GetLevel() >= 200) then
		Talk(1,"","B¹n chØ cã thÓ ¨n qu¶ Huy hoµng Trung cÊp.")
		return
	end
	local nUsed = GetTask(T_USEHUYHOANG)
	if (nUsed >= MAX_HUYHOANG_PER_DAY) then
		 Talk(1,"", format("H«m nay b¹n ®· ¨n %d qu¶ Huy hoµng, vâ c«ng th¨ng tiÕn qu¸ nhanh dÔ g©y t¸c dông ph¶n nghÞch, ngµy mai h·y tiÕp tôc vËy.",MAX_HUYHOANG_PER_DAY))
		return 
	end
    SetTask(T_USEHUYHOANG,nUsed+1)
    AddSumExp(EXP_QHH_TR)
    RemoveItem(nItemIdx,1)
	nUsed = GetTask(T_USEHUYHOANG)
	Talk(1,"","H«m nay <sex> ®· sö dông "..nUsed.." qu¶ Huy Hoµng")
	
end

