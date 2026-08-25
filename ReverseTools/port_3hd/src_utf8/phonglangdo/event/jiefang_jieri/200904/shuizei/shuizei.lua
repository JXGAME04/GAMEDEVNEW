Include("\\script\\event\\jiefang_jieri\\200904\\taskctrl.lua");
Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\activitysys\\g_activity.lua")
Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\misc\\eventsys\\eventsys.lua")
tbjf0904_shuizei_exp =
{
	[1]	= {nExp = 10, 	nRate = 0.3},
	[2]	= {nExp = 15, 	nRate = 0.4},
	[3]	= {nExp = 20, 	nRate = 0.2},
	[4]	= {nExp = 25, 	nRate = 0.05},
	[5]	= {nExp = 30, 	nRate = 0.05},
};


function join_shuizei_act()
	if (0 == jf0904_shuizei_IsActtime()) then
		Say("Không phải là trong thời gian hoạt động.", 0);
		return
	end
	
	jf0904_InitTaskctrl();
	
	if(GetTask(jf0904_TSK_shuizei_GetTask) ~= 0) then
		Say("Ngươi đã nhận nhiệm vụ tiêu diệt thủy tặc rồi.", 0);
		return
	end
	
	Say("Thuyền phu bờ Bắc: Dạo này Thủy Tặc thường hoành hành bá đạo, cướp bóc nhiều chiếc thuyền qua lại 2 bờ Phong Lăng Độ. Ngươi có muốn thế thiên hành đạo, tiêu diệt những tên cướp này không?", 2, "Ta đã cho chúng 1 bài học đích đáng./join_shuizei_act_cf", "Ta đang rất bận, khi khác sẽ quay lại./OnCancel");
end

function join_shuizei_act_cf()
	if (GetLevel() < 50 or GetExtPoint(0) < 1) then
		Say("Đại hiệp vẫn chưa đủ cấp 50 hoặc vẫn chưa nộp thẻ, vì vậy không thể tham gia hoạt động", 0);
		return
	end

	if (GetTask(jf0904_TSK_shuizei_FulfilTaskTime) <= 0) then
		Say("Hôm nay không thể nhận thêm nhiệm vụ Tiêu diệt thủy tặc nữa.", 0);
		return
	end
	
	Say("Nhận nhiệm vụ tiêu diệt thủy tặc thành công!", 0);
	SetTask(jf0904_TSK_shuizei_FulfilTaskTime, GetTask(jf0904_TSK_shuizei_FulfilTaskTime) - 1);
	SetTask(jf0904_TSK_shuizei_GetTask, 1);
end

function fullfill_shuizei_act()
	if (0 == jf0904_shuizei_IsActtime()) then
		Say("Không phải là trong thời gian hoạt động.", 0);
		return
	end
	
	if (GetTask(jf0904_TSK_shuizei_GetTask) ~= 1) then
		Say("Vẫn chưa nhận nhiệm vụ thủy tặc, vì vậy không thể trả nhiệm vụ được!", 0);
		return
	end
	
	give_shuizei_award_lingpai();
end


function give_shuizei_award_lingpai()
	GiveItemUI("Giao truy công lệnh", "Xin mời bỏ truy công lệnh vào ô bên dưới, mỗi lần chỉ được bỏ 1 cái.", "submit_zhuigongling", "no")
end

function submit_zhuigongling(nCount)
	if (nCount ~= 1) then
		Say("Bỏ vào số lượng không đúng!", 2, "Xin lỗi! Ta sẽ bỏ lại./give_shuizei_award_lingpai", "Ta sẽ quay lại sau/no")
		return 
	end
	local nItemIdx = GetGiveItemUnit(1);
	local ng, nd, np = GetItemProp(nItemIdx);
	if (ng == 6 and nd == 1 and np == 2015) then
		RemoveItemByIndex(nItemIdx);
		SetTask(jf0904_TSK_shuizei_GetTask, 0);
		Say("Thuyền phu: Chúc mừng đại hiệp! Đây là phần thưởng xứng đáng dành cho vị anh hùng.", 0);
		G_ACTIVITY:OnMessage("FLD_Collect_Item_VNG");
		EventSys:GetType("FengLingDu"):OnPlayerEvent("OnFinishShuiZeiTask", PlayerIndex)
		give_shuizei_award_exp();
	else
		Say("Vật phẩm bạn để vào không chính xác!", 2, "Xin lỗi! Ta sẽ bỏ lại./give_shuizei_award_lingpai", "Ta sẽ quay lại sau/no");
	end
end


function give_shuizei_award_exp()
	local nstep = random(1, 100000);
	local ncurstep 	= 0;
	local naddedexp = 0;
	for i = 1, getn(tbjf0904_shuizei_exp) do
		if (ncurstep + tbjf0904_shuizei_exp[i].nRate * 100000 >= nstep) then
			naddedexp = tbjf0904_shuizei_exp[i].nExp;
			break;
		end
		ncurstep = ncurstep + tbjf0904_shuizei_exp[i].nRate * 100000;
	end
	
	AddOwnExp(naddedexp * 1000000);
	
end

function jf0904_shuizei_IsActtime()
	local nDate = tonumber(GetLocalDate("%Y%m%d"));
	return (nDate >= jf0904_act_dateS);
end

function about_shuizei()
	Say("Lễ quan: Hoạt động tiêu diệt thủy tặc bắt đầu từ ngày 28/04/2009. Trong thời gian hoạt động đại hiệp có thể đến 6 điểm của bờ bắc nam Phong Lăng Độ để lĩnh thưởng.", 0);
end