Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_server.lua")
Include("script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")


function main()
dofile("script/tinhnang/loidaihonchien/mainloidai.lua")
SayEx({"<color=green>Hoµng D­îc S­ <color>: Ta ë ®©y ®Ó phôc vô nh÷ng vÞ anh hïng dòng c¶m, d¸m ®­¬ng ®Çu víi nguy nan. Ng­¬i cã thùc sù muèn chóng tá m×nh kh«ng?",
	"Ta muèn b¸o danh L«i §µi Hçn ChiÕn/thamgialoidai",
	"Ta chØ ®Õn th¨m «ng./no"})
end
cost = 0
function thamgialoidai()
gio = tonumber(date("%H"))
phut = tonumber(date("%M"))
--if GetTask(139) == 0 then
--Talk(1,"","Nh©n vËt chuyÓn sinh 0 kh«ng thÓ tham gia")
--return
--end
if GetLevel() < 90 then
Talk(1,"","§¼ng cÊp d­íi 90 kh«ng thÓ tham gia L«i §µi Hçn ChiÕn")
return
end
--if gio == 11 and GetLevel() > 115 then
--	Talk(1,"","TrËn 11 giê chØ cho nh©n vËt cÊp 95 - 115 tham gia")
--return
--end

if (gio == 16 or gio == 22) and phut >= 00  and phut < 59 then

	if GetCash() >= cost then
		SetDeathScript("\\script\\tinhnang\\loidaihonchien\\bigiet.lua");
		SetFightState(0) -- tr¹ng th¸i chiÕn ®Êu
		LeaveTeam()	-- out paty
		SetCreateTeam(0) --tat tinh nang to doi
		SetPKMode(2,1) -- chuyÓn sang ®å s¸t
		-- SetCurCamp(0) -- chuyÓn qua mµu tr¾ng
		SetCurCamp(4) -- chuyÓn qua mµu ®á
		SetRevPos(53,19); --sau khi ®¨ng nhËp l¹i vÒ ba l¨ng huyÖn
		SetTempRevPos(53, 51904, 102048)--ket thuc thiet lap diem hoi sinh o Ba Lang Huyen
		SetLogoutRV(1)--dung diem phuc sinh dang nhap
		SetPunish(1)	--bat tinh nang chet khong mat' gi`
		SetMask(2019)
		SetTaskTemp(1, 0)
		NewWorld(210, 1628 , 3213);	-- [BW 23/08] nhuong 209 cho Loi dai ty vo (bw); 210 cung map data, Tho Dia Phu da chan

		Msg2Player("§¨ng ký b¸o danh thµnh c«ng. H·y t×m vÞ trÝ thuËn lîi cho m×nh ®Ó c«ng kÝch.")
		Msg2SubWorld("<color=pink>"..GetName().." ®· b¸o danh L«i §µi Hçn ChiÕn")
	else
		Talk(1,"","Ng­¬i kh«ng mang ®ñ "..cost.." l­îng, kh«ng thÓ tham gia")
	end

else
	Talk(1,"","Thêi gian b¸o danh L«i §µi Hçn ChiÕn vµo lóc 14h00 vµ 22h00")
end
end
