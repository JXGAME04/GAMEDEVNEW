-- @IncludeOnce  [LUA54 06/09 toi] tep toan ham: Lua54Dll chi chay than mot lan moi state (chon_includeonce.py)
--	Author: Fong Ki?u
--	Date: 23/10/2020
--	Desc: Npc Lap bang hoa son kim son chuong mon

function main2()

	dofile("script/global/npcchucnang/lapbang.lua")

	if (GetTongName() ~= "") then
		Say("ChØ c? nh©n sü tù do míi ®­îc thµnh lËp bang héi.",0)
		return
	else
		local sSay = "ng­¬i c? chuyÖn g× kh«ng ?"
		local sMsg = {}
		tinsert(sMsg,"Thµnh lËp bang héi/CreatTong")
		tinsert(sMsg,"T×m hiÓu bang héi/about_tong")
		tinsert(sMsg,"§i?u kiÖn lËp bang/QuestionTong")
		tinsert(sMsg,"K?t thóc ®èi tho¹i/Tong_Cancel")
		Say(sSay,getn(sMsg),sMsg)
	end
end

function QuestionTong()
	Talk(4,Tong_Cancel,"Muèn thµnh lËp bang tr­íc h?t ph¶i XuÊt S­, ","Sau ®? ng­¬i ph¶i thu thËp ®­îc Nh¹c V­¬ng Ki?m, ", "§¼ng cÊp ph¶i ®¹t trªn 50 CÊp, tµi l·nh ®¹o trªn 20 cÊp, danh väng trªn 300 ®iÓm ", "Ti?n v¹n tiªu hao 100 v¹n l­îng. C? ®ñ ®i?u kiÖn h·y ®?n gÆp ta. ")
end

function Tong_Cancel()
end

function CreatTong()
	local sMsg = ""
	if GetCamp() ~= 4 then
		sMsg = "Ng­¬i ph¶i xuÊt sö míi c? thÓ thµnh lËp Bang héi. "
		Say(sMsg,0)
		return 
	end
	
	OpenTong(0)
end

function about_tong() 
	Talk(12,"","Ng­¬i hái thµnh lËp bang héi c? ®i?u kiÖn g× , ta tíi t? t? n?i cho ng­¬i nghe !","§Çu tiªn ng­¬i nhÊt ®?nh ph¶i ®· xuÊt s­ ®éc lËp x«ng x¸o giang hå","ti?p theo ng­¬i kh«ng thÓ ë bÊt kú bang héi trung lÇn n÷a ng­¬i muèn cô c? nhÊt ®?nh giang hå danh väng","cuèi cïng ng­¬i thèng suÊt lùc kh«ng thÓ nhá víi 30 cÊp !","§¹t tíi nh÷ng thø nµy yªu cÇu sau ng­¬i chØ cÇn t? tèng kim trªn chi?n tr­êng ®¹t ®­îc mét thanh nh¹c v­¬ng ki?m","lÊy nµy lµm bang chñ t?n vËt lµ ®­îc råi . n?u nh­ ng­¬i thµnh lËp ®?ch bang ph¸i c? ®Çy ®ñ nhi?u ng­êi ñng hé","c¸i nµy bang héi lµ c? thÓ trë thµnh ch?nh thøc bang héi mµ vËn hµnh ","§Çu tiªn ng­¬i ph¶i c? n¨ng lùc , ë 3 ngµy bªn trong ?t nhÊt ph¶i t×m ®­îc 16 c¸ cïng ng­¬i ch? thó t­¬ng ®Çu","®¾c ng­êi cña gia nhËp , n?u nh­ kh«ng ®ñ li?n kh«ng c? kh«ng c? biÖn ph¸p kh¸c","c¸i nµy 3 ngµy c? thÓ gäi lµ kh¶o nghiÖm kú '."," ë ng­¬i bang héi c? 16 ng­êi sau giang hå sÏ cho ng­¬i kh¶o nghiÖm","n?u nh­ 3 h«m sau kh«ng ®ñ 16 ng­êi giang hå c?ng kh«ng th?a nhËn ng­¬i bang héi ","Cuèi cïng , thµnh lËp bang héi ph¶i c? ®¹i l­îng ti?n b¹c . cè g¾ng ®¹t tíi môc tiªu ®i !") 
end 