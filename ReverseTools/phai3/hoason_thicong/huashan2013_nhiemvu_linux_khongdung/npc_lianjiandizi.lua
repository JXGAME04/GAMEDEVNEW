Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\global\\skills_table.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\misc\\daiyitoushi\\toushi_function.lua")	-- ´øÒÕÍ¶Ê¦

-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\task\\150skilltask\\g_task.lua")
Include("\\script\\dailogsys\\g_dialog.lua")

function main()
    local UTask_hs = GetTask(3481)

    if UTask_hs == 0 then
	    Talk(1, "", "§Ö tö luyÖn kiÕm Hoa S¬n: kiÕm ph¸p ph¸i Hoa S¬n ta tinh diÖu v« phØ, kh¾p n¬i ®¹t tíi c¶nh giíi 'Chİnh hîp kú th¾ng, kiÕm trung cÇu th¾ng'")
    elseif UTask_hs == 20*256+10 then
	    Talk(1,"","§Ö tö luyÖn kiÕm Hoa S¬n: VŞ s­ ®Ö nµy cã muèn ®Êu víi ta vµi chiªu?")
    elseif UTask_hs == 20*256+20 then
	    task20_1()
    elseif UTask_hs == 20*256+30 then
	    Talk(2, "", "Ng­êi ch¬i: S­ huynh, cã thÓ thØnh gi¸o huynh vµi chiªu kh«ng?", "§Ö tö luyÖn kiÕm Hoa S¬n: §­¬ng nhiªn råi, ta ë DiÔn Vâ tr­êng ®îi ng­¬i.")
    elseif  UTask_hs >= 20*256+40 then
	    Talk(1, "", "Ng­êi ch¬i: S­ huynh, khiªm nh­êng råi.")
    else
	    Talk(1,"","§Ö tö luyÖn kiÕm Hoa S¬n: VŞ s­ ®Ö nµy cã muèn ®Êu víi ta vµi chiªu?")
    end
end

function task20_1()
	Talk(2, "task20_2", "Ng­êi ch¬i: S­ huynh, cã thÓ thØnh gi¸o huynh vµi chiªu kh«ng?", "§Ö tö luyÖn kiÕm Hoa S¬n: §­¬ng nhiªn lµ ®­îc, ng­¬i ®Õn DiÔn Vâ §­êng t×m ta, c¸c s­ huynh kh¸c còng ë ®ã, ng­¬i cã thÓ ®Êu víi 1 ng­êi bÊt kú.")
end

function task20_2()
	SetTask(3481, 20*256+30)
	SetTask(3482, 0)

	Msg2Player("§Ö tö luyÖn kiÕm muèn ng­¬i §Õn chç DiÔn Vâ §­êng thi ®Êu víi h¾n.")
	AddNote("§Ö tö luyÖn kiÕm muèn ng­¬i §Õn chç DiÔn Vâ §­êng thi ®Êu víi h¾n.")
end

function no()
end
