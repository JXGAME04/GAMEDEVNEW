--Author: Fong KiÒu
--Function: Thæ §Þa phï
--Date: 21/07/2021


Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_vatpham.lua")

function main(nIndex)
	dofile("script/item/ib/tuihoatdong.lua")
	local ngiakv = 60000
	local ntongsoluong = GetTask(TASK_FANCUNG_4)
	if(ntongsoluong > 10000) then
		Talk(1,"","<sex>: b¹n ®· sö dông ®ñ 10 ngh×n c¸i ? Kh«ng thÓ dïng thªm n÷a !")
		return
	end
	if GetCash() >= ngiakv then
		Pay(ngiakv)
		else
			if GetTask(T_PLAYER_XU) >= 1 then
				SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) - 1)
		else
			Talk(1,"","<sex> kh«ng ®ñ 6 v¹n vµ kh«ng ®ñ 1 xu !")
			return
		end
	end
	
	local naddtungphaiList = {253, 258, 263, 239, 243, 248, 313, 308, 318, 298, 303, 268, 276, 283, 288, 331, 333, 338, 343, 353, 358, 363, 368}
	local randIndex = random(1, getn(naddtungphaiList))  
	local itemid = naddtungphaiList[randIndex]
	
		if random(1,100) > 99 then
			local nRanF = random(1,15);
			if (nRanF <  3) then
					local nTienVan = 200000
					local nThongL = nTienVan/10000
					Earn(nTienVan)
					Msg2Player("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..nThongL.." <color=red> V¹n khi sö dông Tói Ho¹t §éng!")
			elseif (nRanF <  5) then
					local nIndexx = AddItem(6,1, itemid,0,0,0,0 )
					local s_name = GetItemName(nIndexx)
					Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Tói Ho¹t §éng!")
			elseif (nRanF ==  10) then
					local nrandomVip = random(1,8)
					if nrandomVip == 5 then		
						local nIndexx = AddItem2(2,0,random(163,166),0,0,0) -- An Bang	
						local s_name = GetItemName(nIndexx)
						Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Tói Ho¹t §éng!")
						AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Tói Ho¹t §éng! ",3)
					else 
						local nIndexx =  AddItem2(2,0,random(158,162),0,0,0) -- dinh quoc			
						local s_name = GetItemName(nIndexx)
						AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Tói Ho¹t §éng! ",3)
						Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Tói Ho¹t §éng!")
					end
			end
		else
		SetTask(TASK_FANCUNG_4, GetTask(TASK_FANCUNG_4) + 1)
		-- SetTask(TASK_FANCUNG_4, GetTask(TASK_FANCUNG_4) - 100)
		local nExp = GetLevel()^3*2
		AddSumExp(nExp)
		Msg2Player("Test <color=green>"..ntongsoluong.." <color=red> c¸i!")
		if (ntongsoluong == 2000  or ntongsoluong == 4000 or  ntongsoluong == 6000  or ntongsoluong == 8000   or ntongsoluong == 9999) then
			local nrandomVip = random(1,12)
					if nrandomVip == 5 then		
						local nIndexx = AddItem2(2,0,random(140,141),0,0,0) -- Vo Danh	
						local s_name = GetItemName(nIndexx)
						Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Tói Ho¹t §éng!")
						AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Tói Ho¹t §éng! ",3)
					else 
						local nIndexx =  AddItem2(2,0,random(142,145),0,0,0) --dong sat	
						local s_name = GetItemName(nIndexx)
						AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Tói Ho¹t §éng! ",3)
						Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Tói Ho¹t §éng!")
					end
			end
	end
	RemoveItem(nIndex)
end

