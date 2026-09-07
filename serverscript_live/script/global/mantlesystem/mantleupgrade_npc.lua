-- [LOCAL54 06/09 toi] 7 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local PF_InlayMoLai, PF_InlayLoi, PF_MoLaiWashBox, PF_WashLoi, PF_DoiTinhNgoc, PF_MoLaiHopNangCap, HandIn_onFirstTimeStarLevelOk
Include("\\script\\global\\mantlesystem\\mantleupgrade_head.lua")
Include("\\script\\dailogsys\\dailogsay.lua")
Include("\\script\\activitysys\\playerfunlib.lua")

tbAllPlayerProcess = tbAllPlayerProcess or {};	-- [PB 01/09] thoren.lua dofile lai moi lan bam NPC -> khong duoc reset phien dang mo

function onMantleSystem()
    if not PlayerIndex then
        return 0;
    end
    tbAllPlayerProcess[PlayerIndex] = tbMantleSystem:New();
    -- [VA 31/08c] khe player tai su dung: xoa cache loc trung thong bao
    if tbPF_MsgCu then
        tbPF_MsgCu[PlayerIndex] = nil;
    end
    if tbPF_MsgCu2 then
        tbPF_MsgCu2[PlayerIndex] = nil;
    end
    local tbTalkOption = {
        "<dec><npc>C¨n cø vµo cÊp ®ét ph¸ cña Phi Phong cã thÓ kh¶m sè l­îng Tinh ThÇn Th¹ch kh¸c nhau, t¨ng sao sÏ t¨ng kh¶ n¨ng thuéc tÝnh mang ®Õn cña Tinh ThÇn Th¹ch, TiÖm Phi Phong cã thÓ dïng Tinh Ngäc ®æi c¸c lo¹i nguyªn liÖu ®ét ph¸, t¨ng sao.",
        "Phi Phong t¨ng sao/onStarUpGrade",
        "Phi Phong ®ét ph¸/onBreakThrough",
        "Phi Phong kh¶m Tinh ThÇn Th¹ch/onMosaicStone",
        "TiÖm nguyªn liÖu Phi Phong/onMaterialShop",
    }
    
    if GetTask(3550) == 0 then
			tinsert(tbTalkOption, "Nép Phi Phong ®· t¨ng sao/onFirstTimeStarLevel")
		end
	
		tinsert(tbTalkOption, "Kh«ng lµm g× c¶ /onCancle")
		
    CreateTaskSay(tbTalkOption);
end

function onStarUpGrade()
    if not PlayerIndex then
        return 0;
    end
    local tbCurMantle = tbAllPlayerProcess[PlayerIndex];
    if not tbCurMantle then
        return 0;
    end
    tbCurMantle.nType = TYPE_STAR_UPGRADE;
    tbCurMantle.nNeedMoney = 1000000;
    tbCurMantle.nNeedItem = 1;
    tbCurMantle.szTitle = "H·y ®Æt vµo Phi Phong cÇn t¨ng sao vµ nguyªn liÖu: ";
    GiveItemUI(
        "Phi Phong t¨ng sao", "§Æt vµo phi phong, cÊp sao cµng cao, thuéc tÝnh cµng cao. <color=red>Sau khi t¨ng sao phi phong sÏ khãa víi nh©n vËt.<color>", 
        "doMantleUpGrade", "onCancle", 1, "onStarUpGradeCallBackCheckItem", 1);
	SetUiGiveItemMsg(tbCurMantle.szTitle);
end

function onBreakThrough()
    local tbTalkOption = {
        "<dec><npc>Phi Phong th­êng 10 Sao cã thÓ dïng <color=red>Tinh ThÇn Ngäc<color> ®Ó ®ét ph¸ th­êng, sau khi thµnh c«ng nhËn ®­îc Phi Phong cÊp kÕ, Phi Phong Ngù Phong 10 Sao cã thÓ dïng <color=red>B¸ch LuyÖn Thµnh C­¬ng<color> n©ng cÊp thµnh Hoµng Kim Phi Phong V« Cùc; HuyÒn Kim + 10 V« Cùc cã thÓ dïng <color=red>B¸ch LuyÖn Thµnh C­¬ng<color> trùc tiÕp më lç kh¶m 10 sao.",
        "Phi Phong ®ét ph¸ th­êng/onBreakThroughPro",
        "Phi Phong ®ét ph¸ bËc cao (Ngù Phong trë lªn)/onBreakThroughCao",
        -- [WASH 01/09] an muc tay TRUC TIEP kieu cu (box cu) - chi giu bang tay luyen theo VLTK
        --"TÈy thuéc tÝnh Èn (Long Ng©m trë lªn)/onTayAnAttr",
        "TÈy luyÖn thuéc tÝnh Èn Phi Phong/onWashMantle",
        --"½ø½×ÎÞ¼«/onAdvanceToWuJi",
        --"Ðþ½ðÎÞ¼«¶þ´ÎÍ»ÆÆ/onBreakThroughWuJi",
        "Trë vÒ/onMantleSystem",
        " KÕt thóc ®èi tho¹i!/onCancle",
    }
    CreateTaskSay(tbTalkOption);
end

