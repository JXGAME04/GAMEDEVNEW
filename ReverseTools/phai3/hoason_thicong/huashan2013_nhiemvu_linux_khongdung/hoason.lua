Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\task\\newtask\\newtask_head.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\task\\newtask\\education\\knowmagic.lua")
Include("\\script\\global\\skills_table.lua")
Include("\\script\\global\\map_helper.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include( "\\script\\missions\\freshman_match\\head.lua" )
function main() 
	dialog_main()	
end


function dialog_main()
	local szTitle = "LÇn nµy ph¸i <color=blue>Hoa S¬n<color> ta t¸i suÊt,  giang hå l¹i thªm nhiÒu biÕn ®éng"
	local tbOpt =
	{
		"§Õn ph¸i Hoa S¬n/vep",
		"Gia nhËp ph¸i Hoa S¬n/noi",
		"Tho¸t/no",
	}
	
	Say(szTitle, getn(tbOpt), tbOpt)
end

function noi()
	local FactionNumber = GetLastFactionNumber()
	local nSeries = GetSeries()
	if nSeries ~= 2 then
		Talk(1, "", "Nh©n vËt hÖ Thñy míi cã thÓ gia nhËp Hoa S¬n")
		return
	end
	if FactionNumber >= 0 then
		Talk(1, "", "Ng­¬i ®· gia nhËp m«n ph¸i.")
		return
	end
	if GetLevel()<10 then
		Talk(1, "", "H·y tù luyÖn tËp thªm ®Õn cÊp 10 råi quay l¹i ®©y.")
		return
	end
	Say("Ph¸i <color=cyan>Hoa S¬n<color> ta ®· kh«ng tham gia giang hå h¬n 10 n¨m nay, lÇn nµy t¸i xuÊt ¾t sÏ khiÕn bän giang hå tµ ph¸i ph¶i khiÕp sî!",2, "Gia nhËp Hoa S¬n/go", "§Ó ta suy nghÜ kü l¹i xem/no")
end


function go() 
	Talk(1,"","Chóc mõng c¸c h¹ ®· gia nhËp <color=blue>Hoa S¬n<color>")
--NewWorld(987,1389,3086)
	SetTask(169,10)
	nt_setTask(137,71)
	SetTask(3481,10*256);
	SetRank(82)	
	SetFaction("huashan")  
	SetLastFactionNumber(10)
	SetCurCamp(3)
	SetCamp(3)
	add_hs(GetLevel())
	Msg2Player("ChÝnh thøc gia nhËp Hoa S¬n ph¸i, häc ®­îc kiÕm ph¸p c¬ b¶n")
--AddGoldItem(0, 4639)
--AddGoldItem(0, 4649)
--Msg2Player("NhËn ®­îc <color=yellow>trang bÞ hoµng kim cÊp Thanh C©u.")
end 

function vep() 
Msg2Player("Ngêi yªn, chóng ta ®i Hoa S¬n ph¸i")
NewWorld(987,1389,3086)
end 

function no() 
end 

