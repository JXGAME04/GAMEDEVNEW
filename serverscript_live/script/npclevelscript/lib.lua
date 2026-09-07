-- ================================================================================================
-- [HE THONG] script/npclevelscript/lib.lua
-- Muc dich  : Ham chia tham so cho npclevelscript.
-- Duoc nap  : Include tu 1 tep (vd property.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): GetParam (2), Linear (23), Quadratic (28)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- script viet hoa By http://tranhba.com  lÊy ®­îc dïng | ph©n chia ®Ých tham sæ ®Ých hµm sè . 
function GetParam(strParam, index) 
nLastBegin = 1 
for i=1, index - 1 do 
nBegin = strfind(strParam, "|", nLastBegin) 
-- script viet hoa By http://tranhba.com if nBegin == nil then 
-- script viet hoa By http://tranhba.com  print(strParam) 
-- script viet hoa By http://tranhba.com  return "0" 
-- script viet hoa By http://tranhba.com end 
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

-- script viet hoa By http://tranhba.com ÏßÐÔº¯Êý£¬È¡Õûy=kx+c
function Linear(x,k,c) 
	return floor(k*x+c);
end; 

-- script viet hoa By http://tranhba.com ¶þ´Îº¯Êý£¬È¡Õûy=ax^2+bx+c
function Quadratic(x,a,b,c) 
	return floor(a*x*x+b*x+c);
end; 
