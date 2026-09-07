-- [LOCAL54 06/09 toi] 17 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local KM_Pow256, KM_GetByte, KM_SetByte, KM_TenMach, KM_BaoVeCap, KM_MachMoi, KM_TenDiem, KM_DemItem, KM_LayThongSo, KM_DanCuaLan, KM_DuDiem, KM_TruDiem, KM_DuTaiNguyen, KM_TruTaiNguyen, KM_CongBaoDay, KM_BaoKetQua, KM_MuaKhiDoanh
-- ============================================================================
-- Xung huyet kinh mach -- theo DUNG common.lua + client.lua cua ban chuan VLTK.
-- Moi CON SO tu meridian_data.lua (sinh tu bang chuan); moi CAU CHU tu
-- meridian_lang.lua (nguyen van lang.lua chuan).
-- 27/08b (PHAN 1 - cua so xung huyet):
--   * mach 9-12 tieu HUYEN NGUYEN (task 4318 chuan) + Dinh Mach Dan 4871/4872
--   * mach cu cap>16 va duong DHMD: tra tron duoc 1 DHMD = 1000 HMD (head.lua)
--   * duong BAO VE = Huyet Long Dang cap theo bang MeridianProtect chuan x10
--   * duong LONG HON = Dang + Don theo bang xuelongdan chuan + 3 lop mat na
--   * bao day 4440/4441/4491 chay that (MAX_TASK da nang 4600)
-- Ghi chu kinh te: du an chi co MOT id Huyet Long Dang/Don (4848/4849), chua co
-- 14 cap vat pham rieng nhu chuan -> CAP chi hien trong thong bao, tru bang id chung.
-- ============================================================================
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("\\script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\player\\meridian_lang.lua")
Include("\\script\\player\\meridian_data.lua")

KM_ITEM_HMD    = 4844   -- Ho Mach Don
KM_ITEM_DHMD   = 4870   -- Dai Ho Mach Don (Cap 1)
KM_ITEM_DMD    = 4871   -- Dinh Mach Dan (mach moi, cap <= 16)
KM_ITEM_DMD1   = 4872   -- Dinh Mach Dan (Lv1) (mach moi, cap > 16)
KM_ITEM_HLDANG = 4848   -- Huyet Long Dang
KM_ITEM_HLDAN  = 4849   -- Huyet Long Don
KM_TASK_XUANYUAN = 4318 -- diem Huyen Nguyen, id chuan (Meridian.TSK_XUAN_YUAN)
KM_DOI_DHMD    = 1000   -- 1 Dai Ho Mach Don = 1000 Ho Mach Don (head.lua chuan)

-- bien nhiem vu dem so lan that bai (bao day), theo bang chuan
KM_BAODAY = {
	[1]={4440,1}, [2]={4440,2}, [3]={4440,3}, [4]={4440,4},
	[5]={4441,1}, [6]={4441,2}, [7]={4441,3}, [8]={4441,4},
	[9]={4491,1}, [10]={4491,2},[11]={4491,3},[12]={4491,4},
};

-- bang MeridianProtect chuan (common.lua): [cap] = cap Huyet Long Dang, so luong luon 10
KM_BAOVE_A = {[2]=1,[3]=2,[4]=3,[5]=4,[6]=5,[7]=6,[8]=7,[9]=8,[10]=9,[13]=12}   -- mach 1,2
KM_BAOVE_B = {[5]=4,[6]=5,[7]=6,[8]=2,[9]=8,[10]=9,[12]=11,[13]=12,[15]=14}     -- mach 5-8

-- mat na duong Long Hon Ho The (client.lua chuan)
KM_LH_MACH = {[1]=1,[2]=1,[5]=1,[6]=1,[7]=1,[8]=1}
KM_LH_CAP  = {[3]=1,[4]=1,[5]=1,[6]=1,[7]=1,[8]=1,[15]=1,[16]=1}

function KM_Pow256(n)
	local d = 1
	local i = 1
	while i < n do
		d = d * 256
		i = i + 1
	end
	return d
end

function KM_GetByte(nTask, nByte)
	local d = KM_Pow256(nByte)
	return mod(floor(GetTask(nTask) / d), 256)
end

