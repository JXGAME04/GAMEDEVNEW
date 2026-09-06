--hoangnhk
function addnpcother()
	
	--Npc Hoa s¬n
	local nNpcIdx;
 --   AddNpcNew(238,1,2,2616*32,3586*32,"\\script\\npckhac\\hoason\\xaphuhoason.lua",6,"DÞch Tr¹m Xa Phu")
 --   AddNpcNew(245,1,2,2602*32,3591*32,"\\script\\npckhac\\hoason\\laotau.lua",6,"Hoa S¬n L·o TÈu")
--	AddNpcNew(189,1,2,2616*32,3593*32,"\\script\\npckhac\\hoason\\thuongbachly.lua",6,"ThiÕu L©m §Ö Tö Th­êng B¸ch Lý ")
--	AddNpcNew(189,1,2,2618*32,3597*32,"\\script\\npckhac\\hoason\\thuongbachly.lua",6,"ThiÕu L©m §Ö Tö Lý §Þch Phong")
 --   AddNpcNew(117,1,2,2351*32,3495*32,"\\script\\npckhac\\hoason\\kimson.lua",6,"Kim S¬n ch­ëng m«n")
	--Hoanh Son
--	AddNpcNew(309,1,56,49889,104108,"\\script\\npckhac\\hoanhson\\detu.lua",6,285)
--	AddNpcNew(309,1,56,49740,103962,"\\script\\npckhac\\hoanhson\\detu1.lua",6,285)
--	AddNpcNew(309,1,56,51104,102560,"\\script\\npckhac\\hoanhson\\doccokiem.lua",6,281)
	--Phong Lang do
