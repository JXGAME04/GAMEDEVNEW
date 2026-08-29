-- [PETSYS 28/08] cac ham Linux chua co tren JX1 - compat TU NHUONG:
-- moi khoi chi dinh nghia khi ham C chua ton tai (ban C moi se de len).

if (GetCashCoin == nil) then
	-- xu cua JX1 = ExtPoint (admin point.lua: tienxu -> EarnExtPoint)
	function GetCashCoin()
		return GetExtPoint()
	end
end

if (PayCoin == nil) then
	function PayCoin(nCoin)
		if (GetExtPoint() < nCoin) then
			return 0
		end
		SetExtPoint(GetExtPoint() - nCoin)
		return 1
	end
end

if (GetServerDate == nil) then
	function GetServerDate(szFmt)
		return GetLocalDate(szFmt)
	end
end

if (TabFile_GetColCount == nil) then
	function TabFile_GetColCount(szAlias)
		return 0
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
