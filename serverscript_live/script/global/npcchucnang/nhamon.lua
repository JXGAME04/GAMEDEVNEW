--Author: Fong KiÒu
--Date: 2021
--Function: Nha DÞch §¹i Lao

Include("\\script\\header\\taskid.lua")
Include("\\script\\header\\fuyuan.lua")
Include("\\script\\log_game\\save_log.lua")

function main(NpcIndex)
	local w,x,y = GetWorldPos();
	if (w == 1) then--bien kinh
	nhadich();
	return end
	if (w == 78) then--lam an
	nhadich();
	return end
	Talk(1,"",10936)
end

function nhadich() 
if (GetFightState() ~= 0) then 
	SetFightState(0)
end
	Say(10114,3,
	"Ta ®Õn ®Çu thó/atone",
	"Ta ®Õn th¨m tï/visit",
	"Kh«ng hái n÷a!/no") 
	LeaveTeam()
end 

function atone()
	local PK_value = GetPK();
	local my_money = GetCash();
	local ExpPer = floor(GetExp()/GetNextExp()*100)
	if (PK_value == 0) then
		Talk(2,"",10115,10116) 
	elseif (ExpPer < -50) then
		Talk(1,"",10118) 
		Msg2Player("LuËt ph¸p quy ®Þnh: Phµm nh÷ng kÎ kinh nghiÖm ©m v­ît qua 50%, kh«ng cho tù thó chuéc téi") 
	else 
		Say("<player>: Tr­íc ®©y ta lì tay s¸t th­¬ng ng­êi kh¸c, nay ®Õn tù thó", 1,"TiÕp tôc ®èi tho¹i/want_atone") 
	end 
end 

function want_atone() 
	local PK_value = GetPK(); 
	local my_money = GetCash();
	Say("Tay ng­¬i ®· nhuèm m¸u, ph¶i ngåi ®¹i lao ®Ó s¸m hèi téi lçi, tu t©m d­ìng t¸nh<enter>……<enter>Téi tr¹ng nh­ sau<enter>Cai ph¹m <color=Red><player><color> hiÖn ®iÓm PK lµ <color=Red>"..PK_value.."<color>, ®Þnh hèi lé quan sai <color=Red>"..my_money.."<color>. Céng gép 2 téi, nhèt vµo ®¹i lao "..PK_value.." ngµy.",2,
	"Vµo ®¹i lao/go_atone",
	"Hay lµ ta ch¹y trèn th«i/no")
end; 

function go_atone()
	local PK_value = GetPK()
	local my_money = GetCash()
	local w,x,y = GetWorldPos()
	Pay(my_money)
	Msg2Player("Quan sai tÞch thu toµn bé "..my_money.." l­îng cña b¹n. B¹n bÞ ®­a vµo ®¹i lao") 
	FuYuan_Pause();
	SetTask(TASK_DUNGCHUNG3, SetNumber(3,GetTask(TASK_DUNGCHUNG3),3,100+PK_value))
	if(w == 176) then
		SetTask(TASK_DUNGCHUNG, SetNumber(1,GetTask(TASK_DUNGCHUNG),9,1))
	else
		SetTask(TASK_DUNGCHUNG, SetNumber(1,GetTask(TASK_DUNGCHUNG),9,2))
	end;
	SetTimer(2*60*60*18,9) --2h chay ham OnTimer
	SetCreateTeam(0)
	NewWorld(208,1785,3062)
	logHoatDong("Name:"..GetName().."\tAccount:"..GetAccount().."\t bÞ nhèt vµo ®¹i lao cóp trªn ng­êi tÊt c¶ tiÒn mÆt "..my_money.." luong")
end 

function visit()
	Say("Mau ®i chç kh¸c! Nh×n c¸i g×? Cã ph¶i lµ ®ång bän kh«ng!",2,
	"Hèi lé 5000 l­îng b¹c/go_visit",
	"§i ra/no") 
end 

function go_visit() 
	local w,x,y = GetWorldPos();
	if (GetCash() >= 5000) then 
	Talk(1,"",10120) 
	Msg2Player("Hèi lé Nha DÞch 5000 l­îng b¹c, vµo ®¹i lao ®Ó th¨m tï nh©n") 
	Pay(5000)
	SetCreateTeam(0); 
	SetTask(TASK_DUNGCHUNG3, SetNumber(3,GetTask(TASK_DUNGCHUNG3),3,1));
	if(w == 176) then
		SetTask(TASK_DUNGCHUNG, SetNumber(1,GetTask(TASK_DUNGCHUNG),9,1));
	else
		SetTask(TASK_DUNGCHUNG, SetNumber(1,GetTask(TASK_DUNGCHUNG),9,2));
	end;
	NewWorld(208,1785,3062) 
	else 
	Talk(1,"",10121) 
	end 
end 

function no() 
end 