function onBreakThroughPro()
    if not PlayerIndex then
        return 0;
    end
    local tbCurMantle = tbAllPlayerProcess[PlayerIndex];
    if not tbCurMantle then
        return 0;
    end
    tbCurMantle.nType = TYPE_PRO_BREAK_THROUGH;
    tbCurMantle.nNeedMoney = 1000000;
    tbCurMantle.nNeedItem = 1;
    tbCurMantle.szTitle = "H·y ®Æt vµo Phi Phong vµ Thiªn Tinh Ngäc: ";
    GiveItemUI(
        "Phi Phong ®ét ph¸", "Hoµng Kim Phi Phong 10 Sao dïng <color=green>Thiªn Tinh Ngäc<color> ®ét ph¸ lªn cÊp kÕ, sè lç kh¶m t¨ng 1. ThÊt b¹i t¨ng 1 <color=red>®iÓm chóc phóc<color>, ®iÓm chóc phóc ®Çy th× ch¾c ch¾n thµnh c«ng.", 
        "doMantleUpGrade", "onCancle", 1, "onCallBackCheckItem", 1);
	SetUiGiveItemMsg(tbCurMantle.szTitle);
end

-- [PF13 31/08] dot pha bac cao: Ngu Phong -> Phe Quang -> ... -> So Phuong
-- (chuoi 13 bac theo client VLTK 2023; nguyen lieu + so luong theo bac,
-- CheckCaoBreakThrough tu doi chieu; ti le 100%).
function onBreakThroughCao()
    if not PlayerIndex then
        return 0;
    end
    local tbCurMantle = tbAllPlayerProcess[PlayerIndex];
    if not tbCurMantle then
        return 0;
    end
    tbCurMantle.nType = TYPE_CAO_BREAK_THROUGH;
    tbCurMantle.nNeedMoney = 1000000;
    tbCurMantle.nNeedItem = 0; -- so luong that theo bac, CheckCaoBreakThrough dat
    tbCurMantle.szTitle = "H·y ®Æt vµo Phi Phong 10 Sao vµ nguyªn liÖu ®ét ph¸: ";
    GiveItemUI(
        "Phi Phong ®ét ph¸ bËc cao", "Phi Phong 10 Sao tõ Ngù Phong trë lªn dïng <color=green>B¸ch LuyÖn Thµnh C­¬ng<color> (tíi HuyÒn Kim) hoÆc <color=green>§¹i B¸ch LuyÖn Thµnh C­¬ng<color> (tõ V« Cùc) hîp thµnh Phi Phong bËc kÕ, thµnh c«ng 100%.",
        "doMantleUpGrade", "onCancle", 1, "onCallBackCheckItem", 1);
	SetUiGiveItemMsg(tbCurMantle.szTitle);
end

-- [PF13 31/08] tay thuoc tinh an Long Ngam+ (xem CheckTayAn trong head.lua)
function onTayAnAttr()
    if not PlayerIndex then
        return 0;
    end
    local tbCurMantle = tbAllPlayerProcess[PlayerIndex];
    if not tbCurMantle then
        return 0;
    end
    tbCurMantle.nType = TYPE_TAY_AN_ATTR;
    tbCurMantle.nNeedMoney = 0; -- CheckTayAn tu dat theo nguyen lieu
    tbCurMantle.nNeedItem = 0;
    tbCurMantle.szTitle = "H·y ®Æt vµo Phi Phong Long Ng©m vµ nguyªn liÖu tÈy: ";
    GiveItemUI(
        "TÈy thuéc tÝnh Èn", "§Æt Phi Phong Long Ng©m trë lªn + <color=green>1 LÖnh Bµi Phi Phong Long Ng©m<color> (miÔn phÝ b¹c) hoÆc <color=green>2 §¹i B¸ch LuyÖn Thµnh C­¬ng<color> + 20000 v¹n l­îng ®Ó quay l¹i 2 dßng thuéc tÝnh Èn.",
        "doMantleUpGrade", "onCancle", 1, "onCallBackCheckItem", 1);
	SetUiGiveItemMsg(tbCurMantle.szTitle);
end

function onAdvanceToWuJi()
    if not PlayerIndex then
        return 0;
    end
    local tbCurMantle = tbAllPlayerProcess[PlayerIndex];
    if not tbCurMantle then
        return 0;
    end
    tbCurMantle.nType = TYPE_ADVANCE_TO_WUJI;
    tbCurMantle.nNeedMoney = 100000000;
    tbCurMantle.nNeedItem = 100;
    tbCurMantle.szTitle = "H·y ®Æt vµo Phi Phong Ngù Phong 10 Sao vµ B¸ch LuyÖn Thµnh C­¬ng#";
    GiveItemUI(
        "N©ng cÊp V« Cùc", "Phi Phong Ngù Phong 10 Sao cã thÓ dïng 100 B¸ch LuyÖn Thµnh C­¬ng ghÐp thµnh <color=red>Hoµng Kim Phi Phong V« Cùc<color>.", 
        "doMantleUpGrade", "onCancle", 1, "onCallBackCheckItem", 1);
	SetUiGiveItemMsg(tbCurMantle.szTitle);
end

function onBreakThroughWuJi()
    if not PlayerIndex then
        return 0;
    end
    local tbCurMantle = tbAllPlayerProcess[PlayerIndex];
    if not tbCurMantle then
        return 0;
    end
    tbCurMantle.nType = TYPE_SEC_BREAK_THROUGH;
    tbCurMantle.nNeedMoney = 20000000;
    tbCurMantle.nNeedItem = 25;
    tbCurMantle.szTitle = "H·y ®Æt vµo HuyÒn Kim + 10 Phi Phong V« Cùc vµ B¸ch LuyÖn Thµnh C­¬ng#";
    GiveItemUI(
        "HuyÒn Kim V« Cùc ®ét ph¸ lÇn 2", "§Æt vµo HuyÒn Kim V« Cùc +10 vµ B¸ch LuyÖn Thµnh C­¬ng ®Ó ®ét ph¸ lÇn 2, nhËn thªm 1 lç kh¶m 10 sao, tèi ®a 5 lç. Më 5 lç lÇn l­ît cÇn 15, 20, 25, 30, 35 B¸ch LuyÖn Thµnh C­¬ng.", 
        "doMantleUpGrade", "onCancle", 1, "onCallBackCheckItem", 1);
	SetUiGiveItemMsg(tbCurMantle.szTitle);
