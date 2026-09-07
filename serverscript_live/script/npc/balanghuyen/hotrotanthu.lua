-- [LOCAL54 06/09 toi] 2 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local check_code_3, OKGiftCode
--Author: Fong KiÒu
--Date: 07/07/2021
--Function: Hç trî t©n thñ

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("script\\global\\hocvocong.lua")
Include("script\\global\\trangbixanh.lua")
Include("script\\global\\npcchucnang\\lapbang.lua")
Include("\\script\\codenew.lua")
-- Include("\\script\\giftcode_fancung.lua")
-- -- Include("\\script\\giftcode_new.lua")

function main()
	 dofile("script/npcthon/balanghuyen/hotrotanthu.lua")
local nCurTime = tonumber(GetLocalDate("%y%m%d%H%M"))
if(nCurTime < SERVER_OPEN) then
Talk(1,"","§óng 19h míi Open Server  .")
return 1
end
	SayEx({"<color=green><npc>: <color>Chèn th«n lµng yªn tØnh nµy, kh«ng muèn cã sù quÊy ph¸. <sex> ®Õn ®©y ®Ó lµm g× ?",
	--"NhËn LÖnh bµi t©n thñ/lenhbaitanthu346",
	
	"LËp bang/main2",
	"NhËn hç trî t©n thñ/nhanhotrotanthu",  -- oke
	"NhËn hç trî CÊp 110 /hotrocap90",  -- oke
	"Giftcode Phi Phong Th¸ng 7/giftcodethienhoang",  -- oke
	-- "NhËn l¹i Kim Phong /nhankp",  -- oke
	-- "NhËn th­ëng GiftCode/GiftCode",  -- oke
	"GiftCode Like + Chia sÏ/giftcodedenbu",
	-- "NhËn th­ëng GiftCode §Òn Bï/giftcodedenbu",  -- oke	
	"Hç trî vò khÝ c¬ b¶n/laydoxanh0",
	
	"Ta chØ ®Õn th¨m «ng./no"})
end


function giftcodedenbu()
if CalcFreeItemCellCount() < 30 then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ 30 « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return
		end	

	if GetTask(TASK_TANTHU2) == 0 then
			SetTask(TASK_TANTHU2,1)
			for i=238,240 do
			local TT = AddItem(4,i,0,0,0,0,0 )
			SetPlayerItemLock(TT, -2)  -- TT
			end
			for i=1,6 do
			local THBT = AddItem(4,353,0,0,0,0,0 )
			SetPlayerItemLock(THBT, -2)  -- THBT
			end
			local BTL = AddItem(6,1,4822,0,0,0,0)
				SetPlayerItemLock(BTL, -2)
				
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do

			AddItemSL(4847,500,-2)  -- chan nguyen don
			AddItemSL(4847,500,-2)  -- chan nguyen don
			AddItemSL(4847,500,-2)  -- chan nguyen don
			AddItemSL(4847,500,-2)  -- chan nguyen don
			Msg2SubWorld("<color=blue> Chóc mõng <color> <color=yellow>"..GetName().." <color=blue> ®· nhËn ®­îc<color=white> GiftCode Like + Chia SÏ <color> ")
	else
		Talk(1,"","Xin lçi b¹n ®· nhËn giftcode nµy råi !")
	end
end

function nhankp()
if GetTask(TASK_DUPHONG5) >= 1 then
Talk(1,"","B¹n ®· nhËn råi kh«ng thÓ nhËn n÷a!")
return
	end	
local nIndex ={};
for i=177,185 do 
	nIndex[i]  = AddGoldItem(i,0)
	SetPlayerItemLock(nIndex[i], -2) --kho¸ vÜnh viÔn
end
SetTask(TASK_DUPHONG5, GetTask(TASK_DUPHONG5) + 1)  -- céng thªm 1 b»ng 2
end

