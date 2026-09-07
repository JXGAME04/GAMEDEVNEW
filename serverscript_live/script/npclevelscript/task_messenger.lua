-- ================================================================================================
-- [HE THONG] script/npclevelscript/task_messenger.lua
-- Muc dich  : AI + THUOC TINH NPC theo cap (cot LevelScript trong settings/npcs.txt tro toi tep nay theo ID bam). 14 ham chuan: OnCreate/OnDeath/GetAttrib...
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)
-- Include   : property.lua
-- Ham (dong): Line (2), Link (9), GetNpcLevelData (36), GetNpcKeyData (236)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
Include("\\script\\npclevelscript\\property.lua")

function Line(x,x1,y1,x2,y2) 
if(x2==x1) then 
return y2 
end 
	return (y2-y1)*(x-x1)/(x2-x1)+y1
end 

function Link(x,points) 
num = getn(points) 
if(num<2) then 
return -1 
end 
for i=1,num do 
if(points[i][3]==nil) then 
points[i][3]=Line 
end 
end 
if(x < points[1][1]) then 
return points[1][3](x,points[1][1],points[1][2],points[2][1],points[2][2]) 
end 
if(x > points[num][1]) then 
return points[num][3](x,points[num-1][1],points[num-1][2],points[num][1],points[num][2]) 
end 

c = 2 
for i=2,num do 
if((x >= points[i-1][1]) and (x <= points[i][1])) then 
c = i 
break 
end 
end 
return points[c][3](x,points[c-1][1],points[c-1][2],points[c][1],points[c][2]) 
end 

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
if(StyleName=="PhysicalDamageBase") then -- script viet hoa By http://tranhba.com  bªn ngoµi phæ c«ng kÝch 
return 2*floor(5*Level*2/3) 
end; 
if(StyleName=="PhysicalMagicBase") then -- script viet hoa By http://tranhba.com  bªn trong phæ c«ng kÝch 
if(Series==0) then 
return 2*floor((Link(Level,{{1,5},{20,50},{60,300},{100,600}})/2)*2/3) 
elseif(Series==1) then 
return 0 
elseif(Series==2) then 
return 0 
elseif(Series==3) then 
return 0 
elseif(Series==4) then 
return 0 
end 
return 100 
end; 
if(StyleName=="PoisonDamageBase") then -- script viet hoa By http://tranhba.com  bªn ngoµi ®éc c«ng kÝch 
return 0 
end; 
if(StyleName=="PoisonMagicBase") then -- script viet hoa By http://tranhba.com  bªn trong ®éc c«ng kÝch 
if(Series==0) then 
return 0 
elseif(Series==1) then 
return 2*floor((Link(Level,{{1,5},{20,50},{60,300},{100,600}})/20)*2/3) 
elseif(Series==2) then 
return 0 
elseif(Series==3) then 
return 0 
elseif(Series==4) then 
return 0 
end 
return 100 
end; 
if(StyleName=="ColdDamageBase") then -- script viet hoa By http://tranhba.com  bªn ngoµi b¨ng c«ng kÝch 
return 0 
end; 
if(StyleName=="ColdMagicBase") then -- script viet hoa By http://tranhba.com  bªn trong b¨ng c«ng kÝch 
if(Series==0) then 
return 0 
elseif(Series==1) then 
return 0 
elseif(Series==2) then 
return 2*floor((Link(Level,{{1,5},{20,50},{60,300},{100,600}})/2)*2/3) 
elseif(Series==3) then 
return 0 
elseif(Series==4) then 
return 0 
end 
return 100 
end; 
if(StyleName=="FireDamageBase") then -- script viet hoa By http://tranhba.com  bªn ngoµi löa c«ng kÝch 
return 0 
end; 
if(StyleName=="FireMagicBase") then -- script viet hoa By http://tranhba.com  néi háa c«ng kÝch 
if(Series==0) then 
return 0 
elseif(Series==1) then 
return 0 
elseif(Series==2) then 
return 0 
elseif(Series==3) then 
return 2*floor((Link(Level,{{1,5},{20,50},{60,300},{100,600}})/2)*2/3) 
elseif(Series==4) then 
return 0 
end 
return 100 
end; 
if(StyleName=="LightingDamageBase") then -- script viet hoa By http://tranhba.com  bªn ngoµi l«i c«ng kÝch 
return 0 
end 
if(StyleName=="LightingMagicBase") then -- script viet hoa By http://tranhba.com  bªn trong l«i c«ng kÝch 
if(Series==0) then 
return 0 
elseif(Series==1) then 
return 0 
elseif(Series==2) then 
return 0 
elseif(Series==3) then 
return 0 
elseif(Series==4) then 
return 2*floor((Link(Level,{{1,5},{20,50},{60,300},{100,600}})/2)*2/3) 
end 
return 100 
end; 
if(StyleName=="Level1" or StyleName=="Level2" or StyleName=="Level3" or StyleName=="Level4") then 
return floor(Link(Level,{{1,1},{100,60},{101,60}})) 
end; 
if(StyleName=="AuraSkillLevel") then 
param1=GetParam(ParamStr,1); 
param2=GetParam(ParamStr,2); 
return SetAuraSkillLevel(Level,param1,param2); 
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

if(StyleName=="Skill1" or StyleName=="Skill2" ) then 
if(Series==0) then 
return 424 
elseif(Series==1) then 
return 425 
elseif(Series==2) then 
return 426 
elseif(Series==3) then 
return 427 
elseif(Series==4) then 
return 428 
end 
return 428 
end 
if(StyleName=="Skill4"or StyleName=="Skill3") then 
if(Series==0) then 
return 419 
elseif(Series==1) then 
return 420 
elseif(Series==2) then 
return 421 
elseif(Series==3) then 
return 422 
elseif(Series==4) then 
return 423 
end 
return 419 
end 
return Linear(Level,1,1); 
end; 

function GetNpcKeyData(Series,Level, StyleName, Param1, Param2, Param3) 
if (StyleName == "Exp") then 
return SetExp(Level, Param1, Param2,Param3); 
end; 
if (StyleName == "Life") then 
return SetLife(Level, Param1, Param2,Param3); 
end; 
if (StyleName == "AttackRating") then 
return SetAR(Level, Param1, Param2,Param3); 
end; 
if (StyleName == "Defense") then 
return SetDP(Level, Param1, Param2,Param3); 
end; 
if (StyleName == "MinDamage") then 
return SetMinDamage(Level, Param1, Param2,Param3); 
end; 
if (StyleName == "MaxDamage") then 
return SetMaxDamage(Level, Param1, Param2,Param3); 
end; 

return Quadratic(Level,Param1,Param2,Param3); 
end;
