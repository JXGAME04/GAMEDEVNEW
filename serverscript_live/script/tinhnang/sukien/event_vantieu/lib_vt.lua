--Author: Fong KiÒu
--Function: Lib vËn tiªu
--Date: 28/11/2020

Include("\\script\\lib\\lib_server.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_trap.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\lib\\lib_ham.lua")

TIME_VANTIEU = {13,23} --gio bat dau thoi gian ket thuc hoat dong van tieu trong ngay

NPC_TTHANHSON = {
	{83386,143078},
	{83688,142690},
	{83440,141766},
	{82977,141406},
	{82671,140270},
	{82315,139078},
	{81223,138366},
	{81238,137920},
	{80849,135616},
	{80311,135056},
	{79815,133918},
	{78828,132952},
	{77844,131488},
	{77464,130256},
	{76440,129184},
	{75733,129016},
	{75557,127802},
	{76438,126652},
	{77953,126072},
	{78580,124268},
	{79007,123916},
	{78447,122400},
	{77938,121480},
	{76754,121210},
	{75449,120262},
	{75170,120882},
	{73685,121074},
	{73009,122072},
	{72379,121686},
	{71373,120540},
	{70195,120272},
	{69150,119234},
	{68635,119588},
	{67876,119882},
	{67321,120980},
	{66055,121142},
	{64873,120924},
	{64666,120282},
	{63938,120156},
	{63410,120460},
	{62158,121726},
	{61128,122054},
	{60437,122308},
	{60115,121590},
	{60238,120238},
	{59973,118840},
	{60187,117764},
	{61109,116756},
	{62093,115094},
	{62480,115030}
}

BOSS_TTHANHSON = {
	{78957,124476,"D­¬ng §Ønh Thiªn"   },
	{72793,122074,"§oµn TrÝ H­ng"},
	{64069,119871,"Tèng ViÔn KiÒu"  },
}

KIND_TIEUXA = {
	{1,2001," §ång Tiªu Xa","<color=green>"}, -- dong tieu xa
	{2,2001," B¹c Tiªu Xa" ,"<color=purple>"}, -- bac tieu xa
	{3,2001," Vµng Tiªu Xa","<color=gold>"}, -- vang tieu xa
}

MID_PHUONGTUONG								 	= 1
MID_HOASON 												= 2
MID_THANHDO											 	= 21
MID_THANHTHANHSON 							= 21
MID_TUONGDUONG 									= 78
MID_PHUCNGUUSONDONG					 	= 90
ITEM_TIEUKY												= {6,1,4771,0,0,0,0, 500000}
ITEM_HOANTIEUCHI 									= {6,1,4772,0,0,0,0, 1000000}
ITEM_UYNHIEMTRANG								= {6,1,4773,0,0,0,0, 2000000}
ITEM_HOTIEULENH 									= {6,1,4774,0,0,0,0, 150000} --môc cuèi sè tiÒn v¹n mua ho tieu lenh

FILE_TIEUDAU 							= "\\script\\event\\event_vantieu\\tieudau.lua"
FILE_TIEUSU 								= "\\script\\event\\event_vantieu\\tieusu.lua"
DROPRATENPC 							= "\\script\\event\\event_vantieu\\drop_npc.lua"
ONDEATHNPC 							= "\\script\\event\\event_vantieu\\death_npc.lua"
DROPRATETIEU 						= "\\script\\event\\event_vantieu\\drop_tieu.lua"
ONDEATHTIEU 							= "\\script\\event\\event_vantieu\\death_tieu.lua"
MAX_CUOP_TIEU 						= 5
TIME_LIMIT_VT						= 30*60*18 --thêi gian tèi ®a nÕu kh«ng vËn xong sÏ kh«ng hoµn thµnh nhiÖm vô
MAXVANTIEU 							= 20--sè lÇn tèi ®a vËn tiªu trong ngµy
NEED_MONEY 							= 500000
NEED_MONEY_GO 					= 100000
LEVEL_LIMIT_VT						= 50 --dang cap toi thieu tham gia van tieu

NPC_TALKTIEU ={--index--npctieusuid--idmap--x1--y1--npcidlongmontieusu--idmap--x2--y2
	{1,115,"Tiªu S­ Tiªu Côc Song ¦ng"	,MID_THANHDO,2612,4489, 376,"Long M«n Tiªu S­ ",MID_THANHTHANHSON,1951,3506},	-- thµnh ®« thanh thanh son
	--{2,308,"Tiªu s­ tiªu côc Song ¦ng"	,MID_PHUONGTUONG,1586,3184, 376,"Long M«n tiªu s­ ",MID_HOASON ,2600,3504},	-- ph­îng t­êng ®i hoa son	
	--{3,379,"Tiªu s­ tiªu côc Song ¦ng"	,MID_TUONGDUONG,1581,3208, 376,"Long M«n tiªu s­ ",MID_PHUCNGUUSONDONG,1925,3359},	-- t­¬ng d­¬ng phuc nguu dong
}

function admin_vantieu()
	SayEx({12655,
	"Ta muèn ®Õn tiªu s­ Thanh Thµnh S¬n/#dentieusu(1)",	
	"Ta muèn ®Õn tiªu s­ Hoa S¬n/#dentieusu(2)",
	"Ta muèn ®Õn tiªu s­ Phôc Ng­u S¬n/#dentieusu(3)",
	"Ta muèn ®Õn Npc S¬n TÆc/dennpcsontac",
	"Ta muèn ®Õn Npc Boss/dennpcboss",
	"Ta chØ ghÐ ngang qua./no"
	})
