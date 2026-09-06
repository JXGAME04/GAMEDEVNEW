-------------------------------------------------------------------------
-- FileName		:	enemy_flyrefresh8089.lua
-- Author		:	xiaoyang
-- CreateTime	:	2005-04-13 11:10:14
-- Desc			:   ·çÖ®Æï¹Ø¿¨µÄbossÉí±ßË¢³ö¹Ö[80-89¼¶]
-------------------------------------------------------------------------
IncludeLib( "FILESYS" );
Include("\\script\\task\\tollgate\\messenger\\fengzhiqi\\enemy_flyrefresh.lua") 
Include("\\script\\task\\newtask\\newtask_head.lua") --µ÷ÓÃ nt_getTask Í¬²½±äÁ¿µ½¿Í»§¶ËµÄÀà
Include("\\script\\task\\tollgate\\messenger\\messenger_losetask.lua")  --È¡ÏûÈÎÎñ
Include("\\script\\task\\tollgate\\messenger\\messenger_timeer.lua")    --µ÷ÓÃ¼ÆÊ±Æ÷
TabFile_Load( "\\settings\\task\\tollgate\\messenger\\messenger_tollprize.txt" , "tollprize");	 --»ñµÃÍ¬°éÐÞÁ·µÄ±í¸ñ

FLYREFRESH_EXP =  tonumber( TabFile_GetCell( "tollprize" ,3,"shuachu_exp"))   --bossÉí±ßË¢³öÐ¡¹Ö¾­Ñé
FLYREFRESH_MAPID= 388  --·çÖ®ÆïµÄµØÍ¼id
FLYREFRESH_MESSENGEREXP= tonumber( TabFile_GetCell( "tollprize" ,3 ,"shuachu_jifen"))  --·çÖ®ÆïµÄbossÉí±ßË¢³öÐ¡¹ÖËÀÍö»ñµÃÐÅÊ¹»ý·Ö


function OnDeath()
	local name = GetName()
	local Uworld1215 = nt_getTask(1215)  --bossÉí±ßË¢¹ÖµÄ´ò¹Ö¿ª¹Ø
	if (  messenger_middletime() == 10 ) then --Íæ¼ÒÔÚµØÍ¼ÖÐµÄÊ±¼ä
		Msg2Player("Xin lçi! "..name.."! B¹n ®· hÕt thêi gian thùc hiÖn nhiÖm vô TÝn sø! NhiÖm vô thÊt b¹i!.")
		losemessengertask()
	--elseif ( Uworld1215 == 0 ) then  --Ã»ÓÐË¢¹ÖµÄÈÎÎñ
	--	Msg2Player("Õâ¸ö¼Ò»ïÒÑ¾­±»±ðÈË°üÁË£¬Äã´òËûÊÇ²»»áÓÐÈÎºÎ½±ÀøµÃ¡£")
	--	return
	else
		messenger_killrefresh()
	end
end
-- [FIX 25/08] engine JX1 goi OnRevive VO DIEU KIEN moi lan NPC gan script nay hoi sinh
-- (ScriptFuns.cpp:7204/KNpc.cpp:8676); ban Linux khong dinh nghia -> ScriptError lap.
function OnRevive(npcindex)
end
