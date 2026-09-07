# Phan tich tinh: cho ap dung tinh nang Lua 5.4 trong cay script may chu (1772 tep, 323936 dong)

| Mau | Tong | Y nghia / tinh nang 5.4 |
|---|---:|---|
| `x = x .. y` TRONG vong lap | 116 | table.concat (23x o 20k phan tu) |
| `while ... getn(` | 11 | #t (O(1)) |
| `strfind` trong vong lap (tach chuoi thu cong) | 84 | string.gmatch / gsub |
| `dostring(` | 14 | load() 1 lan + cache |
| `call(` bat loi kieu Lua 4 | 49 | pcall/xpcall (22x) |
| `floor(a/b)` | 394 | a // b (4,4x) |
| tag method | 11 | metatable |
| ham toan cuc dinh nghia | 8680 (phuong thuc A:B 1221) | |
| **ham toan cuc CHI dung trong tep minh** (khong engine/C++/settings/chuoi goi ten) | **1588** | local function (1,3x + het va cham ten giua Include) |
| bien toan cuc muc tep chi dung noi bo | 1915 | local (2,9x doc trong vong lap; het ro ri trang thai) |
| ham > 200 dong | 40 | tach ham |

## Top 40 tep theo diem co hoi (3*concat_loop + 5*getn_while + 2*strfind_loop + 2*dostring + call + 0.5*ham_noi_bo)
| tep | dong | concat/vong | getn while | strfind vong | dostring | call | ham noi bo | bien noi bo | ham>200 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| script/giaodich/daugia/auction_manager.lua | 1536 | 3 | 3 | 2 | 0 | 4 | 53 | 15 | 0 |
| script/kiemthu/test/bangthanh_f.lua | 1343 | 10 | 1 | 0 | 0 | 0 | 7 | 5 | 0 |
| scriptjx2/lib/say.lua | 186 | 6 | 0 | 3 | 1 | 0 | 5 | 1 | 0 |
| script/kiemthu/item/test_loren_admin.lua | 647 | 8 | 0 | 0 | 0 | 0 | 8 | 14 | 0 |
| script/lib/lib_ham.lua | 406 | 7 | 0 | 0 | 1 | 0 | 8 | 8 | 0 |
| scriptjx2/lib/debug.lua | 338 | 2 | 0 | 6 | 2 | 1 | 4 | 3 | 0 |
| script/global/seasonnpc.lua | 1449 | 0 | 2 | 2 | 0 | 0 | 19 | 4 | 0 |
| script/giaodich/thu/mailmanager.lua | 702 | 2 | 0 | 4 | 0 | 0 | 18 | 14 | 0 |
| script/tinhnang/phuban/leaguematch/npc/officer.lua | 615 | 6 | 0 | 1 | 0 | 0 | 5 | 0 | 0 |
| scriptjx2/tong_vn/tong_mix.lua | 1413 | 0 | 0 | 0 | 0 | 0 | 44 | 57 | 0 |
| script/kiemthu/item/test_phiphong_admin.lua | 597 | 1 | 3 | 0 | 0 | 0 | 7 | 17 | 0 |
| script/tinhnang/activitysys/functionlib.lua | 406 | 0 | 0 | 5 | 4 | 2 | 0 | 1 | 0 |
| script/kiemthu/test/event_rank.lua | 246 | 1 | 2 | 0 | 0 | 0 | 10 | 5 | 0 |
| script/tinhnang/phuban/leaguematch/npc/helper.lua | 468 | 4 | 0 | 0 | 0 | 0 | 10 | 1 | 0 |
| script/vatpham/lenhbaitanthu.lua | 1444 | 0 | 0 | 1 | 0 | 0 | 29 | 2 | 0 |
| script/kiemthu/test/citywar_e3.lua | 247 | 5 | 0 | 0 | 0 | 0 | 1 | 0 | 0 |
| script/tinhnang/chientruong/battlehead.lua | 1347 | 0 | 0 | 1 | 0 | 0 | 26 | 49 | 0 |
| script/timerserver.lua | 954 | 1 | 0 | 2 | 0 | 3 | 8 | 4 | 1 |
| script/kiemthu/item/test_kinhmach_admin.lua | 221 | 0 | 0 | 0 | 0 | 0 | 28 | 7 | 0 |
| script/lib/common.lua | 179 | 3 | 0 | 2 | 0 | 0 | 1 | 1 | 0 |
| scriptjx2/lib/compose.lua | 185 | 3 | 0 | 0 | 0 | 0 | 7 | 2 | 0 |
| script/global/skills_table.lua | 1549 | 0 | 0 | 0 | 0 | 0 | 24 | 0 | 1 |
| script/tinhnang/sukien/tongwar/head.lua | 806 | 1 | 0 | 0 | 0 | 0 | 18 | 4 | 0 |
| script/vatpham/lenhbaiadmin.lua | 558 | 0 | 0 | 0 | 0 | 0 | 24 | 0 | 0 |
| scriptjx2/lib/string.lua | 288 | 2 | 0 | 2 | 0 | 0 | 3 | 2 | 0 |
| script/kiemthu/partner/partner_test_bdh.lua | 340 | 1 | 0 | 0 | 1 | 0 | 12 | 4 | 0 |
| script/kiemthu/item/bdh_admin.lua | 282 | 0 | 0 | 0 | 0 | 0 | 20 | 0 | 0 |
| script/tinhnang/phuban/tongcastle/tongcastle.lua | 1106 | 0 | 0 | 5 | 0 | 0 | 0 | 3 | 0 |
| script/tinhnang/phuban/yandibaozang/saizi.lua | 120 | 3 | 0 | 0 | 0 | 0 | 2 | 0 | 0 |
| scriptjx2/tong_vn/workshop/workshop_head.lua | 552 | 0 | 0 | 0 | 0 | 0 | 20 | 23 | 0 |
| script/kiemthu/item/test_hoatdong_admin.lua | 608 | 2 | 0 | 0 | 0 | 0 | 6 | 2 | 0 |
| script/kiemthu/test/tongjx2_menu.lua | 484 | 2 | 0 | 0 | 0 | 0 | 6 | 3 | 0 |
| script/lib/composeclass.lua | 390 | 2 | 0 | 0 | 1 | 1 | 0 | 0 | 0 |
| script/nhanvat/nguoichoi/setmeridian.lua | 423 | 0 | 0 | 0 | 0 | 0 | 18 | 16 | 0 |
| scriptjx2/lib/player.lua | 669 | 0 | 0 | 0 | 0 | 4 | 9 | 2 | 0 |
| script/nhanvat/thucung/petequip.lua | 209 | 2 | 0 | 0 | 0 | 0 | 4 | 1 | 0 |
| script/nhiemvu/tollgate/messenger/posthouse.lua | 1122 | 0 | 0 | 0 | 0 | 0 | 16 | 0 | 0 |
| script/tinhnang/congthanhchien/lib_ctc.lua | 596 | 0 | 0 | 0 | 0 | 0 | 16 | 22 | 0 |
| script/tinhnang/tong_kim_tcap/mobinhtk.lua | 456 | 2 | 0 | 0 | 0 | 0 | 4 | 6 | 0 |
| script/global/npcchucnang/thoren.lua | 1722 | 0 | 0 | 0 | 0 | 0 | 14 | 8 | 0 |

