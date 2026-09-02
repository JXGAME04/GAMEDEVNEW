Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]

function main(sel)
	Uworld_jmdz = GetTask(3487)
	if (GetFaction() == "huashan") or (Uworld_jmdz == 30*256) then
		Say("Phô Dung: Lo¹i thuèc nµy lµ do lóc LËn s­ tû r·nh rçi ®· dÉn c¸c s­ tû muéi ®i h¸i thuèc lµ chÕ t¹o, rÊt lµ linh nghiÖm.", 2, "Giao dÞch/yes", "Kh«ng giao dÞch/no")
	else
		Talk(1,"","Phô Dung: Ch­ëng m«n cã lÖnh, thuèc cña bæn ph¸i chØ b¸n cho ®ång m«n.")
	end
end;

function yes()
Sale(53)
end;

function no()
end;
