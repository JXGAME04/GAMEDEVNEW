-- ============================================================================
-- HD3_ADMIN.LUA - Menu TEST 3 hoat dong ban Linux tren Lenh Bai Admin.
-- Include tu lenhbaiadmin.lua (main() dofile lai) => SUA KHONG CAN RESTART.
-- Cau hinh: script\header\cauhinh_hoatdong.lua (khoi [6]).
-- ============================================================================
Include("\\script\\tinhnang\\3hoatdong\\hd3_driver.lua")

function HD3_AdminMenu()
	SayEx({"<color=yellow>Ho¹t ®éng ban Linux (test)<color>: chän môc",
	"1. S¨n Boss S¸t Thñ/HD3_ADM_ST",
	"2. Phong L¨ng §é/HD3_ADM_PLD",
	"3. V­ît ¶i/HD3_ADM_VA",
	"4. N¹p l¹i CONFIG/HD3_ADM_Reload",
	"KÕt thóc ®èi tho¹i./no"})
end

function HD3_ADM_ST()
	SayEx({"<color=yellow>S¨n Boss S¸t Thñ<color> - cÊp >=" .. HD_CFG("HD3_ST_CAP_TOITHIEU", 90) .. ", trÇn " .. HD_CFG("HD3_ST_MAX_NGAY", 8) .. " lÇn/ngµy",
	"Sinh l¹i NPC 769 + 160 boss (boot)/HD3_ADM_ST_Boot",
	"DÞch chuyÓn: NPC NhiÕp ThÝ TrÇn (Ba L¨ng HuyÖn)/HD3_ADM_ST_Tele",
	"NhËn 5 S¸t Thñ lÖnh cÊp 90 (test gép)/HD3_ADM_ST_Lenh",
	"NhËn 1 S¸t Thñ Gi¶n cÊp 90 (vÐ V­ît ¶i)/HD3_ADM_ST_Gian",
	"Xem biÕn nhiÖm vô (1082, 1192, 1193, 1217)/HD3_ADM_ST_Task",
	"Reset sè lÇn giÕt trong ngµy/HD3_ADM_ST_Reset",
	"Quay l¹i/HD3_AdminMenu"})
end
function HD3_ADM_ST_Boot() HD3_DriverInit() Msg2Player("§· gäi HD3_DriverInit (sinh NPC + boss).") end
function HD3_ADM_ST_Tele() NewWorld(1, 1506, 3198) end
function HD3_ADM_ST_Lenh()
	for i = 1, 5 do AddItem(6, 1, 398, 90, 0, 0) end
	Msg2Player("§· nhËn 5 S¸t Thñ lÖnh cÊp 90 (6,1,398). GÆp NhiÕp ThÝ TrÇn ®Ó gép thµnh S¸t Thñ Gi¶n.")
end
function HD3_ADM_ST_Gian() AddItem(6, 1, 399, 90, 0, 0) Msg2Player("§· nhËn 1 S¸t Thñ Gi¶n cÊp 90 (6,1,399) - vÐ vµo V­ît ¶i.") end
function HD3_ADM_ST_Task()
	Msg2Player(format("1082=%d (chØ sè) | 1192=%d (ngµy) | 1193=%d (sè lÇn) | 1217=%d (tÝch lòy)", GetTask(1082), GetTask(1192), GetTask(1193), GetTask(1217)))
	HD3_ADM_ST()
end
function HD3_ADM_ST_Reset() SetTask(1193, 0) Msg2Player("§· reset sè lÇn giÕt boss trong ngµy.") end

function HD3_ADM_PLD()
	SayEx({"<color=yellow>Phong L¨ng §é<color> - më mçi giê phót :00; giê tèn phÝ 10/14/16/18/20h",
	"Khai cuéc NGAY (më ®¨ng ký thuyÒn)/HD3_ADM_PLD_Now",
	"DÞch chuyÓn: ThuyÒn phu bê Nam (map 336)/HD3_ADM_PLD_Tele336",
	"DÞch chuyÓn: vµo map thuyÒn 337/HD3_ADM_PLD_Tele337",
	"NhËn LÖnh bµi Phong L¨ng §é (4,489)/HD3_ADM_PLD_LB",
	"NhËn LÖnh Bµi Thñy TÆc (6,1,3363)/HD3_ADM_PLD_LBTT",
	"Quay l¹i/HD3_AdminMenu"})
end
function HD3_ADM_PLD_Now() HD3_Adm_PLD_Now() Msg2Player("§· Ðp khai cuéc Phong L¨ng §é (fenglingdu_main).") end
function HD3_ADM_PLD_Tele336() NewWorld(336, 1147, 3018) end
function HD3_ADM_PLD_Tele337() NewWorld(337, 1646, 3233) end
function HD3_ADM_PLD_LB() AddItem(4, 489, 0, 0, 0, 0) Msg2Player("§· nhËn LÖnh bµi Phong L¨ng §é.") end
function HD3_ADM_PLD_LBTT() AddItem(6, 1, 3363, 0, 0, 0) Msg2Player("§· nhËn LÖnh Bµi Thñy TÆc.") end

function HD3_ADM_VA()
	SayEx({"<color=yellow>V­ît ¶i<color> - b¸o danh mçi giê :00; cÇn 1 S¸t Thñ Gi¶n + ®ñ 4 ng­êi tæ ®éi",
	"B¸o danh NGAY (më mission)/HD3_ADM_VA_Now",
	"Trao b¶ng xÕp h¹ng ngµy/HD3_ADM_VA_Rank",
	"NhËn 1 S¸t Thñ Gi¶n cÊp 90 (vÐ vµo)/HD3_ADM_ST_Gian",
	"DÞch chuyÓn: thµnh Ba L¨ng (gÆp DÞch Qu¸n)/HD3_ADM_ST_Tele",
	"Quay l¹i/HD3_AdminMenu"})
end
function HD3_ADM_VA_Now() HD3_Adm_VA_Now() Msg2Player("§· Ðp b¸o danh V­ît ¶i (trigger OnTrigger).") end
function HD3_ADM_VA_Rank() HD3_Adm_VA_Rank() Msg2Player("§· ch¹y b¶ng xÕp h¹ng ngµy (Ladder 10235).") end

function HD3_ADM_Reload()
	if (HD_NapLaiCauHinh ~= nil) then HD_NapLaiCauHinh() end
	Msg2Player("§· n¹p l¹i CONFIG (script\header\cauhinh_hoatdong.lua).")
	HD3_AdminMenu()
end

