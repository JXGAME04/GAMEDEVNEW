# Ban da giai ma (UTF-8) cua ma nguon Lua ban Linux

Nguon goc: `D:\ServerLinux\server1` (Lua 4.0, tep goc tron GBK + TCVN3).
Moi tep giu NGUYEN SO DONG so voi tep goc -> trich dan `tep.lua:123` dung cho ca hai ban.

## satthu

| # | Tep (tuong doi voi script\) | Byte | Dong | Ma hoa goc | JX1 da co |
|---|---|---|---|---|---|
| 1 | `activitysys/activity.lua` | 8685 | 324 | gbk | co |
| 2 | `activitysys/activitydetail.lua` | 4276 | 186 | gbk | co |
| 3 | `activitysys/functionlib.lua` | 10164 | 387 | ascii | co |
| 4 | `activitysys/g_activity.lua` | 2613 | 119 | ascii | co |
| 5 | `activitysys/ladderfunlib.lua` | 1940 | 69 | tcvn3 | co |
| 6 | `activitysys/playerfunlib.lua` | 18746 | 763 | gbk | co |
| 7 | `class/ktabfile.lua` | 4047 | 156 | gbk | **CHUA CO** |
| 8 | `class/lerror.lua` | 1984 | 82 | gbk | **CHUA CO** |
| 9 | `dailogsys/dailogsay.lua` | 4810 | 181 | gbk | co |
| 10 | `event/birthday_jieri/200905/chuangguan/chuangguan.lua` | 2281 | 65 | gbk | **CHUA CO** |
| 11 | `event/birthday_jieri/200905/class.lua` | 2427 | 105 | tcvn3 | co |
| 12 | `event/birthday_jieri/200905/taskctrl.lua` | 6309 | 230 | gbk | co |
| 13 | `event/change_destiny/head.lua` | 5376 | 159 | gbk | **CHUA CO** |
| 14 | `event/change_destiny/mission.lua` | 3290 | 111 | gbk | **CHUA CO** |
| 15 | `global/itemset.lua` | 68641 | 911 | gbk | co |
| 16 | `item/battles/songjinskill.lua` | 7444 | 211 | gbk | **CHUA CO** |
| 17 | `item/class/virtualitem.lua` | 1221 | 51 | ascii | co |
| 18 | `lib/awardtemplet.lua` | 1392 | 61 | gbk | co |
| 19 | `lib/baseclass.lua` | 841 | 55 | ascii | co |
| 20 | `lib/common.lua` | 3475 | 171 | gbk | co |
| 21 | `lib/composeex.lua` | 14325 | 483 | gbk | co |
| 22 | `lib/coordinate.lua` | 6830 | 252 | gbk | **CHUA CO** |
| 23 | `lib/droptemplet.lua` | 5768 | 196 | gbk | **CHUA CO** |
| 24 | `lib/file.lua` | 1694 | 66 | gbk | **CHUA CO** |
| 25 | `lib/gb_taskfuncs.lua` | 2274 | 59 | gbk | co |
| 26 | `lib/log.lua` | 4318 | 152 | gbk | co |
| 27 | `lib/mem.lua` | 1112 | 62 | gbk | **CHUA CO** |
| 28 | `lib/objbuffer_head.lua` | 2602 | 120 | gbk | co |
| 29 | `lib/pay.lua` | 121 | 9 | gbk | co |
| 30 | `lib/pfunction.lua` | 3488 | 163 | gbk | co |
| 31 | `lib/sharedata.lua` | 561 | 15 | ascii | **CHUA CO** |
| 32 | `lib/string.lua` | 6965 | 277 | gbk | **CHUA CO** |
| 33 | `misc/eventsys/eventsys.lua` | 1634 | 73 | gbk | co |
| 34 | `misc/eventsys/type/player.lua` | 476 | 18 | ascii | co |
| 35 | `misc/taskmanager.lua` | 12799 | 393 | gbk | co |
| 36 | `missions/basemission/lib.lua` | 1883 | 76 | gbk | co |
| 37 | `missions/boss/bigboss.lua` | 13447 | 295 | gbk | **CHUA CO** |
| 38 | `missions/challengeoftime/include.lua` | 10875 | 397 | gbk | **CHUA CO** |
| 39 | `missions/challengeoftime/npc/dragonboat_main.lua` | 8225 | 230 | gbk | **CHUA CO** |
| 40 | `missions/challengeoftime/rank_perday.lua` | 3213 | 95 | gbk | **CHUA CO** |
| 41 | `protocol.lua` | 4024 | 136 | gbk | co |
| 42 | `script_protocol/protocol_def_gs.lua` | 4792 | 195 | gbk | co |
| 43 | `task/metempsychosis/task_func.lua` | 7385 | 220 | gbk | **CHUA CO** |
| 44 | `task/metempsychosis/task_head.lua` | 7475 | 155 | gbk | co |
| 45 | `task/newtask/newtask_head.lua` | 1851 | 54 | gbk | co |
| 46 | `task/system/task_string.lua` | 4561 | 188 | gbk | co |
| 47 | `task/task_addplayerexp.lua` | 3175 | 135 | gbk | co |
| 48 | `task/task_award_extend.lua` | 182 | 7 | gbk | co |
| 49 | `task/tollgate/addtollgatenpc.lua` | 713 | 21 | gbk | co |
| 50 | `task/tollgate/killbosshead.lua` | 384819 | 3421 | gbk | co |
| 51 | `task/tollgate/killer/kill_level.lua` | 6079 | 123 | gbk | **CHUA CO** |
| 52 | `task/tollgate/killer/lib_killlevel.lua` | 4824 | 148 | gbk | **CHUA CO** |
| 53 | `task/tollgate/killer/mibao_head.lua` | 1182 | 23 | gbk | **CHUA CO** |
| 54 | `task/tollgate/killer/nieshichen.lua` | 10956 | 233 | gbk | **CHUA CO** |
| 55 | `task/tollgate/killer/shashou_mibao.lua` | 974 | 29 | gbk | **CHUA CO** |
| 56 | `tong/contribution_entry.lua` | 7488 | 197 | gbk | **CHUA CO** |
| 57 | `tong/log.lua` | 1651 | 54 | gbk | **CHUA CO** |
| 58 | `tong/tong_award_head.lua` | 4595 | 186 | gbk | **CHUA CO** |
| 59 | `tong/tong_header.lua` | 6722 | 171 | gbk | **CHUA CO** |
| 60 | `tong/tong_setting.lua` | 5610 | 167 | gbk | **CHUA CO** |
| 61 | `vng_feature/challengeoftime/npcNhiepThiTran.lua` | 2994 | 94 | tcvn3 | **CHUA CO** |
| 62 | `vng_lib/VngTransLog.lua` | 1007 | 39 | tcvn3 | co |
| 63 | `vng_lib/bittask_lib.lua` | 1389 | 46 | ascii | co |
| 64 | `vng_lib/extpoint.lua` | 1959 | 65 | tcvn3 | co |
| 65 | `vng_lib/extpoint_head.lua` | 2871 | 80 | tcvn3 | co |
| 66 | `vng_lib/files_lib.lua` | 2329 | 99 | tcvn3 | co |
| 67 | `vng_lib/taskweekly_lib.lua` | 1029 | 33 | tcvn3 | co |

