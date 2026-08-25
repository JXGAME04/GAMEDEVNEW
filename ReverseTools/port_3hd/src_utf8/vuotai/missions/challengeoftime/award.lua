Include("\\script\\missions\\challengeoftime\\include.lua")
Include("\\script\\misc\\eventsys\\type\\func.lua")
Include("\\script\\misc\\eventsys\\eventsys.lua")
Include("\\script\\vng_event\\thapnienlenhbai\\mainfuc.lua")

-- 随机奖励，随机基数为100000 奖励修改将仙草几率加到1级玄晶，黄金几率加到3级玄晶 福缘直接对应玄晶
map_random_awards = {
	100000,													-- 随机基数为100000
	{0.005, 		{"Tinh Hồng Bảo Thạch",		353}},					-- 猩红宝石
	{0.005, 		{"Lam Thủy Tinh", 			238}},					-- 兰水晶
	{0.005, 		{"Tử Thủy Tinh", 			239}},					-- 紫水晶
	{0.005, 		{"Lục Thủy Tinh", 			240}},					-- 绿水晶
	{0.0005, 	{"Võ Lâm Mật Tịch", 		6, 1, 26, 1, 0, 0}},	-- 武林秘籍
	{0.0005, 	{"Tẩy Tủy Kinh",			6, 1, 22, 1, 0, 0}},	-- 洗髓经
	{0.2, 		{"Phúc Duyên Lộ (Đại) ",	6, 1, 124, 1, 0, 0}},	-- 福缘露（大）
	{0.19587, 		{"Phúc Duyên Lộ (Trung) ", 	6, 1, 123, 1, 0, 0}},	-- 福缘露（中）
	{0.18, 		{"Phúc Duyên Lộ (Tiểu) ",	6, 1, 122, 1, 0, 0}},	-- 福缘露（小）
	{0.15, 	{"Tiên Thảo Lộ ",			6, 1, 71, 1, 0, 0}},	-- 仙草露
	{0.1, 		{"Thiên sơn  Bảo Lộ ",		6, 1, 72, 1, 0, 0}},	-- 天山玉露
	{0.1, 		{"Bách Quả Lộ ",			6, 1, 73, 1, 0, 0}},	-- 百果露
--	{0.00, 		{"大白驹丸",		6, 1, 130, 1, 0, 0}},	-- 大白驹丸
	{0.05, 		{"Lệnh bài Phong Lăng Độ",		489}},					-- 风陵渡令牌
	{0.0005, 	{"Định Quốc Thanh Sa Trường Sam",	0, 159}},				-- 定国之青纱长衫
	{0.0005, 	{"Định Quốc Ô Sa Phát Quán",	0, 160}},				-- 定国之钨砂发冠
	{0.0005,		{"Định Quốc Xích Quyên Nhuyễn Ngoa",	0, 161}},				-- 定国之赤绢软靴
	{0.0005,		{"Định Quốc Tử Đằng Hộ uyển",	0, 162}},				-- 定国之紫藤护腕
	{0.0005,		{"Định Quốc Ngân Tàm Yêu đái",	0, 163}},				-- 定国之银蚕腰带
	{0.0001,	{"An Bang Băng Tinh Thạch Hạng Liên",0, 164}},				-- 安邦之冰晶石项链
	{0.0001,	{"An Bang Cúc Hoa Thạch Chỉ hoàn",0, 165}},				-- 安邦之菊花石指环
	{0.0001,	{"An Bang Điền Hoàng Thạch Ngọc Bội",0, 166}},				-- 安邦之田黄石玉佩
	{0.0001,	{"An Bang Kê Huyết Thạch Giới Chỉ ",0, 167}},				-- 安邦之鸡血石戒指
	{0.00001, {"Mộng Long Kim Ti Chính Hồng Cà Sa",	0, 2}},				-- 梦龙之金丝正红袈裟
	{0.00001, {"Tứ Không Giáng Ma Giới đao",	0, 11}},				-- 四空之降魔戒刀
	{0.00001,	{"Phục Ma Tử Kim Côn",	0, 6}},				-- 伏魔之紫金棍
	{0.00001,	{"Kế Nghiệp Bôn Lôi Toàn Long thương",	0, 21}},				-- 继业之奔雷钻龙枪
	{0.00001,	{"Ngự Long Lượng Ngân Bảo đao",	0, 26}},				-- 御龙之亮银宝刀
	{0.00001,	{"Địa Phách Hắc Diệm Xung Thiên Liên",0, 87}},				-- 地魄之黑焰冲天链
	{0.00001,	{"Băng Hàn Đơn Chỉ Phi Đao",0, 71}},				-- 冰寒之弹指飞刀
	{0.00001,	{"Sâm Hoang Phi Tinh Đoạt Hồn",0, 81}},				-- 森荒之飞星夺魂
	{0.00001,	{"Thiên Quang Định Tâm Ngưng Thần Phù ",0, 77}},				-- 天光之定心凝神符
	{0.00001, {"Chú Phược Diệt Lôi Cảnh Phù ",	0, 67}},				-- 咒缚之灭雷颈符
	{0.00001, {"Minh ảo Tà Sát Độc Nhận",	0, 61}},				-- 冥幻之邪刹毒刃
	{0.00001,	{"Tê Hoàng Phụng Nghi đao",	0, 46}},				-- 栖凰之凤仪刀
	{0.00001,	{"Bích Hải Uyên Ương Liên Hoàn đao",	0, 51}},				-- 碧海之鸳鸯连环刀
	{0.00001,	{"Vô Ma Hồng Truy Nhuyễn Tháp hài",	0, 40}},				-- 无魇之红缁软禢鞋
	{0.00001,	{"Vô Ma Tẩy Tượng Ngọc Khấu ",0, 39}},				-- 无魇之洗象玉扣
	{0.00001,	{"Cập Phong Tam Thanh Phù ",0, 122}},				-- 及丰之三清符
	{0.00001,	{"Vụ ảo Bắc Minh Đạo quán",0, 136}},				-- 雾幻之北冥道冠
	{0.00001,	{"Sương Tinh Thiên Niên Hàn Thiết",0, 126}},				-- 霜晶之千年寒铁
	{0.00001,	{"Lôi Khung Thiên Địa Hộ phù ",	0, 132}},				-- 雷穹之天地护符
	{0.00001,	{"Ma Thị sơn  Hải Phi Hồng Lý ",	0, 115}},				-- 魔嗜之山海飞鸿履
	{0.00001,	{"Ma Hoàng án Xuất Hổ Hạng Khuyên",	0, 107}},				-- 魔煌之按出虎项圈
	{0.00001,	{"Đồng Cừu Kháng Long Hộ Uyển",0, 94}},				-- 同仇之亢龙护手
	{0.00001,	{"Địch Khái Lục Ngọc Trượng",0, 96}},				-- 敌忾之绿玉杖
}