function giftcodethienhoang()
if CalcFreeItemCellCount() < 20 then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ 50 « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return
		end	
	-- if(GetLevel() < 85) then
		-- Talk(1,"","<sex> ch­a ®ñ level 85 kh«ng thÓ tham gia ho¹t ®éng")
		-- return
	-- end
	
	if GetTask(TASK_DUPHONG4) == 0 then
			AddItem2(2,0,7357,0,0,0)  -- phi phong 
			AddItem2(2,0,3498,0,0,0)  -- phi phong  
			Msg2SubWorld("<color=blue> Chóc mõng <color> <color=yellow>"..GetName().." <color=blue> ®· nhËn ®­îc<color=white> GiftCode Th¸ng 7 <color><color=blue> Xin chóc mõng !<color> ")

			SetTask(TASK_DUPHONG4, 1)  -- céng thªm 1 b»ng 2 
			-- Msg2SubWorld("Chóc mõng <color=yellow>"..GetName().."<color> ®· nhËn ®­îc Giftcode Set Thiªn Hoµng kho¸ ! ")
	else
		Talk(1,"","Xin lçi b¹n ®· nhËn giftcode nµy råi !")
	end
end
function nhapmacodenheae()
OpenGetNumber("NhËp GiftCode","nhancodegioihan")
end

function nhangiftocdenew()

	if GetTask(TASK_DUPHONG2) > 0 then
		Talk(1,"","Nhµ ng­¬i ®· nhËn råi! Nhµ ng­¬i bÞ ng¸o ®¸ kh«ng ? ");
	return end;


		local TTL8X = AddItem(6,1,71,0,0,0,0) --- x2 ky nang
		SetPlayerItemLock(TTL8X, -2)
		AddTimeItem(TTL8X,60*60*24*7);  -- thêi gia item 7 ngµy
		local TTL8X = AddItem(6,1,71,0,0,0,0) --- x2 ky nang
		SetPlayerItemLock(TTL8X, -2)
		AddTimeItem(TTL8X,60*60*24*7);  -- thêi gia item 7 ngµy
		-- -X2 KY NANG
		local BTL = AddItem(6,1,4822,0,0,0,0) --- x2 ky nang
		SetPlayerItemLock(BTL, -2)
		AddTimeItem(BTL,60*60*24*7);  -- thêi gia item 7 ngµy
		local BTL = AddItem(6,1,4822,0,0,0,0) --- x2 ky nang
		SetPlayerItemLock(BTL, -2)
		AddTimeItem(BTL,60*60*24*7);  -- thêi gia item 7 ngµy
		
	SetTask(TASK_DUPHONG2,1);
	Msg2SubWorld("<color=yellow>Chóc mõng <color=blue>"..GetName().." <color>®· nhËn ®­îc hç trî new t¹i Hç Trî T©n Thñ.")
end

function nhancodegioihan()
thoigian = tonumber(date("%H%M%d%m"))

if(CheckRoom(6,6) == 0) then
		Talk(1, "", 12266)
	return end;

