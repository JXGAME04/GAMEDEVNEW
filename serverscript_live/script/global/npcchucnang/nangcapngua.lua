--Script n©ng cÊp ngùa by Mr Chinh
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_vatpham.lua")

		ITEM1 = 3846 ---thøc ¨n ngùa trong eventitem.txt
		ITEM2 = 4728 -- can cot don
		-- nKvan = 50000
		-- nTienDong = 1000
		-- nSL1 = 100
		-- nSL2 = 100
		
	tb_PUTITEM = {
		{"Thøc ¨n cho ngùa ", 10},
		{"LuyÖn Cèt §¬n ", 10},
		{"TiÒn v¹n ", 10000},
		{"TiÒn ®ång ", 100}
	}
	
function main(NpcIndex)
 dofile("script/global/npcchucnang/nangcapngua.lua")
if GetSex() == 1 then
		szGioiTinh = "C« N­¬ng"
	else szGioiTinh = "§¹i HiÖp"
	end
	szCauNoi = "Xin chµo "..szGioiTinh.." >=bclr=red>"..GetName()..">=bclr>. Chóc mét ngµy tèt lµnh."

Say(szCauNoi,5,
"N©ng cÊp ngùa th­êng lªn ChiÕu D¹/nangcap1",
"N©ng cÊp ChiÕu D¹ lªn Phi V©n/nangcap2",
"N©ng cÊp Phi V©n lªn B«n Tiªu/nangcap3",
"N©ng cÊp B«n Tiªu lªn Phiªn Vò/nangcap4",
"N©ng cÊp Phiªn Vò lªn Siªu Quang/nangcap5",
-- "N©ng cÊp Siªu Quang lªn Siªu Quang/nangcap6",
-- "N©ng cÊp Siªu Quang lªn Kim Tinh Hæ V­¬ng/nangcap7",
-- "N©ng cÊp Kim Tinh Hæ V­¬ng lªn H·n HuyÕt Long C©u/nangcap8",
-- "N©ng cÊp H·n HuyÕt Long C©u lªn S­ Tö/nangcap9",
"KÕt thóc ®èi tho¹i/no"
)
end

function nangcap1()
		local nKvan = 2500000
		local nTienDong = 10
		local nSL1 = 50
		local nSL2 = 50
szLoiNoi = "ChØ cÇn ®Æt ChiÕn m· cÇn n©ng cÊp vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n - "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång. \n".."- Thøc ¨n cho ng­a: "..nSL1.."\n- LuyÖn cèt ®¬n: "..nSL2.."\n TØ lÖ thµnh c«ng: 100%"
OpenGiveBox("§Æt Ngùa Muèn N©ng CÊp Vµo",szLoiNoi,"batdaunangcap1")