-- 比赛通关奖励：五花、经验、物品
tbAward_Success = {
	[1] = {
		[1]=function(time) -- 比赛通关奖励经验函数
					local min = floor(time / 60);
					if (min >= 25) then
						return 15;
					else
						return floor(172 * (1 - min / 25)) + 10;
					end
			end,
		[2]=0,
		[3]=nil
	},-- Level 1
	[2] = {
		[1]=function(time) -- 比赛通关奖励经验函数
				local min = floor(time / 60);
				if (min >= 25) then
					return 30;
				else
					return floor(233 * (1 - min / 25)) + 20;
				end
			end,
		[2]=0,
		[3]=nil
	},-- Level 2
}

local tbAward_batch = 
{
	[15] = 2,
	[28] = 2,
}
local tbPro = {szName="Bảo Rương Vượt ải",tbProp={6, 1, 2742, 1, 0, 0},}
function award_batch_extend(batch)
	
	local tbPlayerList = GetMatchPlayerList()
	
	for i=1,getn(tbPlayerList) do
		local nPlayerIndex = tbPlayerList[i]
		
		--加帮会贡献度
		CallPlayerFunction(nPlayerIndex, award_batch_contribution, batch)
		-- 越南生日活动时间挑战赛的过关数
		CallPlayerFunction(nPlayerIndex, SetTask, tbBirthday0905.tbTask.tsk_toll_cg_passcount, batch)
		-- 闯关调整 by wangjingjun 2011.03.01
		if %tbAward_batch[batch] and GetMissionV(VARV_BATCH_MODEL) == 1 then
			%tbPro.nCount = %tbAward_batch[batch]
			CallPlayerFunction(nPlayerIndex, tbAwardTemplet.GiveAwardByList, tbAwardTemplet, %tbPro, "chuangguan award", 1)
		end
			-- 小聂弑尘奖励
		if GetMissionV(VARV_BATCH_MODEL) == 1 and batch == GetMissionV(VARV_XIAONIESHICHEN_BATCH) then
			%tbPro.nCount = 1
			CallPlayerFunction(nPlayerIndex, tbAwardTemplet.GiveAwardByList, tbAwardTemplet, %tbPro, "xiaonieshichen awrad", 1)
			
			local nExpCount = 10000000
			nExpCount = Chuangguan_checkdoubleexp(nExpCount)
			CallPlayerFunction(nPlayerIndex, tbAwardTemplet.GiveAwardByList, tbAwardTemplet, {nExp_tl=1,nCount = nExpCount,}, "xiaonieshichen awrad", 1)
		end
	end
	
end

-- 隐藏任务奖励
function award_hidden_mission()
	local tbPlayerList = GetMatchPlayerList()
	for i=1,getn(tbPlayerList) do
		local nPlayerIndex = tbPlayerList[i]
		CallPlayerFunction(nPlayerIndex, award_random_object, map_random_awards);
		-- 越南生日活动时间挑战赛的过关数
		CallPlayerFunction(nPlayerIndex, SetTask, tbBirthday0905.tbTask.tsk_toll_cg_passcount, 29);
	end
	local n_level = GetMatchLevel();
	EventSys:GetType("ChuanGuan"):OnEvent("OnPass", 29, tbPlayerList, n_level)
