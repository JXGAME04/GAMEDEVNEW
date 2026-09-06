--Author: Fong KiÒu
--Date: 09/07/2021
--Function: Lib Trang bÞ Hoµng Kim

Include("\\script\\lib\\lib_ham.lua")

function laydohoangkim()
	SayEx({"Chän trang bÞ Hoµng Kim.",
		--"An Bang /chonanbang",
		"An Bang /chonanbangld",
		--"§Þnh Quèc /chondinhquoc", 
		"§Þnh Quèc /chondinhquocld", 
		--"Nhu T×nh /nhutinh", 
		"Nhu T×nh /nhutinhcucpham", 
		--"HiÖp Cèt /hiepcot", 
		-- "HiÖp Cèt /hiepcotcucpham", 
		-- "Kim Phong /kimphong", 
		"Thiªn Hoµng /thienhoang", 
		-- "Hång ¶nh/trangbihonganh",
		-- "§éng s¸t /dongsat", 
	--	"Kim ¤ /trangbikimo", 
	--	"Trang BÞ MP/trangbiminhphung", 
		-- "Hoµng kim m«n ph¸i/hoangkimmonphai345",
		--"Trang bÞ Liªn §Êu/trangbiliendau",
		"Ta suy nghÜ mét chót n÷a /no"})
end

function trangbikimo()
	AddGoldItem(2063-2,0)--tay khong
	AddGoldItem(2073-2,0)--bong
	AddGoldItem(2083-2,0)--dao
	AddGoldItem(2093-2,0)--chuy
	AddGoldItem(2103-2,0)--thuong
	AddGoldItem(2123-2,0)--kiem
	AddGoldItem(2153-2,0)--song dao
	
	AddGoldItem(2183-2,0)--phi dao
	AddGoldItem(2193-2,0)--ná
	AddGoldItem(2203-2,0)--phi tieu
end
function trangbiminhphung()
	for i=2640, 2650 do
		AddGoldItem(i,0)
	end
end
function trangbihonganh()
	for i=429,432 do AddItem2(2,0,i,0,0,0) end      
end

function trangbiliendau()
	for i=398, 402 do
		AddGoldItem(i,0)
	end
end

function chonanbang()
	for i=164,167 do 
		AddGoldItem(i,0) 
	end
end

function chonanbangld()
	for i=423,426 do AddItem2(2,0,i,0,0,0) end 
end

function chondinhquoc()
	for i=159,163 do 
		AddGoldItem(i,0) 
	end
end

function chondinhquocld()
	for i=402,406 do AddItem2(2,0,i,0,0,0) end 
end

function hoangkimmonphai345()
	Say("Ng­¬i muèn nhËn trang bÞ nµo ?",11,
	"Thiªn v­¬ng./thienvuong",
	"ThiÕu L©m./thieulam",
	"Nga My./ngamy",
	"Thóy yªn./thuyyen",
	"Ngò §éc./namdoc",
	"Thiªn NhÉn./thiennhan",
	"§­êng M«n./duongmon",
	"C¸i Bang./caibang",
	"Vâ §ang./vodang",
	"C«n L«n./conlon",
	"Ta kh«ng cÇn./no")
end

function thienvuong()
	Say("Ng­¬i muèn lÊy trang bÞ nµo ?",4,
	"H¸m thiªn/hamthien",
	"KÕ nghiÖp/kenghiep",
	"Ngù long/ngulong",
	"Ta kh«ng cÇn/no")
end

function ngamy()
	Say("Ng­¬i muèn lÊy trang bÞ nµo ?",4,
	"V« Gian/vogian",
	"V« YÓm/voyem",
	"V« TrÇn/votran",
	"Ta kh«ng cÇn/no")
end

function namdoc()
	Say("Ng­¬i muèn lÊy trang bÞ nµo ?",3,
	"Minh HuyÔn/minhhuyen",
	"U L«ng/ulong",
	"Ta kh«ng cÇn/no")
end

function thuyyen()
	Say("Ng­¬i muèn lÊy trang bÞ nµo ?",3,
	"Tª Hoµng/tehoang",
	"BÝch H¶i/bichhai",
	"Ta kh«ng cÇn/no")
end

function duongmon()
	Say("Ng­¬i muèn lÊy trang bÞ nµo ?",5,
	"S©m Hoµng/samhoang",
	"B¨ng Hµn/banghan",
	"Thiªn Quang/thienquang",
	"§Ých Ph¸ch/dichphach",
	"Ta kh«ng cÇn/no")