end
function batdaunangcap1()
	local i,j, nIndex,kind,genre,detail,parti,level,series,row;
	local nKvan = 2500000
	local nTienDong = 10
	local nSL1 = 50
	local nSL2 = 50
	local nRealIndex = 0;
	local nRealIndexX = 0;
	local nNL1,nNL2, nTD = 0,0,0
	local count = 0;
	for i=0,5 do
				for j=0,3 do
					nIndex = GetROItem(ROOAFFAIR,i,j)
					if (nIndex > 0) then
						nRealIndex = nIndex
						kind,genre,detail,parti,level,series,row = GetCBItem(nRealIndex)
						if (genre == 0 and detail == 10 and parti < 5 and level == 10) then
						count = count + 1
						nRealIndexX = nIndex
						if (GetPlayerItemIsLock(nRealIndexX) == -2) then
							Talk(1,"","ChiÕn m· ®­îc kho¸ b¶o hiÓm vÜnh viÔn kh«ng thÓ n©ng cÊp")
							return
						end
						if (GetPlayerItemIsTimeLimit(nRealIndexX) > 0) then
							Talk(1,"","Trang bÞ cã thêi gian sö dông kh«ng thÓ n©ng cÊp")
							return
						end	
					end
				end
			end
		end
	
	 nNL1  = GetItemCount(0,6,1, ITEM1, -1, -1, pos_equiproom)
	 nNL2  = GetItemCount(0,6,1, ITEM2, -1, -1, pos_equiproom)
	 nTD  = GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom)
	if  nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
			return
		end
	if  nNL1 < nSL1 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSL1.." Thøc ¨n cho ngùa.")
			return
	end
		if  nNL2 < nSL2 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSL2.." LuyÖn Cèt §¬n .")
			return
		end
	if count == 0 then
		Msg2Player("Xin ®Æt chiÕn m· cÇn n©ng cÊp vµo")
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 chiÕn m· CÊp 70")
	elseif count == 1 then
		if GetCash() >= nKvan  and GetItemCount(0,6,1, ITEM1, -1, -1, pos_equiproom) >= nSL1 and GetItemCount(0,6,1, ITEM2, -1, -1, pos_equiproom) >= nSL2 and GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom) >= nTienDong  then
			nTiLeThanhCong = random(1,1)
			if nTiLeThanhCong == 1 then
				RemoveItem(nRealIndexX,1);
				DelItem(0, -1, 6,1 ,ITEM1, -1, -1, pos_equiproom, nSL1) -- xo¸ 
				DelItem(0, -1, 6,1 ,ITEM2, -1, -1, pos_equiproom, nSL2) -- xo¸ 
				DelItem(0, -1, 6,1 ,4835, -1, -1, pos_equiproom, nTienDong) -- xo¸ 
				Pay(nKvan);
				AddItem(0,10,5,10,0,0,0) -- thµnh c«ng ngùa chiÕu d¹
				EndGiveBox()
				Msg2Player("Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 ngùa ChiÕu D¹ Ngäc S­ Tö")
				Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc<color> 1  ChiÕu D¹ Ngäc S­ Tö ")
				Talk(1,"","Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 ngùa ChiÕu D¹ Ngäc S­ Tö")
			else
				Pay(nKvan);
				DelItem(0, -1, 6,1 ,ITEM1, -1, -1, pos_equiproom, nSL1) -- xo¸ 
				DelItem(0, -1, 6,1 ,ITEM2, -1, -1, pos_equiproom, nSL2) -- xo¸ 
				DelItem(0, -1, 6,1 ,4835, -1, -1, pos_equiproom, nTienDong) -- xo¸ 
				EndGiveBox()
				Msg2Player("Chia buån cïng ®¹o h÷u. LÇn n©ng cÊp nµy thÊt b¹i, tæn thÊt "..(nKvan/10000).." v¹n l­îng vµ "..nTienDong.." tiÒn ®ång.");
			end
		else	
			Msg2Player("B¹n cÇn chuÈn bÞ ®ñ nguyªn liÖu míi cã thÓ n©ng cÊp")
		end
	end
end

function nangcap2()
local nKvan = 5000000
local nTienDong = 30
local nSL1 = 100
local nSL2 = 100
szLoiNoi = "ChØ cÇn ®Æt ChiÕn m· cÇn n©ng cÊp vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n - "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång. \n".."- Thøc ¨n cho ng­a: "..nSL1.."\n- LuyÖn cèt ®¬n: "..nSL2.."\n TØ lÖ thµnh c«ng: 80%"
OpenGiveBox("§Æt Ngùa Muèn N©ng CÊp Vµo",szLoiNoi,"batdaunangcap2")

