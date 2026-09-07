-- ============================================================================
-- npc_canhan.lua  [VTCN 06/09]  NPC nhanh CA NHAN (1-9 sao) cua Long Mon Tieu Cuc:
--   "Ong chu Tieu cuc Luc Tam Can" (2157, Long Mon tran) + 14 "Long Mon Tieu Su" (2145).
--
-- VI SAO CO TEP NAY: tasknpc.lua (dot port 06/09) goi LongMenBiaoJu:GetDlgClass() nhung
-- khong Include dialog.lua; hon nua lop LongMenBiaoJu.* giu so dang ky xe trong state
-- cua tasknpc.lua, con OnTimer/OnDeath cua xe chay o state cua biaoche.lua. JX1 moi tep
-- .lua la MOT lua_State rieng (Engine\Src\KLuaScript.cpp:23 lua_open) nen hai ben khong
-- thay nhau -> thoai NPC loi, xe khong di theo. Nhanh ca nhan GOC LINUX con mot ban
-- KHONG TRANG THAI (chi dung bien nhiem vu + NpcParam) nam san trong
-- config\129\extend.lua: ReceiveTaskLow / ReceiveTaskRandom / ReceiveTaskHigh /
-- ReceiveBiaoChe / ReceiveAward / ViewTaskInfor / Transport2Dest / AbandonTask.
-- Tep nay CHI noi thoai NPC vao cac ham do (Linux noi qua NpcFunLib:AddDialogNpc -
-- JX1 khong port). Lui ve ban cu: doi LMBJ_SCRIPT_CANHAN trong
-- script\startgame\lmbj_addnpc.lua.
--
-- Bien nhiem vu: 4178-4187 (variables.lua). Xe = CreateBiaoChe cua engine
-- (pActivity:refreshBiaoChe). Tieu Ky / Tan Lac Tieu Vat = npc_lmbiaoqi.lua /
-- npc_lmbiaowu.lua (deu dung NpcParam, khong can so dang ky Lua).
-- Cua hang Tieu cuc: doi Ho Tieu Lenh (6,1,4774) lay dao cu; gia doc qua HD_CFG
-- (khoa VT_GIA_KMGT / VT_GIA_KBKT / VT_GIA_TXDV), mac dinh 5/5/10 - so TU DAT
-- (Linux de gia trong lop LongMenBiaoJu bi mat), chu game duyet/chinh tren web.
-- ============================================================================
Include("\\script\\activitysys\\config\\129\\head.lua")
Include("\\script\\activitysys\\config\\129\\extend.lua")
Include("\\script\\activitysys\\config\\129\\variables.lua")
Include("\\script\\global\\maplist.lua")
Include("\\script\\header\\cauhinh_hoatdong.lua")

VTCN_ITEM_HTL = {6, 1, 4774}	-- Ho Tieu Lenh (magicscript.txt dong 4775)

VTCN_SHOP = {
	{szName = "Kho¸i M· Gia Tiªn", nP = 4775, szKhoa = "VT_GIA_KMGT", nGia = 5,  szMoTa = "t¨ng tèc Tiªu Xa 15 gi©y"},
	{szName = "Kiªn BÊt Kh¶ Táa",  nP = 4776, szKhoa = "VT_GIA_KBKT", nGia = 5,  szMoTa = "håi 10 phÇn tr¨m m¸u Tiªu Xa"},
	{szName = "Tiªu Xa Di VÞ",     nP = 4778, szKhoa = "VT_GIA_TXDV", nGia = 10, szMoTa = "kÐo Tiªu Xa vÒ chç m×nh ®øng"},
}

function main()
	local nNpcIndex = GetLastDiagNpc()
	local nSetting = GetNpcSettingIdx(nNpcIndex)
	-- [VTCN 06/09] chu game: cap ha xuong 90 (Linux 120)
	if PlayerFunLib:CheckTotalLevel(90, "Ng­¬i vÉn ch­a ®Õn cÊp 90, h·y cè g¾ng h¬n n÷a.", ">=") ~= 1 then
		return
	end
	pActivity:LogCurMapID()
	if nSetting == nNpcZhangGuiID then
		pActivity:VTCN_DlgChuongQuy()
	elseif nSetting == nNpcBiaoShiID then
		pActivity:VTCN_DlgTieuSu()
	end
