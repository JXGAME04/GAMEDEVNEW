
Include("\\Script\\lib\\lib_task.lua")

function main(nItemIdx)
if GetLevel() >= 70 then
	if GetTask(TaskDBCH) >= 2880 then
	Talk(1,"","Thêi gian ñy th¸c tèi ®a lµ <color=red>48 giê <color=red>, b¹n kh«ng thÓ sö dông thªm.")
	else
	RemoveItem(nItemIdx,1)

	SetTask(TaskDBCH,GetTask(TaskDBCH)+480)
	Msg2Player("Thêi gian §¹i B¹ch CÇu Hoµn cña b¹n cßn "..giodbch().." giê "..phutdbch().." phót.")
	end
else
Talk(1,"","§¼ng cÊp 70 míi cã thÓ sö dông B¹ch CÇu Hoµn.")
end
end

function giodbch()
if GetTask(TaskDBCH) < 60 then
return 0
else
gio = floor((GetTask(TaskDBCH)/60),2)
return gio
end
end

function phutdbch()
if GetTask(TaskDBCH) < 60 then
return GetTask(TaskDBCH)
else
gio = floor((GetTask(TaskDBCH)/60),2)
phut = GetTask(TaskDBCH) - gio*60
return phut
end
end

function no()
end