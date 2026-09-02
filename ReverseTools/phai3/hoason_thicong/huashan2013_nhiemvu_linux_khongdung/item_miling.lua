Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
--Èë¿Úº¯Êı
function main(nItemIndex)
	local UTask_hs_90 = GetTask(3486)
	if UTask_hs_90 == 30 then
		SetTask(3486, 40)
		Msg2Player("MËt lÖnh hiÓn thŞ cã ng­êi g©y bÊt lîi cho Nh¹c Minh Phi, ®¹i hiÖp quyÕt ®Şnh quay l¹i T­¬ng D­¬ng xem t×nh h×nh Nh¹c Minh Phi thÕ nµo.")
		AddNote("MËt lÖnh hiÓn thŞ cã ng­êi g©y bÊt lîi cho Nh¹c Minh Phi, ®¹i hiÖp quyÕt ®Şnh quay l¹i T­¬ng D­¬ng xem t×nh h×nh Nh¹c Minh Phi thÕ nµo.")
	end
	Talk(2, "", "MËt lÖnh: KÎ nµo liªn quan ®Õn Nh¹c Minh Phi giÕt hÕt kh«ng tha!", "Ng­êi ch¬i: Kh«ng hay råi, ®¸m ng­êi nµy g©y bÊt lîi cho Minh Phi huynh, ta ph¶i quay l¹i xem sao!")

	return 1
end
