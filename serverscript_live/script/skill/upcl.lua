

function main()
b = HaveMagic(329)
	a = b + 1
if (a < 16) then
c = a + 4
else
c = 20
end

	if HaveMagic(329) < 20 and GetMagicPoint() >= 1 then
	if HaveMagic(mangten[1][4]) >= c then
		if HaveMagic(mangten[1][6]) >= c then
			if HaveMagic(mangten[1][8]) >= c then
				if HaveMagic(mangten[1][10]) >= c then
					if HaveMagic(mangten[1][12]) >= c then
						AddMagicPoint(-1)
						AddMagic(329,a)
						Talk(1,"","Ky Nang "..mangten[1][3].." Linh Ngo Duoc <color=red>"..HaveMagic(329).."<color> Diem")	
	else
	Talk(1,"","Ky Nang <color=red>"..mangten[1][13].."<color> Chua Nang Den Cap <color=blue>"..c.."")
	end
	else
	Talk(1,"","Ky Nang <color=red>"..mangten[1][11].."<color> Chua Nang Den Cap <color=blue>"..c.."")
	end
	else
	Talk(1,"","Ky Nang <color=red>"..mangten[1][9].."<color> Chua Nang Den Cap <color=blue>"..c.."")
	end
	else
	Talk(1,"","Ky Nang <color=red>"..mangten[1][7].."<color> Chua Nang Den Cap <color=blue>"..c.."")
	end
	else
	Talk(1,"","Ky Nang <color=red>"..mangten[1][5].."<color> Chua Nang Den Cap <color=blue>"..c.."")
	end
	else
	Talk(1,"","Ky Nang Dat Muc Toi Da Hoac Het 1 Diem Ky NAng Du")
	end
end


mangten = {
{4,329,"Tuy Tien Ta Cot",338,"Thuc Phuoc Chu",171,"Thanh Phong Phu",174,"Ki Ban Phu",339,"Bac Minh Dao Hai",173,"Thien Thanh Dia Troc"}
}