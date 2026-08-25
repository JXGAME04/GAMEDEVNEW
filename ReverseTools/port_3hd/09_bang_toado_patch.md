# Bang toa do / bang du lieu LIEN QUAN 3 tinh nang, trich tu goc B
(`D:\ServerLinux\Patch\settings`)

> **DINH CHINH (vong 2).** Ban dau tep nay ghi *"nhung tep nay KHONG co trong server1"*
> cho ca 106 bang — **noi qua**. Bo loc chi chon theo TEN, khong kiem goc A.
> Do lai: **34/106 bang thuc su CHI CO O B**, so con lai co san o goc A
> (phan lon trung byte). Cot `Chi co o B` duoi day la ket qua do that.

Vi du bang CHI CO O B va la du lieu loi: `fld_head.lua:18` doc
`\settings\maps\中原北区\渡船\渡船刷怪点.txt`.
Ban da giai ma UTF-8 nam o `src_utf8/_patch_settings/`.

## Bang CHI CO O goc B (34)

| Tep | Byte | Dong | 3 dong dau |
|---|---|---|---|
| `battles\maps\boss\bossàó2.txt` | 121 | 12 | `XPOS	YPOS / 1720	3046 / 1690	3008` |
| `battles\maps\boss\bossàó4.txt` | 121 | 12 | `XPOS	YPOS / 1966	2701 / 1808	2744` |
| `battles\maps\boss\后营2.txt` | 81 | 7 | `XPOS	YPOS / 52064	101088 / 52160	101408` |
| `battles\maps\boss\后营2trap.txt` | 431 | 32 | `XPOS	YPOS / 53088	100064 / 53120	100096` |
| `battles\maps\boss\后营4.txt` | 76 | 7 | `XPOS	YPOS / 65216	82784 / 65472	82752` |
| `battles\maps\boss\后营4trap.txt` | 336 | 27 | `XPOS	YPOS / 64928	83488 / 64960	83520` |
| `battles\maps\boss\大营2.txt` | 128 | 11 | `XPOS	YPOS / 54464	97216 / 54656	98112` |
| `battles\maps\boss\大营2to后营2trap.txt` | 453 | 36 | `XPOS	YPOS / 54336	98208 / 54368	98208` |
| `battles\maps\boss\大营2to野外trap.txt` | 1025 | 80 | `XPOS	YPOS / 54016	96800 / 54048	96768` |
| `battles\maps\boss\大营2野外.txt` | 2611 | 202 | `XPOS	YPOS / 54016	94464 / 54144	94464` |
| `battles\maps\boss\大营4.txt` | 128 | 11 | `XPOS	YPOS / 62304	86112 / 62176	86560` |
| `battles\maps\boss\大营4to后营4trap.txt` | 557 | 44 | `XPOS	YPOS / 63584	85248 / 63616	85280` |
| `battles\maps\boss\大营4to野外trap.txt` | 1181 | 92 | `XPOS	YPOS / 61856	85856 / 61888	85824` |
| `battles\maps\boss\大营4野外.txt` | 2611 | 202 | `XPOS	YPOS / 57760	87968 / 57760	87712` |
| `battles\maps\boss\帅旗2.txt` | 63 | 6 | `XPOS	YPOS / 54176	97376 / 54560	96896` |
| `battles\maps\boss\帅旗4.txt` | 63 | 6 | `XPOS	YPOS / 61984	86528 / 62464	85856` |
| `maps\great_night\风陵渡.txt` | 1520 | 102 | `map_ID	nPosX	nPosY / 336	1370	3102 / 336	1375	3092` |
| `maps\东北区\漠北草原\qingxiaoshitaiboss.txt` | 121 | 12 | `XPOS	YPOS / 1259	3108 / 1362	3062` |
| `maps\东北区\漠北草原\shansinanboss.txt` | 99 | 10 | `XPOS	YPOS / 1566	2814 / 1573	2962` |
| `maps\东北区\漠北草原\wangzuoboss.txt` | 121 | 12 | `XPOS	YPOS / 1259	3108 / 1362	3062` |
| `maps\东北区\长白山麓\xuanjiziboss.txt` | 252 | 24 | `XPOS	YPOS / 1450	3133 / 1541	2896` |
| `maps\东北区\长白山麓\xuanjuedashiboss.txt` | 295 | 27 | `XPOS	YPOS / 1609	4101 / 1626	3984` |
| `maps\中原北区\渡船\渡船刷怪点.txt` | 893 | 65 | `XPOS	YPOS / 50400	102240 / 50560	102048` |
| `maps\中原北区\风陵渡北岸\baiyingyingboss.txt` | 99 | 10 | `XPOS	YPOS / 1110	3016 / 1110	2920` |
| `maps\中原北区\风陵渡北岸\yanxiaoqianboss.txt` | 99 | 10 | `XPOS	YPOS / 1252	2698 / 1247	2546` |
| `maps\中原北区\风陵渡南岸\herenwoboss.txt` | 143 | 14 | `XPOS	YPOS / 1565	2992 / 1580	3106` |
| `maps\江南区\两水洞迷宫\lanyiyiboss.txt` | 22 | 3 | `XPOS	YPOS / 1502	3101 / ` |
| `maps\江南区\两水洞迷宫\zhonglingxiuboss.txt` | 22 | 3 | `XPOS	YPOS / 1612	2952 / ` |
| `maps\江南区\西山屿\big_goldboss.txt` | 97 | 9 | `XPOS	YPOS / 1248	2912 / 1312	3056` |
| `maps\江南区\西山屿\hanmengboss.txt` | 231 | 22 | `XPOS	YPOS / 1276	2410 / 1375	2425` |
| `maps\西北北区\沙漠迷宫\duanmuruiboss.txt` | 22 | 3 | `XPOS	YPOS / 1724	3354 / ` |
| `maps\西北北区\沙漠迷宫\mengcanglangboss.txt` | 22 | 3 | `XPOS	YPOS / 1668	3145 / ` |
| `maps\西北北区\沙漠迷宫\qinglianziboss.txt` | 22 | 3 | `XPOS	YPOS / 1816	3248 / ` |
| `maps\西北北区\莫高窟\tangburanboss.txt` | 317 | 29 | `XPOS	YPOS / 1314	3331 / 1295	3228` |