function KM_SetByte(nTask, nByte, nVal)
	local d = KM_Pow256(nByte)
	local v = GetTask(nTask)
	local cur = mod(floor(v / d), 256)
	SetTask(nTask, v + (nVal - cur) * d)
end

function KM_TenMach(nMach)
	if KM_TEN[nMach] then
		return KM_TEN[nMach]
	end
	return ""
end

function KM_BaoVeCap(nMach, nCap)
	if nMach == 1 or nMach == 2 then
		return KM_BAOVE_A[nCap]
	end
	if nMach >= 5 and nMach <= 8 then
		return KM_BAOVE_B[nCap]
	end
	return nil
end

function KM_MachMoi(nMach)
	if nMach >= 9 then
		return 1
	end
	return 0
end

function KM_TaskDiem(nMach)
	if nMach >= 9 then
		return KM_TASK_XUANYUAN
	end
	return TASK_CHANGNGUYENDAN
end

function KM_TenDiem(nMach)
	if nMach >= 9 then
		return MERIDIAN_NAME_XY
	end
	return MERIDIAN_NAME_ZY
end

function KM_DemItem(nItem)
	return GetItemCount(0, 6, 1, nItem, -1, -1, pos_equiproom)
end

-- thong so mot lan xung: tra bang { nZY, nRate, nLui, loai, nSo }
--   loai 1 = dan thuong (HMD / Dinh Mach Dan theo mach), nSo = so vien
--   loai 2 = tra theo TONG quy doi Ho Mach Don (uu tien DHMD, du -> HMD), nSo = tong
--   loai 3 = Dinh Mach Dan (Lv1) cua mach moi cap>16, nSo = so vien
function KM_LayThongSo(nMach, nCap)
	local row = KM_HUYET[nMach]
	if row == nil then return nil end
	local d = row[nCap]
	if d == nil then return nil end
	local tt = {}
	tt.nLui  = d[1]
	tt.nRate = d[2]
	tt.nZY   = d[3]
	if nCap <= 16 then
		if tt.nRate <= 0 then
			-- chi mach 3/4 di duong Dai Ho Mach Don (tbDaHuMaiDanWay chuan)
			if nMach ~= 3 and nMach ~= 4 then return nil end
			local dh = KM_DAIHOMACH[nCap]
			if dh == nil then return nil end
			tt.nZY   = dh[1]
			tt.loai  = 2
			tt.nSo   = dh[2]
			tt.nRate = dh[3]
		else
			tt.loai = 1
			tt.nSo  = d[4]
		end
	else
		-- cap 17..32: ty le theo so lan that bai (bao day chuan)
		local nMoi = KM_MachMoi(nMach)
		local bd = KM_BAODAY[nMach]
		local nLan = 0
		if bd then nLan = KM_GetByte(bd[1], bd[2]) end
		local tb = KM_TYLE_LUOT[nMoi]
		if tb and tb[nCap] then
			local arr = tb[nCap]
			local k = nLan + 1
			if k > getn(arr) then k = getn(arr) end
			tt.nRate = arr[k]
		end
		if nMoi == 1 then
			tt.loai = 3
			tt.nSo  = d[5]
		else
			tt.loai = 2
			tt.nSo  = d[5] * KM_DOI_DHMD
		end
	end
	return tt
end

-- dan hien thi / tru cho mot lan xung
function KM_DanCuaLan(nMach, tt)
	if tt.loai == 3 then
		return KM_ITEM_DMD1, MERIDIAN_NAME_DMD1
	end
	if tt.loai == 2 then
		return KM_ITEM_HMD, MERIDIAN_NAME_HMD
	end
	if nMach >= 9 then
		return KM_ITEM_DMD, MERIDIAN_NAME_DMD
	end
	return KM_ITEM_HMD, MERIDIAN_NAME_HMD
end

function KM_DuDiem(nMach, nZY)
	if GetTask(KM_TaskDiem(nMach)) < nZY then
		Talk(1, "", format(MERIDIAN_NOTICE_NOT_ENOUGH_ZY, nZY, KM_TenDiem(nMach), KM_TenDiem(nMach)))
		return 0
	end
	return 1
end

function KM_TruDiem(nMach, nZY)
	local t = KM_TaskDiem(nMach)
	SetTask(t, GetTask(t) - nZY)
end