end

function onMosaicStone()
    if not PlayerIndex then
        return 0;
    end
    local tbCurMantle = tbAllPlayerProcess[PlayerIndex];
    if not tbCurMantle then
        return 0;
    end
    tbCurMantle.nType = TYPE_INLAY_STAR_STONE;
    -- [PANEL 29/08] mo PANEL GOC cua ban Linux (Ui\Ui3, 11 o dung toa do goc).
    -- OpenMantleInlayBox gui chinh goi S2C_GIVE_BOX voi nType = 3 nen KHONG
    -- them goi tin moi; duong nop do van la doMantleMosaicStoneBox.
    OpenMantleInlayBox("Phi Phong kh¶m Tinh ThÇn Th¹ch", "H·y ®Æt Phi Phong vµo « gi÷a, Tinh ThÇn Th¹ch vµo ®óng « lç kh¶m muèn kh¶m. <color=red>Kh¶m råi kh«ng th¸o ra ®­îc.<color>",
        "doMantleMosaicStoneBox");
end

function doMantleMosaicStone(tbItemdwID)
    if not PlayerIndex then
        return 0;
    end
    local tbCurMantle = tbAllPlayerProcess[PlayerIndex];
    if not tbCurMantle then
        return 0;
    end

    local nSize = getn(tbItemdwID);
    local tbItemIdx = {};
    for i=1,nSize do
        tbItemIdx[i] = GetItemIndexBydwID(tbItemdwID[i]);
    end
    if tbCurMantle:OnCheckItem(tbItemIdx) == 0 then
        Talk(1, "", tbCurMantle.szErrorMsg);
        g_ReturnInlayResult(0);
        return 0;
    end

    tbCurMantle:ProcessInlayStarStone();
end

-- [PORT 29/08] ban thay the cua doMantleMosaicStone: nhan vat pham tu hop
-- GiveItemUI thay vi tu panel kham. Dung ra bang tbItemIdx dung KHUON ma
-- CheckInlayStarStone mong doi: [1..13] = da cho tung LO, [CUOI] = phi phong.
-- [VA 31/08] Bao loi roi MO LAI phien kham.
-- Bam nut "Kham nam" mot lan la KProtocolProcess.cpp:6502 xoa m_dwGiveBoxId,
-- nen lan bam THU HAI roi vao hu vo va nut trong nhu chet. Do van nam nguyen
-- trong khoang chua (pos_affairitem) nen mo lai la panel hien lai y truoc,
-- chu game chi viec sua do roi bam tiep.
-- Mo lai phien kham. Goi sau MOI lan chay xong -- ca loi lan thanh cong.
function PF_InlayMoLai()
    OpenMantleInlayBox("Phi Phong kh¶m Tinh ThÇn Th¹ch", "H·y ®Æt Phi Phong vµo « gi÷a, Tinh ThÇn Th¹ch vµo ®óng « lç kh¶m muèn kh¶m. <color=red>Kh¶m råi kh«ng th¸o ra ®­îc.<color>",
        "doMantleMosaicStoneBox");
end

function PF_InlayLoi(szMsg)
    Talk(1, "", szMsg);
    PF_InlayMoLai();
    return 0;
end

