function nangskill(
iddtht1,lv1dtht1,lv2dtht1,lv3dtht1,lv4dtht1,lv5dtht1,lv6dtht1,lv7dtht1,lv8dtht1,lv9dtht1,lv10dtht1,
iddtht2,lv1dtht2,lv2dtht2,lv3dtht2,lv4dtht2,lv5dtht2,lv6dtht2,lv7dtht2,lv8dtht2,lv9dtht2,lv10dtht2,
iddtht3,lv1dtht3,lv2dtht3,lv3dtht3,lv4dtht3,lv5dtht3,lv6dtht3,lv7dtht3,lv8dtht3,lv9dtht3,lv10dtht3,
id,
idhtdt1,lv1htdt1,lv2htdt1,lv3htdt1,lv4htdt1,lv5htdt1,lv6htdt1,lv7htdt1,lv8htdt1,lv9htdt1,lv10htdt1,
idhtdt2,lv1htdt2,lv2htdt2,lv3htdt2,lv4htdt2,lv5htdt2,lv6htdt2,lv7htdt2,lv8htdt2,lv9htdt2,lv10htdt2,
idhtdt3,lv1htdt3,lv2htdt3,lv3htdt3,lv4htdt3,lv5htdt3,lv6htdt3,lv7htdt3,lv8htdt3,lv9htdt3,lv10htdt3
)

tsht1 = 0
tsht2 = 0
tsht3 = 0
if (iddtht1 ~= 0) then
if (GetMagicLevel(iddtht1) >= lv10dtht1) then
tsht1 = 10
elseif (GetMagicLevel(iddtht1) >= lv9dtht1) then
tsht1 = 9
elseif (GetMagicLevel(iddtht1) >= lv8dtht1) then
tsht1 = 8
elseif (GetMagicLevel(iddtht1) >= lv7dtht1) then
tsht1 = 7
elseif (GetMagicLevel(iddtht1) >= lv6dtht1) then
tsht1 = 6
elseif (GetMagicLevel(iddtht1) >= lv5dtht1) then
tsht1 = 5
elseif (GetMagicLevel(iddtht1) >= lv4dtht1) then
tsht1 = 4
elseif (GetMagicLevel(iddtht1) >= lv3dtht1) then
tsht1 = 3
elseif (GetMagicLevel(iddtht1) >= lv2dtht1) then
tsht1 = 2
elseif (GetMagicLevel(iddtht1) >= lv1dtht1) then
tsht1 = 1
end
end

if (iddtht2 ~= 0) then
if (GetMagicLevel(iddtht2) >= lv10dtht2) then
tsht2 = 10
elseif (GetMagicLevel(iddtht2) >= lv9dtht2) then
tsht2 = 9
elseif (GetMagicLevel(iddtht2) >= lv8dtht2) then
tsht2 = 8
elseif (GetMagicLevel(iddtht2) >= lv7dtht2) then
tsht2 = 7
elseif (GetMagicLevel(iddtht2) >= lv6dtht2) then
tsht2 = 6
elseif (GetMagicLevel(iddtht2) >= lv5dtht2) then
tsht2 = 5
elseif (GetMagicLevel(iddtht2) >= lv4dtht2) then
tsht2 = 4
elseif (GetMagicLevel(iddtht2) >= lv3dtht2) then
tsht2 = 3
elseif (GetMagicLevel(iddtht2) >= lv2dtht2) then
tsht2 = 2
elseif (GetMagicLevel(iddtht2) >= lv1dtht2) then
tsht2 = 1
end
end