function KM_DuTaiNguyen(nMach, tt)
	local nItem, szTen = KM_DanCuaLan(nMach, tt)
	if tt.nSo <= 0 then return 1 end
	if tt.loai == 2 then
		if KM_DemItem(KM_ITEM_DHMD) * KM_DOI_DHMD + KM_DemItem(KM_ITEM_HMD) >= tt.nSo then
			return 1
		end
		Talk(1, "", format(MERIDIAN_NOTICE_NOT_ENOUGH_HMD, tt.nSo, szTen, szTen))
		return 0
	end
	if KM_DemItem(nItem) < tt.nSo then
		Talk(1, "", format(MERIDIAN_NOTICE_NOT_ENOUGH_HMD, tt.nSo, szTen, szTen))
		return 0
	end
	return 1
end

function KM_TruTaiNguyen(nMach, tt)
	if tt.nSo <= 0 then return end
	if tt.loai == 2 then
		-- uu tien Dai Ho Mach Don, phan du tra bang Ho Mach Don (head.lua chuan)
		local nCo = KM_DemItem(KM_ITEM_DHMD)
		local nDung = floor(tt.nSo / KM_DOI_DHMD)
		if nDung > nCo then nDung = nCo end
		local nDu = tt.nSo - nDung * KM_DOI_DHMD
		if nDung > 0 then
			DelItem(0, -1, 6, 1, KM_ITEM_DHMD, -1, -1, pos_equiproom, nDung)
		end
		if nDu > 0 then
			DelItem(0, -1, 6, 1, KM_ITEM_HMD, -1, -1, pos_equiproom, nDu)
		end
		return
	end
	local nItem = KM_DanCuaLan(nMach, tt)
	DelItem(0, -1, 6, 1, nItem, -1, -1, pos_equiproom, tt.nSo)
end

function KM_CongBaoDay(nMach, bThanhCong)
	local bd = KM_BAODAY[nMach]
	if bd == nil then return end
	if bThanhCong == 1 then
		KM_SetByte(bd[1], bd[2], 0)
	else
		local n = KM_GetByte(bd[1], bd[2]) + 1
		-- [27/08b] chan o 4 nhu chuan (nMaxTime): vua du 5 muc ty le, vua tranh
		-- byte thu 4 (mach 4/8/12) day task vuot int32 (250 shl 24 la so am).
		if n > 4 then n = 4 end
		KM_SetByte(bd[1], bd[2], n)
	end
end

function KM_BaoKetQua(bThanh, nMach, nCap, nZY, tt)
	local szTen = KM_TenMach(nMach)
	-- [27/08b] chuan in TEN HUYET VI, khong in so cap
	local szHuyet = nCap
	if KM_TENHUYET and KM_TENHUYET[nMach] and KM_TENHUYET[nMach][nCap] then
		szHuyet = KM_TENHUYET[nMach][nCap]
	end
	local nItem, szTenDan = KM_DanCuaLan(nMach, tt)
	if bThanh == 1 then
		Msg2Player(format(MERIDIAN_NOTICE_SUCCESS, szHuyet, szTen, nZY, KM_TenDiem(nMach), tt.nSo, szTenDan))
	else
		Msg2Player(format(MERIDIAN_NOTICE_FAILED, szHuyet, szTen, nZY, KM_TenDiem(nMach), tt.nSo, szTenDan))
	end
end


-- ============================ KHI DOANH DAN DIEN ============================
-- Goi 1/7/30 ngay. So lieu lay tu [BreathCosts] cua Meridian.ini ban chuan;
-- nguyen lieu do du an chot (ghi trong chinh tep ini do): diem Chan Nguyen +
-- Khi Doanh Dan (6,1,4873). Han luu o bien nhiem vu 4450 (giay he thong).
KM_TASK_KDDD_HAN = 4450
KM_ITEM_KDD      = 4873
-- [goi] = { so ngay, diem Chan Nguyen, so Khi Doanh Dan }
KM_KDDD_GOI = {
	[0] = {1, 600, 1},
	[1] = {7, 4000, 6},
	[2] = {30, 16000, 24},
};

