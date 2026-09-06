--Author: Fong KiÒu
--Date 07/06/2021
--Function: §Þnh nghÜa ID_Task trong Source vµ Script

-- MON PHAI
T_VAOPHAI									= 1
T_XUATSU									= 2
-- TASK KHONG DUOC DUNG 3
T_NOUSE1									= 3
T_HOCKHINHCONG					= 4
-- VAT PHAM
T_VLMT										= 5
T_TTK											= 6
-- HE THONG
T_HOTRO										= 7
-- VAN TIEU
T_NPCID										= 8
T_LOAITIEUXA							= 9
T_NHANNVVTIEU						= 10
-- DA TAU
T_TAYTUY_TYPE						= 4135	-- [FIX TRUNG TASK 24/08] truoc la 11; 11 = TASK_DUNGCHUNG (packed-digit, 18 cho dung) -- 1 ®iÓm tiem nang --2 diem ky nang -- 3 tat ca cac loai diem
T_TIMDOCHI								= 12
T_TIMMATCHI							= 13
T_TIENDONV								= 14	--tiÕn ®é nhiÖm vô d· tÈu
T_TAYTUYFREE						= 15
T_USEHUYHOANG					= 16
T_USEHATHKIM						= 4137	-- [FIX TRUNG TASK 24/08] truoc la 17; 17 = SetByte cua map Vo Dang (packed-byte, 15 cho dung)
-- PHAN THUONG
T_RUONGFB								= 18
--CHUYEN SINH
T_CHUYENSINH1						= 19
T_LUUSKILL1								= 20
T_LUUSKILL2								= 21
T_LUUSKILL3								= 22
T_LUUSKILL4								= 23
T_NHANLAISKILL						= 24
-- VAT PHAM
T_USETLH									= 25
T_CONGNGUYET						= 26
T_PHUNGNGUYET					= 27
-- KHAC
T_USESIRO5SAC							= 28
T_USEKEMCVONG					= 29
T_USEHHSAOVANG					= 30
T_USEHHSAODO						= 31
-- TASK KHONG DUOC DUNG 32
T_NOUSE2									= 32	--®ang dïng ®Ó häc khinh c«ng
T_USEHUYHIEU							= 33
T_PASSWORD								= 34
T_NAPTHESAI								= 35
-- DINH NGHIA TU SOURCE
T_X2EXP										= 36 -- ®Þnh nghÜa trong source kh«ng ®­îc dïng thêi gian x2 ®iÓm kinh nghiÖm chÕt kh«ng mÊt
T_LIENDAU									= 37  -- da co code ®iÓm tÝch luü liªn ®Êu
T_TONGKIM								= 38  -- da co code	®iÓm tÝch luü tèng kim
T_TICHLUYCT							= 39  -- da co code ®iÓm tÝch luü c«ng tr¹ng uy danh
-- SAT THU
T_NVSTTRONGNGAY 				= 40
T_LOAIBOSS								= 41
T_NHANNVSTHU						= 42
-- VUOT AI
T_NVVATRONGNGAY				= 43
-- CAU HOI
T_REPCAUHOI							= 4136	-- [FIX TRUNG TASK 24/08] truoc la 44; 44 = TASK_NVTHONGTHUONG (packed-digit)
T_CUOPTIEU								= 45
T_DUATOP									= 46
T_NHANGIFTCODE					= 47
T_RIGHTQUESTION					= 48
T_NVVTTRONGNGAY				= 49
-- da set 49
TASK_NAM									= 50
TASK_THANG								= 51
TASK_NGAY								= 52
TSK_HAI_QUA_HH_NGAY		= 4133	-- [FIX TRUNG TASK 24/08] truoc la 53; 53 = TASK_DUNGCHUNG3 (packed-digit, 26 cho dung)
TSK_HAI_QUA_HK_NGAY		= 54
--Task nhiÖm vô m«n ph¸i
TSK_NV_NGAMY						= 55
TSK_NV_DUONGMON 			= 56
TSK_NV_THIENVUONG 			= 4132	-- [FIX TRUNG TASK 24/08] truoc la 57; 57 = moc cap nhat ky nang (global\skills_table.lua:10)
TSK_NV_THIEN_NHAN	 		= 58
TSK_NV_VODANG					= 59
TSK_NV_THUYYEN	 				= 60
TSK_NV_THIEULAM 				= 61
TSK_NV_CAIBANG					= 62
TSK_NV_CONLON	 					= 63
TSK_NV_NGUDOC	 					= 64
TSK_LONGHUYETHOAN 		= 65
--Task Phong L¨ng §é
T_NhiemVuPLD 							= 66
T_SoQuaiPLD 								= 67
T_SoLanNhanNVPLD 					= 68
T_DiThuyen 									= 69
T_MOTUIDUOCPHAM 				= 70
T_TITLE_ID									= 71 --®Þnh nghÜa trong source kh«ng ®­îc dïng
T_TITLE_TIME							= 72 --®Þnh nghÜa trong source kh«ng ®­îc dïng
T_DANHVONG							= 73  -- da co code ®iÓm danh väng ®Þnh nghÜa trong source kh«ng ®­îc dïng
T_PHUCDUYEN							= 74  -- da co code ®iÓm phóc duyªn ®Þnh nghÜa trong source kh«ng ®­îc dïng
T_SONHAXATAC						= 75 -- Task mËt ®å thÇn bÝ s¬n hµ x· t¾c ®Þnh nghÜa trong source kh«ng ®­îc dïng
TSK_CITYTONG_DATE			= 76 --task kiem tra nhan thuong cong thanh chien
T_RSLienDau								= 77
T_ThanhLapChienDoi					= 78 --luu kien chien doi lien dau
T_NhanLD2									= 79 --kiem tra nhan thuong lien dau
T_NhanDiemOnline						= 80
T_LuuSoTran									= 81
T_LuuSoTranThang						= 82
TASKID_FY_START_ONLINE_TIME 	= 83
TASKID_FY_ADDITIONAL_TIME 		= 84
TASKID_GAIN_LAST_DATE 					= 85
TASKID_GAIN_TIMES_IN_DAY 			= 86
-- DA TAU
T_SoNVTrongNgay						= 87
T_TimVatPham								= 88
T_DanhQuai									= 89	--®Þnh nghÜa trong source kh«ng dc thay ®æi
T_NDPhucDuyen							= 90
T_NDDanhVong							= 91
T_NDTongKim								= 92
T_DiemYeuCau								= 93
T_DiemHienCo								= 94
T_NDTriSoPK								= 95
T_HuyNhiemVu							= 96
T_SoLanHuyNV							= 97
T_SoNVHoanThanh						= 98
T_MayMan									= 99
T_CH_SAILIENTIEP					= 100
--101 den 103 sö dông trong source kh«ng dïng ®Ó dïng save nh÷ng ®iÓm ®· ®i qua
T_CH_DANHANTHG					= 104
T_EXP_TRONGBH						= 105
T_CHECKPHETK						= 106
T_CHECKDATETK						= 107
T_ANTI_HACK1							= 108 --®Þnh nghÜa trong src	t¹m thêi ch­a sö dông
T_ANTI_HACK2							= 109 --®Þnh nghÜa trong src	t¹m thêi ch­a sö dông
--110 den 143 sö dông trong source kh«ng ®Ó dïng
-- TASK KHONG DUOC DUNG 201 203 210 220 221 
T_NOUSE3									= 201
T_NOUSE3									= 203 --MateName
T_NOUSE4									= 210
T_NOUSE5									= 220
T_NOUSE6									= 221
T_RUT_KNB								= 250
T_PLAYER_XU							= 251 -- ®­îc ®Þnh nghÜa trong src

