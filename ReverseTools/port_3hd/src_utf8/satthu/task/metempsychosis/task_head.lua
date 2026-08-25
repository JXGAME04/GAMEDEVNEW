-- ====================== 文件信息 ======================

-- 剑侠情缘网络版越南版 - 多次转生头文件
-- 文件名　：task_head.lua
-- 创建者　：子非魚
-- 创建时间：2009-02-04 16:21:20

-- ======================================================
--print(10e6)
IncludeLib("FILESYS")

TB_LEVEL_REMAIN_PROP = {
		--[等级level] = { {magicpoint, prop, resist}<,{}>};
			}

NTRANSLIFE_MAX = 7  -- 转生次数上限			

function Load_TransLifeSetting()
	local b1 = TabFile_Load("\\settings\\task\\metempsychosis\\translife.txt", "TransLifeSetting")
	if b1~=1 then
		print("Load TransLifeSetting Failed!")
		return
	end
	local nRowCount = TabFile_GetRowCount("TransLifeSetting", "LEVEL")
	
	for y = 2, nRowCount do
		local n_level = tonumber(TabFile_GetCell("TransLifeSetting", y, "LEVEL"));
		
		local tb = {};
		
		for z = 1, NTRANSLIFE_MAX do
			
			local n_magicpoint = tonumber(TabFile_GetCell("TransLifeSetting", y, "MAGICPOINT"..z));
			local n_prop = tonumber(TabFile_GetCell("TransLifeSetting", y, "PROP"..z));
			local n_resist = tonumber(TabFile_GetCell("TransLifeSetting", y, "RESIST"..z));
			local n_addskilll = tonumber(TabFile_GetCell("TransLifeSetting", y, "SKILLLIMIT"..z));
			
			if (n_magicpoint) then
				tb[z] = {n_magicpoint, n_prop, n_resist, n_addskilll};
			end
			
		end
		
		if (n_level ~= nil) then
			TB_LEVEL_REMAIN_PROP[n_level] = tb;
		end
	end	
end

Load_TransLifeSetting()

TB_LEVEL_LIMIT = {160, 170, 180, 200, 200, 200, 200};
TB_TRANSTIME_LIMIT = {0, 0, 0, 0, 0, 0}; --modified translife limit time by ThanhLD - 20150111
-- 4转需要的其他道具
TBITEMNEED_4 = {
	[1] = {szName = "Tích Lịch đơn", tbProb = {6,1,2973}, nCount = 999},	-- 霹雳弹
	[2] = {szName = "<Bắc Đẩu Trường Sinh Thuật—Đại Thừa Tâm Pháp>", tbProb = {6,1,2974}, nCount = 1},  -- 北斗长生术—大乘心法
	}

-- 五行人物的天生属性值
TB_BASE_STRG = {35,20,25,30,20};
TB_BASE_DEX = {25,35,25,20,15};
TB_BASE_VIT = {25,20,25,30,25};
TB_BASE_ENG = {15,25,25,20,40};

ZHUANSHENG_DESC		= "METEMPSYCHOSIS";
ZHUANSHENG_TUITION	= 100000000;	-- 转生所需JXB
ZHUANSHENG_XIANDAN_MINEXP	= 2*10e8;	-- 北斗仙丹至少要20亿
ZHUANSHENG_XIANDAN_BASEEXP		= 10e6;	-- 北斗仙丹单位是 1000w
ZHUANSHENG_ITEM_BEGIN	= 20090420---20090420;		-- 制作仙丹开始时间 0点
ZHUANSHENG_ITEM_ENDLE	= 20090503;		-- 制作仙丹结束时间 24点
ZHUANSHENG_ITEM_EXTIME	= 20090601;		-- 仙丹有效使用时间

LG_SHITULEAGUE = 1;				-- 师徒战队
LG_WLLSLEAGUE = 5;				-- 武林联赛战队