--	AddNpcNew(240,1,336,1147*32,3017*32,"\\script\\feature\\phonglangdo\\thuyenphu.lua",6,826)
--	AddNpcNew(240,1,336,1282*32,2905*32,"\\script\\feature\\phonglangdo\\thuyenphu.lua",6,864)
--	AddNpcNew(240,1,336,1537*32,2809*32,"\\script\\feature\\phonglangdo\\thuyenphu.lua",6,825)
--	nNpcIdx = AddNpcNew(240,1,336,42358,92367,"\\script\\feature\\phonglangdo\\thuyenphu.lua",6,31); SetNpcValue(nNpcIdx, 1);
--	nNpcIdx = AddNpcNew(240,1,336,47894,89992,"\\script\\feature\\phonglangdo\\thuyenphu.lua",6,31); SetNpcValue(nNpcIdx, 1);
--	nNpcIdx = AddNpcNew(240,1,336,37730,95297,"\\script\\feature\\phonglangdo\\thuyenphu.lua",6,31); SetNpcValue(nNpcIdx, 1);
--	AddNpcNew(242,1,337,51537,101312,"\\script\\feature\\phonglangdo\\thuyentruong.lua",6,867)
--	AddNpcNew(240,1,337,53313,103058,"\\script\\feature\\phonglangdo\\thuyenvien.lua",6,866)
	for nMap = 996, 1002 do
    AddNpcNew(198, 1, nMap, 1630 * 32, 3279 * 32, "\\script\\global\\npcchucnang\\thoren.lua", 6, "Thî rÌn L·nh §Þa")
    AddNpcNew(384, 1, nMap, 1636*32,3274*32, "\\script\\global\\npcchucnang\\taphoa.lua", 6, "T¹p Hãa L·nh §Þa")
    AddNpcNew(308, 1, nMap, 1670*32,3239*32, "", 6, "Qu¶n Lý L·nh §Þa")
    AddNpcNew(239, 1, nMap, 1725*32,3315*32, "\\script\\global\\npcchucnang\\xaphu.lua", 6, "Xa Phu L·nh §Þa")
    -- AddNpcNew(239, 1, nMap, 1678*32,3231*32, "", 6, "R­¬ng L·nh §Þa")
	AddObj(1,nMap,1678*32,3231*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);SetNpcValue(nNpcIdx,1); --dong	
	end
	--Dai lao
	AddNpcNew(211,1,208,57277,97751,"\\script\\npckhac\\dailao\\laodau.lua",6,712)
	AddNpcNew(625,1,208,57472,98112,"\\script\\global\\npcchucnang\\ruongchua.lua",6,"R­¬ng Chøa")
	--hoi truong lien dau
	--nNpcIdx = AddNpcNew(203,1,396,1556*32,3050*32,"\\script\\global\\npcchucnang\\hieuthuoc.lua",6,"Chñ d­îc ®iÕm"); SetNpcValue(nNpcIdx, 12);
	--nNpcIdx = AddNpcNew(625,1,396,48749,98726,"\\script\\global\\npcchucnang\\ruongchua.lua",6); SetNpcValue(nNpcIdx, 1);
	--AddNpcNew(308,1,396,1514*32,3015*32,"\\script\\feature\\liendau\\hoitruong.lua",6,"Quan viªn héi tr­êng");
	--AddNpcNew(308,1,396,1534*32,3050*32,"\\script\\feature\\liendau\\hoitruong.lua",6,"Quan viªn héi tr­êng");
	--AddNpcNew(308,1,396,1484*32,2999*32,"\\script\\feature\\liendau\\hoitruong.lua",6,"Quan viªn héi tr­êng");
	--nNpcIdx = AddNpcNew(308,1,396,1584*32,2965*32,"\\script\\feature\\liendau\\xaphu.lua",6,"ThÞ vÖ héi tr­êng"); SetNpcValue(nNpcIdx, 1);
	--AddNpcNew(237,1,396,1461*32,3022*32,"\\script\\feature\\liendau\\xaphu.lua",6,42);
	--duoc vuong coc
	--AddNpcNew(188,1,140,76927,116846,"\\script\\npckhac\\duocvuong\\daonhanque.lua",6,339);
	-- local NPC_LUYEN120={
	-- {2147,2463,3669},
	-- {2143,2348,3643},
	-- {2136,2330,3586},
	-- {2148,2368,3519},
	-- {2137,2443,3585},
	-- {2139,2474,3604},
	-- {2146,2458,3459},
	-- {2138,2464,3347},
	-- {2140,2408,3409},
	-- {2141,2351,3350},
	-- {2149,2362,3602},
	-- {2145,2375,3252},
	-- {2142,2462,3181},
	-- {2150,2444,3229},
	-- {2144,2517,3233},
	-- {2151,2487,3290}
	-- };
	--for i=1,getn(NPC_LUYEN120) do
	--	AddNpcNew(NPC_LUYEN120[i][1],100,140,NPC_LUYEN120[i][2]*32,NPC_LUYEN120[i][3]*32,"\\script\\global\\lastdamage\\luyenskill120.lua",
	--	5,nil,1,RANDOMC(0,1,2,3,4),0,500000,100,1000,nil,1,1,nil,1000,30,2,nil)
	--end
	--minh nguyet tran
	-- MINHNGUYET_TRAN = {
	-- {212,1,1566,3150,0,"Thî b¸nh",0,"\\script\\global\\npcchucnang\\thobanh.lua"},
	-- {212,1,1566,3257,0,"Thî b¸nh",0,"\\script\\global\\npcchucnang\\thobanh.lua"},
	-- {212,1,1641,3153,0,"Thî b¸nh",0,"\\script\\global\\npcchucnang\\thobanh.lua"},
	-- {212,1,1632,3262,0,"Thî b¸nh",0,"\\script\\global\\npcchucnang\\thobanh.lua"},
	-- {209,1,1556,3150,0,"T©y H¹ Th­¬ng Nh©n",0,"\\script\\global\\npcchucnang\\tayhathnhan.lua"},
	-- {209,1,1554,3265,0,"T©y H¹ Th­¬ng Nh©n",0,"\\script\\global\\npcchucnang\\tayhathnhan.lua"},
	-- {209,1,1633,3148,0,"T©y H¹ Th­¬ng Nh©n",0,"\\script\\global\\npcchucnang\\tayhathnhan.lua"},
	-- {209,1,1645,3273,0,"T©y H¹ Th­¬ng Nh©n",0,"\\script\\global\\npcchucnang\\tayhathnhan.lua"},
	-- {455,1,1600,3203,0,"Minh NguyÖt l·o nh©n",0,"\\script\\global\\npcchucnang\\minhnguyetlaonhan.lua"},
	-- {625,1,1576,3146,0,"R­¬ng chøa ®å",0,"\\script\\global\\npcchucnang\\ruongchua.lua"},
	-- {625,1,1573,3264,0,"R­¬ng chøa ®å",0,"\\script\\global\\npcchucnang\\ruongchua.lua"},
	-- {625,1,1623,3262,0,"R­¬ng chøa ®å",0,"\\script\\global\\npcchucnang\\ruongchua.lua"},
	-- {625,1,1647,3165,0,"R­¬ng chøa ®å",0,"\\script\\global\\npcchucnang\\ruongchua.lua"},
	-- }	
	-- for i=1,getn(MINHNGUYET_TRAN) do
		-- AddNpcNew(MINHNGUYET_TRAN[i][1],MINHNGUYET_TRAN[i][2],520,MINHNGUYET_TRAN[i][3]*32,MINHNGUYET_TRAN[i][4]*32,MINHNGUYET_TRAN[i][8],6,MINHNGUYET_TRAN[i][6])
	-- end
end;

function addtrapother()
	-- AddTrapEx2(2,2592,3599,25,"\\script\\maps\\hoason\\trap\\hoason6h.lua")
	-- AddTrapEx1(2,2554,3494,15,"\\script\\maps\\hoason\\trap\\hoason10h.lua")

-- for nMap = 996, 1002 do
-- for k=-5,5 do
-- AddTrapEx2(nMap,(1589+k),(3225+k),25,"\\script\\startgame\\khuvucbang\\trapchan1.lua")
-- AddTrapEx1(i,(1589+k+1),(3225+k),15,"\\script\\startgame\\khuvucbang\\trapchan1.lua")
-- end
-- for k=-5,5 do
-- AddTrapEx2(nMap,(1631+k),(3183+k),25,"\\script\\startgame\\khuvucbang\\trapchan2.lua")
-- AddTrapEx1(nMap,(1631+k+1),(3183+k),15,"\\script\\startgame\\khuvucbang\\trapchan2.lua")
-- end