end
function batdaunangcap2()
	local i,j, nIndex,kind,genre,detail,parti,level,series,row;
	local nKvan = 5000000
	local nTienDong = 30
	local nSL1 = 100
	local nSL2 = 100
	local nRealIndex = 0;
	local nRealIndexX = 0;
	local nNL1,nNL2, nTD = 0,0,0
	local count = 0;
		for i=0,5 do
				for j=0,3 do
					nIndex = GetROItem(ROOAFFAIR,i,j)
					if (nIndex > 0) then
						nRealIndex = nIndex
						kind,genre,detail,parti,level,series,row = GetCBItem(nRealIndex)
						if (genre == 0 and detail == 10 and parti == 5 and (level == 10 or level == 5)) then
						count = count + 1
						nRealIndexX = nIndex
						if (GetPlayerItemIsLock(nRealIndexX) == -2) then
							Talk(1,"","ChiÕn m· ®­îc kho¸ b¶o hiÓm vÜnh viÔn kh«ng thÓ n©ng cÊp")
							return
						end
						if (GetPlayerItemIsTimeLimit(nRealIndexX) > 0) then
							Talk(1,"","Trang bÞ cã thêi gian sö dông kh«ng thÓ n©ng cÊp")
							return
						end	
					end
				end
			end
		end
	
	 nNL1  = GetItemCount(0,6,1, ITEM1, -1, -1, pos_equiproom)
	 nNL2  = GetItemCount(0,6,1, ITEM2, -1, -1, pos_equiproom)
	 nTD  = GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom)
	if  nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
			return
		end
	if  nNL1 < nSL1 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSL1.." Thøc ¨n cho ngùa.")
			return
	end
		if  nNL2 < nSL2 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSL2.." LuyÖn Cèt §¬n .")
			return
		end
	if count == 0 then
		Msg2Player("Xin ®Æt chiÕn m· cÇn n©ng cÊp vµo")
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 chiÕn m· ChiÕu D¹")
	elseif count == 1 then
		if GetCash() >= nKvan  and GetItemCount(0,6,1, ITEM1, -1, -1, pos_equiproom) >= nSL1 and GetItemCount(0,6,1, ITEM2, -1, -1, pos_equiproom) >= nSL2 and GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom) >= nTienDong  then
			nTiLeThanhCong = random(0,1)
			if nTiLeThanhCong == 1 then
				RemoveItem(nRealIndexX,1);
				DelItem(0, -1, 6,1 ,ITEM1, -1, -1, pos_equiproom, nSL1) -- xo¸ 
				DelItem(0, -1, 6,1 ,ITEM2, -1, -1, pos_equiproom, nSL2) -- xo¸ 
				DelItem(0, -1, 6,1 ,4835, -1, -1, pos_equiproom, nTienDong) -- xo¸ 
				Pay(nKvan);
				AddItem(0,10,8,10,0,0,0)
				EndGiveBox()
				Msg2Player("Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 ngùa Phi V©n ThÇn m·")
				Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc<color> 1  Phi V©n ThÇn m· ")
				Talk(1,"","Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 chiÕn m· Phi V©n ThÇn m·")
			else
				Pay(nKvan);
				DelItem(0, -1, 6,1 ,ITEM1, -1, -1, pos_equiproom, nSL1) -- xo¸ 
				DelItem(0, -1, 6,1 ,ITEM2, -1, -1, pos_equiproom, nSL2) -- xo¸ 
				DelItem(0, -1, 6,1 ,4835, -1, -1, pos_equiproom, nTienDong) -- xo¸ 
				EndGiveBox()
				Msg2Player("Chia buån cïng ®¹o h÷u. LÇn n©ng cÊp nµy thÊt b¹i, tæn thÊt "..(nKvan/10000).." v¹n l­îng vµ "..nTienDong.." tiÒn ®ång.");
			end
		else	
			Msg2Player("B¹n cÇn chuÈn bÞ ®ñ nguyªn liÖu míi cã thÓ n©ng cÊp")
		end
	end
end

function nangcap3()
	local nKvan = 10000000
	local nTienDong = 50
	local nSL1 = 300
	local nSL2 = 300
szLoiNoi = "ChØ cÇn ®Æt ChiÕn m· cÇn n©ng cÊp vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n - "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång. \n".."- Thøc ¨n cho ng­a: "..nSL1.."\n- LuyÖn cèt ®¬n: "..nSL2.."\n TØ lÖ thµnh c«ng: 60%"
OpenGiveBox("§Æt Ngùa Muèn N©ng CÊp Vµo",szLoiNoi,"batdaunangcap3")
end

function batdaunangcap3()
	local i,j, nIndex,kind,genre,detail,parti,level,series,row;
	local nKvan = 10000000
	local nTienDong = 50
	local nSL1 = 300
	local nSL2 = 300
	local nRealIndex = 0;
	local nRealIndexX = 0;
	local count = 0;
	for i=0,5 do
		for j=0,3 do
			nIndex = GetROItem(ROOAFFAIR,i,j)
			if (nIndex > 0) then
				nRealIndex = nIndex
				kind,genre,detail,parti,level,series,row = GetCBItem(nRealIndex)
				if (genre == 0 and detail == 10 and parti == 8 and level == 10) then
				count = count + 1
				nRealIndexX = nIndex
				if (GetPlayerItemIsLock(nRealIndexX) == -2) then
					Talk(1,"","ChiÕn m· ®­îc kho¸ b¶o hiÓm vÜnh viÔn kh«ng thÓ n©ng cÊp")
					return
				end
				if (GetPlayerItemIsTimeLimit(nRealIndexX) > 0) then
					Talk(1,"","Trang bÞ cã thêi gian sö dông kh«ng thÓ n©ng cÊp")
					return
				end	
			end
		end
	end
