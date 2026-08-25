-- ============================================================================
-- HD3_ADMIN.LUA - Menu TEST 3 hoat dong ban Linux tren Lenh Bai Admin. (v2)
-- Include tu lenhbaiadmin.lua (main() dofile lai) => SUA KHONG CAN RESTART.
-- Cau hinh: script\header\cauhinh_hoatdong.lua (khoi [6]).
-- CAM dau "/" trong nhan menu (ScriptFuns.cpp:717 cat o dau / dau tien).
-- ============================================================================
Include("\\script\\tinhnang\\3hoatdong\\hd3_driver.lua")

function HD3_AdminMenu()
	SayEx({"<color=yellow>Hoπt ÆÈng ban Linux (test)<color>: ch‰n mÙc",
	"1. S®n Boss S∏t ThÒ/HD3_ADM_ST",
	"2. Phong L®ng ßÈ/HD3_ADM_PLD",
	"3. V≠Ót ∂i/HD3_ADM_VA",
	"4. Nπp lπi CONFIG/HD3_ADM_Reload",
	"5. Xem c∏c kho∏ c u h◊nh HD3 Æang hi÷u l˘c/HD3_ADM_ShowCfg",
	"K’t thÛc ÆËi thoπi./no"})
end

function HD3_ADM_ST()
	SayEx({"<color=yellow>S®n Boss S∏t ThÒ<color> - tr«n " .. HD_CFG("HD3_ST_MAX_NGAY", 8) .. " l«n mÈt ngµy; chÿ nh„m c p 90 (chÿ sË 141-160) ph∏t th≠Îng",
	"Sinh lπi NPC 769 vµ 160 boss - Chÿ khi boot lÁi, g‰i l∆p sœ nh©n b∂n NPC/HD3_ADM_ST_Boot",
	"Dﬁch chuy”n: NPC Nhi’p Th› Tr«n (Ph≠Óng T≠Íng)/HD3_ADM_ST_Tele",
	"NhÀn 5 S∏t ThÒ l÷nh c p 90 (test gÈp Gi∂n)/HD3_ADM_ST_Lenh",
	"NhÀn 2 S∏t ThÒ Gi∂n c p 90 (ÆÒ v– b∏o danh V≠Ót ∂i cao c p)/HD3_ADM_ST_Gian2",
	"ß∆t boss Æang nhÀn (task 1082) theo sË nhÀp 1-160/HD3_ADM_ST_SetBoss",
	"Xem bi’n nhi÷m vÙ (1082, 1192, 1193, 1217)/HD3_ADM_ST_Task",
	"Reset sË l«n gi’t trong ngµy/HD3_ADM_ST_Reset",
	"Quay lπi/HD3_AdminMenu"})
end
function HD3_ADM_ST_Boot() HD3_DriverInit() Msg2Player("ß∑ g‰i HD3_DriverInit (t˘ d‰n NPC cÚ cÒa h÷ rÂi sinh lπi - g‰i l∆p an toµn).") end
function HD3_ADM_ST_Tele() NewWorld(1, 1506, 3198) end
function HD3_ADM_ST_Lenh()
	for i = 1, 5 do AddItem(6, 1, 398, 90, mod(i, 5), 0, 0) end
	Msg2Player("ß∑ nhÀn 5 S∏t ThÒ l÷nh c p 90 (ÆÒ 5 h÷). G∆p Nhi’p Th› Tr«n Æ” gÈp thµnh S∏t ThÒ Gi∂n.")
end
function HD3_ADM_ST_Gian2()
	AddItem(6, 1, 399, 90, 0, 0, 0)
	AddItem(6, 1, 399, 90, 1, 0, 0)
	Msg2Player("ß∑ nhÀn 2 S∏t ThÒ Gi∂n c p 90 - ÆÒ v– ÆÈi tr≠Îng b∏o danh V≠Ót ∂i cao c p.")
