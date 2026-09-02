Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\global\\skills_table.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\misc\\daiyitoushi\\toushi_function.lua")	-- ´øÒÕÍ¶Ê¦

-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\task\\150skilltask\\g_task.lua")
Include("\\script\\dailogsys\\g_dialog.lua")

function main()
    local UTask_hs = GetTask(3481)

    if UTask_hs == 60*256+20 then
	    Talk(2, "task60_1", "Ng­êi ch¬i: ta ®Õn ®Ó hç trî tu söa ThÇn KiÕm cña bæn m«n.", "¢n KiÕm Thu: 2 thanh huy li kiÕm nµy vèn lµ thÇn kiÕm cña Ch­ëng m«n c¸c ®êi truyÒn l¹i, nh­ng ®· bŞ tæn h¹i tõ cuéc chiÕn nhiÒu n¨m tr­íc, ta phông mÖnh tu söa ®· l©u, nh­ng vÉn ch­a thÓ hoµn thµnh, vÉn cÇn thªm m·nh thó hung d÷ nhÊt thiªn h¹ dïng m¸u ®Ó tÕ tu, ng­¬i h·y gióp ta ®i lÊy vÒ ®i. Nghe nãi m·nh thó nµy th­êng xuÊt hiÖn t¹i khu vùc T©y Vùc TuyÕt B¸o §éng.")
    elseif UTask_hs == 60*256+40 then
	    task60_2()
    elseif UTask_hs == 60*256+30 then
	    Talk(1, "", "¢n KiÕm Thu: B¹ch hæ huyÕt mµ ta cÇn ®©u?")
    else
	    Talk(1, "", "¢n KiÕm Thu: c¸c h¹ cÇn g×?")
    end
end

function task60_1()
	task60_go()
end

function task60_2()
	if CalcItemCount(-1, 6, 1, 4956, -1) >= 1  then
		ConsumeItem(-1, 1, 6, 1, 4956, -1)
	end

	if CalcItemCount(3, 6, 1, 4959, -1) >= 1  then
		Talk(1, "task60_finish", "¢n KiÕm Thu: ng­¬i lµm tèt l¾m! ThÇn binh Huy Li t¸i xuÊt sÏ kh«ng cßn xa n÷a.")
	else
		Talk(1, "", "¢n KiÕm Thu: B¹ch hæ huyÕt mµ ta cÇn ®©u?")
	end
end

function task60_go()
	SetTask(3481, 60*256+30)
	Msg2Player("§Õn khu vùc TuyÕt B¸o §éng tiªu diÖt §iÕu T×nh B¹ch Hæ ®Ó ®o¹t lÊy m¸u.")
	AddNote("§Õn khu vùc TuyÕt B¸o §éng tiªu diÖt §iÕu T×nh B¹ch Hæ ®Ó ®o¹t lÊy m¸u.")
end

function task60_finish()
	ConsumeItem(3, 1, 6, 1, 4959, -1)

	SetTask(3481, 60*256+50)
	Msg2Player("§· hç trî ¢n KiÕm Thu, cã thÓ kiÓm tra cÈm nang råi.")
	AddNote("§· hç trî ¢n KiÕm Thu, cã thÓ kiÓm tra cÈm nang råi.")
end

function no()
end
