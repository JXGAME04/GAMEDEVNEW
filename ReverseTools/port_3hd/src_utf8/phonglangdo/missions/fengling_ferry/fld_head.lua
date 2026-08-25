--AddEventItem(489)风陵渡领牌
Include("\\script\\lib\\file.lua")
Include("\\script\\lib\\log.lua")
Include("\\script\\lib\\awardtemplet.lua")
--Meridan material - Modified by DinhHQ - 20120717
Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\missions\\fengling_ferry\\lang.lua")
Include("\\script\\misc\\eventsys\\type\\func.lua")

MISSIONID = 15			--未定
FRAME2TIME = 18;		--18帧游戏时间相当于1秒钟
boatMAPS = {337, 338, 339};		--南岸渡船地图依顺序为，337南岸上游、338中游、339下游
boatMAP_POS = {1646, 3233}
northMAP = 336
northMAP_POS = {{1158, 2964}, {1343, 2868}, {1482, 2796}}
TNPC_THIEF = {724, 725}
TNPC_THIEF_COUNT = 30
npcthiefpos = "\\settings\\maps\\中原北区\\渡船\\渡船刷怪点.txt"
FLD_TIMER_1 = 20 * FRAME2TIME	--每20秒公布一下战况
FLD_TIMER_2 = 39 * 60 * FRAME2TIME		--从报名到进入打宝地图40分钟
ENDSIGN_TIME = 10 * 60 * FRAME2TIME/FLD_TIMER_1		--报名时间结束
UPBOSS_TIME = 25 * 60 * FRAME2TIME/FLD_TIMER_1		--开打15分钟时产生第1个BOSS
UPBOSS_TIME2 = 30 * 60 * FRAME2TIME/FLD_TIMER_1		--开打20分钟时产生第2个BOSS
UPBOSS_TIME3 = 35 * 60 * FRAME2TIME/FLD_TIMER_1		--开打25分钟时产生第3个BOSS
REPORT_TIME = 38 * 60 * FRAME2TIME/FLD_TIMER_1 
HUOYUEDU_TIME = 3 * 60 * FRAME2TIME/FLD_TIMER_1 -- 活跃度获得时间，船开启后的3分钟

MS_STATE = 1
MS_TIMEACC_1MIN = 2
MS_TIMEACC_20SEC = 3




function fld_cancel()
end

function fld_wanttakeboat(addr)
	-- Gia nhập môn phái mới lên thuyền Modified - by AnhHH - 20110724
	if (GetLastFactionNumber() == -1)then
		Talk(1,"",MSG_NOFACTION_LIMIT)
		return
	end
	
	local orgworld = SubWorld
	local MapId = boatMAPS[addr]
	if (MapId <= 0) then
		print("error:fenglingdu script wrong! mapid is nil!")
		return
	end
	local idx = SubWorldID2Idx(MapId)		
	if (idx < 0) then
		Say("Xin lỗi! Phía trước đang có nguy hiểm! Tạm thời chưa thể lên thuyền!.",0)
		return
	end
	if (fld_haveroom() == 1) then
		return
	end
	local sz_msg = "Muốn đón thuyền đến bờ Bắc Phong Lăng Độ phải có Phong Lăng Độ lệnh bài hoặc ngươi đưa ta <color=red>200<color> cuốn Mật đồ thần bí, ta sẽ cho ngươi lên thuyền!";
	local str = {	
		"Ta có lệnh bài Phong Lăng Độ/use_lingpai",
		"Ta đã thu thập đủ 200 cuốn Mật đồ thần bí/use_juanzhou",
		"Để ta suy nghĩ lại!/fld_cancel",
			};
	--Điều chỉnh thời gian phong lăng độ tốn phí - Modified by DinhHQ - 20110504
	if (check_new_shuizeitask() == 1) then
		sz_msg = format("Mỗi ngày vào lúc 10:00,14:00,16:00,18:00,20:00,cần phải có %s mới có thể đi tham gia Bờ Bắc Phong Lăng Độ, sau khi thuận lợi vượt qua sẽ có phần thưởng", "Lệnh Bài Thủy Tặc");
		str = {	
		format("Ta có %s/use_suizeilingpai", "Lệnh Bài Thủy Tặc"),
		"Để ta suy nghĩ lại!/fld_cancel",
			};
	end
	
	if (addr == 1) then
		Say(" "..sz_msg, getn(str), str);
	elseif (addr == 2) then
		Say(" "..sz_msg, getn(str), str);
	elseif (addr == 3) then
		Say(" "..sz_msg, getn(str), str);
	end
end



