# Doi chieu tep bao dong: ban Linux (JX2) vs may chu JX1 dang chay

Nguon A: `D:\ServerLinux\server1\script`
Nguon B: `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script`

**Y NGHIA CAC MUC**

| Muc | Nghia | Viec phai lam khi port |
|---|---|---|
| `MISSING` | JX1 chua co tep nay | chep moi |
| `IDENTICAL` | byte y het | khong lam gi |
| `NEAR` | >= 90% dong trung | doc diff, thuong an toan |
| `DIVERGED` | lech nhieu | **CAM de len** - phai hop nhat bang tay |

## satthu

| Tep | Trang thai | Dong (Linux) | Dong (JX1) | %% trung |
|---|---|---|---|---|
| `activitysys/activity.lua` | IDENTICAL | 324 | 324 | 100 |
| `activitysys/activitydetail.lua` | IDENTICAL | 186 | 186 | 100 |
| `activitysys/functionlib.lua` | IDENTICAL | 387 | 387 | 100 |
| `activitysys/g_activity.lua` | IDENTICAL | 119 | 119 | 100 |
| `activitysys/ladderfunlib.lua` | IDENTICAL | 69 | 69 | 100 |
| `activitysys/playerfunlib.lua` | IDENTICAL | 763 | 763 | 100 |
| `class/ktabfile.lua` | MISSING | 156 | - | 0 |
| `class/lerror.lua` | MISSING | 82 | - | 0 |
| `dailogsys/dailogsay.lua` | IDENTICAL | 181 | 181 | 100 |
| `event/birthday_jieri/200905/chuangguan/chuangguan.lua` | MISSING | 65 | - | 0 |
| `event/birthday_jieri/200905/class.lua` | IDENTICAL | 105 | 105 | 100 |
| `event/birthday_jieri/200905/taskctrl.lua` | IDENTICAL | 230 | 230 | 100 |
| `event/change_destiny/head.lua` | MISSING | 159 | - | 0 |
| `event/change_destiny/mission.lua` | MISSING | 111 | - | 0 |
| `global/itemset.lua` | IDENTICAL | 911 | 911 | 100 |
| `item/battles/songjinskill.lua` | MISSING | 211 | - | 0 |
| `item/class/virtualitem.lua` | IDENTICAL | 51 | 51 | 100 |
| `lib/awardtemplet.lua` | NEAR | 61 | 68 | 94.6 |
| `lib/baseclass.lua` | IDENTICAL | 55 | 55 | 100 |
| `lib/common.lua` | IDENTICAL | 171 | 171 | 100 |
| `lib/composeex.lua` | IDENTICAL | 483 | 483 | 100 |
| `lib/coordinate.lua` | MISSING | 252 | - | 0 |
| `lib/droptemplet.lua` | MISSING | 196 | - | 0 |
| `lib/file.lua` | MISSING | 66 | - | 0 |
| `lib/gb_taskfuncs.lua` | IDENTICAL | 59 | 59 | 100 |
| `lib/log.lua` | IDENTICAL | 152 | 152 | 100 |
| `lib/mem.lua` | MISSING | 62 | - | 0 |
| `lib/objbuffer_head.lua` | IDENTICAL | 120 | 120 | 100 |
| `lib/pay.lua` | IDENTICAL | 9 | 9 | 100 |
| `lib/pfunction.lua` | IDENTICAL | 163 | 163 | 100 |
| `lib/sharedata.lua` | MISSING | 15 | - | 0 |
| `lib/string.lua` | MISSING | 277 | - | 0 |
| `misc/eventsys/eventsys.lua` | **DIVERGED** | 73 | 107 | 81.1 |
| `misc/eventsys/type/player.lua` | IDENTICAL | 18 | 18 | 100 |
| `misc/taskmanager.lua` | IDENTICAL | 393 | 393 | 100 |
| `missions/basemission/lib.lua` | IDENTICAL | 76 | 76 | 100 |
| `missions/boss/bigboss.lua` | MISSING | 295 | - | 0 |
| `missions/challengeoftime/include.lua` | MISSING | 397 | - | 0 |
| `missions/challengeoftime/npc/dragonboat_main.lua` | MISSING | 230 | - | 0 |
| `missions/challengeoftime/rank_perday.lua` | MISSING | 95 | - | 0 |
| `protocol.lua` | IDENTICAL | 136 | 136 | 100 |
| `script_protocol/protocol_def_gs.lua` | IDENTICAL | 195 | 195 | 100 |
| `task/metempsychosis/task_func.lua` | MISSING | 220 | - | 0 |
| `task/metempsychosis/task_head.lua` | IDENTICAL | 155 | 155 | 100 |
| `task/newtask/newtask_head.lua` | **DIVERGED** | 54 | 89 | 75.5 |
| `task/system/task_string.lua` | IDENTICAL | 188 | 188 | 100 |
| `task/task_addplayerexp.lua` | IDENTICAL | 135 | 135 | 100 |
| `task/task_award_extend.lua` | IDENTICAL | 7 | 7 | 100 |
| `task/tollgate/addtollgatenpc.lua` | IDENTICAL | 21 | 21 | 100 |
| `task/tollgate/killbosshead.lua` | IDENTICAL | 3421 | 3421 | 100 |
| `task/tollgate/killer/kill_level.lua` | MISSING | 123 | - | 0 |
| `task/tollgate/killer/lib_killlevel.lua` | MISSING | 148 | - | 0 |
| `task/tollgate/killer/mibao_head.lua` | MISSING | 23 | - | 0 |
| `task/tollgate/killer/nieshichen.lua` | MISSING | 233 | - | 0 |
| `task/tollgate/killer/shashou_mibao.lua` | MISSING | 29 | - | 0 |
| `tong/contribution_entry.lua` | MISSING | 197 | - | 0 |
| `tong/log.lua` | MISSING | 54 | - | 0 |
| `tong/tong_award_head.lua` | MISSING | 186 | - | 0 |
| `tong/tong_header.lua` | MISSING | 171 | - | 0 |
| `tong/tong_setting.lua` | MISSING | 167 | - | 0 |
| `vng_feature/challengeoftime/npcNhiepThiTran.lua` | MISSING | 94 | - | 0 |
| `vng_lib/VngTransLog.lua` | IDENTICAL | 39 | 39 | 100 |
| `vng_lib/bittask_lib.lua` | IDENTICAL | 46 | 46 | 100 |
| `vng_lib/extpoint.lua` | IDENTICAL | 65 | 65 | 100 |
| `vng_lib/extpoint_head.lua` | IDENTICAL | 80 | 80 | 100 |
| `vng_lib/files_lib.lua` | IDENTICAL | 99 | 99 | 100 |
| `vng_lib/taskweekly_lib.lua` | IDENTICAL | 33 | 33 | 100 |

