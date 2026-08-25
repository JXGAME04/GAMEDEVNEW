# C34 - AddItem thieu tham so (toan cay script)

JX1 `LuaAddItem`: `if (nParamNum < 7) return 0;` => **khong tao item, im lang**.

Tong **48** cho / **21** tep.


## battles\battlehead.lua
- dong 1309 (6 tham so): `local ItemIdx = AddItem(6,1,JUNGONGPAI,1,0,0)`

## event\birthday_jieri\200905\chuangguan\chuangguan.lua
- dong 59 (6 tham so): `AddItem(6, 1, 2079, 1, 0, 0);`

## event\birthday_jieri\200905\class.lua
- dong 94 (6 tham so): `local n_itemidx = AddItem(6,1,2067,1,0,0);`

## event\jiaoshi_jieri\200910\head.lua
- dong 20 (6 tham so): `local n_itemidx = AddItem(6,1,2166,1,0,0);`

## event\jiefang_jieri\200904\qianqiu_yinglie\head.lua
- dong 183 (6 tham so): `local n_itemidx = AddItem(0,11,446,1,1,0);`
- dong 198 (6 tham so): `local n_itemidx = AddItem(6, 1, 2014, 1, 1, 0);`
- dong 267 (6 tham so): `local n_itemidx = AddItem(0,11,447,1,1,0);`

## global\npcchucnang\nhieptran.lua
- dong 122 (6 tham so): `AddItem(ITEM_STL[1],ITEM_STL[2],ITEM_STL[3],10,i,0)`
- dong 128 (6 tham so): `AddItem(ITEM_STG[1],ITEM_STG[2],ITEM_STG[3],10,i,0)`
- dong 159 (6 tham so): `AddItem(ITEM_STG[1],ITEM_STG[2],ITEM_STG[3],10,random(0,4),0)`

## global\ÌØÊâÓÃµØ\ËÎ½ð±¨Ãûµã\npc\head.lua
- dong 26 (6 tham so): `AddItem(6, 1, 2622, 1, 0, 0)`

## item\hd3_admin.lua
- dong 33 (6 tham so): `for i = 1, 5 do AddItem(6, 1, 398, 90, mod(i, 5), 0) end`
- dong 37 (6 tham so): `AddItem(6, 1, 399, 90, 0, 0)`
- dong 38 (6 tham so): `AddItem(6, 1, 399, 90, 1, 0)`
- dong 84 (6 tham so): `function HD3_ADM_PLD_LB() AddItem(4, 489, 0, 0, 0, 0) Msg2Player("§· n`
- dong 85 (6 tham so): `function HD3_ADM_PLD_LBTT() AddItem(6, 1, 3363, 0, 0, 0) Msg2Player("§`

## item\lenhbaiadmin.lua
- dong 325 (6 tham so): `AddItem(6,1,157,0,0,0)`

## item\tuitanthu.lua
- dong 17 (6 tham so): `local nIndex=AddItem(6,1,2433,1,0,0)`

## missions\citywar_city\camper.lua
- dong 85 (6 tham so): `AddItem(6,1,1076,1,1,0);`

## missions\citywar_global\citywar_function.lua
- dong 46 (6 tham so): `AddItem(6,1,TheItem,1,0,0);`
- dong 154 (6 tham so): `AddItem(6,1,146,5,1,1);`
- dong 191 (6 tham so): `AddItem(6,1,146,5,1,1);`

## startgame\tinhnang\tongkim\head.lua
- dong 26 (6 tham so): `AddItem(6, 1, 2622, 1, 0, 0)`

## task\tollgate\messenger\shanshenmiao\enemy_temrefresh.lua
- dong 37 (0 tham so): `AddItem()`

## tinhnang\congthanhchien\lib_ctc.lua
- dong 352 (6 tham so): `AddItem(6,1,1075,1,1,0);`

## tinhnang\phonglangdo\thuyenphu.lua
- dong 30 (6 tham so): `AddItem(ITEM_LBPLD[1],ITEM_LBPLD[2],ITEM_LBPLD[3],0,0,0)`
- dong 31 (6 tham so): `AddItem(ITEM_LBTT[1],ITEM_LBTT[2],ITEM_LBTT[3],0,0,0)`
- dong 33 (6 tham so): `AddItem(ITEM_MDTB[1],ITEM_MDTB[2],ITEM_MDTB[3],0,0,0)`

## tinhnang\phonglangdo\thuyenphubac.lua
- dong 54 (6 tham so): `AddItem(6, 1, 71, 0, 0, 0)`

## tinhnang\tong_kim_tcap\quanquan.lua
- dong 152 (6 tham so): `AddItem(ITEM_NVKIEM[1],ITEM_NVKIEM[2],ITEM_NVKIEM[3],0,0,0)`

## tinhnang\vuot_ai\sugiasatthu.lua
- dong 125 (6 tham so): `AddItem(ITEM_STL[1],ITEM_STL[2],ITEM_STL[3],10,i,0)`
- dong 131 (6 tham so): `AddItem(ITEM_STG[1],ITEM_STG[2],ITEM_STG[3],10,i,0)`
- dong 162 (6 tham so): `AddItem(ITEM_STG[1],ITEM_STG[2],ITEM_STG[3],10,random(0,4),0)`

## Î÷ÄÏ±±Çø\³É¶¼\³É¶¼\Â·ÈËnpc\Î÷ÄÏ±±Çø-³É¶¼¸®-Â·ÈË23ÂäÆÇ×ÓµÜ¶Ô»°.lua
- dong 37 (6 tham so): `AddItem(0, 0, n, k, m, 1)`
- dong 42 (6 tham so): `AddItem(0, 1, n, k, m, 1)`
- dong 47 (6 tham so): `AddItem(0, 2, n, k, m, 1)`
- dong 51 (6 tham so): `AddItem(0, 3, 0, k, m, 1)`
- dong 56 (6 tham so): `AddItem(0, 4, n, k, m, 1)`
- dong 61 (6 tham so): `AddItem(0, 5, n, k, m, 1)`
- dong 66 (6 tham so): `AddItem(0, 6, n, k, m, 1)`
- dong 71 (6 tham so): `AddItem(0, 7, n, k, m, 1)`
- dong 76 (6 tham so): `AddItem(0, 8, n, k, m, 1)`
- dong 81 (6 tham so): `AddItem(0, 9, n, k, m, 1)`

## Î÷ÄÏ±±Çø\½­½ò´å\Ö°ÄÜnpc\ÔÂÀÏ.lua
- dong 315 (6 tham so): `AddItem(6,1,147,6,0,0);`
- dong 319 (6 tham so): `AddItem(6,1,147,7,0,0);`
- dong 323 (6 tham so): `AddItem(6,1,147,8,0,0);`
- dong 327 (6 tham so): `AddItem(6,1,147,9,0,0);`
- dong 331 (6 tham so): `AddItem(6,1,398,9,0,0);`
