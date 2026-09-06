Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("\\script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")

function myplayersex()
	if GetSex() == 1 then 
		return "N÷ HiÖp";
	else
		return "§¹i HiÖp";
	end
end

function main(nNpcindex)
 dofile("script/event/eventtet/npcsukien.lua");
	SayEx({"<color=yellow>NPC Sù KiÖn TÕt §oµn Viªn<color>: ho¹t ®éng <color=white>Mõng ngµy TÕt §oµn Viªn<color> diÔn ra tõ ngµy <bclr=blue>10-02-2026 ®Õn 28-02-2026<bclr>, tham gia sù kiÖn ®ång ®¹o sÏ nhËn rÊt nhiÒu kinh nghiÖm vµ vËt phÈm gi¸ trÞ.",
	"Mua Nguyªn LiÖu §Æc BiÖt/muaquasungsll",
	"GhÐp M¨m Ngò Qu¶/ghepevent",
    -- "NhËn Th­ëng Mèc Event/NhanMocEvent",
    -- "Reset Event /reset",
	"Ta ph¶i rêi ®i/no"})	
end
function muaquasungsll()
OpenGetNumber ("NhËp sè l­îng" , "muaquasung")
end

function muaquasung()
local nSoLuongV = GetNumberFromUI()
local nDinhGia = 1
local nTongTien = nSoLuongV* nDinhGia
local nDonGia = 1

	if (nSoLuongV < 1 or nSoLuongV > 500) then 
	Talk(1,"","Mçi lÇn ®æi tèi ®a 500 Qu¶ Sung") 
	return end 
	
	if (GetTask(T_PLAYER_XU) < nTongTien) then 
	Talk(1,"","B¹n kh«ng ®ñ <color=yellow>"..nSoLuongV*nDonGia.." Xu <color>§Ó Mua Qu¶ Sung.") 
	return end
	
	if (GetTask(T_PLAYER_XU) > nTongTien) then 
		SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) - nTongTien)
		AddItemSL(4840,nSoLuongV,0) -- event tet 
		Msg2Player("Chóc mõng <color=green>"..myplayersex().."<color> mua thµnh c«ng <color=yellow>"..nSoLuongV.." Qu¶ Sung<color> ")
		else
		Talk(1,"","B¹n kh«ng ®ñ <color=yellow>"..nSoLuongV*nDonGia.." Xu <color>§Ó Mua Qu¶ Sung.!")
	end
end

function ghepevent()
OpenGetNumber("NhËp sè l­îng","nhancodegioihan")
end

function nhancodegioihan()--thay doi hµm num phï hîp víi server
local nSoLuongV = GetNumberFromUI()

	if (nSoLuongV < 1 or nSoLuongV > 500) then 
	Talk(1,"","Mçi lÇn ®æi tèi ®a 500 M¨m Vµng Ngñ Qu¶") 
	return end 
	
	if (GetItemCount(0,6,1,4836,-1,-1, pos_equiproom) < nSoLuongV) then-- ®Õm sè event trong hµnh trang
		Talk(1,"","Nhµ ng­¬i kh«ng cã ®ñ <color=green>CÇu<color>");
		return end;
		
	if (GetItemCount(0,6,1,4837,-1,-1, pos_equiproom) < nSoLuongV) then -- ®Õm sè event trong hµnh trang
		Talk(1,"","Nhµ ng­¬i kh«ng cã ®ñ <color=green>Dõa<color>");
		return end;
		
	if (GetItemCount(0,6,1,4838,-1,-1, pos_equiproom) < nSoLuongV) then-- ®Õm sè event trong hµnh trang
		Talk(1,"","Nhµ ng­¬i kh«ng cã ®ñ <color=green>§ñ<color>");
		return end;
		
	if (GetItemCount(0,6,1,4839,-1,-1, pos_equiproom) < nSoLuongV) then-- ®Õm sè event trong hµnh trang
		Talk(1,"","Nhµ ng­¬i kh«ng cã ®ñ <color=green>Xoµi<color>");
		return end;
		
	if (GetItemCount(0,6,1,4840,-1,-1, pos_equiproom) < nSoLuongV) then-- ®Õm sè event trong hµnh trang
		Talk(1,"","Nhµ ng­¬i kh«ng cã ®ñ <color=green>Xung<color>");
		return end;
	
	DelItem(0, -1, 6,1 ,4836, -1, -1, pos_equiproom, nSoLuongV) -- xo¸ sè Event trong hµnh trang
	DelItem(0, -1, 6,1 ,4837, -1, -1, pos_equiproom, nSoLuongV) -- xo¸ sè Event trong hµnh trang
	DelItem(0, -1, 6,1 ,4838, -1, -1, pos_equiproom, nSoLuongV) -- xo¸ sè Event trong hµnh trang
	DelItem(0, -1, 6,1 ,4839, -1, -1, pos_equiproom, nSoLuongV) -- xo¸ sè Event trong hµnh trang
	DelItem(0, -1, 6,1 ,4840, -1, -1, pos_equiproom, nSoLuongV) -- xo¸ sè Event trong hµnh trang
	AddItemSL(4842,nSoLuongV,0) -- event tet 
	Msg2Player("Chóc mõng <color=green>"..myplayersex().."<color> ghÐp thµnh c«ng <color=yellow>"..nSoLuongV.." M¨m Vµng Ngñ Qu¶. <color> ")
