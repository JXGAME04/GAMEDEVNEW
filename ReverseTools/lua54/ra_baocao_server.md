# Ra cay script E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script - 2986 tep, 315228 dong

## 1. Goi ham Lua 4 cu (qua shim lua4compat) - tong theo ten

| nhom | ham | so lan goi | so tep |
|---|---|---:|---:|
| bang | getn | 1503 | 548 |
| bang | tinsert | 426 | 105 |
| bang | tremove | 3 | 3 |
| bang | sort | 3 | 3 |
| chuoi | strfind | 140 | 62 |
| chuoi | strsub | 150 | 59 |
| chuoi | strlen | 25 | 16 |
| chuoi | strlower | 1 | 1 |
| chuoi | strbyte | 1 | 1 |
| chuoi | gsub | 22 | 8 |
| chuoi | format | 1350 | 295 |
| toan | floor | 3233 | 372 |
| toan | ceil | 24 | 18 |
| toan | abs | 20 | 6 |
| toan | sqrt | 5 | 4 |
| toan | max | 1 | 1 |
| toan | min | 2 | 2 |
| toan | mod | 213 | 98 |
| toan | random | 742 | 263 |
| os | date | 206 | 64 |
| dong | dostring | 8 | 5 |
| dong | call | 34 | 18 |
| dong | dofile | 65 | 63 |

Tong goi cu: 8177 trong 897 tep.

## 2. Tep TU DINH NGHIA ham trung ten shim (doi ten may moc phai TRANH cac tep nay)

- `min`: 1 tep: missions/challengeoftime/award.lua
- `sqrt`: 1 tep: tinhnang/pubg/pubg.lua

## 2b. [LUA54] GAN LAI BIEN DIEU KHIEN VONG FOR - loi im lang (Lua 4 co tac dung, 5.4 khong): 0 cho


## 3. Mau hieu nang thap - tong