end
	
	 nNL1  = GetItemCount(0,6,1, ITEM1, -1, -1, pos_equiproom)
	 nNL2  = GetItemCount(0,6,1, ITEM2, -1, -1, pos_equiproom)
	 nTD  = GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom)
	if  nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
			return
		end
	if  nNL1 < nSL1 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSL1.." Thøc ¨n cho ngùa.")
			return
	end
		if  nNL2 < nSL2 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSL2.." LuyÖn Cèt §¬n .")
			return
		end
	if count == 0 then
		Msg2Player("Xin ®Æt chiÕn m· cÇn n©ng cÊp vµo")
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 chiÕn m· Phi V©n")
	elseif count == 1 then
		if GetCash() >= nKvan  and GetItemCount(0,6,1, ITEM1, -1, -1, pos_equiproom) >= nSL1 and GetItemCount(0,6,1, ITEM2, -1, -1, pos_equiproom) >= nSL2 and GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom) >= nTienDong  then
			nTiLeThanhCong = random(0,10)
			if nTiLeThanhCong < 3 then
				RemoveItem(nRealIndexX,1);
				DelItem(0, -1, 6,1 ,ITEM1, -1, -1, pos_equiproom, nSL1) -- xo¸ 
				DelItem(0, -1, 6,1 ,ITEM2, -1, -1, pos_equiproom, nSL2) -- xo¸ 
				DelItem(0, -1, 6,1 ,4835, -1, -1, pos_equiproom, nTienDong) -- xo¸ 
				Pay(nKvan);
				AddItem(0,10,6,10,0,0,0)
				EndGiveBox()
				Msg2Player("Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 chiÕn m· B«n Tiªu")
				Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc<color> 1  B«n Tiªu ThÇn m· ")
				Talk(1,"","Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 chiÕn m· B«n Tiªu")
			else
				Pay(nKvan);
				DelItem(0, -1, 6,1 ,ITEM1, -1, -1, pos_equiproom, nSL1) -- xo¸ 
				DelItem(0, -1, 6,1 ,ITEM2, -1, -1, pos_equiproom, nSL2) -- xo¸ 
				DelItem(0, -1, 6,1 ,4835, -1, -1, pos_equiproom, nTienDong) -- xo¸ 
				EndGiveBox()
				Msg2Player("Chia buån cïng ®¹o h÷u. LÇn n©ng cÊp nµy thÊt b¹i, tæn thÊt "..(nKvan/10000).." v¹n l­îng vµ "..nTienDong.." tiÒn ®ång.");
			end
		else	
			Msg2Player("B¹n cÇn chuÈn bÞ ®ñ nguyªn liÖu míi cã thÓ n©ng cÊp")
		end
	end
end

function nangcap4()
	local nKvan = 20000000
	local nTienDong = 100
	local nSL1 = 600
	local nSL2 = 600
	szLoiNoi = "ChØ cÇn ®Æt ChiÕn m· cÇn n©ng cÊp vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n - "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång. \n".."- Thøc ¨n cho ng­a: "..nSL1.."\n- LuyÖn cèt ®¬n: "..nSL2.."\n TØ lÖ thµnh c«ng: 40%"
	OpenGiveBox("§Æt Ngùa Muèn N©ng CÊp Vµo",szLoiNoi,"batdaunangcap4")
end


function batdaunangcap4()
	local i,j, nIndex,kind,genre,detail,parti,level,series,row;
	local nRealIndex = 0;
	local nRealIndexX = 0;
	local nKvan = 20000000
	local nTienDong = 100
	local nSL1 = 600
	local nSL2 = 600
	local count = 0;
	for i=0,5 do
		for j=0,3 do
			nIndex = GetROItem(ROOAFFAIR,i,j)
			if (nIndex > 0) then
				nRealIndex = nIndex
				kind,genre,detail,parti,level,series,row = GetCBItem(nRealIndex)
				if (genre == 0 and detail == 10 and parti == 6 and level == 10) then
				count = count + 1
				nRealIndexX = nIndex
				if (GetPlayerItemIsLock(nRealIndexX) == -2) then
					Talk(1,"","ChiÕn m· ®­îc kho¸ b¶o hiÓm vÜnh viÔn kh«ng thÓ n©ng cÊp")
					return
				end
				if (GetPlayerItemIsTimeLimit(nRealIndexX) > 0) then
					Talk(1,"","Trang bÞ cã thêi gian sö dông kh«ng thÓ n©ng cÊp")
					return
				end	
			end
		end
	end
