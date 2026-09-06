

danhsachskillhotro = 
{
{{4,6,8},{23,24,26},{43},{60,62},{77,79},{95,97},{115,116},{131,132},{151,152},{167,168},{1347,1372},{1972,1974},{2114,2136}}, -- [HOASON 01/09c] Hoa Son nhap mon; [VHTD 02/09] Vu Hon 1972/1974, Tieu Dao 2114/2136
{273,36,48,75,252,114,130,150,166,275,1358,1982,2123}, -- [HOASON 01/09c] 1358 Huyen Nhan Van Yen; [VHTD 02/09] 1982 Vu Muc Di Thu, 2123 Thien Ly Doc Hanh
}

function hotrokinangtp1(id)
if (GetMagicLevel(id) == 0) then
slknht = 0
else
slknht = floor(((GetMagicLevel(id) - 1) * 4) / 19)
end
return slknht
end;

function hotrokinangtp2(id)
if (GetMagicLevel(id) == 0) then
slknht = 0
else
slknht = floor(((GetMagicLevel(id) - 1) * 5) / 19)
end
return slknht
end;



function hotrokinangtp3(id)
if (GetMagicLevel(id) == 0) then
slknht = 0
else
slknht = floor(((GetMagicLevel(id) - 1) * 8) / 19)
end
return slknht
end;







function main()

player_Faction = GetFaction()

if (player_Faction == "´äÑÌÃÅ") then 
mp = 6
elseif (player_Faction == "Îå¶¾½Ì") then
mp = 4
elseif (player_Faction == "ÌìÍõ°ï") then
mp = 2
elseif (player_Faction == "ÉÙÁÖÅÉ") then
mp = 1
elseif (player_Faction == "Îäµ±ÅÉ") then
mp = 9
elseif (player_Faction == "ÌìÈÌ½Ì") then
mp = 8
elseif (player_Faction == "ÌÆÃÅ") then
mp = 3
elseif (player_Faction == "À¥ÂØÅÉ") then
mp = 10
elseif (player_Faction == "¶ëáÒÅÉ") then
mp = 5
elseif (player_Faction == "Ø¤°ï") then
mp = 7
elseif (player_Faction == "»ªÉ½ÅÉ") then -- [HOASON 01/09c]
mp = 11
elseif (player_Faction == "Îä»êÌÃ") then -- [VHTD 02/09]
mp = 12
elseif (player_Faction == "åÐÒ£ÅÉ") then -- [VHTD 02/09]
mp = 13
else 
mp = 0
end

soskillht = getn(danhsachskillhotro[1][mp])

diemskilltoida = 20 + GetTask(197)
for i=1,soskillht do
if (soskillht == 3) then
diemskilltoida = diemskilltoida + hotrokinangtp1(danhsachskillhotro[1][mp][i])
elseif (soskillht == 2) then
diemskilltoida = diemskilltoida + hotrokinangtp2(danhsachskillhotro[1][mp][i])
elseif (soskillht == 1) then
diemskilltoida = diemskilltoida + hotrokinangtp3(danhsachskillhotro[1][mp][i])
end

end

if (GetLevel() >= 60) then

if (GetMagicLevel(danhsachskillhotro[2][mp]) >= diemskilltoida) then
Msg2Player("Ki nang da linh ngo den cap do gioi han, hien tai khong the linh ngo them")
else

if (GetMagicPoint()>=2) then

AddMagicPoint(-2)

AddMagic(danhsachskillhotro[2][mp],GetMagicLevel(danhsachskillhotro[2][mp]) + 1)

Msg2Player("Ki nang linh ngo len cap "..GetMagicLevel(danhsachskillhotro[2][mp]))

else

Msg2Player("khong du 2 diem ki nang, khong the linh ngo them ")

end

end
else
Msg2Player("Khong du dang cap 60")
end



end;

		


 
