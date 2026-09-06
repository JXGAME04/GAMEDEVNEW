--Author: Fong KiÒu
--Date: 07/07/2021
--Function: LÖnh Bµi T©n Thñ

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\header\\factionhead.lua")
Include("script\\global\\trangbixanh.lua")
Include("script\\global\\hocvocong.lua")
Include("\\script\\tinhnang\\boss_hoangkim\\lib_bosshk.lua")
Include("\\script\\tinhnang\\congthanhchien\\mapbanghoi.lua")
Include("\\script\\test\\npcwalk.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")
Include("\\script\\global\\admin\\char.lua")
Include("\\script\\global\\admin\\item.lua")
Include("\\script\\global\\admin\\point.lua")
Include("\\script\\global\\admin\\quanly.lua")
-- Include("\\script\\global\\npcchucnang\\thoren.lua")
Include("\\script\\test\\event_rank.lua")



NOW_END_SAY 				= "KÕt thóc ®èi tho¹i./no"
PLAYER_INFOMATION = "<sex> - Tªn: <color=pink>%s<color>.\n- Täa ®é: <color=gold>%s (%d/%d) (%d/%d)<color>.\n- Index: <color=green>%d<color>\n- §iÓm sù kiÖn hiÖn t¹i lµ <color=green>%d<color> ®iÓm\n- <color=white>§iÓm ch©n nguyªn hiÖn t¹i lµ <color=green>%d<color> ®iÓm !"
PLAYER_EVENT = "- Bã Hång Tam S¾c: <color=pink>%s<color> c¸i.\n- Bã Hång Tø S¾c: <color=gold>%s<color> c¸i.\n- Giá Hoa Hång VÜnh Cöu: <color=green>%d<color> c¸i."
PLAYER_DIEM = "- Vâ L©m MËt TÞch: <color=pink>%s<color> cuèn.\n- TÈy Tuû Kinh: <color=gold>%s<color> cuèn."
function main(itemIdx)
	 dofile("script/item/lenhbaitanthu.lua")
	local nName = GetName()
	local nW,nX,nY = GetWorldPos()
	local nMapName = GetMapName(nW)	
	local nDSK = GetTask(TASK_DSK)	
	local nChanNguyen = GetTask(TASK_CHANGNGUYENDAN);
	SayEx({format(PLAYER_INFOMATION,nName,nW,nX*32,nY*32,nX,nY,PlayerIndex,nDSK,nChanNguyen),
	--"§æi ®iÓm sù kiÖn /doiqht",	
	--"§æi LÖnh Bµi Boss /doilbb",	
	"Dïng §iÓm Sù KiÖn §æi Item/diemsukiemdoi",
	-- "Tµi kho¶n trªn m¹ng/xemnguoichoi",
	"NhËn ®iÓm Tµi L·nh §¹o/TaiLanhDao",
	"§æi Xu thµnh TiÒn V¹n/doixuthanhkv",
	"§æi TiÒn V¹n thµnh Xu/doikvthanhxu",		
	-- "NhËn L¹i Khinh C«ng/nhanlaikinhkhong",	
	-- "Xem tiÕn tr×nh Event/xemdiem",
	-- "Xem §iÓm TTK vµ VLMT/xemdiemTTK",
	"Chøc n¨ng chuyÓn ph¸i /chuyenphai",
	
	-- "NhËn Th­ëng TOP 1 ThÕ Giíi /topTG1",
	-- "NhËn Th­ëng TOP 2 ThÕ Giíi /topTG2",
	-- "NhËn Th­ëng TOP 3 ThÕ Giíi /topTG3",
	 -- "NhËn Th­ëng TOP 4 -10 ThÕ Giíi /topTG4",
	-- "NhËn Th­ëng TOP 1 M«n Ph¸i /topTG1MP",
	-- "Tham gia nhËn CODE miÔn phÝ/codegame",
	-- "NhËn th­ëng tÝnh luü n¹p 200./mocnapthe",
	-- "NhËn th­ëng tÝnh luü n¹p 500./mocnapthe500",
	-- "NhËn th­ëng tÝnh luü n¹p 1tr./mocnapthe1000",
	-- "NhËn th­ëng tÝnh luü n¹p 2tr./mocnapthe2000",
	-- "T×m hiÓu ®iÓm sù kiÖn trong ngµy/diemsk",
	"KÕt thóc./no"})
if(GetAccount() == "4") or (GetAccount() == "quocanh96") or (GetAccount() == "thienho") or (GetAccount() == "vodanhtieutu1") or (GetAccount() == "5")then 
		SayEx({format(PLAYER_INFOMATION,nName,nW,nX*32,nY*32,nX,nY,PlayerIndex,nDSK,nChanNguyen),
	-- "T¨ng 5 cÊp/#tangncapadmin(5)",
	-- "T¨ng 50 cÊp/#tangncapadmin(50)",
	-- "NhËn GM/nhangm",
	-- "hocvocong /show_kynang150Chon",
	-- "Huy GM/huygm",
	-- "Nhan Ngua/nhannguagm",
	-- "Nhan LBB/nhanlbb",
	-- "Nhan QHT/nhanqht",
	"Nhan kich/nhankich",
	"Chøc n¨ng chuyÓn ph¸i /chuyenphai",
	-- "Thong Bao/NhanBuHoatDong",
	-- "Tong kim/AdminTestTKH",
	-- "N©ng cÊp b¹ch kim/nangcappanviphe",
	"N©ng cÊp NhÉn V« Danh/doitemhkmp",
	-- "Hñy vËt phÈm/tieuhuytrangbi", 
	-- "§i MBTN /dimacbacthaon",
	-- "Chøc n¨ng chuyÓn ph¸i /chuyenphai",
	-- "NhËn th­ëng tÝnh luü n¹p 200./mocnapthe",
	-- "NhËn th­ëng tÝnh luü n¹p 2tr./mocnapthe500",
	"KÕt thóc./no"})
	end

end




-- EVENT_RANK_LIST = EVENT_RANK_LIST or {}
function nhankich()

-- AddItem(0,10,13,1,0,0,0)
-- laydoxanh0()
-- SetPlayerTitle(96, 1*60, 0)
-- AddItem2(2,0,10,0,0,0) -- An Bang
-- AddItem2(2,0,11,0,0,0) -- An Bang
-- AddItem2(2,0,0,0,0,0) -- An Bang
-- AddSkillState(1557,30,1, 30*23*60*60*18,-1)
-- if (GetNpcVip() < 2) then
		-- AddSkillState(1557, 1, 1,30*24*60*60*18, 1);
		-- else
	-- Talk(1,"","C¸c h¹ ®ang cßn thêi gian nh©n ®«i kinh nghiÖm luyÖn kü n¨ng");	

-- end
-- SetTask(TASK_HD_EVENTNEW, 20000)
-- SetTask(TASK_HD_MISS_PLD, 5) 

-- SetTask(T_NVVTTRONGNGAY, 1) 
-- SetTask(TASK_TANTHU3, 1) 
-- SetTask(TASK_NEWTHOREN6,1) 
-- local nVIP = GetTask(TASK_NEWTHOREN7)
-- Msg2Player("Test <color=green>"..nVIP.." <color=red> c¸i!")
-- local itemIdx = DropItemPUBG(nNpcIndex, 0, random(0,139), 0, 10, 0, 0, 2)
	-- AddTimeItem(itemIdx,60*60*24*7)
	-- AddItemSL(2134,5000,-2) -- Nguyen Lieu
-- AddItemSL(1820,5000,0) -- ho mach don
-- AddItemSL(4841,5000,0) -- event tet 
-- AddItemSL(2134,500,0) -- event tet 
-- AddItem2(2, 0, 7388, 0, 0, 0)
-- AddItem2(2, 0, 7381, 0, 0, 0)
-- AddItem2(2, 0, 7382, 0, 0, 0)
-- AddItem2(2, 0, 140, 0, 0, 0)
-- AddItem2(2, 0, 140, 0, 0, 0)
-- AddItem2(2, 0, 141, 0, 0, 0)
-- AddItem2(2, 0, 141, 0, 0, 0)
-- AddItem2(2, 0, 7385, 0, 0, 0)
-- AddItem(0,10,12,5,0,0,0)
-- AddItem(0,10,12,5,0,0,0)
-- AddItem(0,10,13,4,0,0,0)
-- AddItem(0,10,13,5,0,0,0)

-- AddTimeItem(itemIdx,1* 18)
-- mIndex = AddItem(6,1,4813,0,0,0,0)
-- AddItemSL(4835,5000,0) --tien dong
		-- SetPlayerItemLock(mIndex, -2)
		-- AddTimeItem(mIndex,1*18);  -- thêi gia item 5 ngµy
	-- AddProp(1000)
	-- AddMagicPoint(200)
