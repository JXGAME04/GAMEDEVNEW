-- 新任务系统正派支线管理npc
-- BY：XIAOYANG（2004-11-30）

Include("\\script\\task\\newtask\\branch\\zhongli\\branch_liunanyun.lua")
Include("\\script\\task\\newtask\\branch\\zhengpai\\branch_aoyunzong.lua")
Include("\\script\\task\\newtask\\branch\\xiepai\\passerby_tuobahuaichuan.lua")
Include("\\script\\task\\newtask\\newtask_head.lua")

function main()
local curCamp = nt_getCamp()
if  ( curCamp == 0 ) then
task_aoyunzong()
elseif  ( curCamp == 1 ) then
task_liunanyun()
elseif  ( curCamp == 2 ) then
task_tuobahuaichuan()
else
Say("Xu蕋 hi謓 l鏸 sai Camp")
end
end