function doMantleMosaicStoneBox(nCount)
    if not PlayerIndex then
        return 0;
    end
    local tbCurMantle = tbAllPlayerProcess[PlayerIndex];
    if not tbCurMantle then
        -- [VA 31/08b] relog lam mat phien thoai -> tu dung lai phien kham,
        -- khong de nut "Kham nam" chet (m_dwGiveBoxId da bi xoa sau lan bam
        -- dau, do van nam nguyen o pos_affairitem).
        tbAllPlayerProcess[PlayerIndex] = tbMantleSystem:New();
        tbCurMantle = tbAllPlayerProcess[PlayerIndex];
        tbCurMantle.nType = TYPE_INLAY_STAR_STONE;
    end
    nCount = nCount or 0;
    -- [CHONLO 31/08] GetGiveItemSlot la binding MOI trong CoreServer.dll.
    -- DLL chua swap thi roi ve duong tu xep cu - khong duoc goi ham nil.
    local bCoSlot = 0;
    if GetGiveItemSlot then
        bCoSlot = 1;
    end
    local nMantle = 0;
    local tbStone = {};       -- duong tu xep: da theo thu tu bo vao
    local tbStoneSlot = {};   -- duong chon lo: [1..13] = da cho lo do
    local szLa = "";
    local szChong = "";
    local bPhiPhongNhamO = 0;
    local bDaNhamO = 0;
    local nSotHangDuoi = 0;
    for i = 1, nCount do
        local nIdx = GetGiveItemUnit(i);
        if nIdx and nIdx > 0 then
            local eType = tbCurMantle:GetItemType(nIdx);
            local nSlot = -1;
            if bCoSlot == 1 then
                nSlot = GetGiveItemSlot(i);
            end
            if bCoSlot == 1 and nSlot == -1 then
                -- [VA 31/08c] mon HANG DUOI khoang chua (phien give-box khac de
                -- lai, client dang an v ~= 0): khong thuoc phien kham nay -> bo
                -- qua HET, ke ca Phi Phong sot (khong de no lam DICH kham tang
                -- hinh) lan da/chong sot (khong chan luong bang mon vo hinh).
                -- Nguoi choi thu hoi bang nut Huy.
                nSotHangDuoi = nSotHangDuoi + 1;
            elseif eType == ITEM_TYPE_HJ_PIFENG or eType == ITEM_TYPE_BJ_PIFENG then
                if bCoSlot == 1 and nSlot >= 0 and nSlot <= 12 then -- [PF13 31/08]
                    bPhiPhongNhamO = 1;
                else
                    nMantle = nIdx;
                end
            elseif eType == ITEM_TYPE_XINGCS then
                -- Ban Linux tu choi da xep chong (head.lua:714) vi
                -- ProcessInlayStarStone an TRON vien - de lot la mat ca chong.
                if GetItemStackCount(nIdx) > 1 then
                    if szChong == "" then
                        szChong = GetItemName(nIdx);
                    end
                elseif bCoSlot == 1 and nSlot >= 0 and nSlot <= 12 then -- [PF13 31/08]
                    tbStoneSlot[nSlot + 1] = nIdx;
                elseif bCoSlot == 1 and nSlot == 13 then
                    bDaNhamO = 1;
                else
                    tinsert(tbStone, nIdx);
                end
            elseif szLa == "" then
                -- Bay trung ten: magicscript P=4888 la NGUYEN LIEU (G=6) nhung
                -- ten DUNG BANG "Tinh Than Thach", dung chung anh voi da that
                -- (G=9) - ca hai deu la thiet ke goc ben Linux, cam doi.
                szLa = GetItemName(nIdx);
            end
        end
    end
    if szLa ~= "" then
        return PF_InlayLoi("Kh«ng ®Æt vËt phÈm kh¸c ngoµi Phi Phong, Tinh ThÇn Th¹ch. <color=red>"..szLa.." <color> kh«ng ph¶i ®¸ kh¶m (®¸ kh¶m tªn cã tiÒn tè, vÝ dô Phôc T« Tinh ThÇn Th¹ch).");
    end
    if szChong ~= "" then
        return PF_InlayLoi("Mçi lç kh¶m chØ ®Æt vµo 1 Tinh ThÇn Th¹ch, h·y t¸ch <color=red>"..szChong.." <color> ra råi thö l¹i.");
    end
    if bPhiPhongNhamO == 1 then
        return PF_InlayLoi("Phi Phong h·y ®Æt vµo « chÝnh gi÷a.");
    end
    if bDaNhamO == 1 then
        return PF_InlayLoi("¤ chÝnh gi÷a dµnh cho Phi Phong, h·y ®Æt Tinh ThÇn Th¹ch vµo c¸c « lç kh¶m.");
    end
    if nMantle == 0 then
        return PF_InlayLoi("H·y ®Æt vµo Phi Phong cÇn kh¶m.");
    end
    -- [PF13 31/08] 13 lo + phi phong o phan tu CUOI (head.lua doc nSize+1)
    local tbItemIdx = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, nMantle};
    if bCoSlot == 1 then
        local nDat = 0;
        for i = 1, 13 do
            if tbStoneSlot[i] then
                tbItemIdx[i] = tbStoneSlot[i];
                nDat = nDat + 1;
            end
        end
        if nDat == 0 and getn(tbStone) > 0 then
            -- da nam ngoai 6 o nhap (mon hang duoi do phien khac de lai)
            return PF_InlayLoi("H·y ®Æt Tinh ThÇn Th¹ch vµo c¸c « lç kh¶m quanh Phi Phong.");
        end
        if nDat == 0 then
            return PF_InlayLoi("H·y ®Æt vµo Tinh ThÇn Th¹ch.");
        end
    else
        -- duong tu xep cu. [VA 31/08b] chi xep vao lo TRONG da tang sao:
        -- lo 0 sao nhan da nhung khong cong thuoc tinh (head.lua:501 co y de
        -- lo cuoi 0 sao cho lan dot pha ke tiep).
        if getn(tbStone) == 0 then
            return PF_InlayLoi("H·y ®Æt vµo Tinh ThÇn Th¹ch.");
        end
        local nMaxHole = GetEquipMaxStoneNum(nMantle);
        local tbHasStone = pack(GetStarStoneOnEquip(nMantle, -1));
        local nPut = 0;
        -- [VA 31/08d] vong 1: uu tien lo trong DA kich hoat (>0 sao)
        for i = 1, nMaxHole do
            if nPut >= getn(tbStone) then
                break;
            end
            if ((not tbHasStone[i]) or tbHasStone[i] == 0) and GetStoneLevelOnEquip(nMantle, i) > 0 then
                nPut = nPut + 1;
                tbItemIdx[i] = tbStone[nPut];
            end
        end
        -- vong 2: lo trong 0 sao -- kham NAM NGU nhu ban Linux, co canh bao
        for i = 1, nMaxHole do
            if nPut >= getn(tbStone) then
                break;
            end
            if tbItemIdx[i] == 0 and ((not tbHasStone[i]) or tbHasStone[i] == 0) then
                nPut = nPut + 1;
                tbItemIdx[i] = tbStone[nPut];
            end
        end
        if nPut == 0 then
            return PF_InlayLoi("Phi Phong nµy kh«ng cßn lç trèng ®Ó kh¶m.");
        end
        if nPut < getn(tbStone) then
            return PF_InlayLoi("Sè Tinh ThÇn Th¹ch nhiÒu h¬n sè lç trèng.");
        end
    end
    if tbCurMantle:OnCheckItem(tbItemIdx) == 0 then
        return PF_InlayLoi(tbCurMantle.szErrorMsg);
    end
    tbCurMantle:ProcessInlayStarStone();
    -- Duong thanh cong chi an DA, PHI PHONG van nam trong khoang chua ->
    -- mo lai de panel hien lai y truoc va phien give-box song tiep.
    PF_InlayMoLai();
