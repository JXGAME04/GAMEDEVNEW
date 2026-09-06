-- test_tongjx2.lua - kiem tra he du lieu bang hoi kieu JX2 (dot 1)
-- CACH DUNG: chep vao bin\server\script\test\test_tongjx2.lua roi cho mot NPC
-- bat ky goi:  dofile("script/test/test_tongjx2.lua")  main()
-- (hoac tam thay noi dung script cua mot NPC it dung bang dong dofile + main tren)
--
-- Y NGHIA: ho Apply* CHI GUI len relay (dung mo hinh JX2) -> gia tri hien ra
-- SAU KHI relay echo ve, tuc la bam nut ghi xong phai BAM LAI NPC de thay so moi.
-- Muon thu do ben: ghi vai gia tri -> tat relay -> bat lai -> bam NPC xem con khong.

TJX2_TASKID = 1002	-- bien nhiem vu bang (phai > 48 theo luat JX2)

function tjx2_get_mytong()
	local szTong = GetTongName()
	if (szTong == nil or szTong == "") then
		return 0
	end
	return TONG_Name2ID(szTong)
end

function main()
	local nCount = TONG_GetTongCount()
	local nMyTong = tjx2_get_mytong()
	local msg = "He JX2: ban sao dang co <color=yellow>"..nCount.."<color> bang.<enter>"
	if (nCount == 0) then
		msg = msg.."<color=red>Ban sao rong!<color> Kiem tra: relay da chay ban moi chua? GS co noi vao relay khong?<enter>"
		Say(msg, 0)
		return
	end
	if (nMyTong ~= 0) then
		msg = msg.."Bang cua ban: <color=green>"..TONG_GetName(nMyTong).."<color> (id "..nMyTong..")<enter>"
		msg = msg.."TaskValue["..TJX2_TASKID.."] = <color=cyan>"..TONG_GetTaskValue(nMyTong, TJX2_TASKID).."<color><enter>"
		msg = msg.."Ngan quy JX2 (64-bit) = <color=cyan>"..TONG_GetMoney(nMyTong).."<color><enter>"
		msg = msg.."Thanh vien = "..TONG_GetMemberCount(nMyTong)..", online (GS nay) = "..TONG_GetOnlineCount(nMyTong).."<enter>"
		msg = msg.."Bang chu: "..TONG_GetMaster(nMyTong).."<enter>"
		msg = msg.."Ban trong ho so bang: "..TONGM_IsExist(nMyTong, GetName())..", figure = "..TONGM_GetFigure(nMyTong, GetName()).."<enter>"
		msg = msg.."Cong hien (khoa 7) = "..TONGM_GetOffer(nMyTong, GetName()).."<enter>"
		msg = msg.."CheckRight(2006 - tuyet ky) = "..TONGM_CheckRight(nMyTong, GetName(), 2006).."<enter>"
		Say(msg, 7,
			"Ghi TaskValue = 777/tjx2_set777",
			"Cong TaskValue them 5/tjx2_add5",
			"Cong ngan quy +123456789/tjx2_money",
			"Tru ngan quy -456789/tjx2_moneytru",
			"Cong cong hien +99/tjx2_offer",
			"Cap quyen 2006 cho ban/tjx2_addright",
			"Thu quyen 2006/tjx2_delright")
	else
		msg = msg.."<color=red>Ban chua vao bang<color> - vao bang roi bam lai de test ghi/doc.<enter>Danh sach bang (toi da 10):<enter>"
		local id = TONG_GetFirstTong()
		local n = 0
		while (id ~= 0 and n < 10) do
			msg = msg.."- "..TONG_GetName(id).." ("..TONG_GetMemberCount(id).." tv, tien "..TONG_GetMoney(id)..")<enter>"
			id = TONG_GetNextTong(id)
			n = n + 1
		end
		Say(msg, 0)
	end
end

function tjx2_set777()
	local id = tjx2_get_mytong()
	if (id == 0) then
		return
	end
	TONG_ApplySetTaskValue(id, TJX2_TASKID, 777)
	Msg2Player("Da gui SET TaskValue["..TJX2_TASKID.."] = 777. Bam lai NPC de xem echo tu relay.")
end

function tjx2_add5()
	local id = tjx2_get_mytong()
	if (id == 0) then
		return
	end
	TONG_ApplyAddTaskValue(id, TJX2_TASKID, 5)
	Msg2Player("Da gui ADD TaskValue["..TJX2_TASKID.."] +5. Bam lai NPC de xem.")
end

function tjx2_money()
	local id = tjx2_get_mytong()
	if (id == 0) then
		return
	end
	TONG_ApplyAddMoney(id, 123456789)
	Msg2Player("Da gui cong ngan quy +123456789. Bam lai NPC de xem.")
end

function tjx2_moneytru()
	local id = tjx2_get_mytong()
	if (id == 0) then
		return
	end
	TONG_ApplyAddMoney(id, -456789)
	Msg2Player("Da gui tru ngan quy -456789. Bam lai NPC de xem.")
end

function tjx2_offer()
	local id = tjx2_get_mytong()
	if (id == 0) then
		return
	end
	TONGM_ApplyAddOffer(id, GetName(), 99)
	Msg2Player("Da gui cong 99 cong hien cho "..GetName()..". Bam lai NPC de xem.")
end

function tjx2_addright()
	local id = tjx2_get_mytong()
	if (id == 0) then
		return
	end
	TONGM_ApplyAddRight(id, GetName(), 2006)
	Msg2Player("Da gui cap quyen 2006. Bam lai NPC - neu ban KHONG phai bang chu thi CheckRight se doi 0 -> 1.")
end

function tjx2_delright()
	local id = tjx2_get_mytong()
	if (id == 0) then
		return
	end
	TONGM_ApplyDelRight(id, GetName(), 2006)
	Msg2Player("Da gui thu quyen 2006. Bam lai NPC de xem.")
end
