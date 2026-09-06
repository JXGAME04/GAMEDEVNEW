Include("\\script\\chienlenh\\cl_def.lua")	-- [CL 04/09] so hieu nhiem vu Chien Lenh
-- onkillnpc.lua - [DONDATAU 30/08]
-- MOC "nguoi choi giet mot NPC". Duoc goi tu C++ trong KNpc::OnDeath
-- (khai bao KNpc.cpp:1648, loi goi :1702) - mot lan cho MOI xac NPC
-- do nguoi choi ha. LUU Y: KNpc::Die la ham KHAC (ket thuc :1646).
--
-- Truoc day moc nay o nho trong script\tinhnang\datau\danhquai.lua cua he
-- Da Tau CU. He do da go (ban thay = task\newtask\tasklink, port 15-16/08),
-- nhung phan dem giet quai cua BAN DONG HANH thi van phai chay moi lan giet
-- quai - nen tach ra day, khong con dinh gi toi Da Tau.
--
-- CHU Y HIEU NANG: ham nay chay MOI LAN co NPC chet vi tay nguoi choi.
-- Dung them viec nang vao day.

Include("\\script\\task\\partner\\bdh_killhook.lua")

function OnPlayerKillNpc(nNpcIndex, nDamageIndex)
	local nPlayerIndex = NpcIdx2PIdx(nDamageIndex)
	if (nPlayerIndex == 0) then	-- quai giet quai thi thoi
		return
	end
	PlayerIndex = nPlayerIndex
	call(BDH_OnKillNpc, {nNpcIndex}, "x")
	CL_CongNhom(CL_NHOM_QUAI, 1)	-- [CL 04/09] nhiem vu diet 500/1000/3000 quai
end

-- Ten cu, giu lai cho DLL chua swap (KNpc.cpp:1698 van goi ten nay).
function OnDeathMonsterDaTau(nNpcIndex, nDamageIndex)
	OnPlayerKillNpc(nNpcIndex, nDamageIndex)
end

-- [PB 30/08] da bo "function OnRevive() end" - MA CHET.
-- KNpc.cpp:8929 phat OnRevive qua bien ActionScript cua tung NPC,
-- khong qua tep nay, nen ham do khong bao gio duoc goi.
