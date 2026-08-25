-- ============================================================================
-- HD3_ADMIN.LUA - Menu TEST 3 hoat dong ban Linux tren Lenh Bai Admin. (v2)
-- Include tu lenhbaiadmin.lua (main() dofile lai) => SUA KHONG CAN RESTART.
-- Cau hinh: script\header\cauhinh_hoatdong.lua (khoi [6]).
-- CAM dau "/" trong nhan menu (ScriptFuns.cpp:717 cat o dau / dau tien).
-- ============================================================================
Include("\\script\\tinhnang\\3hoatdong\\hd3_driver.lua")

function HD3_AdminMenu()
	SayEx({"<color=yellow>Ho¹t ®éng ban Linux (test)<color>: chän môc",
	"1. S¨n Boss S¸t Thñ/HD3_ADM_ST",
	"2. Phong L¨ng §é/HD3_ADM_PLD",
	"3. V­ît ¶i/HD3_ADM_VA",
	"4. N¹p l¹i CONFIG/HD3_ADM_Reload",
	"5. Xem c¸c kho¸ cÊu h×nh HD3 ®ang hiÖu lùc/HD3_ADM_ShowCfg",
	"6. N¹p l¹i toµn bé script (sau khi gâ ?gm RLAS)/HD3_ADM_ReloadAll",
	"KÕt thóc ®èi tho¹i./no"})
end

function HD3_ADM_ST()
	SayEx({"<color=yellow>S¨n Boss S¸t Thñ<color> - trÇn " .. HD_CFG("HD3_ST_MAX_NGAY", 8) .. " lÇn mét ngµy; chØ nhãm cÊp 90 (chØ sè 141-160) ph¸t th­ëng",
	"Sinh l¹i NPC 769 vµ 160 boss - ChØ khi boot lçi, gäi lÆp sÏ nh©n b¶n NPC/HD3_ADM_ST_Boot",
	"DÞch chuyÓn: NPC NhiÕp ThÝ TrÇn (Ph­îng T­êng)/HD3_ADM_ST_Tele",
	"NhËn 5 S¸t Thñ lÖnh cÊp 90 (test gép Gi¶n)/HD3_ADM_ST_Lenh",
	"NhËn 2 S¸t Thñ Gi¶n cÊp 90 (®ñ vÐ b¸o danh V­ît ¶i cao cÊp)/HD3_ADM_ST_Gian2",
	"§Æt boss ®ang nhËn (task 1082) theo sè nhËp 1-160/HD3_ADM_ST_SetBoss",
	"Xem biÕn nhiÖm vô (1082, 1192, 1193, 1217)/HD3_ADM_ST_Task",
	"Reset sè lÇn giÕt trong ngµy/HD3_ADM_ST_Reset",
	"Quay l¹i/HD3_AdminMenu"})
end
function HD3_ADM_ST_Boot() HD3_DriverInit() Msg2Player("§· gäi HD3_DriverInit (tù dän NPC cò cña hÖ råi sinh l¹i - gäi lÆp an toµn).") end
function HD3_ADM_ST_Tele() NewWorld(1, 1506, 3198) end
function HD3_ADM_ST_Lenh()
	for i = 1, 5 do AddItem(6, 1, 398, 90, mod(i, 5), 0, 0) end
	Msg2Player("§· nhËn 5 S¸t Thñ lÖnh cÊp 90 (®ñ 5 hÖ). GÆp NhiÕp ThÝ TrÇn ®Ó gép thµnh S¸t Thñ Gi¶n.")
end
function HD3_ADM_ST_Gian2()
	AddItem(6, 1, 399, 90, 0, 0, 0)
	AddItem(6, 1, 399, 90, 1, 0, 0)
	Msg2Player("§· nhËn 2 S¸t Thñ Gi¶n cÊp 90 - ®ñ vÐ ®éi tr­ëng b¸o danh V­ît ¶i cao cÊp.")
end
function HD3_ADM_ST_SetBoss()
	OpenGetNumber("NhËp chØ sè boss (1-160, nhãm 90 = 141-160)", "HD3_ADM_ST_SetBossCB")
