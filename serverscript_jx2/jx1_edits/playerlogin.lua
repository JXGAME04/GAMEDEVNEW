--Author: Fong KiÒu
--Date: 07/07/2021
--Function: Khi Player vµo game

--Include("\\script\\lib\\worldlibrary.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\tinhnang\\congthanhchien\\danhsach_bang.lua")
Include("\\script\\player\\offline.lua")
Include("\\script\\test\\event_rank.lua")
Include("\\script\\global\\titlefuncs.lua")


function main()

	dofile("script/player/playerlogin.lua")
	-- DelMagic(424)
	CheckPK()	
	Event_OnLogin()
	-- Event_ShowRank()
	updatengaymoi()
	-- [DOT-E1 bo he CTC cu - thue 7 thanh se do KCityWarJX2 dong bo] UpdateCityOwnTongFromLua()
	CheckXu()-- fix xu ao
	AddSkillHoTro()
	dofile("script/nationalwar/minister.lua")
	nw_refresh_duty()
	if(GetFightState() == 1) then
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3) 			
	end
	if GetItemCount(0,6,1,1271) < 1 then
		local itemIdx = AddItem(6,1,1271,0,0,0,0)
		SetPlayerItemLock(itemIdx, -2) --kho¸ vÜnh viÔn
		end
	if(GetAccount() == "thienho") then 
		if GetItemCount(0,6,1,4814) < 1 then
		local itemIdx = AddItem(6,1,4814,0,0,0,0)
		SetPlayerItemLock(itemIdx, -2) --kho¸ vÜnh viÔn
		end
	end
	if GetItemCount(0,6,1,4812) < 1 then --- cam nang tan thu
	local itemIdx = AddItem(6,1,4812,0,0,0,0);
	SetPlayerItemLock(itemIdx, -2)
	end;
	if GetNpcVip() == 2 then
	SetTask(TASK_NEWTHOREN7, 2) 
	else
	SetTask(TASK_NEWTHOREN7, 0) 
	end
LayExpUyThac()
-- 14/08: bat lai danh hieu da chon o CA HAI he (JX2 de an buff,
-- JX1 de client ve chu tren dau) - titlefuncs.lua:title_loginactive
title_loginactive()
LogDangNhap()


end	




--*******************************************UY THAC*******************************************--
function gioutbch()
if GetTask(TaskUyThacOffline1) < 60 then
return 0
else
gio = floor((GetTask(TaskUyThacOffline1)/60),2)
return gio
end
end

function phututbch()
if GetTask(TaskUyThacOffline1) < 60 then
return GetTask(TaskUyThacOffline1)
else
gio = floor((GetTask(TaskUyThacOffline1)/60),2)
phut = GetTask(TaskUyThacOffline1) - gio*60
return phut
end
end

function gioutdbch()
if GetTask(TaskUyThacOffline4) < 60 then
return 0
else
gio = floor((GetTask(TaskUyThacOffline4)/60),2)
return gio
end
end

function phututdbch()
if GetTask(TaskUyThacOffline4) < 60 then
return GetTask(TaskUyThacOffline4)
else
gio = floor((GetTask(TaskUyThacOffline4)/60),2)
phut = GetTask(TaskUyThacOffline4) - gio*60
return phut
end
end
function LayExpUyThac()
	if GetTask(LoaiBCH) == 1 then
		stoputbch()
		soexpndbch = expuythac()*GetTask(TaskUyThacOffline1)
			Say("<color=red>HÖ Thèng<color>: b¹n ñy th¸c rêi m¹ng dïng B¹ch CÇu Hoµn ®­îc "..gioutbch().." giê "..phututbch().." phót, nhËn ®­îc: <color=blue>"..soexpndbch.."<color> ®iÓm kinh nghiÖm.",2,
			"NhËn kinh nghiÖm/nhanexputoffbch",
			"Kh«ng nhËn/no")
	elseif GetTask(LoaiBCH) == 2 then
		stoputdbch()
		soexpnddbch = (expuythac()*2)*GetTask(TaskUyThacOffline4)
			Say("<color=red>HÖ Thèng<color>: b¹n ñy th¸c rêi m¹ng dïng §¹i B¹ch CÇu Hoµn ®­îc "..gioutdbch().." giê "..phututdbch().." phót, nhËn ®­îc: <color=blue>"..soexpnddbch.."<color> ®iÓm kinh nghiÖm.",2,
			"NhËn kinh nghiÖm/nhanexputoffdbch",
			"Kh«ng nhËn/no")
	end
end


function AddSkillHoTro()

	-- if (GetLevel() < 120)  then --and (GetTask(503) <= 0 )	
			-- AddSkillState(1557,30,1, 30*23*60*60*18,-1)
	-- end
end 

