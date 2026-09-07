function main(ItemName)
idx = PlayerIndex
name1 = GetName()
acc1 = GetAccount()
ip1 = GetIP()
 
PlayerIndex = idx

local nYr,nMo,nDy,nHr,nMi,nSe,nDyfW = GetTimeNow()		
thoigian = tonumber(date("%d%m%y"))

local danhsach = openfile("dulieu/LogVutDo/Drop "..thoigian..".txt", "a");
if danhsach then
write(danhsach,""..nHr.."Giê"..nMi.."Phót"..nSe.."Gi©y: ["..acc1.."]["..name1.."]["..ip1.."] - "..ItemName.."\n");
closefile(danhsach)
end
end
