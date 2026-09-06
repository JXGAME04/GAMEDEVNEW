-- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com - 
-- script viet hoa By http://tranhba.com Create by yfeng 2004-05-20 
-- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com - 
Include("\\script\\npclevelscript\\property.lua")
-- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com - 
-- script viet hoa By http://tranhba.com ¸ù¾Ý2¸öµã£¬ÇóÏßÐÎº¯Êýf(x)=k*x+b
-- script viet hoa By http://tranhba.com y= (y2-y1)*(x-x1)/(x2-x1)+y1
-- script viet hoa By http://tranhba.com  khi x2=x1, cã x=c, nªn th¼ng t¾p lµ mét c¸i th¼ng ®øng víi x trôc ®Ých th¼ng t¾p 
-- script viet hoa By http://tranhba.com  ®©y lµ cã thÓ lÊy ®­îc y= tïy ý trÞ gi¸ 
-- script viet hoa By http://tranhba.com  v× vËy , nÕu nh­ ®· biÕt hai giê (x1,y1),(x2,y2) nh­ng cÇu xin ph¶i qu¸ nµy 2 ®iÓm ®Ých 
-- script viet hoa By http://tranhba.com  hµm sè v× # 
function Line(x,x1,y1,x2,y2) 
if(x2==x1) then 
return y2 
end 
	return (y2-y1)*(x-x1)/(x2-x1)+y1
end 

-- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com - 
-- script viet hoa By http://tranhba.com ¸ù¾Ý2¸öµã£¬Çó2´ÎÐÎº¯Êýf(x)=a*x2+c
-- script viet hoa By http://tranhba.com y= (y2-y1)*x*x/(x2*x2-x1*x1)-(y2-y1)*x1*x1/(x2*x2-x1*x1)+y1
-- script viet hoa By http://tranhba.com  khi x1 hoÆc lµ x2 < 0 ,y =0 
-- script viet hoa By http://tranhba.com  khi x2=x1, cã x=c, lµ mét c¸i th¼ng ®øng víi x trôc ®Ých th¼ng t¾p 
-- script viet hoa By http://tranhba.com  ®©y lµ cã thÓ lÊy ®­îc y= tïy ý trÞ gi¸ 
-- script viet hoa By http://tranhba.com  v× vËy , nÕu nh­ ®· biÕt hai giê (x1,y1),(x2,y2) nh­ng cÇu xin ph¶i qu¸ nµy 2 ®iÓm ®Ých 
-- script viet hoa By http://tranhba.com  hµm sè v× #extrac 
function Conic(x,x1,y1,x2,y2) 
if((x1 < 0) or (x2<0))then 
return 0 
end 
if(x2==x1) then 
return y2 
end 
	return (y2-y1)*x*x/(x2*x2-x1*x1)-(y2-y1)*x1*x1/(x2*x2-x1*x1)+y1
end 

-- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com - 
-- script viet hoa By http://tranhba.com ¸ù¾Ý2¸öµã£¬Çó-2´ÎÐÎº¯Êýf(x)=a*sqrt(x2)+c
-- script viet hoa By http://tranhba.com y=(y2-y1)*x/(sqrt(x2)-sqrt(x1))+y1-(y2-y1)/((sqrt(x2)-sqrt(x1))
-- script viet hoa By http://tranhba.com  khi x2 hoÆc lµ x1<0, y=0, 
-- script viet hoa By http://tranhba.com  khi x1=x2, cã x=c, lµ mét c¸i th¼ng ®øng víi x trôc ®Ých th¼ng t¾p 
-- script viet hoa By http://tranhba.com  ®©y lµ cã thÓ lÊy ®­îc y= tïy ý trÞ gi¸ 
-- script viet hoa By http://tranhba.com  v× vËy , nÕu nh­ ®· biÕt hai giê (x1,y1),(x2,y2) nh­ng cÇu xin ph¶i qu¸ nµy 2 ®iÓm ®Ých 
-- script viet hoa By http://tranhba.com  hµm sè v× #extrac 
function Extrac(x,x1,y1,x2,y2) 
if((x1 < 0) or (x2<0))then 
return 0 
end 
if(x2==x1) then 
return y2 
end 
	return (y2-y1)*(x-x1)/(x2-x1)+y1
end 

-- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com - 
-- script viet hoa By http://tranhba.com  miªu héi liªn tiÕp tuyÕn :Link(x,points) 
-- script viet hoa By http://tranhba.com  c¨n cø points cung cÊp mét lo¹t ®iÓm , dïng l©n cËn ®Ých hai ®iÓm miªu héi ®­êng cong 
-- script viet hoa By http://tranhba.com return y trÞ gi¸ 
-- script viet hoa By http://tranhba.com x ®­a vµo trÞ gi¸ 
-- script viet hoa By http://tranhba.com points ®iÓm tËp häp 
-- script viet hoa By http://tranhba.com  h×nh nh­ #points lµ h×nh nh­ {{x1,y1,func=xxx},{x2,y2,func=xxx},...{xn,yn,func=xxx}} ®Ých ¸nh x¹ 
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


