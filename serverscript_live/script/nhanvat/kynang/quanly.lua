
mangidnv = {0}

function main()
Talk(1,"no","Ky nang dung de tan gai :))")
end;
--function main()
--if GetRank() == 235 then 
--Say("<color=blue>Theo Doi:<color> Moi lua chon",3,"Trang thai binh thuong/hienhinh","Di chuyen den vi tri nhan vat/dcdvtnv","Thoat/no")
---else 
--Say("<color=blue>Theo Doi:<color> Moi lua chon",3,"Trang thai theo doi/tanghinh","Di chuyen den vi tri nhan vat/dcdvtnv","Thoat/no")
--end
--end;

function tanghinh()
Talk(2,"tanghinhok","Trong trang thai theo doi nhan vat <color=fire>se tro nen vo hinh<color>","Nhung khong the di chuyen <color=red>theo cach binh thuong<color>, ma phai thong qua ky nang Theo Doi")
end;

function hienhinh()
Talk(1,"hienhinhok","Khi tro ve trang thai binh thuong nhan vat se tro ve <color=fire>binh thuong<color> nhu moi nhan vat khac")
end;

function hienhinhok()
if GetRank() == 235 then 
SetRank(GetTask(1))
end
end;

function tanghinhok()
SetTask(1,GetRank())
SetRank(235)
end;

function dcdvtnvtd()
SetTaskTemp(10,1)
dcdvtnvtdt()
end;

function dcdvtnvtdt()
Say("<color=blue>Theo Doi:<color> Moi lua chon <color=fire>ID nhan vat<color> can den",11,"0/id0","1/id1","2/id2","3/id3","4/id4","5/id5","6/id6","7/id7","8/id8","9/id9","Thoat/no")
end;


function dcdvtnv()
Say("<color=blue>Theo Doi:<color> Moi lua chon <color=fire>cach di chuyen<color>",3,"Nhan vat co ID vua chon truoc/dcdvtnvtdvct","Chon lai ID nhan vat/dcdvtnvtd","Thoat/no")
end;

function dcdvtnvtdvct()

idcdcd = mangidnv[1]
idnv = PlayerIndex
PlayerIndex = idcdcd
w,x,y = GetWorldPos();
PlayerIndex = idnv

if W ~= 0 and x ~= 0 and y ~= 0 then
if GetRank() == 235 then
NewWorld(w,x,y);
KickOutSelf()
else
NewWorld(w,x,y);
end
Msg2Player("Da den vi tri nhan vat co ID "..idcdcd);
else
Msg2Player("Nhan vat khong con trong game !");
end

end;

function dcdvtnvtdtth()

idcdcd = GetTaskTemp(11)*100 + GetTaskTemp(12)*10 + GetTaskTemp(13)
mangidnv[1] = idcdcd

idnv = PlayerIndex
PlayerIndex = idcdcd
w,x,y = GetWorldPos();
PlayerIndex = idnv

if W ~= 0 and x ~= 0 and y ~= 0 then
if GetRank() == 235 then
NewWorld(w,x,y);
KickOutSelf()
else
NewWorld(w,x,y);
end
Msg2Player("Da den vi tri nhan vat co ID "..idcdcd);
else
Msg2Player("Nhan vat khong con trong game !");
end

end;





function no()


end;



function id0()
if GetTaskTemp(10) == 1 then
SetTaskTemp(11,0)
SetTaskTemp(10,2)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 2 then
SetTaskTemp(12,0)
SetTaskTemp(10,3)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 3 then
SetTaskTemp(13,0)
dcdvtnvtdtth()
end
end;


function id1()
if GetTaskTemp(10) == 1 then
SetTaskTemp(11,1)
SetTaskTemp(10,2)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 2 then
SetTaskTemp(12,1)
SetTaskTemp(10,3)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 3 then
SetTaskTemp(13,1)
dcdvtnvtdtth()
end
end;



function id2()
if GetTaskTemp(10) == 1 then
SetTaskTemp(11,2)
SetTaskTemp(10,2)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 2 then
SetTaskTemp(12,2)
SetTaskTemp(10,3)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 3 then
SetTaskTemp(13,2)
dcdvtnvtdtth()
end
end;


function id3()
if GetTaskTemp(10) == 1 then
SetTaskTemp(11,3)
SetTaskTemp(10,2)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 2 then
SetTaskTemp(12,3)
SetTaskTemp(10,3)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 3 then
SetTaskTemp(13,3)
dcdvtnvtdtth()
end
end;


function id4()
if GetTaskTemp(10) == 1 then
SetTaskTemp(11,4)
SetTaskTemp(10,2)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 2 then
SetTaskTemp(12,4)
SetTaskTemp(10,3)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 3 then
SetTaskTemp(13,4)
dcdvtnvtdtth()
end
end;


function id5()
if GetTaskTemp(10) == 1 then
SetTaskTemp(11,5)
SetTaskTemp(10,2)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 2 then
SetTaskTemp(12,5)
SetTaskTemp(10,3)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 3 then
SetTaskTemp(13,5)
dcdvtnvtdtth()
end
end;


function id6()
if GetTaskTemp(10) == 1 then
SetTaskTemp(11,6)
SetTaskTemp(10,2)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 2 then
SetTaskTemp(12,6)
SetTaskTemp(10,3)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 3 then
SetTaskTemp(13,6)
dcdvtnvtdtth()
end
end;


function id7()
if GetTaskTemp(10) == 1 then
SetTaskTemp(11,7)
SetTaskTemp(10,2)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 2 then
SetTaskTemp(12,7)
SetTaskTemp(10,3)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 3 then
SetTaskTemp(13,7)
dcdvtnvtdtth()
end
end;


function id8()
if GetTaskTemp(10) == 1 then
SetTaskTemp(11,8)
SetTaskTemp(10,2)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 2 then
SetTaskTemp(12,8)
SetTaskTemp(10,3)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 3 then
SetTaskTemp(13,8)
dcdvtnvtdtth()
end
end;

function id9()
if GetTaskTemp(10) == 1 then
SetTaskTemp(11,9)
SetTaskTemp(10,2)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 2 then
SetTaskTemp(12,9)
SetTaskTemp(10,3)
dcdvtnvtdt()
elseif GetTaskTemp(10) == 3 then
SetTaskTemp(13,9)
dcdvtnvtdtth()
end
end;