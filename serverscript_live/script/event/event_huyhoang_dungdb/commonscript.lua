
--Map Huy Hoang Tieu
-- 182 Nghiet Long Dong
-- 167 Diem Thuong Son
-- 200 Co Duong Dong
-- 92 Thuc Cuong Son

--Map Huy Hoang Trung
-- 321 Truong Bach son Nam
-- 322 Truong Bach son Bac
-- 225 Sa Mac 1
-- 226 Sa Mac 2
-- 227 Sa Mac 3

--Map Huy Hoang Dai
-- 340 Mac Cao Quat
-- 336 Phong Lang Do

Include("script\\event\\event_huyhoang_dungdb\\lib_huyhoang.lua")
Include("\\script\\lib\\lib_ham.lua")

function QuickTestHuyHoang()
    SayEx({"Function: Chøc n¨ng kiÓm tra nhanh b¶n ®å Huy Hoµng !",
        "Thªm H¹t Huy Hoµng/Add_HatHuyHoang",
		"§i §iÓm Th­¬ng S¬n/diemthuongson",
		"§i Sa M¹c TÇng 3/samac3",	
		"§i M¹c Cao QuËt/maccaoquat",
		"§i L©m Du Quan/lamduquan",		
        --"B¶n ®å Huy Hoµng TiÓu/MapHuyHoangTieu",
        --"B¶n ®å Huy Hoµng Trung/MapHuyHoangTrung",
        --"B¶n ®å Huy Hoµng §¹i/MapHuyHoangDai",
        "Tho¸t/Exit"})
end

function DenMapHuyHoang()
    SayEx("Ng­¬i muèn ®i ®©u ?",
	"§iÓm Th­¬ng S¬n/diemthuongson",
	"Sa M¹c TÇng 3/samac3",	
	"M¹c Cao QuËt/maccaoquat",
	"L©m Du Quan/lamduquan",
	"Kh«ng ®i/Exit")
end

function MapHuyHoangTieu()
    Say("Ng­¬i muèn ®Õn ®©u ?", 3,
    "Ch©n nói tr­êng b¹ch/channuitb",
    --"§iÓm Th­¬ng S¬n/diemthuongson",
    --"Cæ D­¬ng §éng/coduongdong",
    --"Thôc C­¬ng S¬n/thuccuongson",
    "Tho¸t/exit"
   )
end

function MapHuyHoangTrung()
    Say("Ng­¬i muèn ®Õn ®©u ?", 3,
        --"Tr­êng B¹ch S¬n Nam/truongbachsonnam",
        "Tr­êng B¹ch S¬n B¾c/truongbachsonbac",
        --"Sa M¹c TÇng 1/samac1",
        --"Sa M¹c TÇng 2/samac2",
        --"Sa M¹c TÇng 3/samac3",
        "Tho¸t/Exit"
    )
end

function MapHuyHoangDai()
    Say("Ng­¬i muèn ®i ®©u ?", 2,
        "M¹c Cao QuËt/maccaoquat",
        --"Phong L¨ng §é/phonglangdo",
        "Tho¸t/Exit"
    )
end

function lamduquan()
	NewWorld(319, 1653, 3546) SetFightState(1)
end

function channuitb()
	NewWorld(320, 43532/32,94632/32) SetFightState(1)
end

function nghietlongdong()
   NewWorld(182, 1896, 3460) SetFightState(1)
end

function diemthuongson()
    NewWorld(167, 1517, 2467) SetFightState(1)
end

function coduongdong()
    NewWorld(200, 1610, 3170) SetFightState(1)
end

function thuccuongson()
    NewWorld(92, 1632, 3292) SetFightState(1)
end

function truongbachsonnam()
    NewWorld(321, 972, 2324) SetFightState(1)
end

function truongbachsonbac()
    NewWorld(322, 1584, 3155) SetFightState(1)
end

function samac1()
    NewWorld(225, 1584, 3194) SetFightState(1)
end

function samac2()
    NewWorld(226, 1584, 3172) SetFightState(1)
end

function samac3()
    NewWorld(227,1587,3122) SetFightState(1)
end

function phonglangdo()
    NewWorld(336, 1590, 2520) SetFightState(1)
end

function maccaoquat()
    NewWorld(340, 1859,2761) SetFightState(1)
end

function gotobalang()
    NewWorld(53,1581,3242) SetFightState(1)
end

function exit()
end
