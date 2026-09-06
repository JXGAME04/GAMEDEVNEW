VKPHATSANG = {
	[1] = 32, -- ®ao mµu vµng s¸ng
	--[1] = 34, -- ®ao mµu xanh s¸ng
	--[2] = 35, -- ®ao mµu tr¾ng s¸ng
	--[5] = 86, -- ®ao 15x ®Ñp
	[0] = 37, -- kiÕm mµu tr¾ng s¸ng
	--[5] = 109, -- kiÕm xanh ®Ñp	
	--[4] = 38, -- kÝch mµu tr¾ng s¸ng
	[3] = 39, -- kÝch mµu löa nung
	[2] = 40, -- bæng mµu vµng s¸ng hoµng kim
	--[5] = 41, -- bæng mµu löa nung
	--[5] = 85, -- bæng 15x ®Ñp
	[4] = 43, -- chuú ph¸t s¸ng tr¾ng
	--[5] = 87, -- chuú 15x ®Ñp
	[5] = 29, -- song ®ao mµu tÝm ®Ñp
}

--itempart_head = 0,	// Í· // mò
--itempart_body,		// ÉíÌå //¸o
--itempart_belt,		// Ñü´ø // ®ai l­ng
--itempart_weapon,	// ÎäÆ÷ // vò khÝ
--itempart_foot,	//giµy
--itempart_cuff, //bao tay
--itempart_amulet, //d©y chuyÒn
--itempart_ring1,	//nhÉn
--itempart_ring2,
--itempart_pendant,  //ngäc béi
--itempart_horse, // ngùa
--itempart_mask,	// mat na
--itempart_mantle,//#phi phong
--itempart_num,

--pos_hand = 1,		// tren tay
--pos_equip,			// mang tren nguoi		
--pos_equiproom,		// 	//hanh trang
--pos_repositoryroom,	//  ruong
--pos_exbox1room,		// mo rong ruong 1
--pos_exbox2room,		// mo rong ruong 2
--pos_exbox3room,		// mo rong ruong 3
--pos_equiproomex,	// mo rong hanh trang
--pos_traderoom,		// giao dich
--pos_trade1,			// 
--pos_immediacy,		// o phim tat
--pos_give,//
--pos_affairitem, //
--pos_tremble,		// Kham nam/nang cap trang bi xanh
--pos_compone,		// Luyen huyen tinh khoang thach
--pos_comptwo,		// Nang cap huyen tinh khoang thach 
--pos_compthree,		// Nang cap khoang thach thuoc tinh
--pos_distill,		// Rut Option trang bi
--pos_forge,			// Che tao trang bi tim 
--pos_enchase,		// Kham nam trang bi
--pos_skilltakewith,	//skill phim tat index 21 moi nhac len
--pos_immediacyskill, //skill phim tat index 22 khi dat xuong
--pos_num,

--Fong Kieu thay ®æi ngo¹i h×nh trang bÞ

function main(nItemIdx)
	local pos_equipfk = 2
	local itempart_weaponfk = 3
	-- dofile("script/item/change_item_res.lua")
	local nEQIndex,nItemGenre,nDetail,nParticur,nEQLevel = FindItemEx(pos_equipfk, itempart_weaponfk)
	if(nEQIndex <= 0) then
		return
	end
	if(nEQLevel == 10 and nParticur >= 0 and nParticur <= 5) then
		if(RemoveItem(nItemIdx,1)==1) then
			SetGlowLightItem(nEQIndex, VKPHATSANG[nParticur])
		end
	else
		Talk(1,"","ChØ cã thÓ thay ®æi vò khÝ cËn chiÕn cÊp 10")
	end
end