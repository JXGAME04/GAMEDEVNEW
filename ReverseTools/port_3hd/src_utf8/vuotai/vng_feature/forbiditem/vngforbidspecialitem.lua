Include("\\script\\item\\forbiditem.lua")

tbVNGForbidItem = {}
tbVNGForbidItem.CHALLENGE_OF_TIME = 1
tbVNGForbidItem.SEVEN_CITY = 2
tbVNGForbidItem.VLDNB = 3
tbVNGForbidItem.tbForbidItem = 
	{
		["Thần Hành Phù"] = {6, 1, 1266},
--		["Tâm Tâm Tương ánh phù"] = {6, 1, 18}, disable trong file forbitheart.txt
--		["Hồi thành phù (nhỏ) "] = {6, 1, 1082},
--		["Hồi thành phù (lớn) "] = {6, 1, 1083},
		["Tống Kim Chiêu thư "] = {6, 1, 155},
		["Mạc Bắc Truyền Tống Lệnh"] = {6, 1, 1448},
		["Lệnh bài Vi Sơn đảo"] = {6, 1, 2432},
		["Lệnh bài vi sơn đảo lễ bao"] = {6, 1, 2525},
	}

tbVNGForbidItem.tbMapSet = 
	{
		--Vuot ai
		[1] =
			{
				464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479,
				480, 481, 482, 483, 484, 485, 486, 487, 488, 489, 490, 491, 492, 493, 494, 495, 
				957, --ai 30
			},
		--seven city
		[2] = 
			{
				926, 927, 928, 929, 930, 931, 932,
			},
		--VLDNB	
		[3] =
			{
				605, 608, 609, 606, 610, 611, 607, 612, 613,
			},
		--Arena
		[4] =
			{
				975
			},
		--Arena TK
		[5] =
			{
				380
			},
	}
	