T_SAVE_TK_KILLPLAYER		= 300
T_SAVE_TK_KILLNPC				= 301
T_SAVE_TK_DEATH					= 302
T_SAVE_TK_MAXLT				= 303
T_SAVE_TK_POINT					= 304	--save l¹i ®iÓm tÝch luü khi bÞ ®Èy ra ngoµi Tèng Kim khi trËn míi reset vÒ 0 hÕt
T_SAVE_TK_CURLT					= 305
T_SAVE_TK_NUMLAG				= 306
T_SAVE_TK_RANK					= 307	--save l¹i qu©n hµm khi bÞ ®Èy ra ngoµi Tèng Kim khi trËn míi reset vÒ 0 hÕt
T_SAVE_TK_NHATBV				= 308
T_SAVE_TK_POINTKILLPL		= 309
T_SAVE_TK_POINTKILLNPC	= 310
T_SAVE_TK_POINTLT				= 311
T_SAVE_TK_POINTBV				= 312
T_SAVE_TK_POINTFLAG		= 313
TSK_HOPLEVATVA					= 314
TSK_NUMPLDNGAY					= 315 --sè lÇn ®i thuyÒn mçi ngµy
TSK_GIAOTRUYCONGL			= 316 --sè lÇn giao truy c«ng lÖnh trong ngµy
T_NVDTLienTiepDay					= 317 --sè lÇn tr¶ nhiÖm vô liªn tiÕp trong ngµy d· tÈu
T_MAX_SDLBHT						= 318
TSK_DANH_BOSS_VT				= 319
TASK_TANTHU 			= 320		--[1]nhan item [2]hocskill9x[3]nhantop[4]testgame[5]ho tro tao bang hoi[6][7][8][9]thuongcap
TASK_THOIGIAN9		= 321 --THOI GIAN nhan danh vong