-- AddSkillState(1552, 50, 1, 60*60*2*18, -1)
-- local nIndexX = AddItemSL(1126,1,0)--27 --313
-- ForceClearStateSkillEffect()
-- ForceClearStateSkillEffectSkillId(661)
-- nIndex = AddItem(0,10,8,10,0,0,10) -- ngua Phi Van
			-- local s_name = GetItemName(nIndexX)
			-- Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>ghÐp thµnh c«ng, nhËn ®­îc <color> "..s_name.." .")
-- local nIndexX = AddItem2(2, 0, 7366, 0, 0, 0)
-- for i = 7380,7397 do 
	-- local nIndexX = AddItem2(2, 0, i, 0, 0, 0)
-- end
-- nIndex = AddItem2(2, 0, 158, 0, 0, 0)
			-- SetPlayerItemLock(nIndex, -2) --kho¸ vÜnh viÔn

-- local nIndexX = AddItem2(2, 0, 3225, 0, 0, 0)
-- local nIndexX = AddItem2(2, 0, 93, 0, 0, 0)
-- Earn(20000000)
-- local nIndexX = AddItemSL(1023,1,0)--27 --313
-- SetPlayerItemLock(nIndexX, -2) --kho¸ vÜnh viÔn
				-- local s_name = GetItemName(nIndexX)
				-- Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>ghÐp thµnh c«ng, nhËn ®­îc <color> "..s_name.." .")
 -- KickOutPlayer("LonelySword") -- 

-- AddItem2(2,0,random(163,166),0,0,0) -- An Bang
-- SetTask(TASK_DUPHONG1, 0)
-- SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + 50000)
-- AddItemSL(2434,1,0)
-- AddItemSL(1126,1,0)
-- local nIndex ={};
			-- for i=167,175 do 
			-- nIndex[i]  = AddItem2(2,0,i,0,0,0) 
			-- SetPlayerItemLock(nIndex[i], -2) --kho¸ vÜnh viÔn
			-- end  
-- ResetAP()             -- Dat lai diem tiem nang
	-- ResetJN()             -- Dat lai diem ky nang
	-- if (GetLevel() < 115) then
		 -- for i = GetLevel(), 115-1 do 
		 -- AddOwnExp(100000000000)
		 -- end
	 -- end
-- for i = 0,8 do 
	-- local nMeridianType = i;
	-- local nMeridianLevel = 16;
	-- SetMeridian(nMeridianType, nMeridianLevel);
-- end
-- SetMeridian(2, 16);
-- SetMeridian(3, 16);
-- AddItemSL(4826,300,0)
-- DelItem(0, -1, 6,1 ,4844, -1, -1, pos_equiproom, 5000)

-- AddItem(4,905,0,0,0,0,0 )

-- AddSkillState(1566, 50, 1,60*60*24*18*30, -1)
-- SetPlayerHonorId(PlayerIndex,53)

-- SetMask(2016)
	-- AddReBorn(1,130)
	-- SetReBornQnPoint()
	-- SetReBornSkill() 
end
function SetReBornQnPoint()


local as={
    {35, 25, 25, 15}, -- Kim
    {20, 35, 20, 25}, -- Moc
    {25, 25, 25, 25}, -- Thuy
    {30, 20, 30, 20}, -- Hoa
    {20, 15, 25, 40}, -- Tho
}

local player_series = GetSeries() + 1;
SetBasePoint(
    as[player_series][1],
    as[player_series][2],
    as[player_series][3],
    as[player_series][4]
);  

local nXiSuiPonit = GetTask(T_TTK) * 5       --- Tay tuy



end;

function SetReBornSkill()

          if (HaveMagic(695)>=0) then

			 DelMagic(695)
          end
		  
		 local  a = GetLevel()             ----Lay cap do hien tai cua nguoi choi
		 local  p = GetRestSP()            ----Lay so diem ky nang con lai
		 local  c = RollbackSkill(0)       ----Xoa toan bo ky nang va tra ve tong diem da su dung
		 local  h = GetTask(T_VLMT)        ----So lan da hoc bi kip cap 50
		 local  r = GetReBornJPoint()      ----Lay diem ky nang giu lai sau khi chuyen sinh
		 local  n = GetReBornQPoint()      ----Lay diem tiem nang giu lai sau khi chuyen sinh
		  --AddMagicPoint(a+h+r-1,0)
		  AddMagicPoint(a-p-1+h+r,1)
          KickOutSelf()                  
end
function doikvthanhxu()
	SayEx({
		"Shop ®æi tiÒn v¹n: muèn ®æi bao nhiªu V¹n xin mêi chän",
		format("§æi 100 v¹n nhËn 5 Xu/#okaydoikv(%d)",1000000),
		format("§æi 1000 v¹n nhËn 50 Xu/#okaydoikv(%d)",10000000),
		format("§æi 2000 v¹n nhËn 100 Xu/#okaydoikv(%d)",20000000),
		--format("10000 ®iÓm tÝch luü nhËn ®iÓm kinh nghiÖm/#okaytichluytk2exp(%d)",10000),
		NOW_END_SAY
	})
end

function okaydoikv(nKv)
	local nLanDoi = 3
	local nDieuKien = 50
	local ketqua = nKv / 10000
	if GetCash() < nKv then
		Talk(1,"","<sex> kh«ng ®ñ "..ketqua.." v¹n mµ muèn lõa ta sao?")
		return
	end
	if GetTask(T_SoNVHoanThanh) > nDieuKien then
	Pay(nKv)
	local SLTien = nKv / 200000
	SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + SLTien)
	

	
	Msg2Player("Xin chóc mõng ®· nhËn ®­îc "..SLTien.." Xu")
	thoigian = tonumber(date("%H%M%d%m"))
			LoginLog = openfile("dulieu/logDoiKvThanhTienXu.txt", "a");
			if LoginLog then
			write(LoginLog,""..GetAccount().." - "..GetName().." -"..GetLevel().."  - Time: "..thoigian.." - So Tien Doi: "..SLTien.." Xu\n");
			end
			closefile(LoginLog)
	else
	Talk(1,"","§¹i HiÖp ph¶i lµm trªn "..nDieuKien.." míi cã thÓ ®æi ®­îc tiÒn V¹n. Sè NhiÖm Vô D· TÈu hiÖn t¹i cña ®¹i hiÖp lµ "..GetTask(T_SoNVHoanThanh).." / "..nDieuKien.." nhiÖm vô!")
	end
end
function diemsukiemdoi()
	local nName = GetName()
	local nW,nX,nY = GetWorldPos()
	local nMapName = GetMapName(nW)	
	local nDSK = GetTask(TASK_DSK)	
	local nChanNguyen = GetTask(TASK_CHANGNGUYENDAN);
	SayEx({format(PLAYER_INFOMATION,nName,nW,nX*32,nY*32,nX,nY,PlayerIndex,nDSK,nChanNguyen),
	"§æi ®iÓm sù kiÖn lÊy QHT/doiqht",	
	"§æi ®iÓm sù kiÖn lÊy HuyÕt Long §¬n/doihldon",	
	"§æi ®iÓm sù kiÖn lÊy LuyÖn Cèt §¬n/doiLcdon",	
	"§æi ®iÓm sù kiÖn lÊy Thøc ¡n Cho Ngùa/doitangua",	
	--"§æi LÖnh Bµi Boss /doilbb",	
	-- "T×m hiÓu ®iÓm sù kiÖn trong ngµy/diemsk",
	"KÕt thóc./no"})
end
function nhanlbb()
	AddItemSL(1023,100,0) -- LB bosss
end
function nhanqht()
	AddSkillState(450, 1, 1, 60*60*24*24*5, -1)
end
function AdminTestTKH()
	SayEx({ADMIN_SUPPORT_TEXT, 
	"B¸o danh bªn Tèng trung cÊp/ditongtc",
	"B¸o danh bªn Kim trung cÊp/dikimtc",
	"HËu doanh phe Tèng/haudoanhphetong",
	"HËu doanh phe Kim/haudoanhphekim",	
	"§µi so¸i phe Tèng/daisoaiphetong",
	"§µi so¸i phe Kim/daisoaiphekim",
	-- "T¾t TK /CloseMissonTK",
	NOW_END_SAY})
