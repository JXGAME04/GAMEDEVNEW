-- cl_thehaohoa.lua - [CL 04/09] The CHIEN LENH HAO HOA (ma vat pham 4977).
--
-- LUAT QUAN TRONG NHAT CUA TEP NAY:
--   Goi CL_MuaVip() TRUOC, CHI KHI no tra ve 1 moi duoc xoa the.
--   Tra ve 2 nghia la nhan vat DA CO Hao Hoa roi -> KHONG duoc tieu the, neu khong
--   nguoi choi mat 500 xu ma khong duoc gi (mua hai the la mot ca co that: khong
--   co gi chan nguoi ta mua hai lan o Ky Tran Cac).
--
-- CL_MuaVip nam trong Core\Src\KChienLenh.cpp: no tu kiem mua dang mo, tu doc
-- trang thai nguoi choi, tu cong diem thuong kich hoat va tu xa xuong MySQL.

if (MODEL_GAMECLIENT == 1) then
	return
end

function main(nItemIdx)
	if (CL_MuaVip == nil) then
		Msg2Player("M∏y chÒ ch≠a mÎ Chi’n L÷nh, ch≠a dÔng thŒ nµy Æ≠Óc.")
		return
	end
	local nKq = CL_MuaVip()
	if (nKq == 1) then
		-- CHI xoa the khi da bat duoc that su
		RemoveItemByIndex(nItemIdx, 1)
		Msg2Player("ß∑ k›ch hoπt Chi’n L÷nh Hµo Hoa! Toµn bÈ ph«n th≠Îng nh∏nh Hµo Hoa Æ∑ mÎ kho∏.")
		GhiLog("HE", format("[CL] %s kich hoat the Hao Hoa", GetName()))
	elseif (nKq == 2) then
		-- KHONG xoa the
		Msg2Player("Nh©n vÀt nµy Æ∑ k›ch hoπt Chi’n L÷nh Hµo Hoa rÂi - thŒ v…n cﬂn trong hµnh trang.")
	else
		Msg2Player("Ch≠a dÔng Æ≠Óc: mÔa Chi’n L÷nh ch≠a mÎ ho∆c Æ∑ k’t thÛc.")
	end
end
