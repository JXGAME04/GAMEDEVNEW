IncludeLib("ITEM")
Include("\\script\\lib\\common.lua")
Include("\\script\\activitysys\\playerfunlib.lua")
-- [PORT 29/08] bo Include khong co ben JX1
-- [PORT 29/08] bo Include khong co ben JX1
TYPE_STAR_UPGRADE       = 1;
TYPE_PRO_BREAK_THROUGH  = 2;
TYPE_ADVANCE_TO_WUJI    = 3;
TYPE_SEC_BREAK_THROUGH  = 4;
TYPE_INLAY_STAR_STONE   = 5;
TYPE_CAO_BREAK_THROUGH  = 6;   -- [PF13 31/08] dot pha bac cao (Ngu Phong tro len)
TYPE_TAY_AN_ATTR        = 7;   -- [PF13 31/08] tay thuoc tinh an Long Ngam+

ITEM_TYPE_ERROR     = 0;       -- ·Ç·¨ÎïÆ·
ITEM_TYPE_HJ_PIFENG = 1;       -- »Æ½ðÅû·ç
ITEM_TYPE_BJ_PIFENG = 2;       -- »Æ½ðÅû·ç
ITEM_TYPE_XINGYU    = 3;       -- ÐÇÓñ
ITEM_TYPE_TIANXY    = 4;       -- ÌìÐÇÓñ
ITEM_TYPE_BLCG      = 5;       -- °ÙÁ¶³É¸Ö
ITEM_TYPE_XINGCS    = 6;
-- [PF13 31/08] nguyen lieu bac cao theo client VLTK 2023
ITEM_TYPE_DBLCG     = 7;       -- Dai Bach Luyen Thanh Cuong (P=4933, dot pha bac 9+)
ITEM_TYPE_PHACHTN   = 8;       -- Phach Tinh Ngoc (P=4934, tang sao cao cap)
ITEM_TYPE_DAITT     = 9;       -- Dai Tinh Thach (P=4935, tang sao sieu cap)
ITEM_TYPE_KIMTINH   = 10;      -- Kim Tinh (P=4936, tang sao bac 12+)
ITEM_TYPE_CAO_HONHOP = -7;     -- ma hieu: CheckBreakThrough nhan ca BLCG lan DBLCG
tbPF_TenVLSao = {
    [3] = "Tinh Ngäc",
    [8] = "Ph¸ch Tinh Ngäc",
    [9] = "§¹i Tinh Th¹ch",
    [10] = "Kim Tinh",
};
tbPF_TenVLCao = {
    [5] = "B¸ch LuyÖn Thµnh C­¬ng",
    [7] = "§¹i B¸ch LuyÖn Thµnh C­¬ng",
};       -- ÐÇ³½Ê¯

-- ================= [PF13 31/08] THUOC TINH AN (Long Ngam/So Phuong) =================
-- Theo trang VNG 11/2023: dat 10 sao thi 2 dong an xuat hien ngau nhien (1 dong
-- nhom 1 phong thu + 1 dong nhom 2 tan cong). LECH CO Y: khong co han 14 ngay,
-- khong co buoc kich hoat 3/5 Dai Bach Luyen - dong an tu sinh va vinh vien;
-- tay lai = 1 Lenh Bai Long Ngam HOAC 2 Dai Bach Luyen + 20000 van luong.
tbPF_AnNhom1 = { -- [PF13 31/08] gia tri theo trang VNG, cho DE CHINH
    {114, 25}, -- magic_allres_p (Khang tat ca %)
    {193, 16}, -- magic_sorbdamage_p (Xac suat hoa giai sat thuong %)
    {173, 16}, -- magic_fatallystrikeres_p (Khang don chi mang %)
    {252, 16}, -- magic_anti_enhancehit_rate (Ty le giam trong kich %)
    {30, 120}, -- magic_armordefense_v (Phong thu (diem) ~ triet tieu sat thuong)
    {104, 25}, -- magic_physicsres_p (Khang vat ly %)
    {103, 25}, -- magic_lightingres_p (Khang Loi %)
    {105, 25}, -- magic_coldres_p (Khang Bang %)
    {102, 25}, -- magic_fireres_p (Khang Hoa %)
    {101, 25}, -- magic_poisonres_p (Khang Doc %)
    {86, 25}, -- magic_lifemax_p (Sinh luc toi da %)
    {110, 16}, -- magic_stuntimereduce_p (Giam thoi gian choang %)
    {196, 16}, -- magic_anti_hitrecover (Thoi gian phuc hoi (khang keo dai dong tac))
    {106, 25}, -- magic_freezetimereduce_p (Giam thoi gian lam cham %)
    {97, 100}, -- magic_strength_v (Suc manh)
    {99, 100}, -- magic_vitality_v (Sinh khi)
    {98, 100}, -- magic_dexterity_v (Than phap)
    {100, 100}, -- magic_energy_v (Noi cong)
    {85, 1500}, -- magic_lifemax_v (Sinh luc toi da (diem))
    {89, 1500}, -- magic_manamax_v (Noi luc toi da (diem))
    {90, 25}, -- magic_manamax_p (Noi luc toi da %)
};
tbPF_AnNhom2 = { -- [PF13 31/08] gia tri theo trang VNG, cho DE CHINH
    {51, 18}, -- magic_add_damage_p (Tang sat thuong % (VNG 'cong kich ky nang 180' -> 18, DE CHINH))
    {227, 16}, -- magic_enhancehit_rate (Xac suat trong kich %)
    {71, 16}, -- magic_deadlystrike_p (Ti le chi tu %)
    {224, 25}, -- magic_anti_allres_yan_p (Bo qua tat ca khang tinh)
    {126, 16}, -- magic_addphysicsdamage_p (Sat thuong vat ly % (VNG 'diem 60' -> %; 61 physicsdamage_v khong co handler))
    {121, 120}, -- magic_addphysicsdamage_v (Gia tang sat thuong (diem))
    {204, 25}, -- magic_anti_poisontimereduce_p (Thoi gian doc phat giam %)
    {67, 16}, -- magic_steallife_p (Hut sinh luc %)
    {152, 120}, -- magic_fatallystrikeenhance_p (Hieu qua trong kich/chi mang % (72 fatallystrike_p khong co handler))
    {205, 16}, -- magic_do_hurt_p (Ti le tao thanh sat thuong (bi thuong) %)
    {248, 25}, -- magic_anti_lightingres_yan_p (Bo qua Loi Phong)
    {222, 25}, -- magic_anti_coldres_yan_p (Bo qua Bang Phong)
    {217, 25}, -- magic_anti_fireres_yan_p (Bo qua Hoa Phong)
    {221, 25}, -- magic_anti_poisonres_yan_p (Bo qua Doc Phong)
    {244, 25}, -- magic_anti_physicsres_yan_p (Bo qua Pho Phong)
    {123, 150}, -- magic_addcolddamage_v (Bang sat (VNG 1500 -> 150, DE CHINH))
    {122, 150}, -- magic_addfiredamage_v (Hoa sat (VNG 1500 -> 150, DE CHINH))
    {124, 150}, -- magic_addlightingdamage_v (Loi sat (VNG 1500 -> 150, DE CHINH))
    {125, 60}, -- magic_addpoisondamage_v (Doc sat (VNG 180 -> 60, DE CHINH))
};
PF_AN_KEY_TOITHIEU = 6734; -- Long Ngam tro len (key goldequip)

function PF_LaPhiPhongCoAn(nItemIdx)
    local nKey = GetGlodEqIndex(nItemIdx);
    if nKey and nKey >= PF_AN_KEY_TOITHIEU then
        return 1;
    end
    return 0;
end

-- quay 2 dong an va ghi vao khe 0/1 cua custom-magic (SetMagicAttrib ghi de CA 8 khe
-- - phi phong hoang kim khong dung khe nay cho gi khac, da kiem khi lam tooltip).
-- [PF13 31/08b] doc TYPE khe an thu nhat (khe 6 = phan tu 19 cua GetMagicAttrib:
-- moi khe 3 gia tri type/v0/v2, khe 6 bat dau o 6*3+1 = 19). 0 = chua co dong an.
function PF_LayKheAn(nItemIdx)
    local tb = pack(GetMagicAttrib(nItemIdx));
    return tb[19] or 0;
end

-- ghi 2 dong an vao khe 6-7, GIU NGUYEN khe 0-5 (thuoc tinh vang co san cua
-- goldequip - phi phong hoang kim sinh ra DA CO khe 0 khac 0, cam ghi de)
function PF_GhiKheAn(nItemIdx, t1, v1, t2, v2)
    local tb = pack(GetMagicAttrib(nItemIdx));
    SetMagicAttrib(nItemIdx,
        tb[1],tb[2],tb[3], tb[4],tb[5],tb[6], tb[7],tb[8],tb[9],
        tb[10],tb[11],tb[12], tb[13],tb[14],tb[15], tb[16],tb[17],tb[18],
        t1, v1, 0, t2, v2, 0);
end

function PF_RollAnAttr(nItemIdx)
    local n1 = random(1, getn(tbPF_AnNhom1));
    local n2 = random(1, getn(tbPF_AnNhom2));
    local t1 = tbPF_AnNhom1[n1];
    local t2 = tbPF_AnNhom2[n2];
    PF_GhiKheAn(nItemIdx, t1[1], t1[2], t2[1], t2[2]);
    SyncItem(nItemIdx);
    Msg2Player("<color=purple>Phi Phong ®· sinh thuéc tÝnh Èn: dßng 1 ph¸t huy ngay, dßng 2 më khi ®¹t 10 sao.<color>");
    return 1;
end

