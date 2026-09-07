-- ================================================================================================
-- [HE THONG] script/global/TalkLib.lua
-- Muc dich  : Thu vien hoi thoai (Say).
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)
-- Ham (dong): talkdefault (4), ketthuc (8)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================


print("=====[ Script TalkLib loaded ]=====");

function talkdefault()
Say("Chµo mõng ®Õn víi VLTK: <color=red>C«ng Thµnh ChiÕn 2003<color><enter>Phiªn b¶n gèc thuéc b¶n quyÒn cña <color=green>Kingsoft<color> <enter>§­îc ViÖt Hãa vµ ph¸t triÓn Néi dung [G7VN Team] <enter><enter>+Trang chñ: <color=yellow>http://g7vn.net<color>",1,"KÕt thóc!/ketthuc");
end;

function ketthuc()
end