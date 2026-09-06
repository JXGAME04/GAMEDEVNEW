--Author: Fong KiÒu
--Date: 09/07/2021
--Function: Lib vËt phÈm

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_vatpham.lua")

function AddRandXuHoatDong()
	local numran = random(1,100)
	local numaddx = 0
	
	if(numran > 99) then
		numaddx = 100
		AddEventItem(ID_XU_TIENDONG, numaddx)
	elseif(numran > 90) then
		numaddx = 10
		AddEventItem(ID_XU_TIENDONG, numaddx)
	elseif(numran > 80) then
		numaddx = 2
		AddEventItem(ID_XU_TIENDONG, numaddx)
	elseif(numran > 60) then
		numaddx = 1
		AddEventItem(ID_XU_TIENDONG, numaddx)		
	end
	if(numaddx > 0) then
		local szNews = format("Chóc mõng <color=red>"..GetName().."<color> ®· më hép lÔ vËt nhËn ®­îc tiÒn xu.")
		AddGlobalCountNewsEx(szNews,3)	
		Msg2Player(format("May m¾n nhËn ®­îc %d TiÒn xu", numaddx))
	end
end

function AddRandNHKTHoatDong()
	local numran = random(1,100)
	local numaddx = 0
	
	if(numran > 99) then
		numaddx = 100
		AddItem(ITEM_NHKT[1],ITEM_NHKT[2],ITEM_NHKT[3], 0,0,0,0,0,0,0,0,0,numaddx,0,0)
	elseif(numran > 90) then
		numaddx = 10
		AddItem(ITEM_NHKT[1],ITEM_NHKT[2],ITEM_NHKT[3], 0,0,0,0,0,0,0,0,0,numaddx,0,0)
	elseif(numran > 80) then
		numaddx = 2
		AddItem(ITEM_NHKT[1],ITEM_NHKT[2],ITEM_NHKT[3], 0,0,0,0,0,0,0,0,0,numaddx,0,0)
	elseif(numran > 60) then
		numaddx = 1
		AddItem(ITEM_NHKT[1],ITEM_NHKT[2],ITEM_NHKT[3], 0,0,0,0,0,0,0,0,0,numaddx,0,0)
	end
	if(numaddx > 0) then
		local szNews = format("Chóc mõng <color=red>"..GetName().."<color> ®· më hép lÔ vËt nhËn ®­îc Ngò hµnh kú th¹ch.")
		AddGlobalCountNewsEx(szNews,3)	
		Msg2Player(format("May m¾n nhËn ®­îc %d Ngò hµnh kú th¹ch", numaddx))
	end
end

function layvatpham()
	SayEx({"Xin chµo <color=red><player><color><enter> xin mêi chän vËt phÈm.",
	"NhËn thÇn hµnh phï/thanhanhphu235",
	"NhËn nh¹c v­¬ng kiÕm/nhacvuongkkkk",				
	"NhËn LÖnh Bµi Boss /nhanlbb",				
	--"NhËn thæ ®Þa phï/thodiaphu235",
	-- "Tiªn th¶o lé/tienthaolo447",
	"NhËn X¸ Lîi Kim §¬n/xaloikimdon",
	-- "R­¬ng NT HC/ruonghoangkim",
	-- "R­¬ng Tr©n B¶o/ruonghoangkim2",
	--"Qu¶ huy hoµng/quahuyhoang223",
	"NhËn tói d­îc phÈm/tuiduocpham1238",
	-- "LÖnh bµi t©n thñ/lenhbaitanthu346",
	--"LÖnh bµi Admin/lbadmin9987",
	"Vâ L©m mËt tÞch TTK/volaammattichttk667",
	--"LÖnh bµi gäi boss/lbgoiboss12",
	-- "Phóc Duyªn Lé/addPhucDuyenLo",
	"§¹i thµnh bÝ kÝp/daithanhbikip",
	--"Ngò Hµnh Kú Th¹ch/Nhan500NHKT",
	-- "LÖnh bµi hoµn thµnh D· TÈu/lenhbaihtdatau",
	-- "Mai Thuý §¸/NhanPTLBCC",
	"KÕt thóc ®èi tho¹i/no"})
end
function nhanlbb()
	AddItemSL(1023,100,0)
end
function lenhbaihtdatau()
	for i=1,5 do
		AddItem(6,1,4818,0,0,0,0)--Lenh bai hoan thanh da tau
	end
end

function ruonghoangkim()
	for i=1,5 do
		AddItem(6,1,4816,0,0,0,0)--Bao ruong NT HC
	end
end

function ruonghoangkim2()
	for i=1,5 do
		AddItem(6,1,4817,0,0,0,0)--Bao ruong Tran bao
	end
end

function NhanPTLBCC()
	for i=1,50 do
		AddItem(6,1,155,0,0,0,0)--lenh bai
		AddItem(6,1,156,0,0,0,0)--chien co
		for j = 177, 194 do
			AddItem(6,1,j,0,0,0,0)--lag tong kim
		end
		for k = 1, 10 do
			AddItem(6,1,k,0,0,0,0)
		end
	end
