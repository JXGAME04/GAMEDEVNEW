-- ================================================================================================
-- [HE THONG] script/header/tongkim.lua
-- Muc dich  : TONG KIM: cau hinh dau truong, gio mo, map.
-- Duoc nap  : Include tu 8 tep (vd mission01.lua, timerserver.lua, task01.lua, task02.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : LIMITED_1_ACCOUNT.lua, worldlibrary.lua, lib_ham.lua
-- Ham (dong): CheckLimited_Join (6), ExcuteCheck (17), SAVEDULIEU (33), DELDULIEU (57), Split (63)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
--Author: Kinnox
--Date: 17/09/2020
Include("\\script\\tinhnang\\tong_kim_tcap\\LIMITED_1_ACCOUNT.lua")
Include("\\script\\lib\\worldlibrary.lua")
Include("\\script\\lib\\lib_ham.lua")


function CheckLimited_Join()
dofile("script/header/tongkim.lua");
local oldPlayerIndex = PlayerIndex
	for i =1,MAX_PLAYER do
		PlayerIndex = i
		ExcuteCheck()
	end
	PlayerIndex = oldPlayerIndex
end

	
	
function ExcuteCheck()
	for i = 1,getn(LIMITED_1_ACCOUNT) do -- limited 1 account login TONGKIM
		if GetIP() == LIMITED_1_ACCOUNT[i][1] and GetName() == LIMITED_1_ACCOUNT[i][2] then
			local nNamePlayer;
			local nKey;
				nNamePlayer	= "";
				nKey	= "";
				SAVEDULIEU(i,nKey,nNamePlayer);	
			return
		end	
				
	end
end

function SAVEDULIEU(i,nKey,nNamePlayer)
	-- (21/08) BOT KHONG GHI TEP. GetIP() cua bot tra CHUOI RONG (LuaGetIP ->
	-- GetClientInfo tra NULL vi bot khong co ket noi mang), nen moi bot deu
	-- mang cung mot "IP" rong - luu vao bang la vo nghia. Ma moi lan luu la
	-- GHI DE CA TEP nay bang SaveData tren luong game: hang tram bot bao danh
	-- trong cua so 60 giay = hang tram lan ghi dia dong bo, bang lai phinh dan
	-- nen tong chi phi la O(n^2). Nguy hiem hon: ham nay duoc goi SAU
	-- AddMSPlayer nhung TRUOC common_tong/common_kim, nen mot loi ghi tep
	-- (openfile tra nil) se giet ca ham -> bot da chiem khe mission, duoc dem
	-- vao can bang quan so, nhung KHONG duoc gan camp va KHONG vao map 379.
	if (nKey == "" and nNamePlayer ~= "") then
		return
	end
	if nKey == "" and nNamePlayer == "" then
		LIMITED_1_ACCOUNT[i] = {nKey,nNamePlayer};
		local nNAME_Change = TaoBang(LIMITED_1_ACCOUNT,"LIMITED_1_ACCOUNT");
		SaveData("script/tinhnang/tong_kim_tcap/LIMITED_1_ACCOUNT.lua",nNAME_Change);
	else
		LIMITED_1_ACCOUNT[getn(LIMITED_1_ACCOUNT)+1] = {nKey,nNamePlayer};
		local nNAME = TaoBang(LIMITED_1_ACCOUNT,"LIMITED_1_ACCOUNT");
		SaveData("script/tinhnang/tong_kim_tcap/LIMITED_1_ACCOUNT.lua",nNAME);
	end
end

function DELDULIEU()
	LIMITED_1_ACCOUNT= {{"KEY_COMPUTER","NAME_PlAYER"}};
	local nNAME = TaoBang(LIMITED_1_ACCOUNT,"LIMITED_1_ACCOUNT");
	SaveData("script/tinhnang/tong_kim_tcap/LIMITED_1_ACCOUNT.lua",nNAME);
end





function Split(str,splitor)
	if(splitor==nil) then
		splitor=","
	end
	local strArray={}
	local strStart=1
	local splitorLen = strlen(splitor)
	local index=strfind(str,splitor,strStart)
	if(index==nil) then
		strArray[1]=str
		return strArray
	end
	local i=1
	while index do
		strArray[i]=strsub(str,strStart,index-1)
		i=i+1
		strStart=index+splitorLen
		index = strfind(str,splitor,strStart)
	end
	strArray[i]=strsub(str,strStart,strlen(str))
	return strArray
end