end
	
	 nNL1  = GetItemCount(0,6,1, ITEM1, -1, -1, pos_equiproom)
	 nNL2  = GetItemCount(0,6,1, ITEM2, -1, -1, pos_equiproom)
	 nTD  = GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom)
	if  nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
			return
		end
	if  nNL1 < nSL1 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSL1.." Thøc ¨n cho ngùa.")
			return
	end
		if  nNL2 < nSL2 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSL2.." LuyÖn Cèt §¬n .")
			return
		end
	if count == 0 then
		Msg2Player("Xin ®Æt chiÕn m· cÇn n©ng cÊp vµo")
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 chiÕn m· Phiªn Vò")
	elseif count == 1 then
		if GetCash() >= nKvan  and GetItemCount(0,6,1, ITEM1, -1, -1, pos_equiproom) >= nSL1 and GetItemCount(0,6,1, ITEM2, -1, -1, pos_equiproom) >= nSL2 and GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom) >= nTienDong  then
			nTiLeThanhCong = random(0,9)
			if nTiLeThanhCong < 5 then
				RemoveItem(nRealIndexX,1);
				Pay(nKvan);
				DelItem(0, -1, 6,1 ,ITEM1, -1, -1, pos_equiproom, nSL1) -- xo¸ 
				DelItem(0, -1, 6,1 ,ITEM2, -1, -1, pos_equiproom, nSL2) -- xo¸ 
				DelItem(0, -1, 6,1 ,4835, -1, -1, pos_equiproom, nTienDong) -- xo¸ 
				AddItem(0,10,7,10,0,0,0)
				EndGiveBox()
				Msg2Player("Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 ngùa Phiªn Vò ThÇn m·")
				Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc<color> 1  Phiªn Vò ThÇn m· ")
				Talk(1,"","Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 chiÕn m· Phiªn Vò ThÇn m·")
			else
				Pay(nKvan/10);
				DelItem(0, -1, 6,1 ,ITEM1, -1, -1, pos_equiproom, nSL1) -- xo¸ 
				DelItem(0, -1, 6,1 ,ITEM2, -1, -1, pos_equiproom, nSL2) -- xo¸ 
				DelItem(0, -1, 6,1 ,4835, -1, -1, pos_equiproom, nTienDong) -- xo¸ 
				EndGiveBox()
				Msg2Player("Chia buån cïng ®¹o h÷u. LÇn n©ng cÊp nµy thÊt b¹i, tæn thÊt "..(nKvan/100000).." v¹n l­îng vµ "..nTienDong.." tiÒn ®ång.");
			end
			else	
			Msg2Player("B¹n cÇn chuÈn bÞ ®ñ nguyªn liÖu míi cã thÓ n©ng cÊp")
		end
	end
end

function nangcap5()
	local nKvan = 20000000
	local nTienDong = 150
	local nSL1 = 600
	local nSL2 = 600
	szLoiNoi = "ChØ cÇn ®Æt ChiÕn m· cÇn n©ng cÊp vµo.\nC¸c nguyªn liÖu kh¸c ®Ó s¼n trong r­¬ng ®å bao gåm:\n - "..(nKvan/10000).." v¹n l­îng.\n".."- "..nTienDong.." tiÒn ®ång. \n".."- Thøc ¨n cho ng­a: "..nSL1.."\n- LuyÖn cèt ®¬n: "..nSL2.."\n TØ lÖ thµnh c«ng: 40%"
	OpenGiveBox("§Æt Ngùa Muèn N©ng CÊp Vµo",szLoiNoi,"batdaunangcap5")
end

function batdaunangcap5()
	local i,j, nIndex,kind,genre,detail,parti,level,series,row;
	local nRealIndex = 0;
	local nRealIndexX = 0;
	local nKvan = 20000000
	local nTienDong = 150
	local nSL1 = 600
	local nSL2 = 600
	local count = 0;
	for i=0,5 do
		for j=0,3 do
			nIndex = GetROItem(ROOAFFAIR,i,j)
			if (nIndex > 0) then
				nRealIndex = nIndex
				kind,genre,detail,parti,level,series,row = GetCBItem(nRealIndex)
				if (genre == 0 and detail == 10 and parti == 7 and level == 10) then
				count = count + 1
				nRealIndexX = nIndex
				if (GetPlayerItemIsLock(nRealIndexX) == -2) then
					Talk(1,"","ChiÕn m· ®­îc kho¸ b¶o hiÓm vÜnh viÔn kh«ng thÓ n©ng cÊp")
					return
				end
				if (GetPlayerItemIsTimeLimit(nRealIndexX) > 0) then
					Talk(1,"","Trang bÞ cã thêi gian sö dông kh«ng thÓ n©ng cÊp")
					return
				end	
			end
		end
	end
