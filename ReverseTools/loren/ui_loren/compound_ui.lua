-- compound_ui.lua - [UILOREN v37] nut bam phia may chu cua cua so lo ren
-- Dat tep nay vao \script\item\compound\ tren may chu.
-- CACH DUNG: script NPC / lenh bai Include tep nay roi goi LR_UI_Mo().
--   OpenCompoundItem luu script id DANG CHAY (m_ActionScriptID) vao
--   m_dwCompoundItemId, nen 6 ham LR_UI_* phai nam TRONG NGU CANH script do
--   (Include la du) thi ExecuteScript moi goi trung.
-- Duong di nut bam: client SendUiCmdScript(7, "LR_UI_...")
--   -> KProtocolProcess::UiCommandScript case 7 (danh sach trang 6 ten)
--   -> ExecuteScript(m_dwCompoundItemId, "LR_UI_...", "")
-- Lua 4.0.1: khong ipairs/unpack; GetIdItem tra 0 = o rong.

-- Bang phong GetIdItem (nPos / so o / nType FoundryCompound):
--   4 = MotOre  ( 8 o) -> nType 0  hop thanh Huyen Tinh
--   5 = HaiOre  ( 8 o) -> nType 1  nang cap khoang the 1
--   6 = BaOre   ( 8 o) -> nType 2  nang cap khoang the 2
--   7 = Distill (11 o) -> nType 3  chiet xuat thuoc tinh  (o 3..10 la phu)
--   8 = Forge   ( 2 o) -> nType 4  duc trang bi tim
--   9 = Enchase (11 o) -> nType 5  kham nam               (o 3..10 la phu)

-- Thong bao theo ma FOUNDRY_RESULT (0 thanh cong .. 8 sai luat)
LR_UI_TBKQ = {};
LR_UI_TBKQ[0] = "Lß rÌn thµnh c«ng!";
LR_UI_TBKQ[1] = "Lß rÌn thÊt b¹i, nguyªn liÖu tiªu hao theo luËt.";
LR_UI_TBKQ[2] = "Kh«ng ®ñ tiÒn.";
LR_UI_TBKQ[3] = "Nguyªn liÖu ®· ë cÊp cao nhÊt.";
LR_UI_TBKQ[4] = "ThiÕu nguyªn liÖu, h·y xÕp ®ñ vµo c¸c «.";
LR_UI_TBKQ[5] = "Sai cÊp nguyªn liÖu.";
LR_UI_TBKQ[6] = "Sai thuéc tÝnh phÐp cña nguyªn liÖu.";
LR_UI_TBKQ[7] = "Sai ngò hµnh cña nguyªn liÖu.";
LR_UI_TBKQ[8] = "Nguyªn liÖu kh«ng ®óng luËt ghÐp.";

LR_UI_KHONGRO = "Lß rÌn gÆp lçi kh«ng râ, h·y thö l¹i.";
LR_UI_TRONG = "Ch­a xÕp nguyªn liÖu vµo lß rÌn.";

function LR_UI_GomPhong(nPos, nSoO, nPhuTu)
	-- gom chi so item trong phong: tra ve 2 bang (chinh, phu); bo o rong (0)
	-- nPhuTu < 0: tat ca vao bang chinh (o dau la nguyen lieu chinh)
	-- nPhuTu >= 0: tu o nPhuTu tro di la nguyen lieu phu
	local aryNec = {};
	local aryAlt = {};
	local i;
	for i = 0, nSoO - 1 do
		local nItemIdx = GetIdItem(nPos, i);
		if (nItemIdx ~= nil and nItemIdx > 0) then
			if (nPhuTu >= 0 and i >= nPhuTu) then
				tinsert(aryAlt, nItemIdx);
			else
				tinsert(aryNec, nItemIdx);
			end
		end
	end
	return aryNec, aryAlt;
end

function LR_UI_Lam(nPos, nSoO, nPhuTu, nType)
	local aryNec, aryAlt = LR_UI_GomPhong(nPos, nSoO, nPhuTu);
	if (getn(aryNec) <= 0) then
		Msg2Player(LR_UI_TRONG);
		return;
	end
	-- bPreview = 0: lam that. Tra ve (chi so item sinh ra, ma ket qua)
	local nResultIdx, nResult = FoundryCompound(nType, aryNec, aryAlt, 0);
	local szMsg = nil;
	if (nResult ~= nil) then
		szMsg = LR_UI_TBKQ[nResult];
	end
	if (szMsg == nil) then
		szMsg = LR_UI_KHONGRO;
	end
	Msg2Player(szMsg);
end

-- 6 ham danh sach trang cua case 7 - ten phai TRUNG KHIT voi C++
function LR_UI_MotOre()
	LR_UI_Lam(4, 8, -1, 0);
end

function LR_UI_HaiOre()
	LR_UI_Lam(5, 8, -1, 1);
end

function LR_UI_BaOre()
	LR_UI_Lam(6, 8, -1, 2);
end

function LR_UI_Distill()
	LR_UI_Lam(7, 11, 3, 3);
end

function LR_UI_Forge()
	LR_UI_Lam(8, 2, -1, 4);
end

function LR_UI_Enchase()
	LR_UI_Lam(9, 11, 3, 5);
end

-- Mo cua so lo ren tu NPC / lenh bai (goi trong script da Include tep nay)
function LR_UI_Mo()
	OpenCompoundItem("");
end
