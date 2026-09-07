-- =====================================================================================
-- lmbj_config.lua - [VANTIEU 06/09] LONG MON TIEU CUC, nhanh CA NHAN (1-9 sao)
--
--   *** DAY LA PHAN TU VIET LAI, KHONG PHAI MA GOC LINUX ***
--
-- Ban dump Linux (D:\ServerLinux\server1\script\event\longmenbiaoju\ - 22 tep) THIEU
-- HAN tep dinh nghia cua nhanh ca nhan: 48 thanh vien LongMenBiaoJu.* bi DOC ma khong
-- duoc GAN o bat ky dau (~30 hang so can bang + 18 phuong thuc). Chinh may chu Linux
-- goc cung KHONG boot noi vi loi nay - xem
--     D:\ServerLinux\server1\Logs\script\luaerror_20260806.txt:201-290
-- Chu game DA DUYET cho viet lai phan nay.
--
-- KY HIEU NGUON cua tung con so (ghi ngay tren dong):
--   [L] suy ra tu chinh ma Linux con lai   - co dan chung tep:dong
--   [E] lay tu engine C++ da port          - Sources\Core\Src\KBiaoChe.h / ScriptFuns.cpp
--   [T] TOI TU DAT (khong con dau vet nao) - can chu game duyet, nen dua len cau hinh web
--
-- CACH NAP: tep nay duoc Include o CUOI head.lua (thay cho dong "LongMenBiaoJu:Load()"
-- cu o head.lua:252). Moi tep khac chi can Include head.lua nhu truoc la du.
-- Bo va di kem: ...\vantieu\moi\va_lmbj.py
--
-- LUA 5.4: khong dung %x (upvalue Lua 4); duyet bang bang pairs(); ... phai goi qua
-- local arg = {n = select("#", ...), ...}
-- =====================================================================================

-- WriteYunBiaoLog: KHONG viet lai o day. Dot port nay da co ban rieng dung dinh dang
-- cua engine Linux (Logs\KSG_YunBiaoLog_YYYYMMDD.txt) tai script\lib\lib_lmbiaoche.lua.
Include("\\script\\lib\\lib_lmbiaoche.lua")
-- tbGlobalMapId2Name - can cho lop dem SubWorldName o muc 18.
Include("\\script\\global\\maplist.lua")

LongMenBiaoJu = LongMenBiaoJu or {}


-- =====================================================================================
-- 1. DANH SACH VAT PHAM  (ItemList)
-- =====================================================================================
-- Bi doc o: lang.lua:6-12 (gan szName), taskclass.lua:148,229,298,347,574,
--           broken_biaoche.lua:72, item.lua:61-65, dialog.lua:279,336-338
-- Ma vat pham [L]: D:\ServerLinux\server1\settings\item\004\magicscript.txt:4760-4767
--   4200 Tieu Ky | 4201 Hoan Tieu Chung | 4202 Uy nhiem trang cao cap | 4203 Ho Tieu Lenh
--   4204 Khoai Ma Gia Tien | 4205 Kien Bat Kha Toa | 4206 Thiet Xa Mat Bo | 4207 Tieu Xa Di Vi
-- CD = 180 giay [L]: mo ta cua ca 3 mon 4204/4205/4207 deu ghi "thoi gian cho 3 phut"
--   (magicscript.txt:4764,4765,4767)
-- nSkillId [L]: nhanh BANG dung dung 2 chieu nay cho cung 2 mon do -
--   UseForTongBiaoChe: 4204 -> NpcCastSkill(...,1467,1) ; 4205 -> NpcCastSkill(...,1468,lv)
--   (item.lua:128,133)
-- tbSkillLevel = {1,2,3} [L]: item.lua:24 tra cuu qua biaoche.tbNpcLevel[nType]
--   (biaoche.lua:10 = {1,1,1,2,2,2,3,3,3}) - trung khop bang getSkillLevel cua nhanh
--   bang (item.lua:73-85: cap 1-3 -> 1, cap 4-6 -> 2, cap 7-10 -> 3)
-- nTaskId [L]: dung DUNG so hieu cua nhanh bang -
--   script\activitysys\config\129\variables.lua:57-60
--   (TSK_ItemKMJBTime 3500 / TSK_ItemJBKCTime 3501 / TSK_ItemBCYWTime 3502)
--   => hai nhanh dung chung o hoi chieu, khong the lach CD bang cach doi nhanh.
-- nPrice [T]: gia doi bang Ho Tieu Lenh o cua hang Tieu Cuc (dialog.lua:278-291).
--   Can cu: moi lan ap tieu THANH CONG duoc 2 Ho Tieu Lenh (taskclass.lua:555),
--   toi da 3 lan/ngay => 6 cai/ngay. Dat 5/5/10 de mua duoc khoang 1 mon moi ngay.

