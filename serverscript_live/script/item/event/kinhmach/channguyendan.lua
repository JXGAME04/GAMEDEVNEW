-- [KM 28/08] CHUAN HOA vien Chan Nguyen theo Linux global\meridian\channguyendon.lua:
-- gia tri theo LOAI vien (ban cu +10 dong loat), TRAN hap thu 3000 diem/ngay
-- (chuan vnmeridan_head nMaxValue=3000; chuan: tieu=1 trung=5 dai=10).
-- Diem cong vao TASK_CHANGNGUYENDAN (362) - task he kinh mach dang doc (KM_TaskDiem).
-- Task 4451 = ngay (yyyymmdd), 4452 = diem da hap thu hom nay (da kiem: chua ai dung).
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_task.lua")

KM_CND_GIATRI = {}
KM_CND_GIATRI[4752] = 10
KM_CND_GIATRI[4846] = 5
KM_CND_GIATRI[4847] = 10
KM_CND_TEN = {}
KM_CND_TEN[4752] = "Ch©n Nguyªn §an"
KM_CND_TEN[4846] = "Ch©n Nguyªn §¬n (trung)"
KM_CND_TEN[4847] = "Ch©n Nguyªn §¬n (®¹i)"
KM_CND_TRAN_NGAY = 3000
KM_CND_TASK_NGAY = 4451
KM_CND_TASK_DIEM = 4452

function main(nItemIndex)
	local nG, nD, nP = GetItemProp(nItemIndex)
	local nDiem = KM_CND_GIATRI[nP]
	if nDiem == nil then
		return 1
	end
	local nHomNay = tonumber(GetLocalDate("%Y%m%d"))
	if GetTask(KM_CND_TASK_NGAY) ~= nHomNay then
		SetTask(KM_CND_TASK_NGAY, nHomNay)
		SetTask(KM_CND_TASK_DIEM, 0)
	end
	local nDaHap = GetTask(KM_CND_TASK_DIEM)
	if nDaHap >= KM_CND_TRAN_NGAY then
		Msg2Player(format("H«m nay ®· hÊp thu ®ñ %d ®iÓm Ch©n Nguyªn tõ ®an d­îc, mai h·y dïng tiÕp.", KM_CND_TRAN_NGAY))
		return 1
	end
	if nDaHap + nDiem > KM_CND_TRAN_NGAY then
		nDiem = KM_CND_TRAN_NGAY - nDaHap
	end
	SetTask(KM_CND_TASK_DIEM, nDaHap + nDiem)
	SetTask(TASK_CHANGNGUYENDAN, GetTask(TASK_CHANGNGUYENDAN) + nDiem)
	RemoveItem(nItemIndex, 1, 1)
	Msg2Player(format("Dïng %s: +%d ®iÓm Ch©n Nguyªn (h«m nay %d/%d). HiÖn cã %d ®iÓm.", KM_CND_TEN[nP], nDiem, nDaHap + nDiem, KM_CND_TRAN_NGAY, GetTask(TASK_CHANGNGUYENDAN)))
end