end
function HD3_ADM_ST_SetBossCB()
	local n = GetNumberFromUI()
	if (n == nil or n < 0 or n > 160) then Msg2Player("Sè kh«ng hîp lÖ (0 = xo¸ nhiÖm vô).") return end
	SetTask(1082, n)
	Msg2Player(format("§· ®Æt task 1082 = %d. GiÕt ®óng boss mang chØ sè nµy ®Ó hoµn thµnh.", n))
end
function HD3_ADM_ST_Task()
	Msg2Player(format("1082=%d (chØ sè) - 1192=%d (ngµy) - 1193=%d (sè lÇn) - 1217=%d (tÝch lòy)", GetTask(1082), GetTask(1192), GetTask(1193), GetTask(1217)))
	HD3_ADM_ST()
end
function HD3_ADM_ST_Reset() SetTask(1193, 0) Msg2Player("§· reset sè lÇn giÕt boss trong ngµy.") end

function HD3_ADM_PLD()
	SayEx({"<color=yellow>Phong L¨ng §é<color> - më theo giê trong HD3_PLD_GIO; giê tèn phÝ dïng LÖnh Bµi Thñy TÆc",
	"Khai cuéc NGAY (më ®¨ng ký thuyÒn)/HD3_ADM_PLD_Now",
	"Xem tr¹ng th¸i 3 thuyÒn (mission 15)/HD3_ADM_PLD_TrangThai",
	"DÞch chuyÓn: ThuyÒn phu bê Nam (map 336)/HD3_ADM_PLD_Tele336",
	"Vµo map thuyÒn 337 (xem nót tho¸t bªn d­íi!)/HD3_ADM_PLD_Tele337",
	"Tho¸t khái map thuyÒn vÒ Ph­îng T­êng/HD3_ADM_PLD_Thoat",
	"NhËn LÖnh bµi Phong L¨ng §é (4,489)/HD3_ADM_PLD_LB",
	"NhËn LÖnh Bµi Thñy TÆc (6,1,3363)/HD3_ADM_PLD_LBTT",
	"Quay l¹i/HD3_AdminMenu"})
end
function HD3_ADM_PLD_Now() HD3_Adm_PLD_Now() Msg2Player("§· Ðp khai cuéc Phong L¨ng §é (fenglingdu_main).") end
function HD3_ADM_PLD_TrangThai()
	local tbMap = {337, 338, 339}
	local nOld = SubWorld
	for i = 1, 3 do
		local idx = SubWorldID2Idx(tbMap[i])
		if (idx >= 0) then
			SubWorld = idx
			Msg2Player(format("ThuyÒn %d (map %d): mission=%d, ng­êi=%d, phót=%d", i, tbMap[i], IsMission(15), GetMSPlayerCount(15, 1), GetMissionV(2)))
		end
	end
	SubWorld = nOld
	HD3_ADM_PLD()
end
function HD3_ADM_PLD_Tele336() NewWorld(336, 1147, 3018) end
function HD3_ADM_PLD_Tele337() NewWorld(337, 1646, 3233) end
function HD3_ADM_PLD_Thoat() NewWorld(1, 1506, 3198) Msg2Player("§· tho¸t vÒ Ph­îng T­êng.") end
function HD3_ADM_PLD_LB() AddItem(4, 489, 0, 0, 0, 0, 0) Msg2Player("§· nhËn LÖnh bµi Phong L¨ng §é.") end
function HD3_ADM_PLD_LBTT() AddItem(6, 1, 3363, 0, 0, 0, 0) Msg2Player("§· nhËn LÖnh Bµi Thñy TÆc (6,1,3363).") end

function HD3_ADM_VA()
	SayEx({"<color=yellow>V­ît ¶i<color> - b¸o danh theo giê HD3_VA_GIO; ®éi tr­ëng cÇn 2 S¸t Thñ Gi¶n + tæ ®éi 4 ng­êi",
	"B¸o danh NGAY (më mission c¸c map ¶i)/HD3_ADM_VA_Now",
	"Xem tr¹ng th¸i ¶i (map 464 s¬ cÊp, 480 cao cÊp)/HD3_ADM_VA_TrangThai",
	"Trao b¶ng xÕp h¹ng ngµy (Ladder 10235)/HD3_ADM_VA_Rank",
	"§Æt thµnh tÝch h«m qua (task 2636-2639) ®Ó test nhËn th­ëng/HD3_ADM_VA_SetRank",
	"NhËn 2 S¸t Thñ Gi¶n cÊp 90 (vÐ vµo)/HD3_ADM_ST_Gian2",
	"DÞch chuyÓn tíi NhiÕp ThÝ TrÇn (Ph­îng T­êng)/HD3_ADM_ST_Tele",
	"Quay l¹i/HD3_AdminMenu"})