end

-- [PF13 01/09] TAY LUYEN box xem-truoc-roi-chon (giong client VLTK).
-- doWashRoll: luu dong cu + roll dong moi (ghi tam) + tru nguyen lieu; client
-- hien cot trai (cu) / cot phai (moi). doWashKeep khoi phuc cu; doWashApply giu moi.
-- [WASH 01/09] hoi thoai gioi thieu truoc khi mo box (co dong dong hoi thoai) - chu game yeu cau
function onWashMantle()
    local tbTalkOption = {
        "<dec><npc>TÈy luyÖn thuéc tÝnh Èn Phi Phong: cÇn Phi Phong Long Ng©m 10 sao trë lªn. Mçi lÇn tÈy tèn 1 LÖnh Bµi Phi Phong Long Ng©m (hoÆc 2 §¹i B¸ch LuyÖn Thµnh C­¬ng + 20000 v¹n). Sau khi tÈy cã thÓ chän gi÷ nguyªn dßng cò hoÆc ¸p dông dßng míi.",
        "B¾t ®Çu tÈy luyÖn thuéc tÝnh Èn Phi Phong/onWashMantleBox",
        "Trë vÒ/onBreakThrough",
        "KÕt thóc ®èi tho¹i/onCancle",
    }
    CreateTaskSay(tbTalkOption);
end

function onWashMantleBox()
    if not PlayerIndex then
        return 0;
    end
    tbAllPlayerProcess[PlayerIndex] = tbMantleSystem:New();
    local tb = tbAllPlayerProcess[PlayerIndex];
    tb.nType = TYPE_TAY_AN_ATTR;
    tb.nWashMantleIdx = 0;
    tb.tbWashCu = nil;
    OpenMantleWashBox(
        "TÈy luyÖn thuéc tÝnh Èn Phi Phong",
        "§Æt Phi Phong Long Ng©m 10 sao vµo «; 1 LÖnh Bµi Phi Phong Long Ng©m (hoÆc 2 §¹i B¸ch LuyÖn Thµnh C­¬ng + 20000 v¹n) trõ tõ hµnh trang; bÊm TÈy luyÖn ®Ó quay l¹i 2 dßng Èn. Xem cét tr­íc/sau råi chän Gi÷ nguyªn hoÆc ¸p dông.",
        "doWashRoll");
end

function PF_MoLaiWashBox()
    OpenMantleWashBox(
        "TÈy luyÖn thuéc tÝnh Èn Phi Phong",
        "BÊm TÈy luyÖn ®Ó quay tiÕp; Gi÷ nguyªn hoÆc ¸p dông ®Ó chän kÕt qu¶.",
        "doWashRoll");
end

function PF_WashLoi(szMsg)
    Talk(1, "", szMsg);
    PF_MoLaiWashBox();
    return 0;
end

function doWashRoll(nCount)
    if not PlayerIndex then
        return 0;
    end
    local tb = tbAllPlayerProcess[PlayerIndex];
    if not tb then
        tbAllPlayerProcess[PlayerIndex] = tbMantleSystem:New();
        tb = tbAllPlayerProcess[PlayerIndex];
        tb.nType = TYPE_TAY_AN_ATTR;
    end
    nCount = nCount or 0;
    local nMantle = 0;
    -- [WASH-PB 02/09] box tay luyen chi co 1 o (ObjBoxItem) cho phi phong -> nguyen lieu dem trong HANH TRANG
    -- (giong VLTK), khong con doc tu khay give-box (khay khong bao gio chua duoc Lenh Bai -> luon 'thieu nguyen lieu').
    local nLB = CalcEquiproomItemCount(6, 1, 4937, -1);
    local nDBL = CalcEquiproomItemCount(6, 1, 4933, -1);
    for i = 1, nCount do
        local nIdx = GetGiveItemUnit(i);
        if nIdx and nIdx > 0 then
            local e = tb:GetItemType(nIdx);
            if e == ITEM_TYPE_HJ_PIFENG or e == ITEM_TYPE_BJ_PIFENG then
                nMantle = nIdx;
            end
        end
    end
    if nMantle == 0 then
        return PF_WashLoi("H·y ®Æt vµo Phi Phong Long Ng©m.");
    end
    if PF_LaPhiPhongCoAn(nMantle) ~= 1 then
        return PF_WashLoi("ChØ Phi Phong Long Ng©m trë lªn míi cã thuéc tÝnh Èn.");
    end
    if PF_LayKheAn(nMantle) == 0 and GetStarLevel(nMantle) < 10 then
        return PF_WashLoi("Phi Phong ph¶i ®¹t 10 sao míi cã thuéc tÝnh Èn.");
    end
    if nLB < 1 and nDBL < 2 then
        return PF_WashLoi("CÇn 1 LÖnh Bµi Phi Phong Long Ng©m hoÆc 2 §¹i B¸ch LuyÖn Thµnh C­¬ng trong hµnh trang.");
    end
    local nNeedMoney = 200000000;
    if nLB >= 1 then
        nNeedMoney = 0;
    end
    if GetCash() < nNeedMoney then
        return PF_WashLoi(format("CÇn %dW l­îng.", floor(nNeedMoney/10000)));
    end
    -- luu dong cu (de cot trai + nut Giu nguyen)
    tb.nWashMantleIdx = nMantle;
    tb.tbWashCu = pack(GetMagicAttrib(nMantle));
    -- [WASH-PB 02/09] tru nguyen lieu tu HANH TRANG (uu tien 1 Lenh Bai, khong thi 2 Dai Bach Luyen),
    -- tru TRUOC khi Pay de that bai thi khong mat tien. ConsumeEquiproomItem tra SO MON da tru (hanh trang + tui mo rong).
    if nLB >= 1 then
        if ConsumeEquiproomItem(1, 6, 1, 4937, -1) < 1 then
            return PF_WashLoi("Kh«ng trõ ®­îc LÖnh Bµi Phi Phong Long Ng©m trong hµnh trang.");
        end
    else
        if ConsumeEquiproomItem(2, 6, 1, 4933, -1) < 2 then
            return PF_WashLoi("Kh«ng trõ ®­îc 2 §¹i B¸ch LuyÖn Thµnh C­¬ng trong hµnh trang.");
        end
    end
    Pay(nNeedMoney);
    -- roll dong moi (ghi tam + sync -> client cot phai hien moi)
    PF_RollAnAttr(nMantle);
    PF_MoLaiWashBox();
    return 1;
