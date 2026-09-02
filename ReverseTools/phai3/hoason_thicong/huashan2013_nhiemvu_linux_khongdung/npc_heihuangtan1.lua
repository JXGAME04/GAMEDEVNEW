Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\lib\\progressbar.lua")

function CheckCanCaiJi(UTask_step)
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
	if CalcItemCount(3, 6, 1, 4960, -1) > 0 then
		Msg2Player("§· thu thËp ®ñ méc liÖu, kh«ng cÇn thu thËp thªm n÷a.. ")
		return
	end

	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang ®· ®Çy, h·y s¾p xÕp l¹i cßn İt nhÊt 1 « trèng råi quay l¹i.")
		return
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

	local nItemIdx = AddItem(6,1,4960,1,0,0)
	SetItemBindState(nItemIdx,-2)
	Msg2Player("NhËn ®­îc 1 bã Hång Méc.")
	SetTask(3487, 20*256)
	Msg2Player("§¹i hiÖp ®· thu thËp ®ñ Hång Méc. ")
	AddNote("§¹i hiÖp ®· thu thËp ®ñ Hång Méc. ")
end

function main()
	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang ®· ®Çy, h·y s¾p xÕp l¹i cßn İt nhÊt 1 « trèng råi quay l¹i.")
		return
	end

	if CalcItemCount(3, 6, 1, 4960, -1) > 0 then
		Msg2Player("§· thu thËp ®ñ méc liÖu, kh«ng cÇn thu thËp thªm n÷a.. ")
		return
	end

	local UTask_jmdz = GetTask(3487)
	if UTask_jmdz >= 10*256 and UTask_jmdz < 30*256 then
		if CheckCanCaiJi(mod(UTask_jmdz, 256)) == 1 then
			local nNpcIndex = GetLastDiagNpc()
    			local dwNpcIndex = GetNpcId(nNpcIndex)
			--½ø¶ÈÌõ
    			tbProgressBar:OpenByConfig(1, GetAward, {nNpcIndex, dwNpcIndex}, OnBreak, {nNpcIndex})
		else
			Msg2Player("§¸nh b¹i thñ hé H¾c DiÖp Linh HÇu míi ®­îc chÆt c©y.")
		end
	end

    	return 1
end
