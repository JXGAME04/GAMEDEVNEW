--Author: Fong KiÒu

Include("\\script\\lib\\lib_task.lua")

NOW_END_SAY 				= "KÕt thóc ®èi tho¹i./no"
ADMIN_SUPPORT_TEXT = "Chøc n¨ng hç trî qu¶n trŞ viªn"

function hotro()
	SayEx({ADMIN_SUPPORT_TEXT,
		--"TiÒn xu/tienxu",
		--"TiÒn v¹n/tienvan",
		"§iÓm tèng kim/pointtongkim",
		"§iÓm danh väng/danhvong",
		"§iÓm phóc duyªn/phucduyen",
		"§iÓm l·nh ®¹o/lanhdao",
		"§iÓm kü n¨ng/pointkynang",
		"§iÓm tiÒm n¨ng/pointtiemnang",
		"§iÓm liªn ®Êu/pointch",
		"§iÓm tİch lòy/pointtichluy",	
		NOW_END_SAY})
end

function pointtongkim()
	SetTask(T_TONGKIM,GetTask(T_TONGKIM)+3000)
	Msg2Player("B¹n nhËn ®­îc 3000 ®iÓm tèng kim.")
end

function pointch()
	SetTask(T_LIENDAU,GetTask(T_LIENDAU)+1000)
	Msg2Player("B¹n nhËn ®­îc 1000 ®iÓm liªn ®Êu.")
end

function pointtichluy()
	SetTask(T_TICHLUYCT,GetTask(T_TICHLUYCT)+1000)
	Msg2Player("B¹n nhËn ®­îc 1000 ®iÓm tİch lòy.")
end

function pointkynang()
	AddMagicPoint(200)
	Msg2Player("B¹n nhËn ®­îc 200 ®iÓm kü n¨ng ")
end;

function pointtiemnang()
	AddProp(1000)
	Msg2Player("B¹n nhËn ®­îc 1000 ®iÓm tiÒm n¨ng ")
end
	
function lanhdao()
	for i=1,20 do
		AddLeadExp(100000000)
	end
end

function tienvan()
	Earn(1000000000)
end

function tienxu()
	EarnExtPoint(1000)
end

function danhvong()
	n=random(100,300)
	AddRepute(n)
end

function phucduyen()
	AddFuYuan(1000)
	Msg2Player("§· nhËn ®­îc 1000 ®iÓm phóc duyªn")
end

function pk()
	SetPK(GetPK() + 1)
end
