-- Manh ban do Son Ha Xa Tac: 439 = +100 manh, 2523 = +1000 manh (task 1027).
-- [JX1 PORT 16/08/2026] remap particular theo bang item JX1: 440->439, 2514->2523.
-- [PB 19/08/2026] Them mau cho so manh + SUA LOI thieu dau cach ("tong cong100").
-- File thuan ASCII, tieng Viet bang escape thap phan \\ddd cua Lua 4.
-- Dau cach ASCII truoc <color...> khi phia truoc la chu Viet (Text.cpp:468).

Include("\\script\\task\\newtask\\newtask_head.lua")

function main(nItemIdx)
	local _,_,detail = GetItemProp(nItemIdx)
	local nWorldMaps = nt_getTask(1027)
	local nThem = 0
	if (detail == 439) then
		nThem = 100
	elseif (detail == 2523) then
		nThem = 1000
	end
	if (nThem > 0) then
		nWorldMaps = nWorldMaps + nThem
		nt_setTask(1027,nWorldMaps)
		Msg2Player("B\185n nh\203n \174\173\238c".." <color=Yellow>"..nThem.."<color> ".."m\182nh b\182n \174\229 S\172n H\181 X\183 T\190c! Hi\214n t\185i b\185n c\227 t\230ng c\233ng"..
			" <color=AYellow>"..nWorldMaps.."<color>".." m\182nh b\182n \174\229 S\172n H\181 X\183 T\190c.");
	end
end
