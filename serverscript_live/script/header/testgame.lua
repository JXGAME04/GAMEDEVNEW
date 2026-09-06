--hoangnhk
PLEASE = "H∑y l˘a ch‰n:";
function xanhtest()
	Say(PLEASE,10,
	"VÚ kh› /vukhit123",
	"Y phÙc Nam/aotnam",
	"Y phÙc Nu/aotnu",
	"Hµi/giayt",
	"Y™u Æ∏i/dait",
	"Kh´i m∑o/nont",
	"HÈ uy”n/baotayt",
	"Hπng li™n/hanglient",
	"Nh…n/gioichit",
	"Ng‰c bÈi/ngocboit",
	"Tho∏t/no")
end;

function vukhit123()
	Say(PLEASE,6,
	"VÚ kh› h÷ Kim/selvk",
	"VÚ kh› h÷ MÈc/selvk",
	"VÚ kh› h÷ ThÒy/selvk",
	"VÚ kh› h÷ H·a/selvk",
	"VÚ kh› h÷ ThÊ /selvk",
	"Tho∏t/no")
end;

function aotnam()
	Say(PLEASE,6,
	"∏o h÷ Kim/selaonam",
	"∏o h÷ MÈc/selaonam",
	"∏o h÷ ThÒy/selaonam",
	"∏o h÷ H·a/selaonam",
	"∏o h÷ ThÊ /selaonam",
	"Tho∏t/no")
end;
function aotnu()
	Say(PLEASE,6,
	"∏o h÷ Kim/selaonu",
	"∏o h÷ MÈc/selaonu",
	"∏o h÷ ThÒy/selaonu",
	"∏o h÷ H·a/selaonu",
	"∏o h÷ ThÊ /selaonu",
	"Tho∏t/no")
end;

function giayt()
	Say(PLEASE,6,
	"Gi«y h÷ Kim/selgiay",
	"Gi«y h÷ MÈc/selgiay",
	"Gi«y h÷ ThÒy/selgiay",
	"Gi«y h÷ H·a/selgiay",
	"Gi«y h÷ ThÊ /selgiay",
	"Tho∏t/no")
end;

function dait()
	Say(PLEASE,6,
	"ßai h÷ Kim/seldai",
	"ßai h÷ MÈc/seldai",
	"ßai h÷ ThÒy/seldai",
	"ßai h÷ H·a/seldai",
	"ßai h÷ ThÊ /seldai",
	"Tho∏t/no")
end;

function nont()
	Say(PLEASE,6,
	"MÚ h÷ Kim/selnon",
	"MÚ h÷ MÈc/selnon",
	"MÚ h÷ ThÒy/selnon",
	"MÚ h÷ H·a/selnon",
	"MÚ h÷ ThÊ /selnon",
	"Tho∏t/no")
end;

function baotayt()
	Say(PLEASE,6,
	"Bao tay h÷ Kim/seltay",
	"Bao tay h÷ MÈc/seltay",
	"Bao tay h÷ ThÒy/seltay",
	"Bao tay h÷ H·a/seltay",
	"Bao tay h÷ ThÊ /seltay",
	"Tho∏t/no")
end;

function hanglient()
	Say(PLEASE,6,
	"Hπng li™n h÷ Kim/seldc",
	"Hπng li™n h÷ MÈc/seldc",
	"Hπng li™n h÷ ThÒy/seldc",
	"Hπng li™n h÷ H·a/seldc",
	"Hπng li™n h÷ ThÊ /seldc",
	"Tho∏t/no")
end;

function gioichit()
	Say(PLEASE,6,
	"Nh…n h÷ Kim/selnhan",
	"Nh…n h÷ MÈc/selnhan",
	"Nh…n h÷ ThÒy/selnhan",
	"Nh…n h÷ H·a/selnhan",
	"Nh…n h÷ ThÊ /selnhan",
	"Tho∏t/no")
end;

function ngocboit()
	Say(PLEASE,6,
	"Ng‰c bÈi h÷ Kim/selnb",
	"Ng‰c bÈi h÷ MÈc/selnb",
	"Ng‰c bÈi h÷ ThÒy/selnb",
	"Ng‰c bÈi h÷ H·a/selnb",
	"Ng‰c bÈi h÷ ThÊ /selnb",
	"Tho∏t/no")
