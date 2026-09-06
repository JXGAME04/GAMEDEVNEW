LV120SKILL_LIST =
{
	[0] = {709, 1, 0, 120, "Kü n¨ng cÊp 120", "§¹i Thõa Nh­ Lai Chó "},
	[1] = {708, 1, 1, 120, "Kü n¨ng cÊp 120",	"§¶o H­ Thiªn"},
	[2] = {710, 1, 2, 120, "Kü n¨ng cÊp 120",	"Mª ¶nh Tung"},
	[3] = {711, 1, 3, 120, "Kü n¨ng cÊp 120",	"HÊp Tinh YÓm"},
	[4] = {712, 1, 4, 120, "Kü n¨ng cÊp 120",	"BÕ NguyÖt PhÊt TrÇn"},
	[5] = {713, 1, 5, 120, "Kü n¨ng cÊp 120",	"Ngù TuyÕt Èn"},
	[6] = {714, 1, 6, 120, "Kü n¨ng cÊp 120",	"Hçn Thiªn KhÝ C«ng"},
	[7] = {715, 1, 7, 120, "Kü n¨ng cÊp 120",	"Ma ¢m PhÖ Ph¸ch"},
	[8] = {716, 1, 8, 120, "Kü n¨ng cÊp 120",	"XuÊt ø BÊt NhiÔm"},
	[9] = {717, 1, 9, 120, "Kü n¨ng cÊp 120",	"L­ìng Nghi Ch©n KhÝ "},
	[10] = {1365, 1, 10, 120, "Kü n¨ng cÊp 120",	"Tö Hµ KiÕm KhÝ"},	-- [HOASON 01/09]
	[11] = {1984, 1, 11, 120, "Kü n¨ng cÊp 120",	"Trung Vu L­u Phong"},	-- [VHTD 02/09]
	[12] = {2127, 1, 12, 120, "Kü n¨ng cÊp 120",	"¡n Uèng No Say"},	-- [VHTD 02/09]
}

Include("\\script\\item\\skillbook.lua")

function main(sel)
	
	if GetFactionNo()==-1 then	
		Msg2Player("<color=yellow>Ch­a gia nhËp m«n ph¸i, kh«ng thÓ häc kü n¨ng nµy!<color>")
		return 1
	end
	
	if GetLevel()<120 then
		Msg2Player("<color=yellow>§¼ng cÊp ch­a ®ñ 120, kh«ng thÓ häc kü n¨ng nµy!<color>")
		return 1
	end
	
	if (LearnSkillByBook(GetFactionNo(), LV120SKILL_LIST) == 0) then
		RemoveItem(sel,1)
		return 0
	end
	return 1
end