-- [VTCN 06/09] SUA MA: bang Linux 4200-4207 KHONG phai ma JX1. Trong magicscript.txt cua may chu
-- nay (dong = ParticularType + 1): 4771 Tieu Ky, 4772 Hoan Tieu Chi, 4773 uy nhiem trang cao cap,
-- 4774 Ho Tieu Lenh, 4775 Khoai Ma Gia Tien, 4776 Kien Bat Kha Toa, 4777 Thiet Xa Mat Bo,
-- 4778 Tieu Xa Di Vi. Dong 4200-4207 la 'Thuong Long Van Tinh - Binh' - phat nham mon.
LongMenBiaoJu.ItemList = {
	-- Tieu Ky: rot ra khi Tieu Xa bi pha; nop lai de lay phan thuong an ui
	BiaoQi = {
		szName = "Tiªu Kú",
		tbProp = {6, 1, 4771, 1, 0, 0},	-- [VTCN 06/09] ma JX1 (magicscript.txt), Linux la 4200
		nCount = 1,
		nBindState = -2,
	},
	-- Hoan Tieu Chung: lam moi nhiem vu khi da het luot mien phi
	PingZheng = {
		szName = "Ho¸n Tiªu ChØ",
		tbProp = {6, 1, 4772, 1, 0, 0},	-- [VTCN 06/09] ma JX1 (magicscript.txt), Linux la 4201
		nCount = 1,
		nBindState = -2,
	},
	-- Uy nhiem trang cao cap: doi lay nhiem vu 7/8/9 sao
	WeiRenZhuang = {
		szName = "¸p tiªu ñy nhiÖm tr¹ng cao cÊp",
		tbProp = {6, 1, 4773, 1, 0, 0},	-- [VTCN 06/09] ma JX1 (magicscript.txt), Linux la 4202
		nCount = 1,
		nBindState = -2,
	},
	-- Ho Tieu Lenh: tien te cua cua hang Tieu Cuc
	HuBiaoLing = {
		szName = "Hé Tiªu LÖnh",
		tbProp = {6, 1, 4774, 1, 0, 0},	-- [VTCN 06/09] ma JX1 (magicscript.txt), Linux la 4203
		nCount = 1,
		nBindState = -2,
	},
	-- Khoai Ma Gia Tien: tang 100 phan tram toc do Tieu Xa trong 15 giay
	KuaiMaJiaBian = {
		szName = "Kho¸i M· Gia Tiªn",
		tbProp = {6, 1, 4775, 1, 0, 0},	-- [VTCN 06/09] ma JX1 (magicscript.txt), Linux la 4204
		nCount = 1,
		nBindState = -2,
		nPrice = 5,
		CD = 180,
		nTaskId = 3500,
		nSkillId = 1467,
		tbSkillLevel = {1, 2, 3},
	},
	-- Kien Bat Kha Toa: hoi 10 phan tram mau Tieu Xa
	JianBuKeCui = {
		szName = "Kiªn BÊt Kh¶ Táa",
		tbProp = {6, 1, 4776, 1, 0, 0},	-- [VTCN 06/09] ma JX1 (magicscript.txt), Linux la 4205
		nCount = 1,
		nBindState = -2,
		nPrice = 5,
		CD = 180,
		nTaskId = 3501,
		nSkillId = 1468,
		tbSkillLevel = {1, 2, 3},
	},
	-- Tieu Xa Di Vi: keo Tieu Xa ve dung cho nguoi choi dang dung
	BiaoCheWeiYi = {
		szName = "Tiªu Xa Di VÞ",
		tbProp = {6, 1, 4778, 1, 0, 0},	-- [VTCN 06/09] ma JX1 (magicscript.txt), Linux la 4207
		nCount = 1,
		nBindState = -2,
		nPrice = 10,
		CD = 180,
		nTaskId = 3502,
	},
}

-- =====================================================================================
-- 2. SO HIEU BIEN NHIEM VU  (TASK_ID)
-- =====================================================================================
-- Bi doc o: taskdata.lua:19-79, taskclass.lua:293,310,319,329,336,457,463,542-543,
--           box.lua:78,126, dialog.lua:130-131,172,195,216,230
-- Ban Linux KHONG con so hieu nao -> [T] TOI TU DAT. Cach chon:
--   * MAX_TASK cua ban JX1 nay = 5200 (Sources\Core\Src\KPlayerTask.h:27) => id 0-5199.
--   * Da quet toan bo cay chay that E:\...\TESTLOFFF_ONLINE\bin\server\{script,
--     scriptjx2,settings}: dai 4157-5113 KHONG co bien nhiem vu nao dang dung.
--     Chon 4160-4168 (lien khoi, de tra).
--   * KHONG dam vao 4178-4187 vi do la khoi cua nhanh BANG khi port sang
--     (config\129\variables.lua:46-55), cung nhu 3500-3511 va 3540-3545.
--   * KHONG dam vao 4126-4138 (dot va trung task id 24/08) va 5167-5199 (Chien Lenh).
-- CACH DONG GOI:
--   BIAOCHE_TYPE_FLAG dung 4 byte qua SetBitTask (taskdata.lua:9-15):
--       byte1 = nType (cap sao 1-9)   byte2 = co "di theo qua trap"
--       byte3 = nRouteId (1-26)       byte4 = nInvert (0/1)
--   TASK_TEMP_DATA cung 4 byte: byte1/2 = tuyen tam cua nhiem vu 1 sao,
--       byte3/4 = tuyen tam cua nhiem vu ngau nhien
--   4 bien dem NGAY di qua PlayerFunLib:GetTaskDaily/AddTaskDaily -> dong goi
--       yymmdd*256 + so lan (playerfunlib.lua:368-380,422-434), tuc CHIEM TRON mot
--       bien, tuyet doi khong duoc chia se voi he khac.
LongMenBiaoJu.TASK_ID = {
	BIAOCHE_TYPE_FLAG            = 4160,   -- [T] 4 byte: cap sao / co trap / tuyen / dao chieu
	TASK_START_TIME              = 4161,   -- [T] GetCurServerTime luc Tieu Xa xuat phat
	TASK_TEMP_DATA               = 4162,   -- [T] 4 byte: tuyen tam (1 sao) + tuyen tam (ngau nhien)
	BIAOCHE_FINISH_COUNT         = 4163,   -- [T] dem ngay: so lan NHAN nhiem vu
	BIAOCHE_FINISH_SUCCESS_COUNT = 4164,   -- [T] dem ngay: so lan ap tieu THANH CONG
	BIAOCHE_REFRESH_COUNT        = 4165,   -- [T] dem ngay: so lan lam moi MIEN PHI
	BIAOWU_PICK_COUNT            = 4166,   -- [T] dem ngay: so lan nhat Tieu Vat roi
	TRANSFER_TIME                = 4167,   -- [T] moc lan truyen tong den Tieu Xa gan nhat
}

