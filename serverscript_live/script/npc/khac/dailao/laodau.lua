-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local leave2
--hoangnhk
Include("\\script\\header\\taskid.lua");
Include("\\script\\header\\fuyuan.lua")
Include("\\script\\header\\timerhead.lua")

function main(sel)
	local nTaskValue = GetTask(TASK_DUNGCHUNG3)
	local nTask = GetNumber(3,nTaskValue, 3);
	local PK_value = GetPK(); 
	if (nTask == 1) then
		Say("Th¨m xong ch­a, ®i nhanh mét chót",2,"C¸m ¬n vÞ ®¹i ca nµy, ta ®i ®©y/visit_leave","Xin ®¹i ca th­ th¶ cho chót ®·, ta ®i ngay th«i/no") 
	elseif (PK_value <= 0) then
		Talk(2,"PKclear", 10106,10107) 
	else 
		local n_timerid = GetTimerId(); 
		if (n_timerid ~= 9) then 
						SetTask(TASK_DUNGCHUNG3,SetNumber(3,GetTask(TASK_DUNGCHUNG3),3,100 + PK_value));
		SetTimer(2*60*60*18,9) 
		return 
	end 
		Say(10108,3,
		"Hái thêi gian/ask_timer",
		"Nép tiÒn t¹i ngo¹i/pay_money",
		"D¹! D¹! NÕu vËy ta sÏ trë vµo trong ®ã/no") 
	end 
end 

function ask_timer() 
	if (GetCash() >= 1000) then
		Pay(1000) 
		Talk(2,"","VÞ ®¹i ca nµy! ®©y lµ 1000 l­îng b¹c xin mêi uèng chÐn trµ, ta chØ lµ muèn hái mét chót b©y giê lµ mÊy giê?.","<npc>: Téi nghiÖt cña ng­¬i muèn gi¶m cßn sím l¾m <color=255,0,0>"..GetRestCTime().."<color>, nhanh vÒ chç ngåi ®i") 
	else 
		Talk(2,"","VÞ ®¹i ca nµy! Ta chØ muèn hái b©y giê lµ mÊy giê th«i!","Hái g× mµ hái! Muèn ¨n ®ßn h¶! Cßn kh«ng mau quay vÒ chç ngåi!") 
	end 
end 

function pay_money() 
	PK_PayMoney = GetPK()*GetPK()*10000 
	Talk(1,"pay_money2","Lao ®Çu ®¹i ca, ë ®©y cã "..PK_PayMoney.." l­îng, xem nh­ Ýt lßng thµnh. ChØ v× ta ë trong nµy ®· sinh träng bÖnh kh«ng thÓ kh¸ng cù ®­îc n÷a. Xin ®¹i ca h·y khai ©n cho ra ngoµi t×m ®¹i phu") 
end 

function pay_money2() 
	Say("Xem ng­¬i r¸ch r­íi vËy, sao l¹i cã nhiÒu tiÒn vËy thÕ!",2,
	"§Æt tiÒn vµo tay h¾n/pay_yes",
	"TiÕp tôc thØnh cÇu/pay_no") 
end 

function pay_yes() 
	local PK_PayMoney = GetPK()*GetPK()*10000 
	if(GetCash() >= PK_PayMoney) then 
		Pay(PK_PayMoney) 
		Msg2Player("Lao ®Çu cho phÐp ng­¬i ra ngoµi t×m thÇy thuèc, ng­¬i cã thÓ rêi khái ®¹i lao.") 
		Talk(1,"leave_prison",10112) 
	else 
		pay_no();
	end
end

function pay_no() 
Talk(1,"","Quû ®ãi! Cót ngay! Kh«ng ta quÊt cho 1 roi b©y giê!") 
end

function PKclear() 
	Msg2Player("Sau mét håi xem xÐt th× ng­¬i bÞ ®uæi ra khái ®¹i lao") 
	leave_prison() 
end 

function leave_prison() 
	StopTimer()
	FuYuan_Resume(); 
	leave2() 
end 

function visit_leave() 
	leave2();
end 

function leave2() 
	SetTask(TASK_DUNGCHUNG3, SetNumber(3,GetTask(TASK_DUNGCHUNG3),3,0));
	local leave_pos = GetNumber(1,GetTask(TASK_DUNGCHUNG),9);
	SetCreateTeam(1); 
	if (leave_pos == 1) then
		NewWorld(176,1473,3245) 
	else
		NewWorld(37,1781,3044) 
	end
end 

function no() 
end 