tbStarUpGradeData = {
    [5374] = { -- ¾øÊÀÅû·ç
        tbRatePreStar = {1.0, 0.9, 0.85, 0.80, 0.70, 0.6, 0.55, 0.35, 0.25, 0.2,},
    },
    [5375] = { -- ÆÆ¾üÅû·ç£¨Ë²ÒÆ£©
        tbRatePreStar = {0.5, 0.45, 0.425, 0.375, 0.325, 0.275, 0.25, 0.175, 0.125, 0.1,},
        
    },
    [5376] = { -- °ÁÑ©Åû·ç£¨Ë²ÒÆ£©
        tbRatePreStar = {0.333333333, 0.3, 0.283333333, 0.233333333, 0.2, 0.166666667, 0.15, 0.116666667, 0.083333333, 0.066666667},
    },
    [5377] = { -- ¾ªÀ×Åû·ç£¨Ë²ÒÆ£©
        tbRatePreStar = {0.25, 0.2, 0.1875, 0.175, 0.15, 0.1125, 0.1, 0.0875, 0.0625, 0.05},
    },
    [5378] = { -- Óù·çÅû·ç£¨Ë²ÒÆ£©
				tbRatePreStar = {0.2, 0.16, 0.15, 0.12, 0.1, 0.08, 0.07, 0.07, 0.05, 0.04,},
    },
    [5939] = { -- [PF13 31/08] Phe Quang (ti le noi suy, DE CHINH)
        tbRatePreStar = {2.0, 1.8, 1.6, 1.5, 1.4, 1.2, 1.1, 1.0, 0.8, 0.7},
        eVL = 8,
    },
    [5940] = { -- [PF13 31/08] Khap Than (ti le noi suy, DE CHINH)
        tbRatePreStar = {2.0, 1.8, 1.6, 1.4, 1.3, 1.1, 1.0, 0.9, 0.75, 0.65},
        eVL = 8,
    },
    [5959] = { -- [PF13 31/08] Huyen Kim (ti le noi suy, DE CHINH)
        tbRatePreStar = {1.8, 1.6, 1.5, 1.3, 1.2, 1.0, 0.9, 0.8, 0.7, 0.6},
        eVL = 8,
    },
    [5960] = { -- [PF13 31/08] Vo Cuc (ti le noi suy, DE CHINH)
        tbRatePreStar = {1.8, 1.6, 1.4, 1.2, 1.1, 1.0, 0.85, 0.75, 0.65, 0.55},
        eVL = 9,
    },
    [5961] = { -- [PF13 31/08] Kinh Thien (ti le noi suy, DE CHINH)
        tbRatePreStar = {1.6, 1.5, 1.3, 1.2, 1.0, 0.9, 0.8, 0.7, 0.6, 0.5},
        eVL = 9,
    },
    [6733] = { -- [PF13 31/08] Lang Tuyet (ti le noi suy, DE CHINH)
        tbRatePreStar = {1.6, 1.4, 1.2, 1.1, 1.0, 0.85, 0.75, 0.65, 0.55, 0.5},
        eVL = 9,
    },
    [6734] = { -- [PF13 31/08] Long Ngam (ti le noi suy, DE CHINH)
        tbRatePreStar = {1.5, 1.3, 1.2, 1.0, 0.9, 0.8, 0.7, 0.6, 0.5, 0.45},
        eVL = 10,
    },
    [6735] = { -- [PF13 31/08] So Phuong (ti le noi suy, DE CHINH)
        tbRatePreStar = {1.4, 1.2, 1.1, 1.0, 0.85, 0.75, 0.65, 0.55, 0.48, 0.4},
        eVL = 10,
    },
    szErrLevelMsg = "T¨ng sao Phi Phong nµy ®· ®Çy, kh«ng thÓ ®ét ph¸ t¨ng cÊp.",
    szErrNumMsg = "ChØ cã thÓ ®Æt vµo 1 Phi Phong míi cã thÓ t¨ng sao.",
    szErrFinishMsg = "T¨ng sao Phi Phong nµy ®· ®Çy, kh«ng thÓ ®ét ph¸ t¨ng cÊp.",
    szErrNoMatleMsg = "H·y ®Æt vµo 1 Phi Phong míi cã thÓ t¨ng sao.",
    szErrItemLess = "H·y ®Æt vµo sè l­îng Tinh Ngäc nhÊt ®Þnh.",
    szErrItem = "<color=red>H·y ®Æt vµo Phi Phong cã thÓ t¨ng sao vµ sè l­îng Tinh Ngäc nhÊt ®Þnh.<color>",
    szErrTimes = "Phi Phong cÊp kÕ ch­a më ®ét ph¸.",
}

tbBreakThrough = {
    [5374] = { -- ¾øÊÀÅû·ç
        nRateBreak = 15.0,
        eBreakType = "ProBreak", 
        tbNewMantle = {0,5375},
        nMinOpenDays = 10,
    },
    [5375] = { -- ÆÆ¾üÅû·ç£¨Ë²ÒÆ£©
        nRateBreak = 10.0,
        eBreakType = "ProBreak", 
        tbNewMantle = {0,5376},
        nMinOpenDays = 20,
    },
    [5376] = { -- °ÁÑ©Åû·ç£¨Ë²ÒÆ£©
        nRateBreak = 8.0,
        eBreakType = "ProBreak", 
        tbNewMantle = {0,5377},
        nMinOpenDays = 40,
    },
    [5377] = { -- ¾ªÀ×Åû·ç£¨Ë²ÒÆ£©
        nRateBreak = 5.0,
        eBreakType = "ProBreak", 
        tbNewMantle = {0,5378},
        nMinOpenDays = 60,
    },
    [5378] = { -- Óù·çÅû·ç£¨Ë²ÒÆ£©[²»ÄÜ½øÐÐÆÕÍ¨Í»ÆÆ]
        -- [PF13 31/08] bo duong platina Vo Cuc (JX2/Linux); theo VLTK 2023:
        -- Ngu Phong 10 sao + 30 Bach Luyen Thanh Cuong -> Phe Quang (100%).
        nRateBreak = 100.0,
        eBreakType = "CaoBreak",
        tbNewMantle = {0,5939},
        eVL = ITEM_TYPE_BLCG,
        nNeedItem = 30,
--        nMinOpenDays = 0,
    },
    [5939] = { -- [PF13 31/08] Phe Quang -> Khap Than
        nRateBreak = 100.0,
        eBreakType = "CaoBreak",
        tbNewMantle = {0,5940},
        eVL = ITEM_TYPE_BLCG,
        nNeedItem = 50,
    },
    [5940] = { -- [PF13 31/08] Khap Than -> Huyen Kim
        nRateBreak = 100.0,
        eBreakType = "CaoBreak",
        tbNewMantle = {0,5959},
        eVL = ITEM_TYPE_BLCG,
        nNeedItem = 70,
    },
    [5959] = { -- [PF13 31/08] Huyen Kim -> Vo Cuc
        nRateBreak = 100.0,
        eBreakType = "CaoBreak",
        tbNewMantle = {0,5960},
        eVL = ITEM_TYPE_BLCG,
        nNeedItem = 100,
    },
    [5960] = { -- [PF13 31/08] Vo Cuc -> Kinh Thien
        nRateBreak = 100.0,
        eBreakType = "CaoBreak",
        tbNewMantle = {0,5961},
        eVL = ITEM_TYPE_DBLCG,
        nNeedItem = 10,
    },
    [5961] = { -- [PF13 31/08] Kinh Thien -> Lang Tuyet
        nRateBreak = 100.0,
        eBreakType = "CaoBreak",
        tbNewMantle = {0,6733},
        eVL = ITEM_TYPE_DBLCG,
        nNeedItem = 15,
    },
    [6733] = { -- [PF13 31/08] Lang Tuyet -> Long Ngam
        nRateBreak = 100.0,
        eBreakType = "CaoBreak",
        tbNewMantle = {0,6734},
        eVL = ITEM_TYPE_DBLCG,
        nNeedItem = 20,
    },
    [6734] = { -- [PF13 31/08] Long Ngam -> So Phuong
        nRateBreak = 100.0,
        eBreakType = "CaoBreak",
        tbNewMantle = {0,6735},
        eVL = ITEM_TYPE_DBLCG,
        nNeedItem = 25,
    },
    [6735] = { -- [PF13 31/08] So Phuong = bac toi cao
        nRateBreak = 0,
        eBreakType = "CaoBreak",
        tbNewMantle = nil,
    },
    ["CaoBreak"] = { -- [PF13 31/08]
        szErrLevelMsg = "ChØ cã Phi Phong 10 Sao míi cã thÓ ®ét ph¸ bËc cao.",
        szErrTypeMsg = "Phi Phong nµy ch­a tíi bËc cao, h·y dïng môc ®ét ph¸ th­êng.",
        szErrNumMsg = "ChØ cã thÓ ®Æt vµo 1 Phi Phong.",
        szErrNoMatleMsg = "H·y ®Æt vµo Phi Phong 10 Sao cÇn ®ét ph¸.",
        szErrItemLess = "Ch­a ®ñ nguyªn liÖu, hiÖn ®· ®Æt %d c¸i.",
        szErrItem = "<color=red>H·y ®Æt vµo Phi Phong 10 Sao vµ nguyªn liÖu ®ét ph¸ cña bËc ®ã.<color>",
    },
    ["ProBreak"] = {
        szErrLevelMsg = "ChØ cã Phi Phong 10 Sao míi cã thÓ ®ét ph¸ t¨ng cÊp.",
        szErrTypeMsg = "Phi Phong nµy kh«ng thÓ ®ét ph¸ th­êng.",
        szErrNumMsg = "ChØ cã thÓ ®Æt vµo 1 Phi Phong cã thÓ ®ét ph¸.",
        szErrNoMatleMsg = "H·y ®Æt vµo Phi Phong míi cã thÓ ®ét ph¸.",
        szErrItemLess = "H·y ®Æt vµo 1 Thiªn Tinh Ngäc, hiÖn ®· ®Æt vµo %d c¸i.",
        szErrItem = "<color=red>H·y ®Æt vµo Phi Phong cã thÓ ®ét ph¸ th­êng vµ 1 Thiªn Tinh Ngäc.<color>",
    },
    ["AdvBreak"] = {
        szErrLevelMsg = "ChØ cã Phi Phong Ngù Phong 10 Sao míi cã thÓ n©ng cÊp V« Cùc.",
        szErrTypeMsg = "ChØ cã Phi Phong Ngù Phong 10 Sao míi cã thÓ n©ng cÊp V« Cùc.",
        szErrNumMsg = "ChØ cã thÓ ®Æt vµo 1 Phi Phong cã thÓ ®ét ph¸.",
        szErrNoMatleMsg = "H·y ®Æt vµo Phi Phong Ngù Phong 10 Sao.",
        szErrItemLess = "H·y ®Æt vµo 100 B¸ch LuyÖn Thµnh C­¬ng, hiÖn ®· ®Æt %d c¸i.",
        szErrItem = "<color=red>H·y ®Æt vµo Phi Phong cã thÓ n©ng cÊp V« Cùc vµ 100 B¸ch LuyÖn Thµnh C­¬ng.<color>",
    }
}