end
	
	 nNL1  = GetItemCount(0,6,1, ITEM1, -1, -1, pos_equiproom)
	 nNL2  = GetItemCount(0,6,1, ITEM2, -1, -1, pos_equiproom)
	 nTD  = GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom)
	if  nTD < nTienDong then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTienDong.." TiÒn §ång.")
			return
		end
	if  nNL1 < nSL1 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSL1.." Thøc ¨n cho ngùa.")
			return
	end
		if  nNL2 < nSL2 then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nSL2.." LuyÖn Cèt §¬n .")
			return
		end
	if count == 0 then
		Msg2Player("Xin ®Æt chiÕn m· cÇn n©ng cÊp vµo")
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 chiÕn m· Phiªn Vò")
	elseif count == 1 then
		if GetCash() >= nKvan  and GetItemCount(0,6,1, ITEM1, -1, -1, pos_equiproom) >= nSL1 and GetItemCount(0,6,1, ITEM2, -1, -1, pos_equiproom) >= nSL2 and GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom) >= nTienDong  then
			nTiLeThanhCong = random(0,9)
			if nTiLeThanhCong < 5 then
				RemoveItem(nRealIndexX,1);
				Pay(nKvan);
				DelItem(0, -1, 6,1 ,ITEM1, -1, -1, pos_equiproom, nSL1) -- xo¸ 
				DelItem(0, -1, 6,1 ,ITEM2, -1, -1, pos_equiproom, nSL2) -- xo¸ 
				DelItem(0, -1, 6,1 ,4835, -1, -1, pos_equiproom, nTienDong) -- xo¸ 
				AddItem(0,10,11,10,0,0,0)
				EndGiveBox()
				Msg2Player("Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 ngùa  Siªu Quang")
				Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc<color> 1   Siªu Quang ")
				Talk(1,"","Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 chiÕn m·  Siªu Quang")
			else
				Pay(nKvan);
				DelItem(0, -1, 6,1 ,ITEM1, -1, -1, pos_equiproom, nSL1) -- xo¸ 
				DelItem(0, -1, 6,1 ,ITEM2, -1, -1, pos_equiproom, nSL2) -- xo¸ 
				DelItem(0, -1, 6,1 ,4835, -1, -1, pos_equiproom, nTienDong) -- xo¸ 
				EndGiveBox()
				Msg2Player("Chia buån cïng ®¹o h÷u. LÇn n©ng cÊp nµy thÊt b¹i, tæn thÊt "..(nKvan/100000).." v¹n l­îng vµ "..nTienDong.." tiÒn ®ång.");
			end
			else	
			Msg2Player("B¹n cÇn chuÈn bÞ ®ñ nguyªn liÖu míi cã thÓ n©ng cÊp")
		end
	end
end



function nangcap6()
	PutItem(szLoiNoi,"y/batdaunangcap6","n/no")
end


function batdaunangcap6()
	local i,j, nIndex,kind,genre,detail,parti,level,series,row;
	local nRealIndex = 0;
	local nKvan = 60000000
	local nTienDong = 12000
	local nSL1 = 1200
	local nSL2 = 1200
	local count = 0;
	for i=0,5 do
		for j=0,3 do
			nIndex,kind,genre,detail,parti,level,series,row = GetItemParam(10,i,j);
			if (nIndex > 0 and genre == 0 and detail == 10 and parti == 9 and level == 10)  then
					count = count + 1;
					nRealIndex = nIndex;
					local bLock,nMin = GetItemLock(nRealIndex);
					if (bLock == 2) then 
					Talk(1,"","ChiÕn m· ®­îc kho¸ b¶o hiÓm vÜnh viÔn kh«ng thÓ n©ng cÊp.") return end
			end
		end
	end
	if count == 0 then
		Msg2Player("Xin ®Æt chiÕn m· cÇn n©ng cÊp vµo")
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 chiÕn m· Siªu Quang")
	elseif count == 1 then
		if GetCash() >= nKvan and GetItemCount(ITEM1,5) >= nSL1 and GetItemCount(ITEM2,5) >= nSL2 and GetItemCount(21,3) >= nTienDong  then
			nTiLeThanhCong = random(0,99)
			if nTiLeThanhCong == 55 then
				RemoveItem(nRealIndex,1);
				Pay(nKvan);
				DelItem(ITEM1,5,nSL1);
				DelItem(ITEM2,5,nSL2);
				nIndex = ItemSetAdd(0,0,10,13,10,0,0); --Sieu Quang
				AddItemID(nIndex)
				Msg2Player("Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 chiÕn m· Siªu Quang	")
				Talk("",1,"Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 chiÕn m· Siªu Quang	")
			else
				Pay(nKvan/10);
				DelItem(21,3,(nTienDong/10));
				Msg2Player("Chia buån cïng ®¹o h÷u. LÇn n©ng cÊp nµy thÊt b¹i, tæn thÊt "..(nKvan/100000).." v¹n l­îng vµ "..(nTienDong/10).." tiÒn ®ång.");
			end
		else
			Msg2Player("B¹n cÇn chuÈn bÞ ®ñ nguyªn liÖu míi cã thÓ n©ng cÊp")
		end
	end