-- AddTrapEx2(nMap,1583,3231,25,"\\script\\startgame\\khuvucbang\\trapra1.lua")
-- AddTrapEx1(nMap,1581,3240,15,"\\script\\startgame\\khuvucbang\\trapra1.lua")

-- for k=-5,5 do
-- AddTrap(i,(1589+k)*32,(3182-k)*32,"\\script\\startgame\\khuvucbang\\trapravo.lua")
-- AddTrap(i,(1589+k+1)*32,(3182-k)*32,"\\script\\startgame\\khuvucbang\\trapravo.lua")
-- end


-- for k=-5,5 do
-- AddTrapEx2(nMap,(1642+k),(3171+k),25,"\\script\\startgame\\khuvucbang\\trapra2.lua")
-- AddTrapEx1(nMap,(1642+k+1),(3171+k),15,"\\script\\startgame\\khuvucbang\\trapra2.lua")
-- end
-- end
--=====================Cac trap di ban do luyen cong===============================
	-- for i=1,getn(BALANGHUYEN_VULANGSON) do
	-- AddTrap(BALANGHUYEN_VULANGSON[i][1],BALANGHUYEN_VULANGSON[i][2]*32,BALANGHUYEN_VULANGSON[i][3]*32,"\\script\\maps\\khac\\trap\\balanghuyen-vulangson.lua")
	-- end

	-- for i=1,getn(VULANGSON_BALANGHUYEN) do
	-- AddTrap(VULANGSON_BALANGHUYEN[i][1],VULANGSON_BALANGHUYEN[i][2]*32,VULANGSON_BALANGHUYEN[i][3]*32,"\\script\\maps\\khac\\trap\\vulangson-balanghuyen.lua")
	-- end
	
	-- for i=1,getn(PHUCNGUUDONG_TUONGDUONG) do
	-- AddTrap(PHUCNGUUDONG_TUONGDUONG[i][1],PHUCNGUUDONG_TUONGDUONG[i][2]*32,PHUCNGUUDONG_TUONGDUONG[i][3]*32,"\\script\\maps\\khac\\trap\\phucnguudong-tuongduong.lua")
	-- end

	-- for i=1,getn(TUONGDUONG_PHUCNGUUDONG) do
	-- AddTrap(TUONGDUONG_PHUCNGUUDONG[i][1],TUONGDUONG_PHUCNGUUDONG[i][2]*32,TUONGDUONG_PHUCNGUUDONG[i][3]*32,"\\script\\maps\\khac\\trap\\tuongduong-phucnguudong.lua")
	-- end
	
	-- for i=1,getn(DUONGCHAU_THUCCUONG) do
	-- AddTrap(DUONGCHAU_THUCCUONG[i][1],DUONGCHAU_THUCCUONG[i][2]*32,DUONGCHAU_THUCCUONG[i][3]*32,"\\script\\maps\\khac\\trap\\duongchau-thuccuong.lua")
	-- end	

	-- for i=1,getn(THUCCUONG_DUONGCHAU) do
	-- AddTrap(THUCCUONG_DUONGCHAU[i][1],THUCCUONG_DUONGCHAU[i][2]*32,THUCCUONG_DUONGCHAU[i][3]*32,"\\script\\maps\\khac\\trap\\thuccuong-duongchau.lua")
	-- end	
	
	-- for i=1,getn(BIENKINH_PHUCNGUUTAY) do
	-- AddTrap(BIENKINH_PHUCNGUUTAY[i][1],BIENKINH_PHUCNGUUTAY[i][2]*32,BIENKINH_PHUCNGUUTAY[i][3]*32,"\\script\\maps\\khac\\trap\\bienkinh-phucnguutay.lua")
	-- end

	-- for i=1,getn(PHUCNGUUTAY_BIENKINH) do
	-- AddTrap(PHUCNGUUTAY_BIENKINH[i][1],PHUCNGUUTAY_BIENKINH[i][2]*32,PHUCNGUUTAY_BIENKINH[i][3]*32,"\\script\\maps\\khac\\trap\\phucnguutay-bienkinh.lua")
	-- end
	
	-- for i=1,getn(HOANGHA_LONGMON) do
	-- AddTrap(HOANGHA_LONGMON[i][1],HOANGHA_LONGMON[i][2]*32,HOANGHA_LONGMON[i][3]*32,"\\script\\maps\\khac\\trap\\hoangha-longmon.lua")
	-- end	

	-- for i=1,getn(LONGMON_HOANGHA) do
	-- AddTrap(LONGMON_HOANGHA[i][1],LONGMON_HOANGHA[i][2]*32,LONGMON_HOANGHA[i][3]*32,"\\script\\maps\\khac\\trap\\longmon-hoangha.lua")
	-- end
	
	-- for i=1,getn(HOANGHA_LUUTEN) do
	-- AddTrap(HOANGHA_LUUTEN[i][1],HOANGHA_LUUTEN[i][2]*32,HOANGHA_LUUTEN[i][3]*32,"\\script\\maps\\khac\\trap\\hoangha-luutien.lua")
	-- end	

	-- for i=1,getn(LUUTEN_HOANGHA) do
	-- AddTrap(LUUTEN_HOANGHA[i][1],LUUTEN_HOANGHA[i][2]*32,LUUTEN_HOANGHA[i][3]*32,"\\script\\maps\\khac\\trap\\luutien-hoangha.lua")
	-- end
	
	-- for i=1,getn(DAILY_ACBADIADAO) do
	-- AddTrap(DAILY_ACBADIADAO[i][1],DAILY_ACBADIADAO[i][2]*32,DAILY_ACBADIADAO[i][3]*32,"\\script\\maps\\khac\\trap\\daily-acbadiadao.lua")
	-- end	

	-- for i=1,getn(ACBADIADAO_DAILY) do
	-- AddTrap(ACBADIADAO_DAILY[i][1],ACBADIADAO_DAILY[i][2]*32,ACBADIADAO_DAILY[i][3]*32,"\\script\\maps\\khac\\trap\\acbadiadao-daily.lua")
	-- end		
	
	-- for i=1,getn(TRUONGGIANG_THANHDO) do
	-- AddTrap(TRUONGGIANG_THANHDO[i][1],TRUONGGIANG_THANHDO[i][2]*32,TRUONGGIANG_THANHDO[i][3]*32,"\\script\\maps\\khac\\trap\\truonggiang-thanhdo.lua")
	-- end	

	-- for i=1,getn(THANHDO_TRUONGGIANG) do
	-- AddTrap(THANHDO_TRUONGGIANG[i][1],THANHDO_TRUONGGIANG[i][2]*32,THANHDO_TRUONGGIANG[i][3]*32,"\\script\\maps\\khac\\trap\\thanhdo-truonggiang.lua")
	-- end	

	-- AddTrapEx5(3,THANHDO_THANHTHANHSON[1],THANHDO_THANHTHANHSON[2],THANHDO_THANHTHANHSON[3],THANHDO_THANHTHANHSON[4],THANHDO_THANHTHANHSON[5],"\\script\\maps\\khac\\trap\\thanhdo-thanhthanhson.lua")
	-- AddTrapEx5(3,THANHTHANHSON_THANHDO[1],THANHTHANHSON_THANHDO[2],THANHTHANHSON_THANHDO[3],THANHTHANHSON_THANHDO[4],THANHTHANHSON_THANHDO[5],"\\script\\maps\\khac\\trap\\thanhthanhson-thanhdo.lua")
	
	-- for i=1,getn(HOANHSON_NAMNHAC) do
	-- AddTrap(HOANHSON_NAMNHAC[i][1],HOANHSON_NAMNHAC[i][2]*32,HOANHSON_NAMNHAC[i][3]*32,"\\script\\maps\\khac\\trap\\hoanhson-namnhac.lua")
	-- end	

	-- for i=1,getn(NAMNHAC_HOANHSON) do
	-- AddTrap(NAMNHAC_HOANHSON[i][1],NAMNHAC_HOANHSON[i][2]*32,NAMNHAC_HOANHSON[i][3]*32,"\\script\\maps\\khac\\trap\\namnhac-hoanhson.lua")
	-- end	
	
	-- for i=1,getn(BIENKINH_LAMDUQUAN) do
	-- AddTrap(BIENKINH_LAMDUQUAN[i][1],BIENKINH_LAMDUQUAN[i][2]*32,BIENKINH_LAMDUQUAN[i][3]*32,"\\script\\maps\\khac\\trap\\bienkinh-lamduquan.lua")
	-- end
	
	-- for i=1,getn(LAMDUQUAN_BIENKINH) do
	-- AddTrap(LAMDUQUAN_BIENKINH[i][1],LAMDUQUAN_BIENKINH[i][2]*32,LAMDUQUAN_BIENKINH[i][3]*32,"\\script\\maps\\khac\\trap\\lamduquan-bienkinh.lua")
	-- end
	
	-- for i=1,getn(LAOHODONG_HOANGHANGUYENDAU) do
	-- AddTrap(LAOHODONG_HOANGHANGUYENDAU[i][1],LAOHODONG_HOANGHANGUYENDAU[i][2]*32,LAOHODONG_HOANGHANGUYENDAU[i][3]*32,"\\script\\maps\\khac\\trap\\laohodong-hoanghanguyendau.lua")
	-- end	
	
	-- for i=1,getn(HOANGHANGUYENDAU_LAOHODONG) do
	-- AddTrap(HOANGHANGUYENDAU_LAOHODONG[i][1],HOANGHANGUYENDAU_LAOHODONG[i][2]*32,HOANGHANGUYENDAU_LAOHODONG[i][3]*32,"\\script\\maps\\khac\\trap\\hoanghanguyendau-laohodong.lua")
	-- end	
	
	-- for i=1,getn(LAMDUQUAN_TRUONGBACH) do
	-- AddTrap(LAMDUQUAN_TRUONGBACH[i][1],LAMDUQUAN_TRUONGBACH[i][2]*32,LAMDUQUAN_TRUONGBACH[i][3]*32,"\\script\\maps\\khac\\trap\\lamduquan-truongbach.lua")
	-- end

	-- for i=1,getn(TRUONGBACH_LAMDUQUAN) do
	-- AddTrap(TRUONGBACH_LAMDUQUAN[i][1],TRUONGBACH_LAMDUQUAN[i][2]*32,TRUONGBACH_LAMDUQUAN[i][3]*32,"\\script\\maps\\khac\\trap\\truongbach-lamduquan.lua")
	-- end

	--loi di tat' den samacdiabieu
	-- for i=1,getn(LONGMON_CONLON) do
	-- AddTrap(LONGMON_CONLON[i][1],LONGMON_CONLON[i][2]*32,LONGMON_CONLON[i][3]*32,"\\script\\maps\\khac\\trap\\longmon-conlon.lua")
	-- end	
	
	--loi di tat' den longmontran
	-- for i=1,getn(DIABIEU_KHOAIHOATLAM) do
	-- AddTrap(DIABIEU_KHOAIHOATLAM[i][1],DIABIEU_KHOAIHOATLAM[i][2]*32,DIABIEU_KHOAIHOATLAM[i][3]*32,"\\script\\maps\\khac\\trap\\diabieu-khoaihoatlam.lua")
	-- end
	
	-- for i=1,getn(PHONGLANGDO_LONGMONTRAN) do
	-- AddTrap(PHONGLANGDO_LONGMONTRAN[i][1],PHONGLANGDO_LONGMONTRAN[i][2]*32,PHONGLANGDO_LONGMONTRAN[i][3]*32,"\\script\\maps\\khac\\trap\\phonglangdo-longmontran.lua")
	-- end	

	-- for i=1,getn(LONGMONTRAN_PHONGLANGDO) do
	-- AddTrap(LONGMONTRAN_PHONGLANGDO[i][1],LONGMONTRAN_PHONGLANGDO[i][2]*32,LONGMONTRAN_PHONGLANGDO[i][3]*32,"\\script\\maps\\khac\\trap\\longmontran-phonglangdo.lua")
	-- end	

	-- for i=1,getn(LONGMONTRAN_MACCAOQUAT) do
	-- AddTrap(LONGMONTRAN_MACCAOQUAT[i][1],LONGMONTRAN_MACCAOQUAT[i][2]*32,LONGMONTRAN_MACCAOQUAT[i][3]*32,"\\script\\maps\\khac\\trap\\longmontran-maccaoquat.lua")
	-- end	

	-- for i=1,getn(MACCAOQUAT_LONGMONTRAN) do
	-- AddTrap(MACCAOQUAT_LONGMONTRAN[i][1],MACCAOQUAT_LONGMONTRAN[i][2]*32,MACCAOQUAT_LONGMONTRAN[i][3]*32,"\\script\\maps\\khac\\trap\\maccaoquat-longmontran.lua")
	-- end
	
	-- for i=1,getn(TRUONGBACH_TRUONGBACHNAM) do
	-- AddTrap(TRUONGBACH_TRUONGBACHNAM[i][1],TRUONGBACH_TRUONGBACHNAM[i][2]*32,TRUONGBACH_TRUONGBACHNAM[i][3]*32,"\\script\\maps\\khac\\trap\\truongbach-truongbachnam.lua")
	-- end	
	
	-- for i=1,getn(TRUONGBACHNAM_TRUONGBACH) do
	-- AddTrap(TRUONGBACHNAM_TRUONGBACH[i][1],TRUONGBACHNAM_TRUONGBACH[i][2]*32,TRUONGBACHNAM_TRUONGBACH[i][3]*32,"\\script\\maps\\khac\\trap\\truongbachnam-truongbach.lua")
	-- end	

	-- for i=1,getn(TRUONGBACH_TRUONGBACHNAM1) do
	-- AddTrap(TRUONGBACH_TRUONGBACHNAM1[i][1],TRUONGBACH_TRUONGBACHNAM1[i][2]*32,TRUONGBACH_TRUONGBACHNAM1[i][3]*32,"\\script\\maps\\khac\\trap\\truongbach-truongbachnam1.lua")
	-- end	
	
	-- for i=1,getn(TRUONGBACHNAM_CNTRUONGBACH) do
	-- AddTrap(TRUONGBACHNAM_CNTRUONGBACH[i][1],TRUONGBACHNAM_CNTRUONGBACH[i][2]*32,TRUONGBACHNAM_CNTRUONGBACH[i][3]*32,"\\script\\maps\\khac\\trap\\truongbachnam-channuitruongbach.lua")
	-- end	
	
	-- for i=1,getn(CNTRUONGBACH_TBSONBAC) do
	-- AddTrap(CNTRUONGBACH_TBSONBAC[i][1],CNTRUONGBACH_TBSONBAC[i][2]*32,CNTRUONGBACH_TBSONBAC[i][3]*32,"\\script\\maps\\khac\\trap\\cntruongbach-tbsonbac.lua")
	-- end		
	
	-- for i=1,getn(TBSONBAC_CNTRUONGBACH) do
	-- AddTrap(TBSONBAC_CNTRUONGBACH[i][1],TBSONBAC_CNTRUONGBACH[i][2]*32,TBSONBAC_CNTRUONGBACH[i][3]*32,"\\script\\maps\\khac\\trap\\tbsonbac-cntruongbach.lua")
	-- end	
	
	-- for i=1,getn(CNTRUONGBACH1_TBSONBAC1) do
	-- AddTrap(CNTRUONGBACH1_TBSONBAC1[i][1],CNTRUONGBACH1_TBSONBAC1[i][2]*32,CNTRUONGBACH1_TBSONBAC1[i][3]*32,"\\script\\maps\\khac\\trap\\cntruongbach-tbsonbac1.lua")
	-- end		
	
	-- for i=1,getn(TBSONBAC1_CNTRUONGBACH1) do
	-- AddTrap(TBSONBAC1_CNTRUONGBACH1[i][1],TBSONBAC1_CNTRUONGBACH1[i][2]*32,TBSONBAC1_CNTRUONGBACH1[i][3]*32,"\\script\\maps\\khac\\trap\\tbsonbac-cntruongbach1.lua")
	-- end	
	
	-- for i=1,getn(SAMACDIABIEU_SAMACSONDONG) do
	-- AddTrap(SAMACDIABIEU_SAMACSONDONG[i][1],SAMACDIABIEU_SAMACSONDONG[i][2]*32,SAMACDIABIEU_SAMACSONDONG[i][3]*32,"\\script\\maps\\khac\\trap\\samacdiabieu-samacsondong.lua")
	-- end	

	-- for i=1,getn(SAMACSONDONG_SAMACDIABIEU) do
	-- AddTrap(SAMACSONDONG_SAMACDIABIEU[i][1],SAMACSONDONG_SAMACDIABIEU[i][2]*32,SAMACSONDONG_SAMACDIABIEU[i][3]*32,"\\script\\maps\\khac\\trap\\samacsondong-samacdiabieu.lua")
	-- end	
	
	-- for i=1,getn(CANVIENDONG_HOANGHANGUYENDAU) do
	-- AddTrap(CANVIENDONG_HOANGHANGUYENDAU[i][1],CANVIENDONG_HOANGHANGUYENDAU[i][2]*32,CANVIENDONG_HOANGHANGUYENDAU[i][3]*32,"\\script\\maps\\khac\\trap\\canviendong-hoanghanguyendau.lua")
	-- end	
	
	-- for i=1,getn(HOANGHANGUYENDAU_CANVIENDONG) do
	-- AddTrap(HOANGHANGUYENDAU_CANVIENDONG[i][1],HOANGHANGUYENDAU_CANVIENDONG[i][2]*32,HOANGHANGUYENDAU_CANVIENDONG[i][3]*32,"\\script\\maps\\khac\\trap\\hoanghanguyendau-canviendong.lua")
	-- end	
	
	-- for i=1,getn(LAOHODONG_CANVIENDONG) do
	-- AddTrap(LAOHODONG_CANVIENDONG[i][1],LAOHODONG_CANVIENDONG[i][2]*32,LAOHODONG_CANVIENDONG[i][3]*32,"\\script\\maps\\khac\\trap\\laohodong-canviendong.lua")
	-- end	

	-- for i=1,getn(CANVIENDONG_LAOHODONG) do
	-- AddTrap(CANVIENDONG_LAOHODONG[i][1],CANVIENDONG_LAOHODONG[i][2]*32,CANVIENDONG_LAOHODONG[i][3]*32,"\\script\\maps\\khac\\trap\\canviendong-laohodong.lua")
	-- end	
	
	-- for i=1,getn(THUCCUONG_TIENCUCDONG1) do
	-- AddTrap(THUCCUONG_TIENCUCDONG1[i][1],THUCCUONG_TIENCUCDONG1[i][2]*32,THUCCUONG_TIENCUCDONG1[i][3]*32,"\\script\\maps\\khac\\trap\\thuccuong-tiencucdong1.lua")
	-- end	

	-- for i=1,getn(THUCCUONG_TIENCUCDONG2) do
	-- AddTrap(THUCCUONG_TIENCUCDONG2[i][1],THUCCUONG_TIENCUCDONG2[i][2]*32,THUCCUONG_TIENCUCDONG2[i][3]*32,"\\script\\maps\\khac\\trap\\thuccuong-tiencucdong2.lua")
	-- end	
	
	-- for i=1,getn(THUCCUONG_TIENCUCDONG3) do
	-- AddTrap(THUCCUONG_TIENCUCDONG3[i][1],THUCCUONG_TIENCUCDONG3[i][2]*32,THUCCUONG_TIENCUCDONG3[i][3]*32,"\\script\\maps\\khac\\trap\\thuccuong-tiencucdong3.lua")
	-- end	

	-- for i=1,getn(TIENCUCDONG_THUCCUONG1) do
	-- AddTrap(TIENCUCDONG_THUCCUONG1[i][1],TIENCUCDONG_THUCCUONG1[i][2]*32,TIENCUCDONG_THUCCUONG1[i][3]*32,"\\script\\maps\\khac\\trap\\tiencucdong-thuccuong1.lua")
	-- end	

	-- for i=1,getn(TIENCUCDONG_THUCCUONG2) do
	-- AddTrap(TIENCUCDONG_THUCCUONG2[i][1],TIENCUCDONG_THUCCUONG2[i][2]*32,TIENCUCDONG_THUCCUONG2[i][3]*32,"\\script\\maps\\khac\\trap\\tiencucdong-thuccuong2.lua")
	-- end
	
	-- for i=1,getn(TIENCUCDONG_THUCCUONG3) do
	-- AddTrap(TIENCUCDONG_THUCCUONG3[i][1],TIENCUCDONG_THUCCUONG3[i][2]*32,TIENCUCDONG_THUCCUONG3[i][3]*32,"\\script\\maps\\khac\\trap\\tiencucdong-thuccuong3.lua")
	-- end
	
	-- for i=1,getn(GIANGTANTHON_THANHDO) do
	-- AddTrap(GIANGTANTHON_THANHDO[i][1],GIANGTANTHON_THANHDO[i][2]*32,GIANGTANTHON_THANHDO[i][3]*32,"\\script\\maps\\khac\\trap\\giangtan-thanhdo.lua")
	-- end
	
	-- for i=1,getn(THANHDO_GIANGTAN) do
	-- AddTrap(THANHDO_GIANGTAN[i][1],THANHDO_GIANGTAN[i][2]*32,THANHDO_GIANGTAN[i][3]*32,"\\script\\maps\\khac\\trap\\thanhdo-giangtan.lua")
	-- end
	
	-- for i=1,getn(BALANGHUYEN_NAMNHACTRAN) do
	-- AddTrap(BALANGHUYEN_NAMNHACTRAN[i][1],BALANGHUYEN_NAMNHACTRAN[i][2]*32,BALANGHUYEN_NAMNHACTRAN[i][3]*32,"\\script\\maps\\khac\\trap\\balang-namnhac.lua")
	-- end

	-- for i=1,getn(NAMNHAC_BALANG) do
	-- AddTrap(NAMNHAC_BALANG[i][1],NAMNHAC_BALANG[i][2]*32,NAMNHAC_BALANG[i][3]*32,"\\script\\maps\\khac\\trap\\namnhac-balang.lua")
	-- end

	-- for i=1,getn(TONGKIM_TUONGDUONG) do
	-- AddTrap(TONGKIM_TUONGDUONG[i][1],TONGKIM_TUONGDUONG[i][2]*32,TONGKIM_TUONGDUONG[i][3]*32,"\\script\\maps\\khac\\trap\\tongkim-tuongduong.lua")
	-- end

	-- for i=1,getn(TUONGDUONG_TONGKIM) do
	-- AddTrap(TUONGDUONG_TONGKIM[i][1],TUONGDUONG_TONGKIM[i][2]*32,TUONGDUONG_TONGKIM[i][3]*32,"\\script\\maps\\khac\\trap\\tuongduong-tongkim.lua")
	-- end

	-- for i=1,getn(DUOCVUONG_LONGMON) do
	-- AddTrap(DUOCVUONG_LONGMON[i][1],DUOCVUONG_LONGMON[i][2]*32,DUOCVUONG_LONGMON[i][3]*32,"\\script\\maps\\khac\\trap\\duocvuong-longmon.lua")
	-- end	

	-- for i=1,getn(LONGMON_DUOCVUONG) do
	-- AddTrap(LONGMON_DUOCVUONG[i][1],LONGMON_DUOCVUONG[i][2]*32,LONGMON_DUOCVUONG[i][3]*32,"\\script\\maps\\khac\\trap\\longmon-duocvuong.lua")
	-- end	

	-- for i=1,getn(MINHNGUYET_PHUONGTUONG) do
	-- AddTrap(MINHNGUYET_PHUONGTUONG[i][1],MINHNGUYET_PHUONGTUONG[i][2]*32,MINHNGUYET_PHUONGTUONG[i][3]*32,"\\script\\maps\\khac\\trap\\minhnguyet-phuongtuong.lua")
	-- end		