LoaiBCH					= 325
TaskBCH					= 326
TaskDBCH				= 327
TaskUyThacOffline		= 327
TaskUyThacOffline1 		= 329
TaskUyThacOffline2 		= 330
TaskUyThacOffline3 		= 331
TaskUyThacOffline4 		= 332

TASK_EVENT3004_1 = 333 -- task event game;
TASK_EVENT3004_2 = 334 -- task event game;
TASK_EVENT3004_3 = 335 -- task event game;

TASK_BDTK = 336 -- task event game;
TASK_DSK = 337  -- task ®iÓm sù kiÖn

TASK_NAPTHE = 338  -- task ®iÓm sù kiÖn
TASK_MOCEVENT = 339  -- task ®iÓm sù kiÖn.

TASK_EVENT2011_1 = 340 -- task event game;
TASK_EVENT2011_2 = 341 -- task event game;
TASK_EVENT2011_3 = 342 -- task event game;

TASK_TANTHU1 =343
TASK_TANTHU2 =344
TASK_TANTHU3 =345
TASK_TANTHU4 =346
TASK_TANTHU5 =347
TASK_TANTHU6 =348

TASK_DUPHONG1 =349
TASK_DUPHONG2 =350
TASK_DUPHONG3 =351
TASK_DUPHONG4 =352
TASK_DUPHONG5 =353
TASK_DUPHONG6 =354
TASK_NVST = 355	--[1][2]nhan nv [3][4][5][6][7][8]NVLongngu [9]thanh thi return
TASK_RESET3 = 356 --[1]so lan  tham gia dhha[2]so lan thienbaokho[3]vantieu lan/ngay
NVTHIENBAOKHO = 357	--[1][2]nhannv+soruong tieptheo [3][4]keymoruong
TASK_RESET5 = 358 --[1]so lan  tham gia dhha[2]so lan thienbaokho[3]vantieu lan/ngay

TASK_MAMNGUQUA	= 359
TASK_NHANMOCTKEOGIANGSINH	= 360
TASK_NHANMOCMAMNGUQUA	= 361
TASK_RESTEVENTMAMNGUQUA = 363
TASK_CHANGNGUYENDAN	= 362
TASK_FANCUNG = 364
TASK_FANCUNG_1 = 365
TASK_FANCUNG_2 = 366
TASK_FANCUNG_3 = 367
TASK_FANCUNG_4 = 368
TASK_FANCUNG_5 = 369
TASK_FANCUNG_6 = 370

