--Author: Fong KiÒu
--Function: LÖnh bµi hoµn thµnh d· tÈu
-- (20/08/2026) VIET LAI cho he nhiem vu tasklink dang song (seasonnpc.lua).
-- Ban cu noi voi he Da Tau CU (task 87-98) da thao khoi NPC tu lau nen guard
-- "phai nhan nhiem vu" luon truot -> item bam khong lam gi ca.
-- Nay: nhiem vu tasklink dang lam (course 1, moi loai 1..6) duoc tinh HOAN THANH
-- ngay va bung ruong thuong Y HET tra o NPC: Task_AwardRecord + Task_GiveAward
-- (cua seasonnpc.lua). Item genre 6 chay bGlobal=true nen callback finish_* cua
-- ruong thuong quay ve DUNG script nay - bam nut la an thuong binh thuong.
-- Task 318 = so lan da dung lenh bai (giu nguyen tran 100 cua ban goc).

Include("\\script\\global\\seasonnpc.lua")

function main(nItemIndex)

	if (GetTask(318) > 100) then
		Talk(1,"","Ng­¬i ®· sö dông lÖnh bµi tíi "..100.." lÇn råi")
		return
	end

	-- dang co nhiem vu tasklink? (1021 = loai, 1030 = dong bang, course 1 = dang lam)
	if (tl_getplayertasktype() == 0 or nt_getTask(1030) == 0) then
		Talk(1,"","<sex> ph¶i nhËn nhiÖm vô D· TÈu tr­íc khi sö dông lÖnh bµi")
		return
	end
	if (tl_gettaskcourse() ~= 1) then
		Talk(1,"","NhiÖm vô hiÖn t¹i ®· xong, h·y gÆp D· TÈu ®Ó nhËn th­ëng hoÆc nhËn nhiÖm vô míi")
		return
	end
	-- du cho nhan thuong (mySG doi 5 o trong)
	if (CalcFreeItemCellCount() < 5) then
		Talk(1,"","Hµnh trang cÇn İt nhÊt 5 « trèng ®Ó nhËn th­ëng, h·y s¾p xÕp l¹i")
		return
	end

	Task_AwardRecord()
	SetTask(318, GetTask(318) + 1)
	RemoveItem(nItemIndex,1)
	Task_GiveAward()
	return 1

end

function no()
end