end

function doWashKeep()
    if not PlayerIndex then
        return 0;
    end
    local tb = tbAllPlayerProcess[PlayerIndex];
    if tb and tb.nWashMantleIdx and tb.nWashMantleIdx > 0 and tb.tbWashCu then
        -- [WASH-KEEP 02/09] unpack() cua script\lib\common.lua DE QUY moi phan tu mot tang; 24 gia tri
        -- (8 khe x 3 cua GetMagicAttrib) vuot stack Lua 100 o (KLuaScript.cpp lua_open(100)) -> ScriptError
        -- 'stack overflow' (ScriptError.log 02/09 06:38). Truyen thang 24 o nhu PF_GhiKheAn (head.lua:110).
        local c = tb.tbWashCu;
        SetMagicAttrib(tb.nWashMantleIdx,
            c[1],c[2],c[3], c[4],c[5],c[6], c[7],c[8],c[9],
            c[10],c[11],c[12], c[13],c[14],c[15], c[16],c[17],c[18],
            c[19],c[20],c[21], c[22],c[23],c[24]);
        SyncItem(tb.nWashMantleIdx);
        tb.tbWashCu = pack(GetMagicAttrib(tb.nWashMantleIdx));
        Talk(1, "", "§· gi÷ nguyªn thuéc tÝnh Èn ban ®Çu.");
    end
    PF_MoLaiWashBox();
    return 1;
end

function doWashApply()
    if not PlayerIndex then
        return 0;
    end
    local tb = tbAllPlayerProcess[PlayerIndex];
    if tb then
        tb.tbWashCu = nil;
    end
    Talk(1, "", "§· ¸p dông thuéc tÝnh Èn míi.");
    return 1;
end

-- [NGUYENLIEU 01/09] Tiem nguyen lieu Phi Phong theo ban Linux (goods.txt Linux 2161-2170).
-- Ban cu goi Sale(186, 25, ...) : shop 186 KHONG ton tai (buysell.txt chi co 101 shop)
-- va moneyunit 25 khong hop le -> tiem chet. Nay: mua XU (Sale(102, 1)) + doi Tinh Ngoc.
-- Gia doi theo Linux (UniPriceType 25 = Tinh Ngoc): Thien Tinh Ngoc 1000, Manh 100, Tinh Than Khoang 200.
function onMaterialShop()
    local tbTalkOption = {
        "<dec><npc>Tinh Háa Than b¸n b»ng xu; Thiªn Tinh Ngäc, M¶nh Thiªn Tinh Ngäc, Tinh ThÇn Kho¸ng ®æi b»ng Tinh Ngäc nh­ b¶n gèc.",
        "Mua Tinh Háa Than, LÖnh Phï (xu)/onShopXu",
        "§æi 1000 Tinh Ngäc lÊy Thiªn Tinh Ngäc/onDoiThienTinhNgoc",
        "§æi 100 Tinh Ngäc lÊy M¶nh Thiªn Tinh Ngäc/onDoiManhThienTinhNgoc",
        "§æi 200 Tinh Ngäc lÊy Tinh ThÇn Kho¸ng/onDoiTinhThanKhoang",
        "Trë vÒ/onMantleSystem",
        "KÕt thóc ®èi tho¹i/onCancle",
    }
    CreateTaskSay(tbTalkOption);
end

function onShopXu()
    -- LuaSale chi doc tham so 2 khi CO >= 3 tham so (ScriptFuns.cpp: if GetTopIndex > 2) -> phai truyen 3.
    Sale(102, 1, 0);   -- 1 = moneyunit_extpoint (xu)
end