end

function caibang()
	Say("Ng­¬i muèn lÊy trang bÞ nµo ?",3,
	"§ång Cõu/dongcuu",
	"§Þch Hi/masat",
	"Ta kh«ng cÇn/no")
end

function thiennhan()
	Say("Ng­¬i muèn lÊy trang bÞ nµo ?",4,
	"Ma S¸t/dichhi",
	"Ma Hoµng/mahoang",
	"Ma ThÞ/mathi",
	"Ta kh«ng cÇn/no")
end

function vodang()
	Say("Ng­¬i muèn lÊy trang bÞ nµo ?",3,
	"CËp Phong/capphong",
	"L¨ng Nh¹c/langnhac",
	"Ta kh«ng cÇn/no")
end

function conlon()
	Say("Ng­¬i muèn lÊy trang bÞ nµo ?",4,
	"S­íng Tinh/suongtinh",
	"L«i Khñng/loikhung",
	"Vô HuyÒn/vuhuyen",
	"Ta kh«ng cÇn/no")
end

function thieulam()
	Say("Ng­¬i muèn lÊy trang bÞ nµo ?",4,
	"Méng Long/monglong",
	"Phôc Ma/phucma",
	"Tø Kh«ng/tukhong",
	"Ta kh«ng cÇn/no")
end

function monglong()
	for i=1,5 do AddGoldItem(i,0) end      
end
		
function phucma()
	for i=6,10 do AddGoldItem(i,0) end      
end

function tukhong()
	for i=11,15 do AddGoldItem(i,0) end      
end

function hamthien()
	for i=16,20 do AddGoldItem(i,0) end      
end

function kenghiep()
	for i=21,25 do AddGoldItem(i,0) end      
end
		
function ngulong()
	for i=26,30 do AddGoldItem(i,0) end      
end

function vogian()
	for i=31,35 do AddGoldItem(i,0) end      
end

function voyem()
	for i=36,40 do AddGoldItem(i,0) end      
end

function votran()
	for i=41,45 do AddGoldItem(i,0) end      
end

function tehoang()
	for i=46,50 do AddGoldItem(i,0) end      
end

function bichhai()
	for i=51,55 do AddGoldItem(i,0) end      
end

function ulong()
	for i=56,60 do AddGoldItem(i,0) end      
end

function minhhuyen()
	for i=61,65 do AddGoldItem(i,0) end
end

function chunhuoc()
	for i=66,70 do AddGoldItem(i,0) end      
end

function thienquang()
	for i=76,80 do AddGoldItem(i,0) end      
end

function banghan()
	for i=71,75 do AddGoldItem(i,0) end      
end

function dichphach()
	for i=81,85 do AddGoldItem(i,0) end      
end

function samhoang()
	for i=86,90 do AddGoldItem(i,0) end
end

function dongcuu()
	for i=91,95 do AddGoldItem(i,0) end      
end

function masat()
	for i=96,100 do AddGoldItem(i,0) end      
end

function dichhi()
	for i=101,105 do AddGoldItem(i,0) end      
end

function mahoang()
	for i=106,110 do AddGoldItem(i,0) end      
end

function mathi()
	for i=111,115 do AddGoldItem(i,0) end      
end

function langnhac()
	for i=116,120 do AddGoldItem(i,0) end      
end

function capphong()
	for i=121,125 do AddGoldItem(i,0) end      
end		

function suongtinh()
	for i=126,130 do AddGoldItem(i,0) end      
end

function loikhung()
	for i=131,135 do AddGoldItem(i,0) end      
end

function vuhuyen()
	for i=136,140 do AddGoldItem(i,0) end      
end

function dongsat()
	for i=493,496 do AddItem2(2,0,i,0,0,0) end            
end

function thienhoang()
	for i=167,175 do AddItem2(2,0,i,0,0,0) end      
end

function hiepcot()
	for i=185,188 do AddItem2(2,0,i,0,0,0) end     
end

function hiepcotcucpham()
	for i=185,188 do AddItem2(2,0,i,0,0,0) end       
end

function nhutinhcucpham()
	for i=189,192 do AddItem2(2,0,i,0,0,0) end      
end

function nhutinh()
	for i=189,192 do AddItem2(2,0,i,0,0,0,1) end     
end

function kimphong()
	for i=177,185 do AddGoldItem(i,0) end      
end
