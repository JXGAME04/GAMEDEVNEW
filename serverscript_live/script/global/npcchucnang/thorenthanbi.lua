-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Chuc nang: Thî rÌn ba l¨ng huyÖn

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\log_game\\save_log.lua")

NOW_END_SAY 									= "KÕt thóc ®èi tho¹i./no"


function main(NpcIndex)
	 dofile("script/global/npcchucnang/thorenthanbi.lua")
	SayEx({"<color=yellow><npc><color>: <sex> cÇn g× cã ng©n l­îng ta ®©y s½n lßng",
	"N©ng cÊp Ngùa Hoµng Kim/nangcapnguavip",
	"Thay ®æi ¢n theo hÖ nh©n vËt/nangcapmaycung",
	"Thay ®æi vò khÝ hoµng kim m«n ph¸i/doitemhkmpvukhi",
	NOW_END_SAY})
end


tbAllowCheckOk  = {
	[15]=1,[20]=1,[25]=1,[0]=1,[5]=1,[10]=1,[75]=1,[70]=1,
	[80]=1,[60]=1,[65]=1,[30]=1,[38]=1,[45]=1,[50]=1,[93]=1,
	[95]=1,[100]=1,[110]=1,[115]=1,[120]=1,[125]=1,[130]=1
}
tbAllowOk = {
	[0]=15,[1]=20,[2]=25,[3]=0,[4]=5,[5]=10,[6]=75,[7]=70,
	[8]=80,[9]=60,[10]=65,[11]=30,[12]=38,[13]=45,[14]=50,[15]=93,
	[16]=95,[17]=100,[18]=110,[19]=115,[20]=120,[21]=125,[22]=130
}
function doitemhkmpvukhi()
	OpenGiveBox("§Æt vµo vËt phÈm","- H·y ®¨t vò khÝ hoµng kim m«n ph¸i muèn thay ®æi vµo !\n- TÈy luyÖn tèn 50 TiÒn §ång.","doiresetgoldequipvukhi")
end

function doiresetgoldequipvukhi()
	local i, j, nIndex, kind, genre, detail, parti, level, series, row;
	local nRealIndex = 0
	local nIndexEquip = 0
	local nIndex = 0
	local nCountEquip = 0
	local i = 0
	local checkUpgrade = 0
	local xTD = 50
	local nTienDong = GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom) -- ®Õm sè KNB trong hµnh trang
	
		for i=0,5 do
			for j=0,3 do
				nIndex = GetROItem(ROOAFFAIR,i,j)
				if (nIndex > 0) then
						nRealIndex = nIndex
						kind, genre, detail, parti, level, series, row = GetCBItem(nRealIndex)
					if kind == 3 and row >= 0 and  row <= 139  then -- do pho hkmp
							if tbAllowCheckOk [row] ~= 1 then
								Talk(1, "", "VËt phÈm nµy kh«ng hîp lÖ ®Ó thay ®æi.")
								EndGiveBox()
								return
							end
							
							local nItemLockState = GetPlayerItemIsLock(nIndexEquip)
							local nItemTimeLimit = GetPlayerItemIsTimeLimit(nIndexEquip)
							if nItemTimeLimit > 0  then
								Talk(1,"","Trang bÞ cã thêi gian sö dông kh«ng thÓ thay ®æi")
								EndGiveBox()
								return
							end	
							nCountEquip = nCountEquip + 1
							nIndexEquip = nIndex
				end
			end
		end
	end	
if nTienDong < xTD then
	Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..xTD.." TiÒn §ång.")
	return
end
if nCountEquip > 1 then
	Msg2Player("Xin ®Æt 1 trang bÞ vµo.")
	return
end
	
	if(nTienDong >= xTD) then
			local nRand = random(22)
			local nItemId = tbAllowOk[nRand]
			RemoveItem(nIndexEquip)
			DelItem(0, -1, 6,1 ,4835, -1, -1, pos_equiproom, xTD) -- xo¸ sè knb trong hµnh trang
			local nIndexX = AddItem2(2, 0, nItemId, 0, 0, 0)
			checkUpgrade = 1
			EndGiveBox()
	else
		Talk(1, "", "Hµnh trang kh«ng ®ñ "..xTD.." TiÒn §ång.")
		EndGiveBox()
