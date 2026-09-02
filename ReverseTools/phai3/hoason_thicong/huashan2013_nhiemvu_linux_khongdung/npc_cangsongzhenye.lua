Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\lib\\progressbar.lua")

function CheckTalkFinish(UTask_step)
	if UTask_step == 1 or UTask_step == 3 or UTask_step == 5 or UTask_step == 7 then
		return 1
	else
		return 0
	end
end

--²É¼¯±»´ò¶Ï
function OnBreak (nNpcIndex)
	Msg2Player("Qu¸ tr×nh thu thËp bŞ gi¸n ®o¹n.")
	return 0
end

-- ²É¼¯¹ı³Ì
function GetAward(nNpcIndex, dwNpcIndex)
	if CalcItemCount(3, 6, 1, 4950, -1) > 0 then
		Msg2Player("§¹i hiÖp ®· nhËn ®­îc Th­¬ng Tïng Ch©m DiÖp, kh«ng cÇn lÊy thªm n÷a. ")
		return
	end
	
    	-- ±³°ü¿Õ¼ä²»×ã
    	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang kh«ng ®ñ chç, h·y s¾p xÕp l¹i cßn İt nhÊt mét « trèng råi tíi lÊy.")
        	return 0
    	end

    	-- npc index ·Ç·¨
    	if nNpcIndex == nil then
		Msg2Player("Ng­¬i thu thËp thÊt b¹i.");
        	return 0
    	end

    	-- npc index ·Ç·¨
    	if nNpcIndex <= 0 or GetNpcId(nNpcIndex) ~= dwNpcIndex then
        	Msg2Player("Ng­¬i thu thËp thÊt b¹i.");
        	return 0
    	end

	local UTask_step = GetTask(3482)
	if CheckTalkFinish(UTask_step) ~= 1 then
		SetTask(3482,GetTask(3482)+1)
	end

	if GetTask(3482) == 7 and GetTask(3481) == 20*256+10 then
		SetTask(3481, 20*256+20)
	end

	local nItemIdx = AddItem(6,1,4950,1,0,0)
	SetItemBindState(nItemIdx,-2)
	Msg2Player("NhËn ®­îc Th­¬ng Tïng Ch©m DiÖp. ")
	AddNote("NhËn ®­îc Th­¬ng Tïng Ch©m DiÖp. ")
end

function main()
	local UTask_hs = GetTask(3481)
	if UTask_hs >= 20*256+10 and UTask_hs <= 20*256+40 then
		if CalcItemCount(3, 6, 1, 4950, -1) > 0 then
			Msg2Player("§¹i hiÖp ®· nhËn ®­îc Th­¬ng Tïng Ch©m DiÖp, kh«ng cÇn lÊy thªm n÷a. ")
			return
		end

    		-- ±³°ü¿Õ¼ä²»×ã
    		if CalcFreeItemCellCount() < 1 then
			Msg2Player("Hµnh trang kh«ng ®ñ chç, h·y s¾p xÕp l¹i cßn İt nhÊt mét « trèng råi tíi lÊy.")
        		return 0
    		end

		local nNpcIndex = GetLastDiagNpc()
    		local dwNpcIndex = GetNpcId(nNpcIndex)
		--½ø¶ÈÌõ
    		tbProgressBar:OpenByConfig(1, GetAward, {nNpcIndex, dwNpcIndex}, OnBreak, {nNpcIndex})
	end
end
