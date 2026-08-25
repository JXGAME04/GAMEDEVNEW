--====活动自定义部分,一般以GameID作为Table下标====--

STORM_GAMEID_MAX	= 4	--可能存在的GameID最大值

TB_STORM_POINT = {}	--积分表

TB_STORM_POINT[1] = {	--宋金积分表
	{	--固定夺旗模式
		{60, 16000},
		{50, 10000},
		{40, 5500},
		{30, 4000},
		{20, 2000},
		{10, 500},
		{0, 0}
	}, {--随机夺旗模式
		{60, 15000},
		{50, 10000},
		{40, 4500},
		{30, 3000},
		{20, 1500},
		{10, 300},
		{0, 0}
	}, {--元帅模式
		{60, 14500},
		{50, 10000},
		{40, 4500},
		{30, 2000},
		{20, 500},
		{10, 200},
		{0, 0}
	}, {--杀戮模式
		{60, 14000},
		{50, 9000},
		{40, 4000},
		{30, 1500},
		{20, 300},
		{10, 50},
		{0, 0}
	}
}

TB_STORM_POINT[2] = {	--杀手试炼积分表
	{60, 1350},
	{59, 1200},
	{50, 950},
	{40, 700},
	{30, 400},
	{20, 200},
	{10, 0},
}

TB_STORM_POINT[3] = {	--信使任务积分表
	{60, 500},
	{50, 400},
	{40, 300},
	{30, 200},
	{20, 100},
	{10, 30},
	{0, 0}
}

TB_STORM_POINT[4] = {	--野叟任务积分表
	{60, 30000000},
	{55, 18000000},
	{50, 12000000},
	{40, 6000000},
	{30, 3000000},
	{20, 500000},
	{10, 300000},
	{0, 0}
}

--各种活动的文字描述
TB_STORM_DESCRIBE = {"Chiến trường Tống Kim", "sát thủ ", "Nhiệm vụ Tín Sứ ", "Nhiệm vụ Dã Tẩu"}

--各种活动当日可以进行的次数
TB_STORM_TRYTIMES = {1, 2, 1, 1}

--开始挑战后的对话
TB_STORM_DIALOG = {
	"  Sau khi khiêu chiến, người nhận điểm tích lũy Tống Kim lần này sẽ được đổi thành điểm tích lũy Phong Ba. Điểm tích lũy Tống Kim của bạn hiện tại là: <color=yellow>%d<color>",
	"Không truy hỏi nhiệm vụ sát thủ ",
	"Nhiệm vụ Tín Sứ chưa bắt đầu",
	"  Sau khi khiêu chiến, hệ thống sẽ căn cứ trong một giờ, số lượng hoàn thành nhiệm vụ và độ khó chuyển đổi thành tích lũy Phong Ba hôm nay."
}

--帮助Npc的文字
TB_STORM_HELP = {
	"  Sau khi chiến trường Tống Kim bắt đầu từ phút phút thứ 10, 20, 30, bạn sẽ nhận được khiêu chiến Phong Ba qua các giai đoạn <color=red> tham gia nhận thưởng <color>___tích lũy Phong Ba, tổng cộng <color=yellow>40<color> phút. Nhận được <color=red>tích lũy Tống Kim <color>cũng có thể đổi ra tích lũy Phong Ba (Cao nhất<color=yellow>60<color>điểm) .%s<enter>",
	"  Tham gia nhiệm vụ sát thủ có thể nhận được <color=red>phần thưởng tham gia <color>___tích lũy Phong Ba <color=yellow>40<color> điểm. Thành công xông ải <color=red>trước thời gian<color>, có thể đổi thành tích lũy Phong Ba (Cao nhất <color=yellow>60<color> điểm) . Mỗi ngày hai trận sát thủ, lấy trận <color=red>thành tích tốt nhất<color> tính tích lũy Phong Ba. Sát thủ cao cấp mỗi người mỗi ngày chỉ nhận được cao nhất <color=yellow>100<color> điểm tích lũy Phong Ba, sát thủ sơ cấp cao nhất nhận được <color=yellow>90<color> điểm.",
	"Nhiệm vụ Tín Sứ ",
	"  Bắt đầu khiêu chiến Phong Ba lập tức nhận được <color=red>phần thưởng tham gia <color>___Tích lũy Phong Ba <color=yellow>40<color> điểm. Đồng thời căn cứ theo bạn trong <color=red>vòng một giờ <color> (Thời gian trên mạng, thời gian rời mạng vẫn tăng kinh nghiệm) , hoàn thành nhiệm vụ <color=red>số lượng và độ khó<color>, có thể nhận được tích lũy Phong Ba tương ứng (Cao nhất <color=yellow>60<color> điểm) . Trong hệ thống chuỗi nhiệm vụ Dã Tẩu, mỗi người mỗi ngày nhận được tích lũy Phong Ba cao nhất <color=yellow>100<color>điểm.<enter>"
}

