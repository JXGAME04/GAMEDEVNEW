-- ================================================================================================
-- [HE THONG] script/global/seasonnpc_death.lua
-- Muc dich  : Da Tau: xu ly NPC chet.
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)
-- Ham (dong): OnDeath (15), DoTeamProcess (22), addSeasonnpcExp (37)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================

-- ====================== ÎÄ¼şĞÅÏ¢ ======================

-- ½£ÏÀÇéÔµonline Ò°ÛÅËÀÍö½Å±¾
-- Edited by peres
-- 2006/12/26 PM 15:39

-- æÌÊ¦½ãÎªÎÒ´©ÉÏÒÂ·şÊ±Í»È»Ğ¦×ÅËµµÀ£¬ÃÃÃÃ³öÂäµÃÔ½À´Ô½Ë®ÁéÁË¡£
-- ²»¶®ÕâÏÉÅ®°ãµÄÓñÌåÒÔºó»áÍĞ¸¶¸øÄÄ¸öÄĞÈËÄØ¡£
-- Å®×Ó³¤´óÒÔºóÒ»¶¨»á½«Éí×ÓÍĞ¸¶¸øÄĞÈËÂğ£¿ÎÒµÄÁ³Î¢Î¢ÕÇºìÁËÒ»ÏÂ¡£
-- æÌÊ¦½ãÍäÏÂÑü°ïÎÒ°ÑÈ¹´øÊøºÃ£¬ËıËµ£¬ÊÇµÄ¡£
-- ÒòÎª£¬ÕâÊÇÅ®×ÓÒ»ÉúµÄËŞÃü¡£

-- ======================================================

function OnDeath()
	DoTeamProcess(addSeasonnpcExp);
end;


-- ¶ÔËùÓĞ¶ÓÔ±Ö´ĞĞÍ¬Ò»¹ı³Ì
function DoTeamProcess(funProcess)

local nPreservedPlayerIndex = PlayerIndex;
local nMemCount = GetTeamSize();

	if (nMemCount == 0) then
		funProcess();
	else
		for i = 1, nMemCount do -- ÔÚÕâÀï¿ªÊ¼Ñ­»·±éÀúÃ¿¸öÍæ¼Ò
			PlayerIndex = GetTeamMember(i);
			funProcess();
		end;
		PlayerIndex = nPreservedPlayerIndex; -- Ñ­»·½áÊøºóÔÚÕâÀï¹é»¹Ö÷Íæ¼Ò ID
	end;
end;


function addSeasonnpcExp()
	AddOwnExp(50000);
	Msg2Player("Hoµn thµnh nhiÖm vô D· TÈu nhËn ®­îc <color=green>50000 ®iÓm kinh nghiÖm<color>!");
end;