end

function addobjother()
	-- AddObj(295,21,59737,117935,"",0,0)
	-- AddObj(295,21,75313,120257,"",0,0)
	-- AddObj(294,21,76157,119919,"",0,0)
	-- AddObj(294,21,72685,121429,"",0,0)
	-- AddObj(294,21,60586,121939,"",0,0)
	-- AddObj(295,21,60630,122528,"",0,0)
	-- AddObj(298,21,54779,123730,"",0,0)
	-- AddObj(299,21,66112,123524,"",0,0)
	-- AddObj(295,21,72558,123856,"",0,0)
	-- AddObj(294,21,80337,124850,"",0,0)
	-- AddObj(297,21,81109,124785,"",0,0)
	-- AddObj(299,21,91244,124253,"",0,0)
	-- AddObj(299,21,72122,125254,"",0,0)
	-- AddObj(299,21,63015,139501,"",0,0)
	-- AddObj(296,21,84242,143944,"",0,0)	
-----------------------------
	-- AddObj(294,70,45537,80630,"",0,0)
	-- AddObj(296,70,38575,85123,"",0,0)
	-- AddObj(297,70,45814,89749,"",0,0)
	-- AddObj(279,70,56479,89468,"",0,0)
	-- AddObj(293,70,57598,89247,"",0,0)
	-- AddObj(295,70,55202,90943,"",0,0)
	-- AddObj(279,70,58439,90858,"",0,0)
	-- AddObj(296,70,38368,91785,"",0,0)
	-- AddObj(294,70,50498,91677,"",0,0)
	-- AddObj(293,70,56251,91731,"",0,0)
	-- AddObj(281,70,59595,91242,"",0,0)
	-- AddObj(296,70,46875,93051,"",0,0)
	-- AddObj(293,70,54707,92407,"",0,0)
	-- AddObj(281,70,56938,92909,"",0,0)
	-- AddObj(279,70,61357,93170,"",0,0)
	-- AddObj(293,70,51479,93675,"",0,0)
	-- AddObj(279,70,55672,93743,"",0,0)
	-- AddObj(294,70,61564,93691,"",0,0)
	-- AddObj(297,70,62213,93940,"",0,0)
	-- AddObj(279,70,53136,94259,"",0,0)
	-- AddObj(297,70,57925,94555,"",0,0)
	-- AddObj(281,70,54039,95844,"",0,0)
	-- AddObj(279,70,56774,95637,"",0,0)
	-- AddObj(293,70,58439,95800,"",0,0)
	-- AddObj(293,70,60038,96810,"",0,0)
	-- AddObj(294,70,50002,101428,"",0,0)
	-- AddObj(296,70,50838,102380,"",0,0)
