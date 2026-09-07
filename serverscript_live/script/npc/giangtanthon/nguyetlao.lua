--Author: Fong KiÒu
--Date: 07/07/2021
--Function: NguyÖt L·o Giang T©n Th«n

Include("\\script\\header\\taskid.lua")
Include("\\script\\lib\\lib_ham.lua")

REQ_LEVEL_MATENAME 					= 50
NGANLUONG_LYHON							= 50000

function main(sel)
	
	-- dofile("script/npcthon/giangtanthon/nguyetlao.lua")
	
	local OPTIONS = {}
	tinsert(OPTIONS, 12958)
	if(GetMateName() == "") then
		if(GetSex() == 0) then
			tinsert(OPTIONS, "Ta ®· cã ý trung nh©n, muèn cÇu h«n c« Êy/kethon")
		else
			tinsert(OPTIONS, "Ta ®· cã ý trung nh©n, muèn NguyÖt L·o ®øng ra chñ sù mèi nh©n duyªn nµy/kethon2")
		end
	else
		tinsert(OPTIONS, "Chóng t«i kh«ng hîp nhau, muèn ly h«n/lyhon")
		tinsert(OPTIONS, "Phu thª chóng t«i c¶i v· suèt ngµy, xin NguyÖt L·o cho vµi lêi khuyªn/khuyen")		
	end
	local menPIdx = GetTaskTemp(HAVELOVE_FEMALE)
	if(menPIdx > 0 and GetSex() == 1) then
		local temPIdx = PlayerIndex
		PlayerIndex = menPIdx
		local szNameMan = GetName()
		tinsert(OPTIONS, "Ta ®ång ý lêi cÇu h«n cña "..szNameMan.."./#nhanloicauhon("..menPIdx..")")
		tinsert(OPTIONS, "Ta tõ chèi lêi cÇu h«n cña "..szNameMan.."./#tuchoicauhon("..menPIdx..")")
		PlayerIndex = temPIdx
	end
	tinsert(OPTIONS, "§Ó ta suy nghÜ l¹i./no")
	SayEx(OPTIONS)
end

function kethon2()
	Talk(1,"","DÉn ý trung nh©n cña <sex> ®Õn ®©y ta sÏ gióp")
end

function tuchoicauhon(pIdx)
	Talk(1,"","§· kh«ng thÝch th× d¾t nhau ®Õn t×m ta lµm g×? ®Ó ta tr¶ lêi víi h¾n lµ ng­¬i kh«ng muèn!")
	SetTaskTemp(HAVELOVE_FEMALE, 0)	
	local szNameWoman = GetName()
	PlayerIndex = pIdx
	SetTaskTemp(HAVELOVE_MALE, 0)
	Talk(1,"","C« n­¬ng "..szNameWoman.." nhê ta nãi l¹i víi ng­¬i lµ kh«ng muèn!")
end

function nhanloicauhon(pIdx)
	local temPIdx = PlayerIndex
	local szNameWoman = GetName()
	SetTaskTemp(HAVELOVE_FEMALE, 0)	
	PhanThuongMateName()
	Talk(1,"","Xin chóc mõng ®«i b¹n trÎ, chóc tr¨m n¨m h¹nh phóc sèng ®Õn ®Çu b¹c r¨ng long.")
	
	PlayerIndex = pIdx
	local szNameMan = GetName()
	SetTaskTemp(HAVELOVE_MALE, 0)
	PhanThuongMateName()
	Talk(1,"","Xin chóc mõng ®«i b¹n trÎ, chóc tr¨m n¨m h¹nh phóc sèng ®Õn ®Çu b¹c r¨ng long.")
	
	SetMateName(szNameWoman)
	PlayerIndex = temPIdx
	SetMateName(szNameMan)
	AddGlobalCountNews("Xin chóc mõng ®«i Trai tµi G¸i s¾c <color=yellow>["..szNameMan.."]<color> kÕt duyªn cïng <color=blue>["..szNameWoman.."]<color> chóc tr¨m n¨m h¹nh phóc sèng ®Õn ®Çu b¹c r¨ng long.",3)
