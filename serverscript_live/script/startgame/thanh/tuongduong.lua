-- @IncludeOnce  [LUA54 06/09 toi] tep toan ham: Lua54Dll chi chay than mot lan moi state (chon_includeonce.py)
--hoangnhk
function addnpctuongduong()
	----NPC Chuc nang-----
	local nNpcIdx;
	nNpcIdx = AddObj(1,78,1575*32,3340*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0); SetNpcValue(nNpcIdx, 30);--passerby198
	nNpcIdx = AddObj(1,78,1450*32,3263*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0); SetNpcValue(nNpcIdx, 31);--passerby198
	nNpcIdx = AddObj(1,78,1482*32,3164*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0); SetNpcValue(nNpcIdx, 32);--passerby198
	nNpcIdx = AddObj(1,78,1563*32,3219*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0); SetNpcValue(nNpcIdx, 29);--passerby198
	nNpcIdx = AddObj(1,78,1687*32,3235*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0); SetNpcValue(nNpcIdx, 33);--passerby198
	nNpcIdx = AddNpcNew(384,1,78,1630*32,3261*32,"\\script\\global\\npcchucnang\\taphoa.lua",6,814);SetNpcValue(nNpcIdx, 11);
	nNpcIdx = AddNpcNew(198,1,78,1551*32,3215*32,"\\script\\global\\npcchucnang\\thoren.lua",6,55);SetNpcValue(nNpcIdx, 10);--passerby004
	nNpcIdx = AddNpcNew(203,1,78,1612*32,3244*32,"\\script\\global\\npcchucnang\\hieuthuoc.lua",6,51); SetNpcValue(nNpcIdx, 12);--passerby007
	nNpcIdx = AddNpcNew(230,1,78,1567*32,3194*32,"\\script\\global\\npcchucnang\\banngua.lua",6,64); SetNpcValue(nNpcIdx, 45);--passerby037
	AddNpcNew(237,1,78,1597*32,3376*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)--passerby040-2
	AddNpcNew(237,1,78,1437*32,3211*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)--passerby040-2
	AddNpcNew(393,1,78,1692*32,3232*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)--passerby040-4
	AddNpcNew(393,1,78,1510*32,3139*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)--passerby040-4
	AddNpcNew(108,1,78,1595*32,3288*32,"\\script\\global\\seasonnpc.lua",6,59)--enemy081
	AddNpcNew(377,1,78,1537*32,3245*32,"\\script\\global\\npcchucnang\\lequan.lua",6,57)--passerby181
	AddNpcNew(377,1,78,1587*32,3368*32,"\\script\\global\\npcchucnang\\dichquan.lua",6,717)--passerby181
	--AddNpcNew(309,1,78,1577*32,3376*32,"\\script\\global\\npcchucnang\\congbinhtu.lua",6,868)--passerby094
	--AddNpcNew(309,1,78,1474*32,3271*32,"\\script\\global\\npcchucnang\\congbinhtu.lua",6,868)--passerby094
	--AddNpcNew(309,1,78,1465*32,3183*32,"\\script\\global\\npcchucnang\\congbinhtu.lua",6,868)--passerby094
	--AddNpcNew(309,1,78,1705*32,3235*32,"\\script\\global\\npcchucnang\\congbinhtu.lua",6,868)--passerby094
	AddNpcNew(259,1,78,1490*32,3266*32,"\\script\\global\\npcchucnang\\hangrong.lua",6,48)--passerby063
	AddNpcNew(391,1,78,1530*32,3224*32,"\\script\\global\\npcchucnang\\tientrang.lua",6,70)
	AddNpcNew(380,1,78,1556*32,3183*32,"\\script\\global\\npcchucnang\\cuahangphucduyen.lua",6,35)
	AddNpcNew(308,1,78,1607*32,3259*32,"\\script\\global\\npcchucnang\\truyennhan.lua",6,65)
	--AddNpcNew(308,1,78,1546*32,3117*32,"\\script\\global\\npcchucnang\\sgkietxuat.lua",6,"Sø gi¶ liªn ®Êu")
	--AddNpcNew(87,1,78,1558*32,3116*32,"\\script\\global\\npcchucnang\\sgliendau.lua",6,"Sø gi¶ liªn ®Êu")
	-- nNpcIdx = AddNpcEx1({769},1,nil,78,1511*32,3204*32,"",OTHER_SGSTHU,nil,6) SetNpcValue(nNpcIdx, 2) --nhiep thi tran tuong duong
	-- AddNpcNew(62,1,78,1708*32,3217*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,3);
	-- AddNpcNew(62,1,78,1715*32,3224*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,3);
	-- AddNpcNew(62,1,78,1467*32,3162*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,3);
	-- AddNpcNew(62,1,78,1460*32,3169*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,3);
	-- AddNpcNew(62,1,78,1431*32,3305*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,3);
	-- AddNpcNew(62,1,78,1424*32,3298*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,3);
	-- AddNpcNew(62,1,78,1619*32,3421*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,3);
	-- AddNpcNew(62,1,78,1625*32,3415*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,3);
	AddNpcNew(51,1,78,1593*32,3206*32,"\\script\\global\\npcchucnang\\nhamon.lua",6,532);
	AddNpcNew(51,1,78,1600*32,3214*32,"\\script\\global\\npcchucnang\\nhamon.lua",6,532);
	
	-- AddNpcNew(2011,1,78,1582*32,3236*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"");
	-- AddNpcNew(209,1,78,1606*32,3301*32,"\\script\\global\\npcchucnang\\chusongbac.lua",6)
	--Npc tuong duong 
	
end;

function addtraptuongduong()
	AddTrapEx2(78,1426,3294,10,"\\script\\maps\\tuongduong\\trap\\cong8h.lua")
	AddTrapEx1(78,1457,3165,10,"\\script\\maps\\tuongduong\\trap\\cong10h.lua")
	AddTrapEx2(78,1710,3213,10,"\\script\\maps\\tuongduong\\trap\\cong2h.lua")
	AddTrapEx1(78,1615,3417,10,"\\script\\maps\\tuongduong\\trap\\cong4h.lua")
	
end;

function addobjtuongduong()

	--AddObj(1,78,53997,103503,"",0,0)
	AddObj(4,78,54457,103699,"\\script\\missions\\citywar_global\\citybulletin.lua",0,0)
	
end;