## phonglangdo

| Tep | Trang thai | Dong (Linux) | Dong (JX1) | %% trung |
|---|---|---|---|---|
| `activitysys/activity.lua` | IDENTICAL | 324 | 324 | 100 |
| `activitysys/activitydetail.lua` | IDENTICAL | 186 | 186 | 100 |
| `activitysys/config/32/dailytask.lua` | MISSING | 109 | - | 0 |
| `activitysys/config/32/head.lua` | MISSING | 10 | - | 0 |
| `activitysys/config/32/talkdailytask.lua` | MISSING | 185 | - | 0 |
| `activitysys/config/32/variables.lua` | MISSING | 17 | - | 0 |
| `activitysys/functionlib.lua` | IDENTICAL | 387 | 387 | 100 |
| `activitysys/g_activity.lua` | IDENTICAL | 119 | 119 | 100 |
| `activitysys/ladderfunlib.lua` | IDENTICAL | 69 | 69 | 100 |
| `activitysys/playerfunlib.lua` | IDENTICAL | 763 | 763 | 100 |
| `battles/battle_rank_award.lua` | MISSING | 104 | - | 0 |
| `battles/battlehead.lua` | MISSING | 1347 | - | 0 |
| `battles/doubleexp.lua` | MISSING | 17 | - | 0 |
| `battles/lang.lua` | MISSING | 5 | - | 0 |
| `battles/weeklyrank.lua` | MISSING | 209 | - | 0 |
| `bonusvlmc/head.lua` | MISSING | 18 | - | 0 |
| `dailogsys/dailogsay.lua` | IDENTICAL | 181 | 181 | 100 |
| `event/change_destiny/head.lua` | MISSING | 159 | - | 0 |
| `event/change_destiny/mission.lua` | MISSING | 111 | - | 0 |
| `event/great_night/huangzhizhang/event.lua` | IDENTICAL | 100 | 100 | 100 |
| `event/jiefang_jieri/200904/qianqiu_yinglie/head.lua` | MISSING | 305 | - | 0 |
| `event/jiefang_jieri/200904/shuizei/shuizei.lua` | MISSING | 110 | - | 0 |
| `event/jiefang_jieri/200904/taskctrl.lua` | MISSING | 53 | - | 0 |
| `event/storm/custom.lua` | IDENTICAL | 215 | 215 | 100 |
| `event/storm/function.lua` | IDENTICAL | 451 | 451 | 100 |
| `event/storm/head.lua` | IDENTICAL | 248 | 248 | 100 |
| `global/itemset.lua` | IDENTICAL | 911 | 911 | 100 |
| `global/judgeoffline_limit.lua` | MISSING | 462 | - | 0 |
| `global/signet_head.lua` | MISSING | 30 | - | 0 |
| `global/Â·ÈË_Àñ¹Ù.lua` | KHONG CO O BAN LINUX | - | - | - |
| `item/battles/songjinskill.lua` | MISSING | 211 | - | 0 |
| `item/class/virtualitem.lua` | IDENTICAL | 51 | 51 | 100 |
| `lib/awardtemplet.lua` | NEAR | 61 | 68 | 94.6 |
| `lib/baseclass.lua` | IDENTICAL | 55 | 55 | 100 |
| `lib/basic.lua` | MISSING | 153 | - | 0 |
| `lib/common.lua` | IDENTICAL | 171 | 171 | 100 |
| `lib/composeclass.lua` | MISSING | 326 | - | 0 |
| `lib/composeex.lua` | IDENTICAL | 483 | 483 | 100 |
| `lib/composelistclass.lua` | MISSING | 47 | - | 0 |
| `lib/coordinate.lua` | MISSING | 252 | - | 0 |
| `lib/droptemplet.lua` | MISSING | 196 | - | 0 |
| `lib/file.lua` | MISSING | 66 | - | 0 |
| `lib/gb_taskfuncs.lua` | IDENTICAL | 59 | 59 | 100 |
| `lib/log.lua` | IDENTICAL | 152 | 152 | 100 |
| `lib/objbuffer_head.lua` | IDENTICAL | 120 | 120 | 100 |
| `lib/pay.lua` | IDENTICAL | 9 | 9 | 100 |
| `lib/pfunction.lua` | IDENTICAL | 163 | 163 | 100 |
| `lib/progressbar.lua` | MISSING | 123 | - | 0 |
| `lib/sharedata.lua` | MISSING | 15 | - | 0 |
| `lib/string.lua` | MISSING | 277 | - | 0 |
| `misc/eventsys/eventsys.lua` | **DIVERGED** | 73 | 107 | 81.1 |
| `misc/eventsys/type/func.lua` | IDENTICAL | 109 | 109 | 100 |
| `misc/eventsys/type/player.lua` | IDENTICAL | 18 | 18 | 100 |
| `misc/taskmanager.lua` | IDENTICAL | 393 | 393 | 100 |
| `misc/vngpromotion/ipbonus/ipbonus_2_head.lua` | MISSING | 195 | - | 0 |
| `missions/basemission/lib.lua` | IDENTICAL | 76 | 76 | 100 |
| `missions/boss/bigboss.lua` | MISSING | 295 | - | 0 |
| `missions/fengling_ferry/bigbossdeath.lua` | MISSING | 7 | - | 0 |
| `missions/fengling_ferry/boss.lua` | MISSING | 84 | - | 0 |
| `missions/fengling_ferry/bossdeath.lua` | MISSING | 50 | - | 0 |
| `missions/fengling_ferry/fld_death.lua` | MISSING | 36 | - | 0 |
| `missions/fengling_ferry/fld_head.lua` | MISSING | 321 | - | 0 |
| `missions/fengling_ferry/fld_landingtimer.lua` | MISSING | 4 | - | 0 |
| `missions/fengling_ferry/fld_smalltimer.lua` | MISSING | 88 | - | 0 |
| `missions/fengling_ferry/fldmap_boat1.lua` | MISSING | 30 | - | 0 |
| `missions/fengling_ferry/fldmap_boat2.lua` | MISSING | 17 | - | 0 |
| `missions/fengling_ferry/fldmap_boat3.lua` | MISSING | 17 | - | 0 |
| `missions/fengling_ferry/lang.lua` | MISSING | 2 | - | 0 |
| `missions/fengling_ferry/mission.lua` | MISSING | 153 | - | 0 |
| `missions/fengling_ferry/shuizeideath.lua` | MISSING | 23 | - | 0 |
| `protocol.lua` | IDENTICAL | 136 | 136 | 100 |
| `script_protocol/protocol_def_gs.lua` | IDENTICAL | 195 | 195 | 100 |
| `task/metempsychosis/task_func.lua` | MISSING | 220 | - | 0 |
| `task/metempsychosis/task_head.lua` | IDENTICAL | 155 | 155 | 100 |
| `task/system/task_string.lua` | IDENTICAL | 188 | 188 | 100 |
| `task/task_addplayerexp.lua` | IDENTICAL | 135 | 135 | 100 |
| `vng_lib/VngTransLog.lua` | IDENTICAL | 39 | 39 | 100 |
| `vng_lib/bittask_lib.lua` | IDENTICAL | 46 | 46 | 100 |
| `vng_lib/extpoint.lua` | IDENTICAL | 65 | 65 | 100 |
| `vng_lib/extpoint_head.lua` | IDENTICAL | 80 | 80 | 100 |
| `vng_lib/files_lib.lua` | IDENTICAL | 99 | 99 | 100 |
| `vng_lib/taskweekly_lib.lua` | IDENTICAL | 33 | 33 | 100 |