## Chi tiet: x = x .. trong vong lap (tep: dong)
- script/giaodich/daugia/auction_manager.lua: 618, 663, 665
- script/kiemthu/test/bangthanh_f.lua: 169, 177, 201, 534, 892, 905, 1052, 1137, 1145, 1217
- scriptjx2/lib/say.lua: 77, 96, 124, 133, 177, 179
- script/kiemthu/item/test_loren_admin.lua: 130, 262, 333, 394, 397, 399, 628, 631
- script/lib/lib_ham.lua: 89, 153, 156, 161, 164, 193, 196
- scriptjx2/lib/debug.lua: 223, 225
- script/giaodich/thu/mailmanager.lua: 667, 669
- script/tinhnang/phuban/leaguematch/npc/officer.lua: 316, 318, 320, 322, 324, 360
- script/kiemthu/item/test_phiphong_admin.lua: 586
- script/kiemthu/test/event_rank.lua: 151
- script/tinhnang/phuban/leaguematch/npc/helper.lua: 322, 324, 352, 354
- script/kiemthu/test/citywar_e3.lua: 66, 74, 190, 213, 227
- script/timerserver.lua: 135
- script/lib/common.lua: 48, 88, 116
- scriptjx2/lib/compose.lua: 171, 173, 176
- script/tinhnang/sukien/tongwar/head.lua: 602
- scriptjx2/lib/string.lua: 131, 271
- script/kiemthu/partner/partner_test_bdh.lua: 263
- script/tinhnang/phuban/yandibaozang/saizi.lua: 86, 89, 100
- script/kiemthu/item/test_hoatdong_admin.lua: 560, 573
- script/kiemthu/test/tongjx2_menu.lua: 266, 268
- script/lib/composeclass.lua: 71, 381
- script/nhanvat/thucung/petequip.lua: 110, 169
- script/tinhnang/tong_kim_tcap/mobinhtk.lua: 88, 90
- script/tinhnang/chientruong/weeklyrank.lua: 60
- script/global/mantlesystem/mantleupgrade_head.lua: 369, 1275
- script/kiemthu/item/liendau_admin.lua: 149, 159
- script/tinhnang/phuban/citywar_city/timer.lua: 19, 21
- script/tinhnang/phuban/citywar_city/totaltimer.lua: 9, 11
- script/tinhnang/sukien/equip_publish/wuxingyin/wuxingyin.lua: 205, 253
- script/tinhnang/vng_event/thapnienlenhbai/mainfuc.lua: 16, 24
- scriptjx2/lib/basic.lua: 57, 113
- scriptjx2/lib/composeclass.lua: 317
- script/tinhnang/3hoatdong/hd3_driver.lua: 68
- script/lib/composeex.lua: 488
- script/tinhnang/liendau/league.lua: 117
- script/tinhnang/phuban/leaguematch/macthtype/normal.lua: 24
- script/tinhnang/phuban/yandibaozang/timer_match.lua: 43
- script/tinhnang/phuban/leaguematch/glbmission/schedule.lua: 136
- script/giaodich/thu/maildef.lua: 59
- script/tinhnang/sukien/longmenbiaoju/lmbj_config.lua: 684
- scriptjx2/tong_vn/tong_setting.lua: 82
- script/kiemthu/test/test_tongjx2.lua: 51
- script/tinhnang/activitysys/ladderfunlib.lua: 22
- script/tinhnang/liendau/head.lua: 141
- script/tinhnang/phuban/tong/collectgoods/mission.lua: 91
- script/tinhnang/phuban/tong/collectgoods/schedule/report_smalltimer.lua: 82
- script/tinhnang/phuban/tong/tong_disciple/mission.lua: 90
- script/tinhnang/phuban/tong/tong_disciple/schedule/report_smalltimer.lua: 80
- script/tinhnang/phuban/yandibaozang/player_death.lua: 87
- script/tinhnang/vng_feature/challengeoftime/npcNhiepThiTran.lua: 39
- scriptjx2/tong_vn/log.lua: 10
- scriptjx2/tong_vn/npc/city_totempole.lua: 377
- scriptjx2/tong_vn/npc/tong_totempole.lua: 393