m_codeevent = check_code_3()

		if GetTask(TASK_TANTHU2) > 0 then
			Talk(1,"","Nh©n vËt nµy ®· sö dông Gift Code, kh«ng thÓ sö dông thªm lÇn n÷a")
			return	
		end
		if m_codeevent  > 0 then

	    if Code_New[m_codeevent][2] == 0 then
	
			-- thoigian = tonumber(date("%H%M%d%m"))
			-- LoginLog = openfile("dulieu/logGiaHanGiftCodeLikeNew.txt", "a");
			-- if LoginLog then
			-- write(LoginLog,""..GetAccount().." - "..GetName().." -"..GetLevel().."  - Time: "..thoigian.."\n");
			-- end
			-- closefile(LoginLog)
	
	      
			Code_New[m_codeevent][2] = thoigian
			SetTask(TASK_TANTHU2,1)
			-- if (GetLevel() < 85) then
				-- for i = GetLevel(), 85-1 do 
				-- AddOwnExp(100000000000)
				-- end
			-- end
			-- i = random(1,10)
			-- mIndex = AddItem(0,10,5,i,0,0,10)
			-- SetPlayerItemLock(mIndex, -2)
			-- AddTimeItem(mIndex,60*60*24*3);  -- ngùa 8x thêi gia item 3 ngµy
			
			for i=238,240 do
			local TT = AddItem(4,i,0,0,0,0,0 )
			SetPlayerItemLock(TT, -2)  -- TT
			end
			
			for i=1,6 do
			local THBT = AddItem(4,353,0,0,0,0,0 )
			SetPlayerItemLock(THBT, -2)  -- THBT
			end
			
	
			-- for i=1,3 do
			-- local BCH = AddItem(6,1,4824,0,0,0,0 )
			-- SetPlayerItemLock(BCH, -2)  -- BCH
			-- end
			local BTL = AddItem(6,1,4822,0,0,0,0)
				SetPlayerItemLock(BTL, -2)
				
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do

			AddItemSL(4847,500,-2)  -- chan nguyen don
			AddItemSL(4847,500,-2)  -- chan nguyen don
			AddItemSL(4847,500,-2)  -- chan nguyen don
			AddItemSL(4847,500,-2)  -- chan nguyen don
			Msg2SubWorld("<color=blue> Chóc mõng <color> <color=yellow>"..GetName().." <color=blue> ®· nhËn ®­îc<color=white> GiftCode Like + Chia SÏ <color><color=blue> thµnh c«ng 3 BCH, 1 B¶o Tiªn Lé (x2 kü n¨ng), 1 Bé THBT & Thuû Tinh<color> ")

        	
        	
			local nNAME_Change = TaoBang(Code_New,"Code_New")
			SaveData("script/codenew.lua",nNAME_Change)
			
	else
			Talk(1,"","Gift Code ®· ®­îc ng­êi kh¸c sö dông ")
	end
else
Talk(1,"","Seri hoÆc m· code kh«ng ®óng, GIFT CODE kh«ng tån t¹i !")
end

end

function check_code_3()
for i=1,getn(Code_New) do
	if GetNumberFromUI()  == Code_New[i][1] then
		return i
	end
end
return 0
end

GIFTCODE = "VLNGAOTHE"
FREECELL_TANTHU = 50

function nhanhotrotanthu()
		if (GetTask(TASK_TANTHU) > 0) then
		Talk(1,"","Nhµ ng­¬i ®· nhËn råi! §õng cã ng¸o n÷a !");
		return end;
		if CalcFreeItemCellCount() < FREECELL_TANTHU then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ "..FREECELL_TANTHU.." « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return
		end	
		-- hotrocap90()
		local nIndex ={};
		--- THO DI PHU + THAN HANH PHU
		local mIndex;
		mIndex = AddItem(6,1,1271,0,0,0,0)
		SetPlayerItemLock(mIndex, -2) --kho¸ vÜnh viÔn
		mIndex = AddItem(6,1,437,0,0,0,0)
		SetPlayerItemLock(mIndex, -2) --kho¸ vÜnh viÔn
		-- AddTimeItem(mIndex,60*60*24*7);  -- thêi gia item 7 ngµy
		--- TUI MAU TAN THU
		mIndex = AddItem(6,1,4813,0,0,0,0)
		SetPlayerItemLock(mIndex, -2)
		AddTimeItem(mIndex,60*60*24*5);  -- thêi gia item 5 ngµy
	
		mIndex = AddItem(0,10,2,10,0,0,10)
		SetPlayerItemLock(mIndex, -2)
		-- AddTimeItem(mIndex,60*60*24*7);  -- thêi gia item 7 ngµy
		--- NHAN KIM PHONG
	for i=176,186 do 
	nIndex[i]  = AddItem2(2,0,i,0,0,0)  -- dinh quoc 
	SetPlayerItemLock(nIndex[i], -2) --kho¸ vÜnh viÔn
	AddTimeItem(nIndex[i],60*60*24*30);  -- thêi gia item 30 ngµy
	end 	
	for i = 0,8 do 
	local nMeridianType = i;
	local nMeridianLevel = 4;
	SetMeridian(nMeridianType, nMeridianLevel);
	end
	-- if (GetLevel() < 110) then
		 -- for i = GetLevel(), 110-1 do 
		 -- AddOwnExp(100000000000)
		 -- end
	-- end
	Msg2SubWorld("<color=white>Cao Thñ : <color> <color=yellow>"..GetName().." <color=white> ®· gia nhËp M¸y Chñ Hoa S¬n giang hå s¾p dËy lªn mét phen sãng giã <color>")	
	SetTask(TASK_TANTHU, 1);
