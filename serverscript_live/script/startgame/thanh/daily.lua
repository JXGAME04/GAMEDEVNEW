-- @IncludeOnce  [LUA54 06/09 toi] tep toan ham: Lua54Dll chi chay than mot lan moi state (chon_includeonce.py)
--hoangnhk
function addnpcdaily()
	local nNpcIdx;	
	----NPC Chuc nang-----
    nNpcIdx = AddNpcNew(244,1,162,1536*32,3200*32,"\\script\\global\\npcchucnang\\taphoa.lua",6,84);SetNpcValue(nNpcIdx, 17);
	nNpcIdx = AddNpcNew(197,1,162,1559*32,3260*32,"\\script\\global\\npcchucnang\\thoren.lua",6,55);SetNpcValue(nNpcIdx, 4);
	nNpcIdx = AddNpcNew(203,1,162,1499*32,3209*32,"\\script\\global\\npcchucnang\\hieuthuoc.lua",6,51);SetNpcValue(nNpcIdx, 12);
	nNpcIdx = AddNpcNew(234,1,162,1639*32,3148*32,"\\script\\global\\npcchucnang\\banngua.lua",6,64);SetNpcValue(nNpcIdx, 45);
	nNpcIdx = AddObj(1,162,1626*32,3151*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,64);
	nNpcIdx = AddObj(1,162,50455,100238,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,63);
	nNpcIdx = AddNpcNew(377,1,162,1676*32,3115*32,"\\script\\global\\npcchucnang\\dichquan.lua",6,47);--SetNpcValue(nNpcIndex,50); 
	
	AddNpcNew(393,1,162,1667*32,3126*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	AddNpcNew(236,1,162,1469*32,3269*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	AddNpcNew(237,1,162,1698*32,3276*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	AddNpcNew(108,1,162,1651*32,3226*32,"\\script\\global\\seasonnpc.lua",6,59)
	AddNpcNew(377,1,162,1586*32,3221*32,"\\script\\global\\npcchucnang\\lequan.lua",6,57)
	-- nNpcIdx = AddNpcEx1({769},1,nil,162,1573*32,3228*32,"",OTHER_SGSTHU,nil,6) SetNpcValue(nNpcIdx, 6)--nhiep thi tran dai ly
	AddNpcNew(308,1,162,1630*32,3185*32,"\\script\\global\\npcchucnang\\truyennhan.lua",6,65)
	AddNpcNew(308,1,162,1599*32,3148*32,"\\script\\global\\npcchucnang\\sgkietxuat.lua",6,"Sø gi¶ liªn ®Êu") 
	--AddNpcNew(87,1,162,1592*32,3168*32,"\\script\\global\\npcchucnang\\sgliendau.lua",6,"Sø gi¶ liªn ®Êu") 
	AddNpcNew(62,1,162,1682*32,3112*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbdong
	AddNpcNew(62,1,162,1688*32,3117*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbdong
	AddNpcNew(62,1,162,1729*32,3313*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbtay
	AddNpcNew(62,1,162,1722*32,3319*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbtay
	AddNpcNew(62,1,162,1435*32,3311*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbnam
	AddNpcNew(62,1,162,1442*32,3318*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbnam
	AddNpcNew(328,1,162,1595*32,3128*32,"\\script\\global\\npcchucnang\\nhamon.lua",6,532)
	AddNpcNew(328,1,162,1599*32,3132*32,"\\script\\global\\npcchucnang\\nhamon.lua",6,532)
    AddNpcNew(260,1,162,1672*32,3234*32,"\\script\\global\\npcchucnang\\hangrong.lua",6,48)	
    AddNpcNew(209,1,162,49163,103668,"\\script\\global\\npcchucnang\\chusongbac.lua",6,63)   
   --=====NPC daily======
	-- AddNpcNew(646,1,162,1470*32,3168*32,"\\script\\feature\\nvhoangkim\\macsau.lua",6)--nvhk
	-- AddNpcNew(648,1,162,1468*32,3166*32,"\\script\\feature\\nvhoangkim\\macxaonhi.lua",6)--nvhk
	-- AddNpcNew(217,1,162,1498*32,3240*32,"\\script\\npcthanhthi\\daily\\tiemcamdo.lua",6,40)
	-- AddNpcNew(313,1,162,1496*32,3242*32,"\\script\\npcthanhthi\\daily\\nguoithutien.lua",6,39)
	-- AddNpcNew(195,1,162,1603*32,3202*32,"\\script\\npcthanhthi\\daily\\chuluquan.lua",6,88)
    -- AddNpcNew(287,1,162,1648*32,3142*32,"\\script\\global\\npcchucnang\\thietchuy.lua",6,36)	
    -- AddNpcNew(211,1,162,1543*32,3247*32,"\\script\\npcthanhthi\\daily\\baoke.lua",6,61)        
	-- AddNpcNew(210,1,162,1535*32,3239*32,"\\script\\npcthanhthi\\daily\\baoke1.lua",6,61)        
    -- AddNpcNew(370,1,162,1527*32,3231*32,"\\script\\npcthanhthi\\daily\\conbac.lua",6,62)	    	
	-- AddNpcNew(380,1,162,49519,104048,"\\script\\npcthanhthi\\daily\\conbac1.lua",6,62)       
	-- AddNpcNew(373,1,162,1539*32,3256*32,"\\script\\npcthanhthi\\daily\\conbac2.lua",6,62)       
	-- AddNpcNew(196,1,162,1608*32,3197*32,"\\script\\npcthanhthi\\daily\\tieunhiluquan.lua",6,87) 	
	-- AddNpcNew(1645,1,162,1615*32,3220*32,"\\script\\npcthanhthi\\daily\\khachtro.lua",6,45) 
	-- AddNpcNew(367,1,162,1593*32,3229*32,"\\script\\npcthanhthi\\daily\\khachtro1.lua",6,45) 
	-- AddNpcNew(204,1,162,1581*32,3232*32,"\\script\\npcthanhthi\\daily\\phungphung.lua",6,"Phông Phông") 	
	-- AddNpcNew(213,1,162,1570*32,3208*32,"\\script\\npcthanhthi\\daily\\chutuudiem.lua",6,38) 
	-- AddNpcNew(214,1,162,1567*32,3206*32,"\\script\\npcthanhthi\\daily\\tuubao.lua",6,37) 
	-- AddNpcNew(196,1,162,1574*32,3212*32,"\\script\\npcthanhthi\\daily\\tuubao1.lua",6,37) 
	-- AddNpcNew(330,1,162,1576*32,3217*32,"\\script\\npcthanhthi\\daily\\tuukhach.lua",6,540) 
	-- AddNpcNew(331,1,162,1561*32,3198*32,"\\script\\npcthanhthi\\daily\\tuukhach1.lua",6,68) 
	-- AddNpcNew(329,1,162,1560*32,3193*32,"\\script\\npcthanhthi\\daily\\tuukhach1.lua",6,67) 
	-- AddNpcNew(323,1,162,1548*32,3185*32,"\\script\\npcthanhthi\\daily\\dinhabang.lua",6,537) 
	-- AddNpcNew(375,1,162,1530*32,3156*32,"\\script\\npcthanhthi\\daily\\doantuthanh.lua",6,534) 
	-- AddNpcNew(378,1,162,1515*32,3151*32,"\\script\\npcthanhthi\\daily\\trakhach.lua",6,72) 
	-- AddNpcNew(377,1,162,1491*32,3186*32,"\\script\\npcthanhthi\\daily\\trakhach1.lua",6,72) 
	-- AddNpcNew(225,1,162,1502*32,3176*32,"\\script\\npcthanhthi\\daily\\trabacsi.lua",6,74) 
	-- AddNpcNew(224,1,162,1506*32,3172*32,"\\script\\npcthanhthi\\daily\\chutiemtra.lua",6,73) 
	-- AddNpcNew(219,1,162,1499*32,3179*32,"\\script\\npcthanhthi\\daily\\tieunhiquantra.lua",6,75) 
	-- AddNpcNew(332,1,162,1472*32,3216*32,"\\script\\npcthanhthi\\daily\\dongkhacdi.lua",6,539) 
	-- AddNpcNew(348,1,162,1618*32,3173*32,"\\script\\npcthanhthi\\daily\\daolienanh.lua",6,538) 
	-- AddNpcNew(365,1,162,1635*32,3210*32,"\\script\\npcthanhthi\\daily\\hoanong.lua",6,541) 
	-- AddNpcNew(361,1,162,1682*32,3251*32,"\\script\\npcthanhthi\\daily\\vanvan.lua",6,543) 
	-- AddNpcNew(382,1,162,1447*32,3179*32,"\\script\\npcthanhthi\\daily\\longtruyvu.lua",6,536) 
	-- AddNpcNew(208,1,162,1532*32,3264*32,"\\script\\npcthanhthi\\daily\\tiemvatnuoi.lua",6,807) 
	-- AddNpcNew(279,1,162,1510*32,3276*32,"\\script\\npcthanhthi\\daily\\doanlaohan.lua",6,544) 
	-- AddNpcNew(337,1,162,1551*32,3292*32,"\\script\\npcthanhthi\\daily\\danhanhhoa.lua",6,545) 
	-- AddNpcNew(388,1,162,1565*32,3266*32,"\\script\\npcthanhthi\\daily\\hoctrothoren.lua",6,7) 
	-- AddNpcNew(312,1,162,1588*32,3266*32,"\\script\\npcthanhthi\\daily\\bachlaohan.lua",6,546) 
	-- AddNpcNew(318,1,162,1592*32,3268*32,"\\script\\npcthanhthi\\daily\\xalaothai.lua",6,547) 
	-- AddNpcNew(362,1,162,1631*32,3260*32,"\\script\\npcthanhthi\\daily\\aky.lua",6,542) 
	-- AddNpcNew(64,1,162,1760*32,3142*32,"\\script\\npcthanhthi\\daily\\huvienhoathuong.lua",6,535) 
	-- AddNpcNew(189,1,162,1762*32,3160*32,"\\script\\npcthanhthi\\daily\\hoathuong.lua",6,533) 
	-- AddNpcNew(189,1,162,55810,100284,"\\script\\npcthanhthi\\daily\\hoathuong1.lua",6,533) 
	-- AddNpcNew(189,1,162,57162,100164,"\\script\\npcthanhthi\\daily\\hoathuong2.lua",6,533) 
	-- AddNpcNew(189,1,162,57270,100268,"\\script\\npcthanhthi\\daily\\hoathuong3.lua",6,533) 
	-- AddNpcNew(318,1,162,1754*32,3161*32,"\\script\\npcthanhthi\\daily\\huongkhach.lua",6,34) 
	-- AddNpcNew(344,1,162,1746*32,3150*32,"\\script\\npcthanhthi\\daily\\huongkhach1.lua",6,34) 
	-- AddNpcNew(379,1,162,1737*32,3148*32,"\\script\\npcthanhthi\\daily\\huongkhach2.lua",6,34) 
	-- AddNpcNew(329,1,162,1579*32,3154*32,"\\script\\global\\npcchucnang\\tetuu.lua",6,"TÕ töu") 
	-- AddNpcNew(204,1,162,1511*32,3218*32,"\\script\\npcthanhthi\\daily\\cogaihaithuoc.lua",6,484) 
	-- AddNpcNew(343,1,162,48121,105824,"\\script\\npcthanhthi\\daily\\ngocphung.lua",6,548)       
	-- AddNpcNew(242,1,162,52284,93840,"\\script\\npcthanhthi\\daily\\thuyenphu.lua",6,31);
--thuyen gia 52471,93644
end;

function addtrapdaily()
	AddTrapEx2(162,1437,3307,10,"\\script\\maps\\daily\\trap\\cong8h.lua")
	AddTrapEx2(162,1683,3108,10,"\\script\\maps\\daily\\trap\\cong2h.lua")
	AddTrapEx1(162,1720,3317,10,"\\script\\maps\\daily\\trap\\cong4h.lua")

end;

function addobjdaily()
	--AddObj(1,162,50452,100235,"",1,0)
	-- AddObj(4,162,53753,100250,"\\script\\maps\\bangcaothi.lua",0,0)
	-- AddObj(273,162,46703,101180,"",0,0)
	-- AddObj(273,162,46648,100507,"",0,0)
	-- AddObj(273,162,46652,100689,"",0,0)
	-- AddObj(274,162,47566,100623,"",0,0)
	-- AddObj(274,162,47216,100654,"",0,0)
	--AddObj(1,162,52049,100877,"",1,0)
	-- AddObj(294,162,40978,104149,"",0,0)
	-- AddObj(4,162,47469,104839,"\\script\\maps\\bangcaothi.lua",0,0)
	-- AddObj(3,162,54063,105205,"",0,0)
	-- AddObj(296,162,58953,107733,"",0,0)
	-- AddObj(295,162,54017,112169,"",0,0)	
end;
