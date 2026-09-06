

function main()
b = HaveMagic(328)
	a = b + 1
if (a < 16) then
c = a + 4
else
c = 20
end

	if HaveMagic(328) < 20 and GetMagicPoint() >= 1 then
	if HaveMagic(mangten[1][4]) >= c then
		if HaveMagic(mangten[1][6]) >= c then
			if HaveMagic(mangten[1][8]) >= c then
				if HaveMagic(mangten[1][10]) >= c then
					if HaveMagic(mangten[1][12]) >= c then
						AddMagicPoint(-1)
						AddMagic(328,a)
						Talk(1,"","Ky Nang "..mangten[1][3].." Linh Ngo Duoc <color=red>"..HaveMagic(328).."<color> Diem")	
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
{3,328,"Nhiep Hon Loan Tam",136,"Hoa Lien Phan Hoa",137,"Ao Anh Phi Ho",140,"Phi Hong Vo Tich",364,"Bi To Thanh Phong",143,"Lich Ma Doat Hon"},
}