end
function HD3_ADM_ST_SetBoss()
	OpenGetNumber("NhÀp chÿ sË boss (1-160, nh„m 90 = 141-160)", "HD3_ADM_ST_SetBossCB")
end
function HD3_ADM_ST_SetBossCB()
	local n = GetNumberFromUI()
	if (n == nil or n < 0 or n > 160) then Msg2Player("SË kh´ng hÓp l÷ (0 = xo∏ nhi÷m vÙ).") return end
	SetTask(1082, n)
	Msg2Player(format("ß∑ Æ∆t task 1082 = %d. Gi’t ÆÛng boss mang chÿ sË nµy Æ” hoµn thµnh.", n))
end
function HD3_ADM_ST_Task()
	Msg2Player(format("1082=%d (chÿ sË) - 1192=%d (ngµy) - 1193=%d (sË l«n) - 1217=%d (t›ch lÚy)", GetTask(1082), GetTask(1192), GetTask(1193), GetTask(1217)))
	HD3_ADM_ST()
end
function HD3_ADM_ST_Reset() SetTask(1193, 0) Msg2Player("ß∑ reset sË l«n gi’t boss trong ngµy.") end

function HD3_ADM_PLD()
	SayEx({"<color=yellow>Phong L®ng ßÈ<color> - mÎ theo giÍ trong HD3_PLD_GIO; giÍ tËn ph› dÔng L÷nh Bµi ThÒy T∆c",
	"Khai cuÈc NGAY (mÎ Æ®ng k˝ thuy“n)/HD3_ADM_PLD_Now",
	"Xem trπng th∏i 3 thuy“n (mission 15)/HD3_ADM_PLD_TrangThai",
	"Dﬁch chuy”n: Thuy“n phu bÍ Nam (map 336)/HD3_ADM_PLD_Tele336",
	"Vµo map thuy“n 337 (xem nÛt tho∏t b™n d≠Ìi!)/HD3_ADM_PLD_Tele337",
	"Tho∏t kh·i map thuy“n v“ Ph≠Óng T≠Íng/HD3_ADM_PLD_Thoat",
	"NhÀn L÷nh bµi Phong L®ng ßÈ (4,489)/HD3_ADM_PLD_LB",
	"NhÀn L÷nh Bµi ThÒy T∆c (6,1,3363)/HD3_ADM_PLD_LBTT",
	"Quay lπi/HD3_AdminMenu"})
end
function HD3_ADM_PLD_Now() HD3_Adm_PLD_Now() Msg2Player("ß∑ –p khai cuÈc Phong L®ng ßÈ (fenglingdu_main).") end
function HD3_ADM_PLD_TrangThai()
	local tbMap = {337, 338, 339}
	local nOld = SubWorld
	for i = 1, 3 do
		local idx = SubWorldID2Idx(tbMap[i])
		if (idx >= 0) then
			SubWorld = idx
			Msg2Player(format("Thuy“n %d (map %d): mission=%d, ng≠Íi=%d, phÛt=%d", i, tbMap[i], IsMission(15), GetMSPlayerCount(15, 1), GetMissionV(2)))
		end
	end
	SubWorld = nOld
	HD3_ADM_PLD()
end
function HD3_ADM_PLD_Tele336() NewWorld(336, 1147, 3018) end
function HD3_ADM_PLD_Tele337() NewWorld(337, 1646, 3233) end
function HD3_ADM_PLD_Thoat() NewWorld(1, 1506, 3198) Msg2Player("ß∑ tho∏t v“ Ph≠Óng T≠Íng.") end
function HD3_ADM_PLD_LB() AddItem(4, 489, 0, 0, 0, 0, 0) Msg2Player("ß∑ nhÀn L÷nh bµi Phong L®ng ßÈ.") end
function HD3_ADM_PLD_LBTT() AddItem(6, 1, 3363, 0, 0, 0, 0) Msg2Player("ß∑ nhÀn L÷nh Bµi ThÒy T∆c (6,1,3363).") end