end
function HD3_ADM_VA_Now() HD3_Adm_VA_Now() Msg2Player("§· Ðp b¸o danh V­ît ¶i (trigger OnTrigger).") end
function HD3_ADM_VA_TrangThai()
	local tbMap = {464, 480, 957}
	local nOld = SubWorld
	for i = 1, 3 do
		local idx = SubWorldID2Idx(tbMap[i])
		if (idx >= 0) then
			SubWorld = idx
			Msg2Player(format("Map %d: mission22=%d, ng­êi=%d, tr¹ng th¸i=%d", tbMap[i], IsMission(22), GetMSPlayerCount(22, 1), GetMissionV(4)))
		else
			Msg2Player(format("Map %d: ch­a n¹p (WorldSet)", tbMap[i]))
		end
	end
	SubWorld = nOld
	HD3_ADM_VA()
end
function HD3_ADM_VA_Rank() HD3_Adm_VA_Rank() Msg2Player("§· ch¹y b¶ng xÕp h¹ng ngµy (Ladder 10235).") end
function HD3_ADM_VA_SetRank()
	local nHomQua = tonumber(GetLocalDate("%y%m%d")) - 1
	SetTask(2636, nHomQua)
	SetTask(2637, 600)
	SetTask(2638, 0)
	SetTask(2639, 0)
	Msg2Player(format("§· ®Æt 2636=%d (h«m qua), 2637=600 (10 phót). GÆp NhiÕp ThÝ TrÇn bÊm nhËn th­ëng xÕp h¹ng.", nHomQua))
end

-- [3HD 25/08 C42] Nap lai TOAN BO script cho nhanh khi test.
-- Lenh GM co san:  ?gm RLAS   (= ReLoadAllScript: xoa sach cay script roi nap lai)
-- NHUNG g_IniScriptEngine CHI quet \script va \scriptjx2\tong_vn, KHONG quet
-- \settings => 2 tep trigger cua Vuot Ai bi mat sau RLAS. Nut nay nap bu chung.
function HD3_ADM_ReloadAll()
	ReLoadScript("\\settings\\trigger_include.lua")
	ReLoadScript("\\settings\\trigger_challengeoftime.lua")
	if (HD_NapLaiCauHinh ~= nil) then HD_NapLaiCauHinh() end
	Msg2Player("§· n¹p bï 2 trigger V­ît ¶i + config. Muèn n¹p l¹i toµn bé script th× gâ trong khung chat: <color=yellow>?gm RLAS<color>")
	Msg2Player("Thø tù ®óng: gâ ?gm RLAS tr­íc, råi bÊm nót nµy ®Ó n¹p bï trigger.")
	HD3_AdminMenu()
end
function HD3_ADM_Reload()
	if (HD_NapLaiCauHinh ~= nil) then HD_NapLaiCauHinh() end
	Msg2Player("§· n¹p l¹i CONFIG (script/header/cauhinh_hoatdong.lua) trong state admin. C¸c kho¸ [RESTART] vÉn cÇn restart.")
	HD3_AdminMenu()
end
function HD3_ADM_ShowCfg()
	local k, v = next(tbCHD, nil)
	local n = 0
	while (k ~= nil) do
		if (type(k) == "string" and strsub(k, 1, 4) == "HD3_") then
			if (type(v) == "table") then
				Msg2Player(format("%s = {%d môc}", k, getn(v)))
			elseif (v == nil) then
				Msg2Player(format("%s = nil (b¶ng gèc)", k))
			else
				Msg2Player(format("%s = %s", k, tostring(v)))
			end
			n = n + 1
		end
		k, v = next(tbCHD, k)
	end
	Msg2Player(format("Tæng %d kho¸ HD3_*.", n))
	HD3_AdminMenu()
end

