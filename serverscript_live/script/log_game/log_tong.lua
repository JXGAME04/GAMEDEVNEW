-- Author: Fong Ki“u
-- Date: 28/11/2016
-- Chuc nang: Luu thong tin tao lap bang hoi

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\log_game\\danhsach_bang.lua")

function creattong(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	local nName = GetName()
	local nID = GetTongInfo(0)
	local nTongName = GetTongInfo(1)
	capnhat_danhsach(nName,nID,nTongName)
end

function Find(tenbang, tukhoa)
	if getn(tenbang) == 0 then
		return nil
	end
	for i= 1,getn(tenbang) do
		if tenbang[i][1] == tukhoa then
			return i
		end
	end
end

function capnhat_danhsach(nName,nID,nTongName)
	local nSTT = Find(BANGHOI_DANHSACH_CR, nID)
	if nSTT ~= nil then
		BANGHOI_DANHSACH_CR[nSTT] = {nID,nTongName,nName,date("%H:%M:%S_%d-%m-%y")}
	else
		nSTT = getn(BANGHOI_DANHSACH_CR)+1
		BANGHOI_DANHSACH_CR[nSTT] = {nID,nTongName,nName,date("%H:%M:%S_%d-%m-%y")}
	end
	
	local bTabDanhSach = TaoBang(BANGHOI_DANHSACH_CR,"BANGHOI_DANHSACH_CR","")
	SaveData("script/log_game/danhsach_bang.lua",bTabDanhSach)
	return nSTT
end