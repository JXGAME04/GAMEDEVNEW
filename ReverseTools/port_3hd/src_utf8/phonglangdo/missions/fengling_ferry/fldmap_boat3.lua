Include("\\script\\missions\\fengling_ferry\\fld_head.lua")
function main()
	mapid = boatMAPS[3]
	orgworld = SubWorld
	boatidx = SubWorldID2Idx(mapid)
	if (boatidx < 0) then
		local SignMapId = SubWorldIdx2ID(SubWorld);
		print("ERROR !!!BoatMap3 Is Not In This Server!", mapid); 
		return
	end
	SubWorld = boatidx
	OpenMission(MISSIONID)
	str = "Cuộc đua thuyền ở Phong Lăng Độ chuẩn bị khai cuộc, hãy mau đến Bờ Nam Phong Lăng Độ gặp Thuyền phu Bính ất nộp Phong Lăng Độ lệnh bài hoặc 200 quyển Mật đồ thần bí để đăng ký thuyền!"
	AddGlobalCountNews(str, 3)
	SetMissionV(MS_STATE, 3)
	SubWorld = orgworld
end