function HD3_ADM_VA()
	SayEx({"<color=yellow>V≠Ót ∂i<color> - b∏o danh theo giÍ HD3_VA_GIO; ÆÈi tr≠Îng c«n 2 S∏t ThÒ Gi∂n + tÊ ÆÈi 4 ng≠Íi",
	"B∏o danh NGAY (mÎ mission c∏c map ∂i)/HD3_ADM_VA_Now",
	"Xem trπng th∏i ∂i (map 464 s¨ c p, 480 cao c p)/HD3_ADM_VA_TrangThai",
	"Trao b∂ng x’p hπng ngµy (Ladder 10235)/HD3_ADM_VA_Rank",
	"ß∆t thµnh t›ch h´m qua (task 2636-2639) Æ” test nhÀn th≠Îng/HD3_ADM_VA_SetRank",
	"NhÀn 2 S∏t ThÒ Gi∂n c p 90 (v– vµo)/HD3_ADM_ST_Gian2",
	"Dﬁch chuy”n tÌi Nhi’p Th› Tr«n (Ph≠Óng T≠Íng)/HD3_ADM_ST_Tele",
	"Quay lπi/HD3_AdminMenu"})
end
function HD3_ADM_VA_Now() HD3_Adm_VA_Now() Msg2Player("ß∑ –p b∏o danh V≠Ót ∂i (trigger OnTrigger).") end
function HD3_ADM_VA_TrangThai()
	local tbMap = {464, 480, 957}
	local nOld = SubWorld
	for i = 1, 3 do
		local idx = SubWorldID2Idx(tbMap[i])
		if (idx >= 0) then
			SubWorld = idx
			Msg2Player(format("Map %d: mission22=%d, ng≠Íi=%d, trπng th∏i=%d", tbMap[i], IsMission(22), GetMSPlayerCount(22, 1), GetMissionV(4)))
		else
			Msg2Player(format("Map %d: ch≠a nπp (WorldSet)", tbMap[i]))
		end
	end
	SubWorld = nOld
	HD3_ADM_VA()
end
function HD3_ADM_VA_Rank() HD3_Adm_VA_Rank() Msg2Player("ß∑ chπy b∂ng x’p hπng ngµy (Ladder 10235).") end
function HD3_ADM_VA_SetRank()
	local nHomQua = tonumber(GetLocalDate("%y%m%d")) - 1
	SetTask(2636, nHomQua)
	SetTask(2637, 600)
	SetTask(2638, 0)
	SetTask(2639, 0)
	Msg2Player(format("ß∑ Æ∆t 2636=%d (h´m qua), 2637=600 (10 phÛt). G∆p Nhi’p Th› Tr«n b m nhÀn th≠Îng x’p hπng.", nHomQua))
end

function HD3_ADM_Reload()
	if (HD_NapLaiCauHinh ~= nil) then HD_NapLaiCauHinh() end
	Msg2Player("ß∑ nπp lπi CONFIG (script/header/cauhinh_hoatdong.lua) trong state admin. C∏c kho∏ [RESTART] v…n c«n restart.")
	HD3_AdminMenu()
end
function HD3_ADM_ShowCfg()
	local k, v = next(tbCHD, nil)
	local n = 0
	while (k ~= nil) do
		if (type(k) == "string" and strsub(k, 1, 4) == "HD3_") then
			if (type(v) == "table") then
				Msg2Player(format("%s = {%d mÙc}", k, getn(v)))
			elseif (v == nil) then
				Msg2Player(format("%s = nil (b∂ng gËc)", k))
			else
				Msg2Player(format("%s = %s", k, tostring(v)))
			end
			n = n + 1
		end
		k, v = next(tbCHD, k)
	end
	Msg2Player(format("TÊng %d kho∏ HD3_*.", n))
	HD3_AdminMenu()
end