end
function mocnapthe1000()
	if GetTask(TASK_NAPTHE) < 2 then
		Talk(1,"","Ng­¬i ch­a nhËn mèc 500k kh«ng thÓ nhËn mèc 1000k  !");
	return end;
	if GetTask(TASK_NAPTHE) >= 3 then
		Talk(1,"","Ng­¬i ®· nhËn phÇn th­ëng nµy råi  !");
	return end;
	if(CheckRoom(4,4) == 0) then
		Talk(1, "", 12266)
	return end;
	if GetTask(T_RUT_KNB) >= 10 then
			mIndex = AddItem(0,10,8,10,0,0,10)
			SetPlayerItemLock(mIndex, -2)
			SetTask(TASK_NAPTHE, GetTask(TASK_NAPTHE) + 1)  -- céng thªm 1 b»ng 2 
			Msg2SubWorld("<color=pink>Chóc mõng<color> <color=yellow>"..GetName().."<color><color=pink> ®· nhËn ®­îc tÝch luü n¹p thÎ <color=yellow>1tr<color> nhËn ®­îc Ngùa Phi V©n kho¸ ! ")
	else
		Talk(1,"","Xin lçi b¹n ch­a tÝch luü n¹p ®ñ 1000k!")
	end
end
function split_lines(str)
    local lines = {}
    local pos = 1
    while (1) do
        local nl = strfind(str, "\n", pos)
        if not nl then
            tinsert(lines, strsub(str, pos))
            break
        end
        tinsert(lines, strsub(str, pos, nl - 1))
        pos = nl + 1
    end
    return lines
end

-- Trim function
function trim(s)
    return gsub(s, "^%s*(.-)%s*$", "%1")
end

function XepHangDuaTopTG()
	local file_path = "C:\\server\\dulieu\\topTG.txt"
	local file = openfile(file_path, "r")
	if not file then
		-- Msg2Player("Cannot open file: " .. file_path)
	else
		-- Msg2Player("opened file: " .. file_path)
	end

	-- Read entire content
	local content = read(file, "*a")
	closefile(file)
	
	--Msg2Player(content)
	-- Process each line
	local lines = split_lines(content)
	local players = {}
	local header_skipped = false
	for i = 1, getn(lines) do
	
		local line = lines[i]
		
		--Msg2Player(i.." "..line)
		local name    = trim(strsub(line, 1, 20))
		local account = trim(strsub(line, 21, 36))
		local level   = trim(strsub(line, 37, 42))
		local Fexp     = trim(strsub(line, 43, 58))
		local ip      = trim(strsub(line, 59))
		if name and account and level and Fexp and ip then
			tinsert(players, {
				name = name,
				account = account,
				level = level,
				Fexp = Fexp,
				ip = ip
			})
		end
	end
	Msg2SubWorld("<color=yellow>Danh S¸ch Top Cao Thñ: ")
	-- Print players
	for i = 1, getn(players) do
		local p = players[i]
		Msg2SubWorld("<color=blue>TOP " .. i .. ": <color=green>" .. p.name ..
           "<color=white> - Level <color=yellow>" .. p.level ..
           "<color=white>, Exp <color=yellow>" .. p.Fexp ..
           "<color=white>, IP <color=cyan>" .. p.ip)
	end
end
function thongbao()
	OpenGetString("NhËp Th«ng b¸o ","CheckCodeND") 
	-- KickOutAccount(lancandar4)
end 
function CheckCodeND()
	local NOIDUNG = GetStringFromUI()
        Msg2SubWorld("<color=Green>" .. NOIDUNG .. "<color>")
end  

function nhangm()
	AddSkillState(160,50,1,2*18,-1)
	AddSkillState(86,50,1,2*18,-1)
	-- SetCurCamp(6)
	-- capgm()
end	

function huygm()
	AddSkillState(160,50,1,2,1)
	AddSkillState(86,50,1,2,1)
	AddSkillState(733,50,1,2,1)
	SetCurCamp(GetCamp())

-- KickOutAccount("phongvan1761")
end
function capgm()
 if (GetLevel() < 90) then
		 for i = GetLevel(), 90-1 do 
		 AddOwnExp(100000000000)
		 end
	 end
	 AddMagic(210, 1)
end
function openresetgolditem()
	OpenGiveBox("§Æt vµo vËt phÈm","- H·y ®¨t trang bÞ hoµng kim muèn tÈy luyÖn vµo !\n- TÈy luyÖn tèn 20 TiÒn §ång.","resetgoldequip")
end

function resetgoldequip()
	local nIndexEquip = 0
	local nIndex = 0
	local nCountEquip = 0
	local i = 0
	local checkUpgrade = 0
	local xTD = 20
	local nTienDong = GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom) -- ®Õm sè KNB trong hµnh trang
	if nTienDong < xTD then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..xTD.." TiÒn §ång.")
		return
	end
	local nResDel = DelItem(0, -1, 6,1 ,4835, -1, -1, ROOME, xTD) -- xo¸ sè knb trong hµnh trang
	if(nResDel >= xTD) then
		for i=0,5 do
			for j=0,3 do
				nIndex = GetROItem(ROOAFFAIR,i,j)
				if (nIndex > 0) then
					nCountEquip = nCountEquip + 1
					nIndexEquip = nIndex
					local result = ResetGoldEquipItem(nIndexEquip)
                    if result and type(result) == "number" and result > 0 then
                        checkUpgrade = 1
                    else
                        Talk(1, "", "VËt phÈm nµy kh«ng hîp lÖ ®Ó tÈy luyÖn.")
                    end
					EndGiveBox()
				end
			end
		end
		if(checkUpgrade == 1) then
			Talk(1,"","§· tÈy luyÖn thµnh c«ng ! ")
		else
			Talk(1,"","H·y bá ®óng vËt phÈm quy ®Þnh !")
		end
	else
		Talk(1, "", "Hµnh trang kh«ng ®ñ "..xTD.." TiÒn §ång.")
	end
end



function openupgradeplatina()
	OpenGiveBox("§Æt vËt phÈm vµo ","Upgrade Platina Equip","updateplatina")
end
function updateplatina()
	local nIndexEquip = 0
	local nIndex = 0
	local nCountEquip = 0
	local i = 0
	local checkUpgrade = 0
	for i=0,5 do
		for j=0,3 do
			nIndex = GetROItem(ROOAFFAIR,i,j)
			if (nIndex > 0) then
				nCountEquip = nCountEquip + 1
				nIndexEquip = nIndex
				UpgradePlatinaItem(nIndexEquip)
				EndGiveBox()
				--logHoatDong(format("[%s]\t%s\tAccount:%s\tName:%s\tTieu huy :%d", " trang bi", GetLocalDate("%Y-%m-%d %X"), GetAccount(), GetName(), nIndexEquip))
				checkUpgrade = 1
			end
		end
	end
	if(checkUpgrade == 1) then
		Talk(1,"","Upgrade ok.")
	else
		Talk(1,"","Chua upgrade")
	end
end

function VaoLanhDiaBangHoi()
    local szTongName = GetTongName()
    if szTongName == "" then
        Talk(1, "", "B¹n ch­a gia nhËp bang héi.")
        return
    end

    if type(MAP_BANG_HOI) ~= "table" then
        Talk(1, "", "Kh«ng t×m thÊy d÷ liÖu bang héi.")
        return
    end

    for i = 1, getn(MAP_BANG_HOI) do
        local tb = MAP_BANG_HOI[i]
        local szBangName = tb[1]
        local nMapID = tb[2]

        if szBangName == szTongName then
            
            if nMapID == 996 then
                NewWorld(996, 1703, 3297) 
            elseif nMapID == 997 then
                NewWorld(997, 1703, 3297) 
            elseif nMapID == 998 then
                NewWorld(998, 1703, 3297) 
            elseif nMapID == 999 then
                NewWorld(999, 1703, 3297) 
            elseif nMapID == 1000 then
                NewWorld(1000, 1703, 3297)
            elseif nMapID == 1001 then
                NewWorld(1001, 1703, 3297)
            elseif nMapID == 1002 then
                NewWorld(1002, 1703, 3297) 
            else
                Talk(1, "", "Kh«ng cã th«ng tin map: " .. nMapID)
            end
            return
        end
    end

    Talk(1, "", "Bang Héi cña b¹n ch­a ®¨ng ký L·nh §Þa.")
end

-- Danh sách map lãnh d?a
TB_CITYID_STUNTID =
{
    [1] = {"Ph­îng T­êng", 996},
    [2] = {"Thµnh §«", 997},
    [3] = {"§¹i Lý", 998},
    [4] = {"BiÖn Kinh", 999},
    [5] = {"T­¬ng D­¬ng", 1000},
    [6] = {"D­¬ng Ch©u", 1001},
    [7] = {"L©m An", 1002},
}

