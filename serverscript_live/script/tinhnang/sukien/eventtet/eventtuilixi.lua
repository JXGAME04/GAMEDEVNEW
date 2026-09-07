
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\test\\event_rank.lua")

function main()
dofile("script/event/eventtet/eventtuilixi.lua")
    local tbOpp = {
		"Xem B¶ng XÕp H¹ng/xemxephang",
		"§æi ThiÖp N¨m Míi/doieventtet",
		"Tho¸t khái/NO"
    };

    SayNew("<color=yellow>Th«ng tin Event<color>:\nYªu CÇu: 25 ThiÖp N¨m Míi §æi LÊy Mét R­¬ng HKMP Khãa\n -50 ThiÖp N¨m Míi §æi LÊy Mét Ngùa Giíi H¹n", getn(tbOpp), tbOpp);
end;


function xemxephang()
Event_SaveRankManual()
Event_ShowRank()
end
function doieventtet()
	local tbOpp = {
		"§æi 25 ThiÖp N¨m Míi §æi LÊy Mét R­¬ng HKMP Khãa/okaydoilien",
		-- "§æi 50 ThiÖp N¨m Míi §æi LÊy Mét Ngùa Giíi H¹n/doieventtet",
		"Tho¸t khái/NO"
    };
	SayNew("<color=yellow>Th«ng tin Event<color>:\nYªu CÇu: 25 ThiÖp N¨m Míi §æi LÊy Mét R­¬ng HKMP Khãa\n -50 ThiÖp N¨m Míi §æi LÊy Mét Ngùa Giíi H¹n", getn(tbOpp), tbOpp);
end

function okaydoilien()
	local nTichLuy = 25
	local nTM  = GetItemCount(0,6,1, 139, -1, -1, pos_equiproom)
	if nTM < nTichLuy then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..nTichLuy.." ThiÖp N¨m Míi.")
		return
	end
	DelItem(0, -1, 6,1 ,139, -1, -1, pos_equiproom, nTichLuy)
	AddItemSL(4855,1,-2)  -- ruong hkmp 
	Msg2Player("Xin chóc mõng ®· nhËn ®­îc Mét R­¬ng HKMP Khãa")
end

function NO()
end;



