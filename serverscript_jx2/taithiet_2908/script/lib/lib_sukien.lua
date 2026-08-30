-- Author: 
-- Date: 28/11/2016
-- Chuc nang: Rot them vat pham khi co su kien
Include("\\script\\lib\\lib_vatpham.lua")
-- [CFGDROP 29/08] hai tep duoi day deu la LA (khong Include gi) nen
-- khong tao vong Include.
Include("\\script\\cauhinh\\ch_lib.lua")
Include("\\script\\cauhinh\\ch_drop.lua")
-- [CFGDROP 29/08] Bo doc cau hinh cho tep nay.
-- Vi sao co lop nay ma khong goi thang G_CFG: lib_sukien.lua duoc Include vao
-- rat nhieu state khac nhau; state nao chua nap bo cau hinh thi G_CFG la nil.
-- Ham nay tra ve MAC DINH (= dung so cu) trong truong hop do, nen kem nhat
-- cung khong the doi hanh vi.
function SKD_CFG(szKhoa, macdinh)
	if (G_CFG ~= nil) then
		return G_CFG(szKhoa, macdinh)
	end
	return macdinh
end


EVENT_ACTIVE 	= 1
----------------------------
-- 
----------------------------
RANDMAP 	= SKD_CFG("SKD_RANDMAP", 100)	-- mau so chung; cang cao ti le cang thap
RANDPLD 	= SKD_CFG("SKD_RANDPLD", 100)
RANDOTHER	= SKD_CFG("SKD_RANDOTHER", 10)
RANDTK	 	= SKD_CFG("SKD_RANDTK", 100)
RANDVA	 	= SKD_CFG("SKD_RANDVA", 100)
DROPMDTB	= SKD_CFG("SKD_DROPMDTB", 95)


----------------------------
-- 
----------------------------
function dropeventmap(nNpcIndex)
local nRand = random(1, RANDMAP);
	if nRand > SKD_CFG("SKD_MAP_NGUONG", 80) then
	 DropItem(nNpcIndex,6,1,SKD_CFG("SKD_MAP_ITEM", 4854),0,0,0,0);	-- Event
	end
end

function droptrangbihiepcotnhutinh(nNpcIndex)
local nRand = random(1, RANDMAP);
	if nRand > SKD_CFG("SKD_HCNT_NGUONG", 50) then
	DropItemPUBG(nNpcIndex, 0, random(SKD_CFG("SKD_HCNT_MA_MIN", 185), SKD_CFG("SKD_HCNT_MA_MAX", 192)), 0, 10, 0, 0, 2)
	-- AddTimeItem(itemIdx,60*60*24*7)
	end
end

function dropeventPUBG(nNpcIndex)
local nRand = random(SKD_CFG("SKD_PUBG_MA_MIN", 4851), SKD_CFG("SKD_PUBG_MA_MAX", 4853));
DropItem(nNpcIndex,6,1,nRand,0,0,0,0);	-- Event
end

function droptrangbihkmp(nNpcIndex)
local itemIdx =  DropItemPUBG(nNpcIndex, 0, random(SKD_CFG("SKD_HKMP_MA_MIN", 0), SKD_CFG("SKD_HKMP_MA_MAX", 139)), 0, 10, 0, 0, 2)
AddTimeItem(itemIdx,60*60*24*SKD_CFG("SKD_HKMP_HAN_NGAY", 7))
end


function dropeventHMD(nNpcIndex)
local nRand = random(1, RANDMAP);
	if nRand > SKD_CFG("SKD_HMD_NGUONG", 1) then
	for i=1,SKD_CFG("SKD_HMD_SOLUONG", 10) do	
	 DropItem(nNpcIndex,6,1,SKD_CFG("SKD_HMD_ITEM", 4844),0,0,0,0);	-- Event
	 end
	end
end

function dropmanhhkmp(nNpcIndex)
local naddtungphaiList = {253, 258, 263, 239, 243, 248, 313, 308, 318, 298, 303, 268, 276, 283, 288, 331, 333, 338, 348, 353, 358, 363, 368, 2433}
local randIndex = random(1, getn(naddtungphaiList))  
local itemid = naddtungphaiList[randIndex]
local nRand = random(1, RANDMAP);
	if nRand < SKD_CFG("SKD_MANHHKMP_NGUONG", 80) then
	 DropItem(nNpcIndex,6,1,itemid,0,0,0,0);	-- Event
	end
end
----------------------------
-- 
----------------------------
function dropeventPLD(nNpcIndex)

end

function dropeventboss(nNpcIndex)
local nRand = random(1, RANDOTHER);

		if nRand > SKD_CFG("SKD_BOSS_NGUONG", 1) then
			DropItem(nNpcIndex,4,random(SKD_CFG("SKD_BOSS_MA_MIN", 753), SKD_CFG("SKD_BOSS_MA_MAX", 770)),0,0,0,0,0);	-- 
		elseif nRand == 9 then
			DropItem(nNpcIndex,4,random(747,752),0,0,0,0,0);	-- day chuyen ab 
		end
	-- if nRand > 1 then
	-- DropItem(nNpcIndex,4,random(903,942),0,0,0,0,0);	-- 
	-- elseif nRand == 9 then
	-- DropItem(nNpcIndex,4,random(771,776),0,0,0,0,0);	-- ngäc béi hc
	-- elseif nRand == 10 then
	-- DropItem(nNpcIndex,4,random(783,788),0,0,0,0,0);	-- m¶nh mò §q
	-- end
end
----------------------------
-- 
----------------------------
function dropother(nNpcIndex)
	--local nRand = random(1, RANDOTHER);
	--if nRand > 45 then
	--DropItem(nNpcIndex,4,124,0,0,0,0,0);	-- khieu chien lenh
	--end
end
function dropntiendong(nNpcIndex)
local nRand = random(1, RANDOTHER);
local nbRand = random(SKD_CFG("SKD_TIENDONG_SL_MIN", 1), SKD_CFG("SKD_TIENDONG_SL_MAX", 3));
	if nRand > SKD_CFG("SKD_TIENDONG_NGUONG", 1) then
	for i=1,nbRand do	
	DropItem(nNpcIndex,6,1,SKD_CFG("SKD_TIENDONG_ITEM", 4835),0,0,0,0);	-- tien dong
		end
	end
end
----------------------------
-- 
----------------------------
function dropeventTK(nNpcIndex)

end

----------------------------
-- 
----------------------------
function dropeventVA(nNpcIndex)

end

function dropnvdt(nNpcIndex)
local nRand = random(1, DROPMDTB);
	if nRand < 10 then
	DropItem(nNpcIndex,6,1,205,0,0,0,0);	-- Event
	end
end
function dropnvdt01(nNpcIndex)
local nRand = random(1, DROPMDTB);
	if nRand < 90 then
	DropItem(nNpcIndex,6,1,212,0,0,0,0);	-- Event
	end
end