end

function nangcap7()
	PutItem(szLoiNoi,"y/batdaunangcap7","n/no")
end


function batdaunangcap7()
	local i,j, nIndex,kind,genre,detail,parti,level,series,row;
	local nRealIndex = 0;
	local nKvan = 80000000
	local nTienDong = 15000
	local nSL1 = 1500
	local nSL2 = 1500
	local count = 0;
	for i=0,5 do
		for j=0,3 do
			nIndex,kind,genre,detail,parti,level,series,row = GetItemParam(10,i,j);
			if (nIndex > 0 and genre == 0 and detail == 10 and parti == 13 and level == 10)  then
					count = count + 1;
					nRealIndex = nIndex;
					local bLock,nMin = GetItemLock(nRealIndex);
					if (bLock == 2) then 
					Talk(1,"","ChiÕn m· ®­îc kho¸ b¶o hiÓm vÜnh viÔn kh«ng thÓ n©ng cÊp.") return end
			end
		end
	end
	if count == 0 then
		Msg2Player("Xin ®Æt chiÕn m· cÇn n©ng cÊp vµo")
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 chiÕn m· Siªu Quang")
	elseif count == 1 then
		if GetCash() >= nKvan and GetItemCount(ITEM1,5) >= nSL1 and GetItemCount(ITEM2,5) >= nSL2 and GetItemCount(21,3) >= nTienDong  then
			nTiLeThanhCong = random(0,99)
			if nTiLeThanhCong == 55 then
				RemoveItem(nRealIndex,1);
				Pay(nKvan);
				DelItem(ITEM1,5,nSL1);
				DelItem(ITEM2,5,nSL2);
				nIndex = ItemSetAdd(0,0,10,14,10,0,0); --Kim Tinh ho xuong
				AddItemID(nIndex)
				Msg2Player("Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 chiÕn m· Kim Tinh Hæ V­¬ng")
				Talk("",1,"Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 chiÕn m· Kim Tinh Hæ V­¬ng")
			else
				Pay(nKvan/10);
				DelItem(21,3,(nTienDong/10));
				Msg2Player("Chia buån cïng ®¹o h÷u. LÇn n©ng cÊp nµy thÊt b¹i, tæn thÊt "..(nKvan/100000).." v¹n l­îng vµ "..(nTienDong/10).." tiÒn ®ång.");
			end
		else
			Msg2Player("B¹n cÇn chuÈn bÞ ®ñ nguyªn liÖu míi cã thÓ n©ng cÊp")
		end
	end
end

function nangcap8()
	PutItem(szLoiNoi,"y/batdaunangcap8","n/no")
end