end

-- 奖励物品
function award_item(item)

	local name = item[1];
	if (getn(item) == 2) then
		AddEventItem(item[2]);
	elseif (getn(item) == 3) then
		AddGoldItem(item[2], item[3]);
	elseif (getn(item) == 7) then
		AddItem(item[2], item[3], item[4], item[5], item[6], item[7]);
	end
	Msg2Player("<#> Bạn nhận được " .. name .. "!");
	
end

-- 奖励随机物品
function award_random_object(objects)
	local base = objects[1];		-- 随机基数在第一个位置
	local sum = 0;
	local num = random(1, base);
	for i = 2, getn(objects) do		-- 随机物品从第二个位置开始
		local odds = objects[i][1];
		local item = objects[i][2];
		sum = sum + odds * base;
		if (num <= sum) then
			award_item(item)
			break;
		end
	end
end

function award_to_player_success()
	tbAwardTemplet:GiveAwardByList(tbExtItem, "finish challengeoftime")
	tbTimerLog:weiMing(tbExtItem[1].nPrestige)
end

-- 奖励玩家
function award_player(exp, objects, time)
	
	--Storm 增加积分
	if(GetMissionV(VARV_MISSION_RESULT) == 1) then	--最终奖励
		storm_addpoint(2, LIMIT_FINISH - time)
	end

	-- 奖励经验
	local experience = exp;
	if (type(exp) == "function") then
		experience = exp(time);
	end	
	if (experience ~= 0) then
		local point = experience * 10000;
		--给与120技能熟练度
		AddExp_Skill_Extend(point);
--		if(greatnight_huang_event(3) == 1) then
--		elseif(greatnight_huang_event(3) == 2) then
--			point = point * 2;
--		elseif(greatnight_huang_event(3) == 3) then
--			point = point * 3;
--		else
--		end;
		-- 是否队长
		local name = GetMissionS(VARS_TEAM_NAME);
		if (GetName() == name) then
			point = point * (1 + 0.2);	--奖励更多
		end;
		if (GetMatchLevel() == 2) then
			point = point * 2;
		end
		
		point = BigBoss:AddChuangGuanPoint(point);
		point = Chuangguan_checkdoubleexp(point)
		AddOwnExp(point);
		Msg2Player("<#> Bạn nhận được " .. point .. "điểm kinh nghiệm.");
	end
	
	-- 奖励随机物品
	if (objects ~= nil) then
		award_random_object(objects);
	end

end

-- 掉落物品
function drop_item(index, count)
	local x, y, world = GetNpcPos(index);
	if (count ~= 0) then
		for i = 1, count do
			-- 掉落五花
			DropItem(world, x, y, -1, 1, 2, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0);
		end
	end
	-- 掉落礼品盒
	if ( random(1,100) <= 5 ) then
		DropItem(world, x, y, -1, 6, 1, 1392, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	end
end

-- 奖励
function award_batch_item(item, npc_index, time)
	-- 给每个队员经验和奖励
	local tbPlayerList = GetMatchPlayerList()
	for i=1, getn(tbPlayerList) do
			CallPlayerFunction(tbPlayerList[i], award_player, item[1], item[3], time)
	end
	
	-- 掉落物品
	drop_item(npc_index, item[2]);
end

-- 批次奖励
--触发条件：当本关所有NPC死亡后，并给于所有玩家基本奖励后，将被调用。
--此时的PlayerIndex是打死最后一只本关怪的人 
--如果需要对所以玩家奖励时，必须使用GetNextPlayer的方式，一一获得。
function award_batch(batch, npc_index)
	local tbNpcList = GetNpcList();
	award_batch_item(tbNpcList[batch][2], npc_index, 0);

	local tbAllPlayer = GetMatchPlayerList()
	local n_level = GetMatchLevel();
	local nTime = GetMissionV(VARV_BOARD_TIMER)
	
	G_ACTIVITY:OnMessage("Chuanguan", batch, tbAllPlayer, n_level);
	EventSys:GetType("ChuanGuan"):OnEvent("OnPass", batch, tbAllPlayer, n_level, nTime)
	--Thập Niên Lệnh Bài hỗ trợ tân thủ - NgaVN
--	tbThapnienLenhbai:ChuanguanAward(nBatch, tbAllPlayer)
	--Đóng Phong Vân Lệnh Bài tổ đội hổ trợ tân thủ - Modified By NgaVN - 20140615
	--tbPVLB_PtSpprt:COTAward(batch, tbAllPlayer)
	
	-- LLG_ALLINONE_TODO_20070802
	award_batch_extend(batch)
end

--加帮会贡献度
function award_batch_contribution(batch)
	tongaward_challengeoutoftime(batch)
end


function GetMissionCompleteAward()
	local nLevel = GetMatchLevel()
	if not nLevel then
		return nil
	end
	return tbAward_Success[nLevel]
end

-- 通关奖励
function award_success(npc_index, time)
	local item = GetMissionCompleteAward()
	award_batch_item(item, npc_index, time)
end
