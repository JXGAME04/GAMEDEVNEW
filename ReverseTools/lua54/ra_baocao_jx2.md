# Ra cay script E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\scriptjx2 - 189 tep, 26802 dong

## 1. Goi ham Lua 4 cu (qua shim lua4compat) - tong theo ten

| nhom | ham | so lan goi | so tep |
|---|---|---:|---:|
| bang | getn | 189 | 60 |
| bang | tinsert | 153 | 34 |
| bang | tremove | 1 | 1 |
| bang | sort | 3 | 1 |
| chuoi | strfind | 34 | 11 |
| chuoi | strsub | 28 | 10 |
| chuoi | strlen | 21 | 9 |
| chuoi | gsub | 4 | 2 |
| chuoi | format | 175 | 50 |
| toan | floor | 159 | 40 |
| toan | ceil | 9 | 9 |
| toan | abs | 4 | 2 |
| toan | min | 1 | 1 |
| toan | mod | 26 | 14 |
| toan | random | 151 | 18 |
| os | date | 18 | 10 |
| dong | dostring | 6 | 4 |
| dong | call | 21 | 12 |
| dong | dofile | 2 | 1 |

Tong goi cu: 1005 trong 86 tep.


## 3. Mau hieu nang thap - tong

- while ... getn(...) (goi getn moi vong): **0** cho / 0 tep
- dostring( (bien dich moi lan goi): **6** cho / 4 tep
- Include( trong than ham (nap lai tep moi lan): **1** cho / 1 tep
- x = x .. y trong vong lap (O(n^2)): **24** cho / 14 tep
- ham > 300 dong: **0** cho / 0 tep
- true/false dung lam bien: **0** cho / 0 tep
- dung 'arg' (vararg cu): **83** cho / 17 tep
- tep > 3000 dong: 

## 4. Top 12 tep theo RUI RO HIEU NANG

| tep | dong | while_getn | dostring | Include/ham | concat/lap | ham>300 | diem |
|---|---:|---:|---:|---:|---:|---:|---:|
| lib/say.lua | 186 | 0 | 1 | 0 | 5 | 0 | 14 |
| lib/debug.lua | 338 | 0 | 2 | 0 | 2 | 0 | 12 |
| lib/simulator.lua | 86 | 0 | 2 | 0 | 0 | 0 | 8 |
| lib/common.lua | 171 | 0 | 0 | 0 | 3 | 0 | 6 |
| lib/basic.lua | 153 | 0 | 0 | 0 | 2 | 0 | 4 |
| lib/compose.lua | 185 | 0 | 0 | 0 | 2 | 0 | 4 |
| lib/composeclass.lua | 326 | 0 | 1 | 0 | 0 | 0 | 4 |
| lib/include.lua | 30 | 0 | 0 | 1 | 0 | 0 | 4 |
| lib/string.lua | 288 | 0 | 0 | 0 | 2 | 0 | 4 |
| tong/log.lua | 54 | 0 | 0 | 0 | 1 | 0 | 2 |
| tong/tong_setting.lua | 167 | 0 | 0 | 0 | 1 | 0 | 2 |
| tong/npc/city_totempole.lua | 663 | 0 | 0 | 0 | 1 | 0 | 2 |

## 5. Top 12 tep theo SO GOI HAM CU (no hien dai hoa)

| tep | dong | goi cu | chi tiet |
|---|---:|---:|---|
| tong/npc/city_totempole.lua | 663 | 45 | random:31, getn:6, format:4, tinsert:3, floor:1 |
| tong/npc/tong_totempole.lua | 685 | 45 | random:31, getn:6, format:4, tinsert:3, floor:1 |
| tong_vn/npc/city_totempole.lua | 663 | 45 | random:31, getn:6, format:4, tinsert:3, floor:1 |
| tong_vn/npc/tong_totempole.lua | 685 | 45 | random:31, getn:6, format:4, tinsert:3, floor:1 |
| tong/npc/jitan.lua | 627 | 39 | tinsert:19, floor:8, format:4, mod:4, getn:2, ceil:1 |
| tong_vn/npc/jitan.lua | 627 | 35 | tinsert:15, floor:8, format:4, mod:4, getn:2, ceil:1 |
| lib/composeex.lua | 483 | 29 | format:11, getn:8, floor:4, gsub:2, strfind:1, tinsert:1 |
| tong/workshop/ws_tiangong.lua | 520 | 28 | floor:18, format:4, date:3, mod:2, getn:1 |
| tong_vn/workshop/ws_tiangong.lua | 520 | 28 | floor:18, format:4, date:3, mod:2, getn:1 |
| lib/composeclass.lua | 326 | 25 | format:14, getn:5, floor:3, ceil:1, call:1, dostring:1 |
| lib/pfunction.lua | 163 | 23 | tinsert:5, strsub:5, strfind:4, getn:3, strlen:2, gsub:2 |
| tong/tong_mix.lua | 1215 | 23 | floor:16, abs:2, format:2, getn:2, mod:1 |

## 6. Tep can xem tay

- **dostring** (4 tep): lib/composeclass.lua(1), lib/debug.lua(2), lib/say.lua(1), lib/simulator.lua(2)
- **Include trong ham** (1 tep): lib/include.lua(1)