tbSecBreakThrough = {
    [3485] = { -- ÎÞ¼«Åû·ç(0Ïâ¿×)
        tbNewMantle = {0,4835},
        nMinOpenDays = 0,
    },
    [4835] = { -- ÎÞ¼«Åû·ç(1Ïâ¿×)
        tbNewMantle = {0,4836},
        nMinOpenDays = 10,
    },
    [4836] = { -- ÎÞ¼«Åû·ç(2Ïâ¿×)
        tbNewMantle = {0,4837},
        nMinOpenDays = 20,
    },
    [4837] = { -- ÎÞ¼«Åû·ç(3Ïâ¿×)
        tbNewMantle = {0,4838},
        nMinOpenDays = 40,
    },
    [4838] = { -- ÎÞ¼«Åû·ç(4Ïâ¿×)
        tbNewMantle = {0,4839},
        nMinOpenDays = 60,
    },
    [4839] = { -- ÎÞ¼«Åû·ç(5Ïâ¿×)
        tbNewMantle = nil, -- ²»ÄÜÔÙ¶þ´ÎÍ»ÆÆ
    },
    tbItemNeed = {15,20,25,30,35,0},
    szErrLevelMsg = "H·y ®Æt vµo HuyÒn Kim + 10 Phi Phong V« Cùc.",
    szErrTypeMsg = "Phi Phong nµy c¸ch lÇn ®ét ph¸ tr­íc kh«ng ®ñ 15 ngµy, kh«ng thÓ tiÕn hµnh ®ét ph¸ lÇn 2 HuyÒn Kim V« Cùc.",
    szErrNumMsg = "ChØ cã thÓ ®Æt vµo 1 HuyÒn Kim + 10 Phi Phong V« Cùc.",
    szErrFinishMsg = "5 lç kh¶m cña Phi Phong nµy ®Òu ®· ®­îc më, kh«ng thÓ tiÕn hµnh ®ét ph¸ lÇn 2 HuyÒn Kim V« Cùc n÷a.",
    szErrNoMatleMsg = "H·y ®Æt vµo HuyÒn Kim cã thÓ ®ét ph¸ + 10 Phi Phong.",
    szErrItemLess = "H·y ®Æt vµo %d B¸ch LuyÖn Thµnh C­¬ng, hiÖn ®· ®Æt vµo %d c¸i.",
    szErrItem = "<color=red>H·y ®Æt vµo HuyÒn Kim + 10 Phi Phong V« Cùc vµ 25 B¸ch LuyÖn Thµnh C­¬ng.<color>",
}

tbMantleSystem = {
    nType = 0,              -- Åû·çÉý¼¶ÀàÐÍ ÉýÐÇ¡¢Í»ÆÆ¡¢ÏâÇ¶
    szTitle = "",           -- ¸øÓèÃæ°åÏÂ·½ÌáÊ¾
    nNeedMoney = 0,         -- ËùÐè½ðÇ®
    nNeedItem = 0,          -- ËùÐèÉý¼¶²ÄÁÏ¸öÊý
    nMantleIdx = 0,         -- ·ÅÈëÅû·çµÄItemIndex
    nMantleKey = 0,         -- ·ÅÈëÅû·çµÄKey
    tbAllItemIndex = {},    -- ËùÓÐÎïÆ·µÄItemIndex
    szShowTips = "",        -- Éý¼¶³É¹¦ÂÊ¡¢ÏûºÄ¡¢½ðÇ®ÌáÊ¾
    szErrorMsg = "",        -- ´íÎó·µ»ØÐÅÏ¢
    nSuccessRate = 0,       -- ³É¹¦ÂÊ
    nStarCount = 0,         -- ÐÇÓñÊýÁ¿
    nDungSo = 0,            -- [VA 31/08h] so DON VI nguyen lieu se thuc su tieu thu
                            -- (tha ca chong van chi tru dung so can)
}

function tbMantleSystem:New()
    local tbObj = clone(self);
    return tbObj
end

function tbMantleSystem:InitCheckData()
    self.nMantleIdx = 0;
    self.szShowTips = "";
    self.szErrorMsg = "";
    self.nSuccessRate = 0;
    self.nStarCount = 0;
    self.nDungSo = 0;
end

function tbMantleSystem:OnTest(tbItemIdx)
    local szReStr = self.szTitle;
    local nSize = getn(tbItemIdx);
    for i = 1, nSize do
        local nIndex = tbItemIdx[i];
        szReStr = szReStr..format("\nVËt phÈm: <color=green>%s<color>", GetItemName(nIndex));
    end
    return szReStr;
end

function tbMantleSystem:WriteMantleLog(szLog)
    if szLog == nil or szLog == "" then
        return 0;
    end
    local szWriteStr = format("[HÖ thèng Phi Phong] %s\t%s\t%s\t%s", GetLocalDate("%Y-%m-%d %H:%M"), GetAccount(), GetName(), szLog);
    WriteLog(szWriteStr);
    return 1;
end

function tbMantleSystem:GetServerOpenDays()
    local nOpenDays = 0;
    local nServerTime = tbTimeLineClass:ConvertTimeToTM(nil);
    if nServerTime >= 20120920 then
        nOpenDays = ceil((GetCurServerTime() - tbTimeLineManager.nOpenServerTime) / (24*60*60));
    else
        nOpenDays = 1000;
    end
    return nOpenDays;
end

function tbMantleSystem:GetItemType(nItemIndex)
    if nItemIndex < 0 then
        return ITEM_TYPE_ERROR
    end
    local nG, nD, nP = GetItemProp(nItemIndex);
    if nG == 0 and nD == 12 and nP == 0 then
        local nQuality = GetItemQuality(nItemIndex)
        local nType = ITEM_TYPE_HJ_PIFENG
        if 4 == nQuality then
            nType = ITEM_TYPE_BJ_PIFENG
        end
        return nType
    elseif nG == 6 and nD == 1 and nP == 4881 then
        return ITEM_TYPE_XINGYU
    elseif nG == 6 and nD == 1 and nP == 4882 then
        return ITEM_TYPE_TIANXY
    elseif nG == 6 and nD == 1 and nP == 4884 then
        return ITEM_TYPE_BLCG
    elseif nG == 6 and nD == 1 and nP == 4933 then -- [PF13 31/08]
        return ITEM_TYPE_DBLCG
    elseif nG == 6 and nD == 1 and nP == 4934 then
        return ITEM_TYPE_PHACHTN
    elseif nG == 6 and nD == 1 and nP == 4935 then
        return ITEM_TYPE_DAITT
    elseif nG == 6 and nD == 1 and nP == 4936 then
        return ITEM_TYPE_KIMTINH
    elseif nG == 9 and nD == 1 then
        return ITEM_TYPE_XINGCS
    else
        return ITEM_TYPE_ERROR
    end
end

function tbMantleSystem:OnCheckLock(bBoxLock, bLingLongLock, bItemLock, bExpime)
    -- ´¢ÎïÏäËø¶¨×´Ì¬
	if bBoxLock == 1 and GetBoxLockState() == 1 then
		Talk(1, "", "Ng­¬i ®ang trong tr¹ng th¸i r­¬ng chøa ®å bÞ khãa, h·y më khãa tr­íc!");
		return 0;
	end
	-- ¼ì²éÊÇ·ñÓÐÓÐÐ§ÆÚ
	local nEquipExpiredTime = ITEM_GetExpiredTime(self.nMantleIdx);
	if bExpime == 1 and nEquipExpiredTime > 0 then
		Talk(1, "", "Kh«ng thÓ t¨ng cÊp Phi Phong cã h¹n sö dông.");
		return 0;
	end
	
	return 1;
end

function tbMantleSystem:OnCheckItem(tbItemIdx)
    local nResult = 0;
    self:InitCheckData();
    self.tbAllItemIndex = tbItemIdx;
    if self.nType == TYPE_STAR_UPGRADE then
        nResult = self:CheckStarUpGrade();
    elseif self.nType == TYPE_PRO_BREAK_THROUGH then
        nResult = self:CheckProBreakThrough();
    elseif self.nType == TYPE_CAO_BREAK_THROUGH then -- [PF13 31/08]
        nResult = self:CheckCaoBreakThrough();
    elseif self.nType == TYPE_TAY_AN_ATTR then -- [PF13 31/08]
        nResult = self:CheckTayAn();
    elseif self.nType == TYPE_ADVANCE_TO_WUJI then
        nResult = self:CheckAdvanceToWuJi();
    elseif self.nType == TYPE_SEC_BREAK_THROUGH then
        nResult = self:CheckSecBreakThrough();
    elseif self.nType == TYPE_INLAY_STAR_STONE then
        nResult = self:CheckInlayStarStone();
    end

    return nResult;
end

function tbMantleSystem:GetConfimTitle()
    local szReStr = "";
    if self.nType == TYPE_STAR_UPGRADE then
        szReStr = "<dec><npc>Phi phong sau khi t¨ng sao sÏ khãa cïng nh©n vËt, ®ång ý?";
    elseif self.nType == TYPE_PRO_BREAK_THROUGH then
        szReStr = "<dec><npc>§ång ý ®ét ph¸ th­êng?";
    elseif self.nType == TYPE_CAO_BREAK_THROUGH then -- [PF13 31/08]
        szReStr = "<dec><npc>§ång ý ®ét ph¸ bËc cao? Nguyªn liÖu vµ b¹c sÏ bÞ trõ.";
    elseif self.nType == TYPE_TAY_AN_ATTR then -- [PF13 31/08]
        szReStr = "<dec><npc>§ång ý tÈy l¹i 2 dßng thuéc tÝnh Èn? Dßng cò sÏ mÊt.";
    elseif self.nType == TYPE_ADVANCE_TO_WUJI then
        szReStr = "<dec><npc>§ång ý tiÕn hµnh n©ng cÊp V« Cùc? T¨ng cÊp xong kh«ng thÓ quay l¹i.";
    elseif self.nType == TYPE_SEC_BREAK_THROUGH then
        szReStr = "<dec><npc>§ång ý ®ét ph¸ lÇn 2 HuyÒn Kim V« Cùc? T¨ng cÊp xong kh«ng thÓ quay l¹i.";
    elseif self.nType == TYPE_INLAY_STAR_STONE then
        szReStr = "<dec><npc>Tinh ThÇn Th¹ch sau khi kh¶m vµo sÏ kh«ng thÓ th¸o ra, ®ång ý?";
    end
    return szReStr;