function ShowMapSelectMenu()
    local szTongName = GetTongName()
    if szTongName == "" then
        Talk(1, "", "B¹n ch­a cã bang héi kh«ng thÓ ®¨ng ký l·nh ®Þa!")
        return
    end

    local szBangChu = GetTongInfo(2)
    if GetName() ~= szBangChu then
        Talk(1, "", "B¹n kh«ng ph¶i bang chñ kh«ng thÓ ®¨ng ký l·nh ®Þa!")
        return
    end



	local tbSay = {
    "<color=green><npc>: <color>Muèn ®¨ng ký l·nh ®Þa t¹i thµnh nµo ?",
    "Ph­îng T­êng/DKKV_Map_Index_1",
    "Thµnh §«/DKKV_Map_Index_2",
    "§¹i Lý/DKKV_Map_Index_3",
    "BiÖn Kinh/DKKV_Map_Index_4",
    "T­¬ng D­¬ng/DKKV_Map_Index_5",
    "D­¬ng Ch©u/DKKV_Map_Index_6",
    "L©m An/DKKV_Map_Index_7",
    "Thôi ta di dây./no",
	}
	SayEx(tbSay)
end

function DKKV_Map_Index_1() DKKV_Map_Index(1) end
function DKKV_Map_Index_2() DKKV_Map_Index(2) end
function DKKV_Map_Index_3() DKKV_Map_Index(3) end
function DKKV_Map_Index_4() DKKV_Map_Index(4) end
function DKKV_Map_Index_5() DKKV_Map_Index(5) end
function DKKV_Map_Index_6() DKKV_Map_Index(6) end
function DKKV_Map_Index_7() DKKV_Map_Index(7) end

function DKKV_Map_Index(i)
    local tb = TB_CITYID_STUNTID[i]
    if tb then
        local nMapID = tb[2]
        DKKV_Map(nMapID)
    else
        Talk(1, "", "D÷ liÖu kh«ng hîp lÖ.")
    end
end

function DKKV_Map(nMapID)
    dangkykhuvucbang(nMapID)
end
function dangkykhuvucbang(nMapID)
    local szTongName = GetTongName()
    if szTongName == "" then
        Talk(1, "", "B¹n ch­a cã bang héi kh«ng thÓ thùc hiÖn!")
        return
    end

    local szBangChu = GetTongInfo(2)
    if GetName() ~= szBangChu then
        Talk(1, "", "B¹n kh«ng ph¶i bang chñ kh«ng thÓ ®¨ng ký l·nh ®Þa!")
        return
    end

    if type(MAP_BANG_HOI) ~= "table" then
        MAP_BANG_HOI = {}
    end

    --KiÓm tra
     for i = 1, getn(MAP_BANG_HOI) do
        local v = MAP_BANG_HOI[i]
        if v[1] == szTongName then
            Talk(1, "", "Bang Héi cña b¹n ®· ®¨ng ký l·nh ®Þa.")
            return
        end
        if v[2] == nMapID then
            Talk(1, "", "B¶n ®å nµy ®· ®­îc bang héi kh¸c chiÕm gi÷ .")
            return
        end
    end

   
	local nTime = date("%Y%m%d%H%M%S")
    local nIndex = getn(MAP_BANG_HOI) + 1
	
	
	 local szMapName = "Kh«ng râ "
    for i = 1, getn(TB_CITYID_STUNTID) do
        if TB_CITYID_STUNTID[i][2] == nMapID then
            szMapName = TB_CITYID_STUNTID[i][1]
            break
        end
    end
	MAP_BANG_HOI[nIndex] = { szTongName, nMapID, nTime }
	local BANGI = TaoBang(MAP_BANG_HOI,"MAP_BANG_HOI","")
	SaveData("script/tinhnang/congthanhchien/mapbanghoi.lua", BANGI)
	Msg2SubWorld(format(
        "Bang chñ <color=yellow>%s<color=red> ®· thiÖt lËp thµnh c«ng L·nh §Þa Bang Héi <color=green>%s<color=red> t¹i khu vùc <color=yellow>%s <color>! Thµnh viªn bang héi h·y còng nhau gãp søc cïng ph¸t triÓn l·nh ®Þa cña m×nh !",
        GetName(), szTongName, szMapName
    ))
	-- Msg2SubWorld("Bang chñ <color=yellow>"..GetName().." <color=red> ®· thiÕt lËp thµnh c«ng L·nh §Þa Bang Héi <color=green>"..GetTongName()..". <color=red>Tõ h«m nay, thµnh viªn bang héi h·y còng nhau gãp søc cïng x©y dùng vµ ph¸t triÓn l·nh ®Þa cña m×nh ")

end





function nhaniditemtest()

-- AddItem(6,1,4847,0,0,0,0,0,0,0,0,0,500,0)

OpenGetNumber ("NhËp ID Item" , "nhapiditem")
end

function nhapiditem()
local nIditem = GetNumberFromUI()
	AddItem2(2,0,nIditem,0,0,0,0)
end

function xemnguoichoi()
	local nPCount = GetPlayerCount();
	local nTong = nPCount + 134;
	Msg2Player("<color=pink>§ang Online:<color> "..nTong..".")
end

function TaiLanhDao()
	AddLeadExp(1000000000)
	Talk(1,"","Xin chóc mõng <sex> ®· nhËn ®­îc ®iÓm l·nh ®¹o .")
end

function tangncapadmin(nLevel)
	local maxLevel = 200
	if(GetLevel() + nLevel > maxLevel) then
		nLevel = maxLevel - GetLevel()
		LEVERUP(nLevel)
	else
		LEVERUP(nLevel)
	end
	if(nLevel > 0) then
		Talk(1,"","Xin chóc mõng <sex> ®· t¨ng thªm "..nLevel.." cÊp")
	else
		Talk(1,"","<sex> ®· ®¹t ®­îc ®¼ng cÊp th­îng thõa råi")
	end
end

function LEVERUP(nLevel)
	for i = 1, nLevel do 
		AddOwnExp(100000000000)
	end
end

function doixuthanhkv()
	SayEx({
		"Shop ®æi tiÒn v¹n: muèn ®æi bao nhiªu xu xin mêi chän",
		format("§æi 10 Xu nhËn 50 V¹n/#okaydoixu(%d)",10),
		format("§æi 100 Xu nhËn 500 V¹n/#okaydoixu(%d)",100),
		format("§æi 200 Xu nhËn 1000 V¹n/#okaydoixu(%d)",200),
		--format("10000 ®iÓm tÝch luü nhËn ®iÓm kinh nghiÖm/#okaytichluytk2exp(%d)",10000),
		NOW_END_SAY
	})
end

function okaydoixu(nXu)
	local nLanDoi = 5
	local nDieuKien = 50
	if(GetTask(T_PLAYER_XU) < nXu) then
		Talk(1,"","<sex> kh«ng ®ñ "..nXu.." Xu mµ muèn lõa ta sao?")
		return
	end
	if(GetTask(TASK_DUPHONG1) >= nLanDoi) then
		Talk(1,"","<sex> H«m nay §¹i HiÖp ®· ®æi "..nLanDoi.." lÇn kh«ng thÓ ®æi thªm n÷a !")
		return
	end
	if GetTask(T_SoNVHoanThanh) > nDieuKien then
	SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) -  nXu)
	local SLTien = nXu * 50000
	Earn(SLTien) 
	SetTask(TASK_DUPHONG1, GetTask(TASK_DUPHONG1) + 1)
	local ketqua = SLTien/10000
	Msg2Player("Xin chóc mõng ®· nhËn ®­îc "..ketqua.." V¹n")
	thoigian = tonumber(date("%H%M%d%m"))
			LoginLog = openfile("dulieu/logDoiXuThanhTienVan.txt", "a");
			if LoginLog then
			write(LoginLog,""..GetAccount().." - "..GetName().." -"..GetLevel().."  - Time: "..thoigian.." - So Tien Doi: "..ketqua.." Van\n");
			end
			closefile(LoginLog)
	else
	Talk(1,"","§¹i HiÖp ph¶i lµm trªn "..nDieuKien.." míi cã thÓ ®æi ®­îc tiÒn V¹n. Sè NhiÖm Vô D· TÈu hiÖn t¹i cña ®¹i hiÖp lµ "..GetTask(T_SoNVHoanThanh).." / "..nDieuKien.." nhiÖm vô!")
	end
end

function topTG1()
if (GetName() == "NhÊtTiÔnSong§iªu" )then
if CheckFreeBoxItem(4,364,1,1) == 0 then
	Talk(1,"","Hµnh trang ph¶i ®Ó trèng 6 x 5 « míi cã thÓ nhËn !")
	return
end
if GetTask(TASK_TANTHU4) >= 1 then
	Talk(1,"","Ng­¬i ®· nhËn råi kh«ng thÓ nhËn thªm n÷a !")
	return