## vuotai

| Tep | Trang thai | Dong (Linux) | Dong (JX1) | %% trung |
|---|---|---|---|---|
| `activitysys/activity.lua` | IDENTICAL | 324 | 324 | 100 |
| `activitysys/activitydetail.lua` | IDENTICAL | 186 | 186 | 100 |
| `activitysys/config/1005/check_func.lua` | MISSING | 37 | - | 0 |
| `activitysys/config/1005/partysupport.lua` | MISSING | 55 | - | 0 |
| `activitysys/config/32/dailytask.lua` | MISSING | 109 | - | 0 |
| `activitysys/config/32/head.lua` | MISSING | 10 | - | 0 |
| `activitysys/config/32/talkdailytask.lua` | MISSING | 185 | - | 0 |
| `activitysys/config/32/variables.lua` | MISSING | 17 | - | 0 |
| `activitysys/functionlib.lua` | IDENTICAL | 387 | 387 | 100 |
| `activitysys/g_activity.lua` | IDENTICAL | 119 | 119 | 100 |
| `activitysys/ladderfunlib.lua` | IDENTICAL | 69 | 69 | 100 |
| `activitysys/playerfunlib.lua` | IDENTICAL | 763 | 763 | 100 |
| `battles/battle_rank_award.lua` | MISSING | 104 | - | 0 |
| `battles/battlehead.lua` | MISSING | 1347 | - | 0 |
| `battles/battleinfo.lua` | MISSING | 192 | - | 0 |
| `battles/doubleexp.lua` | MISSING | 17 | - | 0 |
| `battles/lang.lua` | MISSING | 5 | - | 0 |
| `battles/weeklyrank.lua` | MISSING | 209 | - | 0 |
| `bonusvlmc/head.lua` | MISSING | 18 | - | 0 |
| `dailogsys/dailog.lua` | IDENTICAL | 111 | 111 | 100 |
| `dailogsys/dailogsay.lua` | IDENTICAL | 181 | 181 | 100 |
| `dailogsys/g_dialog.lua` | IDENTICAL | 42 | 42 | 100 |
| `event/birthday_jieri/200905/class.lua` | IDENTICAL | 105 | 105 | 100 |
| `event/birthday_jieri/200905/taskctrl.lua` | IDENTICAL | 230 | 230 | 100 |
| `event/change_destiny/head.lua` | MISSING | 159 | - | 0 |
| `event/change_destiny/mission.lua` | MISSING | 111 | - | 0 |
| `event/great_night/huangzhizhang/event.lua` | IDENTICAL | 100 | 100 | 100 |
| `event/jiefang_jieri/200904/qianqiu_yinglie/head.lua` | MISSING | 305 | - | 0 |
| `event/storm/custom.lua` | IDENTICAL | 215 | 215 | 100 |
| `event/storm/function.lua` | IDENTICAL | 451 | 451 | 100 |
| `event/storm/head.lua` | IDENTICAL | 248 | 248 | 100 |
| `global/forbidmap.lua` | IDENTICAL | 117 | 117 | 100 |
| `global/itemset.lua` | IDENTICAL | 911 | 911 | 100 |
| `item/battles/songjinskill.lua` | MISSING | 211 | - | 0 |
| `item/class/virtualitem.lua` | IDENTICAL | 51 | 51 | 100 |
| `item/forbiditem.lua` | IDENTICAL | 166 | 166 | 100 |
| `item/heart_head.lua` | **DIVERGED** | 290 | 131 | 27.6 |
| `lib/awardtemplet.lua` | NEAR | 61 | 68 | 94.6 |
| `lib/baseclass.lua` | IDENTICAL | 55 | 55 | 100 |
| `lib/basic.lua` | MISSING | 153 | - | 0 |
| `lib/common.lua` | IDENTICAL | 171 | 171 | 100 |
| `lib/composeex.lua` | IDENTICAL | 483 | 483 | 100 |
| `lib/coordinate.lua` | MISSING | 252 | - | 0 |
| `lib/droptemplet.lua` | MISSING | 196 | - | 0 |
| `lib/file.lua` | MISSING | 66 | - | 0 |
| `lib/gb_taskfuncs.lua` | IDENTICAL | 59 | 59 | 100 |
| `lib/log.lua` | IDENTICAL | 152 | 152 | 100 |
| `lib/objbuffer_head.lua` | IDENTICAL | 120 | 120 | 100 |
| `lib/pay.lua` | IDENTICAL | 9 | 9 | 100 |
| `lib/pfunction.lua` | IDENTICAL | 163 | 163 | 100 |
| `lib/sharedata.lua` | MISSING | 15 | - | 0 |
| `lib/string.lua` | MISSING | 277 | - | 0 |
| `misc/eventsys/eventsys.lua` | **DIVERGED** | 73 | 107 | 81.1 |
| `misc/eventsys/type/func.lua` | IDENTICAL | 109 | 109 | 100 |
| `misc/eventsys/type/harvestplant.lua` | IDENTICAL | 3 | 3 | 100 |
| `misc/eventsys/type/npc.lua` | IDENTICAL | 77 | 77 | 100 |
| `misc/eventsys/type/player.lua` | IDENTICAL | 18 | 18 | 100 |
| `misc/taskmanager.lua` | IDENTICAL | 393 | 393 | 100 |
| `misc/vngpromotion/ipbonus/ipbonus_2_head.lua` | MISSING | 195 | - | 0 |
| `missions/basemission/lib.lua` | IDENTICAL | 76 | 76 | 100 |
| `missions/boss/bigboss.lua` | MISSING | 295 | - | 0 |
| `missions/challengeoftime/award.lua` | MISSING | 284 | - | 0 |
| `missions/challengeoftime/chuangguang30.lua` | MISSING | 276 | - | 0 |
| `missions/challengeoftime/chuangguang30_playerdeath.lua` | MISSING | 6 | - | 0 |
| `missions/challengeoftime/doubleexp.lua` | MISSING | 17 | - | 0 |
| `missions/challengeoftime/include.lua` | MISSING | 397 | - | 0 |
| `missions/challengeoftime/item/chuangguanbaoxiang.lua` | MISSING | 172 | - | 0 |
| `missions/challengeoftime/mission_match.lua` | MISSING | 101 | - | 0 |
| `missions/challengeoftime/npc.lua` | MISSING | 502 | - | 0 |
| `missions/challengeoftime/npc/dragonboat_main.lua` | MISSING | 230 | - | 0 |
| `missions/challengeoftime/npc/transfer.lua` | MISSING | 57 | - | 0 |
| `missions/challengeoftime/npc_death.lua` | MISSING | 164 | - | 0 |
| `missions/challengeoftime/player_death.lua` | MISSING | 35 | - | 0 |
| `missions/challengeoftime/rank_perday.lua` | MISSING | 95 | - | 0 |
| `missions/challengeoftime/timer_board.lua` | MISSING | 24 | - | 0 |
| `missions/challengeoftime/timer_close.lua` | MISSING | 20 | - | 0 |
| `missions/challengeoftime/timer_match.lua` | MISSING | 68 | - | 0 |
| `missions/yandibaozang/head.lua` | MISSING | 215 | - | 0 |
| `protocol.lua` | IDENTICAL | 136 | 136 | 100 |
| `script_protocol/protocol_def_gs.lua` | IDENTICAL | 195 | 195 | 100 |
| `task/metempsychosis/lang.lua` | MISSING | 44 | - | 0 |
| `task/metempsychosis/task_func.lua` | MISSING | 220 | - | 0 |
| `task/metempsychosis/task_head.lua` | IDENTICAL | 155 | 155 | 100 |
| `task/metempsychosis/translife_5.lua` | MISSING | 230 | - | 0 |
| `task/metempsychosis/translife_6.lua` | MISSING | 310 | - | 0 |
| `task/system/task_string.lua` | IDENTICAL | 188 | 188 | 100 |
| `task/task_addplayerexp.lua` | IDENTICAL | 135 | 135 | 100 |
| `task/task_award_extend.lua` | IDENTICAL | 7 | 7 | 100 |
| `tong/contribution_entry.lua` | MISSING | 197 | - | 0 |
| `tong/log.lua` | MISSING | 54 | - | 0 |
| `tong/tong_award_head.lua` | MISSING | 186 | - | 0 |
| `tong/tong_header.lua` | MISSING | 171 | - | 0 |
| `tong/tong_setting.lua` | MISSING | 167 | - | 0 |
| `vng_event/change_request_baoruong/exp_award.lua` | IDENTICAL | 110 | 110 | 100 |
| `vng_event/thapnienlenhbai/lenhbai_def.lua` | MISSING | 577 | - | 0 |
| `vng_event/thapnienlenhbai/mainfuc.lua` | MISSING | 164 | - | 0 |
| `vng_feature/forbiditem/vngforbidspecialitem.lua` | MISSING | 165 | - | 0 |
| `vng_lib/VngTransLog.lua` | IDENTICAL | 39 | 39 | 100 |
| `vng_lib/bittask_lib.lua` | IDENTICAL | 46 | 46 | 100 |
| `vng_lib/extpoint.lua` | IDENTICAL | 65 | 65 | 100 |
| `vng_lib/extpoint_head.lua` | IDENTICAL | 80 | 80 | 100 |
| `vng_lib/files_lib.lua` | IDENTICAL | 99 | 99 | 100 |
| `vng_lib/taskweekly_lib.lua` | IDENTICAL | 33 | 33 | 100 |

## Tong ket

| Trang thai | So tep |
|---|---|
| MISSING | 125 |
| IDENTICAL | 118 |
| DIVERGED | 5 |
| NEAR | 3 |
| KHONG CO O BAN LINUX | 1 |
