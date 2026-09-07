-- ====================== 文件信息 ======================

-- 剑侠情缘网络版大陆版 - 百人擂台NPC死亡
-- 文件名　：npc_death.lua
-- 创建者　：子非魚
-- 创建时间：2011-04-24 15:34:08

-- ======================================================

Include("\\script\\missions\\bairenleitai\\hundred_arena.lua");

-- 是哪个擂台的 -- > 擂主的名字
-- 是第几关的
-- 
-- GetNpcKind(nNpcIndex)

function OnDeath(nNpcIndex)
	
	if PlayerIndex <= 0 then
		-- 擂主输
		print("HundredArenaNPC Killed By NoOne!!");
		return
	end
	
	-- [JX1 23/08] HundredArena song trong state cua hundred_arena.lua (engine ta 1 state/tep)
	DynamicExecuteByPlayer(PlayerIndex, "\\script\\missions\\bairenleitai\\hundred_arena.lua", "HundredArena:JX1_NpcDeath", nNpcIndex)
	
end

-- [FIX 25/08] engine JX1 goi OnRevive VO DIEU KIEN moi lan NPC gan script nay hoi sinh
-- (ScriptFuns.cpp:7204/KNpc.cpp:8676); ban Linux khong dinh nghia -> ScriptError lap.
function OnRevive(npcindex)
end