function KM_MuaKhiDoanh(nGoi)
	local g = KM_KDDD_GOI[nGoi]
	if g == nil then return end
	-- dieu kien: ca 8 kinh mach dau phai dat cap 16 tro len
	-- [27/08b] cap mach doc bang GetMeridian (binding moi) - du an KHONG luu cap
	-- o task 4001..4008 nhu ban chuan, doc task se luon ra 0 va tu choi oan.
	local i = 1
	while i <= 8 do
		if GetMeridian(i - 1) < 16 then
			Msg2Player("Ch­a më ®­îc KhÝ Doanh §an §iÒn: c¶ 8 kinh m¹ch ®Çu ph¶i ®¹t cÊp 16 trë lªn.")
			return
		end
		i = i + 1
	end
	if GetTask(TASK_CHANGNGUYENDAN) < g[2] then
		Msg2Player(format("Kh«ng ®ñ ®iÓm Ch©n Nguyªn: cÇn %d, ®ang cã %d.", g[2], GetTask(TASK_CHANGNGUYENDAN)))
		return
	end
	local nCo = GetItemCount(0, 6, 1, KM_ITEM_KDD, -1, -1, pos_equiproom)
	if nCo < g[3] then
		Msg2Player(format("Kh«ng ®ñ KhÝ Doanh §an: cÇn %d, ®ang cã %d.", g[3], nCo))
		return
	end
	SetTask(TASK_CHANGNGUYENDAN, GetTask(TASK_CHANGNGUYENDAN) - g[2])
	DelItem(0, -1, 6, 1, KM_ITEM_KDD, -1, -1, pos_equiproom, g[3])
	-- con han thi CONG DON them, het han thi tinh tu bay gio
	local nNay = GetCurrentTime()
	local nHan = GetTask(KM_TASK_KDDD_HAN)
	if nHan < nNay then nHan = nNay end
	nHan = nHan + g[1] * 86400
	SetTask(KM_TASK_KDDD_HAN, nHan)
	Msg2Player(format("§· më KhÝ Doanh §an §iÒn thªm %d ngµy. §· trõ %d ®iÓm Ch©n Nguyªn vµ %d KhÝ Doanh §an.", g[1], g[2], g[3]))
end

