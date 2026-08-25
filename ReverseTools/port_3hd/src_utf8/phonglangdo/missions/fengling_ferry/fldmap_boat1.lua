Include("\\script\\missions\\fengling_ferry\\fld_head.lua");
Include("\\script\\event\\jiefang_jieri\\200904\\taskctrl.lua");

function fenglingdu_main()
	orgworld = SubWorld
	for i = 1, getn( boatMAPS ) do
		mapid = boatMAPS[ i ]

		boatidx = SubWorldID2Idx(mapid)
		if (boatidx < 0) then
			local SignMapId = SubWorldIdx2ID(SubWorld);
			print("ERROR !!!BoatMap1 Is Not In This Server!", mapid); 
			SubWorld = orgworld;
			return
		end
		SubWorld = boatidx
		CloseMission(MISSIONID);
		OpenMission(MISSIONID)
		SetMissionV(MS_STATE, 1)
	end
	SubWorld = orgworld
	str = "Cuộc đua thuyền ở Phong Lăng Độ chuẩn bị khai cuộc, hãy mau đến Bờ Nam Phong Lăng Độ gặp Thuyền phu nộp Phong Lăng Độ lệnh bài hoặc 200 quyển Mật đồ thần bí để đăng ký thuyền!"
	AddGlobalCountNews(str, 3)
	
	local nDate = tonumber(GetLocalDate("%Y%m%d"));		-- by bel 开船前10分钟发出消灭水贼活动的公告
	if (nDate >= jf0904_act_dateS and nDate < jf0904_act_dateE) then
		local szNews = "Còn 10 phút nữa Quan phủ sẽ phát động chiến dịch tiêu diệt Thủy Tặc. Các cao thủ hãy chuẩn bị!";
		AddGlobalNews(szNews);
	end
end