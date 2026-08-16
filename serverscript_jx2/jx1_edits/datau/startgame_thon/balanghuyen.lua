--hoangnhk
Include("\\script\\test\\npcchat.lua")
function addnpcbalang()
	local nNpcIdx;
--luyen kc
-- local MOCNHAN = {
-- {53414,96334},
-- {53365,95976},
-- {53530,95766},
-- {53729,95776},
-- {53942,95944},
-- {54108,96264},
-- {54134,96760},
-- {54078,97000},
-- {53456,96826},
-- {53407,96668}
-- };
-- local COCGO = {
-- {52877,94864},
-- {53028,94930},
-- {53169,95064},
-- {53323,95330},
-- {53735,95310},
-- {53866,95248},
-- {53997,95276},
-- {54103,95568},
-- {54089,95832}
-- };
-- local BAOCAT = {
-- {53254,95944},
-- {53241,95686},
-- {53129,95420},
-- {53003,95236},
-- {52765,95124},
-- {52497,95196},
-- {52539,95642},
-- {52533,95944},
-- {52647,96268},
-- {53139,96290}
-- };
	-- for i=1,getn(MOCNHAN) do
	-- nNpcIdx = AddNpcNew(414,1,53,MOCNHAN[i][1],MOCNHAN[i][2],DEATH_COCGO,
		-- 5,nil,1,RANDOMC(0,1,2,3,4),1,1,nil,nil,1,nil,nil,nil,50,nil,nil,"");
	-- SetNpcValue(nNpcIdx,3);
	-- end
	-- for i=1,getn(COCGO) do
	-- nNpcIdx = AddNpcNew(413,1,53,COCGO[i][1],COCGO[i][2],DEATH_COCGO,
		-- 5,nil,1,RANDOMC(0,1,2,3,4),1,1,nil,nil,1,nil,nil,nil,50,nil,nil,"");
	-- SetNpcValue(nNpcIdx,1);
	-- end
	-- for i=1,getn(BAOCAT) do
	-- nNpcIdx = AddNpcNew(415,1,53,BAOCAT[i][1],BAOCAT[i][2],DEATH_COCGO,
		-- 5,nil,1,RANDOMC(0,1,2,3,4),1,1,nil,nil,1,nil,nil,nil,50,nil,nil,"");
	-- SetNpcValue(nNpcIdx,2);
	-- end
	----NPC Mon Phai trong thon-----
	nNpcIdx = AddNpcNew(189,1,53,1592*32,3129*32,"\\script\\npcthon\\npcmonphai\\thieulam.lua",6,183) SetNpcValue(nNpcIdx, 0);
	nNpcIdx = AddNpcNew(184,1,53,1610*32,3196*32,"\\script\\npcthon\\npcmonphai\\thienvuong.lua",6,247) SetNpcValue(nNpcIdx, 1);
	nNpcIdx = AddNpcNew(177,1,53,1613*32,3174*32,"\\script\\npcthon\\npcmonphai\\duongmon.lua",6,246) SetNpcValue(nNpcIdx, 2);
	nNpcIdx = AddNpcNew(186,1,53,1621*32,3205*32,"\\script\\npcthon\\npcmonphai\\ngudoc.lua",6,178) SetNpcValue(nNpcIdx, 3);
	nNpcIdx = AddNpcNew(83 ,1,53,1636*32,3184*32,"\\script\\npcthon\\npcmonphai\\ngami.lua",6,248) SetNpcValue(nNpcIdx, 4);
	nNpcIdx = AddNpcNew(171,1,53,1581*32,3203*32,"\\script\\npcthon\\npcmonphai\\thuyyen.lua",6,177) SetNpcValue(nNpcIdx, 5);
	nNpcIdx = AddNpcNew(103,1,53,1601*32,3124*32,"\\script\\npcthon\\npcmonphai\\caibang.lua",6,194) SetNpcValue(nNpcIdx, 6);
	nNpcIdx = AddNpcNew(181,1,53,1619*32,3163*32,"\\script\\npcthon\\npcmonphai\\thiennhan.lua",6,240) SetNpcValue(nNpcIdx, 7);
	nNpcIdx = AddNpcNew(188,1,53,1635*32,3189*32,"\\script\\npcthon\\npcmonphai\\vodang.lua",6,249) SetNpcValue(nNpcIdx, 8);
	nNpcIdx = AddNpcNew(309,1,53,1576*32,3145*32,"\\script\\npcthon\\npcmonphai\\conlon.lua",6,181) SetNpcValue(nNpcIdx, 9);
	----NPC Chuc nang-----
