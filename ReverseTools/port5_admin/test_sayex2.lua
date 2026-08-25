-- mo phong SayEx (lib_ham.lua) tren Lua 4.0.1 that; ket qua ban ra qua error()
KQ = ""
function Say(...)
	KQ = KQ .. "[Say nhan " .. arg.n .. " tham so]"
	local i
	for i = 1, arg.n do
		KQ = KQ .. " <<" .. tostring(arg[i]) .. ">>"
	end
end

function SayEx(strSay)
	local strMsg, strSel = "", ""
	local strNum = getn(strSay)
	if strNum < 2 then return end
	if strNum > 2 then
		local i
		for i = 2, strNum - 1 do
			strSel = strSel .. format("%q", strSay[i]) .. ","
		end
		strSel = strSel .. format("%q", strSay[strNum])
		strMsg = "Say(" .. format("%q", strSay[1]) .. "," .. (strNum - 1) .. "," .. strSel .. ");"
	elseif strNum == 2 then
		strSel = format("%q", strSay[strNum])
		strMsg = "Say(" .. format("%q", strSay[1]) .. ",1" .. "," .. strSel .. ");"
	end
	KQ = KQ .. "{dostring dai " .. strlen(strMsg) .. "}"
	dostring(strMsg)
end

function nt_getTask(n) return 20 end
function GetFightState() return 0 end

SayEx({"<color=yellow>Test Tin Su<color> - 1203="..nt_getTask(1203).." | RutVuKhi="..GetFightState().." (0 = khong danh duoc quai + khong the chet)",
"1. Xem trang thai chi tiet/HD_TS_Xem",
"2. Go ket: 20 sang 21 roi toi Dich Quan bam Tiep tuc/HD_TS_GoKet",
"3. Bat trang thai chien dau NGAY - test nhanh trong ai/HD_TS_BatFight",
"4. Huy sach nhiem vu Tin Su - nhu NPC/HD_TS_Huy",
"5. Den ai 3 Thien Bao Kho - map 395/HD_TS_DenAi",
"6. Ve Ba Lang Huyen - don trang thai nhu NPC/HD_TS_VeThanh",
"Ket thuc doi thoai/no"})

error("KETQUA: " .. KQ)
