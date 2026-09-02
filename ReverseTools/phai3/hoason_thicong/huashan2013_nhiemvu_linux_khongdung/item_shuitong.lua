Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
-- Include Files
Include("\\script\\dailogsys\\g_dialog.lua")

Include("\\script\\lib\\progressbar.lua")
Include("\\script\\lib\\awardtemplet.lua")

function CheckTalkFinish(UTask_step)
	if UTask_step == 2 or UTask_step == 3 or UTask_step == 6 or UTask_step == 7 then
		return 1
	else
		return 0
	end
end

--²É¼¯±»´ò¶Ï
local _OnBreak = function(nItemIndex)
	Msg2Player("Qu¸ tr×nh móc n­íc bÞ gi¸n ®o¹n.")
	return 0
end

-- ²É¼¯¹ý³Ì
local _GetAward = function(nItemIndex)
	--ÎïÆ·ÅÐ¶Ï
    	if CalcItemCount(3, 6, 1, 4947, -1) <= 0 then
        	Msg2Player("Thïng n­íc h×nh nh­ bÞ mÊt råi.")
        	return 0
    	end

    	-- ±³°ü¿Õ¼ä²»×ã
    	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang kh«ng ®ñ chç, h·y s¾p xÕp l¹i cßn Ýt nhÊt 1 « trèng míi cã thÓ móc n­íc.")
        	return 0
    	end

	RemoveItemByIndex(nItemIndex)

	local UTask_step = GetTask(3482)
	if CheckTalkFinish(UTask_step) ~= 1 then
		SetTask(3482,GetTask(3482)+2)
	end

	if GetTask(3482) == 7 and GetTask(3481) == 20*256+10 then
		SetTask(3481, 20*256+20)
	end

        local nItemIdx = AddItem(6,1,4948,1,0,0)
	SetItemBindState(nItemIdx,-2)
	Msg2Player("NhËn ®­îc Dung TuyÕt Chi Thñy.")
	AddNote("NhËn ®­îc Dung TuyÕt Chi Thñy.")
end

local CheckPosition = function(nMapId, nX, nY)
    local tbMapList = {
        [987] = {{1358,1365,2909,2919}},
     }

     for i=1, getn(tbMapList[nMapId]) do
         if nX >= tbMapList[nMapId][i][1] and nX <= tbMapList[nMapId][i][2] 
            and nY >= tbMapList[nMapId][i][3] and nY <= tbMapList[nMapId][i][4] then
             return 1
         end
     end

     return 0
end

--Èë¿Úº¯Êý
function main(nItemIndex)
    	local UTask_hs = GetTask(3481)

    	if UTask_hs < 20*256+10 or UTask_hs > 20*256+40 then
	    	Msg2Player("VËt phÈm nµy ®· thÊt b¹i.")
	    	return nil
    	end

	local nMapId, nX, nY = GetWorldPos()
    	if %CheckPosition(nMapId, nX, nY) ~= 1 then
        	Msg2Player("H·y ®Õn thung lòng ph¸i Hoa S¬n móc n­íc.")
        	return 1
    	end

	if CalcItemCount(3, 6, 1, 4948, -1) > 0 then
		Msg2Player("§¹i hiÖp ®· lÊy ®ñ Dung TuyÕt Chi Thñy, kh«ng cÇn lÊy thªm n÷a.")
		return 1
	end

    	-- ±³°ü¿Õ¼ä²»×ã
    	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang kh«ng ®ñ chç, h·y s¾p xÕp l¹i cßn Ýt nhÊt 1 « trèng míi cã thÓ móc n­íc.")
        	return 1
    	end

    	--½ø¶ÈÌõ
    	tbProgressBar:OpenByConfig(6, %_GetAward, {nItemIndex}, %_OnBreak, {nItemIndex})
    	return 1
end