end;


function selvk(sel)
	local genre,detail,parti,level,series = 0,0,0,0,0;
	local nIndex;
	local	type1 , value1_1, value1_3,
				type2 , value2_1, value2_3,
				type3 , value3_1, value3_3,
				type4 , value4_1, value4_3,
				type5 , value5_1, value5_3,
				type6 , value6_1, value6_3,
				type7 , value7_1, value7_3,
				type8 , value8_1, value8_3;
for i=0,8 do	-- [VHTD 02/09d] 0..5 vu khi cu + 6 Trien Thu, 7 Dao Thuan, 8 Thuan Dao (Vu Hon; meleeweapon.txt 61..90)
	genre,detail,parti,level,series = 0,0,i,10,sel;
	nIndex = AddItem2(0,genre,detail,parti,level,series,0,level,level,level,level,level,level);
	if(sel == 0) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		115 , 30, 0,
		121 , 50, 0,
		126 , 100, 0,
		168 , 200, 0,
		136 , 10, 0,
		101 , 25, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 1) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		115 , 30, 0,
		125 , 50, 0,
		137 , 10, 0,
		172 , 50, 0,
		126 , 100, 0,
		103 , 30, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 2) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		115 , 30, 0,
		123 , 100, 0,
		126 , 100, 0,
		169 , 200, 0,
		166 , 200, 0,
		102 , 25, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 3) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		137 , 10, 0,
		170 , 200, 0,
		115 , 30, 0,
		106 , 40, 0,
		85 , 200, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	else
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		166 , 200, 0,
		171 , 200, 0,
		136 , 10, 0,
		108 , 40, 0,
		58 , 1, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	end
	SetMagicAttrib(nIndex,
					type1 , value1_1, value1_3,
					type2 , value2_1, value2_3,
					type3 , value3_1, value3_3,
					type4 , value4_1, value4_3,
					type5 , value5_1, value5_3,
					type6 , value6_1, value6_3,
					type7 , value7_1, value7_3,
					type8 , value8_1, value8_3
					);
	 -- GetMagicAttrib(nIndex);
end

for i=0,3 do	-- [VHTD 02/09d] 0..2 vu khi tam xa cu + 3 Moc Cam (Tieu Dao; rangeweapon.txt 31..40)
	genre,detail,parti,level,series = 0,1,i,10,sel;
	nIndex = AddItem2(0,genre,detail,parti,level,series,0,level,level,level,level,level,level);
if(sel == 0) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		115 , 30, 0,
		121 , 50, 0,
		126 , 100, 0,
		168 , 200, 0,
		136 , 10, 0,
		101 , 25, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 1) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		115 , 30, 0,
		125 , 50, 0,
		137 , 10, 0,
		172 , 50, 0,
		126 , 100, 0,
		103 , 30, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 2) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		115 , 30, 0,
		123 , 100, 0,
		126 , 100, 0,
		169 , 200, 0,
		166 , 200, 0,
		102 , 25, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 3) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		137 , 10, 0,
		170 , 200, 0,
		115 , 30, 0,
		106 , 40, 0,
		85 , 200, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	else
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		166 , 200, 0,
		171 , 200, 0,
		136 , 10, 0,
		108 , 40, 0,
		58 , 1, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	end
	SetMagicAttrib(nIndex,
					type1 , value1_1, value1_3,
					type2 , value2_1, value2_3,
					type3 , value3_1, value3_3,
					type4 , value4_1, value4_3,
					type5 , value5_1, value5_3,
					type6 , value6_1, value6_3,
					type7 , value7_1, value7_3,
					type8 , value8_1, value8_3
					);
	
end
-- GetMagicAttrib(nIndex);
end;

function selaonam(sel)
	local genre,detail,parti,level,series = 0,0,0,0,0;
	local nIndex;
	local	type1 , value1_1, value1_3,
				type2 , value2_1, value2_3,
				type3 , value3_1, value3_3,
				type4 , value4_1, value4_3,
				type5 , value5_1, value5_3,
				type6 , value6_1, value6_3,
				type7 , value7_1, value7_3,
				type8 , value8_1, value8_3;