## Chi tiet: while getn
- script/giaodich/daugia/auction_manager.lua: 105, 177, 1210
- script/kiemthu/test/bangthanh_f.lua: 1051
- script/global/seasonnpc.lua: 1255, 1269
- script/kiemthu/item/test_phiphong_admin.lua: 272, 293, 389
- script/kiemthu/test/event_rank.lua: 58, 193

## Chi tiet: strfind trong vong lap (tach chuoi)
- script/giaodich/daugia/auction_manager.lua: 167, 1524
- scriptjx2/lib/say.lua: 46, 48, 56
- scriptjx2/lib/debug.lua: 245, 246, 252, 253, 260, 261
- script/global/seasonnpc.lua: 1237, 1256
- script/giaodich/thu/mailmanager.lua: 32, 57, 96, 558
- script/tinhnang/phuban/leaguematch/npc/officer.lua: 312
- script/tinhnang/activitysys/functionlib.lua: 27, 97, 177, 202, 235
- script/vatpham/lenhbaitanthu.lua: 334
- script/tinhnang/chientruong/battlehead.lua: 549
- script/timerserver.lua: 106, 410
- script/lib/common.lua: 75, 129
- scriptjx2/lib/string.lua: 52, 96
- script/tinhnang/phuban/tongcastle/tongcastle.lua: 317, 591, 591, 714, 714
- script/lib/worldlibrary.lua: 23
- script/lib/composeex.lua: 180
- script/header/tongkim.lua: 94
- script/tinhnang/phuban/tongwar/match/head.lua: 359
- script/npclevelscript/animal.lua: 110
- script/npclevelscript/animal_userdef.lua: 113
- script/npclevelscript/army.lua: 110
- script/npclevelscript/called_soldier1.lua: 110
- script/npclevelscript/dragonboat_long.lua: 110
- script/npclevelscript/dragonboat_random.lua: 110
- script/npclevelscript/dragonboat_range1.lua: 110
- script/npclevelscript/dragonboat_range2.lua: 110
- script/npclevelscript/dragonboat_short.lua: 110
- script/npclevelscript/fengzhiqi.lua: 111
- script/npclevelscript/lib.lua: 13
- script/npclevelscript/muren.lua: 110
- script/npclevelscript/newanimal.lua: 110
- script/npclevelscript/newnpc.lua: 110
- script/npclevelscript/npclevelscript.lua: 110
- script/npclevelscript/npc_changgemenarea.lua: 110
- script/npclevelscript/npc_fenglingdubei.lua: 110
- script/npclevelscript/npc_fenglingdunan.lua: 110
- script/npclevelscript/npc_jinshandao.lua: 110
- script/npclevelscript/npc_mobeicaoyuan.lua: 110
- script/npclevelscript/npc_mogaoku.lua: 110
- script/npclevelscript/npc_xiaoxiao.lua: 110
- script/npclevelscript/randomtask_long.lua: 110
- script/npclevelscript/randomtask_random.lua: 110
- script/npclevelscript/randomtask_range2.lua: 110
- script/npclevelscript/randomtask_short.lua: 110
- script/npclevelscript/shanshenmiao.lua: 111
- script/npclevelscript/soldier1.lua: 110
- script/npclevelscript/soldier2.lua: 110
- script/npclevelscript/soldier3.lua: 110
- script/npclevelscript/soldier4.lua: 110
- script/npclevelscript/standard.lua: 110
- script/npclevelscript/strong.lua: 111
- script/npclevelscript/stupidanimal.lua: 110
- script/npclevelscript/supernpc.lua: 21
- script/npclevelscript/tasknpc.lua: 110
- script/npclevelscript/task_goldennpc.lua: 110
- script/npclevelscript/task_normalnpc.lua: 110
- script/tinhnang/phuban/tong/collectgoods/npcpoint.lua: 191
- script/tinhnang/phuban/tong/tong_disciple/npcpoint.lua: 191
- script/tinhnang/phuban/tong/tong_disciple/turnoutpoint.lua: 191
- script/tinhnang/phuban/tong/tong_springfestival/file_npc_head.lua: 191
- script/tinhnang/phuban/tong/tong_springfestival/npcpoint.lua: 191
- scriptjx2/lib/coordinate.lua: 215