function fld_TakeBoat(plindex)
	orgplayerindex = PlayerIndex
	PlayerIndex = plindex

	local orgworld = SubWorld
	if ( BOATID == 1 ) then
		boatmapid = 337
		idx = SubWorldID2Idx(boatmapid)
	elseif( BOATID == 2 ) then
		boatmapid = 338
		idx = SubWorldID2Idx(boatmapid)
	elseif ( BOATID == 3 ) then
		boatmapid = 339
		idx = SubWorldID2Idx(boatmapid)
	else
		return 0
	end

	oldsubworldindex = SubWorld
	SubWorld = SubWorldID2Idx(boatmapid)
	if (fld_haveroom() == 1) then
		return 0
	end
	t = 10 - GetMissionV(MS_TIMEACC_1MIN)
	if (t <= 0) then
		return 0
	end
	LeaveTeam()
	--DinhHQ
	--20110405: Fix bug, ngoài thời gian 13h, 15h, 17h, 19h bến 2 3 có thể pk cừu sát
	if (check_new_shuizeitask() == 1) then
		if ( BOATID ~= 1 ) then
			SetTaskTemp(200,1);
			ForbidEnmity(1);			
		end	
		--Meridan material - Modified by DinhHQ - 20120711
		if CalcFreeItemCellCount() >= 1 and PlayerFunLib:CheckTaskDaily(3070, 5, "nomsg", "<") == 1 then
			PlayerFunLib:AddTaskDaily(3070, 1)
			local tbMeridanAward = {szName="Chân Nguyên Đơn (trung)",tbProp={6,1,30228,1,0,0},nCount=2,nBindState=-2}
			tbAwardTemplet:Give(tbMeridanAward, 1, {"KinhMach", "DangKyPLDTonPhiThanhCong"})
		end
	end
	SetCurCamp(1);	
--	if ( BOATID ~= 1 ) then
--		ForbidEnmity(1);
--		SetCurCamp(1);
--	end
	
--	SetTaskTemp(200,1);
	SetFightState(0)
	posx, posy = fld_getadata(npcthiefpos)
	posx = floor(posx/32)
	posy = floor(posy/32)
	AddMSPlayer(MISSIONID,1)
	NewWorld(boatmapid, posx, posy)
	EventSys:GetType("FengLingDu"):OnPlayerEvent("OnBoard", PlayerIndex)
	Msg2Player("còn"..t.." phút thuyền rời bến, đến bờ Bắc Phong Lăng Độ")
	DisabledUseTownP(1)	--限制其在渡船内使用回城符
	SetRevPos(175,1);		--设置重生点在西山村
	SetLogoutRV(1)
	SetCreateTeam(0);
	SetDeathScript("\\script\\missions\\fengling_ferry\\fld_death.lua")
	SubWorld = oldsubworldindex
	PlayerIndex = orgplayerindex
	return 1
end

function fld_haveroom()
	if (GetMSPlayerCount(MISSIONID, 1) >= 100 ) then
		if (BOATID == 1) then
			Say("Ngươi đến trễ rồi! Thuyền đã đầy rồi, hãy chờ chuyến sau đi!", 0)
			return 1
		elseif (BOATID == 2) then
			Say("Ngươi đã đến trễ! Thuyền đã đầy rồi, hãy chờ chuyến sau đi!", 0)
			return 1
		elseif (BOATID == 3) then
			Say("Ngươi đã đến trễ! Thuyền đã đầy rồi, hãy chờ chuyến sau đi!", 0)
			return 1
		end
	end
end

function fld_landingpos(posation)
	if (posation <= 0 and posation >3) then
		print("error: i still not know why!")
		return
	end
	return northMAP, northMAP_POS[posation][1], northMAP_POS[posation][2]
end

function fld_getadata(file)
	local totalcount = GetTabFileHeight(file);
	id = random(totalcount);
	x = GetTabFileData(file, id + 1, 1);
	y = GetTabFileData(file, id + 1, 2);
	return x,y
end

-- 神秘卷轴交付界面
function	use_juanzhou()	--使用神秘卷轴
	GiveItemUI( "Giao Mật đồ thần bí ", "Đặt 200 cuốn Mật đồ thần bí vào ô phía dưới, bạn phải chú ý, nếu Mật đồ thần bí trong ô ít hơn hoặc nhiều hơn 200 cuốn ta sẽ không nhận.", "exchange_juanzhou", "no" );
end;

-- 确定神秘卷轴数量函数
function exchange_juanzhou(ncount)
	local scrollarray = {}
	local scrollcount = 0
	local scrollidx = {}
	local y = 0
	for i=1, ncount do
		local nItemIdx = GetGiveItemUnit(i);
		itemgenre, detailtype, parttype = GetItemProp(nItemIdx)
		if (itemgenre == 6 and detailtype == 1 and parttype ==196) then	
			y = y + 1
			scrollidx[y] = nItemIdx;
			scrollarray[i] = GetItemStackCount(nItemIdx)
			scrollcount = scrollcount + scrollarray[i]
		end
	end
	if (y ~= ncount) then
		Say("Mật đồ thần bí hình như không đúng! Hãy kiểm tra lại xem.", 2, "à! Thì ra đặt nhầm để ta thử lại./use_juanzhou", "Để ta kiểm tra xem sao/no")
		return
	end
	if (scrollcount > 200) then
		Say("Ta chỉ cần 200 cuốn Mật đồ thần bí, còn lại ngươi đem về đi!", 2, "à! Thì ra đặt nhầm để ta thử lại./use_juanzhou", "Để ta kiểm tra xem sao/no")
		return
	end
	if (scrollcount < 200) then
		Say("Mật đồ thần bí chưa đủ! Hãy thử lại xem!", 2, "à! Thì ra đặt nhầm để ta thử lại./use_juanzhou", "Để ta kiểm tra xem sao/no")
		return
	end
	if (scrollcount == 200) then
		if (fld_TakeBoat(PlayerIndex) ~= 1) then
			Say("Thời gian không đợi ai cả! Thuyền đã đi rồi, lần sau ngươi hãy quay lại!", 0)
			return
		end
		for i = 1, y do
			RemoveItemByIndex(scrollidx[i])
		end
	end;		