for i=0,6 do
	genre,detail,parti,level,series = 0,2,i,10,sel;
	nIndex = AddItem2(0,genre,detail,parti,level,series,0,level,level,level,level,level,level);
	if(sel == 0) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		113 , 40, 0,
		101 , 25, 0,
		85 , 200, 0,
		134 , 10, 0,
		88 ,10, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 1) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		85 , 200, 0,
		103 , 30, 0,
		113 , 40, 0,
		134 , 10, 0,
		117 , 10, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 2) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		89 , 200, 0,
		102 , 25, 0,
		88 ,10, 0,
		99 , 20, 0,
		113 , 40, 0,
		110 , 40, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 3) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		113 , 40, 0,
		106 , 40, 0,
		117 , 10, 0,
		104 , 25, 0,
		85 , 200, 0,
		98 , 20, 0,
		0 , 0, 0,
		0 , 0, 0;
	else
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		88 ,10, 0,
		105 , 25, 0,
		85 , 200, 0,
		108 , 40, 0,
		113 , 40, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	end
	SetMagicAttrib(nIndex,
					type1 , value1_1, value1_3,
					type2 , value2_1, value2_3,
					type3 , value3_1, value3_3,
					type4 , value4_1, value4_3,
					type5 , value5_1, value5_3,
					type6 , value6_1, value6_3,
					type7 , value7_1, value7_3,
					type8 , value8_1, value8_3
					);
	
end
end;
function selaonu(sel)
	local genre,detail,parti,level,series = 0,0,0,0,0;
	local nIndex;
	local	type1 , value1_1, value1_3,
				type2 , value2_1, value2_3,
				type3 , value3_1, value3_3,
				type4 , value4_1, value4_3,
				type5 , value5_1, value5_3,
				type6 , value6_1, value6_3,
				type7 , value7_1, value7_3,
				type8 , value8_1, value8_3;
for i=7,13 do
	genre,detail,parti,level,series = 0,2,i,10,sel;
	nIndex = AddItem2(0,genre,detail,parti,level,series,0,level,level,level,level,level,level);
	if(sel == 0) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		113 , 40, 0,
		101 , 25, 0,
		85 , 200, 0,
		134 , 10, 0,
		88 ,10, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 1) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		85 , 200, 0,
		103 , 30, 0,
		113 , 40, 0,
		134 , 10, 0,
		117 , 10, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 2) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		89 , 200, 0,
		102 , 25, 0,
		88 ,10, 0,
		99 , 20, 0,
		113 , 40, 0,
		110 , 40, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 3) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		113 , 40, 0,
		106 , 40, 0,
		117 , 10, 0,
		104 , 25, 0,
		85 , 200, 0,
		98 , 20, 0,
		0 , 0, 0,
		0 , 0, 0;
	else
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		88 ,10, 0,
		105 , 25, 0,
		85 , 200, 0,
		108 , 40, 0,
		113 , 40, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	end
	SetMagicAttrib(nIndex,
					type1 , value1_1, value1_3,
					type2 , value2_1, value2_3,
					type3 , value3_1, value3_3,
					type4 , value4_1, value4_3,
					type5 , value5_1, value5_3,
					type6 , value6_1, value6_3,
					type7 , value7_1, value7_3,
					type8 , value8_1, value8_3
					);
	
end
end;

function selgiay(sel)
	local genre,detail,parti,level,series = 0,0,0,0,0;
	local nIndex;
	local	type1 , value1_1, value1_3,
				type2 , value2_1, value2_3,
				type3 , value3_1, value3_3,
				type4 , value4_1, value4_3,
				type5 , value5_1, value5_3,
				type6 , value6_1, value6_3,
				type7 , value7_1, value7_3,
				type8 , value8_1, value8_3;