## Toan bo 106 bang da trich (ke ca bang co san o goc A)

| Tep | Byte | Dong | Co o goc A | Trung byte |
|---|---|---|---|---|
| `battles\maps\boss\bossàó2.txt` | 121 | 12 | **KHONG** | - |
| `battles\maps\boss\bossàó4.txt` | 121 | 12 | **KHONG** | - |
| `battles\maps\boss\mapinfo.txt` | 3195 | 129 | co | y het |
| `battles\maps\boss\后营2.txt` | 81 | 7 | **KHONG** | - |
| `battles\maps\boss\后营2trap.txt` | 431 | 32 | **KHONG** | - |
| `battles\maps\boss\后营4.txt` | 76 | 7 | **KHONG** | - |
| `battles\maps\boss\后营4trap.txt` | 336 | 27 | **KHONG** | - |
| `battles\maps\boss\大营2.txt` | 128 | 11 | **KHONG** | - |
| `battles\maps\boss\大营2to后营2trap.txt` | 453 | 36 | **KHONG** | - |
| `battles\maps\boss\大营2to野外trap.txt` | 1025 | 80 | **KHONG** | - |
| `battles\maps\boss\大营2野外.txt` | 2611 | 202 | **KHONG** | - |
| `battles\maps\boss\大营4.txt` | 128 | 11 | **KHONG** | - |
| `battles\maps\boss\大营4to后营4trap.txt` | 557 | 44 | **KHONG** | - |
| `battles\maps\boss\大营4to野外trap.txt` | 1181 | 92 | **KHONG** | - |
| `battles\maps\boss\大营4野外.txt` | 2611 | 202 | **KHONG** | - |
| `battles\maps\boss\帅旗2.txt` | 63 | 6 | **KHONG** | - |
| `battles\maps\boss\帅旗4.txt` | 63 | 6 | **KHONG** | - |
| `droprate\boss\aotianjiangjundroprate.ini` | 1519 | 162 | co | y het |
| `droprate\boss\baiyingyingdroprate.ini` | 1833 | 218 | co | y het |
| `droprate\boss\bosstask_lev20.ini` | 6233 | 651 | co | y het |
| `droprate\boss\bosstask_lev30.ini` | 6237 | 651 | co | y het |
| `droprate\boss\bosstask_lev40.ini` | 6252 | 652 | co | y het |
| `droprate\boss\bosstask_lev50.ini` | 6252 | 652 | co | y het |
| `droprate\boss\bosstask_lev60.ini` | 6255 | 653 | co | y het |
| `droprate\boss\bosstask_lev70.ini` | 6251 | 651 | co | y het |
| `droprate\boss\bosstask_lev80.ini` | 6262 | 651 | co | y het |
| `droprate\boss\bosstask_lev90.ini` | 6214 | 645 | co | y het |
| `droprate\boss\damahudroprate.ini` | 2032 | 233 | co | y het |
| `droprate\boss\daoqingzhenrendroprate.ini` | 1704 | 201 | co | y het |
| `droprate\boss\duanmuruidroprate.ini` | 1650 | 196 | co | y het |
| `droprate\boss\gubodroprate.ini` | 1956 | 231 | co | y het |
| `droprate\boss\hanmengdroprate.ini` | 2982 | 347 | co | y het |
| `droprate\boss\heiyishashoudroprate.ini` | 1519 | 162 | co | y het |
| `droprate\boss\helianpiaodroprate.ini` | 1776 | 211 | co | y het |
| `droprate\boss\herenwodroprate.ini` | 1776 | 211 | co | y het |
| `droprate\boss\jingtongdroprate.ini` | 1519 | 162 | co | y het |
| `droprate\boss\lanyiyidroprate.ini` | 1718 | 204 | co | y het |
| `droprate\boss\mengcanglangdroprate.ini` | 1714 | 202 | co | y het |
| `droprate\boss\newboss\huoke.ini` | 538 | 50 | co | y het |
| `droprate\boss\qinglianzidroprate.ini` | 1899 | 201 | co | y het |
| `droprate\boss\qingxiaoshitaidroprate.ini` | 1833 | 218 | co | y het |
| `droprate\boss\qingyizidroprate.ini` | 1519 | 162 | co | y het |
| `droprate\boss\shansinandroprate.ini` | 1640 | 185 | co | y het |
| `droprate\boss\shifangjumiedroprate.ini` | 1519 | 162 | co | y het |
| `droprate\boss\tangburandroprate.ini` | 2087 | 248 | co | y het |
| `droprate\boss\tangfeiyandroprate.ini` | 2140 | 226 | co | y het |
| `droprate\boss\wangzuodroprate.ini` | 1833 | 218 | co | y het |
| `droprate\boss\wanyanxueyidroprate.ini` | 1586 | 178 | co | y het |
| `droprate\boss\xiwangxiedudroprate.ini` | 1519 | 162 | co | y het |
| `droprate\boss\xuanjizidroprate.ini` | 1963 | 233 | co | y het |
| `droprate\boss\xuanjuedashidroprate.ini` | 2080 | 246 | co | y het |
| `droprate\boss\xuannandashidroprate.ini` | 2012 | 212 | co | y het |
| `droprate\boss\xudaiyuedroprate.ini` | 1652 | 175 | co | y het |
| `droprate\boss\yanxiaoqiandroprate.ini` | 1901 | 224 | co | y het |
| `droprate\boss\yelvbilidroprate.ini` | 1722 | 204 | co | y het |
| `droprate\boss\yinqingyundroprate.ini` | 1519 | 162 | co | y het |
| `droprate\boss\zhonglingxiudroprate.ini` | 1776 | 210 | co | y het |
| `droprate\goldennpc\boatboss_droprate.ini` | 1233 | 142 | co | y het |
| `droprate\npcdroprate_changgemenareaboss.ini` | 4228 | 386 | co | y het |
| `droprate\yandibaozang\zengduoboss.ini` | 807 | 78 | co | y het |
| `goldboss.txt` | 3124 | 34 | co | y het |
| `item\heiyishashoudroprate.ini` | 1519 | 162 | co | y het |
| `killer.ini` | 492 | 16 | co | y het |
| `maps\challengeoftime\lineup16.txt` | 187 | 18 | co | y het |
| `maps\challengeoftime\lineup20.txt` | 231 | 22 | co | y het |
| `maps\challengeoftime\lineup24.txt` | 275 | 26 | co | y het |
| `maps\challengeoftime\lineup32.txt` | 363 | 34 | co | y het |
| `maps\challengeoftime\lineup40.txt` | 451 | 42 | co | y het |
| `maps\challengeoftime\lineup56.txt` | 627 | 58 | co | y het |
| `maps\challengeoftime\lineup8.txt` | 99 | 10 | co | y het |
| `maps\citydefence\juniorboss.txt` | 1411 | 102 | co | y het |
| `maps\citydefence\seniorboss.txt` | 151 | 12 | co | y het |
| `maps\damahuboss.txt` | 412 | 24 | co | y het |
| `maps\great_night\风陵渡.txt` | 1520 | 102 | **KHONG** | - |
| `maps\newboss\cuiyanboss.txt` | 44 | 5 | co | y het |
| `maps\newboss\emeiboss.txt` | 44 | 5 | co | y het |
| `maps\newboss\gaibangboss.txt` | 44 | 5 | co | y het |
| `maps\newboss\kunlunboss.txt` | 44 | 5 | co | y het |
| `maps\newboss\shaolinboss.txt` | 44 | 5 | co | y het |
| `maps\newboss\tangmenboss.txt` | 44 | 5 | co | y het |
| `maps\newboss\tianrenboss.txt` | 44 | 5 | co | y het |
| `maps\newboss\tianwangboss.txt` | 44 | 5 | co | y het |
| `maps\newboss\wudangboss.txt` | 44 | 5 | co | y het |
| `maps\newboss\wuduboss.txt` | 44 | 5 | co | y het |
| `maps\tianchimijing\floor4\boss.txt` | 27 | 3 | co | y het |
| `maps\东北区\漠北草原\qingxiaoshitaiboss.txt` | 121 | 12 | **KHONG** | - |
| `maps\东北区\漠北草原\shansinanboss.txt` | 99 | 10 | **KHONG** | - |
| `maps\东北区\漠北草原\wangzuoboss.txt` | 121 | 12 | **KHONG** | - |
| `maps\东北区\长白山麓\xuanjiziboss.txt` | 252 | 24 | **KHONG** | - |
| `maps\东北区\长白山麓\xuanjuedashiboss.txt` | 295 | 27 | **KHONG** | - |
| `maps\中原北区\渡船\渡船刷怪点.txt` | 893 | 65 | **KHONG** | - |
| `maps\中原北区\风陵渡北岸\baiyingyingboss.txt` | 99 | 10 | **KHONG** | - |
| `maps\中原北区\风陵渡北岸\yanxiaoqianboss.txt` | 99 | 10 | **KHONG** | - |
| `maps\中原北区\风陵渡南岸\herenwoboss.txt` | 143 | 14 | **KHONG** | - |
| `maps\江南区\两水洞迷宫\lanyiyiboss.txt` | 22 | 3 | **KHONG** | - |
| `maps\江南区\两水洞迷宫\zhonglingxiuboss.txt` | 22 | 3 | **KHONG** | - |
| `maps\江南区\西山屿\big_goldboss.txt` | 97 | 9 | **KHONG** | - |
| `maps\江南区\西山屿\hanmengboss.txt` | 231 | 22 | **KHONG** | - |
| `maps\西北北区\沙漠迷宫\duanmuruiboss.txt` | 22 | 3 | **KHONG** | - |
| `maps\西北北区\沙漠迷宫\mengcanglangboss.txt` | 22 | 3 | **KHONG** | - |
| `maps\西北北区\沙漠迷宫\qinglianziboss.txt` | 22 | 3 | **KHONG** | - |
| `maps\西北北区\莫高窟\tangburanboss.txt` | 317 | 29 | **KHONG** | - |
| `missions\boss\libai\boxpos.txt` | 2702 | 151 | co | y het |
| `task\random\trea_boss.txt` | 690 | 22 | co | y het |
| `task\tollgate\killer\killer.txt` | 14033 | 161 | co | y het |
| `trigger_challengeoftime.lua` | 704 | 24 | co | y het |
