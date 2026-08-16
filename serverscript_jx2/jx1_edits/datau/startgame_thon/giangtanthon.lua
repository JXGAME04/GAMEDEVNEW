--hoangnhk
function addnpcgiangtan()
	local nNpcIdx;
	--luyen kc
-- local MOCNHAN = {
-- {115751,201104},
-- {115607,201014},
-- {115480,200888},
-- {115397,200688},
-- {115429,200510},
-- {115592,200320},
-- {115697,200264},
-- {115970,200490},
-- {116033,200764},
-- {115961,200996},
-- {115841,201068}
-- };
-- local COCGO = {
-- {116626,200334},
-- {116558,200500},
-- {116555,200768},
-- {116584,201022},
-- {116657,201228},
-- {116781,201288},
-- {116923,201260},
-- {117042,201158},
-- {117120,201004},
-- {116023,201630},
-- {116235,201498},
-- {116353,201630}
-- };
-- local BAOCAT = {
-- {116588,199922},
-- {116793,199894},
-- {116953,200226},
-- {117151,200396},
-- {117235,200712},
-- {117325,200868},
-- {117528,200860},
-- {117603,200694},
-- {117563,200472},
-- {117416,200374}
-- };
	-- for i=1,getn(MOCNHAN) do
	-- nNpcIdx = AddNpcNew(414,1,20,MOCNHAN[i][1],MOCNHAN[i][2],DEATH_COCGO,
		-- 5,nil,1,RANDOMC(0,1,2,3,4),1,1,nil,nil,1,nil,nil,nil,50,nil,nil,"");
	-- SetNpcValue(nNpcIdx,3);
	-- end
	-- for i=1,getn(COCGO) do
	-- nNpcIdx = AddNpcNew(413,1,20,COCGO[i][1],COCGO[i][2],DEATH_COCGO,
		-- 5,nil,1,RANDOMC(0,1,2,3,4),1,1,nil,nil,1,nil,nil,nil,50,nil,nil,"");
	-- SetNpcValue(nNpcIdx,1);
	-- end
	-- for i=1,getn(BAOCAT) do
	-- nNpcIdx = AddNpcNew(415,1,20,BAOCAT[i][1],BAOCAT[i][2],DEATH_COCGO,
		-- 5,nil,1,RANDOMC(0,1,2,3,4),1,1,nil,nil,1,nil,nil,nil,50,nil,nil,"");
	-- SetNpcValue(nNpcIdx,2);
	-- end
	----NPC Mon Phai-----
	nNpcIdx = AddNpcNew(189,1,20,3562*32,6215*32,"\\script\\npcthon\\npcmonphai\\thieulam.lua",6,183);SetNpcValue(nNpcIdx, 0);
	nNpcIdx = AddNpcNew(184,1,20,113990,197900,"\\script\\npcthon\\npcmonphai\\thienvuong.lua",6,247);SetNpcValue(nNpcIdx, 1);
	nNpcIdx = AddNpcNew(177,1,20,3553*32,6232*32,"\\script\\npcthon\\npcmonphai\\duongmon.lua",6,185);SetNpcValue(nNpcIdx, 2);
	nNpcIdx = AddNpcNew(186,1,20,114338,197292,"\\script\\npcthon\\npcmonphai\\ngudoc.lua",6,178);SetNpcValue(nNpcIdx, 3);
	nNpcIdx = AddNpcNew(83 ,1,20,3540*32,6218*32,"\\script\\npcthon\\npcmonphai\\ngami.lua",6,248);SetNpcValue(nNpcIdx, 4);
	nNpcIdx = AddNpcNew(171,1,20,113594,199822,"\\script\\npcthon\\npcmonphai\\thuyyen.lua",6,177);SetNpcValue(nNpcIdx, 5);
	nNpcIdx = AddNpcNew(103,1,20,3563*32,6235*32,"\\script\\npcthon\\npcmonphai\\caibang.lua",6,194);SetNpcValue(nNpcIdx, 6);
	nNpcIdx = AddNpcNew(181,1,20,3584*32,6204*32,"\\script\\npcthon\\npcmonphai\\thiennhan.lua",6,240);SetNpcValue(nNpcIdx, 7);
	nNpcIdx = AddNpcNew(188,1,20,3571*32,6207*32,"\\script\\npcthon\\npcmonphai\\vodang.lua",6,249);SetNpcValue(nNpcIdx, 8);
	nNpcIdx = AddNpcNew(309,1,20,3526*32,6217*32,"\\script\\npcthon\\npcmonphai\\conlon.lua",6,181);SetNpcValue(nNpcIdx, 9);
	----NPC Chuc nang-----
	nNpcIdx = AddNpcNew(341,1,20,3371*32,6255*32,"\\script\\global\\npcchucnang\\taphoa.lua",6,84);SetNpcValue(nNpcIdx, 23);
	nNpcIdx = AddNpcNew(199,1,20,3439*32,6221*32,"\\script\\global\\npcchucnang\\thoren.lua",6,55);SetNpcValue(nNpcIdx, 22);
	nNpcIdx = AddNpcNew(203,1,20,3468*32,6164*32,"\\script\\global\\npcchucnang\\hieuthuoc.lua",6,51);SetNpcValue(nNpcIdx, 24);
	nNpcIdx = AddNpcNew(625,1,20,3456*32,6172*32,"\\script\\global\\npcchucnang\\ruongchua.lua",6);SetNpcValue(nNpcIdx,11);  
	nNpcIdx = AddNpcNew(625,1,20,108448,200730,"\\script\\global\\npcchucnang\\ruongchua.lua",6);SetNpcValue(nNpcIdx,12);  
	nNpcIdx = AddNpcNew(625,1,20,3551*32,6217*32,"\\script\\global\\npcchucnang\\ruongchua.lua",6);SetNpcValue(nNpcIdx,10);  
	AddNpcNew(237,1,20,3458*32,6110*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	AddNpcNew(108,1,20,113212,199416,"\\script\\global\\seasonnpc.lua",6,59)
	AddNpcNew(377,1,20,113662,198144,"\\script\\global\\npcchucnang\\lequan.lua",6,57) 
	-- AddNpcNew(663,1,20,3545*32,6223*32,"\\script\\global\\npcchucnang\\longngu.lua",6)
    -- AddNpcNew(373,1,20,3487*32,6134*32,"\\script\\global\\npcchucnang\\cthanhquan.lua",6,186)
	-- AddNpcNew(311,1,20,3638*32,6264*32,"\\script\\global\\npcchucnang\\vosu.lua",6,201)		
    -- AddNpcNew(87,1,20,113877,199198,"\\script\\global\\npcchucnang\\trogiup.lua",6,"T©n Thñ Sø Gi¶ ")
	----NPC giangtanthon-----

    -- AddNpcNew(342,1,20,3483*32,6150*32,"\\script\\npcthon\\giangtan\\ngohongmai.lua",6,499)
    -- AddNpcNew(247,1,20,3462*32,6199*32,"\\script\\npcthon\\giangtan\\ngolaothai.lua",6,785)
    -- AddNpcNew(179,1,20,3446*32,6168*32,"\\script\\npcthon\\giangtan\\duongnguyen.lua",6,498)
    -- AddNpcNew(180,1,20,3448*32,6169*32,"\\script\\npcthon\\giangtan\\caudnguyen.lua",6,497)
    -- AddNpcNew(447,1,20,3478*32,6126*32,"\\script\\npcthon\\giangtan\\latiensinh.lua",6,495)
	-- AddNpcNew(379,1,20,3487*32,6222*32,"\\script\\npcthon\\giangtan\\quachdung.lua",6,502)
    -- AddNpcNew(382,1,20,3493*32,6206*32,"\\script\\npcthon\\giangtan\\hotunuong.lua",6,500)
    -- AddNpcNew(346,1,20,3493*32,6231*32,"\\script\\npcthon\\giangtan\\thienthiennuong.lua",6,503)
    -- AddNpcNew(363,1,20,3491*32,6236*32,"\\script\\npcthon\\giangtan\\thienthien.lua",6,504)
    -- AddNpcNew(212,1,20,109988,198420,"\\script\\npcthon\\giangtan\\chutuudiem.lua",6,38)
    -- AddNpcNew(455,1,20,112703,195764,"\\script\\npcthon\\giangtan\\nguyetlao.lua",6,496)
	-- AddNpcNew(372,1,20,3413*32,6220*32,"\\script\\npcthon\\giangtan\\phuthanhotu.lua",6,786)
    -- AddNpcNew(360,1,20,3574*32,6193*32,"\\script\\npcthon\\giangtan\\hotu.lua",6,501)
    -- AddNpcNew(312,1,20,3510*32,6216*32,"\\script\\npcthon\\giangtan\\ngolaogia.lua",6,505)
    -- AddNpcNew(240,1,20,3361*32,6282*32,"\\script\\npcthon\\giangtan\\thuyenphu.lua",6,31)
	--nv thon
	--ho tu
	--ngo lao gia
	--phu than ho tu
end;

function addtrapgiangtan()
	AddTrapEx1(20,3401,6267,12,"\\script\\maps\\giangtan\\trap\\cong8h.lua")
	AddTrapEx2(20,3485,6239,12,"\\script\\maps\\giangtan\\trap\\cong6h.lua")
	AddTrapEx1(20,3441,6112,15,"\\script\\maps\\giangtan\\trap\\cong10h.lua")
	AddTrapEx1(20,3589,6239,15,"\\script\\maps\\giangtan\\trap\\cong4h.lua")
	AddTrapEx1(20,3441,6242,10,"\\script\\maps\\giangtan\\trap\\cong7h.lua")
	
end;

function addobjgiangtan()
	-- AddObj(297,20,108627,192949,"",0,0)
	-- AddObj(302,20,112387,195162,"",0,0)
	-- AddObj(301,20,116646,194775,"",0,0)
	--AddObj(1,20,110601,197518,"",0,0)
	-- AddObj(3,20,112584,196847,"",0,0)
	--AddObj(1,20,113652,198912,"",0,0)
	-- AddObj(6,20,113782,199032,"\\script\\maps\\giangtan\\obj\\biada.lua",0,0)
	-- AddObj(7,20,107805,200540,"",1,0)
	-- AddObj(7,20,107805,200540,"",1,0)
	-- AddObj(302,20,110861,199716,"\\script\\maps\\giangtan\\obj\\chaccay.lua",0,0)
	--AddObj(1,20,108447,200723,"",0,0)
	-- AddObj(301,20,119319,203077,"\\script\\maps\\giangtan\\obj\\datrau.lua",0,0)
end;