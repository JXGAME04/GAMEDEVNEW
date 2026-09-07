-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local moruongchua
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