## Chi tiet: dostring
- scriptjx2/lib/say.lua: 20
- script/lib/lib_ham.lua: 97
- scriptjx2/lib/debug.lua: 254, 265
- script/tinhnang/activitysys/functionlib.lua: 45, 56, 224, 233
- script/kiemthu/partner/partner_test_bdh.lua: 182
- script/lib/composeclass.lua: 356
- scriptjx2/lib/composeclass.lua: 292
- scriptjx2/lib/simulator.lua: 51, 60
- script/nhiemvu/system/task_string.lua: 130

## Chi tiet: call(
- script/giaodich/daugia/auction_manager.lua: 410, 1233, 1304, 1326
- scriptjx2/lib/debug.lua: 21
- script/tinhnang/activitysys/functionlib.lua: 320, 333
- script/timerserver.lua: 185, 188, 191
- script/lib/composeclass.lua: 281
- scriptjx2/lib/player.lua: 131, 139, 552, 565
- script/nhiemvu/hoithoai/dailogsay.lua: 58, 83, 109, 133, 149, 163
- scriptjx2/lib/composeclass.lua: 217
- script/lib/worldlibrary.lua: 40
- script/tinhnang/sukien/longmenbiaoju/lmbj_config.lua: 602
- script/cauhinh_web/cfgw_driver.lua: 140
- script/tinhnang/phuban/basemission/dungeon.lua: 247
- script/tinhnang/phuban/basemission/lib.lua: 12, 22
- script/lib/pfunction.lua: 59
- script/lib/progressbar.lua: 54, 61
- script/lib/remoteexc.lua: 65, 94
- script/lib/awardtype/simple.lua: 49, 54
- script/misc/eventsys/eventsys.lua: 69, 101
- scriptjx2/lib/progressbar.lua: 111, 120
- scriptjx2/lib/awardtype/honour.lua: 38, 43
- scriptjx2/lib/awardtype/item.lua: 144, 149
- scriptjx2/lib/awardtype/title.lua: 30, 34
- script/global/onkillnpc.lua: 32
- script/lib/baseclass.lua: 55
- script/tinhnang/activitysys/g_activity.lua: 20
- scriptjx2/lib/awardtype/zhenyuanpoint.lua: 36

