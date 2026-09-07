Include("\\script\\header\\taskid.lua")
Include("\\script\\lib\\lib_ham.lua")

function main()
	dofile("script/npcthanhthi/laman/volamminhchu.lua")
	local OPTIONS = {}
	tinsert(OPTIONS, "<npc>: <sex> cÇn ta gióp ®ì g× kh«ng?")
	tinsert(OPTIONS, "T×m hiÓu C«ng Tr¹ng LÖnh vµ Huy ch­¬ng Tèng Kim./timhieu")
	tinsert(OPTIONS, "§Ó ta suy nghÜ l¹i./no")
	SayEx(OPTIONS)
end

function timhieu()
	Talk(1,"","TÝnh n¨ng nµy ®ang trong qu¸ tr×nh hoµn thiÖn.")
end

function no()
end