--	nNpcIdx = AddNpcNew(625,1,53,1612*32,3168*32,"\\script\\global\\npcchucnang\\ruongchua.lua",6); SetNpcValue(nNpcIdx, 19);
	nNpcIdx = AddObj(1,53,1612*32,3168*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,19); --dong
	nNpcIdx = AddNpcNew(219,1,53,1600*32,3170*32,"\\script\\global\\npcchucnang\\taphoa.lua",6,84); SetNpcValue(nNpcIdx, 23);
	nNpcIdx = AddNpcNew(198,1,53,1586*32,3155*32,"\\script\\global\\npcchucnang\\thoren.lua",6,55); SetNpcValue(nNpcIdx, 22);
	nNpcIdx = AddNpcNew(203,1,53,1598*32,3209*32,"\\script\\global\\npcchucnang\\hieuthuoc.lua",6,51); SetNpcValue(nNpcIdx, 24);
	-- AddNpcNew(305,1,53,1621*32,3184*32,"\\script\\event\\eventnoel\\main_noel.lua",6,"NPC EVENT NOEL")
	AddNpcNew(239,1,53,1578*32,3236*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	local nNpcIdxDT = AddNpcNew(108,1,53,1626*32,3172*32,"\\script\\global\\seasonnpc.lua",6,59)
	SetNpcTimer(nNpcIdxDT, 18 * timer_sec_chat)
	AddNpcNew(377,1,53, 52150, 102241, "\\script\\global\\npcchucnang\\lequan.lua",6,57) 
	-- AddNpcNew(663,1,53,1619*32,3169*32,"\\script\\global\\npcchucnang\\longngu.lua",6)--enemy199
	-- [DOT-E1 bo he CTC cu] AddNpcNew(373,1,53,1601*32,3188*32,"\\script\\tinhnang\\congthanhchien\\congthanhquan.lua",6,186)
	-- AddNpcNew(373,1,53,1615*32,3209*32,"\\script\\tinhnang\\loidai\\vebinhdautruong.lua",6,"L«i §µi Bang Héi")
	-- AddNpcNew(209,1,53,1626*32,3185*32,"\\script\\songbac\\chusongbac.lua",6,"Chñ Sßng B¹c")
	-- AddNpcNew(210,1,53,1629*32,3186*32,"\\script\\songbac\\baoke.lua",6,"B¶o Kª X¨m Træ")
	AddNpcNew(53,1,53,1600*32,3220*32,"\\script\\tinhnang\\boss_hoangkim\\deathhksv.lua",6,"Qu¶n Lý Item Boss Bang Héi")
	AddNpcNew(240,1,53,1635*32,3218*32,"\\script\\tinhnang\\loidaihonchien\\mainloidai.lua",6,"L«i §µi Hæn ChiÕn" ) 
	--AddNpcNew(311,1,53,53489,95320,"\\script\\global\\npcchucnang\\vosu.lua",6,201)
	---NPC EVENT ----
	--AddNpcNew(53,1,53,1618*32,3169*32,"\\script\\event\\30thang4\\tuongquan.lua",6,"Hoµnh Thiªn T­íng Qu©n")
	--Npc ba lang huyen
	--AddNpcNew(362,1,53,1639*32,3210*32,"\\script\\npcthon\\balang\\tieungu.lua",6,252) 
	--AddNpcNew(297,1,53,1632*32,3214*32,"\\script\\npcthon\\balang\\ngudan.lua",6,241) 
	--AddNpcNew(347,1,53,1621*32,3256*32,"\\script\\npcthon\\balang\\aphuong.lua",6,253) 
	AddNpcNew(323,1,53,1506*32,3173*32,"\\script\\global\\npcchucnang\\thamcuu.lua",6,242) 
	-- AddNpcNew(1867,1,53,1620*32,3191*32,"\\script\\event\\eventtet\\eventtuilixi.lua",6,"NPC §æI EVENT") 
	AddNpcNew(1867,1,53,1601*32,3199*32,"\\script\\event\\eventtet\\npcsukien.lua",6,"Sù KiÖn TÕt §oµn Viªn") 
	AddNpcNew(1867,1,53,1620*32,3191*32,"\\script\\event\\chuyensinhdaisu.lua",6,"NPC ChuyÓn Sinh") 
	AddNpcNew(201,1,53,1607*32,3215*32,"\\script\\global\\npcchucnang\\thorenthanbi.lua",6,"Thî rÌn thÇn bÝ ")
	AddNpcNew(308,1,53,1626*32,3185*32,"\\script\\tinhnang\\lien_dau\\sugialiendau.lua",6,"Sø gi¶ liªn ®Êu")
	AddNpcNew(87,1,53,1629*32,3186*32,"\\script\\tinhnang\\lien_dau\\sugiakietxuat.lua",6,"Sø gi¶ kiÖt xuÊt")
	AddNpcNew(377,1,53,1625*32,3198*32,"\\script\\global\\npcchucnang\\dichquan.lua",6,717)
	-- AddNpcNew(2002,1,53,1622*32,3193*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	-- AddNpcNew(2003,1,53,1622*32,3193*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	-- AddNpcNew(2004,1,53,1623*32,3193*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	-- AddNpcNew(2005,1,53,1624*32,3193*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	-- AddNpcNew(2006,1,53,1624*32,3193*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	
	-- AddNpcNew(2011,1,53,1614*32,3220*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	-- AddNpcNew(2012,1,53,1587*32,3217*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	-- AddNpcNew(2013,1,53,1593*32,3186*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	-- AddNpcNew(2014,1,53,1598*32,3197*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	-- AddNpcNew(2015,1,53,1587*32,3212*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	-- AddNpcNew(2015,1,53,1593*32,3181*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	-- AddNpcNew(2015,1,53,1598*32,3192*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	
	-- AddNpcNew(2003,1,53,1621*32,3192*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	-- AddNpcNew(2004,1,53,1621*32,3192*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	-- AddNpcNew(2005,1,53,1621*32,3192*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	-- AddNpcNew(2006,1,53,1621*32,3192*32,"\\script\\global\\npcchucnang\\noscript.lua",6,"") 
	--AddNpcNew(247,1,53,1632*32,3243*32,"\\script\\npcthon\\balang\\colaothai.lua",6,254) 
	--AddNpcNew(359,1,53,57538,98808,"\\script\\npcthon\\balang\\dukhach.lua",6,237) 
	--AddNpcNew(360,1,53,1569*32,3151*32,"\\script\\npcthon\\balang\\thuanthuy.lua",6,239)
	--AddNpcNew(385,1,53,1564*32,3145*32,"\\script\\npcthon\\balang\\lagiathamtu.lua",6,238) 
	--AddNpcNew(246,1,53,1557*32,3196*32,"\\script\\npcthon\\balang\\cuucanlaogia.lua",6,243) 
	--AddNpcNew(212,1,53,1605*32,3120*32,"\\script\\npcthon\\balang\\chutuudiem.lua",6,38) 
	--AddNpcNew(367,1,53,1551*32,3220*32,"\\script\\npcthon\\balang\\ducuuchau.lua",6,250) 
	--AddNpcNew(333,1,53,1568*32,3183*32,"\\script\\npcthon\\balang\\phuongkhavi.lua",6,244) 
	--nNpcIdx = AddNpcNew(240,1,53,1801*32,3154*32,"\\script\\npcthon\\balang\\thuyenphu.lua",6,241);SetNpcValue(nNpcIdx, 2);
	--nNpcIdx = AddNpcNew(242,1,53,1778*32,3176*32,"\\script\\npcthon\\balang\\thuyenphu.lua",6,241);SetNpcValue(nNpcIdx, 2);
	--nNpcIdx = AddNpcNew(242,1,53,49385,93542,"\\script\\npcthon\\balang\\thuyenphu.lua",6,31);SetNpcValue(nNpcIdx, 1);
end;

function addtrapbalang()
	-- AddTrapEx1(53,1648,3244,12,"\\script\\maps\\balang\\trap\\cong4h.lua")
	-- AddTrapEx2(53,1558,3250,12,"\\script\\maps\\balang\\trap\\cong8h.lua")
	-- AddTrapEx1(53,1534,3119,12,"\\script\\maps\\balang\\trap\\cong10h.lua")
	-- AddTrapEx2(53,1624,3093,12,"\\script\\maps\\balang\\trap\\cong2h.lua")
	
end;

function addobjbalang()
	-- AddObj(303,53,57712,96746,"\\script\\maps\\balang\\obj\\ngocboi.lua",0,0)
	-- AddObj(6,53,52041,98881,"\\script\\maps\\balang\\obj\\biada.lua",1,0)
	-- AddObj(4,53,51824,99108,"",0,0)
	-- AddObj(6,53,49418,99490,"\\script\\maps\\balang\\obj\\biada.lua",0,0)
	--AddObj(1,53,51593,101357,"",0,0)
	-- AddObj(4,53,52083,101248,"\\script\\maps\\balang\\obj\\caothi.lua",1,0)
	-- AddObj(6,53,49835,104110,"\\script\\maps\\balang\\obj\\biada.lua",1,0)
	-- AddObj(6,53,53128,103632,"\\script\\maps\\balang\\obj\\biada.lua",0,0)
	-- SetObjValue(AddObj(295,53,43188,105587,"\\script\\maps\\balang\\obj\\dauhieu.lua",0,0),1)
	-- SetObjValue(AddObj(295,53,43178,106701,"\\script\\maps\\balang\\obj\\dauhieu.lua",0,0),2);
	-- SetObjValue(AddObj(296,53,57826,109886,"\\script\\maps\\balang\\obj\\dauhieu.lua",0,0),3)
end;