if (iddtht3 ~= 0) then		
if (GetMagicLevel(iddtht3) >= lv10dtht3) then
tsht3 = 10
elseif (GetMagicLevel(iddtht3) >= lv9dtht3) then
tsht3 = 9
elseif (GetMagicLevel(iddtht3) >= lv8dtht3) then
tsht3 = 8
elseif (GetMagicLevel(iddtht3) >= lv7dtht3) then
tsht3 = 7
elseif (GetMagicLevel(iddtht3) >= lv6dtht3) then
tsht3 = 6
elseif (GetMagicLevel(iddtht3) >= lv5dtht3) then
tsht3 = 5
elseif (GetMagicLevel(iddtht3) >= lv4dtht3) then
tsht3 = 4
elseif (GetMagicLevel(iddtht3) >= lv3dtht3) then
tsht3 = 3
elseif (GetMagicLevel(iddtht3) >= lv2dtht3) then
tsht3 = 2
elseif (GetMagicLevel(iddtht3) >= lv1dtht3) then
tsht3 = 1
end	
end
	
tsht = tsht1 + tsht2 + tsht3
sktd = tsht + 20
lvsk = GetMagicLevel(id)

if (lvsk < sktd) then
if (GetMagicPoint() >= 1) then
AddMagic(id,lvsk+1)
AddMagicPoint(-1)







if (idhtdt1 ~= 0) then
if (HaveMagic(idhtdt1) ~= -1) then
lvhtdt1 = GetMagicLevel(idhtdt1)
if (GetMagicLevel(id) == (lv1htdt1 - 1)) or
 (GetMagicLevel(id) == (lv2htdt1 - 1)) or
  (GetMagicLevel(id) == (lv3htdt1 - 1)) or
   (GetMagicLevel(id) == (lv4htdt1 - 1)) or
    (GetMagicLevel(id) == (lv5htdt1 - 1)) or
	 (GetMagicLevel(id) == (lv6htdt1 - 1)) or
	  (GetMagicLevel(id) == (lv7htdt1 - 1)) or
	   (GetMagicLevel(id) == (lv8htdt1 - 1)) or
	    (GetMagicLevel(id) == (lv9htdt1 - 1)) or
		 (GetMagicLevel(id) == (lv10htdt1 - 1)) then
AddMagic(idhtdt1,lvhtdt1+1)
end
end
end


if (idhtdt2 ~= 0) then
if (HaveMagic(idhtdt2) ~= -1) then
lvhtdt2 = GetMagicLevel(idhtdt2)
if (GetMagicLevel(id) == (lv1htdt2 - 1)) or
 (GetMagicLevel(id) == (lv2htdt2 - 1)) or
  (GetMagicLevel(id) == (lv3htdt2 - 1)) or
   (GetMagicLevel(id) == (lv4htdt2 - 1)) or
    (GetMagicLevel(id) == (lv5htdt2 - 1)) or
	 (GetMagicLevel(id) == (lv6htdt2 - 1)) or
	  (GetMagicLevel(id) == (lv7htdt2 - 1)) or
	   (GetMagicLevel(id) == (lv8htdt2 - 1)) or
	    (GetMagicLevel(id) == (lv9htdt2 - 1)) or
		 (GetMagicLevel(id) == (lv10htdt2 - 1)) then
AddMagic(idhtdt2,lvhtdt2+1)
end
end
end

if (idhtdt3 ~= 0) then
if (HaveMagic(idhtdt3) ~= -1) then
lvhtdt3 = GetMagicLevel(idhtdt3)
if (GetMagicLevel(id) == (lv1htdt3 - 1)) or
 (GetMagicLevel(id) == (lv2htdt3 - 1)) or
  (GetMagicLevel(id) == (lv3htdt3 - 1)) or
   (GetMagicLevel(id) == (lv4htdt3 - 1)) or
    (GetMagicLevel(id) == (lv5htdt3 - 1)) or
	 (GetMagicLevel(id) == (lv6htdt3 - 1)) or
	  (GetMagicLevel(id) == (lv7htdt3 - 1)) or
	   (GetMagicLevel(id) == (lv8htdt3 - 1)) or
	    (GetMagicLevel(id) == (lv9htdt3 - 1)) or
		 (GetMagicLevel(id) == (lv10htdt3 - 1)) then
AddMagic(idhtdt3,lvhtdt3+1)
end
end
end

else
Msg2Player("Ban khong du 1 diem ki nang, khong the nang cap !")
end
else
Msg2Player("Ki nang cua ban da dat dang cap gioi han, khong the nang cap !")
end
end;