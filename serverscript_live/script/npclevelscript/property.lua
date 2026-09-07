-- ================================================================================================
-- [HE THONG] script/npclevelscript/property.lua
-- Muc dich  : Ham tinh thuoc tinh NPC theo cap (23 ham).
-- Duoc nap  : Include tu 18 tep (vd animal_userdef.lua, bairenleitai.lua, dragonboat_boss1.lua, dragonboat_boss2.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : lib.lua
-- Ham (dong): SetLife (4), SetExp (9), SetAR (14), SetDP (19), SetMinDamage (24), SetMaxDamage (29), SetFireResist (34), SetPhysicsResist (39), SetColdResist (44), SetLightResist (49), SetPoisonResist (54), SetPhysicalDamageBase (59), SetPoisonDamageBase (64), SetColdDamageBase (69), SetFireDamageBase (74), SetLightingDamageBase (79), SetSkillLevel1 (84), SetSkillLevel2 (89), SetSkillLevel3 (94), SetSkillLevel4 (99), SetAuraSkillLevel (104), SetPasstSkillLevel (109), SetLifeReplenish (114)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
Include("\\script\\npclevelscript\\lib.lua");

-- script viet hoa By http://tranhba.com  thiÕt trİ sinh m¹ng 
function SetLife(level,param1,param2,param3) 
return Quadratic(level,param1,param2,param3); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ kinh nghiÖm trŞ gi¸ 
function SetExp(level,param1,param2,param3) 
return Quadratic(level,param1,param2,param3); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ mÖnh trung 
function SetAR(level,param1,param2,param3) 
return Quadratic(level,param1,param2,param3); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ nĞ tr¸nh 
function SetDP(level,param1,param2,param3) 
return Quadratic(level,param1,param2,param3); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ trô cét phæ th­¬ng h¹ h¹n 
function SetMinDamage(level,param1,param2,param3) 
return Quadratic(level,param1,param2,param3); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ trô cét phæ th­¬ng th­îng h¹n 
function SetMaxDamage(level,param1,param2,param3) 
return Quadratic(level,param1,param2,param3); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ löa phßng 
function SetFireResist(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ phæ phßng 
function SetPhysicsResist(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ b¨ng phßng 
function SetColdResist(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ l«i phßng 
function SetLightResist(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ ®éc phßng 
function SetPoisonResist(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ phæ c«ng c¬ ®Õm 
function SetPhysicalDamageBase(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ ®éc c«ng c¬ ®Õm 
function SetPoisonDamageBase(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ b¨ng c«ng c¬ ®Õm 
function SetColdDamageBase(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ löa c«ng c¬ ®Õm 
function SetFireDamageBase(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ l«i c«ng c¬ ®Õm 
function SetLightingDamageBase(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ kü n¨ng 1 cÊp bËc 
function SetSkillLevel1(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ kü n¨ng 2 cÊp bËc 
function SetSkillLevel2(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ kü n¨ng 3 cÊp bËc 
function SetSkillLevel3(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ kü n¨ng 4 cÊp bËc 
function SetSkillLevel4(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ hµo quang kü n¨ng cÊp bËc 
function SetAuraSkillLevel(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ bŞ ®éng kü n¨ng cÊp bËc 
function SetPasstSkillLevel(level,param1,param2) 
return Linear(level,param1,param2); 
end; 

-- script viet hoa By http://tranhba.com  thiÕt trİ sinh m¹ng håi phôc trŞ sè 
function SetLifeReplenish(level,param1,param2) 
return Linear(level,param1,param2); 
end; 