end

function tbMantleSystem:DoMantleUpGrade()
    -- [VA 31/08b] 0 = giao dich CHUA chay (khoa ruong/het han) -> npc.lua mo lai hop
    local nKQ = 1;
    if self.nType == TYPE_STAR_UPGRADE then
        nKQ = self:ProcessStarUpGrade();
    elseif self.nType == TYPE_PRO_BREAK_THROUGH then
        nKQ = self:ProcessProBreakThrough();
    elseif self.nType == TYPE_CAO_BREAK_THROUGH then -- [PF13 31/08] dung chung duong xu ly
        nKQ = self:ProcessProBreakThrough();
    elseif self.nType == TYPE_TAY_AN_ATTR then -- [PF13 31/08]
        nKQ = self:ProcessTayAn();
    elseif self.nType == TYPE_ADVANCE_TO_WUJI then
        nKQ = self:ProcessAdvBreakThrough();
    elseif self.nType == TYPE_SEC_BREAK_THROUGH then
        nKQ = self:ProcessSecBreakThrough();      
    elseif self.nType == TYPE_INLAY_STAR_STONE then
        nKQ = self:ProcessInlayStarStone();        
    end
    if nKQ == 0 then
        return 0;
    end
    return 1;
end

function tbMantleSystem:CheckStarUpGrade()
    self.tbDemSao = {}; -- [PF13 31/08] dem rieng tung loai nguyen lieu tang sao
    local nStarLevel = 0;
    local nMantleKey = 0;
    local tbItemIdx = self.tbAllItemIndex;
    local nSize = getn(tbItemIdx);
    for i = 1, nSize do
        local eItemType = self:GetItemType(tbItemIdx[i]);
        if eItemType == ITEM_TYPE_HJ_PIFENG then    -- ÊÇÅû·ç
            nMantleKey = GetGlodEqIndex(tbItemIdx[i]);
            if tbStarUpGradeData[nMantleKey] then
                -- ÊÇ·ñÒÑ¾­·ÅÈëÁËÅû·ç
                if self.nMantleIdx ~= 0 then
                    self.szErrorMsg = tbStarUpGradeData.szErrNumMsg;
                    break;
                end
                -- ÅÐ¶ÏÐÇ¼¶
                nStarLevel = GetStarLevel(tbItemIdx[i]);
                if nStarLevel >= 10 then
                    self.szErrorMsg = tbStarUpGradeData.szErrFinishMsg;
                    break;
                end
                -- ±£´æÅû·çµÄItemIndex
                self.nMantleIdx = tbItemIdx[i];
                self.nMantleKey = nMantleKey;
            else
                self.szErrorMsg = tbStarUpGradeData.szErrItem;
            end
        elseif eItemType == ITEM_TYPE_XINGYU or eItemType == ITEM_TYPE_PHACHTN
            or eItemType == ITEM_TYPE_DAITT or eItemType == ITEM_TYPE_KIMTINH then
            -- [PF13 31/08] 4 loai nguyen lieu tang sao theo bac (VLTK); doi chieu
            -- dung loai sau vong lap. Moi muc = 1 don vi ([VA 31/08b]).
            self.tbDemSao[eItemType] = (self.tbDemSao[eItemType] or 0) + 1;
        else    -- ÆäËûÎïÆ·
            self.szErrorMsg = tbStarUpGradeData.szErrItem;
            break;
        end
    end
    -- Ã»ÓÐ·ÅÈëÅû·ç
    -- [PF13 31/08] doi chieu DUNG loai nguyen lieu tang sao cua bac nay
    local eVLCan = ITEM_TYPE_XINGYU;
    if nMantleKey ~= 0 and tbStarUpGradeData[nMantleKey] and tbStarUpGradeData[nMantleKey].eVL then
        eVLCan = tbStarUpGradeData[nMantleKey].eVL;
    end
    self.nStarCount = self.tbDemSao[eVLCan] or 0;
    if self.szErrorMsg == "" then
        local nKhac = 0;
        if eVLCan ~= ITEM_TYPE_XINGYU and self.tbDemSao[ITEM_TYPE_XINGYU] then nKhac = 1; end
        if eVLCan ~= ITEM_TYPE_PHACHTN and self.tbDemSao[ITEM_TYPE_PHACHTN] then nKhac = 1; end
        if eVLCan ~= ITEM_TYPE_DAITT and self.tbDemSao[ITEM_TYPE_DAITT] then nKhac = 1; end
        if eVLCan ~= ITEM_TYPE_KIMTINH and self.tbDemSao[ITEM_TYPE_KIMTINH] then nKhac = 1; end
        if nKhac == 1 then
            self.szErrorMsg = format("BËc phi phong nµy t¨ng sao b»ng %s, h·y bá nguyªn liÖu kh¸c ra.", tbPF_TenVLSao[eVLCan] or "?");
        end
    end
    if self.nMantleIdx == 0 and self.szErrorMsg == "" then
        self.szErrorMsg = tbStarUpGradeData.szErrNoMatleMsg;
    end
    -- Ã»ÓÐ·ÅÈëÐÇÓñ
    if self.nStarCount == 0 and self.szErrorMsg == "" then
        self.szErrorMsg = tbStarUpGradeData.szErrItemLess;
    end
    -- ÎïÆ·¼ì²é²»Í¨¹ý£¬ÖÕÖ¹¼ì²é
    if self.szErrorMsg ~= "" then
    		self.nSuccessRate = 0
        return 0;
    end
    -- ¼ÆËã³É¹¦ÂÊ
    local nRatePreStar = tbStarUpGradeData[nMantleKey].tbRatePreStar[nStarLevel+1];
    -- [VA 31/08h] cho phep tha CA CHONG: chi dung so vien du dat 100%, phan du
    -- GIU LAI trong hop (truoc day an sach ca chong -> phai tach tay tung vien).
    local nToiDa = self.nStarCount;
    if nRatePreStar > 0 then
        nToiDa = ceil(100.0 / nRatePreStar);
    end
    if nToiDa < 1 then
        nToiDa = 1;
    end
    self.nDungSo = self.nStarCount;
    if self.nDungSo > nToiDa then
        self.nDungSo = nToiDa;
    end
    self.nSuccessRate = nRatePreStar*self.nDungSo;
    -- ÏêÏ¸Ãæ°åÐÅÏ¢
    self.szShowTips = format("TØ lÖ thµnh c«ng: <color=green>%.2f%%<color>", self.nSuccessRate);
    if self.nSuccessRate > 100.0 then
        self.szShowTips = format("TØ lÖ thµnh c«ng: <color=red>%.2f%%<color>", self.nSuccessRate);
    end
    if self.nStarCount > self.nDungSo then
        self.szShowTips = self.szShowTips..format("<enter>§Æt vµo %d viªn, chØ dïng %d viªn lµ ®ñ, phÇn d­ gi÷ nguyªn.", self.nStarCount, self.nDungSo);
    else
        self.szShowTips = self.szShowTips..format("<enter>Sè Tinh Ngäc ®Æt vµo: %d", self.nStarCount);
    end
    self.szShowTips = self.szShowTips.."<enter>B¹c tiªu hao: 100,0000";
    return 1;
end

function tbMantleSystem:ProcessStarUpGrade()
    if self:OnCheckLock(1,1,1,1) == 0 then
        return 0;
    end
    -- ¼ì²é½£ÏÀ±Ò
    local nCurMoney = GetCash();
    if nCurMoney < self.nNeedMoney then
        local szErrorTips = format("HiÖn vµng kh«ng ®ñ, cÇn Ýt nhÊt %dW l­îng míi cã thÓ hoµn thµnh.", floor(self.nNeedMoney/10000));
        Talk(1, "", szErrorTips);
        return 0;
    end
    -- ¿ÛÇ®
    Pay(self.nNeedMoney);
    -- ¿Û³ýÐÇÓñ
    local tbItemIdx = self.tbAllItemIndex;
    local nSize = getn(tbItemIdx);
    -- [VA 31/08h] chi tru DUNG SO can; phan du cua chong o lai trong hop
    local nConTru = self.nDungSo;
    if nConTru <= 0 then
        nConTru = nSize;
    end
    for i = 1, nSize do
        if self.nMantleIdx ~= tbItemIdx[i] and nConTru > 0 then
            RemoveItemByIndex(tbItemIdx[i]);
            nConTru = nConTru - 1;
        end
    end
    local szMsgTips = "";
    local szLogInfo = "";
    local nCurRate = random(1, 100);
    local nCurLevel = GetStarLevel(self.nMantleIdx);
    local szItemName = GetItemName(self.nMantleIdx);
    if nCurRate <= self.nSuccessRate then
        local nReCode = StarLevelUp(self.nMantleIdx);
        if nReCode == 1 then
            SetItemBindState(self.nMantleIdx, -2); -- ÉýÐÇºó£¬×°±¸ÓÀ¾Ã°ó¶¨
            SyncItem(self.nMantleIdx);
            -- [PF13 31/08] Long Ngam+ vua dat 10 sao va chua co dong an -> quay
            -- [PF13 31/08b] kiem KHE 6 (dong an) chu khong phai khe 0 - phi
            -- phong hoang kim sinh ra da co thuoc tinh vang o khe 0
            if nCurLevel + 1 >= 10 and PF_LaPhiPhongCoAn(self.nMantleIdx) == 1 then
                if PF_LayKheAn(self.nMantleIdx) == 0 then
                    PF_RollAnAttr(self.nMantleIdx);
                end
            end
            szMsgTips = format("T¨ng sao Phi Phong thµnh c«ng, nhËn ®­îc 1 c¸i %d Sao %s", nCurLevel+1, szItemName);
            --%dÐÇµÄ%sÉýÐÇ£¬ÏûºÄÐÇÓñÊýÁ¿:%d,³É¹¦ÂÊ:%.2f(Ëæ»úÖµ:%.2f),½á¹û:³É¹¦,»ñµÃ%dÐÇµÄ%s
            szLogInfo = format("StarUpGrade\t%d\t%s\t%d\t%.2f\t%.2f\tsuccess\t%d\t%s",
                nCurLevel, szItemName, self.nStarCount, self.nSuccessRate, nCurRate, nCurLevel+1, szItemName);
        else
            szMsgTips = "T¨ng sao thÊt b¹i.";
            --%dÐÇµÄ%sÉýÐÇ£¬ÏûºÄÐÇÓñÊýÁ¿:%d,³É¹¦ÂÊ:%.2f(Ëæ»úÖµ:%.2f),½á¹û:Ê§°Ü(ErrorCode:%d ÄÚ²¿´íÎó)
            szLogInfo = format("StarUpGrade\t%d\t%s\t\t%d\t%.2f\t%.2f\tfailed\tErrorCode:%d",
                nCurLevel, szItemName, self.nStarCount, self.nSuccessRate, nCurRate, nReCode);
        end
    else
        szMsgTips = "T¨ng sao thÊt b¹i.";
        --%dÐÇµÄ%sÉýÐÇ£¬ÏûºÄÐÇÓñÊýÁ¿:%d,³É¹¦ÂÊ:%.2f(Ëæ»úÖµ:%.2f),½á¹û:Ê§°Ü(ÔËÆø²»¼Ñ£¡£¡)£¬ÐÇ¶È²»±ä
        szLogInfo = format("StarUpGrade\t%d\t%s\t%d\t%.2f\t%.2f\tfailed",
                nCurLevel, szItemName, self.nStarCount, self.nSuccessRate, nCurRate);
    end
    AddStatData("pifeng_star_up");
    Talk(1, "", szMsgTips);
    self:WriteMantleLog(szLogInfo);
    return 1;