T_NVPUBGTRONGNGAY = 371

TASK_NEWTHOREN1 = 372
TASK_NEWTHOREN2 = 373
TASK_NEWTHOREN3 = 374
TASK_NEWTHOREN4 = 375
TASK_NEWTHOREN5 = 376
TASK_NEWTHOREN6 = 377
TASK_NEWTHOREN7 = 378
TASK_NEWTHOREN8 = 379
TASK_NEWTHOREN9 = 380

TASK_HD_MISS_PLD     = 383  -- so ngay thieu
TASK_HD_MISS_VA    = 384  --so ngay thieu
TASK_HD_MISS_VANTIEU    = 385  -- so ngay thieu
TASK_HD_MISS_NVHN    = 386  -- so ngay thieu
TASK_HD_MISS_MOCVIP    = 387  -- so ngay thieu

TASK_HD_BU_NGAY      = 388  -- 
TASK_HD_BU_DANHAN    = 389  -- 

TASK_HD_YESTERDAY_PLD      = 390
TASK_HD_YESTERDAY_VA       = 391
TASK_HD_YESTERDAY_VANTIEU  = 392
TASK_HD_YESTERDAY_NVHN     = 393
TASK_HD_YESTERDAY_VIP      = 394

TASK_HD_MISS_MOCDT    = 395  -- so ngay thieu
TASK_HD_MISS_MOCTBK   = 396  -- so ngay thieu

TASK_HD_YESTERDAY_DT     = 397
TASK_HD_YESTERDAY_TBK     = 398

TASK_HD_EVENTNEW     = 399



--*******************************************************************************
							-- TASK TEMP --
--*******************************************************************************
TMP_INDEX_PLAYER				= 3
TMP_NHANNAPTHE					= 4
TMP_NHANTRANGBIXANH	= 5
TMP_HETRANGBIXANH			= 6
TMP_INDEX_NPC						= 7
TMP_INDEX_ITEM					= 8
TMP_INDEX_OBJ						= 9
TMP_LUUMASERIAL				= 10
TMP_LOAITHENAP					= 11
TMP_LUUMACODE					= 12
TMP_TGIANMOTUI					= 13
TMP_PhongLangDo						= 14
TMP_BUY_CTC							= 15
TMP_TG_LOIDAIB						= 16
TMP_MAP_VUOTAI					= 17
TMP_LienDau_DonDau				= 18
TMP_DthuLienDauCao				= 19
TMP_DthuLienDau						= 20
TMP_BAYTK								= 4138	-- [FIX TRUNG TASK 24/08] truoc la 21; TaskTemp 21 = diem ky nang tam (global\skills_table.lua:20)
TMP_MAP_PUBG					=22

--*******************************************************************************
							-- MISSION --
--*******************************************************************************
MS_TONGKIM							= 1
MS_CTHANHCHIEN					= 2
MS_VUOTAI								= 3
MS_PLANGDO							= 4
MS_KIEMMONQUAN				= 5
MS_LOIDAIBH							= 6
M_LD_DONDAU						= 11
MS_PUBG							=12
--*******************************************************************************
							-- MISSIONV --
