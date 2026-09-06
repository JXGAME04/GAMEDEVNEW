--Author: Fong KiÒu
--Date: 2021
--Function: T©m T©m t­¬ng ¸nh phï

Include("\\script\\header\\forbidmap.lua")

function use_heart()
	local W,X,Y = GetWorldPos()
	local nSubWorldID = W
	
	if checkYDBZMaps(nSubWorldID) == 1 then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return 0
	end

	if (nSubWorldID >= 375 and nSubWorldID <= 386) then
		Msg2Player("Xin lçi! B¹n hiÖn ë khu vùc ®Æc biÖt, kh«ng thÓ sö dôngT©m T©m T­¬ng ¸nh Phï!")
		return 0
	end
	
	if (nSubWorldID >= 416 and nSubWorldID <= 511) then
		Msg2Player("Xin lçi! B¹n hiÖn ë khu vùc ®Æc biÖt, kh«ng thÓ sö dôngT©m T©m T­¬ng ¸nh Phï!")
		return 0
	end
	
	if (nSubWorldID == 44 or nSubWorldID == 197 or nSubWorldID == 208 or nSubWorldID == 209 or nSubWorldID == 210 or nSubWorldID == 211 or nSubWorldID == 212 or (nSubWorldID >= 213 and nSubWorldID <= 223)	or nSubWorldID == 336 or nSubWorldID == 341 or nSubWorldID == 342	or nSubWorldID == 175	or nSubWorldID == 337	or nSubWorldID == 338	or nSubWorldID == 339 or ( nSubWorldID >= 387 and  nSubWorldID <= 395 ) )then 
		Msg2Player("Xin lçi! B¹n hiÖn ë khu vùc ®Æc biÖt, kh«ng thÓ sö dôngT©m T©m T­¬ng ¸nh Phï!")
		return 0
	end

	if (checkHFMaps(nSubWorldID) == 1) then
		Msg2Player("Xin lçi! B¹n hiÖn ë khu vùc ®Æc biÖt, kh«ng thÓ sö dôngT©m T©m T­¬ng ¸nh Phï!")
		return 0
	end

	if (checkBWMaps(nSubWorldID) == 1) then
		Msg2Player("Xin lçi! B¹n hiÖn ë khu vùc ®Æc biÖt, kh«ng thÓ sö dôngT©m T©m T­¬ng ¸nh Phï!")
		return 0
	end
	
	if checkActMaps(nSubWorldID) == 1 then
		Msg2Player("Xin lçi! B¹n hiÖn ë khu vùc ®Æc biÖt, kh«ng thÓ sö dôngT©m T©m T­¬ng ¸nh Phï!")
		return 0
	end
	
	if (checkZQMaps(nSubWorldID) == 1) then
		Msg2Player("Xin lçi! B¹n hiÖn ë khu vùc ®Æc biÖt, kh«ng thÓ sö dôngT©m T©m T­¬ng ¸nh Phï!")
		return 0
	end
	
	if (checkNEWPRACTICEMaps(nSubWorldID) == 1) then
		Msg2Player("Xin lçi! B¹n hiÖn ë khu vùc ®Æc biÖt, kh«ng thÓ sö dôngT©m T©m T­¬ng ¸nh Phï!")
		return 0
	end

	Lover = GetMateName()
	if (Lover == "") then 
		Msg2Player("Ng­êi ch­a kÕt h«n kh«ng thÓ sö dông!")
		return 0
	end
	local nPlayer = FindPlayer(Lover)
	if(nPlayer <= 0) then
		Say("Xin lçi! "..Lover.." t¹m thêi kh«ng Online",0)
		return 0
	end
	local tempid = PlayerIndex
	PlayerIndex = nPlayer
	W,X,Y = GetWorldPos()
	nSubWorldID = W
	local nFightMode = GetFightState()
	PlayerIndex = tempid
	if (nSubWorldID > 70000) then
		Msg2Player("Xin lçi! Phèi ngÉu cña b¹n ®ang ë khu vùc ®Æc biÖt kh«ng thÓ dÉn vµo.")
		return 0
	end
	
	if (nSubWorldID >= 375 and nSubWorldID <= 386) then
		Msg2Player("Xin lçi! Phèi ngÉu cña b¹n ®ang ë khu vùc ®Æc biÖt kh«ng thÓ dÉn vµo.")
		return 0
	end
	
	if (nSubWorldID >= 416 and nSubWorldID <= 511) then
		Msg2Player("Xin lçi! Phèi ngÉu cña b¹n ®ang ë khu vùc ®Æc biÖt kh«ng thÓ dÉn vµo.")
		return 0
	end
	
	if (nSubWorldID == 44 or  nSubWorldID == 197 or nSubWorldID == 208 or nSubWorldID == 209 or nSubWorldID == 210 or nSubWorldID == 211 or nSubWorldID == 212 or (nSubWorldID >= 213 and nSubWorldID <= 223) or nSubWorldID == 336 or nSubWorldID == 341 or nSubWorldID == 342	or nSubWorldID == 175	or nSubWorldID == 337	or nSubWorldID == 338	or nSubWorldID == 339 or ( nSubWorldID >= 387 and  nSubWorldID <= 395 )) then 
		Msg2Player("Xin lçi! Phèi ngÉu cña b¹n ®ang ë khu vùc ®Æc biÖt kh«ng thÓ dÉn vµo.")
		return 0
	end

	if checkActMaps(nSubWorldID) == 1 then
		Msg2Player("Xin lçi! Phèi ngÉu cña b¹n ®ang ë khu vùc ®Æc biÖt kh«ng thÓ sö dông T©m T©m T­¬ng ¸nh Phï.")
		return 0
	end
	
	if (checkBWMaps(nSubWorldID) == 1) then
		Msg2Player("Xin lçi! Phèi ngÉu cña b¹n ®ang ë khu vùc ®Æc biÖt kh«ng thÓ sö dông T©m T©m T­¬ng ¸nh Phï.")
		return 0
	end
	
	if (checkZQMaps(nSubWorldID) == 1) then
		Msg2Player("Xin lçi! Phèi ngÉu cña b¹n ®ang ë khu vùc ®Æc biÖt kh«ng thÓ sö dông T©m T©m T­¬ng ¸nh Phï.")
		return 0
	end
	
	if (checkTONGMaps(SubWorld) == 1) then
		Msg2Player("Xin lçi! Phèi ngÉu cña b¹n ®ang ë khu vùc ®Æc biÖt kh«ng thÓ sö dông T©m T©m T­¬ng ¸nh Phï.")
		return 0
	end
	
	if (checkNEWPRACTICEMaps(nSubWorldID) == 1) then
		Msg2Player("Xin lçi! Phèi ngÉu cña b¹n ®ang ë khu vùc ®Æc biÖt kh«ng thÓ sö dông T©m T©m T­¬ng ¸nh Phï.")
		return 0
	end

	if checkYDBZMaps(nSubWorldID) == 1 then
		Msg2Player("Xin lçi! Phèi ngÉu cña b¹n ®ang ë khu vùc ®Æc biÖt kh«ng thÓ sö dông T©m T©m T­¬ng ¸nh Phï.")
		return 0
	end
	
	if (checkHFMaps(nSubWorldID) == 1) then
		Msg2Player("Xin lçi! Phèi ngÉu cña b¹n ®ang ë khu vùc ®Æc biÖt kh«ng thÓ sö dông T©m T©m T­¬ng ¸nh Phï.")
		return 0
	end
	
	NewWorld(nSubWorldID,X,Y)
	SetFightState(nFightMode)
	return 1
end
