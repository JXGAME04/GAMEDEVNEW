-- Lenh bai hoan thanh nhiem vu Da Tau  (item 6/1/4818)
-- [PB 19/08/2026] VIET LAI cho he tasklink moi.
-- Ban cu (Fong Kieu) goi finishquest() + cac task 87..99 cua he Da Tau JX1 CU o
-- script/tinhnang/datau/ - he do da MO COI sau dot port 15-16/08 (grep toan cay
-- script/ khong con cho nao Include) => lenh bai cu KHONG con tac dung.
--
-- Cach lam: danh dau nhiem vu la DA HOAN THANH nhung CHUA phat thuong, giong het
-- Task_AwardRecord() lam khi nguoi choi tra nhiem vu that:
--     course (task 1028) = 2   -> "da nop, chua lanh thuong"
--     task 1037 = hat giong khoa cung bo 3 phan thuong
-- Nho vay nguoi choi VAN PHAI toi gap NPC Da Tau moi chon duoc phan thuong,
-- dung nhu yeu cau cua chu game.
--
-- KHONG goi storm_addpoint(4, ...) nhu Task_AwardRecord: do la diem su kien Storm,
-- bo qua de khoi keo them ca cay event vao Lua state cua item nay.

Include("\\script\\task\\newtask\\tasklink\\tasklink_head.lua")

function main(nItemIndex)

local nLoai

	-- course: 0 chua nhap mon | 1 dang lam | 2 da nop chua lanh | 3 da lanh
	if (tl_gettaskcourse() ~= 1) then
		Say("Ng­¬i ph¶i nhËn nhiÖm vô D· TÈu tr­íc ®·, råi míi dïng ®­îc lÖnh bµi nµy!", 0)
		return 1
	end

	nLoai = tl_getplayertasktype()
	if (nLoai == nil) or (nLoai < 1) or (nLoai > 6) then
		Say("NhiÖm vô D· TÈu hiÖn t¹i kh«ng hîp lÖ, h·y gÆp D· TÈu ®Ó nhËn l¹i nhiÖm vô.", 0)
		return 1
	end

	-- [PB 19/08] Chot BAT BUOC: task 1030 = so dong trong bang loai. Trang thai
	-- course==1 ma 1030==0 la CO THAT (chinh vi the seasonnpc.lua:1102 moi co
	-- _TaskLinkDebug de va, va no CHI chay khi course==1). Neu lenh bai day len
	-- course=2 khi 1030==0 thi khong con ai va duoc nua: luc tra thuong
	-- tl_giveplayeraward(1) tra TabFile_GetCell(...) = nil roi nhan len nil
	-- => loi Lua, ket nhiem vu VINH VIEN va mat luon lenh bai.
	if (nt_getTask(1030) == 0) then
		Say("NhiÖm vô D· TÈu ®ang lçi d÷ liÖu, h·y gÆp D· TÈu mét lÇn ®Ó hÖ thèng tù söa råi dïng l¹i lÖnh bµi.", 0)
		return 1
	end

	tl_settaskcourse(2)
	nt_setTask(1037, GetGameTime() + tonumber(GetLocalDate("%H%M%S")))

	RemoveItem(nItemIndex, 1)

	Msg2Player("§· hoµn thµnh nhiÖm vô D· TÈu! H·y tíi gÆp D· TÈu ®Ó nhËn phÇn th­ëng.")
	WriteLog(format("[DaTau lenh bai] %s dung lenh bai hoan thanh nhiem vu loai %d",
		GetName() or "", nLoai))

	return 0

end