end

function tbMantleSystem:CheckBreakThrough(szType, eNeedItemType)
    local nStarLevel = 0;
    local nMantleKey = 0;
    local szMantleName= "";
    local tbItemIdx = self.tbAllItemIndex;
    local nSize = getn(tbItemIdx);
    for i = 1, nSize do
        local eItemType = self:GetItemType(tbItemIdx[i]);
        if eItemType == ITEM_TYPE_HJ_PIFENG then
            nMantleKey = GetGlodEqIndex(tbItemIdx[i]);
            if tbBreakThrough[nMantleKey] then
                -- ÅÐ¶ÏÐÇ¼¶
                nStarLevel = GetStarLevel(tbItemIdx[i]);
                if nStarLevel < 10 then
                    self.szErrorMsg = tbBreakThrough[szType].szErrLevelMsg;
                    break;
                end
                -- ¼ì²éÄÜ²»ÄÜÉý¼¶
                if szType ~= tbBreakThrough[nMantleKey].eBreakType then
                    self.szErrorMsg = tbBreakThrough[szType].szErrTypeMsg;
                    break;
                end
                -- ¼ì²éÊÇ·ñÒÑ¾­·ÅÈëÅû·ç
                if self.nMantleIdx ~= 0 then
                    self.szErrorMsg = tbBreakThrough[szType].szErrNumMsg;
                    break;
                end
                -- ¼ì²é¿ª·þÌìÊý£¬´Ó´óÂ½°æÒÆÖ²£¬µ«ÊÇÎÞÓÃ
                --local nOpenDays = self:GetServerOpenDays();
                --if nOpenDays < tbBreakThrough[nMantleKey].nMinOpenDays then
                    --self.szErrorMsg = tbStarUpGradeData.szErrTimes;
                    --break;
                --end
                -- ±ê¼ÇÒÑ·ÅÈëÅû·ç
                self.nMantleIdx = tbItemIdx[i];
                self.nMantleKey = nMantleKey;
            else
                self.szErrorMsg = tbBreakThrough[szType].szErrItem;
            end
        elseif eItemType == eNeedItemType then
            self.nStarCount = self.nStarCount + 1; -- [VA 31/08b] GiveBoxCollect khai trien chong: moi muc = 1 don vi (LECH Linux co chu y)
        elseif eNeedItemType == ITEM_TYPE_CAO_HONHOP and (eItemType == ITEM_TYPE_BLCG or eItemType == ITEM_TYPE_DBLCG) then
            -- [PF13 31/08] CaoBreak: nhan ca 2 loai, CheckCaoBreakThrough doi chieu loai dung theo bac
            self.tbDemCao[eItemType] = (self.tbDemCao[eItemType] or 0) + 1;
        else
            self.szErrorMsg = tbBreakThrough[szType].szErrItem;
            break;
        end
    end
    -- ¼ì²éÊÇ·ñ·ÅÈëÅû·ç
    if self.nMantleIdx == 0 and self.szErrorMsg == "" then
        self.szErrorMsg = tbBreakThrough[szType].szErrNoMatleMsg;
    end
    -- ¼ì²éÎïÁÏ¸öÊý
    -- [VA 31/08h] chi doi DU (truoc day doi DUNG BANG nen tha ca chong la bi
    -- tu choi, phai tach tay tung vien). Phan du giu nguyen trong hop.
    if self.nStarCount < self.nNeedItem and self.szErrorMsg == "" then
        self.szErrorMsg = format(tbBreakThrough[szType].szErrItemLess, self.nStarCount);
    end
    self.nDungSo = self.nNeedItem;
    -- ÎïÆ·¼ì²é²»Í¨¹ý£¬ÖÕÖ¹¼ì²é
    if self.szErrorMsg ~= "" then
        return 0;
    end
    return 1;
end

function tbMantleSystem:CheckProBreakThrough()
    local nResult = self:CheckBreakThrough("ProBreak", ITEM_TYPE_TIANXY);
    if nResult == 0 then
        return nResult;
    end
    -- ¼ÆËã³É¹¦ÂÊ
    self.nSuccessRate = tbBreakThrough[self.nMantleKey].nRateBreak;
    local nCurWishValue = GetCurEquipWishValue(self.nMantleIdx);
    local nMaxWishValue = GetMaxEquipWishValue(self.nMantleIdx);
    if nCurWishValue >= nMaxWishValue then
        self.nSuccessRate = 100.0;
    end
    -- ÏêÏ¸Ãæ°åÐÅÏ¢
    self.szShowTips = format("TØ lÖ thµnh c«ng ®ét ph¸ lÇn nµy: <color=green>%.2f%%<color>", self.nSuccessRate);
    self.szShowTips = self.szShowTips..format("<enter>§iÓm chóc phóc ®ét ph¸ phi phong: <color=red>%d/%d<color>", nCurWishValue, nMaxWishValue);
    self.szShowTips = self.szShowTips.."<enter>B¹c tiªu hao: 100,0000";
    return 1;
end

-- [PF13 31/08] Dot pha bac cao theo VLTK 2023: Ngu Phong -> Phe Quang -> ...
-- -> So Phuong. Bac 5..8 dung Bach Luyen Thanh Cuong, bac 9+ dung Dai Bach
-- Luyen (theo mo ta item client VLTK). Ti le 100% (hop thanh khong that bai);
-- SO LUONG theo bac = tbBreakThrough[key].nNeedItem (20 vien 11->12 theo VNG,
-- cac bac khac noi suy - DE CHINH).
function tbMantleSystem:CheckCaoBreakThrough()
    self.tbDemCao = {};
    local nResult = self:CheckBreakThrough("CaoBreak", ITEM_TYPE_CAO_HONHOP);
    if nResult == 0 then
        return 0;
    end
    local tbCfg = tbBreakThrough[self.nMantleKey];
    if tbCfg.tbNewMantle == nil then
        self.szErrorMsg = "Phi Phong ®· lµ bËc tèi cao, kh«ng thÓ ®ét ph¸ tiÕp.";
        return 0;
    end
    local eVLCan = tbCfg.eVL or ITEM_TYPE_DBLCG;
    local nCan = tbCfg.nNeedItem or 1;
    local szTenVL = tbPF_TenVLCao[eVLCan] or "?";
    local nCo = self.tbDemCao[eVLCan] or 0;
    local nKhac = 0;
    if eVLCan ~= ITEM_TYPE_BLCG and self.tbDemCao[ITEM_TYPE_BLCG] then nKhac = 1; end
    if eVLCan ~= ITEM_TYPE_DBLCG and self.tbDemCao[ITEM_TYPE_DBLCG] then nKhac = 1; end
    if nKhac == 1 then
        self.szErrorMsg = format("BËc nµy ®ét ph¸ b»ng %d %s, h·y bá nguyªn liÖu kh¸c ra.", nCan, szTenVL);
        return 0;
    end
    if nCo < nCan then
        self.szErrorMsg = format("CÇn %d %s, hiÖn míi ®Æt %d c¸i (phÇn d­ sÏ gi÷ nguyªn).", nCan, szTenVL, nCo);
        return 0;
    end
    self.nStarCount = nCo;
    self.nNeedItem = nCan;
    self.nDungSo = nCan;  -- ProcessProBreakThrough chi tru dung so nay
    self.nSuccessRate = tbCfg.nRateBreak;
    self.szShowTips = format("TØ lÖ thµnh c«ng: <color=green>%.2f%%<color><enter>Tiªu hao: %d %s + 100,0000 l­îng b¹c", self.nSuccessRate, nCan, szTenVL);
    return 1;
end

