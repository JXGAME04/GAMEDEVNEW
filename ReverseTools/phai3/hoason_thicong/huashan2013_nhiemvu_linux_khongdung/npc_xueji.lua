Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
--»ªÉ½ÅÉ90¼¶¼¼ÄÜÈÎÎñ Ñª¼£NPC
Include("\\script\\missions\\basemission\\lib.lua")
Include("\\script\\activitysys\\playerfunlib.lua")

function main()
	local UTask_hs_90 = GetTask(3486)
	if UTask_hs_90 == 10 or UTask_hs_90 == 20 or UTask_hs_90 == 30 then
		huashantask()
	elseif UTask_hs_90 > 20 then
		Talk(1, "", "Ng­êi ch¬i: N¬i nµy kh«ng nªn ë l©u.")
	else
		Talk(1, "", "Ng­êi ch¬i: HuyÕt Y thËt kú l¹....")
	end
end

function huashantask()
	if GetGlbValue(1283) ~= 0 then
		Msg2Player("Mét c¸i bÉy ®· ®­îc kÝch ho¹t, mêi kiÓm tra l¹i sau.")
		return
	end

	if GetTask(3486) == 10 then
		SetTask(3486, 20)
	elseif GetTask(3486) == 30 then
		if CalcItemCount(3, 6, 1, 4961, -1) > 0 then
			Talk(1, "", "Ng­êi ch¬i: N¬i nµy kh«ng nªn ë l©u.")
			return
		end
	end

	local tbNpcRose = 
    	{
		szName = "Ng­êi thÇn bÝ ",                                                      	-- npcÓÎÏ·ÖÐÃû×Ö 
        	nLevel = 90,                                                                  -- npcµÈ¼¶
        	nNpcId = 2105,                                                                  -- npc Id
		nMapId = 78,
        	nPosX = 1471 * 32,
        	nPosY = 3564 * 32,
        	nIsboss = 0,                                                                    -- npcÊÇ·ñÎªBoss
        	tbNpcParam = {[1] = 0},                            -- npc²ÎÊý
        	bNoRevive = 1,                                                                  -- npcÊÇ·ñÖØÉú
        	szScriptPath = "\\script\\global\\huashan2013\\npc_battle_heiyiren.lua",        -- npc½Å±¾Â·¾¶
    	}
    	basemission_CallNpc(tbNpcRose)
	SetGlbValue(1283, 1)
	Talk(2, "", "Ng­êi ch¬i: Ng­¬i lµ ai?", "Ng­êi ThÇn BÝ: §õng nhiÒu lêi, h·y ®ì kiÕm!")
end
