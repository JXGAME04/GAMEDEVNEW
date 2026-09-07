-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local Getpoisonenhance_p
function GetSkillLevelData(levelname, data, level)

if (levelname == "poisonenhance_p") then
return Getpoisonenhance_p(level)
end;


str1 = ""
return str1
end;

function Param2String(Param1, Param2, Param3)
return Param1..","..Param2..","..Param3
end;

function Getpoisonenhance_p(level)
result = 10+2*level
return Param2String(result,-1,0)
end;