end;


function reset()
if GetTask(TASK_RESTEVENTMAMNGUQUA) > 0 then  
Talk(1,"","§¹i hiÖp ®· reset Event nµy råi") return end
if GetTask(TASK_NHANMOCMAMNGUQUA) < 5 then  
Talk(1,"","§¹i hiÖp ch­a nhËn mèc th­ëng Event ") return end
	if GetTask(TASK_MAMNGUQUA) < 10000 then 
	Talk(1,"","§¹i HiÖp Ch­a Sö Dông §ñ 10000 M¨m Vµng Ngñ Qu¶ , Sè l­îng hiÖn t¹i : "..GetTask(TASK_MAMNGUQUA).." ") 
	return end ---Tïy ChØnh Sè L­îng Resex
	if (GetItemCount(0,6,1,4835,-1,-1, pos_equiproom) < 30) then
	Talk(1,"","Kh«ng mang ®ñ 30 TiÒn §ång, vui lßng kiÓm tra l¹i") 
	return end
SetTask(TASK_MAMNGUQUA,0)
SetTask(TASK_NHANMOCMAMNGUQUA,0)
SetTask(TASK_RESTEVENTMAMNGUQUA,1)
DelItem(0, -1, 6,1 ,4835, -1, -1, ROOME, 10) -- xo¸ sè Event trong hµnh trang
Talk(1,"","B¹n ®· reset Event thµnh c«ng h·y tiÕp tôc sö dông")
end

-------------------------NhËn Th­ëng Event---------------------------------


function NhanMocEvent()
	SayEx({"Chµo"..myplayersex().."T×m Ta Cã ViÖc G× Kh«ng?",
	"NhËn Mèc 2k/Moc2k",
	"NhËn Mèc 4k/Moc4k",
	"NhËn Mèc 6k/Moc6k",
	"NhËn Mèc 8k/Moc8k",
	"NhËn Mèc 10k/Moc10k",
	"Ta ph¶i rêi ®i/no"})	
end
function Moc2k()
		local mamnguqua =   GetTask(TASK_MAMNGUQUA)
if mamnguqua  < 2000 then Talk(1,"","§¹i HiÖp Ch­a Sö Dông §ñ 2.000 M¨m Vµng Ngò Qu¶ , Sè l­îng hiÖn t¹i : "..mamnguqua.." ") return end
if GetTask(TASK_NHANMOCMAMNGUQUA) > 0 then  
Talk(1,"","§¹i hiÖp ®· nhËn phÇn th­ëng nµy råi") return end
SetTask(TASK_NHANMOCMAMNGUQUA,1)
AddGoldItem(168,0)       
AddGoldItem(169,0)       
AddGoldItem(170,0)       
	---thªm phÇn th­ëng vµo ®ay
	logPhanThuong(""..GetAccount().."\t\t"..GetName().."\t\t nhËn Mèc Event ")
		Msg2SubWorld("<pic=135><color=cyan> Chóc mõng <color=pink>"..GetName().."<color> nhËn Mèc 2k Event Thµnh C«ng ")
end

function Moc4k()
		local mamnguqua =   GetTask(TASK_MAMNGUQUA)
if mamnguqua  < 4000 then Talk(1,"","§¹i HiÖp Ch­a Sö Dông §ñ 4.000 M¨m Vµng Ngò Qu¶ , Sè l­îng hiÖn t¹i : "..mamnguqua.." ") return end
if GetTask(TASK_NHANMOCMAMNGUQUA) < 1 then  
Talk(1,"","§¹i hiÖp ch­a nhËn mèc 2k") return end
if GetTask(TASK_NHANMOCMAMNGUQUA) > 1 then  
Talk(1,"","§¹i hiÖp ®· nhËn phÇn th­ëng nµy råi") return end
SetTask(TASK_NHANMOCMAMNGUQUA,2)
AddGoldItem(171,0)       
AddGoldItem(172,0) 
AddGoldItem(173,0) 
	---thªm phÇn th­ëng vµo ®ay
	logPhanThuong(""..GetAccount().."\t\t"..GetName().."\t\t nhËn Mèc Event ")
		Msg2SubWorld("<pic=135><color=cyan> Chóc mõng <color=pink>"..GetName().."<color> nhËn Mèc 4k Event Thµnh C«ng ")