for i=0,3 do
	genre,detail,parti,level,series = 0,5,i,10,sel;
	nIndex = AddItem2(0,genre,detail,parti,level,series,0,level,level,level,level,level,level);
	if(sel == 0) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		111 , 40, 0,
		101 , 25, 0,
		85 , 200, 0,
		134 , 10, 0,
		88 ,10, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 1) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		85 , 200, 0,
		103 , 30, 0,
		111 , 40, 0,
		134 , 10, 0,
		89 , 200, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 2) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		89 , 200, 0,
		102 , 25, 0,
		88 ,10, 0,
		99 , 20, 0,
		111 , 40, 0,
		110 , 40, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 3) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		111 , 40, 0,
		106 , 40, 0,
		89 , 200, 0,
		104 , 25, 0,
		85 , 200, 0,
		98 , 20, 0,
		0 , 0, 0,
		0 , 0, 0;
	else
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		88 ,10, 0,
		105 , 25, 0,
		85 , 200, 0,
		108 , 40, 0,
		111 , 40, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	end
	SetMagicAttrib(nIndex,
					type1 , value1_1, value1_3,
					type2 , value2_1, value2_3,
					type3 , value3_1, value3_3,
					type4 , value4_1, value4_3,
					type5 , value5_1, value5_3,
					type6 , value6_1, value6_3,
					type7 , value7_1, value7_3,
					type8 , value8_1, value8_3
					);
	
end
end;

function seldai(sel)
	local genre,detail,parti,level,series = 0,0,0,0,0;
	local nIndex;
	local	type1 , value1_1, value1_3,
				type2 , value2_1, value2_3,
				type3 , value3_1, value3_3,
				type4 , value4_1, value4_3,
				type5 , value5_1, value5_3,
				type6 , value6_1, value6_3,
				type7 , value7_1, value7_3,
				type8 , value8_1, value8_3;
for i=0,1 do
	genre,detail,parti,level,series = 0,6,i,10,sel;
	nIndex = AddItem2(0,genre,detail,parti,level,series,0,level,level,level,level,level,level);
	if(sel == 0) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		89 , 200, 0,
		101 , 25, 0,
		85 , 200, 0,
		134 , 10, 0,
		88 ,10, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 1) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		85 , 200, 0,
		103 , 30, 0,
		88 ,10, 0,
		134 , 10, 0,
		89 , 200, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 2) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		89 , 200, 0,
		102 , 25, 0,
		88 ,10, 0,
		110 , 40, 0,
		85 , 200, 0,
		99 , 20, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 3) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		88 ,10, 0,
		106 , 40, 0,
		89 , 200, 0,
		104 , 25, 0,
		85 , 200, 0,
		98 , 20, 0,
		0 , 0, 0,
		0 , 0, 0;
	else
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		88 ,10, 0,
		105 , 25, 0,
		85 , 200, 0,
		108 , 40, 0,
		89 , 200, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	end
	SetMagicAttrib(nIndex,
					type1 , value1_1, value1_3,
					type2 , value2_1, value2_3,
					type3 , value3_1, value3_3,
					type4 , value4_1, value4_3,
					type5 , value5_1, value5_3,
					type6 , value6_1, value6_3,
					type7 , value7_1, value7_3,
					type8 , value8_1, value8_3
					);
	
end
end;

function selnon(sel)
	local genre,detail,parti,level,series = 0,0,0,0,0;
	local nIndex;
	local	type1 , value1_1, value1_3,
				type2 , value2_1, value2_3,
				type3 , value3_1, value3_3,
				type4 , value4_1, value4_3,
				type5 , value5_1, value5_3,
				type6 , value6_1, value6_3,
				type7 , value7_1, value7_3,
				type8 , value8_1, value8_3;
for i=0,13 do
	genre,detail,parti,level,series = 0,7,i,10,sel;
	nIndex = AddItem2(0,genre,detail,parti,level,series,0,level,level,level,level,level,level);
	if(sel == 0) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		89 , 200, 0,
		101 , 25, 0,
		85 , 200, 0,
		134 , 10, 0,
		88 ,10, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 1) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		85 , 200, 0,
		103 , 30, 0,
		88 ,10, 0,
		134 , 10, 0,
		89 , 200, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 2) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		89 , 200, 0,
		102 , 25, 0,
		88 ,10, 0,
		99 , 20, 0,
		85 , 200, 0,
		110 , 40, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 3) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		88 ,10, 0,
		106 , 40, 0,
		89 , 200, 0,
		104 , 25, 0,
		85 , 200, 0,
		98 , 20, 0,
		0 , 0, 0,
		0 , 0, 0;
	else
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		88 ,10, 0,
		105 , 25, 0,
		85 , 200, 0,
		108 , 40, 0,
		89 , 200, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	end
	SetMagicAttrib(nIndex,
					type1 , value1_1, value1_3,
					type2 , value2_1, value2_3,
					type3 , value3_1, value3_3,
					type4 , value4_1, value4_3,
					type5 , value5_1, value5_3,
					type6 , value6_1, value6_3,
					type7 , value7_1, value7_3,
					type8 , value8_1, value8_3
					);
	