function tbMantleSystem:ProcessProBreakThrough()
    -- ¼ì²éÄÜ²»ÄÜÉý¼¶
    if self:OnCheckLock(1,1,1,1) == 0 then
        return 0;
    end
    -- ¼ì²é½£ÏÀ±Ò
    local nCurMoney = GetCash();
    if nCurMoney < self.nNeedMoney then
        local szErrorTips = format("HiÖn vµng kh«ng ®ñ, cÇn Ýt nhÊt %dW l­îng míi cã thÓ hoµn thµnh.", floor(self.nNeedMoney/10000));
        Talk(1, "", szErrorTips);
        return 0;
    end
    -- ¿ÛÇ®
    Pay(self.nNeedMoney);
    -- ¿Û³ýÐÇÓñ
    local tbItemIdx = self.tbAllItemIndex;
    local nSize = getn(tbItemIdx);
    -- [VA 31/08h] chi tru DUNG SO can; phan du cua chong o lai trong hop
    local nConTru = self.nDungSo;
    if nConTru <= 0 then
        nConTru = nSize;
    end
    for i = 1, nSize do
        if self.nMantleIdx ~= tbItemIdx[i] and nConTru > 0 then
            RemoveItemByIndex(tbItemIdx[i]);
            nConTru = nConTru - 1;
        end
    end
    local szMsgTips = "";
    local szLogInfo = "";
    local nCurValue = GetCurEquipWishValue(self.nMantleIdx);
    local nCurRate = random(1, 100);
    local szOldItemName = GetItemName(self.nMantleIdx);
    if nCurRate <= self.nSuccessRate then
        local tbHasStone = pack(GetStarStoneOnEquip(self.nMantleIdx, -1));
        -- [PF13 31/08] giu 2 dong an (neu co) sang phi phong bac moi
        local tbAnCu = nil;
        if PF_LaPhiPhongCoAn(self.nMantleIdx) == 1 then
            tbAnCu = pack(GetMagicAttrib(self.nMantleIdx));
            -- [PF13 31/08b] dong an nam o khe 6 (phan tu 19)
            if (tbAnCu[19] or 0) == 0 then tbAnCu = nil; end
        end
        RemoveItemByIndex(self.nMantleIdx);
        local nNewIdx = AddGoldEquipByRow(unpack(tbBreakThrough[self.nMantleKey].tbNewMantle));
        if tbAnCu and PF_LaPhiPhongCoAn(nNewIdx) == 1 then
            -- [PF13 31/08b] chi mang 2 KHE AN sang mon moi; khe 0-5 (thuoc
            -- tinh vang cua BAC MOI) phai giu cua chinh no, khong dem 24 gia
            -- tri cu sang de khoi de mat thuoc tinh vang bac moi
            PF_GhiKheAn(nNewIdx, tbAnCu[19], tbAnCu[20], tbAnCu[22], tbAnCu[23]);
        elseif PF_LaPhiPhongCoAn(nNewIdx) == 1 then
            -- [PF13 31/08d] len Long Ngam lan dau (mon cu chua co dong an):
            -- roll ngay 2 dong theo chuan VNG - dong 1 phat huy/hien tu dau,
            -- dong 2 mo khi du 10 sao (gate hien thi + ap deu ben C++)
            PF_RollAnAttr(nNewIdx);
        end
        local nCurHoles = GetEquipMaxStoneNum(nNewIdx);
        for i = 1, nCurHoles-1 do
            SetStoneLevelOnEquip(nNewIdx, i, 10);
            InlayStarStone(nNewIdx, i, tbHasStone[i]);
        end
        SyncItem(nNewIdx);
        local szItemName = GetItemName(nNewIdx);
        szMsgTips = format("Chóc mõng phi phong ®ét ph¸ thµnh c«ng, nhËn ®­îc 1 Phi phong 0 sao %s ", szItemName);
        --%sÆÕÍ¨Í»ÆÆ£¬ÏûºÄÐÇÓñÊýÁ¿:%d,³É¹¦ÂÊ:%.2f(Ëæ»úÖµ:%.2f),×£¸£Öµ:%d,³É¹¦»ñµÃ0ÐÇ %s
        szLogInfo = format("ProBreakThrough\t%s\t%d\t%.2f\t%.2f\t%d\t%s",
                        szOldItemName, self.nStarCount, self.nSuccessRate, nCurRate, nCurValue, szItemName);
    else
        SetEquipWishValue(self.nMantleIdx, nCurValue+1);
        SyncItem(self.nMantleIdx);
        szMsgTips = "§ét ph¸ thÊt b¹i, ®iÓm chóc phóc phi phong t¨ng 1.";       
        --%sÆÕÍ¨Í»ÆÆ£¬ÏûºÄÐÇÓñÊýÁ¿:%d,³É¹¦ÂÊ:%.2f(Ëæ»úÖµ:%.2f),×£¸£Öµ:%d,Ê§°Ü(ÔËÆø²»¼Ñ£¡£¡£¡)
        szLogInfo = format("ProBreakThrough\t%s\t%d\t%.2f\t%.2f\t%d\tfailed",
                        szOldItemName, self.nStarCount, self.nSuccessRate, nCurRate, nCurValue+1);
    end
    AddStatData("pifeng_level_up");
    Talk(1, "", szMsgTips);
    self:WriteMantleLog(szLogInfo);
end

function tbMantleSystem:CheckAdvanceToWuJi()
    local nResult = self:CheckBreakThrough("AdvBreak", ITEM_TYPE_BLCG);
    if nResult == 0 then
        return nResult;
    end
    -- ¼ÆËã³É¹¦ÂÊ
    self.nSuccessRate = 100.0;
    -- ÏêÏ¸Ãæ°åÐÅÏ¢
    self.szShowTips = format("TØ lÖ n©ng cÊp thµnh c«ng: <color=green>%.2f%%<color>", self.nSuccessRate);
    self.szShowTips = self.szShowTips..format("<enter>Nguyªn liÖu n©ng cÊp: <color=red>B¸ch LuyÖn Thµnh C­¬ng<color> <color=green>%d/%d<color>", 
                                    self.nStarCount, self.nNeedItem);
    self.szShowTips = self.szShowTips.."<enter>B¹c tiªu hao: 1,0000,0000";
    return 1;
end

function tbMantleSystem:ProcessAdvBreakThrough()
    -- ¼ì²éÄÜ²»ÄÜÉý¼¶
    if self:OnCheckLock(1,1,1,1) == 0 then
        return 0;
    end
    -- ¼ì²é½£ÏÀ±Ò
    local nCurMoney = GetCash();
    if nCurMoney < self.nNeedMoney then
        local szErrorTips = format("HiÖn vµng kh«ng ®ñ, cÇn Ýt nhÊt %dW l­îng míi cã thÓ hoµn thµnh.", floor(self.nNeedMoney/10000));
        Talk(1, "", szErrorTips);
        return 0;
    end
    -- ¼ì²é½ø½×²ÄÁÏ
    if self.nStarCount < self.nNeedItem then
        Talk(1, "", "Nguyªn liÖu kh«ng ®ñ, kh«ng thÓ n©ng cÊp V« Cùc.");
        return 0;
    end
    -- ¿ÛÇ®
    Pay(self.nNeedMoney);
    -- ¿Û³ý°ÙÁ¶³É¸Ö
    local tbItemIdx = self.tbAllItemIndex;
    local nSize = getn(tbItemIdx);
    -- [VA 31/08h] chi tru DUNG SO can; phan du cua chong o lai trong hop
    local nConTru = self.nDungSo;
    if nConTru <= 0 then
        nConTru = nSize;
    end
    for i = 1, nSize do
        if tbItemIdx[i] ~= self.nMantleIdx and nConTru > 0 then
            RemoveItemByIndex(tbItemIdx[i]);
            nConTru = nConTru - 1;
        end
    end
    -- ½ø½×ÎÞ¼« 100% ³É¹¦ µÃµ½µÄÊÇ°ó¶¨ÇÒÓÐÈý¸öÔÂÓÐÐ§ÆÚµÄ»Æ½ðÎÞ¼«Åû·ç
    RemoveItemByIndex(self.nMantleIdx);
    local nNewMantleIdx = AddGoldEquipByRow(unpack(tbBreakThrough[self.nMantleKey].tbNewMantle));
    SetItemBindState(nNewMantleIdx, -2);        -- ÓÀ¾Ã°ó¶¨
    ITEM_SetExpiredTime(nNewMantleIdx, 129600); -- ÓÐÐ§ÆÚÈý¸öÔÂ
    SyncItem(nNewMantleIdx);
    Talk(1, "", "N©ng cÊp V« Cùc thµnh c«ng, nhËn ®­îc 1 Hoµng Kim Phi Phong V« Cùc, cã thÓ ®Õn Phã L«i Th­ t¨ng cÊp thµnh HuyÒn Kim V« Cùc.");
    --
    --Åû·ç½ø½×ÎÞ¼«£¬ÏûºÄ°ÙÁ¶³É¸ÖÊýÁ¿:%d£¬½á¹û£º³É¹¦£¬»ñµÃ+0»Æ½ðÎÞ¼«Åû·ç
    self:WriteMantleLog(format("AdvBreakThrough\t%d\tsuccess", self.nStarCount));
end

function tbMantleSystem:CheckSecBreakThrough()
    local nStarLevel = 0;
    local nMantleKey = 0;
    local szMantleName= "";
    local tbItemIdx = self.tbAllItemIndex;
    local nSize = getn(tbItemIdx);
    for i = 1, nSize do
        local eItemType = self:GetItemType(tbItemIdx[i]);
        if eItemType == ITEM_TYPE_BJ_PIFENG then
            nMantleKey = GetPlatinaEquipIndex(tbItemIdx[i]);
            if tbSecBreakThrough[nMantleKey] then
                -- ¼ì²éÊÇ·ñÒÑ¾­·ÅÈëÅû·ç
                if self.nMantleIdx ~= 0 then
                    self.szErrorMsg = tbSecBreakThrough.szErrNumMsg;
                    break;
                end
                -- ÅÐ¶ÏÐÇ¼¶
                nStarLevel = GetPlatinaLevel(tbItemIdx[i]);
                if nStarLevel < 10 then
                    self.szErrorMsg = tbSecBreakThrough.szErrLevelMsg;
                    break;
                end
                -- ¼ì²éÊÇ²»ÊÇÒÑ¾­Âú¼¶
                if tbSecBreakThrough[nMantleKey].tbNewMantle == nil then
                    self.szErrorMsg = tbSecBreakThrough.szErrFinishMsg;
                    break;
                end
                -- ¼ì²éÁ½´Î¶þ´ÎÍ»ÆÆ²Ù×÷ÊÇ·ñ¼ä¸ô15Ìì
                local nCurTime = GetCurrentTime();
                local nPreTime = GetLastBreakTime(tbItemIdx[i]);
                --if nCurTime - nPreTime < 1296000 then -- ¾àÉÏ´ÎÍ»ÆÆÐ¡ÓÚ15Ìì
                --    self.szErrorMsg = tbSecBreakThrough.szErrTypeMsg;
                --    break;
                --end
                -- ¼ì²é¿ª·þÌìÊý£¬´Ó´óÂ½°æÒÆÖ²£¬µ«ÊÇÎÞÓÃ
                --local nOpenDays = self:GetServerOpenDays();
                --if nOpenDays < tbSecBreakThrough[nMantleKey].nMinOpenDays then
                    --self.szErrorMsg = tbStarUpGradeData.szErrTimes;
                    --break;
                --end
                -- ±ê¼ÇÒÑ·ÅÈëÅû·ç
                self.nMantleIdx = tbItemIdx[i];
                self.nMantleKey = nMantleKey;
            else
                self.szErrorMsg = tbSecBreakThrough.szErrItem;
            end
        elseif eItemType == ITEM_TYPE_BLCG then
            self.nStarCount = self.nStarCount + 1; -- [VA 31/08b] GiveBoxCollect khai trien chong: moi muc = 1 don vi (LECH Linux co chu y)
        else
            self.szErrorMsg = tbSecBreakThrough.szErrItem;
            break;
        end
    end
    -- ¼ì²éÊÇ·ñ·ÅÈëÅû·ç
    if self.szErrorMsg == "" and self.nMantleIdx == 0 then
        self.szErrorMsg = tbSecBreakThrough.szErrNoMatleMsg;
    end
    -- ¼ì²éÎïÁÏ¸öÊý
    local nStep = GetEquipMaxStoneNum(self.nMantleIdx) + 1;
    self.nNeedItem = tbSecBreakThrough.tbItemNeed[nStep];
    -- [VA 31/08h] chi doi DU, xem ghi chu o CheckBreakThrough
    if self.szErrorMsg == "" and self.nStarCount < self.nNeedItem then
        self.szErrorMsg = format(tbSecBreakThrough.szErrItemLess, self.nNeedItem, self.nStarCount);
    end
    self.nDungSo = self.nNeedItem;
    -- ÎïÆ·¼ì²é²»Í¨¹ý£¬ÖÕÖ¹¼ì²é
    if self.szErrorMsg ~= "" then
        return 0;
    end
    -- ¼ÆËã³É¹¦ÂÊ
    self.nSuccessRate = 100.0;
    -- ÏêÏ¸Ãæ°åÐÅÏ¢
    self.szShowTips = format("TØ lÖ n©ng cÊp thµnh c«ng: <color=green>%.2f%%<color>", self.nSuccessRate);
    self.szShowTips = self.szShowTips..format("<enter>Nguyªn liÖu n©ng cÊp: <color=red>B¸ch LuyÖn Thµnh C­¬ng<color> <color=green>%d/%d<color>", 
                                    self.nStarCount, self.nNeedItem);
    self.szShowTips = self.szShowTips.."<enter>B¹c tiªu hao: 2000,0000";
    return 1;
