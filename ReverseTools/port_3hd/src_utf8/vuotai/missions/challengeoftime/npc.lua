-- description	: 创建NPC
-- author		: wangbin
-- datetime		: 2005-06-06

Include("\\script\\battles\\battlehead.lua")
Include("\\script\\missions\\challengeoftime\\include.lua")
Include("\\script\\missions\\basemission\\lib.lua")
Include("\\script\\activitysys\\config\\32\\talkdailytask.lua")

-- 怪物数量<=>位置文件
map_posfiles = {
	[8] = {"\\settings\\maps\\challengeoftime\\lineup8.txt", 1},	
	[16] = {"\\settings\\maps\\challengeoftime\\lineup16.txt", 1},	
	[20] = {"\\settings\\maps\\challengeoftime\\lineup20.txt", 1},
	[24] = {"\\settings\\maps\\challengeoftime\\lineup24.txt", 1},
	[32] = {"\\settings\\maps\\challengeoftime\\lineup32.txt", 1},
	[40] = {"\\settings\\maps\\challengeoftime\\lineup40.txt", 1},
	[56] = {"\\settings\\maps\\challengeoftime\\lineup56.txt", 1}
};

-- NPC的ID可以为三种类型：数字、表格、函数
-- NPC参数各列的含义：<NPC类型数量>, <奖励>, <后续处理、ID、名字、等级、五行、是否BOSS(0,1)、数量、位置>
-- 初级怪物

