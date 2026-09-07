-- 
-- by 
-- 2007.10.24
-- ÎÒ..
-- ..
-- ..


-- mission
function YDBZ_ready_missions(map, mission)
	local oldsubworld = SubWorld
	for i = 1, getn(map) do
		index = SubWorldID2Idx(map[i]);
		--print("open mission map:"..index)
		if (index >= 0) then
			SubWorld = index;
			OpenMission(mission);
		end
	end
	SubWorld = oldsubworld
end

-- mission
function YDBZ_ready_close_missions(map, mission, status)
	local oldsubworld = SubWorld
	for i = 1, getn(map) do
		index = SubWorldID2Idx(map[i]);
		if (index >= 0) then
			SubWorld = index;
			--if (GetMissionV(status) ~= 0) then
				CloseMission(mission);
			--end
		end
	end
	SubWorld = oldsubworld
end