-----------------------------	
	-- AddObj(279,90,60151,109515,"",0,0)
	-- AddObj(279,90,61513,109240,"",0,0)
	-- AddObj(279,90,62984,108768,"",0,0)
	-- AddObj(279,90,56645,109607,"",0,0)
	-- AddObj(279,90,58549,109726,"",0,0)
	-- AddObj(291,90,60861,109569,"",0,0)
	-- AddObj(279,90,63953,110315,"",0,0)
	-- AddObj(279,90,59761,112102,"",0,0)
	-- AddObj(296,90,52857,113760,"",0,0)
	-- AddObj(279,90,61404,113773,"",0,0)
	-- AddObj(279,90,55961,115350,"",0,0)
	-- AddObj(291,90,57014,117091,"",0,0)
	-- AddObj(279,90,58044,116839,"",0,0)
-----------------------------
	-- AddObj(296,92,52293,104818,"",0,0)
-----------------------------
	-- AddObj(297,41,66142,89601,"",0,0)
-----------------------------
	-- AddObj(300,122,51381,106653,"",0,0)
-----------------------------
	-- AddObj(296,9,76721,177038,"",0,0)
	-- AddObj(297,9,68782,180397,"",0,0)
-----------------------------
	-- AddObj(6,56,47702,106939,"",1,0)
	-- AddObj(8,56,48203,110404,"",0,0)
