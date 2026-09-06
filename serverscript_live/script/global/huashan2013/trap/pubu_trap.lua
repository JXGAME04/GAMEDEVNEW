Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
--»ªÉ½ÆÙ²¼trapµã
function main(sel)
	local UTask_hs = GetTask(3481)

	if UTask_hs == 60*256+60 then
		SetTask(3481, 60*256+70)
		if CalcItemCount(-1, 6, 1, 4957, -1) >= 1  then
			ConsumeItem(-1, 1, 6, 1, 4957, -1)
		end
		Msg2Player("C¸c h¹ ng¾m nh×n th¸c n­íc, nh­ ®· lÜnh ngé ®­îc ­u miÒu bªn trong, ®· cã thÓ kiÓm tra cÈm nang råi.")
		AddNote("C¸c h¹ ng¾m nh×n th¸c n­íc, nh­ ®· lÜnh ngé ®­îc ­u miÒu bªn trong, ®· cã thÓ kiÓm tra cÈm nang råi.")
	end
end
