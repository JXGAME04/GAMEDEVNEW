-- [VHTD 02/09d] port Linux thuyenphu_tieudao.lua (Describe -> Say, cung API JX1: NewWorld, SetFightState)
function main()
	Say("ThuyÒn Phu: Ng­¬i muèn ®i ®©u?", 3, "§Õn T©y S¬n Th«n/di1", "§Õn L©m An/di2", "Ta kh«ng ®i ®©u c¶/no")
end

function di1()
	NewWorld(175, 1712, 3125)
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
