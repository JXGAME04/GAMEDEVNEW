Include("\\script\\task\\newtask\\newtask_head.lua")

function main(nItemIdx)
	local _,_,detail = GetItemProp(nItemIdx)
	local nWorldMaps = nt_getTask(1027) -- ¿´¿´Íæ¼ÒÉíÉÏÓÐ¶àÉÙ¸öÉ½ºÓÉçð¢Í¼
	if (detail == 439) then
		-- ¸øÍæ¼ÒÔö¼Ó100¸öÉ½ºÓÉçð¢Í¼²ÐÆ¬
		nWorldMaps = nWorldMaps + 100;
		nt_setTask(1027,nWorldMaps);
		Msg2Player("B¹n nhËn ®­îc 100 m¶nh b¶n ®å S¬n Hµ X· T¾c! HiÖn t¹i b¹n cã tæng céng"..nWorldMaps.." m¶nh b¶n ®å S¬n Hµ X· T¾c.");
	elseif (detail == 2523) then
		-- ¸øÍæ¼ÒÔö¼Ó1000¸öÉ½ºÓÉçð¢Í¼²ÐÆ¬
		nWorldMaps = nWorldMaps + 1000;
		nt_setTask(1027,nWorldMaps);
		Msg2Player("§¹i hiÖp nhËn ®­îc 1000 m¶nh b¶n ®å s¬n hµ x· t¾c! Tæng céng cã "..nWorldMaps.." m¶nh b¶n ®å S¬n Hµ X· T¾c.");
	end
end

-- [JX1 PORT 16/08/2026] chep tu ban Linux goc; remap particular theo bang item JX1:
-- 440 -> 439 (manh SHXT x100), 2514 -> 2523 (tui 1000 manh) - cung kieu lech id nhu cuon 206->205.
