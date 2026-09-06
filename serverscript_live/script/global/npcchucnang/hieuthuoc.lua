-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Chuc nang: Ban thuoc

Include("\\script\\lib\\lib_task.lua")

function main(NpcIndex)
	-- dofile("script/global/npc/buysell/hieuthuoc.lua")
	hieuthuoc(NpcIndex)
end

function hieuthuoc(NpcIndex)
	local OK_TRADE = "Giao dÞch/giaodich"
	local NOT_TRADE = "Kh«ng giao dÞch/no"
	SetTaskTemp(TMP_INDEX_NPC, NpcIndex)		
	Say(10076, 2, OK_TRADE, NOT_TRADE)
	
end

function giaodich()
	local nParam = GetNpcValue(GetTaskTemp(TMP_INDEX_NPC))
	if(nParam ~= nil and nParam > 0) then
		Sale(nParam)
	end	
end

function no()
end


