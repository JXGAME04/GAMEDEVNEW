-- [LOCAL54 06/09 toi] 2 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local hotrokinang9xt, hotrokinang9xc


danhsachskillhotro = {

{
{{14,10,10},{271,11,19}},
{{31,35,37},{325,41,32}},
{{54,50,47},{58,249,357}},
{{333,68},{74,71}},
{{334,82},{88,91}},
{{105,113},{108,111}},
{{119,122},{125,128}},
{{138,141},{148,142}},
{{158,164},{267,165}},
{{169,172},{176,182}},
{{1349,1374},{1355,1379}}, -- [HOASON 01/09c]
{{1964,1975},{1963,1979}}, -- [VHTD 02/09] Vu Hon
{{2115,2137},{2120,2140}}, -- [VHTD 02/09] Tieu Dao
},
{
{350,351,353},
{345,347,342},
{302,355,358},
{361,359},
{374,370},
{377,378},
{389,362},
{367,365},
{380,382},
{385,384},
{1364,1382}, -- [HOASON 01/09c]
{1967,1983}, -- [VHTD 02/09] Vu Hon 90
{2124,2141} -- [VHTD 02/09] Tieu Dao 90
}

}



























function hotrokinang9xt(id)
if (GetMagicLevel(id) == 0) then
slknht = 0
elseif (GetMagicLevel(id) <= 20) then
slknht = floor(((GetMagicLevel(id) - 1) * 3) / 19)
elseif (GetMagicLevel(id) > 20) then
slknht = 3
end
return slknht
end;

function hotrokinang9xc(id)
if (GetMagicLevel(id) == 0) then
slknht = 0
elseif (GetMagicLevel(id) <= 20) then
slknht = floor(((GetMagicLevel(id) - 1) * 7) / 19)
elseif (GetMagicLevel(id) > 20) then
slknht = 7
end
return slknht
end;






function skill9x(vt)

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


diemskilltoida = 10 + hotrokinang9xt(danhsachskillhotro[1][mp][1][vt]) + hotrokinang9xc(danhsachskillhotro[1][mp][2][vt]) + GetTask(197)

if (GetLevel() >= 90) then
if (GetMagicLevel(danhsachskillhotro[2][mp][vt]) >= diemskilltoida) then
Msg2Player("Ki nang da linh ngo den cap do gioi han, hien tai khong the linh ngo them")
else

if (GetMagicPoint()>=1) then

AddMagicPoint(-1)

AddMagic(danhsachskillhotro[2][mp][vt],GetMagicLevel(danhsachskillhotro[2][mp][vt]) + 1)

Msg2Player("Ki nang linh ngo len cap "..GetMagicLevel(danhsachskillhotro[2][mp][vt]))

else

Msg2Player("khong du 1 diem ki nang, khong the linh ngo them ")

end

end

else
Msg2Player("Khong du dang cap 90")
end


end;


 