- while ... getn(...) (goi getn moi vong): **11** cho / 5 tep
- dostring( (bien dich moi lan goi): **8** cho / 5 tep
- Include( trong than ham (nap lai tep moi lan): **1** cho / 1 tep
- x = x .. y trong vong lap (O(n^2)): **74** cho / 37 tep
- ham > 300 dong: **0** cho / 0 tep
- true/false dung lam bien: **0** cho / 0 tep
- dung 'arg' (vararg cu): **109** cho / 30 tep
- tep > 3000 dong: codenew.lua(32001), giftcode_fancung.lua(9002), giftcode_new.lua(9005), startgame/npcpos.lua(27305), task/tollgate/killbosshead.lua(3421)

## 4. Top 30 tep theo RUI RO HIEU NANG

| tep | dong | while_getn | dostring | Include/ham | concat/lap | ham>300 | diem |
|---|---:|---:|---:|---:|---:|---:|---:|
| test/bangthanh_f.lua | 1343 | 1 | 0 | 0 | 9 | 0 | 23 |
| activitysys/functionlib.lua | 395 | 0 | 4 | 1 | 0 | 0 | 20 |
| auction_house/auction_manager.lua | 1536 | 3 | 0 | 0 | 2 | 0 | 19 |
| item/test_phiphong_admin.lua | 597 | 3 | 0 | 0 | 1 | 0 | 17 |
| lib/lib_ham.lua | 397 | 0 | 1 | 0 | 5 | 0 | 14 |
| missions/leaguematch/npc/officer.lua | 615 | 0 | 0 | 0 | 6 | 0 | 12 |
| global/seasonnpc.lua | 1440 | 2 | 0 | 0 | 0 | 0 | 10 |
| test/citywar_e3.lua | 247 | 0 | 0 | 0 | 5 | 0 | 10 |
| test/event_rank.lua | 246 | 2 | 0 | 0 | 0 | 0 | 10 |
| missions/leaguematch/npc/helper.lua | 468 | 0 | 0 | 0 | 4 | 0 | 8 |
| lib/common.lua | 171 | 0 | 0 | 0 | 3 | 0 | 6 |
| lib/composeclass.lua | 381 | 0 | 1 | 0 | 1 | 0 | 6 |
| partner/partner_test_bdh.lua | 340 | 0 | 1 | 0 | 1 | 0 | 6 |
| event/equip_publish/wuxingyin/wuxingyin.lua | 307 | 0 | 0 | 0 | 2 | 0 | 4 |
| global/mantlesystem/mantleupgrade_head.lua | 1348 | 0 | 0 | 0 | 2 | 0 | 4 |
| item/liendau_admin.lua | 210 | 0 | 0 | 0 | 2 | 0 | 4 |
| item/test_hoatdong_admin.lua | 608 | 0 | 0 | 0 | 2 | 0 | 4 |
| missions/citywar_city/timer.lua | 32 | 0 | 0 | 0 | 2 | 0 | 4 |
| missions/citywar_city/totaltimer.lua | 32 | 0 | 0 | 0 | 2 | 0 | 4 |
| missions/yandibaozang/saizi.lua | 120 | 0 | 0 | 0 | 2 | 0 | 4 |
| petsys/petequip.lua | 209 | 0 | 0 | 0 | 2 | 0 | 4 |
| task/system/task_string.lua | 188 | 0 | 1 | 0 | 0 | 0 | 4 |
| tinhnang/tong_kim_tcap/mobinhtk.lua | 456 | 0 | 0 | 0 | 2 | 0 | 4 |
| vng_event/thapnienlenhbai/mainfuc.lua | 164 | 0 | 0 | 0 | 2 | 0 | 4 |
| codenew.lua | 32001 | 0 | 0 | 0 | 0 | 0 | 2 |
| giftcode_fancung.lua | 9002 | 0 | 0 | 0 | 0 | 0 | 2 |
| giftcode_new.lua | 9005 | 0 | 0 | 0 | 0 | 0 | 2 |
| activitysys/ladderfunlib.lua | 69 | 0 | 0 | 0 | 1 | 0 | 2 |
| battles/weeklyrank.lua | 209 | 0 | 0 | 0 | 1 | 0 | 2 |
| event/tongwar/head.lua | 806 | 0 | 0 | 0 | 1 | 0 | 2 |

## 5. Top 30 tep theo SO GOI HAM CU (no hien dai hoa)

| tep | dong | goi cu | chi tiet |
|---|---:|---:|---|
| npclevelscript/supernpc.lua | 627 | 99 | floor:95, strfind:2, strsub:2 |
| item/ib/shenxingfu.lua | 1399 | 91 | getn:56, tinsert:16, random:10, floor:6, format:3 |
| auction_house/auction_manager.lua | 1536 | 87 | floor:27, getn:20, format:15, tinsert:7, mod:5, call:4 |
| battles/battlehead.lua | 1347 | 85 | floor:41, getn:16, random:9, strsub:4, date:4, format:3 |
| item/simcity_admin.lua | 761 | 78 | format:41, random:22, tinsert:12, getn:3 |
| npclevelscript/soldier2.lua | 661 | 77 | floor:72, strfind:2, strsub:2, getn:1 |
| npclevelscript/soldier3.lua | 661 | 77 | floor:72, strfind:2, strsub:2, getn:1 |
| npclevelscript/soldier4.lua | 661 | 77 | floor:72, strfind:2, strsub:2, getn:1 |
| npclevelscript/standard.lua | 821 | 75 | floor:70, strfind:2, strsub:2, getn:1 |
| npclevelscript/newnpc.lua | 687 | 74 | floor:69, strfind:2, strsub:2, getn:1 |
| npclevelscript/npc_mogaoku.lua | 682 | 74 | floor:69, strfind:2, strsub:2, getn:1 |
| npclevelscript/npc_xiaoxiao.lua | 682 | 74 | floor:69, strfind:2, strsub:2, getn:1 |
| npclevelscript/tasknpc.lua | 752 | 74 | floor:69, strfind:2, strsub:2, getn:1 |
| event/tongwar/head.lua | 806 | 73 | format:28, getn:22, tinsert:17, mod:3, floor:2, sort:1 |
| npclevelscript/called_soldier1.lua | 700 | 72 | floor:67, strfind:2, strsub:2, getn:1 |
| npclevelscript/soldier1.lua | 659 | 70 | floor:65, strfind:2, strsub:2, getn:1 |
| timerserver.lua | 833 | 69 | getn:24, format:14, strsub:7, mod:6, floor:6, tinsert:4 |
| mail/mailmanager.lua | 702 | 67 | tinsert:19, format:17, getn:14, floor:7, strfind:5, strsub:4 |
| npclevelscript/animal_userdef.lua | 679 | 67 | floor:62, strfind:2, strsub:2, getn:1 |
| npclevelscript/npc_fenglingdunan.lua | 677 | 67 | floor:62, strfind:2, strsub:2, getn:1 |
| npclevelscript/task_goldennpc.lua | 674 | 67 | floor:62, strfind:2, strsub:2, getn:1 |
| npclevelscript/task_normalnpc.lua | 677 | 67 | floor:62, strfind:2, strsub:2, getn:1 |
| npclevelscript/animal.lua | 853 | 66 | floor:61, strfind:2, strsub:2, getn:1 |
| npclevelscript/dragonboat_long.lua | 659 | 66 | floor:61, strfind:2, strsub:2, getn:1 |
| npclevelscript/dragonboat_random.lua | 674 | 66 | floor:61, strfind:2, strsub:2, getn:1 |
| npclevelscript/dragonboat_short.lua | 659 | 66 | floor:61, strfind:2, strsub:2, getn:1 |
| npclevelscript/randomtask_long.lua | 659 | 66 | floor:61, strfind:2, strsub:2, getn:1 |
| npclevelscript/randomtask_random.lua | 674 | 66 | floor:61, strfind:2, strsub:2, getn:1 |
| npclevelscript/randomtask_short.lua | 659 | 66 | floor:61, strfind:2, strsub:2, getn:1 |
| item/lenhbaitanthu.lua | 1444 | 65 | format:30, getn:13, tinsert:7, strsub:7, date:4, dofile:1 |

## 6. Tep can xem tay

- **while getn** (5 tep): auction_house/auction_manager.lua(3), global/seasonnpc.lua(2), item/test_phiphong_admin.lua(3), test/bangthanh_f.lua(1), test/event_rank.lua(2)
- **dostring** (5 tep): activitysys/functionlib.lua(4), lib/composeclass.lua(1), lib/lib_ham.lua(1), partner/partner_test_bdh.lua(1), task/system/task_string.lua(1)
- **Include trong ham** (1 tep): activitysys/functionlib.lua(1)
