

function main()
b = HaveMagic(327)
	a = b + 1
if (a < 16) then
c = a + 4
else
c = 20
end

	if HaveMagic(327) < 20 and GetMagicPoint() >= 1 then
	if HaveMagic(mangten[1][4]) >= c then
		if HaveMagic(mangten[1][6]) >= c then
			if HaveMagic(mangten[1][8]) >= c then
				if HaveMagic(mangten[1][10]) >= c then
					if HaveMagic(mangten[1][12]) >= c then
						AddMagicPoint(-1)
						AddMagic(327,a)
						Talk(1,"","Ky Nang "..mangten[1][3].." Linh Ngo Duoc <color=red>"..HaveMagic(327).."<color> Diem")	
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
{1,327,"Doan Can Hu Cot",72,"Xuyen Tam Doc Thich",336,"Xuyen Y Pha Giap",73,"Van Doc Thuc Tam",64,"Bang Lam Huyen Tinh",67,"Cuu Thien Cuong Loi"},
{2,291,"Pho Do Chung Sinh",93,"Tu Hang Pho Do",89,"Mong Diep",86,"Luu Thuy",92,"Phat Tam Tu Huu",282,"Thanh Am Phan Xuong"},
{3,328,"Nhiep Hon Loan Tam",136,"Hoa Lien Phan Hoa",137,"Ao Anh Phi Ho",140,"Phi Hong Vo Tich",364,"Bi To Thanh Phong",143,"Lich Ma Doat Hon"},
{4,329,"Tuy Tien Ta Cot",338,"Thuc Phuoc Chu",171,"Thanh Phong Phu",174,"Ki Ban Phu",339,"Bac Minh Dao Hai",173,"Thien Thanh Dia Troc"}
}