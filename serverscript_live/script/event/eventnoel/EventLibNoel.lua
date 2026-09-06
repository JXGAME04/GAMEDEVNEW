--Author: Kinnox;
--Date: 22/03/2021;

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("\\script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")

TAB_NEED = {
{1,1317,"Hoa tuy’t ",},
{2,1318,"Cµ rËt ",},
{3,1320,"N„n gi∏ng sinh",},
{4,1321,"Kh®n choµng (xanh)",},
{5,1322,"Kh®n choµng (Æ·)",},
{6,1326,"Ng≠Íi tuy’t Th≠Íng",},
{7,1324,"Ng≠Íi tuy’t choµng kh®n xanh ",},
{8,1325,"Ng≠Íi tuy’t choµng kh®n Æ·",},
}

TAB_Bonus = {
[1]={{0,0},{0,0},{0,0}},
[2]={

{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep 
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{4,239},--Lam Thuy Tinh
{4,240},--Luc Thuy Tinh
{6,1,121},--Phuc Duyen Tieu
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep 
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep 
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,122},--Phuc Duyen Trung
{6,1,123},--Phuc Duyen Dai
{6,1,4820},--Ma Bai
{6,1,1182},--Tien Thao Lo 8gio
{6,1,122},--Phuc Duyen Trung
{6,1,123},--Phuc Duyen Dai
{6,1,123},--Phuc Duyen Dai
{6,1,71},--Tien Thao Lo
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,72},--Thien son bao lo
{6,1,1182},--Tien Thao Lo
{6,1,1182},--Tien Thao Lo
{6,1,2126},--long huyet hoan
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han
{6,1,23},--thiet la han 
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,4815},--le bao bi kiep
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,72},--Thien son bao lo
{6,1,4815},--le bao bi kiep
},
[3]={
{6,1,121},--Phuc Duyen Tieu
{6,1,122},--Phuc Duyen Trung
{6,1,123},--Phuc Duyen Dai
{6,1,123},--Phuc Duyen Dai
{6,1,123},--Phuc Duyen Dai
{6,1,122},--Phuc Duyen Trung
{6,1,4820},--Ma Bai
{6,1,71},--Tien Thao Lo
{6,1,71},--Tien Thao Lo
{6,1,71},--Tien Thao Lo
{6,1,71},--Tien Thao Lo
{6,1,71},--Tien Thao Lo
{6,1,71},--Tien Thao Lo
{6,1,71},--Tien Thao Lo
{6,1,1182},--Tien Thao Lo 8gio
{6,1,124},--que hoa tuu
{6,1,72},--Thien son bao lo
{6,1,2433},--dai thanh bi kip 9x
{6,1,2433},--dai thanh bi kip 9xx
{6,1,23},--thiet la han
{6,1,4815},--le bao bi kiep
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
		nTaskNum = GetTask(TASK_EVENT2011_1);
		--nTaskNum = GetNumber(4,nTaskValue,1);
		if GetTask(TASK_EVENT2011_1) > 2000 then
		return
		end
		Exp = 1E6;
		SetTask(TASK_EVENT2011_1,GetTask(TASK_EVENT2011_1) + 1);
		
	elseif (nNum == 2) then
		--nTaskValue = GetTask(TASK_EVENT2011_2);
		--nTaskNum = GetNumber(4,nTaskValue,1);
		SetTask(TASK_EVENT2011_2,GetTask(TASK_EVENT2011_2) + 1);
		Exp = 2E6;
		nRan = random(1,getn(TAB_Bonus[2]));
		if random(100) > 95 then
			nIndex = AddItem(TAB_Bonus[2][nRan][1],TAB_Bonus[2][nRan][2],TAB_Bonus[2][nRan][3],0,0,0,0);
			AddTimeItem(nIndex,30*24*60*60);
			Msg2Player("ßπi hi÷p nh©n Æ≠Óc 1 vÀt ph»m gi∏ trﬁ");
		else
		Msg2Player("ßπi hi÷p nhÀn Æ≠Óc kinh nghi÷m");
		end
	elseif (nNum == 3) then
		--nTaskValue = GetTask(TASK_EVENT2011_3);
		--nTaskNum = GetNumber(4,nTaskValue,1);
		SetTask(TASK_EVENT2011_3, GetTask(TASK_EVENT2011_3) + 1)
		Exp = 5E6;
		nRan = random(1,getn(TAB_Bonus[3]));
		if random(1,100) > 95 then
			local nRanF = random(1,8);
			if (nRanF == 1) then
			nIndex = AddEventItem(random(771,776),1);-- m∂nh hi÷p cËt k’t
			-- AddTimeItem(nIndex,7*24*60*60);
			elseif (nRanF == 2) then
			nIndex = AddEventItem(random(903,942),1);-- m∂nh NT HC ßQ
			-- AddTimeItem(nIndex,7*24*60*60);
			elseif (nRanF == 3) then
			nIndex = AddEventItem(random(783,788),1);-- m∂nh n„n Æﬁnh quËc
			-- AddTimeItem(nIndex,7*24*60*60);
			elseif (nRanF == 4) then
			nIndex = AddEventItem(random(903,942),1);-- m∂nh NT HC ßQ
			-- AddTimeItem(nIndex,7*24*60*60);
			elseif (nRanF == 5) then
			nIndex = AddEventItem(random(903,942),1);-- m∂nh NT HC ßQ
			-- AddTimeItem(nIndex,7*24*60*60);
			elseif (nRanF == 6) then
				if random(1,10) > 9 then
					 AddGoldItem(random(186, 189),0);--hiepcot
				else
					AddEventItem(random(903,942),1);-- m∂nh NT HC ßQ
				end
			elseif (nRanF == 7) then
				if random(1,10) > 9 then
					 AddGoldItem(random(191, 193),0);--nhu tinh
				else
					AddEventItem(random(903,942),1);-- m∂nh NT HC ßQ
				end
			elseif (nRanF == 8) then
				if random(1,20) > 19 then
					 AddGoldItem(random(159, 163),0); --dinhquoc
				else
					AddEventItem(random(783,788),1);-- m∂nh n„n Æﬁnh quËc
					-- AddTimeItem(nIndex,30*24*60*60);
				end
				Msg2Player("ßπi hi÷p nh©n Æ≠Óc 1 vÀt ph»m gi∏ trﬁ");
			end
		else
		if random(1,100) > 95 then
			nIndex = AddItem(TAB_Bonus[3][nRan][1],TAB_Bonus[3][nRan][2],TAB_Bonus[3][nRan][3],0,0,0,0);
			AddTimeItem(nIndex,30*24*60*60);
			Msg2Player("ßπi hi÷p nh©n Æ≠Óc 1 vÀt ph»m gi∏ trﬁ");
			else
		Msg2Player("ßπi hi÷p nhÀn Æ≠Óc kinh nghi÷m");
			end
		end
	else
	Msg2Player("Hack h∂ mµy!");
	return 
	end
	AddOwnExp(Exp);
end