-- taskdata.lua:60,64 goi NHAM "LongMenBiaoJu.BIAOCHE_TYPE_FLAG" (thieu ".TASK_ID").
-- Do la LOI CO SAN cua ban Linux. Giu nguyen tep goc, dat but danh o day de no tro ve
-- DUNG mot bien - re hon va an toan hon la sua taskdata.lua.
LongMenBiaoJu.BIAOCHE_TYPE_FLAG = LongMenBiaoJu.TASK_ID.BIAOCHE_TYPE_FLAG

-- Bit ghi nho "ca ba chang deu nam trong gio nhan thuong gap boi".
-- Bi doc o: dialog.lua:35,46,85 va taskclass.lua:521
--   bit 1 = luc NHAN nhiem vu | bit 2 = luc XUAT PHAT | bit 3 = luc TRA hang
LongMenBiaoJu.TSK_IS_IN_AWARDRISE_TIME = 4168   -- [T] cung khoi 4160-4168


-- =====================================================================================
-- 3. KHUNG GIO VA GIOI HAN MOI NGAY
-- =====================================================================================
-- StartTime / EndTime bi doc o lang.lua:3-4,69 va taskclass.lua:179-181,220-222,393-395
-- [L] Nhanh BANG ghi ro "khong trong thoi gian ap tieu 10:00-23:00" va kiem
--     (nCurTime > 2300 or nCurTime < 1000) - config\129\extend.lua:268-270 va :528-532
LongMenBiaoJu.StartTime = {10, 0}          -- [L] 10:00
LongMenBiaoJu.EndTime   = {23, 0}          -- [L] 23:00

-- [L] "hom nay da hoan thanh 3 lan ap tieu" + kiem (nTaskValue >= 3):
--     config\129\extend.lua:301-303 va :331-333; bang thong tin in "%d/3" (:521, :680)
LongMenBiaoJu.MAX_TASK_COUNT = 3

-- [L] bang thong tin nhanh bang in "so lan lam moi nhiem vu mien phi hom nay: %d/5"
--     (config\129\extend.lua:525, :684) va kiem nFreeCount < 5 (:746, :759)
LongMenBiaoJu.MAX_REFRESH_COUNT = 5

-- [L] "moi hiep si moi ngay toi da nhat 5 lan tieu vat" + kiem >= 5:
--     config\129\npc_lmbiaowu.lua:96-99
LongMenBiaoJu.MAX_BIAO_WU_PICK_COUNT = 5

-- [L] cap toi thieu de nhat Tieu Vat: chinh cau bao loi cua ban Linux noi ro so nay -
--     lang.lua:98 "dang cap cua nguoi van chua den cap 120 va chua trong sinh..."
--     (box.lua:64 truyen hang nay lam nguong cho PlayerFunLib:CheckTotalLevel)
--     Chu y: nhanh BANG dung 150 (npc_lmbiaowu.lua:74) - hai nhanh von khac nhau.
LongMenBiaoJu.MIN_LEVEL = 120

-- [L] chi loan bao toan server voi Tieu Xa tu 7 sao tro len - dung y het
--     script\global\yunbiao_system.lua:47-59 (getLevel chi tra ve 7/8/9/10)
LongMenBiaoJu.MIN_NOTIFY_TYPE = 7


-- =====================================================================================
-- 4. TRUYEN TONG DEN TIEU XA
-- =====================================================================================
-- [L] gia 10.000 luong: config\129\extend.lua:552-556 (nMoney < 10000) va :913 Pay(10000)
LongMenBiaoJu.TRANSFER_COST = 10000
-- [L] hoi chieu 30 giay: config\129\extend.lua:557-562
--     "cach lan truyen tong truoc chua den 30 giay, hay doi"
LongMenBiaoJu.TRANSFER_CD = 30


-- =====================================================================================
-- 5. THOI LUONG NHIEM VU / VONG DOI NPC  (don vi GIAY)
-- =====================================================================================
-- [L] 30 phut: nhanh bang goi refreshBiaoChe(30*60) - config\129\extend.lua:963 ; moi
--     bang thoai deu ghi "trong vong 30 phut den noi" (extend.lua:321,341,467) va moi
--     cho kiem qua han deu dung 1800 (:258, :635, :974)
LongMenBiaoJu.TASK_TIME = 30 * 60          -- 1800
-- Tieu Xa tu bien mat khi qua han (biaoche.lua:146). Dat bang TASK_TIME cho khop voi
-- bo dem "con %d phut" ma nguoi choi nhin thay (dialog.lua:149,159).
LongMenBiaoJu.BIAOCHE_LIFE = 30 * 60       -- [L] 1800

-- [E] Tieu Xa xa chu qua lau thi bien mat. Lay DUNG hang so cua engine da port:
--     Sources\Core\Src\KBiaoChe.h:23  #define BC_LOST_TICK 5400  -- 300 giay o 18 tick/giay
--     (biaoche.lua:69 so sanh bang GetCurServerTime nen don vi la GIAY)
LongMenBiaoJu.BIAOCHE_ALONE = 300

-- [L] xac Tieu Xa (de nhat lai Tieu Ky) song 600 giay:
--     config\129\npc_lmbiaoche.lua:75  SetNpcTimer(nNpcBQIdx, 600*18)
--     (broken_biaoche.lua:29 nhan lai voi 18 nen day phai la GIAY)
LongMenBiaoJu.BIAOQI_TIME = 600
-- [L] tui Tieu Vat roi song 600 giay: config\129\npc_lmbiaoche.lua:83 SetNpcTimer(...,600*18)
--     (box.lua:33 nhan lai voi 18)
LongMenBiaoJu.BOX_TIME = 600
-- [L] 180 giay dau chi chu Tieu Xa / to doi duoc nhat: config\129\npc_lmbiaowu.lua:106-108
--     (chu thich goc ghi "thoi gian bao ve 3 phut")
LongMenBiaoJu.BOX_PROTECT_TIME = 180


