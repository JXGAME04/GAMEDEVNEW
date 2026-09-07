-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local Getmanareplenish_v
function GetSkillLevelData(levelname, data, level)

if (levelname == "lifereplenish_v") then
return Getlifereplenish_v(level)
end;

if (levelname == "manareplenish_v") then
return Getmanareplenish_v(level)
end;


str1 = ""
return str1
end;

function Param2String(Param1, Param2, Param3)
return Param1..","..Param2..","..Param3
end;

function Getlifereplenish_v(level)
result = floor(level/2)+1
return Param2String(result,20,0)
end;

function Getmanareplenish_v(level)
result = floor(level/3)+1
return Param2String(result,25,0)
end;