end
end;

function seltay(sel)
	local genre,detail,parti,level,series = 0,0,0,0,0;
	local nIndex;
	local	type1 , value1_1, value1_3,
				type2 , value2_1, value2_3,
				type3 , value3_1, value3_3,
				type4 , value4_1, value4_3,
				type5 , value5_1, value5_3,
				type6 , value6_1, value6_3,
				type7 , value7_1, value7_3,
				type8 , value8_1, value8_3;
for i=0,1 do
	genre,detail,parti,level,series = 0,8,i,10,sel;
	nIndex = AddItem2(0,genre,detail,parti,level,series,0,level,level,level,level,level,level);
	if(sel == 0) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		89 , 200, 0,
		101 , 25, 0,
		85 , 200, 0,
		134 , 10, 0,
		88 ,10, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 1) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		85 , 200, 0,
		103 , 30, 0,
		88 ,10, 0,
		134 , 10, 0,
		89 , 200, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 2) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		89 , 200, 0,
		102 , 25, 0,
		88 ,10, 0,
		110 , 40, 0,
		85 , 200, 0,
		99 , 20, 0,
		0 , 0, 0,
		0 , 0, 0;
	elseif(sel == 3) then
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		88 ,10, 0,
		106 , 40, 0,
		89 , 200, 0,
		104 , 25, 0,
		85 , 200, 0,
		98 , 20, 0,
		0 , 0, 0,
		0 , 0, 0;
	else
		type1 , value1_1, value1_3,
		type2 , value2_1, value2_3,
		type3 , value3_1, value3_3,
		type4 , value4_1, value4_3,
		type5 , value5_1, value5_3,
		type6 , value6_1, value6_3,
		type7 , value7_1, value7_3,
		type8 , value8_1, value8_3
	=
		88 ,10, 0,
		105 , 25, 0,
		85 , 200, 0,
		108 , 40, 0,
		89 , 200, 0,
		0 , 0, 0,
		0 , 0, 0,
		0 , 0, 0;
	end
	SetMagicAttrib(nIndex,
					type1 , value1_1, value1_3,
					type2 , value2_1, value2_3,
					type3 , value3_1, value3_3,
					type4 , value4_1, value4_3,
					type5 , value5_1, value5_3,
					type6 , value6_1, value6_3,
					type7 , value7_1, value7_3,
					type8 , value8_1, value8_3
					);
	
end
end;

function seldc(sel)
	local genre,detail,parti,level,series = 0,0,0,0,0;
	local nIndex;
	local	type1 , value1_1, value1_3,
				type2 , value2_1, value2_3,
				type3 , value3_1, value3_3,
				type4 , value4_1, value4_3,
				type5 , value5_1, value5_3,
				type6 , value6_1, value6_3,
				type7 , value7_1, value7_3,
				type8 , value8_1, value8_3;
for i=0,1 do
	genre,detail,parti,level,series = 0,4,i,10,sel;
	nIndex = AddItem2(0,genre,detail,parti,level,series,0,level,level,level,level,level,level);
	if(sel == 0) then
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		97, 20, 0,
		114, 20, 0,
		101, 25, 0,
		92, 6, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	elseif(sel == 1) then
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		103, 30, 0,
		114, 20, 0,
		134, 10, 0,
		92, 3, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	elseif(sel == 2) then
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		102, 25, 0,
		114, 20, 0,
		110, 40, 0,
		92, 3, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	elseif(sel == 3) then
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		98, 20, 0,
		114, 20, 0,
		106, 40, 0,
		92, 6, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	else
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		105, 25, 0,
		114, 20, 0,
		108, 40, 0,
		92, 6, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	end
	SetMagicAttrib(nIndex,
					type1 , value1_1, value1_3,
					type2 , value2_1, value2_3,
					type3 , value3_1, value3_3,
					type4 , value4_1, value4_3,
					type5 , value5_1, value5_3,
					type6 , value6_1, value6_3,
					type7 , value7_1, value7_3,
					type8 , value8_1, value8_3
					);
	