-- =====================================================================================
-- 6. KHOANG CACH (deu la BINH PHUONG, don vi 1/32 o - vi ma goc so sanh ^2 khong khai can)
-- =====================================================================================
-- [E] nguong "chu roi xa Tieu Xa": lay DUNG hang so engine
--     Sources\Core\Src\KBiaoChe.h:22  #define BC_FAR_DIST2 262143u   (= 512^2 - 1, tuc 16 o)
--     Dung o biaoche.lua:119,165 va playerclass.lua:32. Dung chung mot con so voi C++
--     de script va engine khong bao gio lech nhau mot o.
LongMenBiaoJu.FOLLOW_DISTANCE = 262143

-- [L] nguong "dua Tieu Xa den noi de tra hang": nhanh bang kiem
--     sqrt(...) > 600 (config\129\extend.lua:443) -> binh phuong = 360000.
--     Dung o taskclass.lua:155.
LongMenBiaoJu.FINISH_DISTANCE = 360000


-- =====================================================================================
-- 7. DUONG DAN SCRIPT BEN RELAY
-- =====================================================================================
-- [L] chinh head.lua tu goi ve minh bang duong dan nay (head.lua:51,69,89) nen
--     szRelayFile bat buoc phai la no. Dung o taskclass.lua:288,635 va
--     biaoche.lua:139,220,227.
LongMenBiaoJu.szRelayFile = "\\script\\event\\longmenbiaoju\\head.lua"


-- =====================================================================================
-- 8. THUONG CO BAN (kinh nghiem + bac) THEO CAP SAO
-- =====================================================================================
-- Bi doc o: taskclass.lua:477-492 (bo cuoc), :524-540 (thanh cong), :584-600 (con Tieu Ky),
--           box.lua:139-156 (ke cuop nhat Tieu Vat)
-- Thu tu 4 o: {nExpFactor, nCurrencyFactor, nExpBiaoCheFactor, nCurrencyBiaoCheFactor}
--   (taskclass.lua:479 unpack theo dung thu tu nay)
-- Y NGHIA [T] toi dat, chon sao cho dung het CA BON con so va khop voi ten bien:
--   * 2 o dau = phan CO DINH, nhan duoc ke ca khi mat Tieu Xa (bo cuoc / con Tieu Ky /
--               ke cuop nhat duoc) -> GetExpAwardValue / GetCurrencyAwardValue
--   * 2 o sau = phan CONG THEM khi GIAO DUOC Tieu Xa (chu "BiaoChe" trong ten bien)
--               -> GetExpAwardValueSuccess = o1 + o3 ; ...Success = o2 + o4
-- MOC NEO: bo cuoc con duoc them 1.000.000 kinh nghiem (FAILED_EXP ben duoi), va mot
--   lan truyen tong ton 10.000 luong -> de bac o day cung bac 10.000.
-- CAC SO NAY LA [T] - nen dua len bang cau hinh web de chu game chinh sau.
LongMenBiaoJu.tbAwardBaseParam = {
	--        expCoDinh   bacCoDinh   expGiaoHang   bacGiaoHang
	[1] = {     200000,        2000,      300000,        3000 },   -- [T] 1 sao
	[2] = {     400000,        4000,      600000,        6000 },   -- [T] 2 sao
	[3] = {     600000,        6000,      900000,        9000 },   -- [T] 3 sao
	[4] = {     800000,        8000,     1200000,       12000 },   -- [T] 4 sao
	[5] = {    1000000,       10000,     1500000,       15000 },   -- [T] 5 sao
	[6] = {    1200000,       12000,     1800000,       18000 },   -- [T] 6 sao
	[7] = {    1600000,       16000,     2400000,       24000 },   -- [T] 7 sao
	[8] = {    2000000,       20000,     3000000,       30000 },   -- [T] 8 sao
	[9] = {    2600000,       26000,     3900000,       39000 },   -- [T] 9 sao
}

-- [L] 1.000.000 kinh nghiem khi bo cuoc. Hai cho noi cung mot con so:
--   lang.lua:56 "co the ket thuc nhiem vu va nhan 100 van kinh nghiem" (100 van = 1 trieu)
--   config\129\extend.lua:842 PlayerFunLib:AddExp(1000000, ...)
LongMenBiaoJu.FAILED_EXP = 1000000

-- Nhan tieu de nhat ky cho phan thuong co ban (tbAwardTemplet:Give doc o [1] -
-- script\lib\awardtype\item_jx1.lua:19-21). Dat theo dung cach ban Linux dat ten cac
-- muc khac ("LongMenBiaoJu" + ten muc): taskclass.lua:496,545,554-555 va box.lua:158.
LongMenBiaoJu.tbLogTittle = {"LongMenBiaoJu", "BaseAward"}


-- =====================================================================================
-- 9. GIO NHAN THUONG GAP BOI
-- =====================================================================================
-- [L] Khung gio lay tu chinh kich ban relay: relay\longmenbiaoju.lua:50-59
--     13:00-13:59 va 19:00-19:59 (dung 14:00 / 20:00 thi bao ket thuc).
LongMenBiaoJu.tbAwardRiseTime = {
	{1300, 1400},   -- [L] relay\longmenbiaoju.lua:51-54
	{1900, 2000},   -- [L] relay\longmenbiaoju.lua:55-58
}
-- [L] loi rao "cong bo phan thuong GAP BOI" (relay\longmenbiaoju.lua:62) -> he so 2.
--     Duoc dung nhu MOT SO (taskclass.lua:484,531,591 va box.lua:146), khong phai bang,
--     du ten bien co tien to "tb".
LongMenBiaoJu.tbAwardRiseFactor = 2


