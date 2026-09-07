-- [PETSYS 28/08] cac ham Linux chua co tren JX1 - compat TU NHUONG:
-- moi khoi chi dinh nghia khi ham C chua ton tai (ban C moi se de len).

if (GetCashCoin == nil) then
	-- [29/08] XU hien o HANH TRANG = task 251 (TASKVALUE_STATTASK_XU,
	-- GDI_PLAYER_HOLD_FKCOIN) - KHONG phai ExtPoint (do sai hom truoc:
	-- chu co 282 xu ma GetExtPoint tra 0)
	function GetCashCoin()
		return GetTask(251)
	end
end

if (PayCoin == nil) then
	function PayCoin(nCoin)
		if (GetTask(251) < nCoin) then
			return 0
		end
		SetTask(251, GetTask(251) - nCoin)
		return 1
	end
end

if (GetServerDate == nil) then
	function GetServerDate(szFmt)
		return GetLocalDate(szFmt)
	end
end

if (TabFile_GetColCount == nil) then
	-- [30/08] tra 0 lam LoadSkillData chet ngay (mod(0-1,4) ~= 0) ->
	-- dem cot THAT bang cach do dong header den khi gap o rong
	function TabFile_GetColCount(szAlias)
		local nCot = 0
		local i
		for i = 1, 200 do
			local v = TabFile_GetCell(szAlias, 1, i)
			if v == nil or v == "" then
				break
			end
			nCot = i
		end
		return nCot
	end
end

if (ReduceOwnExp == nil) then
	-- ban C that (DirectAddExp am) nam trong CoreServer.dll.moi_petsys_rb2;
	-- khi chua swap thi bao ro thay vi chet script.
	function ReduceOwnExp(nExp)
		Talk(1, "", "Chuc nang nay can ban CoreServer moi - admin swap CoreServer.dll.moi_petsys_rb2 roi thu lai")
		return 0
	end
end

if (unpack == nil) then
	-- ban giong het script\lib\common.lua:16 (state petsys khong Include file do)
	function unpack(tb, n)
		if (not n) then
			n = 1
		end
		if (n >= getn(tb)) then
			return tb[n]
		end
		return tb[n], unpack(tb, n + 1)
	end
end