end

function tbMantleSystem:ProcessSecBreakThrough()
    -- ¼ì²éÄÜ²»ÄÜÉý¼¶
    if self:OnCheckLock(1,1,1,1) == 0 then
        return 0;
    end
    -- ¼ì²é½£ÏÀ±Ò
    local nCurMoney = GetCash();
    if nCurMoney < self.nNeedMoney then
        local szErrorTips = format("HiÖn vµng kh«ng ®ñ, cÇn Ýt nhÊt %dW l­îng míi cã thÓ hoµn thµnh.", floor(self.nNeedMoney/10000));
        Talk(1, "", szErrorTips);
        return 0;
    end
    -- ¿ÛÇ®
    Pay(self.nNeedMoney);
    -- ¿Û³ý°ÙÁ¶³É¸Ö
    local tbItemIdx = self.tbAllItemIndex;
    local nSize = getn(tbItemIdx);
    -- [VA 31/08h] chi tru DUNG SO can; phan du cua chong o lai trong hop
    local nConTru = self.nDungSo;
    if nConTru <= 0 then
        nConTru = nSize;
    end
    for i = 1, nSize do
        if tbItemIdx[i] ~= self.nMantleIdx and nConTru > 0 then
            RemoveItemByIndex(tbItemIdx[i]);
            nConTru = nConTru - 1;
        end
    end
    -- ÐÂÅû·ç¼Ì³Ð¾ÉÅû·çµÄ°ó¶¨×´Ì¬
    local nBindState = GetItemBindState(self.nMantleIdx);
    local tbHasStone = pack(GetStarStoneOnEquip(self.nMantleIdx, -1));
    RemoveItemByIndex(self.nMantleIdx);
    local nNewMantleIdx = AddPlatinaItem(unpack(tbSecBreakThrough[self.nMantleKey].tbNewMantle));
    for i = 1, 10 do UpgradePlatinaItem(nNewMantleIdx) end
    SetLastBreakTime(nNewMantleIdx, GetCurrentTime());
    --AddStatData("pifeng_level_up");
    local nCurHoles = GetEquipMaxStoneNum(nNewMantleIdx);
    for i = 1, nCurHoles do
        SetStoneLevelOnEquip(nNewMantleIdx, i, 10);
        InlayStarStone(nNewMantleIdx, i, tbHasStone[i]);
    end
    if nBindState == 0 then
        szExecPath = "\\script\\global\\npc\\equip_trader.lua";
        nBindState = DynamicExecute(szExecPath, "calc_new_bindstate", 24);
    end
    -- [VA 31/08c] LuaDynamicExecute moi duong deu return 0 gia tri -> nil;
    -- khong rao thi format %d o dong log no ScriptError SAU khi giao dich
    -- da chay va SetItemBindState(nil) bi bo qua im lang.
    nBindState = nBindState or 0;
    SetItemBindState(nNewMantleIdx, nBindState);
    SyncItem(nNewMantleIdx);
    local szTalkMsg = "§ét ph¸ lÇn 2 thµnh c«ng, nhËn ®­îc 1 c¸i lç kh¶m 10 sao míi";
    Talk(1, "", szTalkMsg);
    --Ðþ½ðÎÞ¼«Åû·ç(Ëø»ê×´Ì¬:%s)¶þ´ÎÍ»ÆÆ£¬ÏûºÄ°ÙÁ¶³É¸Ö%d£¬½á¹û£º³É¹¦£¬¿ªÆôÐþ½ðÎÞ¼«10ÐÇÅû·çÏâ¿×µÚ%d¿×
    local szLogInfo = format("SecBreakThrough\t%d\t%d\tsuccess\t%d", nBindState, self.nNeedItem, nCurHoles); -- [VA 31/08b] szLockState chua tung duoc gan (loi goc Kingsoft)
    self:WriteMantleLog(szLogInfo);
end

function tbMantleSystem:CheckInlayStarStone()
    local bHasModify = 0;
    local tbItemIdx = self.tbAllItemIndex;
    local nSize = getn(tbItemIdx) - 1;
    -- ¼ì²éÊÇ·ñ·ÅÈëÆäËûÎïÆ·
    local bNoItem = 1;
    for i = 1, nSize do
        if tbItemIdx[i] ~= 0 then
            if self:GetItemType(tbItemIdx[i]) ~= ITEM_TYPE_XINGCS then
                self.szErrorMsg = "Kh«ng ®Æt vËt phÈm kh¸c ngoµi Phi Phong, Tinh ThÇn Th¹ch.";
                return 0;
            elseif GetItemStackCount(tbItemIdx[i]) > 1 then
                self.szErrorMsg = "Mçi lç kh¶m chØ ®Æt vµo 1 Tinh ThÇn Th¹ch, sau khi t¸ch thö l¹i.";
                return 0;
            else
                bNoItem = 0;
            end
        end
    end
    if bNoItem == 1 then
        self.szErrorMsg = "H·y ®Æt vµo Tinh ThÇn Th¹ch.";
        return 0;
    end
    -- ¼ì²éÊÇ·ñ·ÅÈëÅû·ç
    -- [PF13 31/08] phi phong nam o phan tu CUOI bang (nSize+1), khong con [6]
    if self:GetItemType(tbItemIdx[nSize + 1]) ~= ITEM_TYPE_HJ_PIFENG and 
        self:GetItemType(tbItemIdx[nSize + 1]) ~= ITEM_TYPE_BJ_PIFENG then
        self.szErrorMsg = "H·y ®Æt vµo Phi Phong cÇn kh¶m.";
        return 0;
    end
    self.nMantleIdx = tbItemIdx[nSize + 1]; -- [PF13 31/08]
    -- ¼ì²éÅû·çÊÇ·ñÄÜ¹»ÏâÇ¶
    local nMaxStoneNum = GetEquipMaxStoneNum(self.nMantleIdx);
    if nMaxStoneNum <= 0 then
        self.szErrorMsg = "Phi Phong kh«ng cã lç kh¶m kh«ng thÓ kh¶m.";
        return 0;
    end
    -- ¼ì²éÅû·ç¿×µÄ¼¶
    -- »ñÈ¡µ±Ç°Åû·çÒÑÏâÇ¶µÄ±¦Ê¯
    local tbHasStone = pack(GetStarStoneOnEquip(self.nMantleIdx, -1));
    -- [CHONLO 31/08] hang rao truoc khi kham theo O:
    -- (1) lo chua mo: engine tu choi LANG LE nhung script cu van an da
    -- (2) lo da co da: ghi de la MAT VINH VIEN vien cu (lech CO Y so voi Linux)
    -- [VA 31/08d] BO hang rao 'lo 0 sao': phi phong MOI TINH (ke ca GM phat)
    -- moi lo deu 0 sao -- cam la cam kham hoan toan ca bac dau chuoi. Ban Linux
    -- cho kham NAM NGU (chua cong gi toi khi dot pha kich hoat lo) -> lam theo
    -- Linux; ProcessInlayStarStone canh bao ro so vien nam ngu.
    for i = 1, nSize do
        if tbItemIdx[i] ~= 0 then
            if i > nMaxStoneNum then
                self.szErrorMsg = format("Phi Phong nµy míi më %d lç kh¶m, lç sè %d ch­a më.", nMaxStoneNum, i);
                return 0;
            end
            if tbHasStone[i] and tbHasStone[i] ~= 0 then
                self.szErrorMsg = format("Lç sè %d ®· cã Tinh ThÇn Th¹ch, h·y chän lç trèng.", i);
                return 0;
            end
        end
    end
    local tbCheckList = {};
    for i = 1, nSize do -- ¹¹½¨ÐÇ³½Ê¯²éÖØ±í
        local nP = tbHasStone[i];
        if nP and nP ~= 0 then
            local nLevel = GetStoneLevelOnEquip(self.nMantleIdx, i);
            if nLevel == 0 then
                self.szErrorMsg = "Lç kh¶m 0 sao kh«ng thÓ kh¶m Tinh ThÇn Th¹ch, sau khi t¨ng sao h·y kh¶m.";
                return 0;
            end
            tbCheckList[nP] = 1;
        end
    end
    -- ¼ì²éÊÇ·ñÖØ¸´ÏâÇ¶
    for i = 1, nSize do
        local nG,nD,nP = GetItemProp(tbItemIdx[i]);
        -- [VA 31/08] Ban Linux dua vao GetItemProp tra nil cho o trong. Ban
        -- JX1 (LuaGetItemProp, KJx2WarInfra.cpp:771-793) LUON day du 6 so nen
        -- o trong cho nP = 0 -- trung dung tbHasStone[i] = 0 cua lo trong ->
        -- bao oan "cung loai khong the thay the". Loc theo chinh o trong.
        if tbItemIdx[i] ~= 0 then
            if nP == tbHasStone[i] then
                self.szErrorMsg = "Tinh ThÇn Th¹ch cïng lo¹i kh«ng thÓ thay thÕ.";
                return 0;
            else
                if tbCheckList[nP] then
                    self.szErrorMsg = "Trong cïng mét Phi Phong kh«ng thÓ kh¶m Tinh ThÇn Th¹ch cïng lo¹i.";
                    return 0;
                else
                    tbCheckList[nP] = 1;
                end
            end
        end
    end
    return 1;