--====Task Value====--
STORM_TASKID_BASE	= 1661-1	--TaskID起始值
--各个活动的可用Task变量空间，暂定每个活动需要7个Task变量
TB_STORM_TASKID = {
	{STORM_TASKID_BASE + 21, STORM_TASKID_BASE + 22, STORM_TASKID_BASE + 23, STORM_TASKID_BASE + 24, STORM_TASKID_BASE + 25, STORM_TASKID_BASE + 26, STORM_TASKID_BASE + 45},
	{STORM_TASKID_BASE + 27, STORM_TASKID_BASE + 28, STORM_TASKID_BASE + 29, STORM_TASKID_BASE + 30, STORM_TASKID_BASE + 31, STORM_TASKID_BASE + 32, STORM_TASKID_BASE + 46},
	nil,	--信使任务暂停	{STORM_TASKID_BASE + 33, STORM_TASKID_BASE + 34, STORM_TASKID_BASE + 35, STORM_TASKID_BASE + 36, STORM_TASKID_BASE + 37, STORM_TASKID_BASE + 38},
	{STORM_TASKID_BASE + 39, STORM_TASKID_BASE + 40, STORM_TASKID_BASE + 41, STORM_TASKID_BASE + 42, STORM_TASKID_BASE + 43, STORM_TASKID_BASE + 44, STORM_TASKID_BASE + 47}
}

--====时间/场次判断函数====--
--根据不同gameid，返回相应的nowtime
function storm_gettime(gameid)
	if (gameid == 1) then
		return BT_GetGameData(GAME_KEY)
	elseif (gameid == 2) then
		return GetTask(STORM_TASKID_GAMEID_SS)
	elseif (gameid == 3) then
		return GetGameTime()
	elseif (gameid == 4) then
		return 1
	else
		print("error gameid!")
		return nil
	end
end

--比较 nowtime 与 starttime ，判断是否还在游戏时间内
--返回1表示还可以记录积分，返回nil表示已经超时
function storm_gametime(gameid, starttime, nowtime)
	if (gameid == 1) then
		return starttime == nowtime	--同一场宋金大战
	elseif (gameid == 2) then
		return starttime == nowtime	--同一场杀手试炼
	elseif (gameid == 3) then
		return nowtime > starttime - 3600	--倒计时没有超过1小时
	elseif (gameid == 4) then
		--return nowtime < starttime + 3600	--系统时间没有超过1小时
		return TM_GetRestCount(STORM_TIMERID_YS) ~= nil	--计时器还存在
	else
		print("error gameid!")
		return nil
	end
end

--====奖励部分====--
--奖品格式：{{奖品类型，具体物品，数量}，{奖品类型，具体物品，数量}，......}
--	奖品类型：	1、AddItem；2、AddGoldItem；3、非物品；4、随机物品
--	具体物品：	因奖品类型的不同而有不同意义和格式
--		AddItem：		含6个参数的数组，表示AddItem(x1,x2,x3,x4,x5,x6)的6个参数
--		AddGoldItem：	含2个参数的数组，表示AddGoldItem(x1,x2)的2个参数
--		非物品：		1、AddOwnExp
--		随机物品：		格式同奖品格式，几率平均
--	数量：	如果是AddOwnExp，则表示是经验值

