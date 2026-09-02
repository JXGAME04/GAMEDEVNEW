Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
-- Include Files
Include("\\script\\dailogsys\\g_dialog.lua")

Include("\\script\\lib\\progressbar.lua")
Include("\\script\\lib\\awardtemplet.lua")

--²É¼¯±»´ò¶Ï
local _OnBreak = function(nItemIndex)
	Msg2Player("Qu¸ tr×nh móc n­íc bŞ gi¸n ®o¹n.")
	return 0
end

local CheckPosition = function(nMapId, nX, nY)
    local tbMapList = {
        [987] = {{1228,1232,3246,3260}},
     }

     for i=1, getn(tbMapList[nMapId]) do
         if nX >= tbMapList[nMapId][i][1] and nX <= tbMapList[nMapId][i][2] 
            and nY >= tbMapList[nMapId][i][3] and nY <= tbMapList[nMapId][i][4] then
             return 1
         end
     end

     return 0
end

-- ²É¼¯¹ı³Ì
local _GetAward = function(nItemIndex)
	--ÎïÆ·ÅĞ¶Ï
    	if CalcItemCount(3, 6, 1, 4945, -1) <= 0 then
        	Msg2Player("Gµo móc n­íc h×nh nh­ bŞ mÊt råi.")
        	return 0
    	end

    	-- ±³°ü¿Õ¼ä²»×ã
    	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang kh«ng ®ñ chç, h·y s¾p xÕp l¹i cßn İt nhÊt 1 « trèng míi cã thÓ móc n­íc.")
        	return 0
    	end

	RemoveItemByIndex(nItemIndex)
        SetTask(3481, 10*256+20)
        local nItemIdx = AddItem(6,1,4946,1,0,0)
	SetItemBindState(nItemIdx,-2)
	Msg2Player("NhËn ®­îc Thanh LiÖt Kho¸ng Thñy.")
	AddNote("NhËn ®­îc Thanh LiÖt Kho¸ng Thñy.")
end 

--Èë¿Úº¯Êı
function main(nItemIndex)
    	local UTask_hs = GetTask(3481)

    	if UTask_hs ~= 10*256+10 and UTask_hs ~= 10*256+20 then
	    	Msg2Player("VËt phÈm nµy ®· thÊt b¹i.")
	    	return nil
    	end

    	local nMapId, nX, nY = GetWorldPos()
    	if %CheckPosition(nMapId, nX, nY) ~= 1 then
        	Msg2Player("H·y ®Õn con suèi gÇn ph¸i Hoa S¬n ®Ó móc n­íc.")
        	return 1
    	end

	if CalcItemCount(3, 6, 1, 4946, -1) > 0 then
		Msg2Player("§¹i hiÖp ®· lÊy ®ñ Thanh LiÖt Kho¸ng Thñy, kh«ng cÇn lÊy thªm n÷a.")
		return 1
	end

    	-- ±³°ü¿Õ¼ä²»×ã
    	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang kh«ng ®ñ chç, h·y s¾p xÕp l¹i cßn İt nhÊt 1 « trèng míi cã thÓ móc n­íc.")
        	return 1;
    	end

    	--½ø¶ÈÌõ
    	tbProgressBar:OpenByConfig(6, %_GetAward, {nItemIndex}, %_OnBreak, {nItemIndex})
    	return 1
end
