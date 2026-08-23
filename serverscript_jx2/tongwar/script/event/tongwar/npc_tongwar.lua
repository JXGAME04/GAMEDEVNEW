-- -- [TONGWAR 23/08] NPC loi vao "Vo Lam Truyen Nhan" (MOI - ban Linux KHONG co loi vao:
-- tongWar_Start 0 call site, DIEUKIEN da chung minh). Moi muc con theo pha 1/2/3 da nam san
-- trong tongWar_Start (event\tongwar\head.lua:169-207).
Include("\\script\\event\\tongwar\\head.lua")
Include("\\script\\event\\tongwar\\npc_shizhe.lua")

function main()
	tongWar_Start()
end
