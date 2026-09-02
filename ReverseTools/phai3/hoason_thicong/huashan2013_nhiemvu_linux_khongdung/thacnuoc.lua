Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
--Edit by Youtube PGaming--
Include("\\script\\dailogsys\\dailogsay.lua")
function main() 
if (GetTask(169) == 67) then
	DelCommonItem(6,1,4957)
	SetTask(169,68)
	AddNote("C¸c h¹ ng¾m nh×n th¸c n­íc, nh­ ®· lÜnh ngé ®­îc ­u miªu bªn trong, ®· cã thÓ kiÓm tra cÈm nang råi.") 
Msg2Player("C¸c h¹ ng¾m nh×n th¸c n­íc, nh­ ®· lÜnh ngé ®­îc ­u miªu bªn trong, ®· cã thÓ kiÓm tra cÈm nang råi.") 
end
end