tbVNGForbidItem.tbItemList = {
	["6,1,218"] = {strName = "Công Tốc hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,219"] = {strName = "Bào Tốc hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,220"] = {strName = "Phổ Phòng hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,221"] = {strName = "Độc Phòng hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,222"] = {strName = "Băng Phòng hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,223"] = {strName = "Hỏa Phòng hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,224"] = {strName = "Lôi Phòng hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,225"] = {strName = "Giảm Thương hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,226"] = {strName = "Giảm Hôn hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,227"] = {strName = "Giảm Độc hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,228"] = {strName = "Giảm Băng hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,229"] = {strName = "Phổ Công hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,230"] = {strName = "Độc Công hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,231"] = {strName = "Băng Công hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,232"] = {strName = "Hỏa Công hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,233"] = {strName = "Lôi Công hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,234"] = {strName = "Trường Mệnh hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,235"] = {strName = "Trường Nội hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,115"] = {strName = "Yên Hồng đan", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,116"] = {strName = "Xá Lam đan", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,117"] = {strName = "Nội Phổ hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,118"] = {strName = "Nội Độc hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,119"] = {strName = "Nội Băng hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,120"] = {strName = "Nội Hỏa hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,121"] = {strName = "Nội Điện hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,0,1"] = {strName = "Trường Mệnh hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,0,2"] = {strName = "Gia Bào hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,0,3"] = {strName = "Đại Lực hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,0,4"] = {strName = "Cao Thiểm hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,0,5"] = {strName = "Cao Trung hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,0,6"] = {strName = "Phi Tốc hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,0,7"] = {strName = "Băng Phòng hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[5],},},
	["6,0,8"] = {strName = "Lôi Phòng hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[5],},},
	["6,0,9"] = {strName = "Hỏa Phòng hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[5],},},
	["6,0,10"] = {strName = "Độc Phòng hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[5],},},
	["6,0,60"] = {strName = "Bánh chưng Hạt dẻ", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,0,61"] = {strName = "Bánh chưng Thịt heo", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,0,62"] = {strName = "Bánh chưng Thịt bò", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,73"] = {strName = "Bách Quả Lộ", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,19"] = {strName = "Cát tường hồng bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,69"] = {strName = "Hoàng Kim Bảo Hạp", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,885"] = {strName = "Tín Sứ Mộc yêu bài", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,886"] = {strName = "Tín Sứ Đồng yêu bài", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,887"] = {strName = "Tín Sứ Ngân yêu bài", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,888"] = {strName = "Tín Sứ Kim yêu bài", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,889"] = {strName = "Ngự Tứ Tín Sứ yêu bài", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,910"] = {strName = "Bao Dược hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,1074"] = {strName = "Hộp lễ vật Hồng bao thần bí", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,1075"] = {strName = "Hộp lệ vật [quả Huy Hoàng]", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,1389"] = {strName = "Hình nộm", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,1781"] = {strName = "Cẩm nang thay đổi trời đất", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2318"] = {strName = "Mộc Chế Bảo Hạp", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2319"] = {strName = "Đồng Chế Bảo Hạp", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2320"] = {strName = "Ngân Chế Bảo Hạp", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2321"] = {strName = "Hoàng Kim Bảo rương", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2322"] = {strName = "Bảo rương Bạch Kim", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2347"] = {strName = "Sát Thủ Bí Bảo", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2374"] = {strName = "Bảo rương thần bí của Dã Tẩu", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2375"] = {strName = "Chí Tôn Bí Bảo", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2376"] = {strName = "Tài Bảo Thủy Tặc", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2527"] = {strName = "Hồi thiên tái tạo lễ bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2520"] = {strName = "Phi tốc hoàn lễ bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2517"] = {strName = "Đại lực hoàn lễ bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2377"] = {strName = "Công Thành Chiến Lễ Bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2524"] = {strName = "Độc phòng hoàn lễ bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2515"] = {strName = "Trường mệnh hoàn lễ bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2516"] = {strName = "Gia tốc hoàn lễ bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2518"] = {strName = "Cao thiểm hoàn lễ bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2519"] = {strName = "Cao trung hoàn lễ bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2520"] = {strName = "Phi tốc hoàn lễ bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2521"] = {strName = "Băng phòng hoàn lễ bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2522"] = {strName = "Lôi phòng hoàn lễ bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2523"] = {strName = "Hỏa phòng hoàn lễ bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2524"] = {strName = "Độc phòng hoàn lễ bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2525"] = {strName = "Lệnh bài vi sơn đảo lễ bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2830"] = {strName = "Hoàn Hồn Đơn Lễ Bao", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2831"] = {strName = "Tiêu Diêu Tán", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2837"] = {strName = "Hoàn Hồn Đơn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2953"] = {strName = "Âm Dương Hoạt Huyết Đơn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
	["6,1,2952"] = {strName = "Kích Công Trợ Lực Hoàn", tbForbidInMap = {tbVNGForbidItem.tbMapSet[4],},},
}

function tbVNGForbidItem:IsForbidMap(strItemName, tbMapSet)
	local nW, _, _ = GetWorldPos()
	local tbTempMapSet = {}
	for i = 1, getn(tbMapSet) do
		tbTempMapSet = self.tbMapSet[tbMapSet[i]]
		if not tbTempMapSet then
			Msg2Player("Không xác định được khu vực giới hạn sử dụng vật phẩm")
			return 1
		end
		for j = 1, getn(tbTempMapSet) do
			if nW == tbTempMapSet[j] then
				Msg2Player( format("Khu vực hiện tại không được phép sử dụng <color=yellow>%s",strItemName ))
				return 1
			end
		end
	end	
	return 0	
end

function tbVNGForbidItem:CheckItemUsable(nMapID, strItemCode)
	if not self.tbItemList[strItemCode] then
		return 1
	end
	local tbForbidMap = self.tbItemList[strItemCode].tbForbidInMap
	for i = 1, getn(tbForbidMap) do
		for j = 1, getn(tbForbidMap[i]) do
			if nMapID == tbForbidMap[i][j] then
				return 0
			end
		end
	end
	return 1
end