end
	if (GetLevel() < 85) then
		Talk(1,"","Ng­êi ch¬i ph¶i ®¹t ®¼ng cÊp 90 trë lªn míi cã thÓ nhËn.")
		return
	end

		SetTask(TASK_TANTHU4,1)
		itemidx = AddItem(0,10,5,10,0,0,10)
		-- SetPlayerItemLock(itemidx, -2)
		AddEventItem(195,1)
		SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + 1000)
			for i=168,176 do 
			mIndex = AddGoldItem(i,0) 
			SetPlayerItemLock(mIndex, -2)
			-- AddTimeItem(mIndex,60*60*24*7);  -- thêi gia item 7 ngµy
			end 
		-- SetHSD(itemidx, 2023 , thangvp, ngayvp, gio)
       Msg2SubWorld("<color=blue> Chóc mõng <color> <color=yellow>"..GetName().." <color=blue> ®· nhËn ®­îc<color=white> Th­ëng TOP 1 <color> ")
else
Talk(1,"","Ng­¬i kh«ng cã trong danh s¸ch TOP, vui lßng kiÓm tra l¹i")
	end
end
function topTG2()
if (GetName() == "Mr‘Roooo" )then
if CheckFreeBoxItem(4,364,1,1) == 0 then
	Talk(1,"","Hµnh trang ph¶i ®Ó trèng 6 x 5 « míi cã thÓ nhËn !")
	return
end
if GetTask(TASK_TANTHU4) >= 1 then
	Talk(1,"","Ng­¬i ®· nhËn råi kh«ng thÓ nhËn thªm n÷a !")
	return
end
	if (GetLevel() < 85) then
		Talk(1,"","Ng­êi ch¬i ph¶i ®¹t ®¼ng cÊp 90 trë lªn míi cã thÓ nhËn.")
		return
	end

		SetTask(TASK_TANTHU4,1)
		itemidx = AddItem(0,10,5,7,0,0,10)
		-- SetPlayerItemLock(itemidx, -2)
		AddEventItem(195,1)
			for i=168,176 do 
			mIndex = AddGoldItem(i,0) 
			SetPlayerItemLock(mIndex, -2)
			AddTimeItem(mIndex,60*60*24*7);  -- thêi gia item 7 ngµy
			end 
		SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + 700)
		-- SetHSD(itemidx, 2023 , thangvp, ngayvp, gio)
       Msg2SubWorld("<color=blue> Chóc mõng <color> <color=yellow>"..GetName().." <color=blue> ®· nhËn ®­îc<color=white> Th­ëng TOP 2 <color> ")
else
Talk(1,"","Ng­¬i kh«ng cã trong danh s¸ch TOP, vui lßng kiÓm tra l¹i")
	end
end
function topTG3()
if (GetName() == "HoµiÙNiÖm" )then
if CheckFreeBoxItem(4,364,1,1) == 0 then
	Talk(1,"","Hµnh trang ph¶i ®Ó trèng 6 x 5 « míi cã thÓ nhËn !")
	return
end
if GetTask(TASK_TANTHU4) >= 1 then
	Talk(1,"","Ng­¬i ®· nhËn råi kh«ng thÓ nhËn thªm n÷a !")
	return
end
	if (GetLevel() < 85) then
		Talk(1,"","Ng­êi ch¬i ph¶i ®¹t ®¼ng cÊp 90 trë lªn míi cã thÓ nhËn.")
		return
	end

		SetTask(TASK_TANTHU4,1)
		itemidx = AddItem(0,10,5,7,0,0,10)
		-- SetPlayerItemLock(itemidx, -2)
			for i=168,176 do 
			mIndex = AddGoldItem(i,0) 
			SetPlayerItemLock(mIndex, -2)
			AddTimeItem(mIndex,60*60*24*5);  -- thêi gia item 7 ngµy
			end 
		SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + 300)
		-- SetHSD(itemidx, 2023 , thangvp, ngayvp, gio)
       Msg2SubWorld("<color=blue> Chóc mõng <color> <color=yellow>"..GetName().." <color=blue> ®· nhËn ®­îc<color=white> Th­ëng TOP 3 <color> ")
else
Talk(1,"","Ng­¬i kh«ng cã trong danh s¸ch TOP, vui lßng kiÓm tra l¹i")
	end
end

function topTG4()
if (GetName() == "...DÙM..." ) or (GetName() == "HoaLong" ) or (GetName() == "Ä•A±TÔu•Ä" ) or (GetName() == "MocLongThan" ) or (GetName() == "T¨ngNg­u" ) or (GetName() == "BuffÙBua" ) or (GetName() == "L·oPhËtGia" ) or (GetName() == "" ) or (GetName() == "" ) or (GetName() == "" ) then

if CheckFreeBoxItem(4,364,1,1) == 0 then
	Talk(1,"","Hµnh trang ph¶i ®Ó trèng 6 x 5 « míi cã thÓ nhËn !")
	return
end
if GetTask(TASK_TANTHU4) >= 1 then
	Talk(1,"","Ng­¬i ®· nhËn råi kh«ng thÓ nhËn thªm n÷a !")
	return
end
	if (GetLevel() < 84) then
		Talk(1,"","Ng­êi ch¬i ph¶i ®¹t ®¼ng cÊp 90 trë lªn míi cã thÓ nhËn.")
		return
	end

		SetTask(TASK_TANTHU4,1)
		itemidx = AddItem(0,10,5,random(1,4),0,0,10)
		-- SetPlayerItemLock(itemidx, -2)
		SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + 200)

       Msg2SubWorld("<color=blue> Chóc mõng <color> <color=yellow>"..GetName().." <color=blue> ®· nhËn ®­îc<color=white> Th­ëng TOP 4 - 10 <color> ")
else
Talk(1,"","Ng­¬i kh«ng cã trong danh s¸ch TOP, vui lßng kiÓm tra l¹i")
	end
end
function nhanlaikinhkhong()
	AddMagic(210,1)
	-- OpenResetPass()
end

function mocnapthe()
	if GetTask(TASK_NAPTHE) > 0 then
		Talk(1,"","Ng­¬i ®· nhËn phÇn th­ëng nµy råi  !");
	return end;
	if(CheckRoom(4,4) == 0) then
		Talk(1, "", 12266)
	return end;
	if GetTask(T_RUT_KNB) >= 2 then
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do
			AddItemSL(4844,500,-2) -- ho mach do

			AddItemSL(4847,500,-2)  -- chan nguyen don
			AddItemSL(4847,500,-2)  -- chan nguyen don
			AddItemSL(4847,500,-2)  -- chan nguyen don
			AddItemSL(4847,500,-2)  -- chan nguyen don
			AddItemSL(4847,500,-2)  -- chan nguyen don
			AddItemSL(4847,500,-2)  -- chan nguyen don
			SetTask(TASK_NAPTHE, GetTask(TASK_NAPTHE) + 1)
			Msg2Player("<color=pink>Chóc mõng<color> <color=yellow>"..GetName().."<color><color=pink> ®· nhËn ®­îc tÝch luü n¹p thÎ <color=yellow>200k<color> nhËn ®­îc nhiªu item gi÷ tri ! ")
	else
		Talk(1,"","Xin lçi b¹n ch­a tÝch luü n¹p ®ñ 200k!")
	end
end
FREECELL_TANTHU = 50
function mocnapthe500()
local nIndex ={};
	if CalcFreeItemCellCount() < FREECELL_TANTHU then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ "..FREECELL_TANTHU.." « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return
		end	
	if GetTask(TASK_NAPTHE) < 1 then
		Talk(1,"","Ng­¬i ch­a nhËn mèc 500k kh«ng thÓ nhËn mèc 500k  !");
	return end;
	if GetTask(TASK_NAPTHE) >= 2 then
		Talk(1,"","Ng­¬i ®· nhËn phÇn th­ëng nµy råi  !");
	return end;
	-- if(CheckRoom(4,4) == 0) then
		-- Talk(1, "", 12266)
	-- return end;
	if GetTask(T_RUT_KNB) >= 5 then
			for i=167,175 do 
			nIndex[i]  = AddItem2(2,0,i,0,0,0) 
			-- AddTimeItem(nIndex[i],60*60*24*30);  -- thêi gia item 30 ngµy
			SetPlayerItemLock(nIndex[i], -2) --kho¸ vÜnh viÔn
			end  
			SetTask(TASK_NAPTHE, GetTask(TASK_NAPTHE) + 1)  -- céng thªm 1 b»ng 2 
			Msg2Player("<color=pink>Chóc mõng<color> <color=yellow>"..GetName().."<color><color=pink> ®· nhËn ®­îc tÝch luü n¹p thÎ <color=yellow>500K<color> nhËn ®­îc Set Thiªn Hoµng kho¸ ! ")
	else
		Talk(1,"","Xin lçi b¹n ch­a tÝch luü n¹p ®ñ 500k!")
	end
