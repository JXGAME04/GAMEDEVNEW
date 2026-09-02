Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]

function main(sel)
	Uworld_jmdz = GetTask(3487)
	if (GetFaction() == "huashan") or (Uworld_jmdz == 30*256) then
		Say("M¹c Phong Nhø: Hµnh tÈu giang hå, trang bÞ tèt kh«ng thÓ thiÕu.", 2, "Giao dÞch/yes", "Kh«ng giao dÞch/no")
	else
		Talk(1,"","M¹c Phong Nhø: Ch­ëng m«n cã lÖnh, trang bÞ bæn ph¸i cho b¸n cho ®ång m«n. ")
	end
end;

function yes()
Sale(55)
end;

function no()
end;
