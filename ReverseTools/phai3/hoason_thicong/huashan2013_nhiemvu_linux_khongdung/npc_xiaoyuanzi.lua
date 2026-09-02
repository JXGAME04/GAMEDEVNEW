Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]

function main(sel)
	Uworld_jmdz = GetTask(3487)
	if (GetFaction() == "huashan") or (Uworld_jmdz == 30*256) then
		Say("TiÓu Viªn Tö: ë ®©y cã rÊt nhiÒu thÇn binh lîi khİ, ng­¬i cã muèn mét mãn kh«ng?", 2, "Giao dŞch/yes", "Kh«ng giao dŞch/no")
	else
		Talk(1,"","TiÓu Viªn Tö: Ch­ëng m«n cã lÖnh, binh khİ bæn ph¸i chØ b¸n cho ®ång m«n.")
	end
end;

function yes()
Sale(51)
end;

function no()
end;
