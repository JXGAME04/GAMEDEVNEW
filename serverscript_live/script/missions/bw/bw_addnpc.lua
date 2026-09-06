-- [BW 23/08] 12 NPC Cong Binh Tu (tpl 309) doc tu region maps.pak (port_bw\bw_regionnpc.py,
-- trung byte 2 cay) - engine ta bo NPC thoai trong map-data (NotAddNpcNormal=1) nen tu AddNpc.
-- Khuon: task\tollgate\tinsu_addnpc.lua
bw_dialognpc = {
	{309, 80, 1659, 3020, "\\script\\missions\\bw\\bwmanager.lua"},
	{309, 80, 1852, 3049, "\\script\\missions\\bw\\bwmanager.lua"},
	{309, 80, 1627, 3208, "\\script\\missions\\bw\\bwmanager.lua"},
	{309, 80, 1709, 3251, "\\script\\missions\\bw\\bwmanager.lua"},
	{309, 78, 1464, 3183, "\\script\\missions\\bw\\bwmanager_xiangyang.lua"},
	{309, 78, 1705, 3235, "\\script\\missions\\bw\\bwmanager_xiangyang.lua"},
	{309, 78, 1474, 3272, "\\script\\missions\\bw\\bwmanager_xiangyang.lua"},
	{309, 78, 1577, 3376, "\\script\\missions\\bw\\bwmanager_xiangyang.lua"},
	{309, 11, 3071, 5002, "\\script\\missions\\bw\\bwmanager_chengdu.lua"},
	{309, 11, 3225, 5042, "\\script\\missions\\bw\\bwmanager_chengdu.lua"},
	{309, 11, 3040, 5096, "\\script\\missions\\bw\\bwmanager_chengdu.lua"},
	{309, 11, 3166, 5195, "\\script\\missions\\bw\\bwmanager_chengdu.lua"},
}
function bw_addnpc()
	local nOld = SubWorld
	for i = 1, getn(bw_dialognpc) do
		local t = bw_dialognpc[i]
		local SId = SubWorldID2Idx(t[2])
		if (SId >= 0) then
			SubWorld = SId
			local idx = AddNpc(t[1], 1, SId, t[3] * 32, t[4] * 32, 0, "C«ng B×nh Tö")
			if (idx > 0) then
				SetNpcScript(idx, t[5])
			end
			SubWorld = nOld
		end
	end
end