end

	if(checkUpgrade == 1) then
		Talk(1,"","§· thay ®æi thµnh c«ng ! ")
		else
		Talk(1,"","H·y bá ®óng vËt phÈm quy ®Þnh !")
		EndGiveBox()
	end		
end


function nangcapmaycung()
		local nKvan = 10000000              
		local nTienDong = 200              
		local nNguHanh = 100      
		local nNLChinh = 100         
 		

szLoiNoi = "ChØ cÇn ®Æt Ên vµo.\nC¸c nguyªn liÖu :\n- "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång.\n".."- "..nNguHanh.." Ngò Hµnh Kú Th¹ch. \n".."- "..nNLChinh.." Ngò s¾c long ch©u (Theo hÖ ph¸i).\n TØ lÖ thµnh c«ng: 100%"
OpenGiveBox("§Æt Ngò Hµnh Ên Vµo",szLoiNoi,"batdaunemaycung")

end
function batdaunemaycung()
	local i, j, nIndex, kind, genre, detail, parti, level, series, row;
	local nKvan = 10000000              
	local nTienDong = 200              
	local nNLChinh = 100              
	local nNguHanh = 100              
	local TIENDONG_ID = 4835         
	local NGUHANHKYTHACH_ID = 2134         
	local nRealIndex = 0
	local nRealIndexX = 0
	local nAnID = 0
	local count = 0;
	local nSeries = 0;

	for i = 0, 5 do
		for j = 0, 3 do
			nIndex = GetROItem(ROOAFFAIR, i, j)
			if nIndex > 0 then
				nRealIndex = nIndex
				kind, genre, detail, parti, level, series, row = GetCBItem(nRealIndex)
				if kind == 3 and genre == 0 and detail == 13 and level == 10 then -- Phi phong
					if row >= 7381 and row <= 7385 then
						if GetPlayerItemIsTimeLimit(nRealIndex) > 0 then
							Talk(1, "", "Ngò Hµnh Ên cã thêi gian sö dông, kh«ng thÓ n©ng cÊp.")
							return
						end
						count = count + 1
						nAnID = row
					end
				end
			end
		end
	end
	
			nSeries = GetSeries();
			if count == 0 then
				Msg2Player("Xin ®Æt Ngò Hµnh Ên cÇn n©ng cÊp vµo.")
				return
			elseif count > 1 then
				Msg2Player("ChØ cÇn ®Æt vµo 1 Ngò Hµnh Ên cÇn n©ng cÊp.")
				return
			end
			if ((nAnID == 7381 and nSeries == 0) or (nAnID == 7382 and nSeries == 1) or  (nAnID == 7383 and nSeries == 2)
				or  (nAnID == 7384 and nSeries == 3) or  (nAnID == 7385 and nSeries == 4)) then
				Msg2Player("Nh©n vËt vµ Ên ®· cïng hÖ kh«ng thÓ thay ®æi .")
				Talk(1, "", "Nh©n vËt vµ Ên ®· cïng hÖ kh«ng thÓ thay ®æi .")
			return end
			
			if (nSeries == 0) then
				local n5SLCK  = GetItemCount(0,6,1, 1816, -1, -1, pos_equiproom)
				if n5SLCK < nNLChinh then
					Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Kim.")
					return
				end
			elseif (nSeries == 1) then
				local n5SLCM  = GetItemCount(0,6,1, 1817, -1, -1, pos_equiproom)
				if n5SLCM < nNLChinh then
					Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Méc.")
					return
				end
			elseif (nSeries == 2) then
				local n5SLCT  = GetItemCount(0,6,1, 1818, -1, -1, pos_equiproom)
				if n5SLCT < nNLChinh then
					Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Thñy.")
					return
				end
			elseif (nSeries == 3) then
				local n5SLCH  = GetItemCount(0,6,1, 1819, -1, -1, pos_equiproom)
				if n5SLCH < nNLChinh then
					Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Háa.")
					return
				end
			elseif (nSeries == 4) then
				local n5SLCT  = GetItemCount(0,6,1, 1820, -1, -1, pos_equiproom)
				if n5SLCT < nNLChinh then
					Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Thæ.")
					return
				end
			end

	local nTD  = GetItemCount(0,6,1, TIENDONG_ID, -1, -1, pos_equiproom)
	local nNHKT  = GetItemCount(0,6,1, NGUHANHKYTHACH_ID, -1, -1, pos_equiproom)
	if nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
		return
	end
	if nNHKT < nNguHanh then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNguHanh.." Ngò Hµnh Kú Th¹ch.")
		return
	end

	if GetCash() >= nKvan then
	local nSuccess =  random(1,1)
		if nSuccess == 1 then
			RemoveItem(nRealIndex, 1)
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			DelItem(0, -1, 6,1 ,NGUHANHKYTHACH_ID, -1, -1, pos_equiproom, nNguHanh)
			Pay(nKvan)
			
			if (nSeries == 0) then
				nIndexXz = AddItem2(2, 0,7381, 0, 0, 0)
				DelItem(0, -1, 6,1 ,1816, -1, -1, pos_equiproom, nNLChinh)
			elseif (nSeries == 1) then
				nIndexXz = AddItem2(2, 0,7382, 0, 0, 0)
				DelItem(0, -1, 6,1 ,1817, -1, -1, pos_equiproom, nNLChinh)
			elseif (nSeries == 2) then
				nIndexXz = AddItem2(2, 0,7383, 0, 0, 0)
				DelItem(0, -1, 6,1 ,1818, -1, -1, pos_equiproom, nNLChinh)
			elseif (nSeries == 3) then
				nIndexXz = AddItem2(2, 0,7384, 0, 0, 0)
				DelItem(0, -1, 6,1 ,1819, -1, -1, pos_equiproom, nNLChinh)
			elseif (nSeries == 4) then
				nIndexXz = AddItem2(2, 0,7385, 0, 0, 0)
				DelItem(0, -1, 6,1 ,1820, -1, -1, pos_equiproom, nNLChinh)
			end
			SetPlayerItemLock(nIndexXz, -2)
			s_name = GetItemName(nIndexXz)
			EndGiveBox()

			Msg2Player("Chóc b¹n n©ng cÊp Ngò Hµnh Ên thµnh c«ng!")
			Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color>thay ®æi Ngò Hµnh Ên thµnh c«ng, nhËn ®­îc <color=yellow> "..s_name.." .")
		end
	else
		Msg2Player("Kh¸ch quan ch­a mang ®ñ "..(nKvan/10000).." v¹n l­îng.")
	end