TSK_ZHUANSHENG_FLAG = 2547;
TSK_ZHUANSHENG_1 = 2548;	-- 转生 用来保存最大抗性的任务变量，从低字节开始依次保存火、冰、毒、电的最大抗性
TSK_ZHUANSHENG_2 = 2549;			-- 转生 用来保存最大抗性的任务变量，最低字节保存最大物理抗性，其余字节未使用
TSK_ZHUANSHENG_XIANDAN	= 2581;		-- 制作北斗仙丹
TSK_ZHUANSHENG_AWARD	= 2582;		-- byte1:3转前10名领取翻羽;1:未领,255:已领取


TSK_KILLER_ID = 1082;
TSK_MESSENGER_FENG = 1201;
TSK_MESSENGER_SHAN = 1202;
TSK_MESSENGER_QIAN = 1203;
TSK_TASKLINK_STATE = 1028;
TSK_TASKLINK_CancelTaskLevel = 2571;
TSK_TASKLINK_CancelTaskExp1 = 2570;
TSK_TASKLINK_CancelTaskExp2 = 2575;

TSK_ZHUANSHENG_GRE = {2577, 2578, 2579, 4107}	-- 每次转生所选等级和抗性
TSK_ZHUANSHENG_LASTTIME = 2580;			-- 保存最后次转生的ServerTime 以秒记（相对1700-0-0到目前的描述）
TSKM_ZHUANSHENG_RESISTID = 199;			-- 临时存放选择增加的抗性
--抗性编号，0：火，1：冰，2:毒，3:电，4:物理
TB_BASE_RESIST = {
	[0] = "Phòng lửa",
	[1] = "Phòng băng",
	[2] = "Phòng độc",
	[3] = "Phòng sấm",
	[4] = "Dự phòng",
	}

TSK_TRANSLIFE_4 = 2908		-- 保存是否接受了4转任务	
TSK_LEAVE_SKILL_POINT_4 = 2909		-- 保存剩余的4转技能的剩余技能点	
TSK_USED_SKILL_POINT_4 = 2899		-- 保存已经使用了多少技能点
TSK_LAST_UP_LEVEL_4 = 2910		-- 保存最后一次更新4转技能的剩余技能点的等级 
ZHUANSHENG_TUITION_4	= 100000000;	-- 第4次转生所需JXB
CLEAR_SKILL_4_PRICE = 10000000	-- 4转技能点洗点的价格
--TB_SKILL_4 = {{1123,0},{1124,0},{1125,0},{1126,0},{1127,0},{1128,0},{1129,0},{1130,0}}		-- 保存4转技能的相关id
--TB_SKILL_6 = {{}}
TB_SKILL_4 = {{1123, 10, "Vũ Uy Thuật"},{1124, 10, "Nhược Thủy Thuật"},{1125, 10, "Trấn Nhạc Thuật"},{1126, 10, "Yên Ba Thuật"},{1127, 0, "Trường Sinh Thuật"},{1128, 20, "Bột Hải Thuật"},{1129, 20, "Thần Tuệ Thuật"},{1130, 20, "Truy ảnh Thuật"}}
TB_SKILL_6 = {{1336, 10, "Vũ Uy Thuật (trùng sinh 6)"},{1337, 10, "Nhược Thủy Thuật (trùng sinh 6)"},{1338, 10, "Trấn Sơn Thuật (trùng sinh 6)"},{1339, 10, "Yên Ba Thuật (trùng sinh 6)"},{1340, 0, "Trường Sinh Thuật (trùng sinh 6)"},{1341, 20, "Bá Hải Thuật (trùng sinh 6)"},{1342, 20, "Thần Tuệ Thuật(trùng sinh 6)"},{1343, 20, "Truy Cảnh Thuật (trùng sinh 6)"}}

NSTARTLEVEL_4 = 105		-- 4转技能从105级开始获得技能点
NPERPOINTNEEDLEVEL = 5  -- 每5级获得一个技能点

BEIDOU_TRANSFER_MAX = 5

-- 5ìê
TSK_TRANSLIFE_5 = 4017		   -- 保存是否接受了5转任务
N_TRANSLIFE_TREE_MIN = 40   -- 5转种树要求

-- 5转需要的其他道具
TBITEMNEED_5 = {
	--[1] = {szName = "无极仙丹", tbProb = {6,1,3277}, nCount = 400},	
	--[2] = {szName = "黑龙仙丹", tbProb = {6,1,3278}, nCount = 50},	
	[1] = {szName = "Chuyển sinh thuật(cấp 5)", tbProb = {6,1,30246}, nCount = 2},  -- 北斗长生术—大乘心法
	}
	
