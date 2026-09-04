Include("\\script\\tong\\workshop\\ws_huodong.lua")
function main()
	local szTongName, nTongID = GetTongName()
	if nTongID == 0 then
		Say("<#>Tæng qu¶n ho¹t ®éng: Ng­¬i ch­a gia nhËp bang, ®Õn t×m ta cã viÖc g×?", 0)
		return
	end	
	-- [MAIL 03/09 D9] bang chu: them menu phat luong bang hoi qua thu (thanh vien khac di duong cu)
	if TONG_GetMaster(nTongID) == GetName() then
		Include("\\scriptjx2\\tong_vn\\tong_luong.lua")
		TongLuong_Menu(nTongID)
		return
	end
	local nTypeID = wsGetTypeID(NAME_HUODONG)
	local nWorkshopID = TWS_GetFirstWorkshop(nTongID, nTypeID)
	USE_G_1(nTongID, nWorkshopID)
end