-- ================================================================================================
-- [HE THONG] script/npclevelscript/soldier6.lua
-- Muc dich  : AI + THUOC TINH NPC theo cap (cot LevelScript trong settings/npcs.txt tro toi tep nay theo ID bam). 14 ham chuan: OnCreate/OnDeath/GetAttrib...
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)
-- Include   : property.lua
-- Ham (dong): GetNpcLevelData (2), GetNpcKeyData (174), SetKeyData (197), SetLevelData (210)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
Include("\\script\\npclevelscript\\property.lua")

function GetNpcLevelData(Series,Level, StyleName, ParamStr) 
if(ParamStr=="") then 
print(StyleName) 
end 
if(StyleName=="FireResist") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
return SetFireResist(Level,param1,param2); 
end; 
if(StyleName=="PhysicsResist") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
return SetPhysicsResist(Level,param1,param2); 
end; 
if(StyleName=="ColdResist") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
return SetColdResist(Level,param1,param2); 
end; 
if(StyleName=="LightResist") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
return SetLightResist(Level,param1,param2); 
end; 
if(StyleName=="PoisonResist") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
return SetPoisonResist(Level,param1,param2); 
end; 
if(StyleName=="PhysicalDamageBase") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="PoisonDamageBase") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="ColdDamageBase") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="FireDamageBase") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="LightingDamageBase") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="PhysicalMagicBase") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="PoisonMagicBase") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="ColdMagicBase") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="FireMagicBase") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="LightingMagicBase") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="Level1") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="Level2") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="Level3") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="Level4") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="AuraSkillLevel") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
param3=GetParam(ParamStr,3); 
return SetLevelData(Level, param1, param2, param3); 
end; 
if(StyleName=="AuraSkillId") then 
return ParamStr 
end 
if(StyleName=="PasstSkillLevel") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
return SetPasstSkillLevel(Level,param1,param2); 
end; 
if(StyleName=="PasstSkillId") then 
return ParamStr 
end 
if(StyleName=="LifeReplenish") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
return SetLifeReplenish(Level,param1,param2); 
end; 
if(StyleName=="AIMode") then 
return ParamStr 
end 
if(StyleName=="AIParam1") then 
return ParamStr 
end 
if(StyleName=="AIParam2") then 
return ParamStr 
end 
if(StyleName=="AIParam3") then 
return ParamStr 
end 
if(StyleName=="AIParam4") then 
return ParamStr 
end 
if(StyleName=="AIParam5") then 
return ParamStr 
end 
if(StyleName=="AIParam6") then 
return ParamStr 
end 
if(StyleName=="AIParam7") then 
return ParamStr 
end 
if(StyleName=="AIParam8") then 
return ParamStr 
end 
if(StyleName=="AIParam9") then 
return ParamStr 
end 

if(StyleName=="Skill1" or StyleName=="Skill2" or StyleName=="Skill3" or StyleName=="Skill4") then 
return ParamStr 
end 
return Linear(Level,1,1); 
end; 

function GetNpcKeyData(Series,Level, StyleName, Param1, Param2, Param3) 
if (StyleName == "Exp") then 
return SetExp(Level, Param1, Param2,Param3); 
end; 
if (StyleName == "Life") then 
return SetKeyData(Level, Param1, Param2, Param3); 
end; 
if (StyleName == "AttackRating") then 
return SetKeyData(Level, Param1, Param2, Param3); 
end; 
if (StyleName == "Defense") then 
return SetKeyData(Level, Param1, Param2, Param3); 
end; 
if (StyleName == "MinDamage") then 
return SetKeyData(Level, Param1, Param2, Param3); 
end; 
if (StyleName == "MaxDamage") then 
return SetKeyData(Level, Param1, Param2, Param3); 
end; 

return Quadratic(Level,Param1,Param2,Param3); 
end; 

function SetKeyData(Level, Param1, Param2, Param3) 
if ( Level <= 75 ) then 
return floor(Param1) 
end 
if ( Level <= 95 ) then 
return floor(Param2) 
end 
if ( Level <= 100 ) then 
return floor(Param3) 
end 
return floor(Param3) 
end 

function SetLevelData(Level, Param1, Param2, Param3) 
if ( Level <= 75 ) then 
return floor(Param1) 
end 
if ( Level <= 95 ) then 
return floor(Param2) 
end 
if ( Level <= 100 ) then 
return floor(Param3) 
end 
return floor(Param3) 
end