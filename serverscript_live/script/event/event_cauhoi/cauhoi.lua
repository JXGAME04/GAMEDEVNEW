-- Author: Fong KiÒu
-- Date: 28/11/2016
-- Chuc nang: Thuc thi

Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\event\\event_cauhoi\\lib.lua")

function main(NpcIndex)

	-- dofile("script/event/event_cauhoi/cauhoi.lua")
	
	if GetCamp() == 0 then
		Talk(1,"","<sex> ch­a vµo m«n ph¸i kh«ng thÓ tham gia. ")
		return
	end	
	
	if GetLevel() < MIN_LEVEL_JOIN then
		Talk(1,"","<sex> ch­a ®ñ ®¼ng cÊp "..MIN_LEVEL_JOIN.." ®Ó tham gia ")
		return
	end		
	
	if GetTask(T_VAOPHAI) < 0 then
		Talk(1,"","<sex> ch­a vµo m«n ph¸i kh«ng thÓ tham gia. ")
		return
	end		
	
	if GetTask(T_REPCAUHOI) > MAX_NUM then
		Talk(1,"", "<sex> ®· tr¶ lêi hoµn thµnh "..MAX_NUM.." c©u hái. ")
		return 
	end
	
	--if(GetTask(T_CH_SAILIENTIEP) >= NUM_SAILIENTIEP) then
	--	Talk(1,"", "<sex> ®· tr¶ lêi sai liªn tiÕp "..NUM_SAILIENTIEP.." c©u. H«m nay nghØ ng¬i mai quay l¹i thi nhÐ. ")
	--	return
	--end
	
	--Msg2Player(GetTask(T_CH_DANHANTHG))
	if(GetTask(T_CH_DANHANTHG) > 0) then
		Talk(1,"", "<sex> h«m nay ®· nhËn th­ëng råi nghØ ng¬i mai quay l¹i nhÐ. ")
		return
	end
	
	SetTask(T_REPCAUHOI,GetTask(T_REPCAUHOI)+1)
	cauhoi(NpcIndex)
	
end

function cauhoi(NpcIndex)
	
	DelNpc(NpcIndex) --xo¸ hoa ®¨ng khi tr¶ lêi
	
	local i = random(1,getn(CAUHOI))
	local a = random(1,4)
	local TAB_CAUTRALOI = 
	{
		"<color=green>CÈm nang<color>: "..CAUHOI[i][1].."",
	}
	if a == 1 then
		Say(TAB_CAUTRALOI[1] , 4,
			"A. "..CAUHOI[i][2].."/dung",
			"B. "..CAUHOI[i][3].."/sai",
			"C. "..CAUHOI[i][4].."/sai",
			"D. "..CAUHOI[i][5].."/sai")
	elseif a == 2 then
		Say(TAB_CAUTRALOI[1] , 4,
			"A. "..CAUHOI[i][5].."/sai",
			"B. "..CAUHOI[i][2].."/dung",
			"C. "..CAUHOI[i][3].."/sai",
			"D. "..CAUHOI[i][4].."/sai")
	elseif a == 3 then
		Say(TAB_CAUTRALOI[1] , 4,
			"A. "..CAUHOI[i][4].."/sai",
			"B. "..CAUHOI[i][5].."/sai",
			"C. "..CAUHOI[i][2].."/dung",
			"D. "..CAUHOI[i][3].."/sai")
	elseif a == 4 then
		Say(TAB_CAUTRALOI[1] , 4,
			"A. "..CAUHOI[i][3].."/sai",
			"B. "..CAUHOI[i][4].."/sai",
			"C. "..CAUHOI[i][5].."/sai",
			"D. "..CAUHOI[i][2].."/dung")
	end 
end

function dung()
	local nLevel = GetLevel() 
	--SetTask(T_CH_SAILIENTIEP, 0) --reset sai liªn tiÕp vÒ 0
	for i=1, getn(EXPCAUHOI) do
		if nLevel >= EXPCAUHOI[i][2] and nLevel <= EXPCAUHOI[i][3] then
			--local nRand = random(1,2)
			--if nRand == 1 then
				--local nExp = (EXPCAUHOI[i][1])
				--AddSumExp(nExp)
				--AddRepute(random(1,5))
				--Msg2Player("Tr¶ lêi chÝnh x¸c c©u  sè "..GetTask(T_REPCAUHOI)..". Th­ëng "..nExp.." ®iÓm kinh nghiÖm.")
			--else
				--local nMoney = (EXPCAUHOI[i][4])			
				--Earn(nMoney)
				--AddFuYuan(random(1,5))
				--Msg2Player("Tr¶ lêi chÝnh x¸c c©u  sè "..GetTask(T_REPCAUHOI)..". Th­ëng "..nMoney.." ng©n l­îng.")	
			--end
			SetTask(T_RIGHTQUESTION, GetTask(T_RIGHTQUESTION) + 1)
			Msg2Player("Tr¶ lêi chÝnh x¸c tæng céng <color=yellow>"..GetTask(T_RIGHTQUESTION).."/"..MAX_NUM.."<color>. §óng trªn <color=pink>"..NUM_NHANTHUONG.." c©u<color> ®Õn LÔ quan nhËn th­ëng.")
		end
	end
end

function sai()
	--SetTask(T_CH_SAILIENTIEP, GetTask(T_CH_SAILIENTIEP) + 1)
	--Talk(1,"", "RÊt tiÕc b¹n tr¶ lêi kh«ng chÝnh x¸c <color=pink>c©u "..GetTask(T_REPCAUHOI).."<color>. §· sai liªn tiÕp <color=yellow>"..GetTask(T_CH_SAILIENTIEP).." c©u<color> nÕu v­ît qu¸ c©u "..NUM_SAILIENTIEP.." th× sÏ dõng ho¹t ®éng")
	Talk(1,"", "RÊt tiÕc <sex> tr¶ lêi kh«ng chÝnh x¸c <color=pink>c©u "..GetTask(T_REPCAUHOI).."<color>.")
end

function OnTimer(nIndex)
	DelNpc(nIndex)
end