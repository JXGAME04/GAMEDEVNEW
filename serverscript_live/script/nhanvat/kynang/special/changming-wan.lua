-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local Getlifemax_v
function GetSkillLevelData(levelname, data, level)
	if (levelname == "lifemax_v") then
		return Getlifemax_v()
	end;

	return ""
end;

function Getlifemax_v()
	return Param2String(500,5400,0)
end;

function Param2String(Param1, Param2, Param3)
	return Param1..","..Param2..","..Param3
end;