end

function Nhan500NHKT()
	local num = 500
	AddItem(ITEM_NHKT[1],ITEM_NHKT[2],ITEM_NHKT[3], 0,0,0,0,0,0,0,0,0,num,0,0)
end

function quahuyhoang223()
	local itemIdx = AddItem(6,1,905,0,0,0,0)--qua thap
	AddTimeItem(itemIdx,60*60*24*2)

	itemIdx = AddItem(6,1,906,0,0,0,0)--qua trung
	AddTimeItem(itemIdx,60*60*24*2)

	itemIdx = AddItem(6,1,907,0,0,0,0)--qua cao
	AddTimeItem(itemIdx,60*60*24*2)

	itemIdx = AddItem(6,1,908,0,0,0,0);--qua hoang kim
	AddTimeItem(itemIdx,60*60*24*2)
end

function tienthaolo447()
	AddItem(6,1,71,0,0,0,0)
	AddItem(6,1,1182,0,0,0,0) -- tien thao lo dac biet
end

function daithanhbikip()
	-- AddItem(6,1,27,0,0,0,0)
	-- AddItem(6,1,28,0,0,0,0)

	-- AddItem(6,1,33,0,0,0,0)
	-- AddItem(6,1,34,0,0,0,0)
	-- AddItem(6,1,35,0,0,0,0)
	-- AddItem(6,1,36,0,0,0,0)
	-- AddItem(6,1,37,0,0,0,0)
	-- AddItem(6,1,38,0,0,0,0)
	-- AddItem(6,1,39,0,0,0,0)
	-- AddItem(6,1,40,0,0,0,0)
	-- AddItem(6,1,41,0,0,0,0)
	-- AddItem(6,1,42,0,0,0,0)
	-- AddItem(6,1,43,0,0,0,0)

	-- AddItem(6,1,45,0,0,0,0)
	-- AddItem(6,1,46,0,0,0,0)
	-- AddItem(6,1,47,0,0,0,0)
	-- AddItem(6,1,48,0,0,0,0)
	-- AddItem(6,1,49,0,0,0,0)
	-- AddItem(6,1,50,0,0,0,0)
	-- AddItem(6,1,51,0,0,0,0)
	-- AddItem(6,1,52,0,0,0,0)
	-- AddItem(6,1,53,0,0,0,0)
	-- AddItem(6,1,54,0,0,0,0)
	-- AddItem(6,1,55,0,0,0,0)
	-- AddItem(6,1,56,0,0,0,0)
	-- AddItem(6,1,57,0,0,0,0)
	-- AddItem(6,1,58,0,0,0,0)
	-- AddItem(6,1,59,0,0,0,0)

	AddItem(6,1,2433,0,0,0,0)
	AddItem(6,1,2433,0,0,0,0)
	AddItem(6,1,2433,0,0,0,0)
	AddItem(6,1,2434,0,0,0,0)
	--AddItem(6,1,3208,0,0,0,0)--dai thanh bk 150

	AddItem(6,1,1126,0,0,0,0)--bi kip 120
end
function xaloikimdon()
for i=1,10 do
		AddItem(6,1,2629,0,0,0,0)
end
end
function lbadmin9987()
	local itemIdx = AddItem(6,1,4814,0,0,0,0)
	SetPlayerItemLock(itemIdx, -2) --kho¸ vÜnh viÔn
end

function tuiduocpham1238()
	local itemIdx = AddItem(6,1,4813,0,0,0,0)
	SetPlayerItemLock(itemIdx, -2)
end

function nhacvuongkkkk()
	AddEventItem(195,1)
	-- AddEventItem(507,100)
end

function addPhucDuyenLo()
	AddItem(6,1,121,0,0,0,0)
	AddItem(6,1,122,0,0,0,0)
	AddItem(6,1,123,0,0,0,0)

	AddItem(6,1,124,0,0,0,0)--QHT
	AddItem(6,1,72,0,0,0,0)--TSBL
	AddItem(6,1,73,0,0,0,0)--BQL
end

function lbgoiboss12()
	AddItem(6,1,1023,0,0,0,0)
end

function thanhanhphu235()
	local itemIdx = AddItem(6,1,1271,0,0,0,0)
end

function thodiaphu235()
	AddItem(5,-1,0,0,0,0,0)
end

function lenhbaitanthu346()
	local itemIdx = AddItem(6,1,4812,0,0,0,0)
	SetPlayerItemLock(itemIdx, -2)
end

function volaammattichttk667()
	for i=1,15 do 
		AddItem(6,1,26,0,0,0,0)--vlmt
		AddItem(6,1,22,0,0,0,0)--ttk
	end

	-- AddItem(6,1,12,0,0,0,0)--ban nhuoc tam kinh
	-- AddItem(6,1,18,0,0,0,0)--tam tam tuong anh phu
	-- AddItem(6,1,23,0,0,0,0)--thiet la han
	-- AddItem(6,1,20,0,0,0,0)--hoa hong
	
	-- for i=1,10 do 
		-- AddItem(6,1,i,0,0,0,0)
	-- end
end
