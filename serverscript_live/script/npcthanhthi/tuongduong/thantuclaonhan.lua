--Author: Fong KiÒu
--Date: 2021
--Funtion: NPC thµnh T­¬ng D­¬ng ho¹t ®éng cµy xu miÔn phÝ

Include("\\script\\header\\taskid.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\global\\vatpham.lua")
Include("\\script\\log_game\\save_log.lua")

TYLE_NHKT2XU = 5

function main()
	dofile("script/npcthanhthi/tuongduong/thantuclaonhan.lua")
	local OPTIONS = {}
	tinsert(OPTIONS, "<npc>: <sex> h·y ®i t×m <color=red>Ngò hµnh kú th¹ch<color> mang l¹i ®©y cho ta.")
	tinsert(OPTIONS, "T×m hiÓu ho¹t ®éng [cµy xu] miÔn phÝ./timhieu")
	tinsert(OPTIONS, format("Ta muèn ®æi lÊy 10 tiÒn xu./#NHKT2XU(%d)",10))
	tinsert(OPTIONS, format("Ta muèn ®æi lÊy 50 tiÒn xu./#NHKT2XU(%d)",50))
	tinsert(OPTIONS, format("Ta muèn ®æi lÊy 100 tiÒn xu./#NHKT2XU(%d)",100))
	tinsert(OPTIONS, "§îi chót ta sÏ quay trë l¹i./no")
	SayEx(OPTIONS)
end

function NHKT2XU(nXuInput)
	local nXuAdd = nXuInput
	local nNHKT = nXuInput * TYLE_NHKT2XU
	if(GetItemCount(0, ITEM_NHKT[1],ITEM_NHKT[2], ITEM_NHKT[3]) < nNHKT) then
		Talk(1,"","CÇn cã "..nNHKT.." Ngò hµnh kú th¹ch trong hµnh trang")
		return
	end
	if (ConsumeItem(nNHKT,0,ITEM_NHKT[1],ITEM_NHKT[2],ITEM_NHKT[3]) == nNHKT) then
		AddEventItem(ID_XU_TIENDONG, nXuAdd)
		logDoiKNB(format("[%s]\t%s\tAccount:%s\tName:%s\t NHKT2XU :%d", "Cµy Xu", GetLocalDate("%Y-%m-%d %X"), GetAccount(), GetName(), nNHKT ))
	else
		Talk(1,"","CÇn cã "..nNHKT.." Ngò hµnh kú th¹ch trong hµnh trang")
	end
end

function timhieu()
	Talk(1,"","Tham gia c¸c ho¹t ®éng trong ngµy <color=green>Tèng Kim, V­ît ¶i, Phong l¨ng ®é<color> thu thËp c¸c B¶o R­¬ng më ra <color=red>may m¾n<color> sÏ nhËn ®­îc <color=pink>Ngò hµnh kú th¹ch<color>")
end

function no()
end
