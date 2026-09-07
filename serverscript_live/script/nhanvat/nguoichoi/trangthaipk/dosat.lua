function main()
dofile("script/player/trangthaipk/dosat.lua")

if GetPK() >= 10 then
	Msg2Player("TrŞ PK cña ng­¬i qu¸ nÆng chØ cã vµo Nhµ Lao míi cã thÓ röa s¹ch téi")
	return
end
--kiemtra = KiemTraKhoaRuong()
--if kiemtra == 1 and GetFightState() == 0 then
--Msg2Player("Kh«ng thÓ chuyÓn tr¹ng th¸i khi ®ang ë trong thµnh.");
--else if kiemtra == 0 then
--Msg2Player("Ch­a më khãa kh«ng thÓ thao t¸c")
--else
SetPKState(2)
--end
--end
end