-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Chuc nang: Ruong chua

function main(NpcIndex)
	-- dofile("\script\global\npc\buysell\box.lua")
	moruongchua(NpcIndex)
end

function moruongchua(NpcIndex)
	OpenBox()
	local nRevivalid = GetNpcValue(NpcIndex)
	if(nRevivalid ~= nil and nRevivalid >= 0) then
		SetRevPos(nRevivalid)
	end
end