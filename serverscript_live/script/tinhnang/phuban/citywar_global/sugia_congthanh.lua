-- sugia_congthanh.lua - DOT E: NPC 'Su Gia Cong Thanh' (cua nop Khieu chien
-- lenh + xem dau gia + mua dung cu). Ban goc dat menu nay o ArenaMain()
-- trong infocenter_head.lua va gan cho NPC 'xa phu cong thanh' canh cac xa
-- phu thanh thi (npc-data cua ho); cay ta phai tu spawn nen can wrapper nay.
-- (infocenter.lua CHI la NPC chi duong - khong co menu, dung gan vao day.)
Include("\\script\\missions\\citywar_global\\infocenter_head.lua")

function main()
	ArenaMain();
end
