-- @IncludeOnce  [LUA54 06/09 toi] tep toan ham: Lua54Dll chi chay than mot lan moi state (chon_includeonce.py)
--任务系统中各奖励扩展

--任务Npc死亡奖励经验
--其经验值等量提升120级技能修炼度
function AddExp_Skill_Extend(nExp)
	Add120SkillExp(floor(nExp / 2));	--任务中获得经验的 1/2
end;