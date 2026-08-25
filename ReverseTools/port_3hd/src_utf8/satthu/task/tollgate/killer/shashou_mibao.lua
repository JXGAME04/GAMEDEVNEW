-- ====================== 文件信息 ======================

-- 剑侠情缘网络版越南收费版 - 道具:杀手秘宝
-- 文件名　：shashou_mibao.lua
-- 创建者　：子非魚
-- 创建时间：2010-05-24 11:40:18

-- ======================================================

Include("\\script\\lib\\awardtemplet.lua");		-- 奖励模板
Include("\\script\\task\\tollgate\\killer\\mibao_head.lua")

local n_XuanTian_Chui = 6;

function main(nItemIdx)
	local n_curcnt = CalcItemCount(3, 6,1,2348,-1);
	if (n_curcnt < %n_XuanTian_Chui) then
		Msg2Player(format("Cần %d %s, các hạ đem không đủ số lượng!", %n_XuanTian_Chui, "Huyền Thiên Chùy"));
		return 1
	end
	if (CalcFreeItemCellCount() >= 3 and ConsumeItem(3, %n_XuanTian_Chui, 6,1,2348,-1)) then
		tbAwardTemplet:GiveAwardByList(tbItem_Mibao, format("USE %s", "Sát Thủ Bí Bảo"))
		AddStatData("baoxiangxiaohao_kaishashoumibao", 1)		-- 数据埋点第一期
	else
		Msg2Player("Khoảng trống trong túi không đủ! ");
		return 1
	end
end