## Top 30 tep co nhieu ham chi dung noi bo (ung vien `local function`)
- script/giaodich/daugia/auction_manager.lua: 53/70 ham (vd AUC_Log, AUC_SendTo, AUC_GetMoney, AUC_PayMoney, AUC_MoneyAward, AUC_SoTien)
- scriptjx2/tong_vn/tong_mix.lua: 44/64 ham (vd LimitMemberCountJoinTong_GSV, _TONG_MIX_DEBUG_Print_TABLE, _TONG_MIX_DEBUG, DefFun3, MONEYFUND_ADD_R, MONEYFUND_ADD_G_1)
- script/vatpham/lenhbaitanthu.lua: 29/74 ham (vd nhanqht, AdminTestTKH, mocnapthe1000, thongbao, nhangm, huygm)
- script/kiemthu/item/test_kinhmach_admin.lua: 28/44 ham (vd KMT_LayCap, KMT_Pow256, KMT_LayBaoDay, KM_Chon1, KM_Chon2, KM_Chon3)
- script/tinhnang/chientruong/battlehead.lua: 26/43 ham (vd ExhibitBoss, bt_CheckDeathValid, bt_CheckLifeMax, bt_map2battlelevel, bt_camp_getbonus, bt_addfightnpc)
- script/global/skills_table.lua: 24/29 ham (vd check_update, skills_update, check_faction, add_misc, update_tw, update_gb)
- script/vatpham/lenhbaiadmin.lua: 24/41 ham (vd ktraadmin, reloadfileEvent, laymau001, ai30, LayBinhSiHieuPhu, DemoSetNpcTimeIdle)
- script/kiemthu/item/bdh_admin.lua: 20/32 ham (vd BDH_A_CapKim, BDH_A_CapNgau, BDH_A_Goi, BDH_A_Exp, BDH_A_LenCap, BDH_A_Emo)
- scriptjx2/tong_vn/workshop/workshop_head.lua: 20/27 ham (vd DEF_FUN, RL_DoLevelUp, RL_DoDegrade, RL_DoOpen, RL_DoClose, RL_DoDestroy)
- script/global/seasonnpc.lua: 19/53 ham (vd checkTask_Limit, Task_NewVersionAward, Task_MainDialog, Task_TotalCancel, Task_ProcessInfo, Task_Accept_04)
- script/giaodich/thu/mailmanager.lua: 18/31 ham (vd MailManager_TaskName, MailManager_Split, MailManager_Trim, MailManager_AwardInfo, MailManager_SendTo, MailManager_Headers)
- script/tinhnang/sukien/tongwar/head.lua: 18/39 ham (vd tongwar_getmember, tongwar_bigger, get_tongwar_phase, tongWar_SelectCity, tongWar_CanSelectCity, tongWar_AddSelectCityOpt)
- script/nhanvat/nguoichoi/setmeridian.lua: 18/19 ham (vd KM_Pow256, KM_GetByte, KM_SetByte, KM_TenMach, KM_BaoVeCap, KM_MachMoi)
- script/nhiemvu/tollgate/messenger/posthouse.lua: 16/69 ham (vd messenger_levelmu, messenger_leveltong, messenger_levelyin, messenger_leveljin, messenger_levelyuci, messenger_flyprize)
- script/tinhnang/congthanhchien/lib_ctc.lua: 16/31 ham (vd GetNameBangCong, GetNameBangThu, GetIdBangCong, GetIdBangThu, GetMasterBangCong, GetMasterBangThu)
- script/global/npcchucnang/thoren.lua: 14/46 ham (vd ghepmanhdopho, ghephkmp, nangcapanbang, nangcappan, nangcapptrangsuc, nangcappanvip)
- scriptjx2/tong_vn/tong.lua: 14/26 ham (vd DefFun1, INIT_R, ADD_R, WriteXPLog, Maintain_Stunt, WEEKLY_MAINTAIN_G_2)
- script/global/trangbihoangkim.lua: 13/55 ham (vd trangbikimo, trangbiminhphung, trangbihonganh, trangbiliendau, chonanbang, chondinhquoc)
- script/global/vatpham.lua: 13/22 ham (vd AddRandXuHoatDong, lenhbaihtdatau, ruonghoangkim, ruonghoangkim2, NhanPTLBCC, Nhan500NHKT)
- script/nhiemvu/newtask/tasklink/tasklink_award.lua: 13/19 ham (vd tl_getawardtypeforrate, tl_giveplayeraward_goods, tl_getawardagin, tl_getaward, tl_linkaward_mail, tl_linkaward_count)
- script/kiemthu/partner/partner_test_bdh.lua: 12/14 ham (vd BDH_Log, BDH_Check, BDH_TimNguoiChoi, BDH_TestPersist, BDH_TestTick, BDH_TestTalk)
- script/log_game/save_log.lua: 12/21 ham (vd logTBHK, logEventThang1, logEventThang10, logPhanThuongToiDa, logTriAn, logTOP10)
- script/nhiemvu/metempsychosis/translife_5.lua: 12/13 ham (vd TRANSLIFE5_onFinishSongJin, TRANSLIFE5_onChuanguan, TRANSLIFE5_onYDBZguoguan, TRANSLIFE5_ontianlu_tree, TRANSLIFE5_onFinishSongJinAward, TRANSLIFE5_onChuanguanAward)
- script/nhiemvu/partner/education/partner_educationpeople.lua: 12/105 ham (vd partner_keepjiguan1, partner_keepjiguan2, partner_keepjiguan3, partner_keepjiguan4, partner_keepzhuofeifan, partner_keepheishadizi)
- script/tinhnang/phuban/citywar_global/infocenter_head.lua: 12/38 ham (vd EnterGame, SignupACity, getCityWarElector, ArenaInfo, AllArenaInfo, checkIsTakeQingtongDing)
- script/tinhnang/sukien/event_huyhoang_dungdb/commonscript.lua: 12/20 ham (vd QuickTestHuyHoang, DenMapHuyHoang, MapHuyHoangTieu, MapHuyHoangTrung, MapHuyHoangDai, nghietlongdong)
- script/global/mantlesystem/mantleupgrade_npc.lua: 11/35 ham (vd onTayAnAttr, onAdvanceToWuJi, onBreakThroughWuJi, PF_InlayMoLai, PF_InlayLoi, PF_MoLaiWashBox)
- script/lib/lib_faction.lua: 11/12 ham (vd GetFactionEx, GetFactionEx1, GetFactionEx2, AddFaction, LeaveFaction, ReturnFaction)
- script/misc/vngpromotion/ipbonus/ipbonus_2_head.lua: 11/12 ham (vd IpBonus_Start, IpBonus_Close, IpResetTask, IpBonus_Add, IpBonus_Minus, IpBonus_Minus_Yellow_Box)
- script/nhanvat/kynang/special/jxf_pkitems.lua: 11/18 ham (vd Getlifemax_yan_v, Getmanamax_yan_v, Getallres_yan_p, Getattackspeed_yan_v, Getcastspeed_yan_v, Getaddcoldmagic_v)

