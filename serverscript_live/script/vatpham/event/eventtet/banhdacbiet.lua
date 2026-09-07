-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local NhanVatPham



Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("\\script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
--===============Config Event===============--
DangCapSuDung 	= 60
ToiDaSuDung 	= 20000
TenVatPham		= "M¨m Vµng Ngò Qu¶"
SoKinhNghiem	= {{5000000,5},}

---============================

function main(nItemIndex)
dofile("script/item/event/eventtet/banhdacbiet.lua")
	nTaskValue = GetTask(TASK_MAMNGUQUA);
	
	if (CheckRoom(2,3) == 0) then Talk(1,"","Vui lßng chõa Ýt nhÊt 2x3 « trèng.") return end
      
	if (GetLevel() < DangCapSuDung) then Talk(1,"","§¼ng cÊp trªn "..DangCapSuDung.." míi cã thÓ sö dông vËt phÈm nµy.") return end
	----------------------------------------------------
	if ( nTaskValue >= ToiDaSuDung ) then Talk(1,"","Ng­¬i ®· dïng <color=red>"..ToiDaSuDung.." c¸i "..TenVatPham.." <color>råi kh«ng thÓ sö dông thªm.") return end

	SetTask(TASK_MAMNGUQUA,GetTask(TASK_MAMNGUQUA)+1); 
	Msg2Player("<color=white>B¹n sö dông<color=cyan> "..GetTask(TASK_MAMNGUQUA).."/"..ToiDaSuDung.."<color><color=orange> "..TenVatPham.." <color>nhËn ®­îc <color=yellow>"..SoKinhNghiem[1][2].."<color> triÖu ®iÓm kinh nghiÖm.")
	AddSumExp(SoKinhNghiem[1][1])

    NhanVatPham()
	
	if nTaskValue >= 1000 and nTaskValue <= 20001 and mod(nTaskValue, 1000) == 0 then
			local nrandomVip = random(1,30)
			if nrandomVip == 5 then		
						local nIndexx = AddItem2(2, 0, random(140,141), 0, 0, 0)
						local s_name = GetItemName(nIndexx)
						Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Bao L× X× N¨m Míi	!")
						AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Bao L× X× N¨m Míi	! ",3)
					else 
						local nIndexx =  AddItemSL(139,1,0) --- Thiep nam moi
						local s_name = GetItemName(nIndexx)
						AddGlobalCountNews("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Bao L× X× N¨m Míi	! ",3)
						Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Bao L× X× N¨m Míi	!")
					end
					thoigian = tonumber(date("%H%M%d%m"))
					LoginLog = openfile("dulieu/NhanMocEventTet.txt", "a");
					if LoginLog then
					write(LoginLog,"Thanh Cong: ID:- Name: ["..GetName().."] - Acc: ["..GetAccount().."] - Moc: ["..nTaskValue.."] - Time: ["..thoigian.."] \n");
			end
					closefile(LoginLog)
	end
	if nTaskValue == 19999  then
	local nIndexx = AddItem2(2, 0, 140, 0, 0, 0)
	local s_name = GetItemName(nIndexx)
	local nIndexy = AddItem2(2, 0, 141, 0, 0, 0)
	local s_namex = GetItemName(nIndexy)
	Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_name.." <color=red> khi sö dông Event N¨m Míi	!")
	Msg2SubWorld("Chóc mõng ng­êi ch¬i <color=yellow>"..GetName().." <color=red>. §· may m¾n nhËn ®­îc <color=green>"..s_namex.." <color=red> khi sö dông Event N¨m Míi	!")
	end
	RemoveItem(nItemIndex,1,1)
end;


function NhanVatPham()
 local a = random(1,300)
	if (a == 20) then
		AddItemSL(71,1,0) -- Tiªn Th¶o Lé Th­êng
	elseif(a == 1) then
		AddItemSL(1816,1,0) -- Ngò s¾c long ch©u (Kim)
	elseif(a == 25) then
		AddItemSL(1817,1,0) -- Ngò s¾c long ch©u (Méc)
	elseif(a == 30) then
		AddItemSL(1818,1,0) -- Ngò s¾c long ch©u (Thñy)
	elseif(a == 34) then
		AddItemSL(1819,1,0) -- Ngò s¾c long ch©u (Háa)
	elseif (a == 38) then
		AddItemSL(1820,1,0) -- Ngò s¾c long ch©u (Thæ)
	RemoveItem(nItemIndex,1,1);
	end
end


function logPhanThuong(str)
local gm_Log = "dulieu/SuKien/sukienevent.txt"
local fs_log = openfile(gm_Log, "a");
write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t"..str.."\n");
closefile(fs_log);
end
