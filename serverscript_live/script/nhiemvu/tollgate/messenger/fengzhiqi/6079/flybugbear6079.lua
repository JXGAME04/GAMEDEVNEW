-------------------------------------------------------------------------
-- FileName		:	enemy_flybugbear.lua
-- Author		:	xiaoyang
-- CreateTime	:	2005-04-06 18:30:14
-- Desc			:   ·çÖ®Æï¹Ø¿¨µÄÐ¡¹Ö½Å±¾[60-79¼¶]
-------------------------------------------------------------------------
IncludeLib( "FILESYS" );
Include("\\script\\task\\tollgate\\messenger\\fengzhiqi\\enemy_flybugbear.lua") 
Include("\\script\\task\\newtask\\newtask_head.lua") --µ÷ÓÃ nt_getTask Í¬²½±äÁ¿µ½¿Í»§¶ËµÄÀà
Include("\\script\\task\\tollgate\\messenger\\messenger_losetask.lua")  --È¡ÏûÈÎÎñ
Include("\\script\\task\\tollgate\\messenger\\messenger_timeer.lua")    --µ÷ÓÃ¼ÆÊ±Æ÷
TabFile_Load( "\\settings\\task\\tollgate\\messenger\\messenger_tollprize.txt" , "tollprize");	 --»ñµÃÍ¬°éÐÞÁ·µÄ±í¸ñ

FLYBUGBEAR_EXP =tonumber( TabFile_GetCell( "tollprize" ,2 ,"bug_exp"))  --Ð¡¹Ö¾­Ñé
FLYBUGBEAR_MAPID= 387  --·çÖ®ÆïµÄµØÍ¼id
FLYBUGBEAR_MESSENGEREXP= tonumber( TabFile_GetCell( "tollprize" ,2 ,"bug_jifen"))--·çÖ®ÆïµÄÐ¡¹ÖËÀÍö»ñµÃÐÅÊ¹»ý·Ö


function OnDeath()
	local name = GetName()
	if ( messenger_middletime() == 10  ) then --Íæ¼ÒÔÚµØÍ¼ÖÐµÄÊ±¼ä
		Msg2Player("Xin lçi! "..name.."! B¹n ®· hÕt thêi gian thùc hiÖn nhiÖm vô TÝn sø! NhiÖm vô thÊt b¹i!.")
		losemessengertask()
	else
		messenger_killbugbear()
	end
end
-- [FIX 25/08] engine JX1 goi OnRevive VO DIEU KIEN moi lan NPC gan script nay hoi sinh
-- (ScriptFuns.cpp:7204/KNpc.cpp:8676); ban Linux khong dinh nghia -> ScriptError lap.
function OnRevive(npcindex)
end
