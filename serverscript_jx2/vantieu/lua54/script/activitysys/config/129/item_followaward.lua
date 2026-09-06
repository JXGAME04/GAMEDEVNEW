-- ÁúÃÅïÚ¾Ö£¨ÔöÒæµÀ¾ß°ó¶¨½Å±¾£©
Include("\\script\\activitysys\\config\\129\\head.lua");
Include("\\script\\activitysys\\config\\129\\config.lua");
Include("\\script\\activitysys\\config\\129\\variables.lua")

Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\activitysys\\playerfunlib.lua")

local tbAwardList = {
    [4978] = {
        { szName = "Ch©n Nguyªn §¬n (®¹i)",  tbProp = {6,1,4847,1,0,0}, nRate = 10.0, nCount =  4, nBindState = -2,},
        { szName = "Ch©n Nguyªn §¬n (®¹i)",  tbProp = {6,1,4847,1,0,0}, nRate =  1.0, nCount = 20, nBindState = -2,},
        { szName = "Hé M¹ch §¬n",   tbProp = {6,1,4844,1,0,0}, nRate = 10.0, nCount =  20, nBindState = -2,},
        { szName = "Hé M¹ch §¬n",   tbProp = {6,1,4844,1,0,0}, nRate =  1.0, nCount = 100, nBindState = -2,},
        { nExp_tl = 1, nCount =  20000000, nRate = 10.0, },
        { nExp_tl = 1, nCount =  50000000, nRate =  5.0, },
        { nExp_tl = 1, nCount =  100000000, nRate =  1.0, },
        { szName = "Håi thiªn t¸i t¹o lÔ bao",  tbProp = {6,1,2536,1,0,0}, nRate =  8.0, nCount = 1, nBindState = -2, nExpiredTime = 10080,},
        { szName = "Håi thiªn t¸i t¹o lÔ bao",  tbProp = {6,1,2536,1,0,0}, nRate =  4.0, nCount = 2, nBindState = -2, nExpiredTime = 10080,},
        { szName = "Håi thiªn t¸i t¹o lÔ bao",  tbProp = {6,1,2536,1,0,0}, nRate =  1.0, nCount = 3, nBindState = -2, nExpiredTime = 10080,},
        { szName = "HuyÕt Long §»ng CÊp 9",  tbProp = {6,1,4848,9,0,0}, nRate = 5.0, nCount = 5, nBindState = -2,},
        { szName = "Tö Ngäc",  tbProp = {6,1,2400,1,0,0}, nRate =  3.0, nCount = 1, nBindState = -2,},
        { szName = "B¹ch Ngäc",  tbProp = {6,1,2399,1,0,0}, nRate =  5.0, nCount = 1, nBindState = -2,},
        { szName = "Tói D­îc PhÈm",  tbProp = {6,1,4813,1,0,0}, nRate = 15.0, nCount = 5, nBindState = -2, nExpiredTime = 10080,},
        { szName = "TËt C«ng Trî Lùc Hoµn",  tbProp = {6,1,3570,1,0,0}, nRate = 10.0, nCount = 2, nBindState = -2, nExpiredTime = 10080,},
        { szName = "Tinh ThiÕt Kho¸ng",  tbProp = {6,1,4428,1,0,0}, nRate =  1.0, nCount = 1, nBindState = -2,},
        { szName = "¢m D­¬ng Ho¹t HuyÕt §¬n",  tbProp = {6,1,3571,1,0,0}, nRate = 10.0, nCount = 1, nBindState = -2, nExpiredTime = 10080,},
    },
    [4979] = {
        { szName = "Ch©n Nguyªn §¬n (®¹i)",  tbProp = {6,1,4847,1,0,0}, nRate = 10.0, nCount = 8, nBindState = -2,},
        { szName = "Ch©n Nguyªn §¬n (®¹i)",  tbProp = {6,1,4847,1,0,0}, nRate = 1.0, nCount = 40, nBindState = -2,},
        { szName = "Hé M¹ch §¬n",    tbProp = {6,1,4844,1,0,0}, nRate = 10.0, nCount = 40, nBindState = -2,},
        { szName = "Hé M¹ch §¬n",    tbProp = {6,1,4844,1,0,0}, nRate = 1.0, nCount = 200, nBindState = -2,},
        { nExp_tl = 1, nCount =  50000000, nRate = 10.0, },
        { nExp_tl = 1, nCount =  100000000, nRate =  4.0, },
        { nExp_tl = 1, nCount =  150000000, nRate =  1.0, },
        { szName = "Håi thiªn t¸i t¹o lÔ bao",  tbProp = {6,1,2536,1,0,0}, nRate = 10.0, nCount =  2, nBindState = -2, nExpiredTime = 10080,},
        { szName = "Håi thiªn t¸i t¹o lÔ bao",  tbProp = {6,1,2536,1,0,0}, nRate = 5.0, nCount =  3, nBindState = -2, nExpiredTime = 10080,},
        { szName = "Håi thiªn t¸i t¹o lÔ bao",  tbProp = {6,1,2536,1,0,0}, nRate = 1.0, nCount =  4, nBindState = -2, nExpiredTime = 10080,},
        { szName = "§»ng Long Th¹ch - H¹",  tbProp = {6,1,4866,1,0,0}, nRate =  2.0, nCount =  5, nBindState = -2,},
        { szName = "Tinh ThiÕt Kho¸ng",  tbProp = {6,1,4428,1,0,0}, nRate =  1.0, nCount = 1, nBindState = -2,},
        { szName = "LÖnh ñy Th¸c Cao CÊp",  tbProp = {6,1,4773,1,0,0}, nRate =  5.0, nCount =  3, nBindState = -2,},
        { szName = "HuyÕt Long §»ng CÊp 11",  tbProp = {6,1,4848,11,0,0}, nRate =  13.0, nCount = 5, nBindState = -2,},
        { szName = "HuyÕt Long §»ng CÊp 12",  tbProp = {6,1,4848,12,0,0}, nRate =  5.0, nCount = 5, nBindState = -2,},
        { szName = "Tói D­îc PhÈm",  tbProp = {6,1,4813,1,0,0}, nRate = 20.0, nCount = 5, nBindState = -2, nExpiredTime = 10080,},
        { szName = "Phông NguyÖt Qu¶ Dung", tbProp = {6,1,126,1,0,0}, nRate =  1.0, nCount = 1, nBindState = -2,},
    },
}

function main(nItemIndex)
    local _, _, nP =  GetItemProp(nItemIndex);
    local tbAward = tbAwardList[nP];
    if not tbAward then
        return nil;
    end
    local tbMinCells = {
        [4978] = 5,
        [4979] = 5,
    }
    -- ¼ì²é±³°ü¿Õ¼ä
    local nMincCells = tbMinCells[nP];
    local szTips = format("Tói kh«ng ®ñ chç, ®¶m b¶o cã %d « trèng h·y ®Õn.", nMincCells);
    if PlayerFunLib:CheckFreeBagCell(nMincCells, szTips) ~= 1 then
        return 1;
    end
    local szLogs = format("[Long M«n Tiªu Côc_¸p Tiªu Bang] Dïng ®¹o cô hé tiªu (%s) nhËn th­ëng", GetItemName(nItemIndex));
    tbAwardTemplet:GiveAwardByList(tbAward, szLogs);
    return nil;
end

