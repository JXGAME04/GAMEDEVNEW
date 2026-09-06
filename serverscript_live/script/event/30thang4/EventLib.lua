--Author: Kinnox;
--Date: 22/03/2021;

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
TAB_NEED = {
{1,4830,"Huy Ch­¬ng Vµng ",},
{2,4829,"Ng«i Sao ChiÕn Th¾ng ",},
{3,4832,"C«ng Tr¸ng LÖnh ",},
{4,4831,"Huy Ch­¬ng Tèng Kim ",},
{5,4828,"B¶n §å Chinh Ph¹t ",},
{6,4825,"Huy Ch­¬ng ChiÕn C«ng ",},
{7,4827,"Huy Ch­¬ng Ph¹t Tèng ",},
{8,4826,"Huy Ch­¬ng Ng¹o ThÕ ",},
}

TAB_Bonus = {
[1]={{0,0},{0,0},{0,0}},
[2]={

{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{4,239},--Lam Thuy Tinh
{4,240},--Luc Thuy Tinh
{6,1,121},--Phuc Duyen Tieu
{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{6,1,122},--Phuc Duyen Trung
{6,1,123},--Phuc Duyen Dai
{6,1,123},--Phuc Duyen Dai
{6,1,123},--Phuc Duyen Dai
{6,1,122},--Phuc Duyen Trung
{6,1,123},--Phuc Duyen Dai
{6,1,123},--Phuc Duyen Dai
{6,1,71},--Tien Thao Lo
{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{6,1,4818},--lb da tau
{6,1,72},--Thien son bao lo
{6,1,1182},--Tien Thao Lo
{6,1,1182},--Tien Thao Lo
{6,1,4818},--lb da tau
{6,1,1023},--LB boss
{6,1,1023},--LB boss
{6,1,1023},--LB boss
{6,1,1023},--LB boss
{6,1,1023},--LB boss
{6,1,1023},--LB boss
{6,1,1023},--LB boss
{6,1,1023},--LB boss
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han 
{6,1,124},--que hoa tuu
{6,1,124},--que hoa tuu
{6,1,124},--que hoa tuu
{6,1,124},--que hoa tuu
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,4847},--chan nguyen don
},
[3]={
{6,1,121},--Phuc Duyen Tieu
{6,1,122},--Phuc Duyen Trung
{6,1,1023},-- LB boss
{6,1,123},--Phuc Duyen Dai
{6,1,123},--Phuc Duyen Dai
{6,1,1023},--LB boss
{6,1,122},--Phuc Duyen Trung
{6,1,122},--Phuc Duyen Trung
{6,1,71},--Tien Thao Lo
{6,1,71},--Tien Thao Lo
{6,1,1023},--LB boss
{6,1,1023},--LB boss
{6,1,1023},--LB boss
{6,1,1023},--LB boss
{6,1,1182},--Tien Thao Lo 8gio
{6,1,124},--que hoa tuu
{6,1,72},--Thien son bao lo
{6,1,124},--que hoa tuu
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,4847},--chan nguyen don
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,22},--tay tuy kinh
{6,1,26}--vo lam mat tich
}
}

function Bonus(nNum)
local Exp = 0;
local nRan = 0;
local nIndex = 0;
local nTaskValue = 0;
local nTaskNum = 0;
	if (nNum == 1) then
		Exp = 15E5;
		SetTask(TASK_EVENT3004_1,GetTask(TASK_EVENT3004_1) +1);
	elseif (nNum == 2) then
		SetTask(TASK_EVENT3004_2,GetTask(TASK_EVENT3004_2) +1);
		Exp = 2E6;
		nRan = random(1,getn(TAB_Bonus[2]));
		if random(1,100) > 99 then
			local nRanC = random(1,3);
			if (nRanC == 1) then			
			nIndex = AddItemSL(4849,200,0)  -- Huyet long don 
			elseif (nRanC == 2) then
			nIndex = AddItemSL(4847,200,0)  -- chan nguyen do 
			elseif (nRanC == 3) then
			nIndex = AddItemSL(4844,200,0)  -- ho mach don 
			end
			local s_name = GetItemName(nIndex)
			Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>200 "..s_name.." <color=red> khi sö dông Event!")
		end
		if random(100) > 95 then
			nIndex = AddItem(TAB_Bonus[2][nRan][1],TAB_Bonus[2][nRan][2],TAB_Bonus[2][nRan][3],0,0,0,0);
			-- AddTimeItem(nIndex,30*24*60*60);
			-- Msg2Player("§¹i hiÖp nh©n ®­îc 1 vËt phÈm gi¸ trÞ");
		else
		-- Msg2Player("§¹i hiÖp nhËn ®­îc kinh nghiÖm");
		end
	elseif (nNum == 3) then
		SetTask(TASK_EVENT3004_3,GetTask(TASK_EVENT3004_3) +1);
		Exp = 35E5;
		nRan = random(1,getn(TAB_Bonus[3]));
		if random(1,100) > 99 then
			local nRanF = random(1,8);
			if (nRanF == 1) then
			nIndex = AddItem2(2,0,random(167,172),0,0,0) -- thien hoang
			elseif (nRanF == 2) then
			nIndex = AddItem2(2,0,random(190,192),0,0,0) -- nhu tinh
			elseif (nRanF == 3) then
			nIndex = AddItem2(2,0,random(185,188),0,0,0) -- hiep co
			elseif (nRanF == 4) then
			nIndex = AddItemSL(4844,500,0)  -- ho mach don 
			elseif (nRanF == 5) then
			nIndex = AddItemSL(4847,500,0)  -- chan nguyen don 
			elseif (nRanF == 6) then
			nIndex = AddItemSL(4849,50,0)  -- ho mach don 
			elseif (nRanF == 7) then
			nIndex = AddItem(0,10,5,random(1,5),0,0,10) -- ngua 8x
			elseif (nRanF == 8) then
				if random(1,10) == 10 then		
					nIndex =  AddItem2(2,0,189,0,0,0) -- ao nhu tinh 	
					local s_name = GetItemName(nIndex)
					AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Event! ",3)
				else 
					nIndex = AddItem2(2,0,random(158,162),0,0,0) -- dinh quoc		
					local s_name = GetItemName(nIndex)
					AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Event! ",3)
				end
			end
			local s_name = GetItemName(nIndex)
			AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Event! ",3)
			Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Event!")
		else
		if random(1,100) > 95 then
			nIndex = AddItem(TAB_Bonus[3][nRan][1],TAB_Bonus[3][nRan][2],TAB_Bonus[3][nRan][3],0,0,0,0);
			-- AddTimeItem(nIndex,30*24*60*60);
			-- Msg2Player("§¹i hiÖp nh©n ®­îc 1 vËt phÈm gi¸ trÞ");
			else
			-- Msg2Player("§¹i hiÖp nhËn ®­îc kinh nghiÖm");
			end
		end
	else
	Msg2Player("Hack h¶ mµy!");
	return 
	end
	AddOwnExp(Exp);
end
