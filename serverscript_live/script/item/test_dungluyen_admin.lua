-- Bo test DUNG LUYEN VAN CUONG cho lenh bai admin [DUNGLUYEN-PB 02/09].
-- Dung luyen thuc su lam tai Tho Ren (menu 'Trang bi dung luyen'); day chi phat nguyen lieu/trang bi de test.
-- BAY: bang chon 512 byte; cam dau '/' trong nhan; TCVN3 khong co chu HOA co dau (tru A A D E O O U).
-- Van Cuong P (fusion.txt): 1-10 Thuong Long Giap cap 1-10, 11-20 At, 21-30 Binh, 31-40 Dinh, 41-50 Am Moc Giap, 61-70 Huyen Thuy Giap...
DL_VANCUONG_CAP1 = { 1, 11, 21, 31, 41, 61 };   -- 6 loai thuoc tinh khac nhau, cap 1 (test 'trung loai' = ma 7)
DL_VANCUONG_CAO  = { 5, 8, 15, 18 };            -- cap 5 va 8 (test tran pham chat cot 59: BHPT = 7)
DL_TRANGBI       = { 2289, 2286, 2284 };        -- goldequip rec 0-based: Bach Ho Phu To Ca Sa (ao 2x3), Thuong Gioi (nhan 1x1), Chau Lien (2x1) - cap 2 pham 7

function DL_Them(nG, nD, nP, nSo)
	local n = 0
	for i = 1, nSo do
		local idx = AddItem(nG, nD, nP, 1, 0, 0, 0)
		if idx and idx > 0 then
			n = n + 1
		end
	end
	return n
end

function DL_TestRoot()
	Say("Bé test dung luyÖn V¨n C­¬ng. Dung luyÖn t¹i Thî RÌn - Trang bŞ dung luyÖn. Kho¸ng: chuét ph¶i ®Ó nhËn V¨n C­¬ng/V¨n Tinh.", 6,
		{"NhËn 6 V¨n C­¬ng cÊp 1 (6 lo¹i)/DL_NhanVanCuong1",
		 "NhËn V¨n C­¬ng cÊp 5 vµ 8/DL_NhanVanCuongCao",
		 "NhËn 5 Tinh ThiÕt Kho¸ng, 5 Tinh Tinh Kho¸ng, 5 HuyÒn Háa Than/DL_NhanKhoang",
		 "NhËn V¨n Tinh Gi¸p cÊp 1 x4, cÊp 2 x2/DL_NhanVanTinh",
		 "NhËn 3 trang bŞ HK dung luyÖn ®­îc (¸o 2x3, nhÉn, liªn)/DL_NhanTrangBi",
		 "KÕt thóc ®èi tho¹i/no"})
end

function DL_NhanVanCuong1()
	local n = 0
	for i = 1, getn(DL_VANCUONG_CAP1) do
		n = n + DL_Them(8, 1, DL_VANCUONG_CAP1[i], 1)
	end
	Msg2Player(format("§· nhËn %d V¨n C­¬ng cÊp 1.", n))
end

function DL_NhanVanCuongCao()
	local n = 0
	for i = 1, getn(DL_VANCUONG_CAO) do
		n = n + DL_Them(8, 1, DL_VANCUONG_CAO[i], 1)
	end
	Msg2Player(format("§· nhËn %d V¨n C­¬ng cÊp 5 vµ 8.", n))
end

function DL_NhanKhoang()
	local n = DL_Them(6, 1, 4428, 5) + DL_Them(6, 1, 4429, 5) + DL_Them(6, 1, 4125, 5)
	Msg2Player(format("§· nhËn %d mãn: Tinh ThiÕt Kho¸ng, Tinh Tinh Kho¸ng, HuyÒn Háa Than.", n))
end

function DL_NhanVanTinh()
	local n = DL_Them(6, 1, 4178, 4) + DL_Them(6, 1, 4179, 2)
	Msg2Player(format("§· nhËn %d V¨n Tinh Th­¬ng Long Gi¸p (cÊp 1 x4, cÊp 2 x2).", n))
end

function DL_NhanTrangBi()
	local n = 0
	for i = 1, getn(DL_TRANGBI) do
		local idx = AddItem2(2, 0, DL_TRANGBI[i], 0, 0, 0)
		if idx and idx > 0 then
			n = n + 1
		end
	end
	Msg2Player(format("§· nhËn %d trang bŞ B¹ch Hæ Phñ Tæ (cap 2 V¨n C­¬ng, phÈm chÊt tèi ®a 7).", n))
end