## phonglangdo

| # | Tep (tuong doi voi script\) | Byte | Dong | Ma hoa goc | JX1 da co |
|---|---|---|---|---|---|
| 1 | `activitysys/activity.lua` | 8685 | 324 | gbk | co |
| 2 | `activitysys/activitydetail.lua` | 4276 | 186 | gbk | co |
| 3 | `activitysys/config/32/dailytask.lua` | 3044 | 109 | gbk | **CHUA CO** |
| 4 | `activitysys/config/32/head.lua` | 290 | 10 | tcvn3 | **CHUA CO** |
| 5 | `activitysys/config/32/talkdailytask.lua` | 6677 | 185 | tcvn3 | **CHUA CO** |
| 6 | `activitysys/config/32/variables.lua` | 787 | 17 | tcvn3 | **CHUA CO** |
| 7 | `activitysys/functionlib.lua` | 10164 | 387 | ascii | co |
| 8 | `activitysys/g_activity.lua` | 2613 | 119 | ascii | co |
| 9 | `activitysys/ladderfunlib.lua` | 1940 | 69 | tcvn3 | co |
| 10 | `activitysys/playerfunlib.lua` | 18746 | 763 | gbk | co |
| 11 | `battles/battle_rank_award.lua` | 3058 | 104 | gbk | **CHUA CO** |
| 12 | `battles/battlehead.lua` | 44633 | 1347 | gbk | **CHUA CO** |
| 13 | `battles/doubleexp.lua` | 640 | 17 | gbk | **CHUA CO** |
| 14 | `battles/lang.lua` | 409 | 5 | tcvn3 | **CHUA CO** |
| 15 | `battles/weeklyrank.lua` | 5711 | 209 | gbk | **CHUA CO** |
| 16 | `bonusvlmc/head.lua` | 354 | 18 | tcvn3 | **CHUA CO** |
| 17 | `dailogsys/dailogsay.lua` | 4810 | 181 | gbk | co |
| 18 | `event/change_destiny/head.lua` | 5376 | 159 | gbk | **CHUA CO** |
| 19 | `event/change_destiny/mission.lua` | 3290 | 111 | gbk | **CHUA CO** |
| 20 | `event/great_night/huangzhizhang/event.lua` | 2969 | 100 | gbk | co |
| 21 | `event/jiefang_jieri/200904/qianqiu_yinglie/head.lua` | 8251 | 305 | gbk | **CHUA CO** |
| 22 | `event/jiefang_jieri/200904/shuizei/shuizei.lua` | 3751 | 110 | tcvn3 | **CHUA CO** |
| 23 | `event/jiefang_jieri/200904/taskctrl.lua` | 1973 | 53 | gbk | **CHUA CO** |
| 24 | `event/storm/custom.lua` | 8331 | 215 | gbk | co |
| 25 | `event/storm/function.lua` | 16531 | 451 | gbk | co |
| 26 | `event/storm/head.lua` | 8610 | 248 | gbk | co |
| 27 | `global/itemset.lua` | 68641 | 911 | gbk | co |
| 28 | `global/judgeoffline_limit.lua` | 10015 | 462 | gbk | **CHUA CO** |
| 29 | `global/signet_head.lua` | 799 | 30 | gbk | **CHUA CO** |
| - | `global/Â·ÈË_Àñ¹Ù.lua` | - | - | - | **KHONG CO TRONG BAN LINUX** |
| 30 | `item/battles/songjinskill.lua` | 7444 | 211 | gbk | **CHUA CO** |
| 31 | `item/class/virtualitem.lua` | 1221 | 51 | ascii | co |
| 32 | `lib/awardtemplet.lua` | 1392 | 61 | gbk | co |
| 33 | `lib/baseclass.lua` | 841 | 55 | ascii | co |
| 34 | `lib/basic.lua` | 3518 | 153 | gbk | **CHUA CO** |
| 35 | `lib/common.lua` | 3475 | 171 | gbk | co |
| 36 | `lib/composeclass.lua` | 9758 | 326 | gbk | **CHUA CO** |
| 37 | `lib/composeex.lua` | 14325 | 483 | gbk | co |
| 38 | `lib/composelistclass.lua` | 1160 | 47 | ascii | **CHUA CO** |
| 39 | `lib/coordinate.lua` | 6830 | 252 | gbk | **CHUA CO** |
| 40 | `lib/droptemplet.lua` | 5768 | 196 | gbk | **CHUA CO** |
| 41 | `lib/file.lua` | 1694 | 66 | gbk | **CHUA CO** |
| 42 | `lib/gb_taskfuncs.lua` | 2274 | 59 | gbk | co |
| 43 | `lib/log.lua` | 4318 | 152 | gbk | co |
| 44 | `lib/objbuffer_head.lua` | 2602 | 120 | gbk | co |
| 45 | `lib/pay.lua` | 121 | 9 | gbk | co |
| 46 | `lib/pfunction.lua` | 3488 | 163 | gbk | co |
| 47 | `lib/progressbar.lua` | 3436 | 123 | gbk | **CHUA CO** |
| 48 | `lib/sharedata.lua` | 561 | 15 | ascii | **CHUA CO** |
| 49 | `lib/string.lua` | 6965 | 277 | gbk | **CHUA CO** |
| 50 | `misc/eventsys/eventsys.lua` | 1634 | 73 | gbk | co |
| 51 | `misc/eventsys/type/func.lua` | 1988 | 109 | gbk | co |
| 52 | `misc/eventsys/type/player.lua` | 476 | 18 | ascii | co |
| 53 | `misc/taskmanager.lua` | 12799 | 393 | gbk | co |
| 54 | `misc/vngpromotion/ipbonus/ipbonus_2_head.lua` | 5216 | 195 | tcvn3 | **CHUA CO** |
| 55 | `missions/basemission/lib.lua` | 1883 | 76 | gbk | co |
| 56 | `missions/boss/bigboss.lua` | 13447 | 295 | gbk | **CHUA CO** |
| 57 | `missions/fengling_ferry/bigbossdeath.lua` | 176 | 7 | ascii | **CHUA CO** |
| 58 | `missions/fengling_ferry/boss.lua` | 2290 | 84 | gbk | **CHUA CO** |
| 59 | `missions/fengling_ferry/bossdeath.lua` | 1917 | 50 | gbk | **CHUA CO** |
| 60 | `missions/fengling_ferry/fld_death.lua` | 980 | 36 | gbk | **CHUA CO** |
| 61 | `missions/fengling_ferry/fld_head.lua` | 10386 | 321 | gbk | **CHUA CO** |
| 62 | `missions/fengling_ferry/fld_landingtimer.lua` | 110 | 4 | ascii | **CHUA CO** |
| 63 | `missions/fengling_ferry/fld_smalltimer.lua` | 3063 | 88 | gbk | **CHUA CO** |
| 64 | `missions/fengling_ferry/fldmap_boat1.lua` | 1118 | 30 | gbk | **CHUA CO** |
| 65 | `missions/fengling_ferry/fldmap_boat2.lua` | 621 | 17 | tcvn3 | **CHUA CO** |
| 66 | `missions/fengling_ferry/fldmap_boat3.lua` | 626 | 17 | tcvn3 | **CHUA CO** |
| 67 | `missions/fengling_ferry/lang.lua` | 190 | 2 | tcvn3 | **CHUA CO** |
| 68 | `missions/fengling_ferry/mission.lua` | 4191 | 153 | gbk | **CHUA CO** |
| 69 | `missions/fengling_ferry/shuizeideath.lua` | 782 | 23 | ascii | **CHUA CO** |
| 70 | `protocol.lua` | 4024 | 136 | gbk | co |
| 71 | `script_protocol/protocol_def_gs.lua` | 4792 | 195 | gbk | co |
| 72 | `task/metempsychosis/task_func.lua` | 7385 | 220 | gbk | **CHUA CO** |
| 73 | `task/metempsychosis/task_head.lua` | 7475 | 155 | gbk | co |
| 74 | `task/system/task_string.lua` | 4561 | 188 | gbk | co |
| 75 | `task/task_addplayerexp.lua` | 3175 | 135 | gbk | co |
| 76 | `vng_lib/VngTransLog.lua` | 1007 | 39 | tcvn3 | co |
| 77 | `vng_lib/bittask_lib.lua` | 1389 | 46 | ascii | co |
| 78 | `vng_lib/extpoint.lua` | 1959 | 65 | tcvn3 | co |
| 79 | `vng_lib/extpoint_head.lua` | 2871 | 80 | tcvn3 | co |
| 80 | `vng_lib/files_lib.lua` | 2329 | 99 | tcvn3 | co |
| 81 | `vng_lib/taskweekly_lib.lua` | 1029 | 33 | tcvn3 | co |