-- =====================================================================================
-- 10. THUONG THEM MOI NGAY
-- =====================================================================================
-- [L] lang.lua:111 "Moi ngay thanh cong hoan thanh 3 lan Van Tieu se nhan duoc them
--     mot Le Bao Tieu Cuc." -> dieu kien = 3, vat pham = Le Bao Tieu Cuc.
--     (taskclass.lua:542-548 so sanh so lan THANH CONG trong ngay voi hang nay)
LongMenBiaoJu.nExtraAwardCondition = 3

-- [L] Le Bao Tieu Cuc = 6,1,4534 (chinh script goc gan cho no la
--     item_biaojulibao.lua): D:\ServerLinux\server1\settings\item\004\magicscript.txt:4863
LongMenBiaoJu.tbExtraAward = {
	{szName = "LÔ Bao Tiªu Côc", tbProp = {6, 1, 4809, 1, 0, 0}, nCount = 1, nBindState = -2},	-- [VTCN 06/09] 4534 la dong test rac trong magicscript JX1; 4809 = Ho Tieu Le Hop (mon gan nhat, chua co script su dung)
}

-- =====================================================================================
-- 11. LAY CAC LOP CON  (GetDataClass / GetTaskClass / GetDlgClass)
-- =====================================================================================
-- Bi goi 48 lan trong 22 tep. Ba lop deu tu gan minh vao LongMenBiaoJu o cuoi tep cua no
-- (taskdata.lua:82, taskclass.lua:5,640, dialog.lua:7) nen o day chi viec tra ve.

function LongMenBiaoJu:GetDataClass()
	-- TaskDataClass:New() cua ban goc chi "return self" (taskdata.lua:5-7), nen tra
	-- thang bang lop la dung ngu nghia va khong sinh rac moi lan goi.
	return self.TaskDataClass
end

function LongMenBiaoJu:GetTaskClass()
	local task = self.TaskClass
	if not task then
		return
	end
	-- VA LOI BOOT #2: ban goc goi TaskClass:Load() ngay o THAN CHUNK (taskclass.lua:639).
	-- Luc boot, tep con chay ben Relay - noi KHONG co TabFile_Load - nen no chet va
	-- route.txt KHONG BAO GIO duoc nap (luaerror_20260806.txt:281-284). Doi thanh NAP TRE:
	-- lan dau co ai can den bang tuyen duong thi moi doc, va chi doc khi that su co
	-- TabFile_Load (tuc dang o GameServer).
	if task.bRouteLoaded ~= 1 and TabFile_Load and task.Load then
		task.bRouteLoaded = 1
		task:Load()
	end
	return task
end

function LongMenBiaoJu:GetDlgClass()
	return self.DlgClass
end


