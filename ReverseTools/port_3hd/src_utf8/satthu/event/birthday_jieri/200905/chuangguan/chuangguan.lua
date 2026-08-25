-- 越南资料片生日活动
Include("\\script\\event\\birthday_jieri\\200905\\class.lua");

function guoguan_xunbao()
	Say("Nhiếp Thí Trần: Từ 19/06/2009 đến 24:00 19/07/2009 đại hiệp có thể tiếp nhận nhiệm vụ “Quá quan tầm bảo” từ ta.Sau khi nhận nhiệm vụ, khi đại hiệp tham gia “Vượt ải cao cấp” sẽ có thể nhận được tín vật khai môn “Hùng Tâm Kiếm”, “Hùng Tâm Kiếm” nhận được này  có số lượng nhiều hay ít tùy thuộc vào việc đại hiệp có thể vượt qua được ải 15, 20, 25 hay 29.", 3,
		"Nhận nhiệm vụ “Quá quan tầm bảo”/gettask_chuangguan",
		"Ta đến trả nhiệm vụ “Quá quan tầm bảo”/fullfilltask_chuangguan",
		"Ta chỉ đến xem./OnCancel");
end

function gettask_chuangguan()
	if (GetLevel() < 90) then
		Say("Nhiếp Thí Trần: Đại hiệp chưa đủ điều kiện để nhận nhiệm vụ này.", 0);
		return
	end
	
	if (GetTask(tbBirthday0905.tbTask.tsk_toll_cg_state) == 1) then
		Say("Nhiếp Thí Trần: Đại hiệp đã nhận nhiệm vụ này rồi.", 0);
		return
	end
	
	Say("Nhiếp Thí Trần: Tốt quá, chúc đại hiệp tham gia vượt ải thành công.", 0);
	SetTask(tbBirthday0905.tbTask.tsk_toll_cg_state, 1);
	SetTask(tbBirthday0905.tbTask.tsk_toll_cg_passcount, 0);
end

function fullfilltask_chuangguan()
	if (GetTask(tbBirthday0905.tbTask.tsk_toll_cg_state) == 0) then
		Say("Nhiếp Thí Trần: Đại hiệp vẫn chưa nhận nhiệm vụ \"Quá quan tầm bảo\".", 0);
		return
	end
	
	if (CalcFreeItemCellCount() < 10) then
		Say("Nhiếp Thí Trần: Để bảo đảm sự an toàn cho vật phẩm, xin mời hãy để hành trang thừa 10 ô trống.", 0);
		return
	end
	
	local nToll = GetTask(tbBirthday0905.tbTask.tsk_toll_cg_passcount);
	if (nToll < 15) then
		Say("Nhiếp Thí Trần: Ngươi không thể hoàn thành nhiệm vụ này à?", 0);
		return
	end
	
	local nAwardCount = 0;
	if (nToll >= 29) then
		nAwardCount = 5;
	elseif (nToll >= 25) then
		nAwardCount = 3;
	elseif (nToll >= 20) then
		nAwardCount = 2;
	else
		nAwardCount = 1;
	end
	Say("Nhiếp Thí Trần: Tốt lắm, đây là phần thưởng xứng đáng dành cho đại hiệp!", 0);
	Msg2Player(format("Nhận được %d Hùng Tâm Kiếm.", nAwardCount));
	SetTask(tbBirthday0905.tbTask.tsk_toll_cg_state, 0);
	SetTask(tbBirthday0905.tbTask.tsk_toll_cg_passcount, 0);
	for i = 1, nAwardCount do
		AddItem(6, 1, 2070, 1, 0, 0);
	end
end

function OnCancel()

end