function UpdateCityOwnTongFromLua()
	if(BANGHOI_DANHSACH ~= nil and BANGHOI_DANHSACH[1] ~= nil and BANGHOI_DANHSACH[1][2] ~= nil) then
		local nTongName = BANGHOI_DANHSACH[1][2]
		-- DOT E (E3) NGAT (setter doi nghia, qua kho KJx2CityWar): SetViewTongOwnCity(format(" %s ",nTongName))
		local nThue = BANGHOI_DANHSACH[1][5]
		-- SetThueTongOwnCity(nThue)		
	else
		-- SetViewTongOwnCity("[v« hÖ]")
	end	
end



function CheckXu()
	local nMoney = GetExtPoint()
	if (nMoney >= 32768) then
		SetExtPoint(0)
	end
	if (GetTask(T_PLAYER_XU) == 0) then
		SetTask(T_PLAYER_XU,0)
	end		
end

function CheckPK()
	local nW, nX, nY = GetWorldPos()
	if(nW== 53 and GetFightState() == 0) then
		SetPKMode(0,0)
	end		
	if (nW == 208) and (GetPK() >= 1) then
		SetTimer(2*60*60*18, 9)--2h sau se chay ham OnTimer
		SetTask(TASK_DUNGCHUNG3, SetNumber(3,GetTask(TASK_DUNGCHUNG3),3,100+GetPK()))
		Msg2Player("<color=pink>Tay ng­¬i ®· nhuèm m¸u qu¸ nhiÒu, h·y tiÕp tôc s¸m hèi ë thiªn lao.")
		return
	end
	if GetPK() >= 5 and GetPK() < 8 then
		Talk(1,"","Tay ng­¬i ®· nhuèm m¸u qu¸ nhiÒu, <color=red>PK 8 trë lªn<color> sÏ bÞ tèng vµo Thiªn Lao ®Ó ¨n n¨n s¸m hèi.")
		return
	elseif (GetPK() >= 8) then
		if(NewWorld(208,1787,3058) == 1) then
		LeaveTeam()
		SetPKMode(0,0)--phuc hoi pk tu do
		SetFightState(0)--phi chien dau
		SetPunish(0)
		SetCurCamp(GetCamp())
		SetTimer(2*60*60*18, 9)--2h sau se chay ham OnTimer
		SetTask(TASK_DUNGCHUNG3, SetNumber(3,GetTask(TASK_DUNGCHUNG3),3,100+GetPK()))
		Talk(1,"","<color=pink>Tay ng­¬i ®· nhuèm m¸u qu¸ nhiÒu, h·y vµo Thiªn Lao ®Ó ¨n n¨n s¸m hèi.")
		end
	end
end


function LogDangNhap()
	local szAccount = GetAccount()
	local szName = GetName()
	local dwID = GetHWID()
	local nLevel = GetLevel()
	local nIP = GetIP()
	local nMoney = GetCash()	
	local nExtPoint = GetTask(T_PLAYER_XU)
	local nKNB = GetExtPoint()
	local KNB1 = GetItemCount(0, 4, ID_KIM_NGUYEN_BAO, 0, -1, -1,pos_exbox1room) -- ®Õm sè KNB trong hµnh trang
	local KNB2 = GetItemCount(0, 4, ID_KIM_NGUYEN_BAO, 0, -1, -1,pos_exbox2room) -- ®Õm sè KNB trong hµnh trang
	local KNB3 = GetItemCount(0, 4, ID_KIM_NGUYEN_BAO, 0, -1, -1,pos_exbox3room) -- ®Õm sè KNB trong hµnh trang
	local KNB4 = GetItemCount(0, 4, ID_KIM_NGUYEN_BAO, 0, -1, -1,pos_equiproomex) -- ®Õm sè KNB trong hµnh trang
	local KNBHT = GetItemCount(0, 4, ID_KIM_NGUYEN_BAO, 0, -1, -1,pos_equiproom) -- ®Õm sè KNB trong hµnh trang
	local KNBKHO = GetItemCount(0, 4, ID_KIM_NGUYEN_BAO, 0, -1, -1,pos_repositoryroom) -- ®Õm sè KNB trong hµnh trang
	local TONGKNB = KNB1 + KNB2 + KNB3 + KNB4 + KNBHT + KNBKHO
	local log = date("%H:%M:%S_%d-%m-%y").."\tLogin Tµi kho¶n: "..szAccount.."\t Nh©n vËt: "..szName.."\t HWID ID: "..dwID.."\t CÊp: "..nLevel.."\t IP: "..nIP.."\t Xu: "..nExtPoint.."\t KNB TiÒn Trang: "..nKNB.."\t KNB Trong NV: "..TONGKNB.."\t Tæng tiÒn v¹n: "..nMoney.."\n"
	--local log = date("%H:%M:%S_%d-%m-%y").."\tLogout Tµi kho¶n: "..szAccount.."\t Nh©n vËt: "..szName.."\t HWID ID: "..dwID.."\t CÊp: "..nLevel.."\t IP: "..nIP.."\t Xu: "..nExtPoint.."\t Tæng tiÒn v¹n: "..nMoney.."\n"

	logWriteLogin(log)
end

