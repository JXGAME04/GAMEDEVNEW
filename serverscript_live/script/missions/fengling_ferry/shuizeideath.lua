Include("\\script\\lib\\awardtemplet.lua")

Include("\\script\\event\\jiefang_jieri\\200904\\shuizei\\shuizei.lua");
Include("\\script\\misc\\eventsys\\type\\func.lua")
Include("\\script\\header\\cauhinh_hoatdong.lua")	-- [3HD] noi cauhinh
function OnDeath( nNpcIndex )
	--local nRate = 0.01;
	--local nCurRate	= random(10000000)
	--if (nCurRate <= 10000000 * nRate) then	
	local nCurRate	= random(1,100)
	if nCurRate < HD_CFG("HD3_PLD_TILE_TRUYCONG", 50) then
		local x, y, world = GetNpcPos(nNpcIndex);
		if (jf0904_shuizei_IsActtime() == 1) then
			local nItemIdx = DropItem(nNpcIndex, 6, 1, 2024, 1, 0, 0);
			
			local nCurtime = tonumber(GetLocalDate("%H%M"));
			local nRestMin = 24 * 60 - (floor(nCurtime/100)*60+floor(mod(nCurtime, 100)));
			ITEM_SetExpiredTime(nItemIdx, nRestMin);
			SyncItem(nItemIdx);
		end
	end
	
end

-- [3HD 25/08 C41] Engine JX1 goi OnRevive cho MOI NPC co script chet moi lan
-- hoi sinh; ban Linux khong dinh nghia (engine JX2 khong goi) => ScriptError
-- "attempt to call a nil value" lap lai moi nhip. Ham rong = giu nguyen hanh vi.
function OnRevive()
end