function main(str)
	if str == nil then return end

	local nType  = tonumber(StrSplit(str, 0))   -- ma mach, dem tu 0
	local nCap   = tonumber(StrSplit(str, 1))   -- cap dinh xung toi
	local nTang  = tonumber(StrSplit(str, 2))   -- 1 = Long Hon Ho The
	local nBaoVe = tonumber(StrSplit(str, 3))   -- 2 = Huyet vi bao ve

	if nType == nil then return end
	-- ma nguong 100/101/102 = mua Khi Doanh Dan Dien goi 1/7/30 ngay
	if nType >= 100 then
		KM_MuaKhiDoanh(nType - 100)
		return
	end
	if nCap == nil then return end
	local nMach = nType + 1
	if nMach < 1 or nMach > KM_MAX_MACH then return end
	if nCap  < 1 or nCap  > KM_MAX_CAP  then return end

	local tt = KM_LayThongSo(nMach, nCap)
	if tt == nil then
		Talk(1, "", MSG_CLOSE_MERIDIANS)
		return
	end

	-- ---- duong LONG HON HO THE (tbEhanceRateWay chuan) ----
	if nTang == 1 then
		if KM_LH_MACH[nMach] == nil or KM_LH_CAP[nCap] == nil then
			Talk(1, "", MERIDIAN_NOTICE_MERIDIAN_INVALID)
			return
		end
		local hl = nil
		if KM_HUYETLONG[nMach] then hl = KM_HUYETLONG[nMach][nCap] end
		if hl == nil or hl[7] == nil or hl[7] <= 0 or hl[4] < 0 then
			Talk(1, "", MERIDIAN_NOTICE_ACUP_INVALID)
			return
		end
		-- hang xuelongdan chuan: {DangCap, DangSo, DonCap, DonSo, ChanNguyen, HoMachDon, TyLe}
		if hl[2] > 0 and KM_DemItem(KM_ITEM_HLDANG) < hl[2] then
			Talk(1, "", format(MERIDIAN_NOTICE_NOT_ENOUGH_ITEM, hl[2], ITEM_NAME_XUELONGTENG, hl[1]))
			return
		end
		if hl[4] > 0 and KM_DemItem(KM_ITEM_HLDAN) < hl[4] then
			Talk(1, "", format(MERIDIAN_NOTICE_NOT_ENOUGH_ITEM, hl[4], ITEM_NAME_XUELONGDAN, hl[3]))
			return
		end
		local lh = {}
		lh.loai = 1
		lh.nSo  = hl[6]
		lh.nZY  = hl[5]
		if KM_DuDiem(nMach, lh.nZY) == 0 then return end
		if KM_DuTaiNguyen(nMach, lh) == 0 then return end
		KM_TruDiem(nMach, lh.nZY)
		KM_TruTaiNguyen(nMach, lh)
		if hl[2] > 0 then
			DelItem(0, -1, 6, 1, KM_ITEM_HLDANG, -1, -1, pos_equiproom, hl[2])
		end
		if hl[4] > 0 then
			DelItem(0, -1, 6, 1, KM_ITEM_HLDAN, -1, -1, pos_equiproom, hl[4])
		end
		if random(KM_TYLE_GOC) <= hl[7] then
			SetMeridian(nType, nCap)
			KM_CongBaoDay(nMach, 1)
			KM_BaoKetQua(1, nMach, nCap, lh.nZY, lh)
		else
			KM_CongBaoDay(nMach, 0)
			KM_BaoKetQua(0, nMach, nCap, lh.nZY, lh)
			Msg2Player(format(MERIDIAN_NOTICE_FAILED_PUNISH, MERIDIAN_NOTICE_FAILED_PROTECTION))
		end
		return
	end

	-- ---- duong HUYET VI BAO VE (MeridianProtect chuan: Dang cap dinh san x10) ----
	if nBaoVe == 2 then
		local nCapDang = KM_BaoVeCap(nMach, nCap)
		if nCapDang == nil or tt.nRate >= KM_TYLE_GOC then
			Talk(1, "", MERIDIAN_PROTECT_INVALID)
			return
		end
		if KM_DemItem(KM_ITEM_HLDANG) < 10 then
			Talk(1, "", format(MERIDIAN_NOTICE_NOT_ENOUGH_ITEM, 10, ITEM_NAME_XUELONGTENG, nCapDang))
			return
		end
		if KM_DuDiem(nMach, tt.nZY) == 0 then return end
		if KM_DuTaiNguyen(nMach, tt) == 0 then return end
		KM_TruDiem(nMach, tt.nZY)
		KM_TruTaiNguyen(nMach, tt)
		DelItem(0, -1, 6, 1, KM_ITEM_HLDANG, -1, -1, pos_equiproom, 10)
		if random(KM_TYLE_GOC) <= tt.nRate then
			SetMeridian(nType, nCap)
			KM_CongBaoDay(nMach, 1)
			KM_BaoKetQua(1, nMach, nCap, tt.nZY, tt)
		else
			KM_CongBaoDay(nMach, 0)
			KM_BaoKetQua(0, nMach, nCap, tt.nZY, tt)
			Msg2Player(MERIDIAN_NOTICE_HOLD_ACUP)
		end
		return
	end

	-- ---- duong THUONG (NormalMethod chuan) ----
	if KM_DuDiem(nMach, tt.nZY) == 0 then return end
	if KM_DuTaiNguyen(nMach, tt) == 0 then return end
	KM_TruDiem(nMach, tt.nZY)
	KM_TruTaiNguyen(nMach, tt)
	if random(KM_TYLE_GOC) <= tt.nRate then
		SetMeridian(nType, nCap)
		KM_CongBaoDay(nMach, 1)
		KM_BaoKetQua(1, nMach, nCap, tt.nZY, tt)
	else
		KM_CongBaoDay(nMach, 0)
		if nCap <= 16 then
			SetMeridian(nType, tt.nLui)
		end
		KM_BaoKetQua(0, nMach, nCap, tt.nZY, tt)
		if nCap <= 16 and tt.nLui < nCap - 1 then
			Msg2Player(format(MERIDIAN_NOTICE_FAILED_PUNISH, format(MERIDIAN_NOTICE_FAILED1, tt.nLui)))
		else
			Msg2Player(format(MERIDIAN_NOTICE_FAILED_PUNISH, MERIDIAN_NOTICE_FAILED_PROTECTION))
		end
	end
end
