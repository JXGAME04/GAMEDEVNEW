IncludeLib("FILESYS")
	
TongBlackList = {}
	
function LoadBlackList(szPath)
	local bOk = TabFile_Load(szPath, szPath)
	if (not bOk) or (bOk ~= 1) then
		OutputMsg("Load TongBlackList Failed")
		return
	end
	
	local nCount = TabFile_GetRowCount(szPath)
	for i = 1, nCount do 
		local nTongId = tonumber(TabFile_GetCell(szPath, i, 1))
		if nTongId then
			TongBlackList[nTongId] = 1
		end
	end
	TabFile_UnLoad(szPath)
	OutputMsg(format("Load TongBlackList Successfully. BlackListCount = %d", nCount))
end

function IsInBlackList(nTongId)
	if TongBlackList[nTongId] then
		return 1
	end
	return 0
end

LoadBlackList("\\settings\\tong\\blacklist.txt")