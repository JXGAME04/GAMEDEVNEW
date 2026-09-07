-- ================================================================================================
-- [HE THONG] script/lib/awardtemplet.lua
-- Muc dich  : MAU PHAT THUONG (award templet) cho nhiem vu/hoat dong: dinh nghia loai thuong va cach trao (awardtype/*.lua).
-- Duoc nap  : Include tu 75 tep (vd activity.lua, partysupport.lua, extend.lua, item_addproperty.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : item_jx1.lua, zhenyuan_jx1.lua, exp_jx1.lua
-- Ham (dong): tbAwardTemplet:RegType (12), tbAwardTemplet:GivByRandom (16), tbAwardTemplet:Give (31), tbAwardTemplet:GiveAwardByList (58)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================


-- 文件名　：awardtemplet.lua
-- 创建者　：zhongchaolong
-- 创建时间：2008-03-20 18:55:45
IncludeLib("ITEM")
IncludeLib("ITEM")

tbAwardTemplet = {}

tbAwardTemplet.TYPE = {}

function tbAwardTemplet:RegType(szKey, pClass)
	self.TYPE[szKey] = pClass
end

function tbAwardTemplet:GivByRandom(tbItem, nAwardCount, tbLogTitle)
	if tbItem == nil then
		return 0
	end
	local rtotal = 10000000
	local rcur=random(1,rtotal);
	local rstep=0;
	for i=1,getn(tbItem) do
		rstep=rstep+floor(tbItem[i].nRate*rtotal/100);
		if(rcur <= rstep) then
			return self:Give(tbItem[i], nAwardCount, tbLogTitle)
		end
	end
end

function tbAwardTemplet:Give(tbItem, nAwardCount, tbLogTitle)
	if not tbItem then --物品表为空
		return 0
	end
	nAwardCount = nAwardCount or 1
	if type(tbItem[1]) == "table" then -- 如果是多个物品
		if tbItem[1].nRate then --按概率给某一个
			for i = 1, nAwardCount do
				self:GivByRandom(tbItem, 1, tbLogTitle)
			end
			return 1
		else --按顺序给全部
			for i = 1,  getn(tbItem) do
				self:Give(tbItem[i], nAwardCount, tbLogTitle)
			end	
			return 1;
		end
	else
		for k, v in pairs(self.TYPE) do
			if tbItem[k] then
				v:Give(tbItem, nAwardCount, tbLogTitle)
				return 1
			end
		end
	end
end

function tbAwardTemplet:GiveAwardByList(tbItem, szLogTitle, nAwardCount)
	return self:Give(tbItem, nAwardCount, {szLogTitle})
end

-- [WLLS port 20/08/2026] tu keo awardtype tbProp vao MOI state dung templet
-- (truoc day tung consumer phai tu Include; thieu la Give() im lang khong trao do)
Include("\\script\\lib\\awardtype\\item_jx1.lua")
Include("\\script\\lib\\awardtype\\zhenyuan_jx1.lua")	-- [TONGCASTLE 23/08] diem Chan Nguyen (shenmuling)
Include("\\script\\lib\\awardtype\\exp_jx1.lua")	-- [TONGWAR 23/08 phan bien F10] nExp/nExp_tl (Hoang Chan Don, Qua Dai HK, Than Moc Lenh)
