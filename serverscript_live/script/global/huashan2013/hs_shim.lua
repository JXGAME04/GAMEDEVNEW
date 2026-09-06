-- [HOASON 01/09] shim cho bo script Linux huashan2013 chay tren JX1 (ham/bien Linux khong co o JX1)
-- G_TASK (task\150skilltask\g_task.lua Linux): JX1 hoc ky nang 150 bang sach/NPC -> khong them muc menu
if not G_TASK then G_TASK = {} end
function G_TASK:OnMessage(szFaction, tbDailog, szFn) end
function G_TASK:Talk() end
-- task\lv120skill\head.lua Linux: task id da lam nhiem vu ky nang 120 (JX1 khong dung -> GetTask = 0)
LV120_SKILL_ID = 2463
-- misc\daiyitoushi\toushi_function.lua Linux (Dai Nghe Dau Su = doi phai): JX1 doi phai bang Lenh bai tan thu
function daiyitoushi_main(nFaction)
	Talk(1, "", "§¹i NghÖ §Çu S­ (®æi m«n ph¸i theo b¶n Linux) ch­a më. H·y dïng LÖnh Bµi T©n Thñ ®Ó chuyÓn ph¸i.")
end
-- item helper Linux -> JX1 (GetItemCount(nNature,g,d,p) / ConsumeItem(nCount,nNature,g,d,p))
function HaveCommonItem(g, d, p) return GetItemCount(0, g, d, p) end
function DelCommonItem(g, d, p) return ConsumeItem(1, 0, g, d, p) end
function IsNpcHide(nNpcIdx) return 0 end
function M2g2Player(s) return Msg2Player(s) end
function SetNpcAI(nNpcIdx, nType) return SetNpcAIType(nNpcIdx, nType) end
function ClearFactionRecord() return ClearFactionIfnfo() end
function no() end