end

function Moc6k()
		local mamnguqua =   GetTask(TASK_MAMNGUQUA)
if mamnguqua  < 6000 then Talk(1,"","§¹i HiÖp Ch­a Sö Dông §ñ 6.000 M¨m Vµng Ngò Qu¶ , Sè l­îng hiÖn t¹i : "..mamnguqua.." ") return end
if GetTask(TASK_NHANMOCMAMNGUQUA) < 2 then  
Talk(1,"","§¹i hiÖp ch­a nhËn mèc 4k") return end
if GetTask(TASK_NHANMOCMAMNGUQUA) > 2 then  
Talk(1,"","§¹i hiÖp ®· nhËn phÇn th­ëng nµy råi") return end
SetTask(TASK_NHANMOCMAMNGUQUA,3)
AddGoldItem(174,0)       
AddGoldItem(175,0)
AddGoldItem(176,0)
	---thªm phÇn th­ëng vµo ®ay
	logPhanThuong(""..GetAccount().."\t\t"..GetName().."\t\t nhËn Mèc Event ")
		Msg2SubWorld("<pic=135><color=cyan> Chóc mõng <color=pink>"..GetName().."<color> nhËn Mèc 6k Event Thµnh C«ng ")
end

function Moc8k()
		local mamnguqua =   GetTask(TASK_MAMNGUQUA)
if mamnguqua  < 8000 then Talk(1,"","§¹i HiÖp Ch­a Sö Dông §ñ 8.000 M¨m Vµng Ngò Qu¶ , Sè l­îng hiÖn t¹i : "..mamnguqua.." ") return end
if GetTask(TASK_NHANMOCMAMNGUQUA) < 3 then  
Talk(1,"","§¹i hiÖp ch­a nhËn mèc 6k") return end
if GetTask(TASK_NHANMOCMAMNGUQUA) > 3 then  Talk(1,"","§¹i hiÖp ®· nhËn phÇn th­ëng nµy råi") return end
SetTask(TASK_NHANMOCMAMNGUQUA,4)

		mIndexC = AddGoldItem(434,0); --Hong Anh
		-- SetPlayerItemLock(mIndexC, -2)
		AddTimeItem(mIndexC,60*60*24*30);  -- thêi gia item 7 ngµy
		mIndex = AddGoldItem(435,0); --Hong Anh
		-- SetPlayerItemLock(mIndex, -2)
		AddTimeItem(mIndex,60*60*24*30);  -- thêi gia item 7 ngµy
	---thªm phÇn th­ëng vµo ®ay
	logPhanThuong(""..GetAccount().."\t\t"..GetName().."\t\t nhËn Mèc Event ")
		Msg2SubWorld("<pic=135><color=cyan> Chóc mõng <color=pink>"..GetName().."<color> nhËn Mèc 8k Event Thµnh C«ng ")
end

function Moc10k()
		local mamnguqua =   GetTask(TASK_MAMNGUQUA)
if mamnguqua  < 10000 then Talk(1,"","§¹i HiÖp Ch­a Sö Dông §ñ 10.000 M¨m Vµng Ngò Qu¶ , Sè l­îng hiÖn t¹i : "..mamnguqua.." ") return end
if GetTask(TASK_NHANMOCMAMNGUQUA) < 4 then  
Talk(1,"","§¹i hiÖp ch­a nhËn mèc 8k") return end
if GetTask(TASK_NHANMOCMAMNGUQUA) > 4 then  Talk(1,"","§¹i hiÖp ®· nhËn phÇn th­ëng nµy råi") return end
SetTask(TASK_NHANMOCMAMNGUQUA,5)
		mIndexC = AddGoldItem(436,0); --Hong Anh
		-- SetPlayerItemLock(mIndexC, -2)
		AddTimeItem(mIndexC,60*60*24*30);  -- thêi gia item 7 ngµy
		mIndex = AddGoldItem(437,0); --Hong Anh
		-- SetPlayerItemLock(mIndex, -2)
		AddTimeItem(mIndex,60*60*24*30);  -- thêi gia item 7 ngµy
	---thªm phÇn th­ëng vµo ®ay
	
	logPhanThuong(""..GetAccount().."\t\t"..GetName().."\t\t nhËn Mèc Event ")
		Msg2SubWorld("<pic=135><color=cyan> Chóc mõng <color=pink>"..GetName().."<color> nhËn Mèc 10k Event Thµnh C«ng ")
end

function logPhanThuong(str)
local gm_Log = "dulieu/Event/NhanMocEvent.txt"
local fs_log = openfile(gm_Log, "a");
write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t"..str.."\n");
closefile(fs_log);
end