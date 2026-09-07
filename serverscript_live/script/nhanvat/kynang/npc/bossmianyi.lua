function GetSkillLevelData(levelname, data, level)

if (levelname == "fasthitrecover_v") then----------受伤动作时间减少点
return Getfasthitrecover_v(level)
end;

if (levelname == "fatallystrikeres_p") then--------抵抗致命一击%
return Getfatallystrikeres_p(level)
end;

if (levelname == "freezetimereduce_p") then------迟缓时间减少%
return Getfreezetimereduce_p(level)
end;

if (levelname == "poisontimereduce_p") then------中毒时间减少%
return Getpoisontimereduce_p(level)
end;

if (levelname == "stuntimereduce_p") then---------------眩晕时间减少%
return Getstuntimereduce_p(level)
end;

if (levelname == "meleedamagereturn_p") then------------受近身攻击时伤害反弹增加%
return Getmeleedamagereturn_p(level)
end;

if (levelname == "rangedamagereturn_p") then------------受远程攻击时伤害反弹增加%
return Getrangedamagereturn_p(level)
end;


if (levelname == "fastwalkrun_p") then------------提高移动速度%
return Getfastwalkrun_p(level)
end;

if (levelname == "allres_p") then------------所有抗性%
return Getallres_p(level)
end;


if (levelname == "physicsres_p") then------------普防增加%
return Getphysicsres_p(level)
end;


if (levelname == "poisonres_p") then------------毒防%
return Getpoisonres_p(level)
end;


if (levelname == "fireres_p") then------------火防%
return Getfireres_p(level)
end;


if (levelname == "lightingres_p") then------------雷防%
return Getlightingres_p(level)
end;


if (levelname == "coldres_p") then------------冰防%
return Getcoldres_p(level)
end;

if (levelname == "lifereplenish_v") then------------每半秒增加生命回复点
return Getlifereplenish_v(level)
end;


if (levelname == "addphysicsdamage_v") then------------外功系普攻伤害增加：#d1+点
return Getaddphysicsdamage_v(level)
end;


if (levelname == "addfiredamage_v") then------------外功系火攻伤害增加：#d1+点
return Getaddfiredamage_v(level)
end;


if (levelname == "addcolddamage_v") then------------外功系冰攻伤害增加：#d1+点
return Getaddcolddamage_v(level)
end;


if (levelname == "addlightingdamage_v") then------------外功系雷攻伤害增加：#d1+点
return Getaddlightingdamage_v(level)
end;


if (levelname == "addpoisondamage_v") then------------外功系毒攻伤害增加：#d1+点/次
return Getaddpoisondamage_v(level)
end;


if (levelname == "addphysicsdamage_p") then------------外功系普攻伤害增加：#d1+%
return Getaddphysicsdamage_p(level)
end;

if (levelname == "stun_p1") then------------攻击致昏对手几率：增加#d1-%
return Getstun_p(level)
end;





str1 = ""
return str1
end;

function Param2String(Param1, Param2, Param3)
return Param1..","..Param2..","..Param3
end;

function Getfasthitrecover_v(level)
return Param2String(100,129600,0)
end;

function Getfreezetimereduce_p(level)
return Param2String(200,129600,0)
end;

function Getpoisontimereduce_p(level)
return Param2String(300,129600,0)
end;

function Getstuntimereduce_p(level)
return Param2String(200,129600,0)
end;

function Getfatallystrikeres_p(level)
return Param2String(99,129600,0)
end

function Getmeleedamagereturn_p(level)
return Param2String(45,129600,0)
end;

function Getrangedamagereturn_p(level)
return Param2String(45,129600,0)
end;


function Getfastwalkrun_p(level)
return Param2String(1000,129600,0)
end;


function Getallres_p(level)
return Param2String(50,129600,0)
end;

function Getphysicsres_p(level)
return Param2String(50,129600,0)
end;

function Getpoisonres_p(level)
return Param2String(50,129600,0)
end;

function Getfireres_p(level)
return Param2String(50,129600,0)
end;

function Getlightingres_p(level)
return Param2String(50,129600,0)
end;

function Getcoldres_p(level)
return Param2String(50,129600,0)
end;

function Getlifereplenish_v(level)
return Param2String(100,129600,0)
end;

function Getaddphysicsdamage_v(level)
return Param2String(200,129600,0)
end;

function Getaddfiredamage_v(level)
return Param2String(200,129600,0)
end;

function Getaddcolddamage_v(level)
return Param2String(200,129600,0)
end;

function Getaddlightingdamage_v(level)
return Param2String(200,129600,0)
end;

function Getaddpoisondamage_v(level)
return Param2String(30,129600,0)
end;

function Getaddphysicsdamage_p(level)
return Param2String(100,129600,0)
end;

function Getstun_p(level)
return Param2String(20,129600,0)
end;