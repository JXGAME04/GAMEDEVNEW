Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
function main()
	local UTask_hs_150 = GetTask(2885)
	if UTask_hs_150 == 1 then
		Talk(9, "task150_1", "C« g¸i khãc: hu hu hu, sî qu¸!", "Ng­êi ch¬i: C« n­¬ng, cã chuyÖn g× thÕ?", "C« g¸i khãc: xem t­íng m¹o cña ®¹i hiÖp, ®¹i hiÖp cã ph¶i lµ ng­êi trong giang hå?", "§óng vËy.", "C« g¸i khãc: ng­êi giang hå thËt ®¸ng sî, huhu, tê mê s¸ng h«m qua, n« gia ®ét nhiªn thÊy cã vµi bãng ®en tõ cöa sæ bay vµo, n« gia sî qu¸, véi gi¶ vê ngñ, mÆc dï kh«ng cã chuyÖn g×, nh­ng s¸ng nay nghÜ l¹i thËt lµ ®¸ng sî.", "Ng­êi ch¬i: c« n­¬ng ®õng sî, c« n­¬ng cßn nh×n thÊy hay nghe thÊy g× n÷a kh«ng?", "C« g¸i khãc: N« gia...n« gia chØ nghe lo¸ng tho¸ng hä nãi '7 ngµy sau, trÊn khÈu nam m«n' g× g× ®ã, cßn l¹i kh«ng nghe râ.", "Ng­êi ch¬i: ®a t¹ chØ gi¸o.", "Ng­êi ch¬i: H·y mau nãi chuyÖn nµy cho T« S­ huynh biÕt.")
	else
		Talk(1, "", "C« g¸i khãc: hu hu hu, sî qu¸!")
	end
end

function task150_1()
	SetTask(2885, 2)
	Msg2Player("Trë vÒ m«n ph¸i t×m T« Phãng. ")
end
