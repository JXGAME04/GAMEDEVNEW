-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local LogDangXuat
--Author: Fong Ki“u
--Date: 2021
--Function: Player tho∏t kh·i game

Include("\\script\\log_game\\save_log.lua")
-- [TIN SU 25/08] noi mat xich logout cua Linux (global\logout.lua:9,28-31):
-- thoat game trong ai 387-395 phai don trang thai + doi 1203: 20->21 de lan sau
-- 'Tiep tuc nhiem vu' bat lai SetFightState. Ham tu kiem map ben trong.
Include("\\script\\task\\tollgate\\messenger\\messenger_lievegame.lua")

function main()
	LogDangXuat()
	messenger_livegame()	-- [TIN SU 25/08] tu kiem map 387-395 ben trong
end

function OnLogout()
	
end

function LogDangXuat()
	local szAccount = GetAccount()
	local szName = GetName()
	local dwID = 0
	local nLevel = 0
	local nIP = 0
	local nExtPoint = 0
	local nMoney = 0
	local log = date("%H:%M:%S_%d-%m-%y").."\tLogout Tµi kho∂n: "..szAccount.."\t Nh©n vÀt: "..szName.."\t HWID ID: "..dwID.."\t C p: "..nLevel.."\t IP: "..nIP.."\t Xu: "..nExtPoint.."\t TÊng ti“n vπn: "..nMoney.."\n"
	logWriteLogin(log)
end