end

 function dentieusu(index)
 SetFightState(1) NewWorld(NPC_TALKTIEU[index][9],NPC_TALKTIEU[index][10],NPC_TALKTIEU[index][11])
 end

 function dennpcsontac()
	 local index = random(1,getn(NPC_TTHANHSON))
	 SetFightState(1) NewWorld(MID_THANHTHANHSON,NPC_TTHANHSON[index][1]/32,NPC_TTHANHSON[index][2]/32)	
 end

 function dennpcboss()
	local index = random(1,getn(BOSS_TTHANHSON))
	 SetFightState(1) NewWorld(MID_THANHTHANHSON,BOSS_TTHANHSON[index][1]/32,BOSS_TTHANHSON[index][2]/32)	
 end

 function addnpcvantieu()
	-- son tac
	 local nNpcIdx
	 for i=1,getn(NPC_TTHANHSON) do
		nNpcIdx = AddNpcEx3({24,552,26},100,{0,1,2,3,4},MID_THANHTHANHSON,
		NPC_TTHANHSON[i][1],NPC_TTHANHSON[i][2],DROPRATE9X,DEATHFILE9X,
			 nil,5,nil,nil,nil,100,35,40,45,800,nil)
	 end
	
	for i=1,getn(BOSS_TTHANHSON) do
		nNpcIdx = AddNpcEx3({701},120,{0,1,2,3,4},MID_THANHTHANHSON,
		BOSS_TTHANHSON[i][1],BOSS_TTHANHSON[i][2],DROPRATENPC,ONDEATHNPC,
		BOSS_TTHANHSON[i][3],5,6000*EXP_RATE,100000,nil,5000,100,1,1,2500,50)
	 end	
	
	-- npc lien quan
	 for i=1,getn(NPC_TALKTIEU) do
		nNpcIdx = AddNpcEx1({NPC_TALKTIEU[i][2]},1,nil,NPC_TALKTIEU[i][4],
		 NPC_TALKTIEU[i][5]*32,NPC_TALKTIEU[i][6]*32,"",FILE_TIEUDAU,NPC_TALKTIEU[i][3],6)
		 SetNpcValue(nNpcIdx,NPC_TALKTIEU[i][1])
		
		 nNpcIdx = AddNpcEx1({NPC_TALKTIEU[i][7]},1,nil,NPC_TALKTIEU[i][9],
		 NPC_TALKTIEU[i][10]*32,NPC_TALKTIEU[i][11]*32,"",FILE_TIEUSU,NPC_TALKTIEU[i][8],6)
		 SetNpcValue(nNpcIdx,NPC_TALKTIEU[i][1])
	 end		
 end

-- function addtrapvantieu()
	-- local Count = 0
	-- Count = getn(TUONGDUONG_PHUCNGUUDONG)
	-- for i=1,Count do
		-- AddTrapEx5(TUONGDUONG_PHUCNGUUDONG[i][1],TUONGDUONG_PHUCNGUUDONG[i][2]*32,TUONGDUONG_PHUCNGUUDONG[i][3]*32,"\\script\\maps\\khac\\trap\\tuongduong-phucnguudong.lua")
	-- end
	
	-- Count = getn(PHUCNGUUDONG_TUONGDUONG)
	-- for i=1,Count do
		-- AddTrapEx5(PHUCNGUUDONG_TUONGDUONG[i][1],PHUCNGUUDONG_TUONGDUONG[i][2]*32,PHUCNGUUDONG_TUONGDUONG[i][3]*32,"\\script\\maps\\khac\\trap\\phucnguudong-tuongduong.lua")
	-- end	
	
	-- Count = getn(DUONGCHAU_HOASON)
	-- for i=1,Count do
		-- AddTrapEx5(DUONGCHAU_HOASON[i][1],DUONGCHAU_HOASON[i][2]*32,DUONGCHAU_HOASON[i][3]*32,"\\script\\maps\\khac\\trap\\duongchau-hoason.lua")
	-- end
	
	-- Count = getn(HOASON_DUONGCHAU)
	-- for i=1,Count do
		-- AddTrapEx5(HOASON_DUONGCHAU[i][1],HOASON_DUONGCHAU[i][2]*32,HOASON_DUONGCHAU[i][3]*32,"\\script\\maps\\khac\\trap\\hoason-duongchau.lua")
	-- end

	-- Count = getn(PHUONGTUONG_THANHTHANHSON)
	-- for i=1,Count do
		-- AddTrapEx5(PHUONGTUONG_THANHTHANHSON[i][1],PHUONGTUONG_THANHTHANHSON[i][2]*32,PHUONGTUONG_THANHTHANHSON[i][3]*32,"\\script\\maps\\khac\\trap\\phuongtuong-thanhthanhson.lua")
	-- end	
	
	-- Count = getn(THANHTHANHSON_PHUONGTUONG)
	-- for i=1,Count do
		-- AddTrapEx5(THANHTHANHSON_PHUONGTUONG[i][1],THANHTHANHSON_PHUONGTUONG[i][2]*32,THANHTHANHSON_PHUONGTUONG[i][3]*32,"\\script\\maps\\khac\\trap\\thanhthanhson-phuongtuong.lua")
	-- end		
	
	
	 -- AddTrapEx5(3,THANHDO_THANHTHANHSON[1],THANHDO_THANHTHANHSON[2],THANHDO_THANHTHANHSON[3],THANHDO_THANHTHANHSON[4],THANHDO_THANHTHANHSON[5],"\\script\\maps\\khac\\trap\\thanhdo-thanhthanhson.lua")
	 -- AddTrapEx5(3,THANHTHANHSON_THANHDO[1],THANHTHANHSON_THANHDO[2],THANHTHANHSON_THANHDO[3],THANHTHANHSON_THANHDO[4],THANHTHANHSON_THANHDO[5],"\\script\\maps\\khac\\trap\\thanhthanhson-thanhdo.lua")
	
-- end

function addobjvantieu()
end