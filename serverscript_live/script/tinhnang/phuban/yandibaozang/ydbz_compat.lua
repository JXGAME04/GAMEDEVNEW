-- ==========================================================================
-- VIEM DE - LOP DEM: ham ban Linux co ma JX1 goi ten khac
-- SINH TU DONG boi ReverseTools\viemde\v22_compat.py - DUNG SUA TAY
-- ==========================================================================

-- Ban Linux: GetItemBindState(nItemIndex)
-- Ban JX1  : GetPlayerItemIsLock(nItemIndex) - da doc ma engine
--            (ScriptFuns LuaGetPlayerItemIsLock -> Item[idx].GetPlayerItemLock()),
--            tra ve DUNG truong ma SetItemBindState ghi vao. Anh xa 1:1.
if GetItemBindState == nil then
	function GetItemBindState(nItemIndex)
		if GetPlayerItemIsLock == nil then
			return 0
		end
		return GetPlayerItemIsLock(nItemIndex) or 0
	end
end