-- script viet hoa By http://tranhba.com  tÝnh to¸n NPC thuéc tÝnh 
-- script viet hoa By http://tranhba.com  cã thÓ tÝnh to¸n ®Ých thuéc tÝnh bao gåm # 
-- script viet hoa By http://tranhba.com Exp#Life#LifeReplenish#AttackRating#Defense#MinDamage#MaxDamage# 
-- script viet hoa By http://tranhba.com Level1#Level2#Level3#Level4 . 



-- script viet hoa By http://tranhba.com  lÊy ®­îc dïng | ph©n chia ®Ých tham sæ ®Ých hµm sè . 

function GetParam(strParam, index) 
nLastBegin = 1 
for i=1, index - 1 do 
nBegin = strfind(strParam, "|", nLastBegin) 
nLastBegin = nBegin + 1
end; 
num = 1 

strnum = strsub(strParam, nLastBegin) 
nEnd = strfind(strnum, "|") 
if nEnd == nil then 
return strnum 
end 
str1 = strsub(strnum,1,nEnd -1); 
return str1 
end; 



-- script viet hoa By http://tranhba.com  nhËp khÈu chñ hµm sè , thø nhÊt tham sæ v× NPC cÊp bËc , chê hai tham sæ v× sè liÖu lo¹i h×nh , ng­êi thø ba tham sæ lµ truyÒn vµo ®Ých npcs trung ®Ých tù phï chuçi . 
-- script viet hoa By http://tranhba.com  bÞ mét npc ®¬n ®éc dïng ch©n vèn cã thÓ kh«ng c©n nh¾c ng­êi thø ba tham sæ ®Ých ®­a vµo . 
function GetNpcLevelData(Series, Level, StyleName, ParamStr) 
if(StyleName=="Level1" or StyleName=="Level2" or StyleName=="Level3" or StyleName=="Level4") then 
return floor(Link(Level,{{1,1},{100,60}})) 
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

if( StyleName=="Skill3" or StyleName=="Skill4") then 
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
return 424 
end 

if(StyleName=="PhysicalDamageBase") then -- script viet hoa By http://tranhba.com  bªn ngoµi phæ c«ng kÝch 
return floor(5*Level*2/3)*4 
end; 
if(StyleName=="PhysicalMagicBase") then -- script viet hoa By http://tranhba.com  bªn trong phæ c«ng kÝch 
if(Series==0) then 
return floor((Link(Level,{{1,5},{20,50},{60,300},{100,600}})/2)*2/3)*4 
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
return floor((Link(Level,{{1,5},{20,50},{60,300},{100,600}})/20)*2/3)*4 
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
return floor((Link(Level,{{1,5},{20,50},{60,300},{100,600}})/2)*2/3)*4 
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
return floor((Link(Level,{{1,5},{20,50},{60,300},{100,600}})/2)*2/3)*4 
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
return floor((Link(Level,{{1,5},{20,50},{60,300},{100,600}})/2)*2/3)*4 
end 
return 100 
end; 
if(StyleName=="PhysicsResist") then -- script viet hoa By http://tranhba.com  phæ kh¸ng 
if(Level <= 80) then 
return 50 
elseif(Level > 80 and Level <= 90) then 
return 60 
elseif(Level > 90) then 
return 75 
end 
return 75 
end 
if(StyleName=="ColdResist") then -- script viet hoa By http://tranhba.com  b¨ng phßng 
if(Level <= 80) then 
return 50 
elseif(Level > 80 and Level <= 90) then 
return 60 
elseif(Level > 90) then 
return 75 
end 
return 75 
end 
if(StyleName=="FireResist") then -- script viet hoa By http://tranhba.com  löa phßng 
if(Level <= 80) then 
return 50 
elseif(Level > 80 and Level <= 90) then 
return 60 
elseif(Level > 90) then 
return 75 
end 
return 75 
end 
if(StyleName=="LightResist") then -- script viet hoa By http://tranhba.com  l«i phßng 
if(Level <= 80) then 
return 50 
elseif(Level > 80 and Level <= 90) then 
return 60 
elseif(Level > 90) then 
return 75 
end 
return 75 
end 
if(StyleName=="PoisonResist") then -- script viet hoa By http://tranhba.com  ®éc phßng 
if(Level <= 80) then 
return 50 
elseif(Level > 80 and Level <= 90) then 
return 60 
elseif(Level > 90) then 
return 75 
end 
return 75 
end 

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