TB_TRANSLIFE_ERRORMSG = {
	[1] = "<dec><npc>Tu luyện Bắc Đẩu Trường Sinh Thuật cần phải cởi bỏ tất cả  trang bị trên người!",
	[2] = "<dec><npc>Hình như tiền vẫn chưa đủ <color=red>100000000<color>.",
	[3] = "<dec><npc>Tu luyện Bắc Đẩu Trường Sinh Thuật cần phải bỏ quan hệ Sư Đồ",
	[4] = "<dec><npc>Vị thiếu hiệp này vẫn chưa học <Bắc Đẩu Trường Sinh Thuật - Cơ Sở Thiên>, vật này ở Kỳ Trân Các có bán.",
	[5] = "<dec><npc>Vị đại hiệp này, cấp độ của ngươi vẫn chưa đủ, ngươi hãy đi rèn luyện thêm đi.",
	[6] = "<dec><npc>Cần hủy bỏ quan hệ chiến hữu Võ Lâm Liên Đấu mới có thể tiến hành trùng sinh.",
	[7] = "<dec><npc><Bắc Đẩu Trường Sinh Thuật - Tâm Pháp Thiên> nhiều nhất chỉ có thể tu luyện 5 tầng, ngươi đã học đủ rồi.",
	[8] = "<dec><npc>Nhiệm vụ sát thủ vẫn chưa hoàn thành! Hãy hoàn tất nhiệm vụ rồi quay lại nhé.",
	[9] = "<dec><npc>Nhiệm vụ tín sứ vẫn chưa hoàn thành! Hãy hoàn tất nhiệm vụ rồi quay lại nhé.",
	[10]= "<dec><npc>Nhiệm vụ dã tẩu vẫn chưa hoàn thành! Hãy hoàn tất nhiệm vụ rồi quay lại nhé.",
	[11]= "<dec><npc>Dã tẩu thu được cơ hội hủy bỏ nhiệm vụ! Hãy hoàn tất nhiệm vụ này rồi quay lại nhé.",
	[12]= "<dec><npc>Khoảng cách 2 lần trùng sinh phải là %d ngày.",
	[13]= "<dec><npc>Chuyển sinh 4 cần có <color=red> 200 vạn <color> lượng, xin hãy chuẩn bị đủ rồi đến đây.",
	[14]= "<dec><npc>Chuyển sinh 4 cần có <color=red> 999 <color> cái Tích Lịch Đơn, xin hãy chuẩn bị đủ rồi đến đây.",
	[15]= "<dec><npc>Chuyển sinh 4 cần có <color=red> ôBắc Đẩu Trường Sinh Thuật—Đại Thừa Tâm Phápằ<color>, xin hãy chuẩn bị đủ rồi đến đây.",
	[16]= format("<dec><npc>Trùng sinh 5 cần có<color=red>%d<color> cái %s, Xin hãy chuẩn bị đủ rồi đến đây.", TBITEMNEED_5[1].nCount, TBITEMNEED_5[1].szName),
--	[17]= format("<dec><npc>Trùng sinh 5 cần có<color=red>%d<color> cái %s, Xin hãy chuẩn bị đủ rồi đến đây.", TBITEMNEED_5[2].nCount, TBITEMNEED_5[2].szName),
--  [18]= format("<dec><npc>Trùng sinh 5 cần có<color=red>%s<color>, Xin hãy chuẩn bị đủ rồi đến đây.", TBITEMNEED_5[3].szName),
	[17]= "",
	[18]= "",
	[19]= format("<dec><npc>Trùng sinh 5 cần phải trồng<color=red>%d<color>Cây Trùng Sinh, hãy chuẩn bị đủ rồi đến đây.", N_TRANSLIFE_TREE_MIN), 
	[20] = "<dec><npc>Vị đại hiệp này, điểm kinh nghiệm hiện tại của ngươi đã âm, ngươi hãy xử lý hoàn tất hết điểm điểm âm rồi đến đây.",
}