-- doi Tinh Ngoc (4881) lay nguyen lieu; khuon item_tianxingyu.lua
function PF_DoiTinhNgoc(nCan, nIdRa, szTenRa)
    if PlayerFunLib:CheckFreeBagCell(1, "") ~= 1 then
        Msg2Player("Tói ®· ®Çy, kh«ng thÓ ®æi.");
        return 0;
    end
    -- [PB 01/09] ConsumeItem cua JX1 (khuon JX2) tra ve SO MON DA XOA, khong phai 1/0 nhu Linux
    -- -> so sanh voi nCan. Dem bang -1 de tinh ca tui mo rong (ConsumeItem tru ca hai).
    if CalcItemCount(-1, 6, 1, 4881, -1) < nCan then
        Msg2Player("Tinh Ngäc kh«ng ®ñ, cÇn "..nCan.." viªn.");
        return 0;
    end
    if ConsumeItem(3, nCan, 6, 1, 4881, -1) < nCan then
        Msg2Player("§æi thÊt b¹i, h·y thö l¹i.");
        return 0;
    end
    local tbItem = {szName = szTenRa, tbProp = {6, 1, nIdRa, 1, 0, 0}, nBindState = -2,};
    PlayerFunLib:GetItem(tbItem, 1, "§æi "..nCan.." Tinh Ngäc lÊy "..szTenRa);
    return 1;
end

function onDoiThienTinhNgoc()
    PF_DoiTinhNgoc(1000, 4882, "Thiªn Tinh Ngäc");
end
function onDoiManhThienTinhNgoc()
    PF_DoiTinhNgoc(100, 4883, "M¶nh Thiªn Tinh Ngäc");
end
function onDoiTinhThanKhoang()
    PF_DoiTinhNgoc(200, 4887, "Tinh ThÇn Kho¸ng");
end

-- [VA 31/08b] mo lai hop nang cap sau nhanh LOI: m_dwGiveBoxId bi xoa ngay sau
-- lan bam OK dau (KProtocolProcess.cpp:6502) nen khong mo lai la nut OK chet,
-- hop trong y nguyen nhung bam khong an gi. Goi lai chinh handler menu de
-- dung nguyen bo tham so GiveItemUI cua tung loai.
function PF_MoLaiHopNangCap()
    if not PlayerIndex then
        return 0;
    end
    local tbCurMantle = tbAllPlayerProcess[PlayerIndex];
    if not tbCurMantle then
        return 0;
    end
    if tbCurMantle.nType == TYPE_STAR_UPGRADE then
        onStarUpGrade();
    elseif tbCurMantle.nType == TYPE_CAO_BREAK_THROUGH then -- [PF13 31/08]
        onBreakThroughCao();
    elseif tbCurMantle.nType == TYPE_TAY_AN_ATTR then -- [PF13 31/08]
        onTayAnAttr();
    elseif tbCurMantle.nType == TYPE_PRO_BREAK_THROUGH then
        onBreakThroughPro();
    elseif tbCurMantle.nType == TYPE_ADVANCE_TO_WUJI then
        onAdvanceToWuJi();
    elseif tbCurMantle.nType == TYPE_SEC_BREAK_THROUGH then
        onBreakThroughWuJi();
    end
end

function doMantleUpGrade(nItemCount)
    if not PlayerIndex then
        return 0;
    end
    local tbCurMantle = tbAllPlayerProcess[PlayerIndex];
    if not tbCurMantle then
        -- [VA 31/08c] phien thoai da mat (RLS/relog giua chung) -> bao ro
        Talk(1, "", "Phiªn thao t¸c ®· hÕt, h·y ®ãng hép råi nãi chuyÖn l¹i víi Thî RÌn.");
        return 0;
    end
    local tbItemIdx = {};
	for i = 1, nItemCount do
		tinsert(tbItemIdx, GetGiveItemUnit(i));
	end
    -- ¶þ´Î¼ì²é·ÅÈëµÄµÀ¾ß
    tbCurMantle:OnCheckItem(tbItemIdx);
	
    if tbCurMantle.szErrorMsg ~= "" then
        Talk(1, "", tbCurMantle.szErrorMsg);
        PF_MoLaiHopNangCap(); -- [VA 31/08b]
        return 0;
    end
    -- ³É¹¦ÂÊÊÇ·ñÒç³ö£¿
    --if tbCurMantle.nSuccessRate > 101.0 then
        --Talk(1, "", "Äã·ÅÈëµÄ²ÄÁÏ¹ý¶à£¬³É¹¦ÂÊÒç³ö£¬±¾´ÎÉý¼¶ÎÞÐ§¡£");
        --return 0;
    --end
    -- ¼ì²é½£ÏÀ±Ò
    local nCurMoney = GetCash();
    if nCurMoney < tbCurMantle.nNeedMoney then
        local szErrorTips = format("HiÖn vµng kh«ng ®ñ, cÇn Ýt nhÊt %dW l­îng míi cã thÓ hoµn thµnh.", floor(tbCurMantle.nNeedMoney/10000));
        Talk(1, "", szErrorTips);
        PF_MoLaiHopNangCap(); -- [VA 31/08b]
        return 0;
    end
    if tbCurMantle:DoMantleUpGrade() == 0 then
        -- giao dich CHUA chay (khoa ruong / phi phong het han...) -> giu phien
        PF_MoLaiHopNangCap();
        return 0;
    end
    -- [VA 31/08h] THANH CONG -> dung lai phien CUNG LOAI roi mo lai hop, de
    -- tang sao / dot pha lien tuc. Truoc day xoa phien va khong mo lai nen
    -- moi lan xong phai tat bang chon roi noi chuyen lai NPC.
    local nLoai = tbCurMantle.nType;
    tbAllPlayerProcess[PlayerIndex] = tbMantleSystem:New();
    tbAllPlayerProcess[PlayerIndex].nType = nLoai;
    PF_MoLaiHopNangCap();   -- handler tu dat lai nNeedMoney/nNeedItem/szTitle
