--Author: Fong Ki“u
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