end;


-- 风陵渡令牌交付界面
function	use_lingpai()	--使用风陵渡令牌
	GiveItemUI( format("Giao diện giao phí %s Lệnh Bài", "Lệnh bài Phong Lăng Độ"), format("Dùng 1 cái %s đặt vào ô trống phía dưới. N?u ngươi lấy những thứ rác rưởi khác đặt vào, ta sẽ không thèm nhận", "Lệnh bài Phong Lăng Độ"), "exchange_lingpai_1", "no" );
end;

function use_suizeilingpai()
--Modified By DinhHQ - 20110930
	GiveItemUI( format("Giao diện giao phí %s Lệnh Bài", "Lệnh Bài Thủy Tặc"), format("Dùng 1 cái %s đặt vào ô trống phía dưới. N?u ngươi lấy những thứ rác rưởi khác đặt vào, ta sẽ không thèm nhận", "Lệnh Bài Thủy Tặc"), "exchange_lingpai_2", "no", 1 );
end

function exchange_lingpai_1(ncount)
	exchange_lingpai(ncount, 1)
end

function exchange_lingpai_2(ncount)
	exchange_lingpai(ncount, 2)
end

-- 确定
function exchange_lingpai(ncount, ntype)
	if (ncount == 0) then
		Say("Hừ! Không có lệnh bài mà muốn ngồi thuyền?", 0)
		return
	end
	
	if (ncount > 1) then
		Say("Không phải ta bảo ngươi đừng đặt lung tung hay sao?", 0)
		return
	end

	local nItemIdx = GetGiveItemUnit(1);
	local nStackCount = GetItemStackCount(nItemIdx);
	
	if (nStackCount > 1) then
		Say("Thuyền Phu Bờ Nam:  Ta chỉ cần một cái Lệnh Bài, những thứ khác ta không cần", 0)
		return
	end
	
	local itemgenre, detailtype, particular = GetItemProp(nItemIdx);
	
	if (ntype ==1) then
		if (itemgenre ~= 4 or detailtype ~= 489) then
			Say(format("Thuyền Phu Bờ Nam:  Ta chỉ cần một cái Lệnh Bài, những thứ khác ta không cần", "Lệnh bài Phong Lăng Độ"), 2, "à! Thì ra đặt nhầm để ta thử lại./use_lingpai", "Để ta kiểm tra xem sao/no")
			return
		end
	else
		if (itemgenre ~= 6 or particular ~= 2745) then
			Say(format("Thuyền Phu Bờ Nam:  Ta chỉ cần một cái Lệnh Bài, những thứ khác ta không cần", "Lệnh Bài Thủy Tặc"), 2, "à! Thì ra đặt nhầm để ta thử lại./use_suizeilingpai", "Để ta kiểm tra xem sao/no")
			return
		end
	end
	--Meridan material - Modified by DinhHQ - 20120718
	if particular == 2745 and CalcFreeItemCellCount() < 1 then
		Say(MSG_NOTENOUGH_ITEM2745, 0)
		return
	end
	if (fld_TakeBoat(PlayerIndex) ~= 1) then
		Say("Thời gian không đợi ai cả! Thuyền đã đi rồi, lần sau ngươi hãy quay lại!", 0)
		return
	end
	
	RemoveItemByIndex(nItemIdx);
	if particular == 2745 then
		AddStatData("shuizeilingpai_shiyongshuliang", 1)	--数据埋点第一期
	end
	tbLog:PlayerActionLog("EventChienThang042011","BaoDanhPhongLangDo")	-- 报名日志
	--ghi log tính năng key - Modified By DinhHQ - 20120410
	if particular == 2745 then
		tbLog:PlayerActionLog("TinhNangKey","BaoDanhPLD_SDLenhBaiThuyTac")
	else
		tbLog:PlayerActionLog("TinhNangKey","BaoDanhPLD_SDLenhBaiPLD")
	end
end;

function check_new_shuizeitask()
	local nHour = tonumber(GetLocalDate("%H"));
	--Điều chỉnh thời gian phong lăng độ tốn phí - Modified by DinhHQ - 20110504
	local tb_sptime = {
		[10] = 1,
		[14] = 1,
		[16] = 1,
		[18] = 1,
		[20] = 1,
	};
	if (tb_sptime[nHour] and tb_sptime[nHour] == 1) then
		return 1
	else
		return 0
	end
end	

function no()
end;
