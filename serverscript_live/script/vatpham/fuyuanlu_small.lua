--Author: Fong KiÒu
--Date: 2021
--Function: Item Phuc Duyen Lo Trung

Include("\\script\\header\\fuyuan.lua")

function main(nItemIdx)
	
	FuYuan_Start()

	if( FuYuan_Add( 10 ) ~= 1 ) then
		Msg2Player( "B¹n t¹m thêi ch­a thÓ sö dông Phóc Duyªn, tr­íc tiªn h·y ®i t×m Vâ L©m Minh Chñ TruyÒn Nh©n ë c¸c thµnh thÞ lín ®Ó më c«ng n¨ng nµy. " )
		return 1
	end	
	
	RemoveItem(nItemIdx,1)
end