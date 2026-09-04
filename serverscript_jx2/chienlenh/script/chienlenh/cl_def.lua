-- cl_def.lua - [CL 04/09] Chien Lenh: so hieu nhiem vu + ham phu.
--
-- MOI CAU HINH (diem, muc tieu, ten, bang thuong, ngay mua) nam o MySQL, chu tu
-- nhap tu trang web admin. Tep nay CHI giu so hieu nhiem vu, vi so hieu do GAN
-- CUNG voi cho moc trong ma may chu - doi so la nhiem vu cam ma khong bao gi.
--
-- Cac ham C++ dung o day nam trong Core\Src\KChienLenh.cpp.

if (CL_NV ~= nil) then
	return
end

CL_NV = {
	TK_1       =  1,	-- tham gia 1 lan Tong Kim
	TK_3       =  2,	-- tham gia 3 lan Tong Kim
	TK_RANK    =  3,	-- dat Thong Linh tro len
	QUAI_500   =  4,
	QUAI_1000  =  5,
	QUAI_3000  =  6,
	ONLINE_10  =  7,
	ONLINE_60  =  8,
	ONLINE_180 =  9,
	PLD        = 10,	-- Phong Lang Do cap ben Bac
	DATAU_1    = 11,
	DATAU_10   = 12,
	CTC_VAO    = 13,	-- vao tran Cong Thanh
	CTC_1000   = 14,	-- dat 1000 diem cong trang
	DATAU_T40  = 15,	-- tuan: 40 lan
	DATAU_T70  = 16,	-- tuan: 70 lan
	BOSS_CO    = 17,	-- co mat khi BOSS Hoang Kim tu vong
	BOSS_GIET  = 18,	-- ke ket lieu BOSS Hoang Kim
	VUOTAI     = 19,	-- Vuot Ai qua ai
	VIEMDE     = 20,	-- vao Viem De Bao Tang
}

-- Cong cung mot luot cho NHIEU nhiem vu (vd giet quai tinh cho ca 500/1000/3000).
function CL_CongNhom(tbId, nSo)
	if (CL_Cong == nil) then	-- DLL chua swap
		return 0
	end
	local nXong = 0
	for i = 1, getn(tbId) do
		nXong = nXong + CL_Cong(tbId[i], nSo or 1)
	end
	return nXong
end

CL_NHOM_QUAI  = {CL_NV.QUAI_500, CL_NV.QUAI_1000, CL_NV.QUAI_3000}
CL_NHOM_DATAU = {CL_NV.DATAU_1, CL_NV.DATAU_10, CL_NV.DATAU_T40, CL_NV.DATAU_T70}
CL_NHOM_TK    = {CL_NV.TK_1, CL_NV.TK_3}
CL_NHOM_ONL   = {CL_NV.ONLINE_10, CL_NV.ONLINE_60, CL_NV.ONLINE_180}

-- Nhip moi phut: goi tu timerserver.lua RunTime().
-- Cong 1 phut online cho MOI nguoi dang choi, roi de C++ tu kiem reset va xa.
function CL_Tick_Wrap()
	if (CL_Tick == nil) then
		return
	end
	local nSo = GetPlayerCount()
	for i = 1, nSo do
		local nIdx = GetPlayerIdx(i)
		if (nIdx and nIdx > 0) then
			PlayerIndex = nIdx
			CL_CongNhom(CL_NHOM_ONL, 1)
		end
	end
	CL_Tick()
end