tbFightNpc = {
	[1] = {
		tbNormalNpc = {
			-- 1
			{1, {1.5, 0, nil},
				{nil, 975, "Sương Đao ", 75, -1, 0, 16, func_npc_getpos}},
			-- 2
			{1, {1.5, 0, nil},
				{nil, 976, "Phi Sa", 75, -1, 0, 16, func_npc_getpos}},
			-- 3
			{1, {3, 0, nil},
				{nil, 977, "Sương Liêm", 75, -1, 0, 20, func_npc_getpos}},
			-- 4
			{2,	{9, 0, nil},
				{nil, 975, "Sương Đao ", 75, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 75, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 5
			{1, {3, 0, nil},
				{nil, 978, "Thừa Phong", 75, -1, 0, 20, func_npc_getpos}},
			-- 6
			{1, {4.5, 0, nil},
				{nil, 979, "Thủy Hồn", 75, 1, 0, 24, func_npc_getpos}},
			-- 7
			{1, {4.5, 0, nil},
				{nil, 980, "Thần Tý ", 75, 3, 0, 24, func_npc_getpos}},
			-- 8
			{2,	{6, 10, nil},
				{nil, 976, "Phi Sa", 75, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 75, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 9
			{1, {5, 0, nil},
				{nil, 981, "Tranh Hán", 75, 2, 0, 32, func_npc_getpos}},
			-- 10
			{1, {5, 0, nil},
				{nil, 982, "Phá Lang", 75, 4, 0, 32, func_npc_getpos}},
			-- 11
			{2,	{9, 0, nil},
				{nil, 977, "Sương Liêm", 75, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 75, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 12
			{1, {6, 0, nil},
				{nil, 983, "ảnh Côn", 75, 0, 0, 40, func_npc_getpos}},
			-- 13
			{2,	{6, 10, nil},
				{nil, 978, "Thừa Phong", 75, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 75, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 14
			{1,	{4.5, 0, nil},
				{nil, 987, "Giang Trầm Yến", 75, -1, 0, 20, func_npc_getpos}},
			-- 15
			{1,	{6, 0, nil},
				{nil, 984, "Đao Tử ", 75, -1, 0, 32, func_npc_getpos}},
			-- 16
			{2,	{9, 0, nil},
				{nil, 979, "Thủy Hồn", 75, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 75, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 17
			{1,	{4.5, 0, nil},
				{nil, 988, "Cô Dự  Tẩu", 75, -1, 0, 20, func_npc_getpos}},
			-- 18
			{2,	{6, 10, nil},
				{nil, 980, "Thần Tý ", 75, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 75, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 19
			{1,	{4.5, 0, nil},
				{nil, 989, "Chưởng Bát Phương", 75, -1, 0, 20, func_npc_getpos}},
			-- 20
			{1,	{6, 0, nil},
				{nil, 985, "Lang bổng", 75, -1, 0, 24, func_npc_getpos}},
			-- 21
			{2,	{10.5, 0, nil},
				{nil, 981, "Tranh Hán", 75, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 75, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 22
			{1,	{4.5, 0, nil},
				{nil, 990, "Tiễn Than Thê ", 75, -1, 0, 20, func_npc_getpos}},
			-- 23
			{1,	{4.5, 0, nil},
				{nil, 991, "Nhậm Tông Hoành", 75, -1, 0, 20, func_npc_getpos}},
			-- 24
			{1,	{7.5, 30, nil},
				{nil, 992, "Đồ Tàn Sinh", 75, -1, 0, 20, func_npc_getpos}},
			-- 25
			{2,	{9, 0, nil},
				{nil, 984, "Đao Tử ", 75, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 75, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 26
			{1, {7.5, 0, nil},
				{nil, 986, "Hắc Cân", 75, -1, 0, 56, func_npc_getpos}},
			-- 27
			{2, {24, 15, nil},
				{nil, 1006, "Nhiếp Thí Trần", 75, -1, 0, 1, {1568, 3227}},
				{nil, 985, "Lang bổng", 75, -1, 0, 16, func_npc_getpos}},
			-- 28
			{1, {0, 0, nil},
				{nil, 993, func_npc_get_eachname, 75, -1, 1, 8, func_npc_getpos}},
		}, --tbNormalNpc
		
		tbHidenNpc = {
			-- 挑战赏金十大猎人排行榜玩家
			{1,	{0, 0, nil}, {nil, {994, 1001}, {func_ladder_getname, 10119}, 75, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 挑战初级时间的挑战排行榜玩家
			{1,	{0, 0, nil}, {nil, {994, 1001}, {func_ladder_getname, 10179}, 75, func_npc_getseries, 1, 1, {1568, 3227}}}
		},--tbHidenNpc
		
	}, -- [1]
	[2] = {
		tbNormalNpc = {
			-- 1
			{1, {2, 0, nil},
				{nil, 1007, "Sương Đao ", 95, -1, 0, 16, func_npc_getpos}},
			-- 2
			{1, {2, 0, nil},
				{nil, 1008, "Phi Sa", 95, -1, 0, 16, func_npc_getpos}},
			-- 3
			{1, {3, 0, nil},
				{nil, 1009, "Sương Liêm", 95, -1, 0, 20, func_npc_getpos}},
			-- 4
			{2, {9, 0, nil},
				{nil, 1007, "Sương Đao ", 95, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 95, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 5
			{1, {3, 0, nil},
				{nil, 1010, "Thừa Phong", 95, -1, 0, 20, func_npc_getpos}},
			-- 6
			{1, {5, 0, nil},
				{nil, 1011, "Thủy Hồn", 95, 1, 0, 24, func_npc_getpos}},
			-- 7
			{1, {5, 0, nil},
				{nil, 1012, "Thần Tý ", 95, 3, 0, 24, func_npc_getpos}},
			-- 8
			{2, {9, 15, nil},
				{nil, 1008, "Phi Sa", 95, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 95, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 9
			{1, {6, 0, nil},
				{nil, 1013, "Tranh Hán", 95, 2, 0, 32, func_npc_getpos}},
			-- 10
			{1, {6, 0, nil},
				{nil, 1014, "Phá Lang", 95, 4, 0, 32, func_npc_getpos}},
			-- 11
			{2, {12, 0, nil},
				{nil, 1009, "Sương Liêm", 95, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 95, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 12
			{1, {9, 0, nil},
				{nil, 1015, "ảnh Côn", 95, 0, 0, 40, func_npc_getpos}},
			-- 13
			{2, {9, 15, nil},
				{nil, 1010, "Thừa Phong", 95, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 95, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 14
			{1, {6, 0, nil},
				{nil, 1019, "Giang Trầm Yến", 95, -1, 0, 20, func_npc_getpos}},
			-- 15
			{1, {9, 0, nil},
				{nil, 1016, "Đao Tử ", 95, -1, 0, 32, func_npc_getpos}},
			-- 16
			{2, {14, 0, nil},
				{nil, 1011, "Thủy Hồn", 95, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 95, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 17
			{1, {6, 0, nil},
				{nil, 1020, "Cô Dự  Tẩu", 95, -1, 0, 20, func_npc_getpos}},
			-- 18
			{2, {12, 20, nil},
				{nil, 1012, "Thần Tý ", 95, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 95, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 19
			{1, {6, 0, nil},
				{nil, 1021, "Chưởng Bát Phương", 95, -1, 0, 20, func_npc_getpos}},
			-- 20
			{1, {8, 0, nil},
				{nil, 1017, "Lang bổng", 95, -1, 0, 24, func_npc_getpos}},
			-- 21
			{2, {14, 0, nil},
				{nil, 1013, "Tranh Hán", 95, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 95, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 22
			{1, {6, 0, nil},
				{nil, 1022, "Tiễn Than Thê ", 95, -1, 0, 20, func_npc_getpos}},
			-- 23
			{1, {6, 0, nil},
				{nil, 1023, "Nhậm Tông Hoành", 95, -1, 0, 20, func_npc_getpos}},
			-- 24
			{1, {10, 20, nil},
				{nil, 1024, "Đồ Tàn Sinh", 95, -1, 0, 20, func_npc_getpos}},
			-- 25
			{2, {15, 0, nil},
				{nil, 1016, "Đao Tử ", 95, func_npc_getseries, 0, 16, func_npc_getpos},
				{func_npc_proceed, func_npc_getid, func_npc_getname, 95, func_npc_getseries, 1, 1, {1568, 3227}}},
			-- 26
			{1, {9, 0, nil},
				{nil, 1018, "Hắc Cân", 95, -1, 0, 56, func_npc_getpos}},
			-- 27
			{2, {32, 32, nil},
				{nil, 1038, "Nhiếp Thí Trần", 95, -1, 1, 1, {1568, 3227}},
				{nil, 1017, "Lang bổng", 95, -1, 0, 16, func_npc_getpos}},
			-- 28
			{1, {0, 0, nil},
				{nil, 1025, func_npc_get_eachname, 75, -1, 1, 8, func_npc_getpos}},
		}, -- tbFightNpc

		tbHidenNpc = {
			-- 挑战赏金十大猎人排行榜玩家
			{1,	{0, 0, nil}, {nil, {1026, 1033}, {func_ladder_getname, 10119}, 75, -1, 1, 1, {1568, 3227}}},
			-- 挑战高级时间的挑战排行榜玩家
			{1,	{0, 0, nil}, {nil, {1026, 1033}, {func_ladder_getname, 10180}, 75, -1, 1, 1, {1568, 3227}}}
		} ,--tbHidenNpc
	}, --[2]
}

-- 小聂弑尘 和 新boss
map_new_Ncp = {
	[1] = {nil, 1673, "Tiểu Nhiếp Thí Trần", 95, -1, 1, 1, {1568, 3227}},
	[2] = { nNpcId = 1674, nLevel = 95,	bNoRevive = 1, szName = "Vượt ải_Cổ Bách",	nIsboss = 1,},
	[3] = { nNpcId = 1675, nLevel = 95,	bNoRevive = 1, szName = "Vượt ải_Huyền Giác Đại Sư",	nIsboss = 1,},
	[4] = { nNpcId = 1676, nLevel = 95,	bNoRevive = 1, szName = "Vượt ải_Đường Bất Nhiễm",	nIsboss = 1,},
	[5] = { nNpcId = 1677, nLevel = 95,	bNoRevive = 1, szName = "Vượt ải_Lam Y Y",	nIsboss = 1,},
	[6] = { nNpcId = 1678, nLevel = 95,	bNoRevive = 1, szName = "Vượt ải _Thanh Hiểu Sư Thái",	nIsboss = 1,},
	[7] = { nNpcId = 1679, nLevel = 95,	bNoRevive = 1, szName = "Vượt ải_Chung Linh Tú",	nIsboss = 1,},
	[8] = { nNpcId = 1680, nLevel = 95,	bNoRevive = 1, szName = "Vượt ải_Hà Nhân Ngả",	nIsboss = 1,},
	[9] = { nNpcId = 1681, nLevel = 95,	bNoRevive = 1, szName = "Vượt ải _Đoan Mộc Duệ",	nIsboss = 1,},
	[10] = { nNpcId = 1682, nLevel = 95,	bNoRevive = 1, szName = "Vượt ải_Đạo Thanh Chân Nhân",	nIsboss = 1,},
	[11] = { nNpcId = 1683, nLevel = 95,	bNoRevive = 1, szName = "Vượt ải _Toàn Cơ Tử",	nIsboss = 1,},
};

-- 新地图传送npc
map_transfer_npc = {
	[1] = { nNpcId = 1684,	bNoRevive = 1, szName = "Vượt ải_Người tiếp dẫn Mật Phòng", szScriptPath = "\\script\\missions\\challengeoftime\\npc\\transfer.lua"},
}

-- 随机奖励，随机基数为100000
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
};

-- 获取NPC的ID
function get_npc_id(item, index)
	local id = item[NPC_ATTRIDX_ID];
	local id_type = type(id);
	if (id_type == "function") then
		return id(item, index);
	elseif (id_type == "number") then
		return id;
	elseif (id_type == "table") then
		local pos = random(1, getn(id));
		return id[pos];
	end
end

-- 获取NPC的名字
function get_npc_name(item, index)
	local name = item[NPC_ATTRIDX_NAME];
	local name_type = type(name);

	if (name_type == "string") then
		return name;
	elseif (name_type == "function") then
		return name(item, index);
	elseif (name_type == "table") then
		local func = name[1];
		return func(item, index);
	else
		return nil;
	end
end

-- 获取NPC的级别
function get_npc_level(item, index)
	return item[NPC_ATTRIDX_LEVEL];
end

-- 获取NPC的五行
function get_npc_series(item, index)
	local series = item[NPC_ATTRIDX_SERIES];
	local series_type = type(series);
	if (series_type == "function") then
		return series(item, index);
	elseif (series_type == "number") then
		if (series < 0) then
			local pos = random(1, getn(map_series));
			return map_series[pos];
		else
			return series;
		end
	else
		return nil;
	end
end

-- NPC的后续处理（累加当前使用的玩家索引）
function npc_proceed(item)
	local proceed = item[NPC_ATTRIDX_PROCEED];
	local proceed_type = type(proceed);
	if (proceed_type == "function") then
		proceed(item);
	end
end

-- 获取NPC的数量
function get_npc_count(item)
	return item[NPC_ATTRIDX_COUNT];
end

-- 获取NPC的位置
function get_npc_pos(item, index)
	local pos = item[NPC_ATTRIDX_POSITION];
	local pos_type = type(pos);
	if (pos_type == "table") then
		return pos[1], pos[2];
	elseif (pos_type == "function") then
		return pos(item, index);
	else
		return nil;
	end
end

-- 是否BOSS
function get_npc_isboss(item)
	return item[NPC_ATTRIDX_ISBOSS];
end

-- 当前NPC设置表
function GetNpcList()
	local nLevel = GetMatchLevel()
	if nLevel > getn(tbFightNpc) then
		return {}
	end
	return tbFightNpc[nLevel].tbNormalNpc
end

-- 当前隐藏NPC设置表
function GetHidenNpcList()
	local nLevel = GetMatchLevel()
	if nLevel > getn(tbFightNpc) then
		return {}
	end
	return tbFightNpc[nLevel].tbHidenNpc
end

-- 获取批次数量
function get_batch_count()
	return getn(GetNpcList())
end

-- 根据设定创建NPC，返回创建的NPC的数量
function add_npc(item)
	local npc_count = get_npc_count(item);			-- NPC数量
	local res_count = 0;							-- 产生的NPC数量
	
	for index = 1, npc_count do
		local id = get_npc_id(item, index);			-- NPC的ID
		local level = get_npc_level(item, index);	-- NPC等级
		local isboss = get_npc_isboss(item, index);	-- 是否boss
		local series = get_npc_series(item, index);
		local name = get_npc_name(item, index);
		local px, py = get_npc_pos(item, index);
		npc_proceed(item);

		local npc_index = AddNpcEx(
			id,			-- ID
			level,		-- 等级
			series,		-- 五行
			SubWorld,	-- 地图
			px * 32,	-- X坐标
			py * 32,	-- Y坐标
			1,			-- 不重生
			name,		-- 名字
			isboss);	-- 是否BOSS
		if (npc_index ~= nil and npc_index > 0) then
			res_count = res_count + 1;
			-- 设置死亡脚本
			SetNpcDeathScript(npc_index, SCRIPT_NPC_DEATH);
		else
			-- DEBUG
			local msg = format("Failed to AddNpcEx(%d,%d,%d,%d,%d,%d,%d,%s,%d)!!!",
				id, level, series, SubWorld, px, py, 1, name, isboss);
			print(msg);
		end
	end
	
	return res_count;
end

function create_all_npc(npcs)
	local times = npcs[1];
	local count = 0;
	for i = 1, times do
		count = count + add_npc(npcs[i + 2]);
	end
	
	SetMissionV(VARV_NPC_COUNT, count);
	return count;
end

-- 在地图上生成NPC
function create_batch_npc(batch)
	local tbNpcList = GetNpcList()
	local npc = tbNpcList[batch]	-- 初级怪

	local npc_count = create_all_npc(npc)
	

	
	-- 闯关调整 by wangjingjun 2011.03.01
	if batch == GetMissionV(VARV_XIAONIESHICHEN_BATCH) and GetMissionV(VARV_BATCH_MODEL) == 1 then

		add_npc(map_new_Ncp[1])
	--	basemission_CallNpc(map_new_Ncp[1], SubWorldIdx2ID(SubWorld),nX, nY)
		SetMissionV(VARV_NPC_COUNT, npc_count + 1);
		WriteLog("Tiểu Nhiếp Thí Trần đã tham gia chiến cuộc!")
		Msg2MSAll(MISSION_MATCH, "Tiểu Nhiếp Thí Trần đã tham gia chiến cuộc!");
	end

	local msg = "Xuất hiện" .. npc_count .. "con quái!"
	Msg2MSAll(MISSION_MATCH, msg)
	
	-- 第11关刷出日常任务对话NPC
	if (batch == 11) then
		-- 日常任务刷新对话NPC
		%tbTalkDailyTask:AddTalkNpc(SubWorldIdx2ID(SubWorld), SubWorldIdx2ID(SubWorld));
	end
end


-- 增加闯关传送npc
function add_transfer_npc()
	local nX,nY, nMapIndex = GetPos()
	basemission_CallNpc(map_transfer_npc[1], SubWorldIdx2ID(nMapIndex),1568*32, 3227*32)
end