end

function PhanThuongMateName()
	AddItem(6,1,11,0,0,0,0)
	Msg2Player("NguyÖt L·o ta cã chót quµ nhá tÆng hai ng­¬i ®Ó mõng ngµy c­íi")
end

function kethon()
	if(GetTaskTemp(HAVELOVE_MALE) > 0) then
		Talk(1,"",12955)
		return
	end
	if(GetMateName() ~= "") then
		if(GetSex() == 0) then
			Talk(1,"",12963)
		else
			Talk(1,"",12969)
		end
		return
	end
	if(GetSex() == 1) then
		Talk(1,"",12960)
		return
	end
	if(GetLevel() < REQ_LEVEL_MATENAME) then
		Talk(1,"",12964)
		Msg2Player("B¹n ch­a ®ñ "..REQ_LEVEL_MATENAME.." cÊp!")
		return 
	end
	local nSize = GetTeamSize()
	if(nSize < 2) then
		Talk(1,"",12970)
		return
	end
	if(nSize > 2) then
		Talk(1,"",10287)
		return 
	end
	local tempid = PlayerIndex
	local nTeamId = GetTeam()
	for i=0,7 do
		local MemId = GetTeamMem(nTeamId, i)
		if(MemId > 0 and MemId ~= tempid) then
			PlayerIndex = MemId
			local nNpcIdx = GetPlayerNpcIdx()
			local sex = GetSex()
			PlayerIndex = tempid
			nNpcIdx = FindAroundNpc(GetNpcID(nNpcIdx))
			if(nNpcIdx <= 0) then
				Talk(1,"",12970)
				return
			 end
			if(sex == 0) then
				Talk(1,"",12961)
				return 
			end
		end
	end
	PlayerIndex = tempid
	Say(12965,2,
	"§óng, ta muèn cÇu h«n c« Êy/cauhon",
	"§Ó ta suy nghÜ l¹i/no")
end;

function cauhon()
	if(GetMateName() ~= "") then
		return 
	end
	if(GetCamp() ~= 4 and GetFactionNo() == 0 and GetTongName() == "") then
		Talk(1,"",12959)
		return 
	end
	local nSize = GetTeamSize()
	if(nSize < 2) then
		Talk(1,"",12966)
		return 
	end
	if(nSize > 2) then
		Talk(1,"",10287)
		return 
	end
	local tempid = PlayerIndex
	local nTeamId = GetTeam()
	local DesId = 0;
	for i=0,7 do
		local MemId = GetTeamMem(nTeamId, i)
		if(MemId > 0 and MemId ~= tempid) then
			PlayerIndex = MemId
			local nNpcIdx = GetPlayerNpcIdx()
			local sex = GetSex()
			local love = GetMateName()
			local level = GetLevel()
			local camp = GetCamp()
			local facno = GetFactionNo()
			local tog = GetTongName()
			PlayerIndex = tempid
			nNpcIdx = FindAroundNpc(GetNpcID(nNpcIdx))
			if(nNpcIdx <= 0) then
			Talk(1,"",12966)
				return 
			end
			if(sex == 0) then
			Talk(1,"",12961)
				return 
			end
			if(love ~= "") then
			Talk(1,"",10290)
				return 
			end
			if(level < REQ_LEVEL_MATENAME) then
			Talk(1,"",10291)
				return 
			end
			if(camp ~= 4 and facno == 4 and tog == "") then
			Talk(1,"",12962)
				return 
			end
			DesId = MemId
		end
	end
	PlayerIndex = tempid
	SetTaskTemp(HAVELOVE_MALE, 1)
	local szNameMan = GetName()
	Talk(1,"",10292)
	PlayerIndex = DesId
	SetTaskTemp(HAVELOVE_FEMALE, tempid) --SetTimer(18,4)
	Talk(1,"","VÞ thiÕu hiÖp "..szNameMan.. " ngá lêi cÇu h«n ý c« n­¬ng thÕ nµo ®èi tho¹i vµ tr¶ lêi ta")