-----------------------------
	-- AddObj(297,319,64650,104781,"",0,0)
	-- AddObj(295,319,51523,115218,"",0,0)
-----------------------------
	-- AddObj(294,320,44735,72158,"",0,0)
	-- AddObj(297,320,50166,74255,"",0,0)
	-- AddObj(296,320,50273,95017,"",0,0)
	-- AddObj(295,320,36410,100335,"",0,0)
	-- AddObj(6,320,36905,100426,"",0,0)
	-- AddObj(296,320,44778,100929,"",0,0)
-----------------------------
	-- AddObj(294,321,31163,73306,"",0,0)
	-- AddObj(296,321,50208,101038,"",0,0)
-----------------------------
	-- AddObj(294,322,50600,100382,"",0,0)
	-- AddObj(296,322,65634,131351,"",0,0)
-----------------------------
	-- AddObj(296,340,59647,110255,"",0,0)
-----------------------------
	-- AddObj(296,2,83615,126507,"",0,0)
	-- AddObj(295,2,73430,130451,"",0,0)
-----------------------------
	-- AddObj(295,140,77677,119173,"",0,0)
-----------------------------
	-- AddObj(1,325,50994,99061,"",0,0)
	-- AddObj(1,325,49967,102280,"",0,0)
	-- AddObj(1,242,51384,101666,"",0,0)
-----------------------------
	-- AddObj(303,242,57712,96746,"",0,0)
-----------------------------
	-- AddObj(479,2,82304,112288,"",0,0)
	-- AddObj(480,2,82976,111904,"",0,0)
	-- AddObj(480,2,83328,111840,"",0,0)
	-- AddObj(480,2,84224,111840,"",0,0)
-----------------------------	
end