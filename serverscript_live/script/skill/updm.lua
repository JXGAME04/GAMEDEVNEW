

function main()
b = HaveMagic(399)
	a = b + 1
if (a < 16) then
c = a + 4
else
c = 20
end

	if HaveMagic(399) < 20 and GetMagicPoint() >= 1 then
	if HaveMagic(mangten[1][4]) >= c then
		if HaveMagic(mangten[1][6]) >= c then
			if HaveMagic(mangten[1][8]) >= c then
				if HaveMagic(mangten[1][10]) >= c then
					if HaveMagic(mangten[1][12]) >= c then
						AddMagicPoint(-1)
						AddMagic(399,a)
						Talk(1,"","Ky Nang "..mangten[1][3].." Linh Ngo Duoc <color=red>"..HaveMagic(399).."<color> Diem")	
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
{4,399,"Loan Hoan Kich",462,"Dia Diem Hoa",464,"Doc Thu Cot",436,"Xuyen Tam Thich",434,"Han Bang Thich",439,"Loi Kich Thuat"}
}