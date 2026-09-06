--ÐÇÓñÔ­Ê¯µÀ¾ß½Å±¾

Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\lib\\awardtemplet.lua")

local tbAwardList =
{  
    { szName = "Tinh Ngäc",   tbProp = {6, 1, 4881, 1, 0, 0}, nCount = 15, nRate = 40.0},
    { szName = "Tinh Ngäc",   tbProp = {6, 1, 4881, 1, 0, 0}, nCount = 20, nRate = 30.0},
    { szName = "Tinh Ngäc",   tbProp = {6, 1, 4881, 1, 0, 0}, nCount = 50, nRate = 10.5},
    { szName = "Tinh Ngäc",   tbProp = {6, 1, 4881, 1, 0, 0}, nCount = 70, nRate = 7.5},
    { szName = "Tinh Ngäc",   tbProp = {6, 1, 4881, 1, 0, 0}, nCount = 90, nRate = 5.0},
    { szName = "M¶nh Thiªn Tinh Ngäc", tbProp = {6, 1, 4883, 1, 0, 0}, nCount = 1, nRate = 5.0},
    { szName = "Tinh ThÇn Kho¸ng", tbProp = {6, 1, 4887, 1, 0, 0}, nCount = 1, nRate = 2.0},
    --{ szName = "°ÙÁ¶³É¸Ö",   tbProp = {6, 1, 4884, 1, 0, 0}, nCount = 1, nRate = 0.1},
}

-- µÀ¾ßÈë¿Úº¯Êý
function main(nItemIndex)
    --Check cell
    if PlayerFunLib:CheckFreeBagCell(1,"Tói kh«ng ®ñ chç, ph¶i cã Ýt nhÊt 1 « trèng.") ~= 1 then
        return 1;
    end

    if CalcItemCount(3, 6, 1, 4886, -1) <= 0 then
        Msg2Player("Kh«ng cã Tinh Háa Than, kh«ng thÓ më Tinh Ngäc Nguyªn Th¹ch.")
	return 1;
    end

    
    if ConsumeItem(3, 1, 6, 1, 4886, -1) == 1 then
        -- ¸øÎïÆ·
        tbAwardTemplet:GiveAwardByList(tbAwardList, "[Më Tinh Ngäc Nguyªn Th¹ch] nhËn th­ëng")
        AddStatData("pifeng_starstone_open");
    else
        Msg2Player("KhÊu trõ Tinh Háa Than thÊt b¹i, h·y thö l¹i")
	return 1
    end
end