end
function mocnapthe2000()
    local factionName = GetFaction()
    local factionIdx = -1
    if factionName == "ÉÙÁÖÅÉ" then factionIdx = 0
    elseif factionName == "ÌìÍõ°ï" then factionIdx = 1
    elseif factionName == "ÌÆÃÅ" then factionIdx = 2
    elseif factionName == "Îå¶¾½Ì" then factionIdx = 3
    elseif factionName == "¶ëáÒÅÉ" then factionIdx = 4
    elseif factionName == "´äÑÌÃÅ" then factionIdx = 5
    elseif factionName == "Ø¤°ï" then factionIdx = 6
    elseif factionName == "ÌìÈÌ½Ì" then factionIdx = 7
    elseif factionName == "Îäµ±ÅÉ" then factionIdx = 8
    elseif factionName == "À¥ÂØÅÉ" then factionIdx = 9 
	end

    if factionIdx == -1 then
        Msg2Player("M«n ph¸i kh«ng hî trî nhËn th­ëng!")
        return
    end

    SetTaskTemp(2024, factionIdx)  

 local tbPhai = {
    [0] = {"ThiÕu L©m QuyÒn/chon1", "ThiÕu L©m §ao/chon2", "ThiÕu L©m Bæng/chon3"},
    [1] = {"Thiªn V­¬ng Th­¬ng/chon1", "Thiªn V­¬ng Chïy/chon2", "Thiªn V­¬ng §ao/chon3"},
    [2] = {"§­êng M«n Ná./chon1", "§­êng M«n Phi §ao/chon2", "§­êng M«n Phi Tiªu/chon3"},
    [3] = {"Ngò §éc §ao/chon1", "Ngò §éc Ch­ëng/chon2"},
    [4] = {"Nga My KiÕm/chon1", "Nga My Ch­ëng/chon2"},
    [5] = {"Thóy Yªn §ao/chon1", "Thóy Yªn Song §ao/chon2"},
    [6] = {"C¸i Bang Bæng/chon1", "C¸i Bang Rång/chon2"},
    [7] = {"Thiªn NhÉn KÝch/chon1", "Thiªn NhÉn Néi C«ng/chon2"},
    [8] = {"Vâ §ang KiÕm/chon1", "Vâ §ang KhÝ T«ng/chon2"},
    [9] = {"C«n L«n §ao/chon1", "C«n L«n KhÝ/chon2"}
}

	local tbOption = {"Chµo <sex> <color=red><player><color> tÝch lòy n¹p cña b¹n lµ <color=yellow>"..GetTask(T_RUT_KNB) .."<color> Kim Nguyªn B¶o",}
    for i = 1, getn(tbPhai[factionIdx]) do
        tinsert(tbOption, tbPhai[factionIdx][i])
    end
    tinsert(tbOption, "Ta chØ xem qua th«i./no")
    SayEx(tbOption)

end
function chon1()
    local fac = GetTaskTemp(2024)
    NhapMoc3000k(fac, 1)
end

function chon2()
    local fac = GetTaskTemp(2024)
    NhapMoc3000k(fac, 2)
end

function chon3()
    local fac = GetTaskTemp(2024)
    NhapMoc3000k(fac, 3)
end


function NhapMoc3000k(factionIdx, skillLine)
    local tbItemData = {
        [0] = { [1] = {0},   [2] = {12},  [3] = {7} },-- ThiÕu L©m : QuyÒn , §ao , Bæng
        [1] = { [1] = {22},  [2] = {19},  [3] = {28} }, --Thiªn V­¬ng : Th­¬ng ; Chïy, §ao
        [2] = { [1] = {78},  [2] = {71},  [3] = {81} },--§­êng M«n : §Þa Ph¸ch , Phi §ao , Phi Tiªu
        [3] = { [1] = {63},  [2] = {57} }, --Ngò §éc : §ao ;Ch­ëng 
        [4] = { [1] = {32},  [2] = {35} },-- Ngamy: KiÕm , Ch­ëng
        [5] = { [1] = {46},  [2] = {52} },--Thóy Yªn : §ao , Song §ao
        [6] = { [1] = {97},  [2] = {90} },-- C¸i Bang : Bæng , Rång
        [7] = { [1] = {101}, [2] = {109} },--Thiªn NhÉn : Th­¬ng, Thiªn ngo¹i
        [8] = { [1] = {122}, [2] = {117} },--Vâ §ang KiÕm , KhÝ
        [9] = { [1] = {126}, [2] = {131} }--C«n L«n : §ao , KhÝ
    }
	if GetTask(TASK_NAPTHE) < 3 then
		Talk(1,"","Ng­¬i ch­a nhËn mèc 1000k kh«ng thÓ nhËn mèc 2000k  !");
	return end;
	if GetTask(TASK_NAPTHE) >= 4 then
		Talk(1,"","Ng­¬i ®· nhËn phÇn th­ëng nµy råi  !");
	return end;
	if CalcFreeItemCellCount() < FREECELL_TANTHU then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ "..FREECELL_TANTHU.." « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return
		end	
	
	if GetTask(T_RUT_KNB) >= 20 then
	   local tb = tbItemData[factionIdx]
		if tb and tb[skillLine] then
			for i = 1, getn(tb[skillLine]) do
			mIndex = AddItem2(2, 0, tb[skillLine][i], 0, 0, 0)
			SetPlayerItemLock(mIndex, -2)
			local s_name = GetItemName(mIndex)
			SetTask(TASK_NAPTHE, GetTask(TASK_NAPTHE) + 1)  -- céng thªm 1 b»ng 2 
			Msg2SubWorld("<color=yellow>Chóc mõng<color><color=green> " .. GetName() .. " <color><color=yellow>®· nhËn ®­îc tÝch luü n¹p thÎ <color><color=green>2tr<color>, <color=yellow>nhËn ®­îc <color> "..s_name.." .")
			end
			Msg2Player("§· nhËn phÇn th­ëng mèc n¹p 2000K!")
		else
			Msg2Player("Kh«ng t×m thÊy d÷ liÖu vËt phÈm cho lùa chän nµy.")
		end
	else
		Talk(1,"","Xin lçi b¹n ch­a tÝch luü n¹p ®ñ 2000k!")
	end
	
end
function xemdiemTTK()
	local VLMT = GetTask(T_VLMT)
	local TTK = GetTask(T_TTK)
	
SayEx({format(PLAYER_DIEM,VLMT,TTK),
"KÕt thóc./no"})
end


function chuyenphai()

Say("Vui lßng chän m«n ph¸i cÇn chuyÓn:\nL­u ý: <color=yellow>ChuyÓn qua HÖ Kim tù ®éng ®æi giíi tÝnh Nam, chuyÓn qua HÖ Thñy tù ®éng chuyÓn giíi tÝnh n÷ <color> ",13,
"ThiÕu L©m /doiphai1",
"Thiªn V­¬ng /doiphai1",
"§­êng M«n /doiphai1",
"Ngò §éc /doiphai1",
"Nga My /doiphai1",
"Thóy Yªn /doiphai1",
"C¸i Bang /doiphai1",
"Thiªn NhÉn /doiphai1",
"Vâ §ang /doiphai1",
"C«n L«n /doiphai1",
"Hoa S¬n /doiphai1",
"Vò Hån /doiphai1",
"Tiªu Dao /doiphai1") -- [HOASON 01/09] [VHTD 02/09] 13 phai
end

function doiphai1(nsel)
local nEqcount=GetEquipCount()
 if (nEqcount>0) then
	  Talk(1,"",13014)
	  return
 end
 if (0 == GetCamp() ) or (4 == GetCamp() ) then
		Talk(1,"","<sex> ch­a gia nhËp m«n ph¸i hoÆt ch÷ ®á , kh«ng thÓ ®æi ph¸i . ")
		return
	end

 
local KNBDP = 0
local KNB = GetItemCount(0,6,1, 4835, -1, -1, pos_equiproom) -- ®Õm sè tien dong  trong hµnh trang
	if KNB < KNBDP then
		Talk(1, "", "Kh¸ch quan kh«ng mang ®ñ "..KNBDP.." TiÒn §ång.")
		return
	end
DelItem(0, -1, 6,1 ,4835, -1, -1, ROOME, KNBDP) -- xo¸ sè tien dong trong hµnh trang

local lv = GetLevel();
local nCurFac = nsel+1;
local nLastFacId = GetFactionNo() + 1
local nSkill120Level,nSkill120Exp = GetMagicLevel(SKILL120AR[nLastFacId])

local tbSkill90Old = {}
for i = 1, getn(SKILL90_ARRAY[nLastFacId]) do
	local tbSkill = SKILL90_ARRAY[nLastFacId][i]
	if tbSkill[3] == 1 then
		local nId = tbSkill[2]
		local nLv,nExp = GetMagicLevel(nId)
		if nLv > 0 then
			tinsert(tbSkill90Old, {level = nLv, exp = nExp})
		end
	end
