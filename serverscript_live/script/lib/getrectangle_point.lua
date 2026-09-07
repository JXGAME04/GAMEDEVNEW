-- ================================================================================================
-- [HE THONG] script/lib/getrectangle_point.lua
-- Muc dich  : Tinh diem trong hinh chu nhat (port Linux, Hoa Son 01/09).
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)
-- Ham (dong): getRectanglePoint (28)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- [HOASON 01/09] chep nguyen ban Linux script\lib\getrectangle_point.lua
--大概如下图线,一个斜率45度的矩形,返回该矩形point点的table
--tbRectangle = 
--{
--	tbtoppoint={1583,2989},
--	nleftstep = 6,
--	nrightstep = 3,
--}
--该矩形的左边边长为6,右边边长为5,所以nleftstep=6,nrightstep=3
--					           ◇   --tbtoppoint{x,y} 顶点坐标
--					         ◇  ◇
--					       ◇  ◇  ◇
--					     ◇  ◇  ◇  ◇
--					   ◇  ◇  ◇  ◇  ◇
--					 ◇  ◇  ◇  ◇  ◇
--				     ◇  ◇  ◇  ◇
--		           ◇  ◇  ◇
--					       ◇  ◇
--					         ◇
--
--tbRectangle为如上格式的tb
--返回值为table,格式如下
--tb ={
--	{x1,y1},
--	{x2,y2},
--}
--
function getRectanglePoint(tbRectangle)
	local ntopx = tbRectangle.tbtoppoint[1]
	local ntopy = tbRectangle.tbtoppoint[2]
	local nleftstep = tbRectangle.nleftstep
	local nrightstep = tbRectangle.nrightstep
	local ny = ntopy
	local tbpoint = {}
	local ni = 1
	for nx=ntopx,ntopx-nleftstep,-1 do
		for nk = 0,nrightstep-1,1 do 
			tbpoint[ni]={}
			tbpoint[ni][1] = nx+nk;
			tbpoint[ni][2] = ny+nk;
			ni = ni + 1
		end
		ny = ny + 1
	end;
	return tbpoint
end;