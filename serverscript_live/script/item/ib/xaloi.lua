--Author: Fong KiÒu
--Function: Thæ §Þa phï
--Date: 21/07/2021

Include("\\script\\header\\revivepos_head.lua")
Include("\\script\\header\\forbidmap.lua")
Include("\\script\\header\\taskid.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\Script\\Global\\station.lua")

function main(nIndex)

	-- dofile("script/item/ib/xaloi.lua")
	
if GetPK() == 0 or GetPK() == 10 then
	Talk(1,"","§iÓm PK cña b¹n hiÖn t¹i lµ: <color=red>"..GetPK().."<color>, kh«ng thÓ sö dông vËt phÈm ") 
	return
end
	SetPK(GetPK()-1)
	Msg2Player("B¹n ®· gi¶m ®­îc 1 PK. §iÓm PK hiÖn t¹i cña b¹n lµ: "..GetPK().." !"); 
	RemoveItem(nIndex)
end

