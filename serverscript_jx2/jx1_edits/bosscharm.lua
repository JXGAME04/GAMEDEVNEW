Include("\\script\\global\\forbidmap.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\lib\\lib_server.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_map.lua")

NOW_END_SAY 				= "KÕt thóc ®èi tho¹i./no"

function main(nItemIdx)
    dofile("script/item/bosscharm.lua")
    
    local nLevel = 10
    local ForbiddenMap = {44, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 326, 327, 328, 329, 330, 331, 334, 335, 337, 338, 339, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374}
    local GoldenBoss = {
        {562, "§¹o Thanh Ch©n Nh©n", 4, 12},
        {563, "Gia LuËt TÞ Ly", 3, 12},
        {564, "§oµn Dù", 3, 12}, -- edit by phong kieu
        {565, "§oan Méc DuÖ ", 3, 12},
        {566, "Cæ B¸ch", 0, 12},
        {567, "Chung Linh Tó ", 2, 12},
        {568, "Hµ Linh Phiªu", 2, 12},
        {582, "Lam Y Y", 1, 12},
        {583, "M¹nh Th­¬ng L­¬ng", 3, 12},
        {739, "V­¬ng T¸ ", 0, 10},
        {740, "HuyÒn Gi¸c §¹i S­ ", 0, 12},
        {741, "§­êng BÊt NhiÔm", 1, 12},
        {742, "B¹ch Doanh Doanh", 1, 12},
        {743, "Thanh HiÓu S­ Th¸i ", 2, 12},
        {744, "Yªn HiÓu Tr¸i", 2, 12},
        {745, "Hµ Nh©n Ng· ", 3, 12},
        {746, "B¾c KiÒu Phong", 4, 10}, -- edit by phong kieu
        {747, "TuyÒn C¬ Tö ", 4, 12},
        {511, "Tr­¬ng T«ng ChÝnh", 4, 270},
        {513, "DiÖu Nh­ ", 2, 270},
        {523, "LiÔu Thanh Thanh", 1, 270}
    }

    local bosspro = {}
    local wb, xb, yb = GetWorldPos()
    local nMapId = wb;	-- 15/08: bien W khong ton tai o ban port

    if (GetFightState() == 0) then
        Msg2Player("§ang ë trong tr¹ng th¸i phi chiÕn ®Êu kh«ng thÓ sö dông.")
        return 1
    end

    -- 15/08: ban goc cho dung Lenh bai trong DAI MAP BANG HOI 586..604
    -- (D:\ServerLinux\server1\script\itemosscharm.lua:4-16). Ban port bi
    -- viet lai thanh 'chi dung o Hoa Son' (wb ~= 2) nen trong lanh dia bang
    -- LUON bi tu choi. Ban goc dung SubWorldIdx2MapCopy - JX1 khong co ham do,
    -- dung thang map id tu GetWorldPos.
    if (wb < 586 or wb > 604) then
        Msg2Player("Kh«ng thÓ sõ dông vËt phÉm nµy t¹i ®©y.")
        return 1
    end


    local selectedElement = random(0, 4)

    for i = 1, getn(GoldenBoss) do
        bosspro[i] = GoldenBoss[i][4]
    end

    local doubleodds = 160 - (nLevel - 1) * 10
    local j = 1
    if (random(1, doubleodds) == 1) then
        j = 2
    end

    for k = 1, j do
        local numth = randomaward(bosspro)
        local GoldenBossId = GoldenBoss[numth][1]
        local GoldenBossName = GoldenBoss[numth][2]

        local bosstieu = AddNpc(GoldenBossId, 100, SubWorldID2Idx(wb), xb * 32, yb * 32, selectedElement)
        if (bosstieu > 0) then
            SetNpcScript(bosstieu, "\\script\\tinhnang\\boss_hoangkim\\bosslbdeath.lua")
            SetNpcDropScript(bosstieu, "\\script\\tinhnang\\boss_hoangkim\\bosslbdrop.lua")
            SetNpcTimer(bosstieu, 60 * 60 * 18)
            SetNpcName(bosstieu, GoldenBossName)
        end

        AddOwnExp(500000)
        -- logHoatDong("[Account:]"..GetAccount()..", ["..GetName().."] Dïng lÖnh bµi gäi Boss Hoµng Kim: "..GoldenBossName.. "\t\n")
        Msg2Player("B¹n gäi ra 1 Boss LÖnh Bµi: "..GoldenBossName)
    end

    RemoveItem(nItemIdx)
    return 0
end
function randomaward(aryProbability)
    local nRandNum
    local nSum = 0
    local nArgCount = getn(aryProbability)
    local nCompareSum = 0
    for i = 1, nArgCount do
        nSum = nSum + aryProbability[i]
    end
    nRandNum = random(1, nSum)
    for i = 1, nArgCount do
        nCompareSum = nCompareSum + aryProbability[i]
        if (nRandNum <= nCompareSum) then
            return i
        end
    end
end

function no()
end
