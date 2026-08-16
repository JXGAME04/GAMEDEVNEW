--hoangnhk
function addnpclongmon()
	local nNpcIdx;
--luyen kc
-- local MOCNHAN = {
-- {60542,148538},
-- {60373,148636},
-- {60272,148886},
-- {60373,149002},
-- {60536,148828},
-- {60738,148928},
-- {60703,148534},
-- {60816,148744}
-- };
-- local COCGO = {
-- {60079,148208},
-- {59963,148436},
-- {61041,149130},
-- {60980,149416},
-- {60713,149558},
-- {60318,148078}
-- };
-- local BAOCAT = {
-- {60171,148996},
-- {60102,148774},
-- {60136,148520},
-- {60273,148346},
-- {60436,148284},
-- {60620,148328},
-- {60842,149054},
-- {60975,148822}
-- };
	-- for i=1,getn(MOCNHAN) do
	-- nNpcIdx = AddNpcNew(414,1,121,MOCNHAN[i][1],MOCNHAN[i][2],DEATH_COCGO,
		-- 5,nil,1,RANDOMC(0,1,2,3,4),1,1,nil,nil,1,nil,nil,nil,50,nil,nil,"");
	-- SetNpcValue(nNpcIdx,3);
	-- end
	-- for i=1,getn(COCGO) do
	-- nNpcIdx = AddNpcNew(413,1,121,COCGO[i][1],COCGO[i][2],DEATH_COCGO,
		-- 5,nil,1,RANDOMC(0,1,2,3,4),1,1,nil,nil,1,nil,nil,nil,50,nil,nil,"");
	-- SetNpcValue(nNpcIdx,1);
	-- end
	-- for i=1,getn(BAOCAT) do
	-- nNpcIdx = AddNpcNew(415,1,121,BAOCAT[i][1],BAOCAT[i][2],DEATH_COCGO,
		-- 5,nil,1,RANDOMC(0,1,2,3,4),1,1,nil,nil,1,nil,nil,nil,50,nil,nil,"");
	-- SetNpcValue(nNpcIdx,2);
	-- end
	----NPC Mon Phai-----
	nNpcIdx = AddNpcNew(189,1,121,1949*32,4527*32,"\\script\\npcthon\\npcmonphai\\thieulam.lua",6,183);SetNpcValue(nNpcIdx, 0);
	nNpcIdx = AddNpcNew(184,1,121,1958*32,4559*32,"\\script\\npcthon\\npcmonphai\\thienvuong.lua",6,247);SetNpcValue(nNpcIdx, 1);
	nNpcIdx = AddNpcNew(177,1,121,1984*32,4515*32,"\\script\\npcthon\\npcmonphai\\duongmon.lua",6,185);SetNpcValue(nNpcIdx, 2);
	nNpcIdx = AddNpcNew(186,1,121,1896*32,4488*32,"\\script\\npcthon\\npcmonphai\\ngudoc.lua",6,178);SetNpcValue(nNpcIdx, 3);
	nNpcIdx = AddNpcNew(83 ,1,121,1956*32,4490*32,"\\script\\npcthon\\npcmonphai\\ngami.lua",6,248);SetNpcValue(nNpcIdx, 4);
	nNpcIdx = AddNpcNew(171,1,121,1977*32,4486*32,"\\script\\npcthon\\npcmonphai\\thuyyen.lua",6,177);SetNpcValue(nNpcIdx, 5);	
	nNpcIdx = AddNpcNew(103,1,121,1904*32,4538*32,"\\script\\npcthon\\npcmonphai\\caibang.lua",6,194);SetNpcValue(nNpcIdx, 6);
	nNpcIdx = AddNpcNew(181,1,121,2004*32,4466*32,"\\script\\npcthon\\npcmonphai\\thiennhan.lua",6,240);SetNpcValue(nNpcIdx, 7);
	nNpcIdx = AddNpcNew(188,1,121,1923*32,4558*32,"\\script\\npcthon\\npcmonphai\\vodang.lua",6,249);SetNpcValue(nNpcIdx, 8);
	nNpcIdx = AddNpcNew(309,1,121,62054,144266,"\\script\\npcthon\\npcmonphai\\conlon.lua",6,181);SetNpcValue(nNpcIdx, 9);
	
	----NPC Chuc nang-----
	nNpcIdx = AddNpcNew(243,1,121,1914*32,4525*32,"\\script\\global\\npcchucnang\\taphoa.lua",6,84);SetNpcValue(nNpcIdx, 23);
	nNpcIdx = AddNpcNew(198,1,121,2001*32,4512*32,"\\script\\global\\npcchucnang\\thoren.lua",6,55);SetNpcValue(nNpcIdx, 22);
	nNpcIdx = AddNpcNew(203,1,121,1935*32,4553*32,"\\script\\global\\npcchucnang\\hieuthuoc.lua",6,51);SetNpcValue(nNpcIdx, 24);
	AddObj(1,121,1934*32,4513*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0); 
	AddObj(1,121,2000*32,4472*32,"\\script\\global\\npcchucnang\\ruongchua.lua",0,0);
	AddNpcNew(393,1,121,1980*32,4594*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	AddNpcNew(237,1,121,1924*32,4426*32,"\\script\\global\\npcchucnang\\xaphu.lua",6,42)
	AddNpcNew(108,1,121,1960*32,4501*32,"\\script\\global\\seasonnpc.lua",6,59)
	AddNpcNew(377,1,121,1959*32,4517*32,"\\script\\global\\npcchucnang\\lequan.lua",6,57)

	----NPC longmon----------

end;

function addtraplongmon()
	AddTrapEx2(121,2033,4480,7,"\\script\\maps\\longmon\\trap\\cong2h.lua")
	AddTrapEx1(121,1975,4625,12,"\\script\\maps\\longmon\\trap\\cong4h.lua")
	AddTrapEx2(121,1887,4555,15,"\\script\\maps\\longmon\\trap\\cong8h.lua")
	AddTrapEx1(121,1900,4422,13,"\\script\\maps\\longmon\\trap\\cong10h.lua")
end;

function addobjlongmon()
	-- SetObjValue(AddObj(297,121,58988,131202,"\\script\\maps\\longmon\\obj\\dauhieu.lua",0,0),1)
	-- SetObjValue(AddObj(297,121,70636,133458,"\\script\\maps\\longmon\\obj\\dauhieu.lua",0,0),2)
	-- SetObjValue(AddObj(294,121,55248,136454,"\\script\\maps\\longmon\\obj\\dauhieu.lua",0,0),3)
	-- SetObjValue(AddObj(295,121,54923,140107,"\\script\\maps\\longmon\\obj\\dauhieu.lua",0,0),4)
	-- SetObjValue(AddObj(296,121,73206,140611,"\\script\\maps\\longmon\\obj\\dauhieu.lua",0,0),5)
	--AddObj(1,121,64006,143071,"",0,0)
	--AddObj(1,121,61876,144405,"",1,0)
end;