--日风暴积分奖励
TB_STORM_AWARD_DAY = {
	--需要积分	奖励名称	奖品
	{300,	"1 viên Huyền Tinh cấp 10 + 3000w kinh nghiệm",	{{1,{6,1,147,10,0,0},1},{3,1,30000000}}},
	{270,	"1 Thần Bí Khoáng thạch + 1200w kinh nghiệm",	{{1,{6,1,398,0,0,0},1},	{3,1,12000000}}},
	{240,	"1 viên Huyền Tinh cấp 6 + 800w kinh nghiệm",		{{1,{6,1,147,6,0,0},1},	{3,1,8000000}}},
	{210,	"Một hồng bao thần bí + 500w kinh nghiệm",	{{1,{6,1,402,0,0,0},1},	{3,1,5000000}}},
	{180,	"1 viên Huyền Tinh cấp 5 + 300w kinh nghiệm",		{{1,{6,1,147,5,0,0},1},	{3,1,3000000}}},
	{150,	"Ba Tiên Thảo lộ + 150 kinh nghiệm",		{{1,{6,1,71,0,0,0},3},	{3,1,1500000}}},
	{120,	"Hai Tiên Thảo lộ + 100w kinh nghiệm",		{{1,{6,1,71,0,0,0},2},	{3,1,1000000}}},
	{80,	"1 Tiên Thảo lộ + 50w kinh nghiệm ",		{{1,{6,1,71,0,0,0},1},	{3,1,500000}}}
}

--临时数组：一套安邦
tmp_ab = {
	{2,{0,164},1},
	{2,{0,165},1},
	{2,{0,166},1},
	{2,{0,167},1}
}
--周风暴排名奖励
TB_STORM_AWARD_WEEK = {
	--需要排名（负数）	奖励名称	奖品
	{-1,	"Một bộ An Bang", 	tmp_ab},
	{-10,	"Nhận được một bộ An Bang", {{4,tmp_ab,1}}}
}

--临时数组：技能+1大黄金
tmp_jydhj = {
	{2,{0,4},1},	{2,{0,9},1},	{2,{0,15},1},	{2,{0,19},1},	{2,{0,24},1},
	{2,{0,30},1},	{2,{0,35},1},	{2,{0,38},1},	{2,{0,43},1},	{2,{0,50},1},
	{2,{0,55},1},	{2,{0,59},1},	{2,{0,63},1},	{2,{0,68},1},	{2,{0,74},1},
	{2,{0,80},1},	{2,{0,84},1},	{2,{0,88},1},	{2,{0,95},1},	{2,{0,100},1},
	{2,{0,105},1},	{2,{0,109},1},	{2,{0,113},1},	{2,{0,120},1},	{2,{0,125},1},
	{2,{0,130},1},	{2,{0,135},1},	{2,{0,138},1}
}
--临时数组：大黄金
tmp_dhj = {
	{2,{0,2},1},	{2,{0,6},1},	{2,{0,11},1},	{2,{0,16},1},	{2,{0,21},1},
	{2,{0,26},1},	{2,{0,31},1},	{2,{0,39},1},	{2,{0,42},1},	{2,{0,46},1},
	{2,{0,51},1},	{2,{0,60},1},	{2,{0,61},1},	{2,{0,67},1},	{2,{0,71},1},
	{2,{0,76},1},	{2,{0,81},1},	{2,{0,87},1},	{2,{0,92},1},	{2,{0,96},1},
	{2,{0,101},1},	{2,{0,106},1},	{2,{0,115},1},	{2,{0,119},1},	{2,{0,121},1},
	{2,{0,126},1},	{2,{0,132},1},	{2,{0,136},1}
}
--月风暴排名奖励
TB_STORM_AWARD_MONTH = {
	--需要排名（负数）	奖励名称	奖品
	{-1,	"Nhẫn Đại Mạ Hổ một cặp",		{{2,{0,197},1},	{2,{0,202},1}}},
	{-1,	"Kỹ năng ngẫu nhiêu + 1 Hoàng Kim",	{{4,tmp_jydhj,1}}},
	{-10,	"Ngẫu nhiên một Hoàng Kim",		{{4,tmp_dhj,1}}}
}

--奖励总表
TB_STORM_AWARD = {
	--1文字描述	2奖励方式	3是否领取过的TaskID		4积分TaskID/排名LadderID	5奖励表
	{"Hôm nay",	0,		STORM_TASKID_DAY_AWARD,		STORM_TASKID_DAY_POINT,		TB_STORM_AWARD_DAY},
	{"Tuần trước",	1,		STORM_TASKID_WEEK_AWARD,	2,							TB_STORM_AWARD_WEEK},
	{"Tháng trước",	1,		STORM_TASKID_MONTH_AWARD,	3,							TB_STORM_AWARD_MONTH},
	{"hôm qua",	0,		STORM_TASKID_DAY_LASTAWARD,	STORM_TASKID_DAY_LASTPOINT,	TB_STORM_AWARD_DAY}
	--奖励方式：	0、按积分；1、按排名
	--排名LadderID：暂时用2、3分别代表上周、上个月排名
}

