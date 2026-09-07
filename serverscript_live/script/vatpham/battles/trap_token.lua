--Author: Fong KiÒu
--Date: 2021
--Function: Item bÉy Tèng Kim

Include("\\script\\header\\forbidmap.lua")
Include("\\script\\lib\\lib_task.lua")

function main(nItemIdx)

	local W,X,Y = GetWorldPos()
	local nMapId = W
	if (checkSJMaps(nMapId) ~= 1 ) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return
	end
	
	local nGoldId,nGen,nDetail,nParti,nLevel,nSeries = GetCBItem(nItemIdx)
	local s_name = GetItemName(nItemIdx)
	local n_curtime = GetGameTime()
	local n_lasttime = GetTaskTemp(TMP_BAYTK)
	
	if (ST_IsRideHorse() == 1) then
		Msg2Player("C­ìi ngùa kh«ng thÓ sö dông vËt phÈm nµy!")
		return
	end
	
	if (n_curtime <= n_lasttime) then
		Msg2Player("1 gi©y sau míi cã thÓ tiÕp tôc sö dông!")
		return
	end
	
	if(nParti == 393) then
		if ( GetLevel() >= 40 and GetLevel() <= 79) then
			CastSkill( 343,7)
			Msg2Player("B¹n ®· sö dông 1 Xuyªn T©m LÖnh")
		elseif ( GetLevel() >= 80 and GetLevel() <= 119) then
			CastSkill( 343,9)
			Msg2Player("B¹n ®· sö dông 1 Xuyªn T©m LÖnh")
		else
			CastSkill( 343,14)
			Msg2Player("B¹n ®· sö dông 1 Xuyªn T©m LÖnh")
		end
	end
	
	if(nParti == 394) then
		if ( GetLevel() >= 40 and GetLevel() <= 79) then
			CastSkill( 303,5)
			Msg2Player("B¹n ®· sö dông 1 §éc Thø LÖnh")
		elseif ( GetLevel() >= 80 and GetLevel() <= 119) then
			CastSkill( 303,8)
			Msg2Player("B¹n ®· sö dông 1 §éc Thø LÖnh")
		else
			CastSkill( 303,12)
			Msg2Player("B¹n ®· sö dông 1 §éc Thø LÖnh")
		end
	end	

	if(nParti == 395) then
		if ( GetLevel() >= 40 and GetLevel() <= 79) then
			CastSkill(345 ,7)
			Msg2Player("B¹n ®· sö dông 1 Hµn B¨ng LÖnh")
		elseif ( GetLevel() >= 80 and GetLevel() <= 119) then
			CastSkill(345 ,9)
			Msg2Player("B¹n ®· sö dông 1 Hµn B¨ng LÖnh")
		else
			CastSkill( 345,14)
			Msg2Player("B¹n ®· sö dông 1 Hµn B¨ng LÖnh")
		end
	end
	
	if(nParti == 396) then
		if ( GetLevel() >= 40 and GetLevel() <= 79) then
			CastSkill( 347,7)
			Msg2Player("B¹n ®· sö dông 1 §Þa Háa LÖnh")
		elseif ( GetLevel() >= 80 and GetLevel() <= 119) then
			CastSkill( 347,9)
			Msg2Player("B¹n ®· sö dông 1 §Þa Háa LÖnh")
		else
			CastSkill( 347,14)
			Msg2Player("B¹n ®· sö dông 1 §Þa Háa LÖnh")
		end
	end
	
	if(nParti == 397) then
		if ( GetLevel() >= 40 and GetLevel() <= 79) then
			CastSkill( 349,7)
			Msg2Player("B¹n ®· sö dông 1 L«i KÝch LÖnh")
		elseif ( GetLevel() >= 80 and GetLevel() <= 119) then
			CastSkill( 349,9)
			Msg2Player("B¹n ®· sö dông 1 L«i KÝch LÖnh")
		else
			CastSkill( 349,14)
			Msg2Player("B¹n ®· sö dông 1 L«i KÝch LÖnh")
		end
	end		
	
	SetTaskTemp(TMP_BAYTK, GetGameTime())
	RemoveItem(nItemIdx,1)
	
end


