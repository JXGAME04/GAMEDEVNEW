--hoangnhk
function addnpcthanhdo() 
	local nNpcIdx;
	----NPC Chuc nang-----
	nNpcIdx = AddNpcNew(357,1,11,3094*32,5135*32,"\\script\\global\\npcchucnang\\taphoa.lua",6,84);SetNpcValue(nNpcIdx, 14);
	nNpcIdx = AddNpcNew(199,1,11,3104*32,5120*32,"\\script\\global\\npcchucnang\\thoren.lua",6,55);SetNpcValue(nNpcIdx, 1);   
	nNpcIdx = AddNpcNew(230,1,11,3163*32,5093*32,"\\script\\global\\npcchucnang\\banngua.lua",6,64);SetNpcValue(nNpcIdx, 46);
	nNpcIdx = AddNpcNew(203,1,11,3142*32,5133*32,"\\script\\global\\npcchucnang\\hieuthuoc.lua",6,51);SetNpcValue(nNpcIdx, 12);
	nNpcIdx = AddObj(1,11,3157*32,5076*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,5); --giua
	nNpcIdx = AddObj(1,11,3186*32,5180*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,6); --dong  	
	nNpcIdx = AddObj(1,11,3054*32,4983*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,7); --tay	
	nNpcIdx = AddObj(1,11,3001*32,5104*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,8); --nam	
	nNpcIdx = AddObj(1,11,3237*32,5028*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,9); --bac
    nNpcIdx = AddNpcNew(377,1,11,3018*32,5087*32,"\\script\\global\\npcchucnang\\dichquan.lua",6,47);--SetNpcValue(nNpcIdx,50);--passerby181 
	
	AddNpcNew(238,1,11,3262*32,5004*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	AddNpcNew(238,1,11,3028*32,4957*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	AddNpcNew(238,1,11,3195*32,5190*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	AddNpcNew(238,1,11,3014*32,5092*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	AddNpcNew(108,1,11,3154*32,5067*32,"\\script\\global\\seasonnpc.lua",6,59)
	AddNpcNew(377,1,11,3117*32,5003*32,"\\script\\global\\npcchucnang\\lequan.lua",6,57)
	AddNpcNew(308,1,11,3134*32,5044*32,"\\script\\global\\npcchucnang\\truyennhan.lua",6,65)
	--AddNpcNew(309,1,11,3225*32,5043*32,"\\script\\global\\npcchucnang\\congbinhtu.lua",6,868)
	--AddNpcNew(309,1,11,3167*32,5194*32,"\\script\\global\\npcchucnang\\congbinhtu.lua",6,868)
	--AddNpcNew(309,1,11,3041*32,5097*32,"\\script\\global\\npcchucnang\\congbinhtu.lua",6,868)
	--AddNpcNew(309,1,11,3073*32,5004*32,"\\script\\global\\npcchucnang\\congbinhtu.lua",6,868)	
	nNpcIdx = AddNpcEx1({769},1,nil,11,3210*32,4974*32,"",OTHER_SGSTHU,nil,6) SetNpcValue(nNpcIdx, 7)--nhiep thi tran thanh do 	
	AddNpcNew(1645,1,11,3147*32,5048*32,"\\script\\global\\npcchucnang\\nhamon.lua",6,380)
	AddNpcNew(1645,1,11,3152*32,5053*32,"\\script\\global\\npcchucnang\\nhamon.lua",6,380)
	-- AddNpcNew(62,1,11,3222*32,5228*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbdong
	-- AddNpcNew(62,1,11,3216*32,5234*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbdong
	-- AddNpcNew(62,1,11,3016*32,4947*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbtay
	-- AddNpcNew(62,1,11,3010*32,4953*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbtay
	-- AddNpcNew(62,1,11,2977*32,5141*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbnam
	-- AddNpcNew(62,1,11,2983*32,5146*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbnam
	-- AddNpcNew(62,1,11,3288*32,4992*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbbac
	-- AddNpcNew(62,1,11,3281*32,4986*32,"\\script\\global\\npcchucnang\\vebinh.lua",6,4)-- vbbac	
	AddNpcNew(308,1,11,3213*32,5149*32,"\\script\\global\\npcchucnang\\sgkietxuat.lua",6,"Sø gi¶ liªn ®Êu")
	--AddNpcNew(87,1,11,3223*32,5158*32,"\\script\\global\\npcchucnang\\sgliendau.lua",6,"Sø gi¶ Liªn §Êu")	
    -- AddNpcNew(259,1,11,3181*32,4946*32,"\\script\\global\\npcchucnang\\hangrong.lua",6,48) 
    -- AddNpcNew(209,1,11,3083*32,5064*32,"\\script\\global\\npcchucnang\\chusongbac.lua",6,63)  
	--=====NPC thanhdo=====
	-- AddNpcNew(329,1,11,3226*32,5118*32,"\\script\\feature\\nvhoangkim\\congtutieu.lua",6,"C«ng Tö TiÕu" )--nvhk
	-- AddNpcNew(329,1,11,3096*32,5025*32,"\\script\\global\\npcchucnang\\tetuu.lua",6,"TÕ töu")		
	-- AddNpcNew(287,1,11,3130*32,5047*32,"\\script\\global\\npcchucnang\\thietchuy.lua",6,36)		
	-- AddNpcNew(211,1,11,3079*32,5061*32,"\\script\\npcthanhthi\\thanhdo\\baoke.lua",6,61)      
	-- AddNpcNew(210,1,11,3085*32,5067*32,"\\script\\npcthanhthi\\thanhdo\\baoke1.lua",6,61)       
    -- AddNpcNew(256,1,11,3120*32,5050*32,"\\script\\npcthanhthi\\thanhdo\\mainuong.lua",6,471) 
    -- AddNpcNew(327,1,11,3103*32,5037*32,"\\script\\npcthanhthi\\thanhdo\\thuanvu.lua",6,470) 	
    -- AddNpcNew(326,1,11,3105*32,5036*32,"\\script\\npcthanhthi\\thanhdo\\thachtriet.lua",6,469) 
    -- AddNpcNew(323,1,11,3239*32,5004*32,"\\script\\npcthanhthi\\thanhdo\\dunhan.lua",6,467)	
    -- AddNpcNew(305,1,11,3230*32,5035*32,"\\script\\npcthanhthi\\thanhdo\\xuanco.lua",6,472) 	
    -- AddNpcNew(275,1,11,3196*32,5069*32,"\\script\\npcthanhthi\\thanhdo\\tutau.lua",6,475) 	
    -- AddNpcNew(358,1,11,3186*32,5080*32,"\\script\\npcthanhthi\\thanhdo\\tutai.lua",6,474) 
    -- AddNpcNew(378,1,11,3177*32,5112*32,"\\script\\npcthanhthi\\thanhdo\\ongchuco.lua",6,480) 
    -- AddNpcNew(233,1,11,3163*32,5090*32,"\\script\\npcthanhthi\\thanhdo\\nguoichamsoc.lua",6,479)
    -- AddNpcNew(331,1,11,3138*32,5089*32,"\\script\\npcthanhthi\\thanhdo\\trautruongcuu.lua",6,478)
    -- AddNpcNew(212,1,11,3123*32,5107*32,"\\script\\npcthanhthi\\thanhdo\\chutuudiem.lua",6,38)
    -- AddNpcNew(345,1,11,3125*32,5105*32,"\\script\\npcthanhthi\\thanhdo\\chutuudiem.lua",6,5)	
    -- AddNpcNew(204,1,11,3132*32,5126*32,"\\script\\npcthanhthi\\thanhdo\\cogaihaithuoc.lua",6,484)
    -- AddNpcNew(217,1,11,3176*32,5171*32,"\\script\\npcthanhthi\\thanhdo\\tiemcamdo.lua",6)
    -- AddNpcNew(218,1,11,3168*32,5175*32,"\\script\\npcthanhthi\\thanhdo\\nguoithutien.lua",6,39)	
    -- AddNpcNew(365,1,11,3160*32,5157*32,"\\script\\npcthanhthi\\thanhdo\\chuluquan.lua",6,88)
    -- AddNpcNew(196,1,11,3161*32,5158*32,"\\script\\npcthanhthi\\thanhdo\\tieunhiluquan.lua",6,87)
    -- AddNpcNew(339,1,11,3152*32,5181*32,"\\script\\npcthanhthi\\thanhdo\\tonuyen.lua",6,489)
    -- AddNpcNew(359,1,11,3130*32,5237*32,"\\script\\npcthanhthi\\thanhdo\\tongcunhan.lua",6,491)
	-- AddNpcNew(321,1,11,3110*32,5212*32,"\\script\\npcthanhthi\\thanhdo\\anmay.lua",6)
	-- AddNpcNew(346,1,11,3089*32,5192*32,"\\script\\npcthanhthi\\thanhdo\\halanchi.lua",6,490)		
	-- AddNpcNew(221,1,11,3072*32,5158*32,"\\script\\npcthanhthi\\thanhdo\\bichhoa.lua",6,486)	
	-- AddNpcNew(355,1,11,3070*32,5160*32,"\\script\\npcthanhthi\\thanhdo\\yennhi.lua",6,487)
	-- AddNpcNew(383,1,11,3070*32,5153*32,"\\script\\npcthanhthi\\thanhdo\\truongtutau.lua",6,488)
	-- AddNpcNew(336,1,11,3081*32,5148*32,"\\script\\npcthanhthi\\thanhdo\\phuongquat.lua",6,483)
	-- AddNpcNew(200,1,11,3106*32,5118*32,"\\script\\npcthanhthi\\thanhdo\\hoctrothoren.lua",6,58)	
	-- AddNpcNew(353,1,11,3073*32,5048*32,"\\script\\npcthanhthi\\thanhdo\\tieumuoi.lua",6,468)	
	-- AddNpcNew(361,1,11,3068*32,5098*32,"\\script\\npcthanhthi\\thanhdo\\tieuminh.lua",6,477)	
	-- AddNpcNew(363,1,11,3066*32,5100*32,"\\script\\npcthanhthi\\thanhdo\\thainhi.lua",6,476)
	-- AddNpcNew(269,1,11,3032*32,5071*32,"\\script\\npcthanhthi\\thanhdo\\nhitham.lua",6,473)
	-- AddNpcNew(308,1,11,3015*32,5059*32,"\\script\\npcthanhthi\\thanhdo\\tieudau.lua",6,379)	
	-- AddNpcNew(335,1,11,3083*32,5012*32,"\\script\\npcthanhthi\\thanhdo\\tieuhonhon.lua",6,465)
	-- AddNpcNew(285,1,11,3192*32,5125*32,"\\script\\npcthanhthi\\thanhdo\\doanthantoan.lua",6,485)	
	-- AddNpcNew(64,1,11,3226*32,5108*32,"\\script\\npcthanhthi\\thanhdo\\tuvanphapsu.lua",6,482)	
	-- AddNpcNew(189,1,11,3229*32,5112*32,"\\script\\npcthanhthi\\thanhdo\\tutang.lua",6,481)	
	-- AddNpcNew(351,1,11,3216*32,5109*32,"\\script\\npcthanhthi\\thanhdo\\huongkhach.lua",6,34)	
	-- AddNpcNew(385,1,11,3221*32,5114*32,"\\script\\npcthanhthi\\thanhdo\\huongkhach1.lua",6,34)
	-- AddNpcNew(376,1,11,3217*32,5124*32,"\\script\\npcthanhthi\\thanhdo\\huongkhach2.lua",6,34)	
	-- AddNpcNew(329,1,11,99596,165812,"\\script\\npcthanhthi\\thanhdo\\trakhach.lua",6,72) 
	-- AddNpcNew(224,1,11,99640,165764,"\\script\\npcthanhthi\\thanhdo\\tiemtra.lua",6)
	-- AddNpcNew(205,1,11,96800,164172,"\\script\\npcthanhthi\\thanhdo\\tiemvatnuoi.lua",6,99)
	-- AddNpcNew(206,1,11,96705,164090,"\\script\\npcthanhthi\\thanhdo\\tiemvatnuoi1.lua",6,807) 
	-- AddNpcNew(318,1,11,99531,160308,"\\script\\npcthanhthi\\thanhdo\\tieubaba.lua",6,466)	
	-- AddNpcNew(366,1,11,102537,158384,"\\script\\npcthanhthi\\thanhdo\\tanvienngoai.lua",6,463)
	-- AddNpcNew(313,1,11,103489,159508,"\\script\\npcthanhthi\\thanhdo\\nguydaigia.lua",6,464)	
end;

function addtrapthanhdo()
	AddTrapEx2(11,2978,5137,10,"\\script\\maps\\thanhdo\\trap\\cong8h.lua")
	AddTrapEx1(11,3008,4951,10,"\\script\\maps\\thanhdo\\trap\\cong10h.lua")
	AddTrapEx2(11,3283,4982,10,"\\script\\maps\\thanhdo\\trap\\cong2h.lua")
	AddTrapEx1(11,3212,5232,10,"\\script\\maps\\thanhdo\\trap\\cong4h.lua")

end;

function addobjthanhdo()
	-- SetObjValue(AddObj(297,11,104804,147468,"\\script\\maps\\thanhdo\\obj\\dauhieu.lua",0,0),1)
	-- AddObj(283,11,96006,152457,"",0,0)
	-- SetObjValue(AddObj(294,11,90549,153519,"\\script\\maps\\thanhdo\\obj\\dauhieu.lua",0,0),2)
	-- AddObj(283,11,94915,153457,"",0,0)
	-- AddObj(283,11,97238,153353,"",0,0)
	-- AddObj(283,11,96047,154249,"",0,0)
	-- AddObj(283,11,98704,153639,"",0,0)
	-- AddObj(283,11,96816,155304,"",0,0)
	-- AddObj(283,11,98929,155189,"",0,0)
	-- AddObj(4,11,96242,158621,"\\script\\maps\\bangcaothi.lua",0,0)
	--AddObj(1,11,97728,159419,"",0,0)
	-- AddObj(3,11,3277*32,4987*32,"\\script\\maps\\bangcaothi.lua",1,0)
	--AddObj(1,11,103709,160803,"",0,0)
	-- SetObjValue(AddObj(295,11,91413,162620,"\\script\\maps\\thanhdo\\obj\\dauhieu.lua",0,0),3)
	-- AddObj(3,11,101120,162556,"\\script\\maps\\thanhdo\\obj\\bangniemyet.lua",0,0)
	-- AddObj(3,11,101227,162668,"",0,0)
	--AddObj(1,11,101022,162451,"",0,0)
	--AddObj(1,11,96025,163348,"",0,0)
	-- AddObj(6,11,96381,163618,"",0,0)
	--AddObj(1,11,101914,165717,"",0,0)
	-- AddObj(3,11,102088,166413,"\\script\\maps\\bangcaothi.lua",0,0)
	-- AddObj(296,11,108786,168843,"",0,0)
	-- SetObjValue(AddObj(295,11,86997,170033,"\\script\\maps\\thanhdo\\obj\\dauhieu.lua",0,0),4)
	-- SetObjValue(AddObj(296,11,103227,173235,"\\script\\maps\\thanhdo\\obj\\dauhieu.lua",0,0),5)
end;