-- =====================================================================================
-- 12. SO TIEU XA TREN GAMESERVER
-- =====================================================================================
-- LongMenBiaoJu.BiaoCheList (head.lua:6) la so cua RELAY (ghi bang SyncBiaoChe /
-- DeleteBiaoChe, chi chua toa do de tra cuu tu xa). Ben GameServer can mot so RIENG giu
-- doi tuong BiaoCheClass that -> hai bang duoi day: tra theo TEN CHU va theo NPC INDEX.
-- [VANTIEU 06/09 va#4] PHAI "or {}": head.lua Include tep nay o dong cuoi,
-- ma head.lua bi Include lai moi lan mot tep khac trong thu muc duoc nap
-- (JX1 khong dedupe Include) - "= {}" se xoa sach Tieu Xa dang song.
LongMenBiaoJu.GSBiaoCheByName = LongMenBiaoJu.GSBiaoCheByName or {}
LongMenBiaoJu.GSBiaoCheByIndex = LongMenBiaoJu.GSBiaoCheByIndex or {}

function LongMenBiaoJu:FindBiaoChe(szOwner)
	-- Goi o: taskclass.lua:85,98,112,151,515,579 - item.lua:19,43 - playerclass.lua:20
	if not szOwner then
		return
	end
	local npc = self.GSBiaoCheByName[szOwner]
	if not npc then
		return
	end
	if not npc.nNpcIndex then           -- da DelNpc ma quen go so
		self.GSBiaoCheByName[szOwner] = nil
		return
	end
	return npc
end

function LongMenBiaoJu:FindBiaoCheByIndex(nNpcIndex)
	-- Goi o: biaoche.lua:186 (OnTimer) va :207 (OnDeath)
	if not nNpcIndex then
		return
	end
	local npc = self.GSBiaoCheByIndex[nNpcIndex]
	if npc and npc.nNpcIndex == nNpcIndex then
		return npc
	end
end

function LongMenBiaoJu:NewBiaoChe(nType, nMapId, nX, nY, szOwner)
	-- Goi o: taskclass.lua:116 (di chuyen ban do) va :620 (xuat phat).
	-- KHONG dung BiaoCheClass:New(): ham do goi SyncDataToRelay() VO DIEU KIEN
	-- (biaoche.lua:14-19), nen khi CreateNpc that bai (ban do khong nam tren may chu nay,
	-- biaoche.lua:31-34) se goi GetNpcPos(nil) -> ScriptError. O day tu rap lai, co kiem.
	local cls = self.BiaoCheClass
	if not cls or not szOwner then
		return
	end
	local cu = self:FindBiaoChe(szOwner)
	if cu then                          -- moi nguoi chi duoc mot Tieu Xa
		return cu
	end
	local npc = cls:_new()
	if not npc:CreateNpc(nType, nMapId, nX, nY, szOwner) then
		return
	end
	self.GSBiaoCheByName[szOwner] = npc
	self.GSBiaoCheByIndex[npc.nNpcIndex] = npc
	npc:SyncDataToRelay()
	return npc
end

function LongMenBiaoJu:RemoveBiaoChe(npc)
	-- Goi o: biaoche.lua:177,194,202 va taskclass.lua:102. Chi go khoi so, KHONG xoa NPC:
	-- ben goi tu quyet dinh co DelNpc hay khong (vd chuyen may chu thi giu NPC lai).
	if not npc then
		return
	end
	if npc.szOwner and self.GSBiaoCheByName[npc.szOwner] == npc then
		self.GSBiaoCheByName[npc.szOwner] = nil
	end
	if npc.nNpcIndex then
		self.GSBiaoCheByIndex[npc.nNpcIndex] = nil
	end
end

function LongMenBiaoJu:NewBrokenBiaoChe(nMapIndex, nX32, nY32, szOwner, nTaskTime)
	-- Goi o: taskclass.lua:57 (OnBiaoCheBroken). Tham so khop het voi
	-- BrokenBiaoCheClass:CreateNpc (broken_biaoche.lua:12).
	local cls = self.BrokenBiaoCheClass
	if not cls then
		return
	end
	return cls:CreateNpc(nMapIndex, nX32, nY32, szOwner, nTaskTime)
end

function LongMenBiaoJu:NewDropBox(nType, nMapIndex, nX32, nY32, szOwner)
	-- Goi o: taskclass.lua:62 (OnBiaoCheBroken). Khop BoxClass:CreateNpc (box.lua:15).
	local cls = self.BoxClass
	if not cls then
		return
	end
	return cls:CreateNpc(nType, nMapIndex, nX32, nY32, szOwner)
end


-- =====================================================================================
-- 13. SO NGUOI CHOI DANG AP TIEU  (phuc vu viec di theo qua trap)
-- =====================================================================================
-- event_process.lua:10,13,27,29,41 goi self:FindPlayer / NewPlayer / DelPlayer ma ban
-- Linux khong dinh nghia o dau. Lop PlayerClass thi co san (playerclass.lua:50).
LongMenBiaoJu.PlayerList = LongMenBiaoJu.PlayerList or {}   -- [va#4] xem ghi chu muc 12

function LongMenBiaoJu:FindPlayer(szName)
	if not szName then
		return
	end
	return self.PlayerList[szName]
end

function LongMenBiaoJu:NewPlayer(szName)
	local cls = self.PlayerClass
	if not cls or not szName then
		return
	end
	local p = cls:New()
	p.szName = szName
	self.PlayerList[szName] = p
	return p
end

function LongMenBiaoJu:DelPlayer(szName)
	if szName then
		self.PlayerList[szName] = nil
	end
end


-- =====================================================================================
-- 14. GOI HAM XUYEN MAY CHU  (GameServer <-> Relay <-> GameServer)
-- =====================================================================================
-- Duong di cua mot lan goi (taskclass.lua:92 la cho duy nhat dung):
--   GS(A) LongMenBiaoJu:RemoteCall(szHam, tbThamSo, fnGoiLai, tbThamSoGoiLai)
--     -> Relay  LongMenBiaoJu:OnGSTransfer    (head.lua:33,  CO SAN)
--     -> phat cho MOI GS  LongMenBiaoJu:OnRemoteCall     (viet o day)
--     -> GS dang giu Tieu Xa chay ham roi tra ket qua ve
--     -> Relay  LongMenBiaoJu:OnGSReturn      (head.lua:73,  CO SAN)
--     -> GS(A)  LongMenBiaoJu:OnRemoteRespond (viet o day) -> goi fnGoiLai
-- Cho dat so goi lai: TaskClass.RelayRequest / TaskClass.RequestId da duoc khai bao san
-- o taskclass.lua:7-8 nhung khong dung o dau -> dung chinh no, dung y ban goc.

function LongMenBiaoJu:RemoteCall(szFunction, tbParam, fnCallBack, tbCallBackParam)
	local task = self.TaskClass
	if not task or type(szFunction) ~= "string" then
		return
	end
	local nId = 0
	if fnCallBack then
		task.RequestId = (task.RequestId or 0) + 1
		if task.RequestId > 1000000 then
			task.RequestId = 1
		end
		nId = task.RequestId
		task.RelayRequest[nId] = {fn = fnCallBack, tb = tbCallBackParam or {}}
	end
	local h = OB_Create()
	ObjBuffer:PushObject(h, nId)
	ObjBuffer:PushObject(h, szFunction)
	ObjBuffer:PushObject(h, tbParam or {})
	RemoteExecute(self.szRelayFile, "LongMenBiaoJu:OnGSTransfer", h)
	OB_Release(h)
end

function LongMenBiaoJu:OnRemoteCall(HParam)
	-- Chay ben GAMESERVER. Relay phat goi nay cho TAT CA GameServer (head.lua:69).
	if OB_IsEmpty(HParam) == 1 then
		return
	end
	local nRelayCallBack = ObjBuffer:PopObject(HParam)
	local szFunction = ObjBuffer:PopObject(HParam)
	local tbCallParam = ObjBuffer:PopObject(HParam)
	if type(szFunction) ~= "string" or type(tbCallParam) ~= "table" then
		return
	end
	-- CHI may chu DANG GIU Tieu Xa moi duoc tra loi. Neu de may chu nao cung tra thi
	-- may tra som nhat (voi ket qua rong) se an mat ket qua that: head.lua:80-81 go so
	-- goi lai ngay o lan tra dau tien.
	if tbCallParam[1] and not self:FindBiaoChe(tbCallParam[1]) then
		return
	end
	local task = self:GetTaskClass()
	if not task then
		return
	end
	local fn = task[szFunction]
	if type(fn) ~= "function" then
		return
	end
	local nSo = tbCallParam.n or #tbCallParam
	local tbRet = {fn(task, unpack(tbCallParam, 1, nSo))}
	tbRet.n = #tbRet
	if nRelayCallBack == 0 then
		return
	end
	local h = OB_Create()
	ObjBuffer:PushObject(h, nRelayCallBack)
	ObjBuffer:PushObject(h, tbRet)
	RemoteExecute(self.szRelayFile, "LongMenBiaoJu:OnGSReturn", h)
	OB_Release(h)
end

function LongMenBiaoJu:OnRemoteRespond(HParam)
	-- Chay ben GAMESERVER da dat lenh. Relay gui ve (head.lua:86-90).
	if OB_IsEmpty(HParam) == 1 then
		return
	end
	local nId = ObjBuffer:PopObject(HParam)
	local tbResult = ObjBuffer:PopObject(HParam)
	local task = self.TaskClass
	if not task or not nId or nId == 0 then
		return
	end
	local tbReq = task.RelayRequest[nId]
	task.RelayRequest[nId] = nil
	if not tbReq or type(tbReq.fn) ~= "function" then
		return
	end
	if type(tbResult) ~= "table" then
		tbResult = {}
	end
	-- Ghep: tham so dat truoc + ket qua tu xa dat sau. Vi du duy nhat dang dung
	-- (taskclass.lua:92-94): {self, szOwner, nType, nMapId, nX, nY} + {nLife, nCreateTime}
	-- -> CreateBiaoCheForTransfer(self, szOwner, nType, nMapId, nX, nY, nLife, nCreateTime)
	local tbArg = {}
	local nA = tbReq.tb.n or #tbReq.tb
	local i
	for i = 1, nA do
		tbArg[i] = tbReq.tb[i]
	end
	local nB = tbResult.n or #tbResult
	for i = 1, nB do
		tbArg[nA + i] = tbResult[i]
	end
	tbArg.n = nA + nB
	call(tbReq.fn, tbArg)
end


-- =====================================================================================
-- 15. GIO NHAN THUONG GAP BOI - BA HAM KIEM
-- =====================================================================================
-- IsInAwardRiseTime          : dialog.lua:33,45,84 va taskclass.lua:520
-- CanGetAwardRise            : taskclass.lua:482,529,589 va box.lua:144
-- ResetRecordIsAwardRiseTime : dialog.lua:34
-- Y do cua ban goc doc ra tu 4 cho ghi bit:
--   dialog.lua:35 bit1 khi NHAN nhiem vu | dialog.lua:46 bit2 khi XUAT PHAT
--   dialog.lua:85 bit3 khi TRA hang      | taskclass.lua:520-522 XOA bit3 neu luc tra
--   hang da ra ngoai gio -> phai du CA BA moc moi duoc nhan gap boi.

function LongMenBiaoJu:IsInAwardRiseTime()
	local nCurTime = tonumber(GetLocalDate("%H%M"))
	if not nCurTime then
		return 0
	end
	local k, tb
	for k, tb in pairs(self.tbAwardRiseTime) do
		if nCurTime >= tb[1] and nCurTime < tb[2] then
			return 1
		end
	end
	return 0
end

function LongMenBiaoJu:CanGetAwardRise()
	local nId = self.TSK_IS_IN_AWARDRISE_TIME
	if GetBitTask(nId, 1, 1) == 1 and GetBitTask(nId, 2, 1) == 1 and GetBitTask(nId, 3, 1) == 1 then
		return 1
	end
	return 0
end

function LongMenBiaoJu:ResetRecordIsAwardRiseTime()
	local nId = self.TSK_IS_IN_AWARDRISE_TIME
	SetBitTask(nId, 1, 1, 0)
	SetBitTask(nId, 2, 1, 0)
	SetBitTask(nId, 3, 1, 0)
end


-- =====================================================================================
-- 16. TINH THUONG CO BAN
-- =====================================================================================
-- taskclass.lua:479-481 / :526-528 / :586-588 va box.lua:141-143 deu tach 4 o cua
-- tbAwardBaseParam roi goi hai cap ham nay.
--   ...Value        = phan CO DINH  (bo cuoc / con Tieu Ky / ke cuop nhat duoc)
--   ...ValueSuccess = phan CO DINH + phan CONG THEM khi giao duoc Tieu Xa

function LongMenBiaoJu:GetExpAwardValue(nExpFactor, nExpBiaoCheFactor)
	return nExpFactor or 0
end

function LongMenBiaoJu:GetCurrencyAwardValue(nCurrencyFactor, nCurrencyBiaoCheFactor)
	return nCurrencyFactor or 0
end

function LongMenBiaoJu:GetExpAwardValueSuccess(nExpFactor, nExpBiaoCheFactor)
	return (nExpFactor or 0) + (nExpBiaoCheFactor or 0)
end

function LongMenBiaoJu:GetCurrencyAwardValueSuccess(nCurrencyFactor, nCurrencyBiaoCheFactor)
	return (nCurrencyFactor or 0) + (nCurrencyBiaoCheFactor or 0)
end


-- =====================================================================================
-- 17. NHAT KY
-- =====================================================================================
-- LongMenBiaoJu:WriteLog duoc goi o taskclass.lua:122 voi so tham so KHONG co dinh.
-- Lua 5.4: bat buoc dung local arg = {n = select("#", ...), ...}
-- Ghi qua WriteYunBiaoLog cua script\lib\lib_lmbiaoche.lua (da Include o dau tep) -
-- ham do TU dat moc thoi gian dau dong nen o day khong dat lai nua.
function LongMenBiaoJu:WriteLog(...)
	local arg = {n = select("#", ...), ...}
	local szLine = "[LongMenBiaoJu]"
	local i
	for i = 1, arg.n do
		szLine = szLine .. "\t" .. tostring(arg[i])
	end
	WriteYunBiaoLog(szLine)
end


-- =====================================================================================
-- 18. HAM ENGINE MA BAN JX1 CHUA CO  (lop dem thuan Lua)
-- =====================================================================================
-- 7 ham duoi day co trong engine JX2 nhung KHONG co trong ScriptFuns.cpp cua JX1
-- (da soat 06/09). Chung chi bi goi trong biaoche.lua / taskclass.lua, nen dat o day de
-- khong phai sua tep goc. Neu sau nay port han sang C++ thi cac khoi "== nil" tu dong
-- nhuong cho.

-- SubWorldName(nMapIndex) -> ten ban do. Goi o taskclass.lua:67,73 va biaoche.lua:72,149.
-- JX1 chi co SubWorldIdx2ID (ScriptFuns.cpp) nen ghep them bang ten
-- tbGlobalMapId2Name cua script\global\maplist.lua (sinh tu settings\MapList.ini).
if SubWorldName == nil then
	function SubWorldName(nMapIndex)
		if not nMapIndex or nMapIndex < 0 then
			return ""
		end
		local nMapId = SubWorldIdx2ID(nMapIndex)
		if not nMapId or not tbGlobalMapId2Name then
			return ""
		end
		return tbGlobalMapId2Name[nMapId] or ""
	end
end

-- GetNpcCurLife / SetNpcCurLife: engine JX1 co san cap ham DUNG ngu nghia (doc/ghi
-- m_CurrentLife, KHONG dong tran mau nhu SetNpcLife) -
-- Sources\Core\Src\ScriptFuns.cpp:4173 va :4184
if GetNpcCurLife == nil then
	function GetNpcCurLife(nNpcIndex)
		return NPCINFO_GetNpcCurrentLife(nNpcIndex)
	end
end
if SetNpcCurLife == nil then
	function SetNpcCurLife(nNpcIndex, nLife)
		return NPCINFO_SetNpcCurrentLife(nNpcIndex, nLife)
	end
end

-- SetNpcAI(idx, nMode) cua JX2 == SetNpcAIType cua JX1 (ScriptFuns.cpp, ghi m_AiMode)
if SetNpcAI == nil then
	function SetNpcAI(nNpcIndex, nMode)
		return SetNpcAIType(nNpcIndex, nMode)
	end
end

-- SetNpcActiveRegion(idx, 1): ben JX2 la co "giu NPC hoat dong", KHONG phai ban kinh.
-- JX1 chi co SetNpcActiveRange (ghi m_CurrentActiveRadius) - dat ban kinh = 1 se LAM
-- HONG viec di chuyen cua Tieu Xa. He van tieu san co cua JX1
-- (script\event\event_vantieu\tieudau.lua:165-171) tao xe tieu ma KHONG he goi ham nay,
-- nen de trong la an toan nhat.
if SetNpcActiveRegion == nil then
	function SetNpcActiveRegion(nNpcIndex, nValue)
		return
	end
end

-- StopNpcAction: dung xe lai tai cho. JX1 khong co, thay bang lenh di den chinh o dang
-- dung (NpcWalk nhan toa do O, con GetNpcPos tra toa do 1/32 o).
if StopNpcAction == nil then
	function StopNpcAction(nNpcIndex)
		local nX32, nY32 = GetNpcPos(nNpcIndex)
		if nX32 then
			NpcWalk(nNpcIndex, floor(nX32 / 32), floor(nY32 / 32))
		end
	end
end

-- IsHide: JX1 khong co trang thai "an than" cua nguoi choi (da soat ScriptFuns.cpp).
-- Tra 0 = khong an -> biaoche.lua:87 xu ly y het nhu nguoi choi binh thuong.
if IsHide == nil then
	function IsHide()
		return 0
	end
end


-- =====================================================================================
-- 19. VA LOI BOOT #1 - head.lua:252 goi LongMenBiaoJu:Load() ngay o THAN CHUNK
-- =====================================================================================
-- Load() dung OB_LoadShareData - ham CHI CO ben Relay. Khi GameServer nap head.lua (moi
-- tep trong thu muc deu Include no) thi ham do la nil va ca chuoi Include gay ScriptError
-- day trang: luaerror_20260806.txt:201-290.
-- Cach va: BO loi goi o than chunk (do bo va va_lmbj.py lam), doi thanh NAP TRE - chi
-- nap khi that su co OB_LoadShareData (tuc dang chay ben Relay) va chi nap MOT lan.
LongMenBiaoJu.bLoaded = LongMenBiaoJu.bLoaded or 0

function LongMenBiaoJu:EnsureLoaded()
	if self.bLoaded == 1 then
		return 1
	end
	if not OB_LoadShareData then      -- khong phai Relay -> khong co gi de nap
		return
	end
	self.bLoaded = 1
	self:Load()
	return 1
end

-- Boc 3 cua ngo ben Relay: vao bang duong nao cung tu nap so lieu truoc.
-- [VANTIEU 06/09 va#4] CO CHAN BOC CHONG: head.lua (va tep nay theo no)
-- chay lai moi lan mot tep trong thu muc duoc nap, khong chan thi moi lan
-- lai boc them mot lop len ba ham nay.
if LongMenBiaoJu.bDaBoc ~= 1 then
LongMenBiaoJu.bDaBoc = 1
local _lmbj_OnGSTransfer = LongMenBiaoJu.OnGSTransfer
function LongMenBiaoJu:OnGSTransfer(HParam, HResult, nGameServerID)
	self:EnsureLoaded()
	return _lmbj_OnGSTransfer(self, HParam, HResult, nGameServerID)
end

local _lmbj_GenerateTask = LongMenBiaoJu.GenerateTask
function LongMenBiaoJu:GenerateTask(HParam, HResult)
	self:EnsureLoaded()
	return _lmbj_GenerateTask(self, HParam, HResult)
end

local _lmbj_AcceptRandomTask = LongMenBiaoJu.AcceptRandomTask
function LongMenBiaoJu:AcceptRandomTask(ParamHandle)
	self:EnsureLoaded()
	return _lmbj_AcceptRandomTask(self, ParamHandle)
end
end   -- [va#4] het khoi chan boc chong
