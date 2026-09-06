Include("\\script\\startgame\\npcpos.lua");

function addtrainingnpc()
	for i=1,getn(BALANGHUYEN) do
	AddNpcNew(RANDOMC(31,42,43),10,53,BALANGHUYEN[i][1],BALANGHUYEN[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,40,5,10,15,nil,600,nil,nil,DROPFILE);
		-- --SetNpcBoss2(NpcIndex,1)
	end
	for i=1,getn(GIANGTANTHON) do
	AddNpcNew(RANDOMC(31,42,43),10,20,GIANGTANTHON[i][1],GIANGTANTHON[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,40,5,10,15,nil,600,nil,nil,DROPFILE);
	end
	--for i=1,getn(LONGMONTRAN) do
	--AddNpcNew(RANDOMC(31,42,43),10,121,LONGMONTRAN[i][1],LONGMONTRAN[i][2],DEATHFILE,
		--5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,40,5,10,15,nil,600,nil,nil,DROPFILE);
	--end
	--for i=1,getn(NAMNHACTRAN) do
	--AddNpcNew(RANDOMC(31,42,43),10,54,NAMNHACTRAN[i][1],NAMNHACTRAN[i][2],DEATHFILE,
		--5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,40,5,10,15,nil,600,nil,nil,DROPFILE);
	--end	
	for i=1,getn(BIENKINH) do
	AddNpcNew(RANDOMC(11,33,34,12),10,37,BIENKINH[i][1],BIENKINH[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,50,10,15,20,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(DAILY) do
	AddNpcNew(RANDOMC(11,33,34,12),10,162,DAILY[i][1],DAILY[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,50,10,15,20,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(DUONGCHAU) do
	AddNpcNew(RANDOMC(11,33,34,12),10,80,DUONGCHAU[i][1],DUONGCHAU[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,50,10,15,20,nil,600,nil,nil,DROPFILE);
	end
	--for i=1,getn(LAMAN) do
	--AddNpcNew(RANDOMC(11,33,34,12),10,176,LAMAN[i][1],LAMAN[i][2],DEATHFILE,
	--	5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,50,10,15,20,nil,600,nil,nil,DROPFILE);
	--end
	for i=1,getn(PHUONGTUONG) do
	AddNpcNew(RANDOMC(11,33,34,12),10,1,PHUONGTUONG[i][1],PHUONGTUONG[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,50,10,15,20,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(THANHDO) do
	AddNpcNew(RANDOMC(11,33,34,12),10,11,THANHDO[i][1],THANHDO[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,50,10,15,20,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(TUONGDUONG) do
	AddNpcNew(RANDOMC(11,33,34,12),10,78,TUONGDUONG[i][1],TUONGDUONG[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,50,10,15,20,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(HOASON) do
	AddNpcNew(RANDOMC(11,33,34,12),10,2,HOASON[i][1],HOASON[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,50,10,15,20,nil,600,nil,nil,DROPFILE);		
	end
	for i=1,getn(THANHTHANHSON) do
	AddNpcNew(RANDOMC(24,25,26),20,21,THANHTHANHSON[i][1],THANHTHANHSON[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,60,15,20,25,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(DUOCVUONGCOC) do
	AddNpcNew(RANDOMC(34,38,42),20,140,DUOCVUONGCOC[i][1],DUOCVUONGCOC[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,60,15,20,25,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(VULANGSON) do
	AddNpcNew(RANDOMC(10,25,26),20,70,VULANGSON[i][1],VULANGSON[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,60,15,20,25,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(PHUCNGUUSONDONG) do	
	AddNpcNew(RANDOMC(7,8,10),30,90,PHUCNGUUSONDONG[i][1],PHUCNGUUSONDONG[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,70,20,25,30,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(THUCCUONGSON) do	
	AddNpcNew(RANDOMC(17,36,37),30,92,THUCCUONGSON[i][1],THUCCUONGSON[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,70,20,25,30,nil,600,nil,nil,DROPFILE);
	end	
	for i=1,getn(PHUCNGUUSONTAY) do	
	AddNpcNew(RANDOMC(7,8,10),40,41,PHUCNGUUSONTAY[i][1],PHUCNGUUSONTAY[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,80,25,30,35,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(HOANGHANGUYENDAU) do	
	AddNpcNew(RANDOMC(2,4,26),40,122,HOANGHANGUYENDAU[i][1],HOANGHANGUYENDAU[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,80,25,30,35,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(LUUTIENDONG) do	
	AddNpcNew(RANDOMC(146,161,162),50,125,LUUTIENDONG[i][1],LUUTIENDONG[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,90,30,35,40,nil,600,nil,nil,DROPFILE);
	end	
	for i=1,getn(ACBADIADAO) do	
	AddNpcNew(RANDOMC(140,149,150),50,163,ACBADIADAO[i][1],ACBADIADAO[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,90,30,35,40,nil,600,nil,nil,DROPFILE);
	end		
	for i=1,getn(TRUONGGIANGNGUYENDAU) do	
	AddNpcNew(412,60,9,TRUONGGIANGNGUYENDAU[i][1],TRUONGGIANGNGUYENDAU[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,100,35,40,45,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(HOANHSONPHAI) do	
	AddNpcNew(RANDOMC(24,25,26),60,56,HOANHSONPHAI[i][1],HOANHSONPHAI[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,100,35,40,45,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(LAMDUQUAN) do		
	AddNpcNew(RANDOMC(588,589),70,319,LAMDUQUAN[i][1],LAMDUQUAN[i][2],DEATHFILE ,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,120,45,50,55,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(LAOHODONG) do	
	AddNpcNew(RANDOMC(140,149,150),70,123,LAOHODONG[i][1],LAOHODONG[i][2],DEATHFILE ,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,120,45,50,55,nil,600,nil,nil,DROPFILE);
	end
	for i=1,getn(SAMACDIABIEU) do		
	AddNpcNew(RANDOMC(556,557,558),80,224,SAMACDIABIEU[i][1],SAMACDIABIEU[i][2],DEATHFILE ,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,120,45,50,55,nil,1000,nil,nil,DROPFILE);
	end
	for i=1,getn(CHANNUITRUONGBACH) do	
	AddNpcNew(RANDOMC(590,591,592,593),80,320,CHANNUITRUONGBACH[i][1],CHANNUITRUONGBACH[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,120,45,50,55,nil,1000,nil,nil,DROPFILE);
	end
	for i=1,getn(TRUONGBACHNAM) do	
	AddNpcNew(RANDOMC(598,599,600,601),90,321,TRUONGBACHNAM[i][1],TRUONGBACHNAM[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,130,45,50,55,nil,1000,nil,nil,DROPFILE);
	end
	for i=1,getn(TRUONGBACHBAC) do		
	AddNpcNew(RANDOMC(594,595,596,597),90,322,TRUONGBACHBAC[i][1],TRUONGBACHBAC[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,130,45,50,55,nil,1000,nil,nil,DROPFILE);
	end
	for i=1,getn(KHOALANGDONG) do	
	AddNpcNew(RANDOMC(155,156),90,75,KHOALANGDONG[i][1],KHOALANGDONG[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,250,45,50,55,nil,1000,nil,nil,DROPFILE);
	end
	for i=1,getn(SAMAC1) do	
	AddNpcNew(RANDOMC(561,560,917),90,225,SAMAC1[i][1],SAMAC1[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,130,45,50,55,nil,1000,nil,nil,DROPFILE);
	end
	for i=1,getn(SAMAC2) do	
	AddNpcNew(RANDOMC(561,560,917),90,226,SAMAC2[i][1],SAMAC2[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,130,45,50,55,nil,1000,nil,nil,DROPFILE);
	end
	for i=1,getn(SAMAC3) do
	AddNpcNew(RANDOMC(534,535,536,537,538),90,227,SAMAC3[i][1],SAMAC3[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,130,45,50,55,nil,1000,nil,nil,DROPFILE);	
	end
	for i=1,getn(PHONGLANGDO) do	
	AddNpcNew(RANDOMC(707,708),90,336,PHONGLANGDO[i][1],PHONGLANGDO[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,130,45,50,55,nil,1000,nil,nil,DROPFILE);
	end
	for i=1,getn(MACCAOQUAT) do	
	AddNpcNew(RANDOMC(703,704,705,706),90,340,MACCAOQUAT[i][1],MACCAOQUAT[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,130,45,50,55,nil,1000,nil,nil,DROPFILE);
	end	
	for i=1,getn(CANVIENDONG) do
	AddNpcNew(RANDOMC(13,14),90,124,CANVIENDONG[i][1],CANVIENDONG[i][2],DEATHFILE ,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,130,45,50,55,nil,1000,nil,nil,DROPFILE);
	end
	for i=1,getn(TIENCUCDONG) do
	AddNpcNew(RANDOMC(838,147,148),90,93,TIENCUCDONG[i][1],TIENCUCDONG[i][2],DEATHFILE,
		5,nil,1,RANDOMC(0,1,2,3,4),nil,nil,nil,130,45,50,55,nil,1000,nil,nil,DROPFILE);
	end
end;