end
end;

function selnhan(sel)
	local genre,detail,parti,level,series = 0,0,0,0,0;
	local nIndex;
	local	type1 , value1_1, value1_3,
				type2 , value2_1, value2_3,
				type3 , value3_1, value3_3,
				type4 , value4_1, value4_3,
				type5 , value5_1, value5_3,
				type6 , value6_1, value6_3,
				type7 , value7_1, value7_3,
				type8 , value8_1, value8_3;
for i=0,0 do
	genre,detail,parti,level,series = 0,3,i,10,sel;
	nIndex = AddItem2(0,genre,detail,parti,level,series,0,level,level,level,level,level,level);
	if(sel == 0) then
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		97, 20, 0,
		89, 200, 0,
		101, 25, 0,
		96, 6, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	elseif(sel == 1) then
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		103, 30, 0,
		89, 200, 0,
		134, 10, 0,
		96, 6, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	elseif(sel == 2) then
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		102, 25, 0,
		89, 200, 0,
		110, 40, 0,
		96, 6, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	elseif(sel == 3) then
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		98, 25, 0,
		89, 200, 0,
		106, 40, 0,
		96, 6, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	else
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		105, 25, 0,
		89, 200, 0,
		108, 40, 0,
		96, 6, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	end
	SetMagicAttrib(nIndex,
					type1 , value1_1, value1_3,
					type2 , value2_1, value2_3,
					type3 , value3_1, value3_3,
					type4 , value4_1, value4_3,
					type5 , value5_1, value5_3,
					type6 , value6_1, value6_3,
					type7 , value7_1, value7_3,
					type8 , value8_1, value8_3
					);
	
end
end;

function selnb(sel)
	local genre,detail,parti,level,series = 0,0,0,0,0;
	local nIndex;
	local	type1 , value1_1, value1_3,
				type2 , value2_1, value2_3,
				type3 , value3_1, value3_3,
				type4 , value4_1, value4_3,
				type5 , value5_1, value5_3,
				type6 , value6_1, value6_3,
				type7 , value7_1, value7_3,
				type8 , value8_1, value8_3;
for i=0,1 do
	genre,detail,parti,level,series = 0,9,i,10,sel;
	nIndex = AddItem2(0,genre,detail,parti,level,series,0,level,level,level,level,level,level);
	if(sel == 0) then
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		97, 20, 0,
		89, 200, 0,
		101, 25, 0,
		88, 6, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	elseif(sel == 1) then
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		103, 30, 0,
		89, 200, 0,
		134, 10, 0,
		88, 6, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	elseif(sel == 2) then
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		102, 25, 0,
		89, 200, 0,
		110, 40, 0,
		88, 6, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	elseif(sel == 3) then
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		98, 25, 0,
		89, 200, 0,
		106, 40, 0,
		88, 6, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	else
		type1, value1_1, value1_3,
		type2, value2_1, value2_3,
		type3, value3_1, value3_3,
		type4, value4_1, value4_3,
		type5, value5_1, value5_3,
		type6, value6_1, value6_3,
		type7, value7_1, value7_3,
		type8, value8_1, value8_3
	=
		85, 200, 0,
		105, 25, 0,
		89, 200, 0,
		108, 40, 0,
		88, 6, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	end
	SetMagicAttrib(nIndex,
					type1 , value1_1, value1_3,
					type2 , value2_1, value2_3,
					type3 , value3_1, value3_3,
					type4 , value4_1, value4_3,
					type5 , value5_1, value5_3,
					type6 , value6_1, value6_3,
					type7 , value7_1, value7_3,
					type8 , value8_1, value8_3
					);
	
end
end;