end

-- Ong chu Tieu cuc Luc Tam Can (Long Mon tran): nhan / bo / thuong / thong tin / truyen tong / cua hang
function pActivity:VTCN_DlgChuongQuy()
	local szMsg = "<npc>Giang hå hiÓm ¸c, lßng ng­êi khã ®o¸n, nhê vËy mµ viÖc lµm ¨n cña Tiªu côc ta ngµy cµng tèt. HiÖn nh©n lùc kh«ng ®ñ, ta thÊy ®¹i hiÖp cèt c¸ch tinh c¬, chi b»ng gióp chóng ta vËn mét chuyÕn Tiªu Xa, phÇn th­ëng nhÊt ®Þnh lµm ng­¬i hµi lßng. Hoµn thµnh nhiÖm vô cßn nhËn ®­îc <color=yellow>Hé Tiªu LÖnh <color>®Ó ®æi ®¹o cô ë cöa hµng Tiªu côc."
	local tbOpt = {}
	tinsert(tbOpt, {"NhËn nhiÖm vô ¸p Tiªu", self.VTCN_DlgNhan, {self}})
	tinsert(tbOpt, {"Tõ bá nhiÖm vô ¸p Tiªu", self.AbandonTask, {self}})
	tinsert(tbOpt, {"NhËn phÇn th­ëng nhiÖm vô ¸p Tiªu", self.ReceiveAward, {self, 0}})
	tinsert(tbOpt, {"KiÓm tra th«ng tin ¸p Tiªu", self.ViewTaskInfor, {self}})
	tinsert(tbOpt, {"TruyÒn tèng ®Õn vÞ trÝ cña Tiªu Xa (1 v¹n l­îng)", self.Transport2Dest, {self}})
	tinsert(tbOpt, {"Cöa hµng Tiªu côc (®æi Hé Tiªu LÖnh)", self.VTCN_DlgShop, {self}})
	tinsert(tbOpt, {"§ãng"})
	CreateNewSayEx(szMsg, tbOpt)
end

function pActivity:VTCN_DlgNhan()
	if self:CheckReceiveTaskCondition() ~= 1 then
		return
	end
	local szMsg = "<npc>§¹i hiÖp rÊt can ®¶m! Ta cã c¸c lo¹i nhiÖm vô ¸p Tiªu, h·y l­îng søc mµ chän. GÇn ®©y trong rõng th­êng cã c­íp Tiªu, ¸p Tiªu Xa cao cÊp nhí cã tæ ®éi ®i cïng. Tiªu Xa bÞ c­íp th× víi ng­¬i hay ta ®Òu lµ tæn thÊt lín."
	local tbOpt = {}
	tinsert(tbOpt, {"NhËn nhiÖm vô ¸p Tiªu 1 sao", self.ReceiveTaskLow, {self}})
	tinsert(tbOpt, {"NhËn nhiÖm vô ¸p Tiªu ngÉu nhiªn (2 ®Õn 9 sao)", self.ReceiveTaskRandom, {self}})
	tinsert(tbOpt, {"NhËn nhiÖm vô ¸p Tiªu cao cÊp (cÇn ¸p tiªu ñy nhiÖm tr¹ng cao cÊp)", self.ReceiveTaskHigh, {self}})
	tinsert(tbOpt, {"§ãng"})
	CreateNewSayEx(szMsg, tbOpt)
end