## vuotai

| # | Tep (tuong doi voi script\) | Byte | Dong | Ma hoa goc | JX1 da co |
|---|---|---|---|---|---|
| 1 | `activitysys/activity.lua` | 8685 | 324 | gbk | co |
| 2 | `activitysys/activitydetail.lua` | 4276 | 186 | gbk | co |
| 3 | `activitysys/config/1005/check_func.lua` | 1162 | 37 | tcvn3 | **CHUA CO** |
| 4 | `activitysys/config/1005/partysupport.lua` | 1784 | 55 | tcvn3 | **CHUA CO** |
| 5 | `activitysys/config/32/dailytask.lua` | 3044 | 109 | gbk | **CHUA CO** |
| 6 | `activitysys/config/32/head.lua` | 290 | 10 | tcvn3 | **CHUA CO** |
| 7 | `activitysys/config/32/talkdailytask.lua` | 6677 | 185 | tcvn3 | **CHUA CO** |
| 8 | `activitysys/config/32/variables.lua` | 787 | 17 | tcvn3 | **CHUA CO** |
| 9 | `activitysys/functionlib.lua` | 10164 | 387 | ascii | co |
| 10 | `activitysys/g_activity.lua` | 2613 | 119 | ascii | co |
| 11 | `activitysys/ladderfunlib.lua` | 1940 | 69 | tcvn3 | co |
| 12 | `activitysys/playerfunlib.lua` | 18746 | 763 | gbk | co |
| 13 | `battles/battle_rank_award.lua` | 3058 | 104 | gbk | **CHUA CO** |
| 14 | `battles/battlehead.lua` | 44633 | 1347 | gbk | **CHUA CO** |
| 15 | `battles/battleinfo.lua` | 11813 | 192 | gbk | **CHUA CO** |
| 16 | `battles/doubleexp.lua` | 640 | 17 | gbk | **CHUA CO** |
| 17 | `battles/lang.lua` | 409 | 5 | tcvn3 | **CHUA CO** |
| 18 | `battles/weeklyrank.lua` | 5711 | 209 | gbk | **CHUA CO** |
| 19 | `bonusvlmc/head.lua` | 354 | 18 | tcvn3 | **CHUA CO** |
| 20 | `dailogsys/dailog.lua` | 2602 | 111 | tcvn3 | co |
| 21 | `dailogsys/dailogsay.lua` | 4810 | 181 | gbk | co |
| 22 | `dailogsys/g_dialog.lua` | 951 | 42 | gbk | co |
| 23 | `event/birthday_jieri/200905/class.lua` | 2427 | 105 | tcvn3 | co |
| 24 | `event/birthday_jieri/200905/taskctrl.lua` | 6309 | 230 | gbk | co |
| 25 | `event/change_destiny/head.lua` | 5376 | 159 | gbk | **CHUA CO** |
| 26 | `event/change_destiny/mission.lua` | 3290 | 111 | gbk | **CHUA CO** |
| 27 | `event/great_night/huangzhizhang/event.lua` | 2969 | 100 | gbk | co |
| 28 | `event/jiefang_jieri/200904/qianqiu_yinglie/head.lua` | 8251 | 305 | gbk | **CHUA CO** |
| 29 | `event/storm/custom.lua` | 8331 | 215 | gbk | co |
| 30 | `event/storm/function.lua` | 16531 | 451 | gbk | co |
| 31 | `event/storm/head.lua` | 8610 | 248 | gbk | co |
| 32 | `global/forbidmap.lua` | 3087 | 117 | gbk | co |
| 33 | `global/itemset.lua` | 68641 | 911 | gbk | co |
| 34 | `item/battles/songjinskill.lua` | 7444 | 211 | gbk | **CHUA CO** |
| 35 | `item/class/virtualitem.lua` | 1221 | 51 | ascii | co |
| 36 | `item/forbiditem.lua` | 4543 | 166 | gbk | co |
| 37 | `item/heart_head.lua` | 9613 | 290 | gbk | co |
| 38 | `lib/awardtemplet.lua` | 1392 | 61 | gbk | co |
| 39 | `lib/baseclass.lua` | 841 | 55 | ascii | co |
| 40 | `lib/basic.lua` | 3518 | 153 | gbk | **CHUA CO** |
| 41 | `lib/common.lua` | 3475 | 171 | gbk | co |
| 42 | `lib/composeex.lua` | 14325 | 483 | gbk | co |
| 43 | `lib/coordinate.lua` | 6830 | 252 | gbk | **CHUA CO** |
| 44 | `lib/droptemplet.lua` | 5768 | 196 | gbk | **CHUA CO** |
| 45 | `lib/file.lua` | 1694 | 66 | gbk | **CHUA CO** |
| 46 | `lib/gb_taskfuncs.lua` | 2274 | 59 | gbk | co |
| 47 | `lib/log.lua` | 4318 | 152 | gbk | co |
| 48 | `lib/objbuffer_head.lua` | 2602 | 120 | gbk | co |
| 49 | `lib/pay.lua` | 121 | 9 | gbk | co |
| 50 | `lib/pfunction.lua` | 3488 | 163 | gbk | co |
| 51 | `lib/sharedata.lua` | 561 | 15 | ascii | **CHUA CO** |
| 52 | `lib/string.lua` | 6965 | 277 | gbk | **CHUA CO** |
| 53 | `misc/eventsys/eventsys.lua` | 1634 | 73 | gbk | co |
| 54 | `misc/eventsys/type/func.lua` | 1988 | 109 | gbk | co |
| 55 | `misc/eventsys/type/harvestplant.lua` | 106 | 3 | ascii | co |
| 56 | `misc/eventsys/type/npc.lua` | 2091 | 77 | tcvn3 | co |
| 57 | `misc/eventsys/type/player.lua` | 476 | 18 | ascii | co |
| 58 | `misc/taskmanager.lua` | 12799 | 393 | gbk | co |
| 59 | `misc/vngpromotion/ipbonus/ipbonus_2_head.lua` | 5216 | 195 | tcvn3 | **CHUA CO** |
| 60 | `missions/basemission/lib.lua` | 1883 | 76 | gbk | co |
| 61 | `missions/boss/bigboss.lua` | 13447 | 295 | gbk | **CHUA CO** |
| 62 | `missions/challengeoftime/award.lua` | 10052 | 284 | gbk | **CHUA CO** |
| 63 | `missions/challengeoftime/chuangguang30.lua` | 7980 | 276 | gbk | **CHUA CO** |
| 64 | `missions/challengeoftime/chuangguang30_playerdeath.lua` | 145 | 6 | tcvn3 | **CHUA CO** |
| 65 | `missions/challengeoftime/doubleexp.lua` | 657 | 17 | gbk | **CHUA CO** |
| 66 | `missions/challengeoftime/include.lua` | 10875 | 397 | gbk | **CHUA CO** |
| 67 | `missions/challengeoftime/item/chuangguanbaoxiang.lua` | 6477 | 172 | gbk | **CHUA CO** |
| 68 | `missions/challengeoftime/mission_match.lua` | 2406 | 101 | gbk | **CHUA CO** |
| 69 | `missions/challengeoftime/npc.lua` | 18610 | 502 | gbk | **CHUA CO** |
| 70 | `missions/challengeoftime/npc/dragonboat_main.lua` | 8225 | 230 | gbk | **CHUA CO** |
| 71 | `missions/challengeoftime/npc/transfer.lua` | 1929 | 57 | gbk | **CHUA CO** |
| 72 | `missions/challengeoftime/npc_death.lua` | 5134 | 164 | gbk | **CHUA CO** |
| 73 | `missions/challengeoftime/player_death.lua` | 819 | 35 | gbk | **CHUA CO** |
| 74 | `missions/challengeoftime/rank_perday.lua` | 3213 | 95 | gbk | **CHUA CO** |
| 75 | `missions/challengeoftime/timer_board.lua` | 669 | 24 | gbk | **CHUA CO** |
| 76 | `missions/challengeoftime/timer_close.lua` | 431 | 20 | gbk | **CHUA CO** |
| 77 | `missions/challengeoftime/timer_match.lua` | 1614 | 68 | gbk | **CHUA CO** |
| 78 | `missions/yandibaozang/head.lua` | 7649 | 215 | gbk | **CHUA CO** |
| 79 | `protocol.lua` | 4024 | 136 | gbk | co |
| 80 | `script_protocol/protocol_def_gs.lua` | 4792 | 195 | gbk | co |
| 81 | `task/metempsychosis/lang.lua` | 2894 | 44 | tcvn3 | **CHUA CO** |
| 82 | `task/metempsychosis/task_func.lua` | 7385 | 220 | gbk | **CHUA CO** |
| 83 | `task/metempsychosis/task_head.lua` | 7475 | 155 | gbk | co |
| 84 | `task/metempsychosis/translife_5.lua` | 8440 | 230 | gbk | **CHUA CO** |
| 85 | `task/metempsychosis/translife_6.lua` | 9985 | 310 | ascii | **CHUA CO** |
| 86 | `task/system/task_string.lua` | 4561 | 188 | gbk | co |
| 87 | `task/task_addplayerexp.lua` | 3175 | 135 | gbk | co |
| 88 | `task/task_award_extend.lua` | 182 | 7 | gbk | co |
| 89 | `tong/contribution_entry.lua` | 7488 | 197 | gbk | **CHUA CO** |
| 90 | `tong/log.lua` | 1651 | 54 | gbk | **CHUA CO** |
| 91 | `tong/tong_award_head.lua` | 4595 | 186 | gbk | **CHUA CO** |
| 92 | `tong/tong_header.lua` | 6722 | 171 | gbk | **CHUA CO** |
| 93 | `tong/tong_setting.lua` | 5610 | 167 | gbk | **CHUA CO** |
| 94 | `vng_event/change_request_baoruong/exp_award.lua` | 3166 | 110 | tcvn3 | co |
| 95 | `vng_event/thapnienlenhbai/lenhbai_def.lua` | 28426 | 577 | tcvn3 | **CHUA CO** |
| 96 | `vng_event/thapnienlenhbai/mainfuc.lua` | 6513 | 164 | tcvn3 | **CHUA CO** |
| 97 | `vng_feature/forbiditem/vngforbidspecialitem.lua` | 9859 | 165 | tcvn3 | **CHUA CO** |
| 98 | `vng_lib/VngTransLog.lua` | 1007 | 39 | tcvn3 | co |
| 99 | `vng_lib/bittask_lib.lua` | 1389 | 46 | ascii | co |
| 100 | `vng_lib/extpoint.lua` | 1959 | 65 | tcvn3 | co |
| 101 | `vng_lib/extpoint_head.lua` | 2871 | 80 | tcvn3 | co |
| 102 | `vng_lib/files_lib.lua` | 2329 | 99 | tcvn3 | co |
| 103 | `vng_lib/taskweekly_lib.lua` | 1029 | 33 | tcvn3 | co |

## _settings (bang du lieu ngoai)

| Tep | Byte | Dong | Ma hoa goc |
|---|---|---|---|
| `settings/killer.ini` | 492 | 16 | gbk |
| `settings/goldboss.txt` | 3124 | 34 | tcvn3 |
| `settings/trigger_challengeoftime.lua` | 704 | 24 | gbk |
| `settings/maplist.ini` | 193907 | 6371 | gbk |
| `settings/map_type.txt` | 1869 | 35 | gbk |
| `settings/cavelist.ini` | 25968 | 1353 | gbk |
| `settings/dungeonmap.ini` | 37 | 4 | ascii |
| `settings/huoyuedu/huoyuedu.txt` | 1865 | 43 | gbk |
| `settings/systemtimetask.txt` | 24 | 2 | ascii |
| `settings/revivepos.ini` | 6810 | 645 | ascii |
