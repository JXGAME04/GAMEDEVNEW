-- tong_luong.lua - [MAIL 03/09 D9] PHAT LUONG BANG HOI qua thu (chu 03/09). Include vao state NPC Tong quan hoat dong
-- (scriptjx2\tong_vn\npc\huodong_zongguan.lua). Bang chu chon muc luong moi thanh vien, tru quy bang (TONG_GetMoney),
-- moi thanh vien (ca offline) nhan 1 thu 'Luong bang hoi' dinh kem Ngan luong; 1 lan/ngay/bang (TONG task 1200 = yyyymmdd).
Include("\\script\\mail\\mailmanager.lua")

TONGLUONG_MUC       = {10000, 50000, 100000, 500000}   -- Ngan luong moi thanh vien (menu)
TONGLUONG_TASK_NGAY = 1200                             -- TONG_GetTaskValue: ngay da phat (yyyymmdd)
TONGLUONG_HAN_NGAY  = 30

function TongLuong_Menu(nTongID)
	local nQuy = TONG_GetMoney(nTongID)
	local nTV = TONG_GetMemberCount(nTongID, -1)
	Say("<#>Tæng qu¶n ho¹t ®éng: Bang chñ muèn lµm g×?<enter>Quü bang: "..nQuy.." Ng©n l­îng, thµnh viªn: "..nTV, 3,
		"Ph¸t l­¬ng bang héi (qua th­)/TongLuong_ChonMuc",
		"Ho¹t ®éng bang/TongLuong_HoatDongCu",
		"Rêi khái/OnCancelTongLuong")
end

function OnCancelTongLuong()
end

-- duong cu cua NPC (ws_huodong.lua USE_G_1) cho bang chu
function TongLuong_HoatDongCu()
	local _, nTongID = GetTongName()
	local nTypeID = wsGetTypeID(NAME_HUODONG)
	local nWorkshopID = TWS_GetFirstWorkshop(nTongID, nTypeID)
	USE_G_1(nTongID, nWorkshopID)
end

function TongLuong_ChonMuc()
	local _, nTongID = GetTongName()
	local nTV = TONG_GetMemberCount(nTongID, -1)
	local m = TONGLUONG_MUC
	Say("<#>Chän møc l­¬ng mçi thµnh viªn ("..nTV.." ng­êi). Tæng trõ quü = møc x sè thµnh viªn.", 5,
		m[1].." Ng©n l­îng (tæng "..(m[1] * nTV)..")/#TongLuong_Phat(1)",
		m[2].." Ng©n l­îng (tæng "..(m[2] * nTV)..")/#TongLuong_Phat(2)",
		m[3].." Ng©n l­îng (tæng "..(m[3] * nTV)..")/#TongLuong_Phat(3)",
		m[4].." Ng©n l­îng (tæng "..(m[4] * nTV)..")/#TongLuong_Phat(4)",
		"Rêi khái/OnCancelTongLuong")
end

function TongLuong_Phat(nMuc)
	local szTong, nTongID = GetTongName()
	if nTongID == 0 or TONG_GetMaster(nTongID) ~= GetName() then
		Msg2Player("ChØ Bang chñ míi ®­îc ph¸t l­¬ng.")
		return
	end
	local nMoiNguoi = TONGLUONG_MUC[nMuc] or 0
	if nMoiNguoi <= 0 then
		return
	end
	local nHomNay = tonumber(GetLocalDate("%Y%m%d")) or 0
	if TONG_GetTaskValue(nTongID, TONGLUONG_TASK_NGAY) == nHomNay then
		Msg2Player("H«m nay bang ®· ph¸t l­¬ng råi, mai h·y ph¸t tiÕp.")
		return
	end
	local tbTen = {}
	local nMem = TONG_GetFirstMember(nTongID, -1)
	while nMem and nMem > 0 do
		local szTen = TONGM_GetName(nTongID, nMem)
		if szTen and szTen ~= "" then
			tinsert(tbTen, szTen)
		end
		nMem = TONG_GetNextMember(nTongID, nMem, -1)
	end
	if getn(tbTen) == 0 then
		Msg2Player("Bang ch­a cã thµnh viªn.")
		return
	end
	local nTong = nMoiNguoi * getn(tbTen)
	local nQuy = TONG_GetMoney(nTongID)
	if nQuy < nTong then
		Msg2Player("Quü bang kh«ng ®ñ: cÇn "..nTong.." Ng©n l­îng, hiÖn cã "..nQuy..".")
		return
	end
	if TONG_ApplyAddMoney(nTongID, -nTong) ~= 1 then
		Msg2Player("Trõ quü bang thÊt b¹i, h·y thö l¹i.")
		return
	end
	TONG_ApplySetTaskValue(nTongID, TONGLUONG_TASK_NGAY, nHomNay)
	local nOk = 0
	local szSender = "Bang héi "..szTong
	local szND = "Bang chñ "..GetName().." ph¸t l­¬ng cho thµnh viªn bang "..szTong..".<enter>L­¬ng kú nµy: "..nMoiNguoi.." Ng©n l­îng, ®Ýnh kÌm trong th­.<enter>Tr©n träng"
	for i = 1, getn(tbTen) do
		local nId = MailManager_SendMail(tbTen[i], szSender, "L­¬ng bang héi", szND, "money:"..nMoiNguoi, TONGLUONG_HAN_NGAY, "bangluong")
		if nId > 0 then
			nOk = nOk + 1
		end
	end
	TONG_ApplyAddEventRecord(nTongID, "Bang chñ ph¸t l­¬ng "..nMoiNguoi.." Ng©n l­îng cho "..nOk.." thµnh viªn (tæng "..nTong..")")
	Msg2Player("§· göi l­¬ng "..nMoiNguoi.." Ng©n l­îng cho "..nOk.."/"..getn(tbTen).." thµnh viªn, trõ quü bang "..nTong..".")
	GhiLog("MAIL", format("Bang %s: bang chu %s phat luong %d x %d thanh vien (ok %d), tru quy %d", szTong, GetName(), nMoiNguoi, getn(tbTen), nOk, nTong))
end