end
local tbSkill15Old = {}
for i = 1, getn(SKILL150_ARRAY[nLastFacId]) do
	local tbSkill = SKILL150_ARRAY[nLastFacId][i]
	if tbSkill[3] == 1 then
		local nId = tbSkill[2]
		local nLv,nExp = GetMagicLevel(nId)
		if nLv > 0 then
			tinsert(tbSkill15Old, {level = nLv, exp = nExp})
		end
	end
end

DelAllMagic()         -- Xoa tat ca ky nang
ClearFactionIfnfo()   -- Xoa thong tin mon phai
	
if nCurFac == 1 then
	SetSeries(0)
	SetSex(0)
	SetFaction(FACTION_INFO[nCurFac][2])
	SetCamp(FACTION_INFO[nCurFac][3])
	SetCurCamp(FACTION_INFO[nCurFac][3])
	SetRank(FACTION_INFO[nCurFac][4])
	hockynang(nCurFac)
elseif nCurFac == 2 then
	SetSeries(0)
	SetSex(0)
	SetFaction(FACTION_INFO[nCurFac][2])
	SetCamp(FACTION_INFO[nCurFac][3])
	SetCurCamp(FACTION_INFO[nCurFac][3])
	SetRank(FACTION_INFO[nCurFac][4])
	hockynang(nCurFac)
elseif nCurFac == 3 then
	SetSeries(1)
	SetFaction(FACTION_INFO[nCurFac][2])
	SetCamp(FACTION_INFO[nCurFac][3])
	SetCurCamp(FACTION_INFO[nCurFac][3])
	SetRank(FACTION_INFO[nCurFac][4])
	hockynang(nCurFac)
elseif nCurFac == 4 then
	SetSeries(1)
	SetFaction(FACTION_INFO[nCurFac][2])
	SetCamp(FACTION_INFO[nCurFac][3])
	SetCurCamp(FACTION_INFO[nCurFac][3])
	SetRank(FACTION_INFO[nCurFac][4])
	hockynang(nCurFac)
elseif nCurFac == 5 then
	SetSeries(2)
	SetSex(1)
	SetFaction(FACTION_INFO[nCurFac][2])
	SetCamp(FACTION_INFO[nCurFac][3])
	SetCurCamp(FACTION_INFO[nCurFac][3])
	SetRank(FACTION_INFO[nCurFac][4])
	hockynang(nCurFac)
elseif nCurFac == 6 then
	SetSeries(2)
	SetSex(1)
	SetFaction(FACTION_INFO[nCurFac][2])
	SetCamp(FACTION_INFO[nCurFac][3])
	SetCurCamp(FACTION_INFO[nCurFac][3])
	SetRank(FACTION_INFO[nCurFac][4])
	hockynang(nCurFac)
elseif nCurFac == 7 then
	SetSeries(3)
	SetFaction(FACTION_INFO[nCurFac][2])
	SetCamp(FACTION_INFO[nCurFac][3])
	SetCurCamp(FACTION_INFO[nCurFac][3])
	SetRank(FACTION_INFO[nCurFac][4])
	hockynang(nCurFac)
elseif nCurFac == 8 then
	SetSeries(3)
	SetFaction(FACTION_INFO[nCurFac][2])
	SetCamp(FACTION_INFO[nCurFac][3])
	SetCurCamp(FACTION_INFO[nCurFac][3])
	SetRank(FACTION_INFO[nCurFac][4])
	hockynang(nCurFac)
elseif nCurFac == 9 then
	SetSeries(4)
	SetFaction(FACTION_INFO[nCurFac][2])
	SetCamp(FACTION_INFO[nCurFac][3])
	SetCurCamp(FACTION_INFO[nCurFac][3])
	SetRank(FACTION_INFO[nCurFac][4])
	hockynang(nCurFac)
elseif nCurFac == 10 then
	SetSeries(4)
	SetFaction(FACTION_INFO[nCurFac][2])
	SetCamp(FACTION_INFO[nCurFac][3])
	SetCurCamp(FACTION_INFO[nCurFac][3])
	SetRank(FACTION_INFO[nCurFac][4])
	hockynang(nCurFac)
elseif nCurFac == 11 then -- Hoa Son (he Thuy)
	SetSeries(2)
	SetFaction(FACTION_INFO[nCurFac][2])
	SetCamp(FACTION_INFO[nCurFac][3])
	SetCurCamp(FACTION_INFO[nCurFac][3])
	SetRank(FACTION_INFO[nCurFac][4])
	hockynang(nCurFac)
elseif nCurFac == 12 then -- Vu Hon (he Hoa) [VHTD 02/09]
	SetSeries(3)
	SetFaction(FACTION_INFO[nCurFac][2])
	SetCamp(FACTION_INFO[nCurFac][3])
	SetCurCamp(FACTION_INFO[nCurFac][3])
	SetRank(FACTION_INFO[nCurFac][4])
	hockynang(nCurFac)
elseif nCurFac == 13 then -- Tieu Dao (he Tho) [VHTD 02/09]
	SetSeries(4)
	SetFaction(FACTION_INFO[nCurFac][2])
	SetCamp(FACTION_INFO[nCurFac][3])
	SetCurCamp(FACTION_INFO[nCurFac][3])
	SetRank(FACTION_INFO[nCurFac][4])
	hockynang(nCurFac)
end	

local tbSkillNew = SKILL90_ARRAY[nCurFac]
local iTransfer = 1
for i = 1, getn(tbSkillNew) do
	local tbNew = tbSkillNew[i]
	if tbNew[3] == 1 and tbSkill90Old[iTransfer] then
		local nSkillId = tbNew[2]
		local nLv = tbSkill90Old[iTransfer].level
		local nExp = tbSkill90Old[iTransfer].exp
		AddMagic(nSkillId, nLv)
		-- IncSkillExp(nSkillId, nExp)
		iTransfer = iTransfer + 1
	end
end
local tbSkillNew150 = SKILL150_ARRAY[nCurFac]
local iTransfer = 1
for i = 1, getn(tbSkillNew150) do
	local tbNew = tbSkillNew150[i]
	if tbNew[3] == 1 and tbSkill15Old[iTransfer] then
		local nSkillId = tbNew[2]
		local nLv = tbSkill15Old[iTransfer].level
		local nExp = tbSkill15Old[iTransfer].exp
		AddMagic(nSkillId, nLv)
		-- IncSkillExp(nSkillId, nExp)
		iTransfer = iTransfer + 1
	end
end
	
	for i=1,getn(SKILL90_ARRAY[nCurFac])  do	
		if(SKILL90_ARRAY[nCurFac][i][3] == 0) then
		AddMagic(SKILL90_ARRAY[nCurFac][i][2],0);
		end
	end

	if (nSkill120Level > 0) then
		AddMagic(SKILL120AR[nCurFac], nSkill120Level)
		IncSkillExp(SKILL120AR[nCurFac], nSkill120Exp)
	end
	Msg2SubWorld("<color=pink>Chóc mõng<color> <color=yellow>"..GetName().."<color><color=pink> ®· chuyÓn ph¸i thµnh c«ng! Giang hå s¾p dËy lªn mét phen sãng giã ")
	
	SetFightState(0)
	ResetAP()             -- Dat lai diem tiem nang
	ResetJN()             -- Dat lai diem ky nang

end
function ResetAP()

local as={
{35,25,25,15},
{20,35,20,25},
{25,25,25,25},
{30,20,30,20},
{20,15,25,40},
}

player_series = GetSeries() + 1;
SetBasePoint(as[player_series][1],as[player_series][2],as[player_series][3],as[player_series][4]);  

local nXiSuiPonit=GetTask(T_TTK)*5        
AddProp(nXiSuiPonit)

ResetBaseAttrib(0,as[player_series][1])
ResetBaseAttrib(1,as[player_series][2])
ResetBaseAttrib(2,as[player_series][3])
ResetBaseAttrib(3,as[player_series][4])

end;
function ResetJN()

	      local  a = GetLevel()       
          local  p = GetRestSP()          
		  local  c = RollbackSkill(0)		
          local  h = GetTask(T_VLMT)			
		  local  r = GetReBornJPoint()  
		  local  n = GetReBornQPoint()   
		  AddMagicPoint(a-p-1+h+r,1)
         			   
          KickOutSelf()                 
end;


function xemdiem()
	local EVENTCT = GetTask(TASK_EVENT2011_1)
	local EVENTPT = GetTask(TASK_EVENT2011_2)
	local EVENTKS = GetTask(TASK_EVENT2011_3)	
SayEx({format(PLAYER_EVENT,EVENTCT/100000,EVENTPT/100000,EVENTKS/100000),
"KÕt thóc./no"})
end

