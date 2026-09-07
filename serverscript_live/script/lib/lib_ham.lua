-- ================================================================================================
-- [HE THONG] script/lib/lib_ham.lua
-- Muc dich  : THU VIEN HAM TIEN ICH CHUNG cua du an: thoi gian, chuoi, bang, thong bao, tien te.
-- Duoc nap  : Include tu 84 tep (vd eventlib2010.lua, npcevent.lua, eventlib.lua, tuongquan.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : lib_task.lua
-- Ham (dong): UpdateTongExpAndLevel (34), AddItemSL (49), admincheck (54), MsgEx (63), SayEx (71), MsgToPlayerInTaskTemp (90), MsgToPlayerInTask (102), MsgToPlayerInTaskValue (114), FindNameInTab (126), TaoBang (134), SaveData (163), TaoBangNew (169), SaveDataNew (194), LuuBang (200), updatengaymoi (221), LuuTrangThaiHoatDongHomQua (313), CapNhatThieuHoatDong (330), GetNgayHomNay (370), UpdateNgayMoiAllPlayer (378), no (392), Exit (395)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- Author: Fong KiÒu
-- Date: 28/06/2021
-- Chuc nang: Ham ho tro
Include("\\script\\lib\\lib_task.lua")

MAX_PLAYER 	= 400

hethong 				= 0
thegioi				= 1
lancan					= 2
monphai				= 3
thanhthi				= 4
banghoi				= 5
todoi					= 6
A,B,C,D,E,F = GetTimeNow()
TimeGame = (""..C.."/"..B.."/"..A.."")
GMLIST = {
	--"phuc1","colauhong2","taythi23",
	"thienho","2","","","","","","","","",
}


DIEMBANGHOINANGCAP = {
    {1000, 2},
    {5000, 3},
    {12000, 4},
    {20000, 5},
    {30000, 6},
    {50000, 7},
    {70000, 8},
    {100000, 9},
    {150000, 10},
}

function UpdateTongExpAndLevel(nAddExp)
    local nExp = GetTongExp()
    local nNewLevel = 0

    
    local i = 1
    while DIEMBANGHOINANGCAP[i] do
        if nExp >= DIEMBANGHOINANGCAP[i][1] then
            nNewLevel = DIEMBANGHOINANGCAP[i][2]
        end
        i = i + 1
    end
    SetTongLevel(nNewLevel)
end

function AddItemSL(ID, SOLUONG, KHOA)
	local mIndex = AddItem(6,1,ID,0,0,0,0,0,0,0,0,0,SOLUONG,0,0,0,0,0,0,KHOA,0) --- -2 khoa vinh vien
   return mIndex
end

function admincheck()
	for i=1,getn(GMLIST) do
		if GetAccount() == GMLIST[i] then
			return 1
		end
	end
	return 0
end

function MsgEx(sMessage,sMsgType)
	if (sMsgType==0 or sMsgType==nil) then
		Msg2Player(sMessage);
	else
		Talk(sMsgType,"",sMessage);
	end
end;

function SayEx(strSay)
	local strMsg,strSel = "","";
	local strNum = getn(strSay);
	if strNum < 2 then
		return
	end;
	if strNum > 2 then
		for i=2,strNum - 1 do
			strSel = strSel..format("%q", strSay[i])..",";
		end;
		strSel = strSel..format("%q", strSay[strNum]);
		strMsg = "Say("..format("%q", strSay[1])..","..(strNum - 1)..","..strSel..");";
	elseif strNum == 2 then
		strSel = format("%q", strSay[strNum]);
		strMsg = "Say("..format("%q", strSay[1])..",1"..","..strSel..");";
	end;
	dostring(strMsg);
end;

function MsgToPlayerInTaskTemp(IdTaskTemp,TAB_MSG)
	local idbt = PlayerIndex
	for i =1,MAX_PLAYER do
	PlayerIndex = i
	local Min = GetTaskTemp(IdTaskTemp)
	if (Min ~= 0) then
		Msg2Player(TAB_MSG)
	end
	end
	PlayerIndex = idbt
end

function MsgToPlayerInTask(IdTask,TAB_MSG)
	local idbt = PlayerIndex
	for i =1,MAX_PLAYER do
	PlayerIndex = i
	local Min = GetTask(IdTask)
	if (Min >= 1) then
		Msg2Player(TAB_MSG)
	end
	end
	PlayerIndex = idbt
end

function MsgToPlayerInTaskValue(IdTask,nValue,TAB_MSG)
	local idbt = PlayerIndex
	for i =1,MAX_PLAYER do
	PlayerIndex = i
	local Min = GetTask(IdTask)
	if (Min == nValue) then
		Msg2Player(TAB_MSG)
	end
	end
	PlayerIndex = idbt
end

function FindNameInTab(Name,TabName,nTT)
	for i = 1,getn(TableName) do
	if TableName[i][nTT] == Name then
	return 1
	end
	end
end

function TaoBang(tTable,sTableName,sTab)
    sTab = sTab or "";  
    sTmp = ""  
    sTmp = sTmp..sTab..sTableName.."={"  
 
    local tStart = 0  
    for key, value in pairs(tTable) do  
  
        if tStart == 1 then  
            sTmp = sTmp..",\r\n"  
 
        else  
            sTmp = sTmp.."\r\n"  
            tStart = 1  
        end  
        local sKey = (type(key) == "string") and format("[%q]",key) or format("[%d]",key);  
        if(type(value) == "table") then  
            sTmp = sTmp..TaoBang(value, sKey, sTab.."\t");  
        else  
            local sValue = (type(value) == "string") and format("%q",value) or tostring(value);  
            sTmp = sTmp..sTab.."\t"..sKey.." = "..sValue 
        end  
 
    end 
    sTmp = sTmp.."\r\n"..sTab.."}"  
    return sTmp
  
end  

function SaveData(file, string)   ---Tuy ngan nhung wan trong nhat >''<
	file_op = openfile( file, "w+" )  --- a+ la vit them hem xoa file dau -- w+ -- la che do doc ghi xoa file dau  
    write(file_op,string) 
    closefile(file_op)  
end 

function TaoBangNew(tTable, sTableName, sTab)
    sTab = sTab or ""
    local sTmp = ""

    if sTableName and sTableName ~= "" then
        sTmp = sTmp .. sTab .. sTableName .. " = {\n"
    else
        sTmp = sTmp .. sTab .. "{\n"
    end

    for key, value in pairs(tTable) do
        local sKey = (type(key) == "string") and format("[%q]", key) or format("[%d]", key)
        if type(value) == "table" then
      
            sTmp = sTmp .. sTab .. "\t" .. sKey .. " = " .. TaoBangNew(value, "", sTab .. "\t") .. ",\n"
        else
            local sValue = (type(value) == "string") and format("%q", value) or tostring(value)
            sTmp = sTmp .. sTab .. "\t" .. sKey .. " = " .. sValue .. ",\n"
        end
    end

    sTmp = sTmp .. sTab .. "}"
    return sTmp
end

function SaveDataNew(file, content)
    local f = openfile(file, "w+")
    write(f, content)
    closefile(f)
end


function LuuBang(file, string)
	local f,e = openfile( file, "w+" )
	if f then
		write(f,string)
		closefile(f)
		return 1
	else
		local _,_,path = strfind(file, "(.+[/_\\]).+$") 
		if path ~= nil then execute("mkdir ".."\""..gsub(path, "/", "\\").."\"") end
		f,e = openfile( file, "w+" )
		if f then
			write(f,string) 
			closefile(f)
			return 2
		else
			return 0
		end
	end
end

function updatengaymoi()
	local nYr,nMo,nDy,nHr,nMi,nSe,nDyfW = GetTimeNow()
	local nNam = GetTask(TASK_NAM)
	local nThang = GetTask(TASK_THANG)
	local nNgay = GetTask(TASK_NGAY)
	if(nNam == 0 or nThang == 0 or nNgay ==0) then
		SetTask(TASK_NAM,nYr)
		SetTask(TASK_THANG,nMo)
		SetTask(TASK_NGAY,nDy)
	elseif(nNam ~= nYr or nThang ~= nMo or nNgay ~= nDy) then --reset 1 ngay moi
		LuuTrangThaiHoatDongHomQua()
		
		SetTask(TASK_NAM,nYr)
		SetTask(TASK_THANG,nMo)
		SetTask(TASK_NGAY,nDy)
		
		SetTask(T_USEHUYHOANG,0)								--reset task can thiet tai day
		SetTask(T_USEHATHKIM,0)
		SetTask(T_USETLH,0)
		SetTask(T_NAPTHESAI,0)
		SetTask(T_CUOPTIEU,0)
		SetTask(T_NVSTTRONGNGAY,0)						--vuot ai, boss sat thu
		SetTask(T_NVVATRONGNGAY,0)
		SetTask(TSK_LONGHUYETHOAN,0)
		SetTask(T_RIGHTQUESTION,0)
		SetTask(T_REPCAUHOI,0)
		SetTask(T_CH_SAILIENTIEP,0)
		SetTask(T_CH_DANHANTHG,0)
		SetTask(T_NVVTTRONGNGAY,0)
		SetTask(TSK_HAI_QUA_HH_NGAY,0)
		SetTask(TSK_HAI_QUA_HK_NGAY,0)
		SetTask(T_USEHUYHOANG,0)
		SetTask(T_USEHATHKIM,0)
		SetTask(T_NhiemVuPLD,0)									--phong lang do
		SetTask(T_SoQuaiPLD,0)
		SetTask(T_SoLanNhanNVPLD,0)
		SetTask(T_DiThuyen,0)
		SetTask(T_MOTUIDUOCPHAM,0)						--mo tui duoc pham
		
		SetTask(T_SoLanHuyNV,0)	--task d· tÈu
		SetTask(T_SoNVTrongNgay,0)
		SetTask(T_TimVatPham,0)
		SetTask(T_TIENDONV,0)
		SetTask(T_TIMDOCHI,0)
		SetTask(T_TIMMATCHI,0)
		SetTask(T_DanhQuai,0)
		SetTask(T_NDTriSoPK,0)
		SetTask(T_NDPhucDuyen,0)
		SetTask(T_NDDanhVong,0)
		SetTask(T_NDTongKim,0)
		SetTask(T_DiemYeuCau,0)
		SetTask(T_DiemHienCo,0)
		SetTask(T_CHECKPHETK, 0) --hÕt trËn set phe tham gia 0
		SetTask(T_CHECKDATETK, 0)--hÕt trËn set thêi gian tham gia 0
		SetTask(T_EXP_TRONGBH, 0)--nhËn exp trèng bang héi
		SetTask(TSK_HOPLEVATVA, 0) --sö dông hép lÔ vËt v­ît ¶i trong ngµy
		SetTask(TSK_NUMPLDNGAY, 0) --sè lÇn ®i PLD trong ngµy
		SetTask(TSK_GIAOTRUYCONGL, 0) --sè lÇn giao truy c«ng lÖnh trong ngµy
		SetTask(T_MAX_SDLBHT, 0)
		SetTask(T_NVDTLienTiepDay, 0)
		SetTask(TASK_DUPHONG1, 0)
		SetTask(TASK_DUPHONG3, 0)
		SetTask(T_NHANNVVTIEU,0)
		SetTask(TASK_RESET5,0)
		SetTask(TASK_RESET3,0);
		SetTask(TASK_TANTHU3,0); -- ®· nhËn th­ëng ho¹t ®éng trong ngµy qua ngµy míi lµm míi 
		SetTask(TASK_TANTHU5,0); -- ®· ®ñ mèc 4 ho¹t ®éng trong ngµy qua ngµy míi lµm míi 
		SetTask(TASK_NEWTHOREN6,0); 
		SetChatFlag(0)
		CapNhatThieuHoatDong()
		print(format("===> Reset Task Account:[%s]  Player[%s] %d-%d-%d",GetAccount(),GetName(),nNgay,nThang,nNam))
	end	
end

TB_BU_HD = {
	-- [BUHD 30/08] CHI CON MOT DONG. Nam dong kia dem bang task ma KHONG AI GHI
	-- (PLD 315 / VA 43 / Hang Ngay 345 / Vip 377 / DT 87) - he cu da go, he
	-- moi dem bang task khac. De nguyen thi moi ngay moi nhan vat bi cong 'no ao'
	-- vao task 383..397 roi nam do vinh vien, trong khi KHONG CO CHO NAO PHAT
	-- (lenhbaitanthu.lua:107 da comment). Tat di khong doi gi nguoi choi thay.
	--
	-- MUON BAT LAI: bo comment dong tuong ung VA danh so lai lien tuc tu [1],
	-- vi hai ham duyet bang bang `while TB_BU_HD[i]` - ho mot so la dung ngay.
	-- Truoc do phai quyet he MOI dem luot bang task nao (viec thiet ke).
    [1] = { "VT",      T_NVVTTRONGNGAY, TASK_HD_MISS_VANTIEU, 2, 1000000000, TASK_HD_YESTERDAY_VANTIEU },
	-- [1] = { "PLD", TSK_NUMPLDNGAY,  TASK_HD_MISS_PLD,     2, 1000000000, TASK_HD_YESTERDAY_PLD },
	-- [2] = { "VA",       T_NVVATRONGNGAY, TASK_HD_MISS_VA,      2, 1000000000, TASK_HD_YESTERDAY_VA },
	-- [4] = { "H»ng Ngµy",     TASK_TANTHU3,    TASK_HD_MISS_NVHN,   1, 1000000000, TASK_HD_YESTERDAY_NVHN },
	-- [5] = { "Vip",           TASK_NEWTHOREN6, TASK_HD_MISS_MOCVIP, 1, 1000000000, TASK_HD_YESTERDAY_VIP },
	-- [6] = { "DT",           T_SoNVTrongNgay, TASK_HD_MISS_MOCDT, 40, 10000000, TASK_HD_YESTERDAY_DT },
}

function LuuTrangThaiHoatDongHomQua()
    local i = 1
    while TB_BU_HD[i] do
        local nDaLam = GetTask(TB_BU_HD[i][2])
        local nCan   = TB_BU_HD[i][4]

        if nDaLam < 0 then nDaLam = 0 end
        if nDaLam > nCan then nDaLam = nCan end

        SetTask(TB_BU_HD[i][6], nDaLam)

        i = i + 1
    end
end

MAX_BU_DAYS = 7

function CapNhatThieuHoatDong()
    local nToday = GetNgayHomNay()


    if GetTask(TASK_HD_BU_NGAY) == nToday then
        return
    end

    local i = 1
    while TB_BU_HD[i] do
        local nDoneYesterday = GetTask(TB_BU_HD[i][6])  
        local nCan           = TB_BU_HD[i][4]         
        local nMissLuotOld   = GetTask(TB_BU_HD[i][3])  

        if nDoneYesterday < 0 then nDoneYesterday = 0 end
        if nDoneYesterday > nCan then nDoneYesterday = nCan end
        if nMissLuotOld < 0 then nMissLuotOld = 0 end

        local nThieuLuotHomQua = nCan - nDoneYesterday
        if nThieuLuotHomQua < 0 then nThieuLuotHomQua = 0 end

        local nMissLuotNew = nMissLuotOld + nThieuLuotHomQua

        
        local nMaxLuot = MAX_BU_DAYS * nCan
        if nMissLuotNew > nMaxLuot then
            nMissLuotNew = nMaxLuot
        end

        SetTask(TB_BU_HD[i][3], nMissLuotNew)
        i = i + 1
    end


    SetTask(TASK_HD_BU_NGAY, nToday)


    SetTask(TASK_HD_BU_DANHAN, 0)
end



function GetNgayHomNay()
    return GetTask(TASK_NAM) * 10000
         + GetTask(TASK_THANG) * 100
         + GetTask(TASK_NGAY)
end






function UpdateNgayMoiAllPlayer()
	local oldPlayerIndex = PlayerIndex
	for i =1,GetCountPlayerMax() do
		PlayerIndex = i
		updatengaymoi()
	end
	PlayerIndex = oldPlayerIndex
end

function no()
end

function Exit()
end