Param1 = GetParam(ParamStr,1); 
Param2 = GetParam(ParamStr,2); 

result = GetData(Level, Param1, Param2); 
return result; 
end; 

-- script viet hoa By http://tranhba.com  mÊu chèt sè liÖu ®Ých tÝnh to¸n hµm sè 
function GetNpcKeyData(Series, Level, StyleName, Param1, Param2, Param3) 
if (StyleName == "Exp") then 
return GetExp(Level, Param1, Param2); 
end; 

if (StyleName == "Life") then 
return GetLife(Level, Param1, Param2); 
end; 

if (StyleName == "AttackRating") then 
if(Level < 30) then 
return GetAttackRating(Level, Param1, Param2) 
end 
return floor(GetAttackRating(Level, Param1, Param2)*1.5); 
end; 

if (StyleName == "Defense") then 
if(Level < 30) then 
return GetDefense(Level, Param1, Param2) 
end 
return floor(GetDefense(Level, Param1, Param2)*2); 
end; 

if (StyleName == "MinDamage") then 
return 1.5*GetMinDamage(Level, Param1, Param2); 
end; 

if (StyleName == "MaxDamage") then 
return 1.5*GetMaxDamage(Level, Param1, Param2); 
end; 

result = Param1 * Level * Level + Param2 * Level + Param3;
return result; 
end; 

-- script viet hoa By http://tranhba.com  th«ng dông tuyÕn tÝnh tÝnh to¸n c«ng thøc . 
function GetData(Level, Param1, Param2) 
result = Param2 * Level + Param1;
return floor(result); 
end; 

-- script viet hoa By http://tranhba.com ÒÔÏÂµÄDataPara1±íÊ¾ÏßÐÔº¯Êýy=kx+bÖÐµÄb£¬DataPara2±íÊ¾k¡£
-- script viet hoa By http://tranhba.com  kinh nghiÖm trÞ gi¸ tÝnh to¸n c«ng thøc 
function GetExp(Level, Param1, Param2) 
return floor(Param1); 
end; 

-- script viet hoa By http://tranhba.com  sinh m¹ng trÞ gi¸ tÝnh to¸n hµm sè 
function GetLife(Level, Param1, Param2) 
return floor(Param1); 
end; 

-- script viet hoa By http://tranhba.com  mÖnh trung tû sè tÝnh to¸n hµm sè 
function GetAttackRating(Level, Param1, Param2) 

if (Level <= 10) then 
DataPara1=10 
DataPara2=3 
return GetData(Level, DataPara1, DataPara2) 
end; 

if (Level <= 20) then 
DataPara1=50 
DataPara2=4 
return GetData(Level-10, DataPara1, DataPara2) 
end; 

if (Level <= 30) then 
DataPara1=100 
DataPara2=5 
return GetData(Level-20, DataPara1, DataPara2) 
end; 

if (Level <= 40) then 
DataPara1=250 
DataPara2=5 
return GetData(Level-30, DataPara1, DataPara2) 
end; 

if (Level <= 50) then 
DataPara1=400 
DataPara2=5 
return GetData(Level-40, DataPara1, DataPara2) 
end; 

if (Level <= 60) then 
DataPara1=500 
DataPara2=5 
return GetData(Level-50, DataPara1, DataPara2) 
end; 

if (Level <= 70) then 
DataPara1=600 
DataPara2=5 
return GetData(Level-60, DataPara1, DataPara2) 
end; 

if (Level <= 80) then 
DataPara1=700 
DataPara2=5 
return GetData(Level-70, DataPara1, DataPara2) 
end; 

if (Level <= 90) then 
DataPara1=800 
DataPara2=5 
return GetData(Level-80, DataPara1, DataPara2) 
end; 

if (Level <= 100) then 
DataPara1=900 
DataPara2=5 
return GetData(Level-90, DataPara1, DataPara2) 
end; 

result = Param2 * Level + Param1;
return floor(result); 
end; 



-- script viet hoa By http://tranhba.com  nÐ tr¸nh tû sè tÝnh to¸n hµm sè . 
function GetDefense(Level, Param1, Param2) 

if (Level <= 10) then 
DataPara1=10 
DataPara2=0 
return GetData(Level, DataPara1, DataPara2) 
end; 

if (Level <= 20) then 
DataPara1=50 
DataPara2=3 
return GetData(Level-10, DataPara1, DataPara2) 
end; 

if (Level <= 30) then 
DataPara1=100 
DataPara2=5 
return GetData(Level-20, DataPara1, DataPara2) 
end; 

if (Level <= 40) then 
DataPara1=150 
DataPara2=5 
return GetData(Level-30, DataPara1, DataPara2) 
end; 