-- Long Mon Tieu Su (14 diem): xuat phat / thuong / thong tin / truyen tong / cua hang
function pActivity:VTCN_DlgTieuSu()
	local szMsg = "<npc>Tiªu S­ cña Long M«n Tiªu Côc ®©y. NhËn nhiÖm vô ë ¤ng chñ Tiªu côc t¹i Long M«n trÊn råi ®Õn ®iÓm xuÊt ph¸t gÆp ta ®Ó Tiªu Xa lªn ®­êng; hé tèng xe tíi Tiªu S­ ë ®iÓm cuèi trong 30 phót ®Ó nhËn th­ëng."
	local tbOpt = {}
	tinsert(tbOpt, {"B¾t ®Çu ¸p Tiªu (cho Tiªu Xa xuÊt ph¸t)", self.ReceiveBiaoChe, {self}})
	tinsert(tbOpt, {"NhËn phÇn th­ëng nhiÖm vô ¸p Tiªu", self.ReceiveAward, {self, 1}})
	tinsert(tbOpt, {"KiÓm tra th«ng tin ¸p Tiªu", self.ViewTaskInfor, {self}})
	tinsert(tbOpt, {"TruyÒn tèng ®Õn vÞ trÝ cña Tiªu Xa (1 v¹n l­îng)", self.Transport2Dest, {self}})
	tinsert(tbOpt, {"Cöa hµng Tiªu côc (®æi Hé Tiªu LÖnh)", self.VTCN_DlgShop, {self}})
	tinsert(tbOpt, {"§ãng"})
	CreateNewSayEx(szMsg, tbOpt)
end

function pActivity:VTCN_DlgShop()
	local nCo = CalcItemCount(-1, VTCN_ITEM_HTL[1], VTCN_ITEM_HTL[2], VTCN_ITEM_HTL[3], -1)
	local szMsg = format("<npc>§¹i hiÖp gióp Tiªu côc hé tiªu nªn Tiªu S­ chóng ta ®· c¶i tiÕn vµi mãn ®¹o cô dïng cho Tiªu Xa, lÊy <color=yellow>Hé Tiªu LÖnh <color>ra ®æi. Ng­¬i ®ang cã <color=yellow>%d <color>Hé Tiªu LÖnh.", nCo)
	local tbOpt = {}
	for i = 1, getn(VTCN_SHOP) do
		local tb = VTCN_SHOP[i]
		local nGia = HD_CFG(tb.szKhoa, tb.nGia)
		tinsert(tbOpt, {format("%s - %s (%d Hé Tiªu LÖnh)", tb.szName, tb.szMoTa, nGia), self.VTCN_Mua, {self, i}})
	end
	tinsert(tbOpt, {"§ãng"})
	CreateNewSayEx(szMsg, tbOpt)
end

function pActivity:VTCN_Mua(i)
	local tb = VTCN_SHOP[i]
	if not tb then
		return
	end
	local nGia = HD_CFG(tb.szKhoa, tb.nGia)
	local nCo = CalcItemCount(-1, VTCN_ITEM_HTL[1], VTCN_ITEM_HTL[2], VTCN_ITEM_HTL[3], -1)
	if nCo < nGia then
		Talk(1, "", format("Hé Tiªu LÖnh cña ng­¬i kh«ng ®ñ %d c¸i.", nGia))
		return
	end
	if PlayerFunLib:CheckFreeBagCell(1, "Hµnh trang ®· ®Çy, kh«ng thÓ nhËn ®­îc.") ~= 1 then
		return
	end
	if ConsumeItem(-1, nGia, VTCN_ITEM_HTL[1], VTCN_ITEM_HTL[2], VTCN_ITEM_HTL[3], -1) ~= 1 then
		Talk(1, "", "KhÊu trõ Hé Tiªu LÖnh thÊt b¹i, h·y ®Æt Hé Tiªu LÖnh vµo trong tói.")
		return
	end
	local tbItem = {szName = tb.szName, tbProp = {6, 1, tb.nP, 1, 0, 0}, nBindState = -2}
	PlayerFunLib:GetItem(tbItem, 1, "[Long M«n Tiªu Côc] §æi Hé Tiªu LÖnh lÊy ®¹o cô")
	Msg2Player(format("§· ®æi %d Hé Tiªu LÖnh lÊy 1 %s.", nGia, tb.szName))
	WriteYunBiaoLog(format("[Long M«n Tiªu Côc] Tµi kho¶n: %s, Nh©n vËt: %s ®æi %d Hé Tiªu LÖnh lÊy %s", GetAccount(), GetName(), nGia, tb.szName))
end