## Ham > 200 dong
- script/timerserver.lua: dong 140 (223 dong)
- script/global/skills_table.lua: dong 1260 (258 dong)
- script/nhiemvu/newtask/tasklink/tasklink_award.lua: dong 12 (267 dong)
- script/npclevelscript/animal.lua: dong 128 (274 dong)
- script/npclevelscript/animal_userdef.lua: dong 131 (231 dong)
- script/npclevelscript/called_soldier1.lua: dong 128 (245 dong)
- script/npclevelscript/dragonboat_long.lua: dong 128 (213 dong)
- script/npclevelscript/dragonboat_random.lua: dong 128 (228 dong)
- script/npclevelscript/dragonboat_range1.lua: dong 128 (258 dong)
- script/npclevelscript/dragonboat_range2.lua: dong 128 (258 dong)
- script/npclevelscript/dragonboat_short.lua: dong 128 (213 dong)
- script/npclevelscript/fengzhiqi.lua: dong 129 (225 dong)
- script/npclevelscript/newanimal.lua: dong 128 (274 dong)
- script/npclevelscript/newnpc.lua: dong 128 (241 dong)
- script/npclevelscript/npc_fenglingdunan.lua: dong 128 (231 dong)
- script/npclevelscript/npc_mogaoku.lua: dong 128 (236 dong)
- script/npclevelscript/npc_xiaoxiao.lua: dong 128 (236 dong)
- script/npclevelscript/randomtask_long.lua: dong 128 (213 dong)
- script/npclevelscript/randomtask_random.lua: dong 128 (228 dong)
- script/npclevelscript/randomtask_range2.lua: dong 128 (258 dong)
- script/npclevelscript/randomtask_short.lua: dong 128 (213 dong)
- script/npclevelscript/shanshenmiao.lua: dong 129 (225 dong)
- script/npclevelscript/soldier1.lua: dong 128 (204 dong)
- script/npclevelscript/soldier2.lua: dong 128 (206 dong)
- script/npclevelscript/soldier3.lua: dong 128 (206 dong)
- script/npclevelscript/soldier4.lua: dong 128 (206 dong)
- script/npclevelscript/standard.lua: dong 128 (242 dong)
- script/npclevelscript/tasknpc.lua: dong 128 (236 dong)
- script/npclevelscript/task_goldennpc.lua: dong 128 (234 dong)
- script/npclevelscript/task_normalnpc.lua: dong 128 (237 dong)
- script/startgame/khac/othermap.lua: dong 88 (269 dong)
- script/header/testgame.lua: dong 126 (225 dong)
- script/nhiemvu/tollgate/messenger/qianbaoku/enemy_tureboss.lua: dong 19 (211 dong)
- script/npclevelscript/dragonboat_boss1.lua: dong 12 (252 dong)
- script/npclevelscript/dragonboat_boss2.lua: dong 12 (252 dong)
- script/npclevelscript/highchallenge_boss.lua: dong 12 (252 dong)
- script/npclevelscript/jianzhongmigong.lua: dong 12 (236 dong)
- script/npclevelscript/lowchallenge_boss.lua: dong 12 (252 dong)
- script/npclevelscript/qianbaoku.lua: dong 12 (236 dong)
- script/npclevelscript/randomtask_boss2.lua: dong 12 (252 dong)