--Author: Fong KiÒu
--Function: Thæ §Þa phï
--Date: 21/07/2021


Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\test\\event_rank.lua")

function main(nIndex)
	dofile("script/item/event/tuilyxi.lua")
	local ngiakv = 70000
	local ntongsoluong = GetTask(TASK_HD_EVENTNEW)

	if GetCash() >= ngiakv then
		Pay(ngiakv)
	else
		if GetTask(T_PLAYER_XU) >= 1 then
				SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) - 1)
		else
			Talk(1,"","<sex> kh«ng ®ñ 7 v¹n vµ kh«ng ®ñ 1 xu !")
			return
		end
	end
	Event_AddScore(1)	
		-- SetTask(TASK_HD_EVENTNEW, GetTask(TASK_HD_EVENTNEW) + 1)
		if random(1,100) > 99 then
			local nRanF = random(1,15);
			if (nRanF <  3) then
					local nIndexx = AddItemSL(2134,1,-2) --- ngu hanh ky thach
					local s_name = GetItemName(nIndexx)
					Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Bao L× X× N¨m Míi	!")
			elseif (nRanF <  6) then
					local nIndexx = AddItemSL(70,1,-2) --- tien thao lo trung
					local s_name = GetItemName(nIndexx)
					Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Bao L× X× N¨m Míi	!")
			elseif (nRanF ==  10) then
					local nrandomVip = random(1,10)
					if nrandomVip == 5 then		
						local nIndexx = AddItem2(2,0,random(163,166),0,0,0) -- An Bang		
						local s_name = GetItemName(nIndexx)
						Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Bao L× X× N¨m Míi	!")
						AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Bao L× X× N¨m Míi	! ",3)
					else 
						local nIndexx =  AddItem2(2,0,random(158,162),0,0,0) -- dinh quoc			
						local s_name = GetItemName(nIndexx)
						AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Bao L× X× N¨m Míi	! ",3)
						Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Bao L× X× N¨m Míi	!")
					end
			end
	end
		
	Exp = 15E5;
	AddOwnExp(Exp)	
	if (ntongsoluong == 2000  or ntongsoluong == 4000 or  ntongsoluong == 6000  or ntongsoluong == 8000   or ntongsoluong == 10000 or ntongsoluong == 12000 or ntongsoluong == 14000 or ntongsoluong == 16000  or ntongsoluong == 18000 or ntongsoluong == 20000
	 or ntongsoluong == 22000  or ntongsoluong == 24000  or ntongsoluong == 26000  or ntongsoluong == 28000  or ntongsoluong == 30000 or ntongsoluong == 32000  or ntongsoluong == 34000  or ntongsoluong == 36000  or ntongsoluong == 38000  or ntongsoluong == 40000 ) then
			local nrandomVip = random(1,20)
				if nrandomVip == 5 then		
						local nIndexx = AddItemSL(4855,1,-2)  -- ruong hkmp 
						local s_name = GetItemName(nIndexx)
						Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Bao L× X× N¨m Míi	!")
						AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Bao L× X× N¨m Míi	! ",3)
					else 
						local nIndexx =  	 AddItemSL(139,1,-2) --- Thiep nam moi
						local s_name = GetItemName(nIndexx)
						AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Bao L× X× N¨m Míi	! ",3)
						Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Bao L× X× N¨m Míi	!")
					end
					--Msg2Player("Tæng <color=green>"..ntongsoluong.." <color=red> c¸i!")
					thoigian = tonumber(date("%H%M%d%m"))
					LoginLog = openfile("dulieu/NhanMocEventTet.txt", "a");
					if LoginLog then
					write(LoginLog,"Thanh Cong: ID:- Name: ["..GetName().."] - Acc: ["..GetAccount().."] - Moc: ["..ntongsoluong.."] - Time: ["..thoigian.."] \n");
			end
					closefile(LoginLog)
		end
	-- Msg2Player("Tæng <color=green>"..ntongsoluong.." <color=red> tói!")
	RemoveItem(nIndex)
end

