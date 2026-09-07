-- ================================================================================================
-- [HE THONG] script/lib/lib_server.lua
-- Muc dich  : CAU HINH MAY CHU (CFGNEN 30/08): nap cauhinh/ch_*.lua; sua cau hinh -> sua cauhinh/, KHONG sua o day.
-- Duoc nap  : Include tu 46 tep (vd eventlib2010.lua, npcevent.lua, eventlib.lua, tuongquan.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : ch_lib.lua, ch_chung.lua
-- Ham (dong): GLB_CFG (6)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- [CFGNEN 30/08] hai tep duoi day la LA (khong Include gi).
Include("\\script\\cauhinh\\ch_lib.lua")
Include("\\script\\cauhinh\\ch_chung.lua")
-- [CFGNEN 30/08] Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu)
-- khi bo cau hinh chua nap, nen kem nhat cung khong the doi hanh vi.
function GLB_CFG(szKhoa, macdinh)
	if (G_CFG ~= nil) then
		return G_CFG(szKhoa, macdinh)
	end
	return macdinh
end

-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Chuc nang: Lib toan server
SERVER_OPEN = GLB_CFG("GLB_GIO_MO_SERVER", 2506251900) -- yy/mm/dd - hh//mm [21-nam] [03-thang] [14-ngay] [19gio] [00phut]
SERVER_TEST						= GLB_CFG("GLB_CHE_DO_TEST", 1)
MONEY_RATE 					= GLB_CFG("GLB_TILE_TIEN", 1) -- sö dông trong lib c©u hái, sù kiÖn hoa ®¨ng
EXP_RATE 							= GLB_CFG("GLB_TILE_EXP", 20)
DAMAGE_UPPER_BOSS 	= GLB_CFG("GLB_SATTHUONG_BOSS_HK", 1)	--t¨ng thªm søc m¹nh kh¸ng ... cho boss tiÓu
DAMAGE_UP_TTPLD 		= GLB_CFG("GLB_SATTHUONG_QUAI_PLD", 1)	--t¨ng thªm søc m¹nh cho qu¸i ë thuyÒn PLD
DAMAGE_UPPER_TTDL 	= GLB_CFG("GLB_SATTHUONG_BOSS_TTDL", 1)	--t¨ng thªm søc m¹nh kh¸ng ... cho boss thuû tÆc ®Çu lÜnh ë PLD
-- [PB 30/08] bo STRONGBOSS_ST: chi duoc GAN, khong noi nao DOC (he tieu thu da go)
STRONGBOSS_VA				= GLB_CFG("GLB_MANH_BOSS_VUOTAI", 1)	--t¨ng thªm søc m¹nh kh¸ng ... cho boss vµ npc v­ît ¶i
STRONGBOSS_NSTK			= GLB_CFG("GLB_MANH_NGUYENSOAI_TK", 1) 	--t¨ng thªm søc m¹nh kh¸ng ... cho boss nguyªn so¸i trong tèng kim
MAX_MAGIC_LEVEL 		= GLB_CFG("GLB_MAX_DONG_THUOCTINH", 10) --t¨ng lµm 4 giai ®o¹n 6->8->9->10

