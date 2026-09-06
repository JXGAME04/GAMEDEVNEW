-- [VHTD 02/09d] port Linux thuyenphu_vuhon.lua (Describe -> Say, cung API JX1: NewWorld, SetFightState)
function main()
	Say("ThuyÒn Phu: Ng­¬i muèn ®i ®©u?", 3, "§Õn Ba L¨ng HuyÖn/di1", "§Õn L©m An/di2", "Ta kh«ng ®i ®©u c¶/no")
end

function di1()
	NewWorld(53, 1794, 3157)
	SetFightState(1)
end

function di2()
	NewWorld(176, 1607, 2553)
	SetFightState(1)
end

function no()
end

function OnRevive()
end
