--³ÇÊÐ¹«¸æÅÆ
function SayCityStatus()
	nCityID = GetCityArea();
	Say(GetCitySummary(nCityID) .. "\n" .. NW_GetSealInfo(), 0);
end;

function ManageCity()
	nCityID = GetCityArea();
	-- DOT E (E7): client ta khong co UI 0xA3 cua JX2 -> dat thue qua thoai;
	-- gating (Thai Thu / 22h-23h / tran MaxExchangeTax / 1 lan-ngay) nam trong
	-- C CTC_JX2_SetTax - y het goc, chi khac hinh thuc nhap (ghi ban giao).
	OpenCityManageUI(nCityID);	-- giu loi goi goc (no-op) de doi chieu
	Say("Th¸i Thó muèn thiÕt ®Æt thuÕ suÊt míi cho thµnh? (hiÖn t¹i: "..CTC_JX2_GetTax(nCityID).." phÇn tr¨m; mçi ngµy mét lÇn, 22h-23h)", 6,
		"0%/#ctc7_settax(0)", "5%/#ctc7_settax(5)", "10%/#ctc7_settax(10)",
		"15%/#ctc7_settax(15)", "20%/#ctc7_settax(20)", "OnCancel/OnCancel");
end;

function ctc7_settax(nTax)
	local nRet = CTC_JX2_SetTax(GetCityArea(), nTax);
	if (nRet == 0) then
		Say("§· thiÕt ®Æt thuÕ suÊt míi cho thµnh.", 0);
	elseif (nRet == 1) then
		Say("B¹n kh«ng ph¶i lµ Bang chñ cña bang héi!", 0);
	elseif (nRet == 2) then
		Say("Tr­íc m¾t kh«ng cho phÐp thiÕt ®Þnh thuÕ suÊt. Mçi ngµy chØ thiÕt ®Þnh thuÕ suÊt trong thêi gian ng¾n: %d ®iÓm?%d ®iÓm", 0);
	elseif (nRet == 3) then
		Say("B¹n ®· ®Æt thuÕ suÊt cña ngµy h«m nay!!", 0);
	else
		Say("ThuÕ suÊt kh«ng hîp lÖ.", 0);
	end;
end;

function main()
	nCityID = GetCityArea();
	if (nCityID >= 1 and nCityID <= 7) then
		TongName, MasterName = GetCityOwner(nCityID);
		if (MasterName == GetName()) then
			Say("Lµm chøc Th¸i Thó, b¹n cã muèn thiÕt ®Æt thuÕ míi kh«ng?", 2, "Muèn/ManageCity", "Kh«ng, ta chØ muèn xem th«ng tin cña thµnh thÞ. /SayCityStatus");
		else
			SayCityStatus();
		end;
	else
		Say("Khu vùc kh«ng cã qu¶n lý. ", 0);
	end;
end;

