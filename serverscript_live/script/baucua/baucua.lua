

function thaycai(playerName)

	Msg2SubWorld("<color=Green>Ng­êi ch¬i <color><color=yellow>" .. playerName .. "<color=white> ®· trë thµnh <color=cyan>chñ c¸i bµn ch¬i!<color><color=white> tham gia ngay ®Ó thö vËn may!")
end

mat = {
	"Nai",
	"BÇu",
	"Gµ",
	"C¸",
	"Cua",
	"T«m",
}
	
function win(str)
	dofile("script/baucua/baucua.lua")

	if(str) then
	local tmp = str
	
		local player_name = StrSplit(str, 0)
		local win_face = tonumber(StrSplit(str, 1))
		local win_amount = StrSplit(str, 2)
		
		--tinh toan vat pham yeu cau
		Msg2SubWorld("<color=yellow>"..player_name.."<color> ®· ®Æt tróng con <color=green>"..mat[win_face+1]..".<color>\n§· ¨n ®­îc <color=cyan>"..win_amount.." xu.<color> Xin chóc mõng !")

		-- Msg2SubWorld(player_name.." "..mat[win_face+1].." "..win_amount.." xu");
		
	end
end
	
	