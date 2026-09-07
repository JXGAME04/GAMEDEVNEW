-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local hotrokinang5x


danhsachskillhotro = {

{{14,10,10},{271,11,19}},
{{29,30,34},{325,41,32}},
{{45,45,45},{58,249,357}},
{{65,63},{74,71}},
{{85,80},{88,91}},
{{102,99},{108,111}},
{{119,122},{125,128}},
{{145,135},{148,142}},
{{155,153},{267,165}},
{{169,179},{176,182}},
{{1349,1374},{1355,1379}}, -- [HOASON 01/09c] Hoa Son: ho tro = tong quyet 10 (kiem/khi), 5x = 1355/1379
{{1964,1975},{1963,1979}}, -- [VHTD 02/09] Vu Hon: ho tro 20 = 1964/1975, 5x = 1963/1979
{{2115,2137},{2120,2140}} -- [VHTD 02/09] Tieu Dao: 20 = 2115/2137, 5x = 2120/2140

}











function hotrokinang5x(id)
if (GetMagicLevel(id) == 0) then
slknht = 0
elseif (GetMagicLevel(id) <= 20) then
slknht = floor(((GetMagicLevel(id) - 1) * 5) / 19)
elseif (GetMagicLevel(id) > 20) then
slknht = 5
end
return slknht
end;







function skill5x(vt)

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


diemskilltoida = 15 + hotrokinang5x(danhsachskillhotro[mp][1][vt])
if (GetLevel() >= 50) then

if (GetMagicLevel(danhsachskillhotro[mp][2][vt]) >= diemskilltoida) then
Msg2Player("Ki nang da linh ngo den cap do gioi han, hien tai khong the linh ngo them")
else

if (GetMagicPoint()>=1) then

AddMagicPoint(-1)

AddMagic(danhsachskillhotro[mp][2][vt],GetMagicLevel(danhsachskillhotro[mp][2][vt]) + 1)

Msg2Player("Ki nang linh ngo len cap "..GetMagicLevel(danhsachskillhotro[mp][2][vt]))

else

Msg2Player("khong du 1  diem ki nang, khong the linh ngo them ")

end

end
else
Msg2Player("Khong du dang cap 50")
end



end;

		


 
