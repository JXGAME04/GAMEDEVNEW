-- ================================================================================================
-- [HE THONG] script/header/loginao.lua
-- Muc dich  : Map/ao dang nhap.
-- Duoc nap  : Include tu 1 tep (vd timerserver.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): Active_LogginAo (7)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
TAB_NAMEAO= {
"Thiªn","Kim","Hoµng","Ngu","ãc","Chã","§ång","Ngäc","T©m","Léc","Lý","Thó","Chïa","ThÇy","ThÇn","Kinh","ThiÕu","L©m","Tù","NghÌo","NhÊt","Xãm",
"Xµm","v«","®èi","thô","d©m","tÆc","ng­êi","l¹","cµy","tiÒn","H¹","Minh","Sang","D©m","ThiÕt","Méc","Thñy","Háa","Thæ","M¹nh","Kha","§Þnh","NguyÔn","Th«ng",
"Quang","Vò","Nhßa","Nh¹t","Linh","LÖ","Nam","Th¾ng","Chales","Ronando","messi","HuÊn","Hoa","Hång","Huúnh","Phong","C¸","Bµ","¤ng","Ký","øc","Mét","Thêi","Vâ","L©m",
"§iÓu","Hå","Ngäc","Hµ","Mü","T©m","S¬n","Tïng","MTP","HSZ","GL","PK","Tay","Ch¬i","Bè","Giµ","Nhi","Trô","V­¬ng","Liªn","B×nh"
}


function Active_LogginAo()
	local TAB_NAME ={}
	for i = 1,random(2,3) do
		local Name = random(1,getn(TAB_NAMEAO))
		local FistName = random(1,getn(TAB_NAMEAO))
		local LastName = random(1,getn(TAB_NAMEAO))
		TAB_NAME={Name,FistName,LastName};
		--Msg2SubWorld("<color=white>§¹i hiÖp:<color> <color=yellow>"..TAB_NAMEAO[TAB_NAME[1]].." "..TAB_NAMEAO[TAB_NAME[2]].." "..TAB_NAMEAO[TAB_NAME[3]].." <color><color=white>§· tham gia Ng¹o ThÕ <color>");
		Msg2SubWorld("<color=white>Cao Thñ: <color><color=yellow>"..TAB_NAMEAO[TAB_NAME[1]].." "..TAB_NAMEAO[TAB_NAME[2]].." "..TAB_NAMEAO[TAB_NAME[3]].." <color=white> ®· gia nhËp m¸y chñ Ký Sù giang hå s¾p dËy lªn mét phen sãng giã ! <color>");
	end
	TAB_NAME ={};
end
	