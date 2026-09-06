--hoangnhk
function addnpclaman()
	local nNpcIdx;
	----NPC Chuc nang-----
	nNpcIdx = AddNpcNew(203,1,176,1684*32,3318*32,"\\script\\global\\npcchucnang\\hieuthuoc.lua",6,51);SetNpcValue(nNpcIdx, 12);
	nNpcIdx = AddNpcNew(203,1,176,1477*32,3362*32,"\\script\\global\\npcchucnang\\hieuthuoc.lua",6,51);SetNpcValue(nNpcIdx, 12);
	nNpcIdx = AddNpcNew(203,1,176,1544*32,2963*32,"\\script\\global\\npcchucnang\\hieuthuoc.lua",6,51);SetNpcValue(nNpcIdx, 12);
	nNpcIdx = AddNpcNew(203,1,176,1616*32,2976*32,"\\script\\global\\npcchucnang\\hieuthuoc.lua",6,51);SetNpcValue(nNpcIdx, 12);
	nNpcIdx = AddNpcNew(315,1,176,1339*32,3165*32,"\\script\\global\\npcchucnang\\taphoa.lua",6,84);SetNpcValue(nNpcIdx, 2);
	nNpcIdx = AddNpcNew(197,1,176,1333*32,3060*32,"\\script\\global\\npcchucnang\\thoren.lua",6,55);SetNpcValue(nNpcIdx, 1);
	nNpcIdx = AddNpcNew(198,1,176,1684*32,3221*32,"\\script\\global\\npcchucnang\\thoren.lua",6,55);SetNpcValue(nNpcIdx, 1);
	nNpcIdx = AddNpcNew(197,1,176,1519*32,2984*32,"\\script\\global\\npcchucnang\\thoren.lua",6,55);SetNpcValue(nNpcIdx, 1);
	nNpcIdx = AddNpcNew(231,1,176,1494*32,3014*32,"\\script\\global\\npcchucnang\\banngua.lua",6,64);SetNpcValue(nNpcIdx, 42);
	nNpcIdx = AddNpcNew(234,1,176,1492*32,3016*32,"\\script\\global\\npcchucnang\\banngua.lua",6,64);SetNpcValue(nNpcIdx, 42);
	nNpcIdx = AddObj(1,176,1688*32,3277*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,68); --dong--dong
	nNpcIdx = AddObj(1,176,1391*32,3312*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,67);  --nam
	nNpcIdx = AddObj(1,176,1574*32,2933*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,69); --bac
    nNpcIdx = AddNpcNew(377,1,176,1590*32,2925*32,"\\script\\global\\npcchucnang\\dichquan.lua",6,47);--SetNpcValue(nNpcIdx,50);
	
	AddNpcNew(239,1,176,1696*32,3293*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42) -- dong
	AddNpcNew(239,1,176,1372*32,3331*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42) -- nam
	AddNpcNew(393,1,176,1352*32,3014*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42) -- tay
	AddNpcNew(239,1,176,1601*32,2913*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	AddNpcNew(203,1,176,1617*32,29176*32,"\\script\\global\\npcchucnang\\hieuthuoc.lua",6,51)
	AddNpcNew(108,1,176,1562*32,2979*32,"\\script\\global\\seasonnpc.lua",6,59)
	AddNpcNew(377,1,176,1506*32,2983*32,"\\script\\global\\npcchucnang\\lequan.lua",6,57)
	AddNpcNew(308,1,176,1535*32,3011*32,"\\script\\global\\npcchucnang\\truyennhan.lua",6,65)
    --AddNpcNew(87,1,176,1452*32,3279*32,"\\script\\global\\npcchucnang\\sgliendau.lua",6,"Sø gi¶ liªn ®Êu")
	-- AddNpcNew(308,1,176,46632,104292,"\\script\\global\\npcchucnang\\sgkietxuat.lua",6,"Sø gi¶ liªn ®Êu")
	-- nNpcIdx = AddNpcEx1({769},1,nil,176,1372*32,3010*32,"",OTHER_SGSTHU,nil,6) SetNpcValue(nNpcIdx, 5)--nhiep thi tran lam an
	AddNpcNew(209,1,176,1314*32,2847*32,"\\script\\global\\npcchucnang\\thuongnhan.lua",6,35)
	AddNpcNew(62,1,176,1719*32,3330*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbdong
	AddNpcNew(62,1,176,1714*32,3333*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbdong
	AddNpcNew(62,1,176,1349*32,3369*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbtay
	AddNpcNew(62,1,176,1353*32,3372*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbtay
	AddNpcNew(62,1,176,1622*32,2912*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbnam
	AddNpcNew(62,1,176,1615*32,2906*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbnam
	AddNpcNew(53,1,176,47625,103834,"\\script\\global\\npcchucnang\\nhadich.lua",6,96)
	AddNpcNew(387,1,176,1313*32,3194*32,"\\script\\global\\npcchucnang\\thorenthanbi.lua",6,"Thî rÌn thÇn bÝ ")
    AddNpcNew(260,1,176,1389*32,2960*32,"\\script\\global\\npcchucnang\\hangrong.lua",6,48)
    AddNpcNew(209,1,176,1458*32,3028*32,"\\script\\global\\npcchucnang\\chusongbac.lua",6,63)   
	AddNpcNew(228,1,176,1442*32,3061*32,"\\script\\global\\npcchucnang\\tientrang.lua",6,70)
	--=====NPC laman======
	-- AddNpcNew(250,1,176,1629*32,3210*32,"\\script\\feature\\nvhoangkim\\congcong.lua",6,"Th¸i C«ng C«ng")	--nvhk	
    -- AddNpcNew(314,1,176,1630*32,2992*32,"\\script\\feature\\nvhoangkim\\manhpham.lua",6,"M¹nh Phµm")--nvhk
    -- AddNpcNew(319,1,176,1449*32,3024*32,"\\script\\npcthanhthi\\laman\\conbac.lua",6,62)       
    -- AddNpcNew(329,1,176,46353,97360,"\\script\\npcthanhthi\\laman\\conbac2.lua",6,62)       
	-- AddNpcNew(210,1,176,1456*32,3026*32,"\\script\\npcthanhthi\\laman\\baoke.lua",6,61)       
	-- AddNpcNew(210,1,176,1460*32,3031*32,"\\script\\npcthanhthi\\laman\\baoke1.lua",6,61)        
    -- AddNpcNew(334,1,176,1462*32,3037*32,"\\script\\npcthanhthi\\laman\\conbac1.lua",6,62)	     
	-- AddNpcNew(62,1,176,1473*32,3233*32,"\\script\\npcthanhthi\\laman\\vebinhhoangcung.lua",6,91)
	-- AddNpcNew(62,1,176,1470*32,3236*32,"\\script\\npcthanhthi\\laman\\vebinhhoangcung.lua",6,91)
	-- AddNpcNew(62,1,176,1467*32,3239*32,"\\script\\npcthanhthi\\laman\\vebinhhoangcung.lua",6,91)
	-- AddNpcNew(62,1,176,1478*32,3250*32,"\\script\\npcthanhthi\\laman\\vebinhhoangcung.lua",6,91)
	-- AddNpcNew(62,1,176,1481*32,3246*32,"\\script\\npcthanhthi\\laman\\vebinhhoangcung.lua",6,91)
	-- AddNpcNew(62,1,176,1484*32,3243*32,"\\script\\npcthanhthi\\laman\\vebinhhoangcung.lua",6,91)
	-- AddNpcNew(62,1,176,1610*32,3229*32,"\\script\\npcthanhthi\\laman\\vebinhhoangcung.lua",6,91)
	-- AddNpcNew(62,1,176,1618*32,3221*32,"\\script\\npcthanhthi\\laman\\vebinhhoangcung.lua",6,91)
	-- AddNpcNew(62,1,176,1636*32,3204*32,"\\script\\npcthanhthi\\laman\\vebinhhoangcung.lua",6,91)
	-- AddNpcNew(1645,1,176,1623*32,3226*32,"\\script\\npcthanhthi\\laman\\congsai.lua",6,93)	
	-- AddNpcNew(48,1,176,1460*32,3249*32,"\\script\\npcthanhthi\\laman\\nhacnho.lua",6,"Quan nh¾c nhë")	
    -- AddNpcNew(55,1,176,1476*32,3234*32,"\\script\\npcthanhthi\\laman\\thonglinhcamve.lua",6,95)
	-- AddNpcNew(251,1,176,1630*32,3202*32,"\\script\\npcthanhthi\\laman\\tieuthaigiam.lua",6,92)
    -- AddNpcNew(329,1,176,1561*32,2942*32,"\\script\\global\\npcchucnang\\tetuu.lua",6,"TÕ töu")
    -- AddNpcNew(328,1,176,1567*32,2922*32,"\\script\\npcthanhthi\\laman\\khachtro.lua",6,45)
    -- AddNpcNew(244,1,176,1522*32,2898*32,"\\script\\npcthanhthi\\laman\\chutuudiem.lua",6,38)
    -- AddNpcNew(214,1,176,1526*32,2894*32,"\\script\\npcthanhthi\\laman\\tuubao.lua",6,37)
    -- AddNpcNew(316,1,176,1551*32,2892*32,"\\script\\npcthanhthi\\laman\\chutiemcamdo.lua",6,40)
    -- AddNpcNew(218,1,176,1548*32,2892*32,"\\script\\npcthanhthi\\laman\\nguoithutien.lua",6,41)
    -- AddNpcNew(219,1,176,1546*32,2895*32,"\\script\\npcthanhthi\\laman\\nguoithutientiemcamdo.lua",6,41)
    -- AddNpcNew(344,1,176,1550*32,2900*32,"\\script\\npcthanhthi\\laman\\congaitiemcamdo.lua",6,809)
    -- AddNpcNew(201,1,176,1521*32,2983*32,"\\script\\npcthanhthi\\laman\\hoctrothoren.lua",6,58)
    -- AddNpcNew(256,1,176,1486*32,2958*32,"\\script\\npcthanhthi\\laman\\thomay.lua",6,50)
    -- AddNpcNew(194,1,176,1310*32,2757*32,"\\script\\npcthanhthi\\laman\\suthai.lua",6,32)
    -- AddNpcNew(193,1,176,1313*32,2754*32,"\\script\\npcthanhthi\\laman\\nico.lua",6,33)
    -- AddNpcNew(309,1,176,1325*32,2765*32,"\\script\\npcthanhthi\\laman\\huongkhach.lua",6,34)
    -- AddNpcNew(350,1,176,1317*32,2760*32,"\\script\\npcthanhthi\\laman\\huongkhach1.lua",6,34)
    -- AddNpcNew(358,1,176,1316*32,2783*32,"\\script\\npcthanhthi\\laman\\huongkhach2.lua",6,34)
    -- AddNpcNew(318,1,176,1330*32,2785*32,"\\script\\npcthanhthi\\laman\\huongkhach3.lua",6,34)
    -- AddNpcNew(286,1,176,1337*32,2910*32,"\\script\\global\\npcchucnang\\thietchuy.lua",6,36)
    -- AddNpcNew(661,1,176,1368*32,3050*32,"\\script\\npcthanhthi\\laman\\lieunamvan.lua",6,"LiÔu Nam V©n")
    -- AddNpcNew(312,1,176,1394*32,3074*32,"\\script\\npcthanhthi\\laman\\tadaigia.lua",6,76)
    -- AddNpcNew(257,1,176,1395*32,3112*32,"\\script\\npcthanhthi\\laman\\tiemtodieu.lua",6,83)
    -- AddNpcNew(290,1,176,1365*32,3147*32,"\\script\\npcthanhthi\\laman\\tiemtranh.lua",6,85)
    -- AddNpcNew(358,1,176,1339*32,3230*32,"\\script\\npcthanhthi\\laman\\thusinhngheo.lua",6,86)
    -- AddNpcNew(274,1,176,1404*32,3291*32,"\\script\\npcthanhthi\\laman\\totamnuong.lua",6,77)
    -- AddNpcNew(359,1,176,1441*32,3243*32,"\\script\\npcthanhthi\\laman\\dinhvienngoai.lua",6,94)
    -- AddNpcNew(359,1,176,1455*32,3446*32,"\\script\\npcthanhthi\\laman\\thusinh.lua",6,101)
    -- AddNpcNew(48,1,176,1481*32,3415*32,"\\script\\npcthanhthi\\laman\\truongtuan.lua",6,100)
    -- AddNpcNew(207,1,176,1493*32,3377*32,"\\script\\npcthanhthi\\laman\\tiemvatnuoi.lua",6,807)
    -- AddNpcNew(205,1,176,1490*32,3375*32,"\\script\\npcthanhthi\\laman\\tiemcamdo.lua",6,99)
    -- AddNpcNew(246,1,176,1469*32,3359*32,"\\script\\npcthanhthi\\laman\\nguoihaithuoc.lua",6,97)
    -- AddNpcNew(347,1,176,1462*32,3351*32,"\\script\\npcthanhthi\\laman\\condautagia.lua",6,98)
    -- AddNpcNew(321,1,176,1693*32,3171*32,"\\script\\npcthanhthi\\laman\\anmay.lua",6)
    -- AddNpcNew(321,1,176,1654*32,3015*32,"\\script\\npcthanhthi\\laman\\anmay.lua",6)	
    -- AddNpcNew(363,1,176,1744*32,3102*32,"\\script\\npcthanhthi\\laman\\hinhi.lua",6,81)
    -- AddNpcNew(360,1,176,1732*32,3090*32,"\\script\\npcthanhthi\\laman\\tieuxuyen.lua",6,80)
    -- AddNpcNew(344,1,176,1698*32,3059*32,"\\script\\npcthanhthi\\laman\\tieungoc.lua",6,71)
    -- AddNpcNew(244,1,176,1689*32,3029*32,"\\script\\npcthanhthi\\laman\\chutuudiem.lua",6,38)	
    -- AddNpcNew(214,1,176,1687*32,3035*32,"\\script\\npcthanhthi\\laman\\tuubao.lua",6,37)	
    -- AddNpcNew(308,1,176,1694*32,3034*32,"\\script\\npcthanhthi\\laman\\tuuquy.lua",6,69)	
    -- AddNpcNew(334,1,176,1683*32,3038*32,"\\script\\npcthanhthi\\laman\\tuukhach.lua",6,68)	
    -- AddNpcNew(335,1,176,1681*32,3040*32,"\\script\\npcthanhthi\\laman\\tuukhach.lua",6,67)	
    -- AddNpcNew(311,1,176,1643*32,3004*32,"\\script\\npcthanhthi\\laman\\longgioisinh.lua",6,60)		
    -- AddNpcNew(330,1,176,1600*32,2961*32,"\\script\\npcthanhthi\\laman\\truongtieutuyen.lua",6,53)	
    -- AddNpcNew(261,1,176,1581*32,2934*32,"\\script\\npcthanhthi\\laman\\nguoibandau.lua",6,46)	
    -- AddNpcNew(245,1,176,46903,93550,"\\script\\npcthanhthi\\laman\\quanvienthoainhiem.lua",6,44)	
	-- AddNpcNew(267,1,176,45734,98562,"\\script\\npcthanhthi\\laman\\quantra.lua",6,78)	
    -- AddNpcNew(275,1,176,45390,98890,"\\script\\npcthanhthi\\laman\\totamnuong.lua",6,77)	
    -- AddNpcNew(375,1,176,47837,97650,"\\script\\npcthanhthi\\laman\\thuatsi.lua",6,869)
    -- AddNpcNew(195,1,176,43660,103252,"\\script\\npcthanhthi\\laman\\chuluquan.lua",6,88)	
    -- AddNpcNew(196,1,176,43473,103410,"\\script\\npcthanhthi\\laman\\tieunhiluquan.lua",6,87)
    -- AddNpcNew(316,1,176,43121,99390,"\\script\\npcthanhthi\\laman\\hoavienngoai.lua",6,82)
    -- AddNpcNew(329,1,176,41744,98820,"\\script\\npcthanhthi\\laman\\trakhach.lua",6,72)
    -- AddNpcNew(327,1,176,41709,99104,"\\script\\npcthanhthi\\laman\\trakhach.lua",6,72)
    -- AddNpcNew(336,1,176,42148,98984,"\\script\\npcthanhthi\\laman\\trakhach.lua",6,72)
    -- AddNpcNew(47,1,176,41721,99326,"\\script\\npcthanhthi\\laman\\hanhkiem.lua",6,"DiÖp Hµnh KiÖm")
    -- AddNpcNew(225,1,176,42063,98448,"\\script\\npcthanhthi\\laman\\trabacsi.lua",6)
    -- AddNpcNew(224,1,176,42187,98368,"\\script\\npcthanhthi\\laman\\chutiemtra.lua",6)
    -- AddNpcNew(214,1,176,42278,98406,"\\script\\npcthanhthi\\laman\\tieunhiquantra.lua",6,75)
	-- AddNpcNew(242,1,176,51341,81602,"\\script\\npcthanhthi\\laman\\thuyenphu.lua",6,31);
end;

function addtraplaman()
	AddTrapEx2(176,1349,3364,10,"\\script\\maps\\laman\\trap\\cong8h.lua")
	AddTrapEx2(176,1616,2901,10,"\\script\\maps\\laman\\trap\\cong2h.lua")
	AddTrapEx1(176,1711,3332,10,"\\script\\maps\\laman\\trap\\cong4h.lua")

end;

function addobjlaman()	
	-- AddObj(4,176,51494,93094,"\\script\\maps\\bangcaothi.lua",0,0)
	--AddObj(1,176,50369,93883,"",0,0)
	--AddObj(1,176,54041,104922,"",0,0)
	--AddObj(1,176,44540,106039,"",1,0)
	-- AddObj(3,176,54112,106006,"",0,0)
	-- AddObj(296,176,56497,109825,"",0,0)	
end;