function diemsk()
Talk(1, "", "Tham gia ho¹t ®éng Tèng kim ®­îc <color=green>50 <color> ®iÓm, PL§ <color=green>20<color> ®iÓm, D· TÈu hoµn thµnh 40 nhiÖm vô ®­îc <color=green>100<color> ®iÓm.")
end
function codegame()
	Talk(1, "", "GiftCode <color=green>VLNGAOTHE <color>.")
end

function doiqht()
	SayEx({
		"Shop §iÓm Sù KiÖn: muèn ®æi bao nhiªu ®iÓm xin mêi chän",
		format("§æi 300 ®iÓm sù kiÖn nhËn 1 QuÕ Hoa Töu/#okaytichluyqht(%d)",300),
		format("§æi 1500 ®iÓm sù kiÖn nhËn 5 QuÕ Hoa Töu/#okaytichluyqht(%d)",1500),
		format("§æi 3000 ®iÓm sù kiÖn nhËn 10 QuÕ Hoa Töu/#okaytichluyqht(%d)",3000),
		--format("10000 ®iÓm tÝch luü nhËn ®iÓm kinh nghiÖm/#okaytichluytk2exp(%d)",10000),
		NOW_END_SAY
	})
end

function okaytichluyqht(nTichLuy)
	if(GetTask(TASK_DSK) < nTichLuy) then
		Talk(1,"","<sex> kh«ng ®ñ ®iÓm "..nTichLuy.." tÝch luü mµ muèn lõa ta sao?")
		return
	end
	SetTask(TASK_DSK, GetTask(TASK_DSK) - nTichLuy)
	local SLQHT = nTichLuy / 300
	for i=1,SLQHT do
	local itemIdx = AddItem(6,1,124,0,0,0,0) --- QHT
	SetPlayerItemLock(itemIdx, -2)
	end
	Msg2Player("Xin chóc mõng ®· nhËn ®­îc "..SLQHT.." QuÕ Hoa Töu")
end
function doihldon()
	SayEx({
		"Shop §iÓm Sù KiÖn: muèn ®æi bao nhiªu ®iÓm xin mêi chän",
		format("§æi 30 ®iÓm sù kiÖn nhËn 1 HuyÕt Long §¬n/#okaytichluyhldon(%d)",30),
		format("§æi 150 ®iÓm sù kiÖn nhËn 5 HuyÕt Long §¬n/#okaytichluyhldon(%d)",150),
		format("§æi 300 ®iÓm sù kiÖn nhËn 10 HuyÕt Long §¬n/#okaytichluyhldon(%d)",300),
		format("§æi 3000 ®iÓm sù kiÖn nhËn 100 HuyÕt Long §¬n/#okaytichluyhldon(%d)",3000),
		--format("10000 ®iÓm tÝch luü nhËn ®iÓm kinh nghiÖm/#okaytichluytk2exp(%d)",10000),
		NOW_END_SAY
	})
end

function okaytichluyhldon(nTichLuy)
	if(GetTask(TASK_DSK) < nTichLuy) then
		Talk(1,"","<sex> kh«ng ®ñ ®iÓm "..nTichLuy.." tÝch luü mµ muèn lõa ta sao?")
		return
	end
	SetTask(TASK_DSK, GetTask(TASK_DSK) - nTichLuy)
	local SLHLD = nTichLuy / 30
	AddItemSL(4849,SLHLD,-2)--- --- Huyet Long Don
	Msg2Player("Xin chóc mõng ®· nhËn ®­îc "..SLHLD.." HuyÕt Long §¬n")
end
function doiLcdon()
	SayEx({
		"Shop §iÓm Sù KiÖn: muèn ®æi bao nhiªu ®iÓm xin mêi chän",
		format("§æi 3 ®iÓm sù kiÖn nhËn 1 LuyÖn Cèt §¬n/#okaytichluyLcdon(%d)",3),
		format("§æi 15 ®iÓm sù kiÖn nhËn 5 LuyÖn Cèt §¬n/#okaytichluyLcdon(%d)",15),
		format("§æi 30 ®iÓm sù kiÖn nhËn 10 LuyÖn Cèt §¬n/#okaytichluyLcdon(%d)",30),
		format("§æi 300 ®iÓm sù kiÖn nhËn 100 LuyÖn Cèt §¬n/#okaytichluyLcdon(%d)",300),
		--format("10000 ®iÓm tÝch luü nhËn ®iÓm kinh nghiÖm/#okaytichluytk2exp(%d)",10000),
		NOW_END_SAY
	})
end

function okaytichluyLcdon(nTichLuy)
	if(GetTask(TASK_DSK) < nTichLuy) then
		Talk(1,"","<sex> kh«ng ®ñ ®iÓm "..nTichLuy.." tÝch luü mµ muèn lõa ta sao?")
		return
	end
	SetTask(TASK_DSK, GetTask(TASK_DSK) - nTichLuy)
	local SLLCD = nTichLuy / 3
	AddItemSL(4728,SLLCD,-2)--- Luyen Cot Don

	Msg2Player("Xin chóc mõng ®· nhËn ®­îc "..SLLCD.." LuyÖn Cèt §¬n")
end
function doitangua()
	SayEx({
		"Shop §iÓm Sù KiÖn: muèn ®æi bao nhiªu ®iÓm xin mêi chän",
		format("§æi 3 ®iÓm sù kiÖn nhËn 1 Thøc ¡n Cho Ngùa/#okaytichluytangua(%d)",3),
		format("§æi 15 ®iÓm sù kiÖn nhËn 5 Thøc ¡n Cho Ngùan/#okaytichluytangua(%d)",15),
		format("§æi 30 ®iÓm sù kiÖn nhËn 10 Thøc ¡n Cho Ngùa/#okaytichluytangua(%d)",30),
		format("§æi 300 ®iÓm sù kiÖn nhËn 100 Thøc ¡n Cho Ngùa/#okaytichluytangua(%d)",300),
		--format("10000 ®iÓm tÝch luü nhËn ®iÓm kinh nghiÖm/#okaytichluytk2exp(%d)",10000),
		NOW_END_SAY
	})
end

function okaytichluytangua(nTichLuy)
	if(GetTask(TASK_DSK) < nTichLuy) then
		Talk(1,"","<sex> kh«ng ®ñ ®iÓm "..nTichLuy.." tÝch luü mµ muèn lõa ta sao?")
		return
	end
	SetTask(TASK_DSK, GetTask(TASK_DSK) - nTichLuy)
	local SLLCD = nTichLuy / 3
	AddItemSL(3846,SLLCD,-2) --- Thøc ¡n Ngùa
	Msg2Player("Xin chóc mõng ®· nhËn ®­îc "..SLLCD.." Thøc ¡n Cho Ngùa")
end

function doilbb()
	SayEx({
		"Shop ®æi tiÒn v¹n: muèn ®æi bao nhiªu xu xin mêi chän",
		format("§æi 1 Xu nhËn 1 LÖnh Bµi Bosss/#okaydoixuboss(%d)",1),
		format("§æi 5 Xu nhËn 5 LÖnh Bµi Bosss/#okaydoixuboss(%d)",5),
		format("§æi 10 Xu nhËn 10 LÖnh Bµi Bosss/#okaydoixuboss(%d)",10),
		--format("10000 ®iÓm tÝch luü nhËn ®iÓm kinh nghiÖm/#okaytichluytk2exp(%d)",10000),
		NOW_END_SAY
	})
end


function okaydoixuboss(nlbb)
	local nLanDoi = 3
	if(GetTask(T_PLAYER_XU) < nlbb) then
		Talk(1,"","<sex> kh«ng ®ñ "..nlbb.." Xu mµ muèn lõa ta sao?")
		return
	end
	if(GetTask(TASK_DUPHONG3) >= nLanDoi) then
		Talk(1,"","<sex> H«m nay §¹i HiÖp ®· ®æi "..nLanDoi.." lÇn kh«ng thÓ ®æi thªm n÷a !")
		return
	end
	
	SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) -  nlbb)

	for i = 1,nlbb do 
	AddItem(6,1,1023,0,0,0,0)
	end
	SetTask(TASK_DUPHONG3, GetTask(TASK_DUPHONG3) + 1)
	Msg2Player("Xin chóc mõng ®· nhËn ®­îc "..nlbb.." LÖnh Bµi Boss")
	thoigian = tonumber(date("%H%M%d%m"))
			LoginLog = openfile("dulieu/logLBBB.txt", "a");
			if LoginLog then
			write(LoginLog,""..GetAccount().." - "..GetName().." -"..GetLevel().."  - Time: "..thoigian.." - So L­¬ng: "..nlbb.." cai\n");
			end
			closefile(LoginLog)
end


function no()
	SetPKMode(0,0)
end


