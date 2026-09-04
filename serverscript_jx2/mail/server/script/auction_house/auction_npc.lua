-- auction_npc.lua - [DAUGIA 04/09] menu NPC dau gia (Include vao state cua NPC Dich Quan va lenh bai admin).
-- Ky gui ca nhan: chon tien -> nhap gia -> dat mon vao hop -> tru phi -> vao kho. Bang chu: mo phien bang hoi.
-- GM (lenh bai admin): mo phien the gioi. Cac callback OpenGetNumber / GiveItemUI PHAI nam trong state nay.
Include("\\script\\auction_house\\auction_manager.lua")

AUC_TMP = AUC_TMP or {}

function auc_npc_main()
    local szTong, nTong = GetTongName()
    local tb = {}
    tinsert(tb, "K˝ gˆi vÀt ph»m (Ng©n l≠Óng)/auc_npc_kygui_nl")
    tinsert(tb, "K˝ gˆi vÀt ph»m (Xu)/auc_npc_kygui_xu")
    if nTong and nTong > 0 and TONG_GetMaster(nTong) == GetName() then
        tinsert(tb, "Bang chÒ: Æ≠a vÀt ph»m vµo phi™n Æ u gi∏ bang/auc_npc_tong")
    end
    tinsert(tb, "MÎ cˆa sÊ Æ u gi∏/auc_npc_open")
    tinsert(tb, "RÍi kh·i/no")
    Say("Ch≠Îng qu«y Æ u gi∏: K˝ gˆi 24 giÍ, ph› "..AUCTION_DEF.nPersonalPutOnCost.."% gi∏ b∏n, thu’ "..AUCTION_DEF.nAuctionTaxRate.."% khi b∏n Æ≠Óc. Ti“n vµ vÀt ph»m tr∂ qua hÈp th≠.", getn(tb), tb)
end

function auc_npc_open()
    local h = OB_Create()
    ObjBuffer:PushByType(h, OBJTYPE_NUMBER, AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL)
    AUC_SendTo(PlayerIndex, "emSCRIPT_PROTOCOL_AUCTION_OPENWND", h)
    OB_Release(h)
end

function auc_npc_kygui_nl()
    auc_npc_kygui(AUCTION_DEF.tbCurrency.MONEY)
end

function auc_npc_kygui_xu()
    auc_npc_kygui(AUCTION_DEF.tbCurrency.XU)
end

function auc_npc_kygui(nCur)
    if AUC_CountSeller(GetName()) >= AUCTION_DEF.nMaxItemPerSeller then
        Say("ßπi hi÷p Æang k˝ gˆi tËi Æa "..AUCTION_DEF.nMaxItemPerSeller.." m„n, h∑y chÍ b∏n xong ho∆c rÛt bÌt.", 0)
        return
    end
    AUC_TMP[PlayerIndex] = {nType = AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL, szAct = "", nKind = AUCTION_DEF.tbItemTypeEnum.eType_DUTCH, nCur = nCur, nTong = 0}
    OpenGetNumber("NhÀp gi∏ b∏n ("..AUC_CurName(nCur)..")", "auc_npc_price")
end

function auc_npc_price()
    local t = AUC_TMP[PlayerIndex]
    if not t then
        return
    end
    local nPrice = floor(GetNumberFromUI() or 0)
    if nPrice < 1 or nPrice > 2000000000 then
        Say("Gi∏ kh´ng hÓp l÷.", 0)
        AUC_TMP[PlayerIndex] = nil
        return
    end
    t.nPrice = nPrice
    GiveItemUI("ß∆t vÀt ph»m c«n b∏n vµo ´ (1 m„n)", "Gi∏ "..nPrice.." "..AUC_CurName(t.nCur), "auc_npc_give_ok", "auc_npc_give_cancel", 0, "auc_npc_give_check", 0)
end

function auc_npc_give_check(nCount)
    return 1
end

function auc_npc_give_cancel()
    AUC_TMP[PlayerIndex] = nil
end

function auc_npc_give_ok(nCount)
    local t = AUC_TMP[PlayerIndex]
    AUC_TMP[PlayerIndex] = nil
    if not t then
        return
    end
    local nIdx = GetGiveItemUnit(1)
    if nIdx == nil or nIdx <= 0 then
        Msg2Player("Ch≠a Æ∆t vÀt ph»m.")
        return
    end
    local nId = AUC_PutOnItem(t.nType, t.szAct, t.nKind, t.nCur, t.nPrice, nIdx, t.nTong)
    if nId > 0 then
        Msg2Player("ß∑ Æ≠a vµo khu Æ u gi∏, m∑ sË "..nId..". Ti“n b∏n vµ vÀt ph»m tr∂ v“ qua hÈp th≠.")
    end
