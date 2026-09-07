-- ================================================================================================
-- [HE THONG] script/log_game/save_log.lua
-- Muc dich  : HAM GHI LOG CHUNG cua game (21 ham) - log_game/*.lua dung.
-- Duoc nap  : Include tu 41 tep (vd lib.lua, quahuyhoang.lua, lib_vt.lua, lib.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): logTBHK (4), logEventThang1 (11), logEventThang10 (18), logPhanThuongToiDa (25), logTriAn (32), logTOP10 (39), logHack (46), logGiftCode (53), logCreatGiftCode (60), logCreatCardCode (67), logWorldCup (74), logWriteLogin (81), logWriteTrade (88), logWriteBind (95), logWriteKTC (102), logNapThe (109), logDoiNhacVuongKiem (116), logDoiKNB (123), logHoatDong (130), logRutXu (137), logRutXuTK (144)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
--Author: Fong KiÒu
--Date: 2021
--Function: Lib Save All Log GS

function logTBHK(str)
	local gm_Log = "dulieu/event/TrangBiHK.txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t"..str.."\n")
	closefile(fs_log)
end

function logEventThang1(str)
	local gm_Log = "dulieu/event/SuKienThang1.txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t"..str.."\n")
	closefile(fs_log)
end

function logEventThang10(str)
	local gm_Log = "dulieu/event/SuKienThang10.txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t"..str.."\n")
	closefile(fs_log)
end

function logPhanThuongToiDa(str)
	local gm_Log = "dulieu/event/PhanThuongToiDa.txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t"..str.."\n");
	closefile(fs_log)
end

function logTriAn(str)
	local gm_Log = "dulieu/event/PhanThuongTriAn.txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t"..str.."\n")
	closefile(fs_log)
end

function logTOP10(str)
	local gm_Log = "dulieu/event/Top10DuaTOP.txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t"..str.."\n")
	closefile(fs_log)
end

function logHack(str)
	local gm_Log = "dulieu/event/HackGame.txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t"..str.."\n")
	closefile(fs_log)
end

function logGiftCode(str)
	local gm_Log = "dulieu/event/Giftcode.txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t"..str.."\n")
	closefile(fs_log)
end

function logCreatGiftCode(str)
	local gm_Log = "dulieu/event/CreatGiftCode.txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, str)
	closefile(fs_log)
end

function logCreatCardCode(str)
	local gm_Log = "dulieu/thenap/CreatCardCode.txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, str)
	closefile(fs_log)
end

function logWorldCup(str)
	local gm_Log = "dulieu/event/EventWorldCup.txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t"..str.."\n")
	closefile(fs_log)
end

function logWriteLogin(str)
	local gm_Log = "dulieu/player_log/log_game/"..date("%d_%m_%Y")..".txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, str)
	closefile(fs_log)
end

function logWriteTrade(str)
	local gm_Log = "dulieu/player_log/log_trade/"..date("%d_%m_%Y")..".txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, str)
	closefile(fs_log)
end

function logWriteBind(str)
	local gm_Log = "dulieu/player_log/log_bind/"..date("%Y_%m_%d")..".txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t"..str.."\n")
	closefile(fs_log)
end

function logWriteKTC(str)
	local gm_Log = "dulieu/player_log/log_ktc/"..date("%Y_%m_%d")..".txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, str)
	closefile(fs_log)
end

function logNapThe(str)
	local gm_Log = "dulieu/thenap/TheNap.txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t"..str.."\n")
	closefile(fs_log)
end

function logDoiNhacVuongKiem(str)
	local gm_Log = "dulieu/event/doinhacvuongkiem.txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t"..str.."\n")
	closefile(fs_log)
end

function logDoiKNB(str)
	local gm_Log = "dulieu/event/doiknb.txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t\t"..str.."\n")
	closefile(fs_log)
end

function logHoatDong(str)
	local gm_Log = "dulieu/player_log/log_hoatdong/"..date("%Y_%m_%d")..".txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t\t"..str.."\n")
	closefile(fs_log)
end

function logRutXu(str)
	local gm_Log = "dulieu/player_log/log_rutxu/"..date("%Y_%m_%d")..".txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t\t"..str.."\n")
	closefile(fs_log)
end

function logRutXuTK(str)
	local gm_Log = "dulieu/player_log/log_rutxutk/"..date("%Y_%m_%d")..".txt"
	local fs_log = openfile(gm_Log, "a")
	write(fs_log, date("%H:%M:%S_%d-%m-%y").."\t\t"..str.."\n")
	closefile(fs_log)
end