--*******************************************************************************
M_VALUE_1				= 1	--vuot ai ID doi truong
M_VALUE_2				= 2  --vuot ai so luong quai trong map
M_VALUE_3				= 3
M_VALUE_4				= 4
M_VALUE_5				= 5
M_VALUE_6				= 6
M_VALUE_7				= 7
M_VALUE_8				= 8
M_VALUE_9				= 9
M_VALUE_10				= 10
M_VALUE_11				= 11
M_VALUE_12				= 12
M_VALUE_13				= 13
M_VALUE_14				= 14
M_VALUE_15				= 15
M_VALUE_16				= 16
M_VALUE_17				= 17
M_VALUE_18				= 18
M_VALUE_19				= 19
M_VALUE_20				= 20
M_VALUE_21				= 21
M_VALUE_22				= 22
M_VALUE_23				= 23
M_VALUE_24				= 24
M_VALUE_25				= 25
M_VALUE_26				= 26
M_VALUE_27				= 27
M_VALUE_28				= 28
M_VALUE_29				= 29
M_VALUE_30				= 30
M_TICHLUYA				= 31
M_TICHLUYB 			= 32
M_HINHTHUC				= 33
M_SOCOPHEA				= 34
M_SOCOPHEB				= 35
M_SOTRAN					= 36
M_ADDNPCA				= 37	--value 1 ®¸nh dÊu so¸i Tèng xuÊt hiÖn 2 so¸i chÕt 0 lµ ch­a xuÊt hiÖn
M_ADDNPCB				= 38	--value 1 ®¸nh dÊu so¸i Kim xuÊt hiÖn 2 so¸i chÕt 0 lµ ch­a xuÊt hiÖn
M_CTHANHVO_1		= 39	--value 1 ®¸nh dÊu cæng thµnh bÞ vì lËp d­¬ng
M_CTHANHVO_2		= 40	--value 1 ®¸nh dÊu cæng thµnh bÞ vì ®Þnh xuyªn
M_CTHANHVO_3		= 41	--value 1 ®¸nh dÊu cæng thµnh bÞ vì b×nh giang
M_LONGTRULD			= 42	--value 1 ®¸nh dÊu trô thuéc vÒ phe thñ value = 2 thuéc vÒ phe c«ng
M_LONGTRUDX		= 43	--value 1 ®¸nh dÊu trô thuéc vÒ phe thñ value = 2 thuéc vÒ phe c«ng
M_LONGTRUBG			= 44	--value 1 ®¸nh dÊu trô thuéc vÒ phe thñ value = 2 thuéc vÒ phe c«ng
MS_CALLNPCCOUNT_S = 45 --value sö dông trong tèng kim binh sü hiÖu phï
MS_CALLNPCCOUNT_J = 46 --value sö dông trong tèng kim binh sü hiÖu phï
M_VITRI_TRENDUOI	= 47 --value sö dông trong tèng kim x¸c ®Þnh vÞ trÝ trªn d­íi 2 phe
M_NPCIDX_SOAITONG = 48 --value sö dông l­u npcidx so¸i tèng
M_NPCIDX_SOAIKIM = 49 --value sö dông l­u npcidx so¸i kim

--*******************************************************************************
							-- GLBMISSION --
--*******************************************************************************
GBM_VALUE_1				= 1
GBM_VALUE_2				= 2
GBM_VALUE_3				= 3
GBM_VALUE_4				= 4
GBM_VALUE_5				= 5
GBM_VALUE_6				= 6
GBM_VALUE_7				= 7
GBM_VALUE_8				= 8
GBM_VALUE_9				= 9
GBM_VALUE_10			= 10 --10 id dau tien 10 map vuot ai
GM_BenThuyen1				= 11
GM_BenThuyen1				= 12
GM_BenThuyen1				= 13
M_LienDau_DonDau		= 14
M_SoNguoiLDTrung		= 15
M_SoNguoiLDCao			= 16

CSP_CTYPE_NONE			= 0 -- kiÓu lªn ®¶o tÈy tuû
CSP_CTYPE_PROP			= 1 -- tÈy tiÒm n¨ng
CSP_CTYPE_SKILL			= 2 -- tÈy kü n¨ng
CSP_CTYPE_ALL			= 3 -- tÈy c¸c lo¹i

function CheckPlayerTitle()
	local nPTitleID = GetTask(T_TITLE_ID) 
	local nPTitleTime = GetTask(T_TITLE_TIME) 
	if(nPTitleID> 0 and nPTitleTime>GetTimeZero()) then
		SetCurPlayerTitle(GetTask(T_TITLE_ID))
	else --Khi hÕt thêi gian
		SetPlayerTitle(GetTask(T_TITLE_ID),1,1)
	end
end

MISSION_BR_PARAM_KILLS = 0