end

function hotrocap90()
	if (GetTask(TASK_TANTHU) < 1) then
		Talk(1,"","Ng­¬i hay nhËn hç trî t©n thñ ! NhËn cÊp dµnh cho ng­êi ch¬i cñ !");
		return end;
	if CalcFreeItemCellCount() < FREECELL_TANTHU then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ "..FREECELL_TANTHU.." « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return
		end	
	if (GetLevel() >= 110) then
		Talk(1,"","Ng­¬i trªn cÊp 110 kh«ng thÓ nhËn!");
		return end;
	if (GetLevel() < 110) then
		 for i = GetLevel(), 110-1 do 
		 AddOwnExp(100000000000)
		 end
	end
	
	AddMagic(210, 1)
	local nIndex ={};
	for i=397,401 do 
	nIndex[i]  = AddItem2(2,0,i,0,0,0)  -- dinh quoc 
	SetPlayerItemLock(nIndex[i], -2) --kho¸ vÜnh viÔn
	end  
	for i=203,206 do 
	nIndex[i]  = AddItem2(2,0,i,0,0,0)  -- dinh quoc 
	SetPlayerItemLock(nIndex[i], -2) --kho¸ vÜnh viÔn
	AddTimeItem(nIndex[i],60*60*24*30);  -- thêi gia item 30 ngµy
	end 	
end
	
function hotrocap10()
	local nLevel = 90
	local maxLevel = 80
	if(GetLevel() + nLevel > maxLevel) then
		nLevel = maxLevel - GetLevel()
		LEVERUP(nLevel)
	else
		LEVERUP(nLevel)
	end
	AddMagic(210, 1)
	if(nLevel > 0) then
		Talk(1,"","Xin chóc mõng <sex> ®· t¨ng thªm "..nLevel.." cÊp")
	end
end

function LEVERUP(nLevel)
	for i = 1, nLevel do 
		AddOwnExp(100000000000)
	end
end


function GiftCode()

	if GetTask(TASK_TANTHU1) > 0 then
			Talk(1,"","Nhµ ng­¬i ®· nhËn råi!");
		return end;
		
	if (CalcFreeItemCellCount() <  6) then
		Talk(1,"","H·y dän trèng 6 « trèng ®Ó ®¶m b¶o tµi s¶n!")
		return
	end
	
OpenGetString("NhËp GiftCode","CheckCode")
end

function CheckCode()
	if (GetStringFromUI() == GIFTCODE) then
		OKGiftCode()
	else
		Talk(1,"","B¹n ®· nhËp sai m· <color=red>GiftCode <color>råi!")
	end
end

function OKGiftCode()

		if GetTask(TASK_TANTHU1) > 0 then
		Talk(1,"","Nhµ ng­¬i ®· nhËn råi!");
	return end;
	-- for i=238,240 do
	-- local TT = AddItem(4,i,0,0,0,0,0)
	-- SetPlayerItemLock(TT, -2)
	-- end
	local VLMT = AddItem(6,1,26,0,0,0,0)
	SetPlayerItemLock(VLMT, -2)
	local TTK = AddItem(6,1,22,0,0,0,0)
	SetPlayerItemLock(TTK, -2)
	-- local BTL = AddItem(6,1,4822,0,0,0,0)
	-- SetPlayerItemLock(BTL, -2)
	SetTask(TASK_TANTHU1, 1);
	Msg2Player("<color=yellow>Chóc mõng <color=blue>"..GetName().." <color>®· nhËn ®­îc phÇn th­ëng GIFTCODE T©n Thñ.")
end



function no()
SetPKMode(0,0)
end

Include("\\script\\test\\npcchat.lua")
-- Constants


function OnTimer(nNpcIndex, nTimeOut)
	NpcChatHoTro(nNpcIndex)
	SetNpcTimer(nNpcIndex, 18 * timer_sec_chat)
end