function batdaunangcap8()
	local i,j, nIndex,kind,genre,detail,parti,level,series,row;
	local nRealIndex = 0;
	local nKvan = 100000000
	local nTienDong = 18000
	local nSL1 = 1800
	local nSL2 = 1800
	local count = 0;
	for i=0,5 do
		for j=0,3 do
			nIndex,kind,genre,detail,parti,level,series,row = GetItemParam(10,i,j);
			if (nIndex > 0 and genre == 0 and detail == 10 and parti == 14 and level == 10)  then
					count = count + 1;
					nRealIndex = nIndex;
					local bLock,nMin = GetItemLock(nRealIndex);
					if (bLock == 2) then 
					Talk(1,"","ChiÕn m· ®­îc kho¸ b¶o hiÓm vÜnh viÔn kh«ng thÓ n©ng cÊp.") return end
			end
		end
	end
	if count == 0 then
		Msg2Player("Xin ®Æt chiÕn m· cÇn n©ng cÊp vµo")
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 chiÕn m· Kim Tinh Hæ V­¬ng")
	elseif count == 1 then
		if GetCash() >= nKvan and GetItemCount(ITEM1,5) >= nSL1 and GetItemCount(ITEM2,5) >= nSL2 and GetItemCount(21,3) >= nTienDong  then
			nTiLeThanhCong = random(0,99)
			if nTiLeThanhCong == 55 then
				RemoveItem(nRealIndex,1);
				Pay(nKvan);
				DelItem(ITEM1,5,nSL1);
				DelItem(ITEM2,5,nSL2);
				nIndex = ItemSetAdd(0,0,10,18,10,0,0); --Kim Tinh ho xuong
				AddItemID(nIndex)
				Msg2Player("Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 chiÕn m· H·n HuyÕt Long C©u")
				Talk("",1,"Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 chiÕn m· H·n HuyÕt Long C©u")
			else
				Pay(nKvan/10);
				DelItem(21,3,(nTienDong/10));
				Msg2Player("Chia buån cïng ®¹o h÷u. LÇn n©ng cÊp nµy thÊt b¹i, tæn thÊt "..(nKvan/100000).." v¹n l­îng vµ "..(nTienDong/10).." tiÒn ®ång.");
			end
		else
			Msg2Player("B¹n cÇn chuÈn bÞ ®ñ nguyªn liÖu míi cã thÓ n©ng cÊp")
		end
	end
end

function nangcap9()
	PutItem(szLoiNoi,"y/batdaunangcap9","n/no")
end


function batdaunangcap9()
	local i,j, nIndex,kind,genre,detail,parti,level,series,row;
	local nRealIndex = 0;
	local nKvan = 120000000
	local nTienDong = 20000
	local nSL1 = 2000
	local nSL2 = 2000
	local count = 0;
	for i=0,5 do
		for j=0,3 do
			nIndex,kind,genre,detail,parti,level,series,row = GetItemParam(10,i,j);
			if (nIndex > 0 and genre == 0 and detail == 10 and parti == 18 and level == 10)  then
					count = count + 1;
					nRealIndex = nIndex;
					local bLock,nMin = GetItemLock(nRealIndex);
					if (bLock == 2) then 
					Talk(1,"","ChiÕn m· ®­îc kho¸ b¶o hiÓm vÜnh viÔn kh«ng thÓ n©ng cÊp.") return end
			end
		end
	end
	if count == 0 then
		Msg2Player("Xin ®Æt chiÕn m· cÇn n©ng cÊp vµo")
	elseif count > 1 then
		Msg2Player("ChØ cÇn ®Æt vµo 1 chiÕn m· H·n HuyÕt Long C©u")
	elseif count == 1 then
		if GetCash() >= nKvan and GetItemCount(ITEM1,5) >= nSL1 and GetItemCount(ITEM2,5) >= nSL2 and GetItemCount(21,3) >= nTienDong  then
			nTiLeThanhCong = random(0,99)
			if nTiLeThanhCong == 55 then
				RemoveItem(nRealIndex,1);
				Pay(nKvan);
				DelItem(ITEM1,5,nSL1);
				DelItem(ITEM2,5,nSL2);
				nIndex = ItemSetAdd(0,0,10,22,10,0,0); --S­ Tö
				AddItemID(nIndex)
				Msg2Player("Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 chiÕn m· S­ Tö")
				Talk("",1,"Chóc mõng §¹o h÷u n©ng cÊp chiÕn m· thµnh c«ng, nhËn ®­îc 1 chiÕn m· S­ Tö")
			else
				Pay(nKvan/100000);
				DelItem(21,3,(nTienDong/10));
				Msg2Player("Chia buån cïng ®¹o h÷u. LÇn n©ng cÊp nµy thÊt b¹i, tæn thÊt "..(nKvan/100000).." v¹n l­îng vµ "..(nTienDong/10).." tiÒn ®ång.");
			end
		else
			Msg2Player("B¹n cÇn chuÈn bÞ ®ñ nguyªn liÖu míi cã thÓ n©ng cÊp")
		end
	end
end
function no()
end
