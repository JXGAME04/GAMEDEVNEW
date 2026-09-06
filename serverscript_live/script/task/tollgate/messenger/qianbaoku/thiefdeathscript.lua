Include("\\script\\event\\birthday_jieri\\200905\\class.lua");
Include("\\script\\task\\tollgate\\messenger\\lib_messenger.lua")

function OnDeath( nNpcIndex )
	SetTask2Team(tbBirthday0905.tbTask.tsk_msg_curtsk,tbBirthday0905.tbTask.tsk_msg_needcount, 5);
end;
-- [FIX 25/08] engine JX1 goi OnRevive VO DIEU KIEN moi lan NPC gan script nay hoi sinh
-- (ScriptFuns.cpp:7204/KNpc.cpp:8676); ban Linux khong dinh nghia -> ScriptError lap.
function OnRevive(npcindex)
end