end

function onCancle()
    tbAllPlayerProcess[PlayerIndex] = nil;
end

function onStarUpGradeCallBackCheckItem(nItemCount)
	if not PlayerIndex then
		return 0;
	end
	local tbCurMantle = tbAllPlayerProcess[PlayerIndex];
	if not tbCurMantle then
		SetUiGiveItemMsg("Thao t¸c bÞ nhÇm, h·y ®ãng giao diÖn nµy vµ thö l¹i.");
		return 0;
	end

	if nItemCount == 0 then
        SetUiGiveItemMsg(tbCurMantle.szTitle);
    return 0;
	end
	
	local tbItemIdx = {};
	for i = 1, nItemCount do
		tinsert(tbItemIdx, GetGiveItemUnit(i));
	end
  local szShowMsg = "";
	local nSuccess = tbCurMantle:OnCheckItem(tbItemIdx);
	if nSuccess == 0 then
		szShowMsg = tbCurMantle.szErrorMsg;
		SetUiGiveItemMoreConfirmMsg("")
	else
		szShowMsg = tbCurMantle.szShowTips;
		local szMoreConfirmString = format("TØ lÖ thµnh c«ng t¨ng sao: %.2f%%", tbCurMantle.nSuccessRate)
		SetUiGiveItemMoreConfirmMsg(szMoreConfirmString);
	end
	SetUiGiveItemMsg(szShowMsg);
end

function onCallBackCheckItem(nItemCount)
	if not PlayerIndex then
		return 0;
	end
	local tbCurMantle = tbAllPlayerProcess[PlayerIndex];
	if not tbCurMantle then
		SetUiGiveItemMsg("Thao t¸c bÞ nhÇm, h·y ®ãng giao diÖn nµy vµ thö l¹i.");
		return 0;
	end
	
    if nItemCount == 0 then
        SetUiGiveItemMsg(tbCurMantle.szTitle);
        return 0;
    end
    local tbItemIdx = {};
	for i = 1, nItemCount do
		tinsert(tbItemIdx, GetGiveItemUnit(i));
	end
    local szShowMsg = "";
    local nSuccess = tbCurMantle:OnCheckItem(tbItemIdx);
    if nSuccess == 0 then
        szShowMsg = tbCurMantle.szErrorMsg;
    else
        szShowMsg = tbCurMantle.szShowTips;
    end
    SetUiGiveItemMsg(szShowMsg);
end

function onShopCallBack(nItemIdx, nPrice, nCount)
--    local itemgenre, itemdetail, itemParticular = GetItemProp(nItemIdx);
    SetItemBindState(nItemIdx, -2);
    local szBuyLog = format("TiÖm Phi Phong mua %d c¸i %s", nCount, GetItemName(nItemIdx));
    tbMantleSystem:WriteMantleLog(szBuyLog);
    return 1;
end


--ÐÇ³½Ê¯ÏµÍ³Åû·çÉý¼¶Ö¸ÒýÈÎÎñ
function onFirstTimeStarLevel()
    if GetTask(3550) > 0 then
        Talk(1, "", "Ta ®· thÊy râ sù kiÖt xuÊt cña c¸c h¹, kh«ng cÇn ®­a l¹i cho ta.")
    else
        CreateTaskSay({"H·y nép 1 Phi Phong ®· t¨ng sao cho ta xem, xem xong ta sÏ tr¶ l¹i cho ng­¬i.", "Ta muèn giao nép/onFirstTimeStarLevelOk", "Trë vÒ/onMantleSystem"," KÕt thóc ®èi tho¹i!/no"})
    end
end

function onFirstTimeStarLevelOk()
    g_GiveItemUI("Nép 1 Phi Phong ®· t¨ng sao", format("H·y cho ta xem sù kiÖt xuÊt cña ng­¬i"), {HandIn_onFirstTimeStarLevelOk, {}}, {}, 1)
end

function HandIn_onFirstTimeStarLevelOk(nCount)
    if nCount ~= 1 then
        Talk(1,"","Nép 1 Phi Phong ®· t¨ng sao")
        return 0
    end

    if PlayerFunLib:CheckFreeBagCell(1, "default") ~= 1 then
        return 0
    end

    local nItemIndex = GetGiveItemUnit(1)
    if GetStarLevel(nItemIndex) > 0 then
        Talk(1, "", "Trang bÞ sau khi t¨ng sao cã vÎ m¹nh h¬n.")
        SetTask(3550,1)
        PlayerFunLib:AddExp(25000000, 1, "[HÖ thèng Tinh ThÇn Th¹ch] LÇn ®Çu nép Phi Phong ®· t¨ng ao nhËn ®­îc 2000 v¹n kinh nghiÖm.")
        -- [VA 29/08] JX1 doi AddItem >= 7 tham so (nItemLevel), ban Linux goi 6:
        -- thieu 1 tham so la LuaAddItem tra 0 va KHONG cho gi, khong bao loi.
        AddItem(6, 1, 4885, 1, 0, 0, 0)
    else
        Talk(1, "", "Trang bÞ nép ch­a t¨ng sao.")
    end
end