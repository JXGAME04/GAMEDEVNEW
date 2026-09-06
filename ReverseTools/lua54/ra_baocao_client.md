# Ra cay script E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\script - 449 tep, 85425 dong

## 1. Goi ham Lua 4 cu (qua shim lua4compat) - tong theo ten

| nhom | ham | so lan goi | so tep |
|---|---|---:|---:|
| bang | getn | 314 | 209 |
| bang | tinsert | 112 | 26 |
| bang | tremove | 1 | 1 |
| bang | sort | 3 | 1 |
| chuoi | strfind | 117 | 51 |
| chuoi | strsub | 104 | 49 |
| chuoi | strlen | 21 | 9 |
| chuoi | gsub | 6 | 3 |
| chuoi | format | 206 | 53 |
| toan | floor | 2777 | 210 |
| toan | ceil | 6 | 6 |
| toan | abs | 2 | 1 |
| toan | min | 1 | 1 |
| toan | mod | 18 | 10 |
| toan | random | 82 | 14 |
| os | date | 9 | 5 |
| dong | dostring | 6 | 4 |
| dong | call | 21 | 12 |
| dong | dofile | 2 | 1 |

Tong goi cu: 3808 trong 260 tep.


## 3. Mau hieu nang thap - tong

- while ... getn(...) (goi getn moi vong): **0** cho / 0 tep
- dostring( (bien dich moi lan goi): **6** cho / 4 tep
- Include( trong than ham (nap lai tep moi lan): **1** cho / 1 tep
- x = x .. y trong vong lap (O(n^2)): **21** cho / 12 tep
- ham > 300 dong: **0** cho / 0 tep
- true/false dung lam bien: **0** cho / 0 tep
- dung 'arg' (vararg cu): **80** cho / 17 tep
- tep > 3000 dong: 

## 4. Top 12 tep theo RUI RO HIEU NANG

| tep | dong | while_getn | dostring | Include/ham | concat/lap | ham>300 | diem |
|---|---:|---:|---:|---:|---:|---:|---:|
| lib/say.lua | 175 | 0 | 1 | 0 | 5 | 0 | 14 |
| lib/debug.lua | 338 | 0 | 2 | 0 | 2 | 0 | 12 |
| lib/simulator.lua | 86 | 0 | 2 | 0 | 0 | 0 | 8 |
| lib/common.lua | 171 | 0 | 0 | 0 | 3 | 0 | 6 |
| lib/compose.lua | 185 | 0 | 0 | 0 | 2 | 0 | 4 |
| lib/composeclass.lua | 326 | 0 | 1 | 0 | 0 | 0 | 4 |
| lib/include.lua | 30 | 0 | 0 | 1 | 0 | 0 | 4 |
| ui/uimail.lua | 650 | 0 | 0 | 0 | 2 | 0 | 4 |
| lib/basic.lua | 153 | 0 | 0 | 0 | 1 | 0 | 2 |
| lib/string.lua | 277 | 0 | 0 | 0 | 1 | 0 | 2 |
| mail/maildef.lua | 73 | 0 | 0 | 0 | 1 | 0 | 2 |
| tong/log.lua | 50 | 0 | 0 | 0 | 1 | 0 | 2 |

## 5. Top 12 tep theo SO GOI HAM CU (no hien dai hoa)

| tep | dong | goi cu | chi tiet |
|---|---:|---:|---|
| npclevelscript/supernpc.lua | 627 | 99 | floor:95, strfind:2, strsub:2 |
| npclevelscript/soldier2.lua | 661 | 77 | floor:72, strfind:2, strsub:2, getn:1 |
| npclevelscript/soldier3.lua | 661 | 77 | floor:72, strfind:2, strsub:2, getn:1 |
| npclevelscript/soldier4.lua | 661 | 77 | floor:72, strfind:2, strsub:2, getn:1 |
| npclevelscript/standard.lua | 821 | 75 | floor:70, strfind:2, strsub:2, getn:1 |
| npclevelscript/newnpc.lua | 687 | 74 | floor:69, strfind:2, strsub:2, getn:1 |
| npclevelscript/npc_mogaoku.lua | 682 | 74 | floor:69, strfind:2, strsub:2, getn:1 |
| npclevelscript/npc_xiaoxiao.lua | 682 | 74 | floor:69, strfind:2, strsub:2, getn:1 |
| npclevelscript/tasknpc.lua | 752 | 74 | floor:69, strfind:2, strsub:2, getn:1 |
| npclevelscript/called_soldier1.lua | 700 | 72 | floor:67, strfind:2, strsub:2, getn:1 |
| npclevelscript/soldier1.lua | 659 | 70 | floor:65, strfind:2, strsub:2, getn:1 |
| npclevelscript/animal_userdef.lua | 679 | 67 | floor:62, strfind:2, strsub:2, getn:1 |

## 6. Tep can xem tay

- **dostring** (4 tep): lib/composeclass.lua(1), lib/debug.lua(2), lib/say.lua(1), lib/simulator.lua(2)
- **Include trong ham** (1 tep): lib/include.lua(1)