end

function nangcapnguavip()
		local nKvan = 50000000              
		local nTienDong = 100             

szLoiNoi = "ChØ cÇn ®Æt 2 Ngùa cÇn n©ng cÊp vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n - "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång. \n TØ lÖ thµnh c«ng: 100%"
OpenGiveBox("§Æt Ngùa Muèn N©ng CÊp Vµo",szLoiNoi,"batdaunangcapngua")

end
function batdaunangcapngua()
	local i, j, nIndex, kind, genre, detail, parti, level, series, row;
	local nKvan = 50000000              
	local nTienDong = 100               
	local TIENDONG_ID = 4835         
	local nRealIndex = 0
	local nRealIndexX = 0
	local nPhiPhongID = 0
	local count = 0;
	local tbNguaHoangKim = {}   
	local nParti1 = -1
	local nParti2 = -1
	local nLevel1 = -1
	local nLevel2 = -1
	local nSeries = 0;
	local nNLChinh = 200                      
	
	
	for i = 0, 5 do
		for j = 0, 3 do
			nIndex = GetROItem(ROOAFFAIR, i, j)
			if nIndex > 0 then
				nRealIndex = nIndex
				kind, genre, detail, parti, level, series, row = GetCBItem(nRealIndex)
				if  genre == 0 and detail == 10   then 
					if (parti == 12 and level >= 9) or (parti == 13 and level <= 2) then
						if GetPlayerItemIsLock(nRealIndex) == -2 then
							Talk(1, "", "Ngùa ®ang ®­îc kho¸ b¶o hiÓm, kh«ng thÓ n©ng cÊp.")
							return
						end
						if GetPlayerItemIsTimeLimit(nRealIndex) > 0 then
							Talk(1, "", "Ngùa cã thêi gian sö dông, kh«ng thÓ n©ng cÊp.")
							return
						end
						count = count + 1
						tbNguaHoangKim[count] = nIndex 
						
						if count == 1 then
							nParti1 = parti
							nLevel1 = level
						elseif count == 2 then
							nParti2 = parti
							nLevel2 = level
						end
					end
				end
			end
		end
	end
			nSeries = GetSeries();
			if (nSeries == 0) then
				local n5SLCK  = GetItemCount(0,6,1, 1816, -1, -1, pos_equiproom)
				if n5SLCK < nNLChinh then
					Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Kim.")
					return
				end
			elseif (nSeries == 1) then
				local n5SLCM  = GetItemCount(0,6,1, 1817, -1, -1, pos_equiproom)
				if n5SLCM < nNLChinh then
					Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Méc.")
					return
				end
			elseif (nSeries == 2) then
				local n5SLCT  = GetItemCount(0,6,1, 1818, -1, -1, pos_equiproom)
				if n5SLCT < nNLChinh then
					Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Thñy.")
					return
				end
			elseif (nSeries == 3) then
				local n5SLCH  = GetItemCount(0,6,1, 1819, -1, -1, pos_equiproom)
				if n5SLCH < nNLChinh then
					Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Háa.")
					return
				end
			elseif (nSeries == 4) then
				local n5SLCT  = GetItemCount(0,6,1, 1820, -1, -1, pos_equiproom)
				if n5SLCT < nNLChinh then
					Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nNLChinh.." Ngò s¾c long ch©u Thæ.")
					return
				end
			end
			
	if count ~= 2 then
		Msg2Player("Xin ®Æt 2 ngùa cïng lo¹i ®Ó n©ng cÊp .")
		return
	end

	if (nParti1 ~= nParti2) or (nLevel1 ~= nLevel2) then
		Msg2Player("H·y ®Æt 2 ngùa cïng lo¹i ®Ó n©ng cÊp.")
		return
	end


	local nTD  = GetItemCount(0,6,1, TIENDONG_ID, -1, -1, pos_equiproom)

	if nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
		return
	end
	
	if GetCash() >= nKvan then
		local nSuccess =  random(1, 1)
		if nSuccess == 1 then
			for i = 1, count do
				RemoveItem(tbNguaHoangKim[i], 1)
			end
			DelItem(0, -1, 6,1 ,TIENDONG_ID, -1, -1, pos_equiproom, nTienDong)
			Pay(nKvan)
			if (nSeries == 0) then
				DelItem(0, -1, 6,1 ,1816, -1, -1, pos_equiproom, nNLChinh)
			elseif (nSeries == 1) then
				DelItem(0, -1, 6,1 ,1817, -1, -1, pos_equiproom, nNLChinh)
			elseif (nSeries == 2) then
				DelItem(0, -1, 6,1 ,1818, -1, -1, pos_equiproom, nNLChinh)
			elseif (nSeries == 3) then
				DelItem(0, -1, 6,1 ,1819, -1, -1, pos_equiproom, nNLChinh)
			elseif (nSeries == 4) then
				DelItem(0, -1, 6,1 ,1820, -1, -1, pos_equiproom, nNLChinh)
			end
			local nIndexX = AddItem(0,10,nParti1,nLevel1 + 1,0,0,0)
			local s_name = GetItemName(nIndexX)
			EndGiveBox()

			Msg2Player("Chóc b¹n n©ng cÊp Ngùa thµnh c«ng!")
			Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>n©ng cÊp Ngùa thµnh c«ng, nhËn ®­îc <color> "..s_name.." .")
		end
	else
		Msg2Player("Kh¸ch quan ch­a mang ®ñ "..(nKvan/10000).." v¹n l­îng.")
	end
end

function no()
	
end








