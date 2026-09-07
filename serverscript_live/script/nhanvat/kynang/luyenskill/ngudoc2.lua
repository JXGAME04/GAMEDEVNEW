Include("\\script\\skill\\luyenskill\\skillexp.lua")

function main()
Say("Chon Chuc Nang: ",3,
"Chon Skill Luyen./chonskill",
"Up Skill./nangskill",
"Thoat./no")
end
function no()
end
function chonskill()
SetTask(23,2)
Talk(1,"","Da Chon Ky Nang Can Luyen Thanh Cong")
end

function nangskill()
for k=1,getn(ThanhVip) do
if GetTask(25) >= ThanhVip[k][2] and GetTask(25) < ThanhVip[k+1][2] then
Msg2Player("Ky nang Luyen Duoc: "..GetTask(25).."/"..ThanhVip[k+1][2].."")
DelMagic(361)
AddMagic(361,ThanhVip[k][1])
end
end
end