end

-- [PF13 31/08] TAY thuoc tinh an: dat Phi Phong Long Ngam+ (10 sao hoac da co dong an)
-- + 1 Lenh Bai Long Ngam (P=4937) HOAC 2 Dai Bach Luyen + 20000 van luong -> quay lai 2 dong.
-- Cung dung lam duong KICH HOAT cho phi phong 10 sao chua co dong an (vd GM phat san 10 sao).
function tbMantleSystem:CheckTayAn()
    local tbItemIdx = self.tbAllItemIndex;
    local nSize = getn(tbItemIdx);
    self.nDemLB = 0;
    self.nDemDBL = 0;
    for i = 1, nSize do
        local e = self:GetItemType(tbItemIdx[i]);
        if e == ITEM_TYPE_HJ_PIFENG or e == ITEM_TYPE_BJ_PIFENG then
            if self.nMantleIdx ~= 0 then
                self.szErrorMsg = "ChØ ®Æt vµo 1 Phi Phong.";
                return 0;
            end
            self.nMantleIdx = tbItemIdx[i];
        elseif e == ITEM_TYPE_DBLCG then
            self.nDemDBL = self.nDemDBL + 1;
        else
            local nG, nD, nP = GetItemProp(tbItemIdx[i]);
            if nG == 6 and nD == 1 and nP == 4937 then
                self.nDemLB = self.nDemLB + 1;
            else
                self.szErrorMsg = "ChØ ®Æt vµo Phi Phong + LÖnh Bµi Long Ng©m hoÆc §¹i B¸ch LuyÖn Thµnh C­¬ng.";
                return 0;
            end
        end
    end
    if self.nMantleIdx == 0 then
        self.szErrorMsg = "H·y ®Æt vµo Phi Phong Long Ng©m trë lªn.";
        return 0;
    end
    if PF_LaPhiPhongCoAn(self.nMantleIdx) ~= 1 then
        self.szErrorMsg = "ChØ Phi Phong Long Ng©m trë lªn míi cã thuéc tÝnh Èn.";
        return 0;
    end
    if PF_LayKheAn(self.nMantleIdx) == 0 and GetStarLevel(self.nMantleIdx) < 10 then
        self.szErrorMsg = "Phi Phong ph¶i ®¹t 10 sao míi xuÊt hiÖn thuéc tÝnh Èn.";
        return 0;
    end
    if self.nDemLB < 1 and self.nDemDBL < 2 then
        self.szErrorMsg = "CÇn 1 LÖnh Bµi Phi Phong Long Ng©m hoÆc 2 §¹i B¸ch LuyÖn Thµnh C­¬ng.";
        return 0;
    end
    self.nNeedMoney = 200000000; -- 20000 van (VNG); DE CHINH
    if self.nDemLB >= 1 then
        self.nNeedMoney = 0; -- co Lenh Bai thi mien tien
    end
    self.szShowTips = "TÈy sÏ quay l¹i 2 dßng thuéc tÝnh Èn ngÉu nhiªn (1 phßng thñ + 1 tÊn c«ng).";
    return 1;
end

function tbMantleSystem:ProcessTayAn()
    if self:OnCheckLock(1,1,1,1) == 0 then
        return 0;
    end
    if GetCash() < self.nNeedMoney then
        Talk(1, "", format("HiÖn b¹c kh«ng ®ñ, cÇn %dW l­îng.", floor(self.nNeedMoney/10000)));
        return 0;
    end
    Pay(self.nNeedMoney);
    -- tru nguyen lieu: uu tien 1 Lenh Bai, khong co thi 2 Dai Bach Luyen; phan du giu nguyen
    local tbItemIdx = self.tbAllItemIndex;
    local nSize = getn(tbItemIdx);
    local nCanLB = 0;
    local nCanDBL = 0;
    if self.nDemLB >= 1 then
        nCanLB = 1;
    else
        nCanDBL = 2;
    end
    for i = 1, nSize do
        if tbItemIdx[i] ~= self.nMantleIdx and tbItemIdx[i] > 0 then
            local nG, nD, nP = GetItemProp(tbItemIdx[i]);
            if nG == 6 and nD == 1 and nP == 4937 and nCanLB > 0 then
                RemoveItemByIndex(tbItemIdx[i]);
                nCanLB = nCanLB - 1;
            elseif nG == 6 and nD == 1 and nP == 4933 and nCanDBL > 0 then
                RemoveItemByIndex(tbItemIdx[i]);
                nCanDBL = nCanDBL - 1;
            end
        end
    end
    PF_RollAnAttr(self.nMantleIdx);
    self:WriteMantleLog(format("TayAnAttr\t%s\t%d", GetItemName(self.nMantleIdx), self.nDemLB));
    return 1;
end

function tbMantleSystem:ProcessInlayStarStone()
    -- ¼ì²éÄÜ²»ÄÜÉý¼¶
    if self:OnCheckLock(1,1,0,1) == 0 then
        g_ReturnInlayResult(0);
        return 0;
    end
    local tbItemIdx = self.tbAllItemIndex;
    local nSize = getn(tbItemIdx) - 1;
    local nMantleIdx = tbItemIdx[nSize + 1]; -- [PF13 31/08]
    local nCount = 0;
    local nTruot = 0;
    local nNgu = 0;   -- vien roi vao lo 0 sao (nam ngu toi khi dot pha kich hoat)
    local szLogInfo = format("InlayStarStone\t%d\t%s", GetStarLevel(nMantleIdx), GetItemName(nMantleIdx));
    for i = 1, nSize do
        local nStoneIdx = tbItemIdx[i];
        if nStoneIdx > 0 then
            local nG,nD,nP = GetItemProp(nStoneIdx);
            szLogInfo = szLogInfo..format("\t%s\t%d", GetItemName(nStoneIdx), i);
            -- ÏâÇ¶²Ù×÷
            if GetStoneLevelOnEquip(nMantleIdx, i) < 1 then
                nNgu = nNgu + 1;
            end
            if InlayStarStone(nMantleIdx, i, nP) == 1 then
            -- É¾³ýÐÇ³½Ê¯
            RemoveItemByIndex(nStoneIdx);

            AddStatData("pifeng_stargem_in");
            nCount = nCount + 1;
            else
                nTruot = nTruot + 1;
            end
        end
    end
    SyncItem(nMantleIdx);
    -- [VA 31/08b] chi bao thanh cong khi MOI vien deu duoc engine nhan
    -- (InlayStarStone tra 0 = tu choi lang le; truoc day van an da + bao ok).
    if nTruot > 0 then
        g_ReturnInlayResult(0);
        Talk(1, "", format("Cã %d viªn ®¸ kh«ng kh¶m ®­îc (lç kh«ng hîp lÖ), ®¸ vÉn n»m trong hép.", nTruot));
    else
        g_ReturnInlayResult(1);
        if nNgu > 0 then
            Talk(1, "", format("Kh¶m Phi Phong thµnh c«ng. <color=yellow>%d viªn n»m ë lç 0 sao, sÏ ph¸t huy sau khi ®ét ph¸ kÝch ho¹t lç.<color>", nNgu));
        else
            Talk(1, "", "Kh¶m Phi Phong thµnh c«ng.");
        end
    end
    self:WriteMantleLog(szLogInfo);
    return 1;
end

function PlayerLogin()
    if GetTask(3550) < 1 then --ÐÇ³½Ê¯ÏµÍ³Ö¸ÒýÈÎÎñÉÏÏß¹«¸æ
        Msg2Player("<color=yellow>§Õn chç Thî RÌn ë 7 thµnh chÝnh nép 1 phi phong ®· t¨ng sao ®Ó nhËn kinh nghiÖm phong phó!<color>")
    end
end

if login_add then login_add(PlayerLogin, 2) end

-- ===========================================================================
-- [PORT 29/08] SHIM cho 2 ham ban Linux co ma JX1 khong co.
-- Hop GiveItemUI cua JX1 (S2C_GIVE_BOX) chi mang 3 o chuoi: tieu de, noi dung,
-- ten callback -- khong con cho cho chuoi huong dan gui SAU khi hop da mo.
-- Nen day chu do ra Msg2Player: van dung noi dung, chi khac cho hien thi.
-- Day la LECH DUY NHAT so voi ban Linux o phan giao dien tang sao / dot pha.
-- ===========================================================================
-- [VA 31/08b] loc trung: 13 diem goi ban lai cung mot cau moi lan cham hop.
-- Chi Msg2Player khi noi dung DOI so voi lan truoc (theo tung nguoi choi).
tbPF_MsgCu = tbPF_MsgCu or {};
tbPF_MsgCu2 = tbPF_MsgCu2 or {};

function SetUiGiveItemMsg(szMsg)
	if szMsg and szMsg ~= "" then
		local nKhoa = PlayerIndex or 0;
		if tbPF_MsgCu[nKhoa] ~= szMsg then
			tbPF_MsgCu[nKhoa] = szMsg;
			Msg2Player("[Phi Phong] "..szMsg)
		end
	end
end

function SetUiGiveItemMoreConfirmMsg(szMsg)
	if szMsg and szMsg ~= "" then
		local nKhoa = PlayerIndex or 0;
		if tbPF_MsgCu2[nKhoa] ~= szMsg then
			tbPF_MsgCu2[nKhoa] = szMsg;
			Msg2Player("[Phi Phong] "..szMsg)
		end
	end
end
