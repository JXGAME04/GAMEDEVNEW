--hoangnhk
------TaskTemp------------------
ITEMINDEX													= 0
NPCINDEX													= 1
DESPLAYERID 											= 3	--for gm
SELSKILLNO 												= 4	--skill cua phai' so' x
HAVELOVE_MALE 									= 5		--Dang cau hon
HAVELOVE_FEMALE 								= 6		--playerindex cua nam
SEL_MOVEARRAY 									= 7			--lua chon mang~di chuyen
SEL_MOVEBEGIN 										= 8			--lua chon di chuyen
																		--su dung	8->18
TEMP_TASK 												= 19
-------TASKID (loai lon)------------------
TASK_NAME 												= 0--ten nhan vat
TASK_LVGAMESEC 									= 1-- thoi gian roi mang
TASK_LVGAMEEXP 									= 2 --kinhnghiem tich luy roi mangcua nv
TASK_SHXT 												= 10
TASK_DUNGCHUNG 								= 11	--[1]da hoan thanh datau truoc do(phan thuong)
																					--[2]ben kia bo phong lang do
																					--[3][4]lan dat cuoc
																					--[5][6]lan danh khinh cong
																					--[7]loai khinh cong
																					--[8]nv heo rung thanh do
																					--[9]thanh thi dau thu

TASK_DUNGCHUNG2 								= 4134	-- [FIX TRUNG TASK 24/08] truoc la 12; 12 = T_TIMDOCHI (39 cho dung) 	--[1][2]an may thanh do[3][4]giet heo rung thanh do[5]dao nhan que[6]so lan giao thit[7][8]yeu cau thit tuoi[9]xuat su
TASK_DATAU_OLD 									= 13
TASK_DIADOCHI 										= 14	--[1][2][3]b¶n ®å --[4][5][6]sè l­îng t×m ®c [7][8][9]level cua update kinh nghiem
TASK_DATAU_NEED 								= 15
TASK_DATAU_ACCEPT 							= 16	--[1]NhËn nhiÖm vô [2]Lo¹i nhiÖm vô : 0: kh«ng giao, 1: cã giao vËt phÈm [3]®é khã 
																					--[4][5][6]controng [7][8] loai update [9] loai item giao
TASK_DATAU_NUM 								= 17
TASK_LIENDAU_TIME 							= 18	--mua dau thu n

--------------------------------
TASK_NVST 												= 21	--[1][2]nhan nv [3][4][5][6][7][8]NVLongngu [9]thanh thi return
TASK_TANTHU 											= 22		--[1]nhan item [2]hocskill9x[3]nhantop[4]testgame[5]ho tro tao bang hoi[6][7][8][9]thuongcap
TASK_NVDANHVONG 							= 23	--[1]nv thit heo[2]mieng thit [3]nv hai thuoc [4]noiduathuden [5]noibatdau dua thu	[6]aphuong [7]thuongquanthu [8]anmaylongmon [9]phu-hotu
TASK_THOIGIAN 										= 24	--THOI GIAN heo rung
TASK_THOIGIAN2 									= 25	--THOI GIAN dichquan
TASK_LIENDAU 										= 26	--id data
TASK_MOC_DATAU 								= 28	--nhan thuong moc da tau 9 o^

TASK_THOIGIAN3 									= 30 --THOI GIAN nv heo rung thanh do
TASK_DATCUOCT 									= 31	--vong thi thu' n
TASK_DATCUOC1 										= 32	--phu quy cam hap
TASK_DATCUOC2 										= 33	--phu quy cam hap
TASK_DATCUOC3 										= 34	--phu quy cam hap
TASK_DATCUOC4 = 35	--phu quy cam hap
TASK_DATCUOC5 = 36	--phu quy cam hap
TASK_DATCUOC6 = 37	--phu quy cam hap
TASK_THOIGIAN4 = 38	
TASK_THOIGIAN5 = 40 --THOI GIAN hai thuoc

TASK_THOIGIAN6 = 42 --[1][2]luu ngay de resettask

TASK_NVTHONGTHUONG = 44 --[1][2][3][4]id quai nv thong thuong
TASK_RESETWEEK = 45 --[9]thu hien tai
TASK_THOIGIAN7 = 47 --xoa mat ma
TASK_THOIGIAN8 = 48 --THOI GIAN nv thit tuoi
TASK_THITTUOI = 49 -- so luong thit tuoi
TASK_NVDANHVONG2 = 50 --[1][2][3][4][5][6][7]thoi gian nv lien ket vo lam[8]ngolaogia-giangtan[9]gianna-hotu
TASK_THOIGIAN9	= 52 --THOI GIAN nhan danh vong
TASK_DUNGCHUNG3 = 53 --[1]tui tu mau[3][4]nv thong thuong[5]phe hien tai[7][8][9]ngoi tu
TASK_NVDANHVONG3 = 54 --[1][2][3][4]tim hieu cac mon phai [5]nv lien ket vo lam

--cac du lieu data int--
DATA_DICEMONEY = 0 -- tien chu song bac
DATA_DUATOP = 11
DATA_DATCUOC = 12 --dat cuoc
DATA_DCHAVERES = 13 --dat cuoc
DATA_DCBEGIN = 21 --dat cuoc
DATA_DCEND = 30 --dat cuoc
DATA_MAPVUOTAI	= 40 --40-46