end

-- bang chu: dua mon vao phien bang (ten phien = ten bang), kieu Anh hoac Ha Lan, tien vao QUY BANG
function auc_npc_tong()
    Say("Phi™n Æ u gi∏ bang: ch‰n ki”u Æ u vµ loπi ti“n", 5,
        "Ki”u Anh (gi∏ t®ng d«n) - Ng©n l≠Óng/auc_npc_tong_a1",
        "Ki”u Anh (gi∏ t®ng d«n) - Xu/auc_npc_tong_a2",
        "Ki”u Hµ Lan (gi∏ gi∂m d«n) - Ng©n l≠Óng/auc_npc_tong_h1",
        "Ki”u Hµ Lan (gi∏ gi∂m d«n) - Xu/auc_npc_tong_h2",
        "RÍi kh·i/no")
end

function auc_npc_tong_start(nKind, nCur)
    local szTong, nTong = GetTongName()
    if not nTong or nTong <= 0 or TONG_GetMaster(nTong) ~= GetName() then
        Say("Chÿ Bang chÒ mÌi mÎ Æ≠Óc phi™n bang.", 0)
        return
    end
    AUC_TMP[PlayerIndex] = {nType = AUCTION_DEF.tbAuctionTypeEnum.eType_TONG, szAct = szTong, nKind = nKind, nCur = nCur, nTong = nTong}
    OpenGetNumber("Gi∏ khÎi Æi”m ("..AUC_CurName(nCur)..")", "auc_npc_price")
end

function auc_npc_tong_a1() auc_npc_tong_start(AUCTION_DEF.tbItemTypeEnum.eType_ENGLISH, AUCTION_DEF.tbCurrency.MONEY) end
function auc_npc_tong_a2() auc_npc_tong_start(AUCTION_DEF.tbItemTypeEnum.eType_ENGLISH, AUCTION_DEF.tbCurrency.XU) end
function auc_npc_tong_h1() auc_npc_tong_start(AUCTION_DEF.tbItemTypeEnum.eType_DUTCH, AUCTION_DEF.tbCurrency.MONEY) end
function auc_npc_tong_h2() auc_npc_tong_start(AUCTION_DEF.tbItemTypeEnum.eType_DUTCH, AUCTION_DEF.tbCurrency.XU) end

-- GM (lenh bai admin): phien THE GIOI, ten phien tu dong theo gio mo
function auc_admin_world()
    Say("Phi™n Æ u gi∏ th’ giÌi (GM): ch‰n ki”u Æ u vµ loπi ti“n", 5,
        "Ki”u Anh - Ng©n l≠Óng/auc_admin_w_a1",
        "Ki”u Anh - Xu/auc_admin_w_a2",
        "Ki”u Hµ Lan - Ng©n l≠Óng/auc_admin_w_h1",
        "Ki”u Hµ Lan - Xu/auc_admin_w_h2",
        "RÍi kh·i/no")
end

function auc_admin_world_start(nKind, nCur)
    local szAct = "Phi™n "..GetLocalDate("%H:%M %d/%m")
    AUC_TMP[PlayerIndex] = {nType = AUCTION_DEF.tbAuctionTypeEnum.eType_WORLD, szAct = szAct, nKind = nKind, nCur = nCur, nTong = 0}
    OpenGetNumber("Gi∏ khÎi Æi”m ("..AUC_CurName(nCur)..")", "auc_npc_price")
end

function auc_admin_w_a1() auc_admin_world_start(AUCTION_DEF.tbItemTypeEnum.eType_ENGLISH, AUCTION_DEF.tbCurrency.MONEY) end
function auc_admin_w_a2() auc_admin_world_start(AUCTION_DEF.tbItemTypeEnum.eType_ENGLISH, AUCTION_DEF.tbCurrency.XU) end
function auc_admin_w_h1() auc_admin_world_start(AUCTION_DEF.tbItemTypeEnum.eType_DUTCH, AUCTION_DEF.tbCurrency.MONEY) end
function auc_admin_w_h2() auc_admin_world_start(AUCTION_DEF.tbItemTypeEnum.eType_DUTCH, AUCTION_DEF.tbCurrency.XU) end