end

function lyhon()
	if(GetMateName() == "") then
		if(GetSex() == 0) then
			Talk(1,"",10314)
		else
			Talk(1,"",10315)
		end
		return 
	end
	if(GetSex() == 1) then
		Talk(1,"",10316)
		return 
	end
	local nSize = GetTeamSize()
	if(nSize < 2) then
		Talk(1,"",12979)
		return 
	end
	Say(12977,2,"Ta muèn ly h«n ngay lËp tøc/xacnhanhuy","§Ó ta c©n nh¾c l¹i/no")
end

function xacnhanhuy()
	local szLove1 = GetMateName()
	if(szLove1 == "") then
		return 
	end
	local nSize = GetTeamSize()
	if(nSize < 2) then
		Talk(1,"",12979)
		return 
	end
	if(nSize > 2) then
		Talk(1,"",10317)
		return 
	end
	local Name1 = GetName()
	local Name2
	local tempid = PlayerIndex
	local nTeamId = GetTeam()
	local womanPidx = 0
	for i=0,7 do
		local MemId = GetTeamMem(nTeamId, i)
		if(MemId > 0 and MemId ~= tempid) then
			PlayerIndex = MemId
			womanPidx = MemId
			Name2 = GetName()
			local szLove2 = GetMateName()
			local nNpcIdx = GetPlayerNpcIdx()
			PlayerIndex = tempid
			nNpcIdx = FindAroundNpc(GetNpcID(nNpcIdx))
			if(nNpcIdx <= 0) then
				Talk(1,"",12979)
				return 
			end
			if(szLove1 ~= Name2 or szLove2 ~= Name1) then
				Talk(1,"",12978)
				return 
			end
		end
	end
	PlayerIndex = tempid --nam
	if(GetCash() < NGANLUONG_LYHON) then
		Talk(1,"",12981)
		return 
	end
	SetMateName("")
	Pay(NGANLUONG_LYHON)
	Talk(1,"","B©y giê hai ng­¬i ®· kh«ng cßn thuéc vÒ nhau, sau nµy ®­êng ai nÊy ®i.")
	
	PlayerIndex = womanPidx
	SetMateName("")
	Talk(1,"","B©y giê hai ng­¬i ®· kh«ng cßn thuéc vÒ nhau, sau nµy ®­êng ai nÊy ®i.")
end

function khuyen()
	local nSize = GetTeamSize()
	if(nSize < 2) then
		if(GetSex() == 0) then
			Talk(1,"",12973)
		else
			Talk(1,"",12974)
		end
		return
	 end
	if(nSize > 2) then
		if(GetSex() == 0) then --nam
			Talk(1,"",12975)
		else --n÷
			Talk(1,"",12976)
		end
		return 
	end
	local nTeamId = GetTeam()
	local tempid = PlayerIndex
	local desid
	for i=0,7 do
		local nMemId = GetTeamMem(nTeamId, i)
		if(nMemId > 0 and nMemId ~= tempid) then
			desid = nMemId
		end
	end
	PlayerIndex = desid
	local Name2 = GetName()
	local Love2 = GetMateName()
	local nDesNpcId = GetPlayerNpcIdx()
	PlayerIndex = tempid
	local Name1 = GetName()
	local Love1 = GetMateName()
	if(FindAroundNpc(GetNpcID(nDesNpcId)) <= 0) then
		if(GetSex() == 0) then
			Talk(1,"",12973)
		else
			Talk(1,"",12974)
		end
		return 
	end
	
	if(Love1 == "" or Name1 ~= Love2 or Name2 ~= Love1) then
		PlayerIndex = tempid
		if(GetSex() == 0) then
			Talk(1,"",12975)
		else
			Talk(1,"",12976)
		end
		return 
	end
	Talk(1,"",12972)
	PlayerIndex = desid
	Talk(1,"",12972)
end

function no()
end