if (Level <= 50) then 
DataPara1=200 
DataPara2=5 
return GetData(Level-40, DataPara1, DataPara2) 
end; 

if (Level <= 60) then 
DataPara1=300 
DataPara2=5 
return GetData(Level-50, DataPara1, DataPara2) 
end; 

if (Level <= 70) then 
DataPara1=400 
DataPara2=5 
return GetData(Level-60, DataPara1, DataPara2) 
end; 

if (Level <= 80) then 
DataPara1=500 
DataPara2=5 
return GetData(Level-70, DataPara1, DataPara2) 
end; 

if (Level <= 90) then 
DataPara1=600 
DataPara2=5 
return GetData(Level-80, DataPara1, DataPara2) 
end; 

if (Level <= 100) then 
DataPara1=800 
DataPara2=10 
return GetData(Level-90, DataPara1, DataPara2) 
end; 

result = Param2 * Level + Param1;
return floor(result); 
end; 


-- script viet hoa By http://tranhba.com  nhá nhÊt tæn th­¬ng tÝnh to¸n hµm sè 
function GetMinDamage(Level, Param1, Param2) 

if (Level <= 10) then 
DataPara1=3 
DataPara2=0.3 
return floor(GetData(Level, DataPara1, DataPara2)) 
end; 

if (Level <= 20) then 
DataPara1=8 
DataPara2=0.3 
return floor(GetData(Level-10, DataPara1, DataPara2)) 
end; 

if (Level <= 30) then 
DataPara1=13 
DataPara2=0.4 
return floor(GetData(Level-20, DataPara1, DataPara2)) 
end; 

if (Level <= 40) then 
DataPara1=20 
DataPara2=0.5 
return floor(GetData(Level-30, DataPara1, DataPara2)) 
end; 

if (Level <= 50) then 
DataPara1=29 
DataPara2=0.5 
return floor(GetData(Level-40, DataPara1, DataPara2)) 
end; 

if (Level <= 60) then 
DataPara1=39 
DataPara2=0.6 
return floor(GetData(Level-50, DataPara1, DataPara2)) 
end; 

if (Level <= 70) then 
DataPara1=50 
DataPara2=0.7 
return floor(GetData(Level-60, DataPara1, DataPara2)) 
end; 

if (Level <= 80) then 
DataPara1=65 
DataPara2=0.8 
return floor(GetData(Level-70, DataPara1, DataPara2)) 
end; 

if (Level <= 90) then 
DataPara1=85 
DataPara2=0.9 
return floor(GetData(Level-80, DataPara1, DataPara2)) 
end; 

if (Level <= 100) then 
DataPara1=112 
DataPara2=1 
return floor(GetData(Level-90, DataPara1, DataPara2)) 
end; 

result = Param2 * Level + Param1;
return floor(result); 
end; 


-- script viet hoa By http://tranhba.com  lín nhÊt tæn th­¬ng tÝnh to¸n hµm sè 
function GetMaxDamage(Level, Param1, Param2) 

if (Level <= 10) then 
DataPara1=3 
DataPara2=0.3 
return floor(GetData(Level, DataPara1, DataPara2)) 
end; 

if (Level <= 20) then 
DataPara1=8 
DataPara2=0.3 
return floor(GetData(Level-10, DataPara1, DataPara2)) 
end; 

if (Level <= 30) then 
DataPara1=13 
DataPara2=0.4 
return floor(GetData(Level-20, DataPara1, DataPara2)) 
end; 

if (Level <= 40) then 
DataPara1=20 
DataPara2=0.5 
return floor(GetData(Level-30, DataPara1, DataPara2)) 
end; 

if (Level <= 50) then 
DataPara1=29 
DataPara2=0.5 
return floor(GetData(Level-40, DataPara1, DataPara2)) 
end; 

if (Level <= 60) then 
DataPara1=39 
DataPara2=0.6 
return floor(GetData(Level-50, DataPara1, DataPara2)) 
end; 

if (Level <= 70) then 
DataPara1=50 
DataPara2=0.7 
return floor(GetData(Level-60, DataPara1, DataPara2)) 
end; 

if (Level <= 80) then 
DataPara1=65 
DataPara2=0.8 
return floor(GetData(Level-70, DataPara1, DataPara2)) 
end; 

if (Level <= 90) then 
DataPara1=85 
DataPara2=0.9 
return floor(GetData(Level-80, DataPara1, DataPara2)) 
end; 

if (Level <= 100) then 
DataPara1=112 
DataPara2=1 
return floor(GetData(Level-90, DataPara1, DataPara2)) 
end; 

result = Param2 * Level + Param1;
return floor(result); 
end; 
