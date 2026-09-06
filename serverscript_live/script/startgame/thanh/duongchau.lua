--hoangnhk
function addnpcduongchau()
	local nNpcIdx;
	----NPC Chuc nang----- 
	nNpcIdx = AddNpcNew(387,1,80,1689*32,3167*32,"\\script\\global\\npcchucnang\\thoren.lua",6,55);SetNpcValue(nNpcIdx, 4);
	nNpcIdx = AddNpcNew(203,1,80,1776*32,3079*32,"\\script\\global\\npcchucnang\\hieuthuoc.lua",6,51);SetNpcValue(nNpcIdx, 12);
	nNpcIdx = AddNpcNew(344,1,80,1703*32,3020*32,"\\script\\global\\npcchucnang\\taphoa.lua",6,84);SetNpcValue(nNpcIdx, 5);
	nNpcIdx = AddNpcNew(231,1,80,1643*32,3181*32,"\\script\\global\\npcchucnang\\banngua.lua",6,64);SetNpcValue(nNpcIdx, 43);
	nNpcIdx = AddObj(1,80,1757*32,3005*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,34); -- giua
	nNpcIdx = AddObj(1,80,1710*32,3227*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,35); -- dong
	nNpcIdx = AddObj(1,80,1662*32,2985*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,38); -- tay 38
	nNpcIdx = AddObj(1,80,1627*32,3174*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,37); -- nam
	nNpcIdx = AddObj(1,80,1855*32,3030*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,36); -- bac	36
	nNpcIdx = AddNpcNew(377,1,80,1678*32,2999*32,"\\script\\global\\npcchucnang\\dichquan.lua",6,47);--SetNpcValue(nNpcIdx,50);

	AddNpcNew(237,1,80,1824*32,3062*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	AddNpcNew(237,1,80,1671*32,2990*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	AddNpcNew(236,1,80,1593*32,3192*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	AddNpcNew(237,1,80,1726*32,3205*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)	
	AddNpcNew(201,1,80,1692*32,3165*32,"\\script\\global\\npcchucnang\\hoctrothoren.lua",6,7) 
	AddNpcNew(108,1,80,1745*32,2967*32,"\\script\\global\\seasonnpc.lua",6,59)
	AddNpcNew(377,1,80,1744*32,2996*32,"\\script\\global\\npcchucnang\\lequan.lua",6,57)
    AddNpcNew(209,1,80,53087,98724,"\\script\\global\\npcchucnang\\thuongnhan.lua",6,35)   
	nNpcIdx = AddNpcNew(309,1,80,1628*32,3207*32,"\\script\\global\\npcchucnang\\congbinhtu.lua",6,868);SetNpcValue(nNpcIdx,37);
	nNpcIdx = AddNpcNew(309,1,80,1710*32,3250*32,"\\script\\global\\npcchucnang\\congbinhtu.lua",6,868);SetNpcValue(nNpcIdx,35);
	nNpcIdx = AddNpcNew(309,1,80,1852*32,3049*32,"\\script\\global\\npcchucnang\\congbinhtu.lua",6,868);SetNpcValue(nNpcIdx,38);
	nNpcIdx = AddNpcNew(309,1,80,1659*32,3021*32,"\\script\\global\\npcchucnang\\congbinhtu.lua",6,868);SetNpcValue(nNpcIdx,36);
	AddNpcNew(308,1,80,1793*32,3046*32,"\\script\\global\\npcchucnang\\truyennhan.lua",6,65)
	--AddNpcNew(87,1,80,1737*32,3040*32,"\\script\\global\\npcchucnang\\sugialiendau.lua",6,"Sø gi¶ liªn ®Êu")		
	AddNpcNew(308,1,80,56084,97114,"\\script\\global\\npcchucnang\\sgkietxuat.lua",6,"Sø gi¶ liªn ®Êu")
	-- nNpcIdx = AddNpcEx1({769},1,nil,80,1700*32,2963*32,"",OTHER_SGSTHU,nil,6) SetNpcValue(nNpcIdx, 3)--nhiep thi tran duong chau	
	AddNpcNew(62,1,80,1742*32,3263*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbdong
	AddNpcNew(62,1,80,1749*32,3256*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbdong
	AddNpcNew(62,1,80,1646*32,3000*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbtay
	AddNpcNew(62,1,80,1652*32,2994*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbtay
	AddNpcNew(62,1,80,1569*32,3235*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbnam
	AddNpcNew(62,1,80,1577*32,3242*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbnam
	AddNpcNew(62,1,80,1878*32,3024*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbbac
	AddNpcNew(62,1,80,1871*32,3017*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbbac	
	AddNpcNew(1645,1,80,1773*32,3017*32,"\\script\\global\\npcchucnang\\nhamon.lua",6,532)
	AddNpcNew(1645,1,80,1779*32,3023*32,"\\script\\global\\npcchucnang\\nhamon.lua",6,532)	
	AddNpcNew(260,1,80,1637*32,3047*32,"\\script\\global\\npcchucnang\\hangrong.lua",6,48)	
    AddNpcNew(209,1,80,1740*32,3155*32,"\\script\\global\\npcchucnang\\chusongbac.lua",6,63)   
	AddNpcNew(227,1,80,1814*32,3116*32,"\\script\\global\\npcchucnang\\tientrang.lua",6,70)
	--=====NPC duongchau======
	-- AddNpcNew(115,1,80,1592*32,3118*32,"\\script\\feature\\nvhoangkim\\tieusu.lua",6,"Tiªu s­ ") --nvhk
    -- AddNpcNew(178,1,80,1703*32,3119*32,"\\script\\feature\\nvhoangkim\\havotha.lua",6,"H¹ V« Th¶ ")--nvhk
	-- AddNpcNew(329,1,80,1765*32,3013*32,"\\script\\global\\npcchucnang\\tetuu.lua",6,"TÕ Töu")	
    -- AddNpcNew(385,1,80,1791*32,3094*32,"\\script\\npcthanhthi\\duongchau\\tiemvatnuoi.lua",6,808)	
    -- AddNpcNew(211,1,80,1743*32,3150*32,"\\script\\npcthanhthi\\duongchau\\baoke.lua",6,61)        
	-- AddNpcNew(210,1,80,1734*32,3159*32,"\\script\\npcthanhthi\\duongchau\\baoke1.lua",6,61)        
    -- AddNpcNew(334,1,80,1750*32,3154*32,"\\script\\npcthanhthi\\duongchau\\conbac1.lua",6,62)	     	
	-- AddNpcNew(370,1,80,1734*32,3169*32,"\\script\\npcthanhthi\\duongchau\\conbac2.lua",6,62)       
	-- AddNpcNew(335,1,80,56091,100430,"\\script\\npcthanhthi\\duongchau\\conbac.lua",6,62)       
	-- AddNpcNew(275,1,80,1764*32,3055*32,"\\script\\npcthanhthi\\duongchau\\totamnuong.lua",6,77)
    -- AddNpcNew(277,1,80,1736*32,3105*32,"\\script\\npcthanhthi\\duongchau\\hoadanglang.lua",6,734)
    -- AddNpcNew(320,1,80,1718*32,3106*32,"\\script\\npcthanhthi\\duongchau\\cogaibanhoa.lua",6,733)
    -- AddNpcNew(290,1,80,1690*32,3132*32,"\\script\\npcthanhthi\\duongchau\\lacthanhthu.lua",6,732)
    -- AddNpcNew(378,1,80,1658*32,3183*32,"\\script\\npcthanhthi\\duongchau\\ongchudo.lua",6,738)
    -- AddNpcNew(365,1,80,1597*32,3165*32,"\\script\\npcthanhthi\\duongchau\\chuluquan.lua",6,88)
    -- AddNpcNew(196,1,80,1601*32,3160*32,"\\script\\npcthanhthi\\duongchau\\tieunhiluquan.lua",6,87)	
    -- AddNpcNew(48,1,80,1608*32,3152*32,"\\script\\npcthanhthi\\duongchau\\truongtuan.lua",6,100)	
    -- AddNpcNew(225,1,80,1627*32,3138*32,"\\script\\npcthanhthi\\duongchau\\trabacsi.lua",6,74)	
    -- AddNpcNew(224,1,80,1637*32,3128*32,"\\script\\npcthanhthi\\duongchau\\chutiemtra.lua",6,73)	
    -- AddNpcNew(243,1,80,1639*32,3127*32,"\\script\\npcthanhthi\\duongchau\\trakhach1.lua",6,72)
    -- AddNpcNew(245,1,80,1635*32,3132*32,"\\script\\npcthanhthi\\duongchau\\trakhach.lua",6,72)
	-- AddNpcNew(323,1,80,1654*32,3139*32,"\\script\\npcthanhthi\\duongchau\\dukhach.lua",6,237)	
	-- AddNpcNew(353,1,80,1628*32,3102*32,"\\script\\npcthanhthi\\duongchau\\hieuchan.lua",6,724)
	-- AddNpcNew(321,1,80,1600*32,3075*32,"\\script\\npcthanhthi\\duongchau\\anmay.lua",6)
	-- AddNpcNew(321,1,80,1732*32,2980*32,"\\script\\npcthanhthi\\duongchau\\anmay.lua",6)
	-- AddNpcNew(313,1,80,1661*32,3228*32,"\\script\\npcthanhthi\\duongchau\\lieudaigia.lua",6,739)
	-- AddNpcNew(256,1,80,1756*32,3125*32,"\\script\\npcthanhthi\\duongchau\\uyennuong.lua",6,735)	
	-- AddNpcNew(372,1,80,1798*32,3156*32,"\\script\\npcthanhthi\\duongchau\\trandailuc.lua",6,737)	
	-- AddNpcNew(286,1,80,1830*32,3132*32,"\\script\\global\\npcchucnang\\thietchuy.lua",6,36)
	-- AddNpcNew(381,1,80,1799*32,3053*32,"\\script\\npcthanhthi\\duongchau\\trantau.lua",6,652)
	-- AddNpcNew(361,1,80,1805*32,3048*32,"\\script\\npcthanhthi\\duongchau\\chauchau.lua",6,722)
	-- AddNpcNew(662,1,80,1849*32,3050*32,"\\script\\npcthanhthi\\duongchau\\honhon.lua",6)
	-- AddNpcNew(248,1,80,1697*32,2984*32,"\\script\\npcthanhthi\\duongchau\\tongbaba.lua",6,718)	
	-- AddNpcNew(316,1,80,1623*32,3052*32,"\\script\\npcthanhthi\\duongchau\\vienngoai.lua",6,720)	
	-- AddNpcNew(326,1,80,1632*32,3075*32,"\\script\\npcthanhthi\\duongchau\\giangnhattieu.lua",6,723)	
	-- AddNpcNew(330,1,80,1721*32,3052*32,"\\script\\npcthanhthi\\duongchau\\tongtuvan.lua",6,721)	
	-- AddNpcNew(275,1,80,1701*32,3060*32,"\\script\\npcthanhthi\\duongchau\\tongtuvan.lua",6,77)
	-- AddNpcNew(358,1,80,1687*32,3078*32,"\\script\\npcthanhthi\\duongchau\\thamphong.lua",6,730)
	-- AddNpcNew(356,1,80,1692*32,3086*32,"\\script\\npcthanhthi\\duongchau\\nhuy.lua",6,729)	
	-- AddNpcNew(220,1,80,1681*32,3082*32,"\\script\\npcthanhthi\\duongchau\\thamphunhan.lua",6,728)	
	-- AddNpcNew(355,1,80,1679*32,3087*32,"\\script\\npcthanhthi\\duongchau\\soso.lua",6,726)	
	-- AddNpcNew(331,1,80,1675*32,3089*32,"\\script\\npcthanhthi\\duongchau\\tongvienngoai.lua",6,727)	
	-- AddNpcNew(218,1,80,1652*32,3081*32,"\\script\\npcthanhthi\\duongchau\\cogiatiensinh.lua",6)	
	-- AddNpcNew(217,1,80,1645*32,3074*32,"\\script\\npcthanhthi\\duongchau\\tiemcamdo.lua",6)	
	-- AddNpcNew(363,1,80,1699*32,2944*32,"\\script\\npcthanhthi\\duongchau\\begai.lua",6,716)	
	-- AddNpcNew(175,1,80,1706*32,2829*32,"\\script\\npcthanhthi\\duongchau\\trieudachu.lua",6,713)	
	-- AddNpcNew(64,1,80,1754*32,2939*32,"\\script\\npcthanhthi\\duongchau\\trieudachu.lua",6,715)	
	-- AddNpcNew(334,1,80,1733*32,3029*32,"\\script\\npcthanhthi\\duongchau\\tuybantien.lua",6,883)	
	-- AddNpcNew(215,1,80,1730*32,3034*32,"\\script\\npcthanhthi\\duongchau\\chutuulau.lua",6,671)	
	-- AddNpcNew(308,1,80,1594*32,3113*32,"\\script\\npcthanhthi\\duongchau\\tieudau.lua",6,731)	
	-- AddNpcNew(136,1,80,1618*32,3047*32,"\\script\\npcthanhthi\\duongchau\\dukhongkhong.lua",6,"D­ Kh«ng Kh«ng")	
	-- AddNpcNew(359,1,80,56558,99976,"\\script\\npcthanhthi\\duongchau\\nhamthiennhai.lua",6,736)
	-- AddNpcNew(242,1,80,42977,107638,"\\script\\npcthanhthi\\duongchau\\thuyenphu.lua",6,31);
end;

function addtrapduongchau()
	AddTrapEx2(80,1571,3231,10,"\\script\\maps\\duongchau\\trap\\cong8h.lua")
	AddTrapEx1(80,1643,2997,10,"\\script\\maps\\duongchau\\trap\\cong10h.lua")
	AddTrapEx2(80,1873,3013,10,"\\script\\maps\\duongchau\\trap\\cong2h.lua")
	AddTrapEx1(80,1739,3259,10,"\\script\\maps\\duongchau\\trap\\cong4h.lua")

end;

function addobjduongchau()
	--AddObj(1,80,53157,95427,"",0,0)
	--AddObj(1,80,56227,96170,"",0,0)
	-- AddObj(4,80,53385,96314,"\\script\\maps\\bangcaothi.lua",0,0)
	-- AddObj(3,80,56363,96312,"",0,0)
	--AddObj(1,80,59370,96983,"",0,0)
	-- AddObj(4,80,59785,97177,"\\script\\maps\\bangcaothi.lua",0,0)
	-- AddObj(294,80,46650,98737,"",0,0)
	--AddObj(1,80,52068,101619,"",0,0)
	-- AddObj(4,80,51221,102905,"\\script\\maps\\bangcaothi.lua",0,0)
	--AddObj(1,80,54780,103159,"",0,0)
	-- AddObj(4,80,54613,103291,"\\script\\maps\\bangcaothi.lua",0,0)
end;
