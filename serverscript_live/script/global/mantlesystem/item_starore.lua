--–«≥ΩøÛµ¿æﬂΩ≈±æ

Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\lib\\awardtemplet.lua")

local tbAwardList =
{  
    { szName = "Ph∏c TË Tinh Th«n Thπch", tbProp = {9, 1, 1, 1, 0, 0}, nRate = 1.0, nBindState = -2},
    { szName = "Anh DÚng Tinh Th«n Thπch", tbProp = {9, 1, 2, 1, 0, 0}, nRate = 1.0, nBindState = -2},
    { szName = "C≠Íng C´ng Tinh Th«n Thπch", tbProp = {9, 1, 3, 1, 0, 0}, nRate = 1.0, nBindState = -2},
    { szName = "Tinh Chu»n Tinh Th«n Thπch", tbProp = {9, 1, 4, 1, 0, 0}, nRate = 1.0, nBindState = -2},
    { szName = "C≠¨ng Ngπnh Tinh Th«n Thπch", tbProp = {9, 1, 5, 1, 0, 0}, nRate = 1.0, nBindState = -2},
    { szName = "Xuy™n Th›ch Tinh Th«n Thπch", tbProp = {9, 1, 6, 1, 0, 0}, nRate = 1.0, nBindState = -2},
    { szName = "Ch› MÀt Tinh Th«n Thπch", tbProp = {9, 1, 7, 1, 0, 0}, nRate = 1.0, nBindState = -2},
    { szName = "Ph∏ To∏i Tinh Th«n Thπch", tbProp = {9, 1, 8, 1, 0, 0}, nRate = 2.0, nBindState = -2},
    { szName = "Vi™n NhuÀn Tinh Th«n Thπch", tbProp = {9, 1, 9, 1, 0, 0}, nRate = 1.0, nBindState = -2},
    { szName = "Ki™n C≠Íng Tinh Th«n Thπch", tbProp = {9, 1, 10, 1, 0, 0}, nRate = 1.0, nBindState = -2},
    { szName = "ßoπn Li÷t Tinh Th«n Thπch", tbProp = {9, 1, 11, 1, 0, 0}, nRate = 1.0, nBindState = -2},
    { szName = "Ên CË Tinh Th«n Thπch", tbProp = {9, 1, 12, 1, 0, 0}, nRate = 1.0, nBindState = -2},
    { szName = "Thi”m Quang Tinh Th«n Thπch", tbProp = {9, 1, 13, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "PhÛc Quang Tinh Th«n Thπch", tbProp = {9, 1, 14, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "Thi”m Di÷u Tinh Th«n Thπch", tbProp = {9, 1, 15, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "Hoa L÷ Tinh Th«n Thπch", tbProp = {9, 1, 16, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "Ch› M÷nh Tinh Th«n Thπch", tbProp = {9, 1, 17, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "Qu˚ B› Tinh Th«n Thπch", tbProp = {9, 1, 18, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "Ti™n L≠Óng Tinh Th«n Thπch", tbProp = {9, 1, 19, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "B®ng Hµn Tinh Th«n Thπch", tbProp = {9, 1, 20, 1, 0, 0}, nRate = 3.0, nBindState = -2},
    { szName = "T n Ti÷p Tinh Th«n Thπch", tbProp = {9, 1, 21, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "Ki™n Nghﬁ Tinh Th«n Thπch", tbProp = {9, 1, 22, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "Hoa Th∏i Tinh Th«n Thπch", tbProp = {9, 1, 23, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "S∏n Lπn Tinh Th«n Thπch", tbProp = {9, 1, 24, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "Ki™n Nh…n Tinh Th«n Thπch", tbProp = {9, 1, 25, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "Cao N®ng Tinh Th«n Thπch", tbProp = {9, 1, 26, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "Khinh Doanh Tinh Th«n Thπch", tbProp = {9, 1, 27, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "PhÙc T´ Tinh Th«n Thπch", tbProp = {9, 1, 28, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "Thu«n Tﬁnh Tinh Th«n Thπch", tbProp = {9, 1, 29, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "LÁ M∑ng Tinh Th«n Thπch", tbProp = {9, 1, 30, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "CuÂng Nhi÷t Tinh Th«n Thπch", tbProp = {9, 1, 31, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "Th u Minh Tinh Th«n Thπch", tbProp = {9, 1, 32, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "C¨ M…n Tinh Th«n Thπch", tbProp = {9, 1, 33, 1, 0, 0}, nRate = 4.0, nBindState = -2},
    { szName = "Ph∂n Quang Tinh Th«n Thπch", tbProp = {9, 1, 34, 1, 0, 0}, nRate = 4.0, nBindState = -2},
}

-- µ¿æﬂ»Îø⁄∫Ø ˝
function main(nItemIndex)
    --Check cell
    if PlayerFunLib:CheckFreeBagCell(1,"TÛi kh´ng ÆÒ chÁ, ph∂i c„ ›t nh t 1 ´ trËng.") ~= 1 then
        return 1;
    end

    -- ∏¯ŒÔ∆∑
    tbAwardTemplet:GiveAwardByList(tbAwardList, "[MÎ Tinh Th«n Kho∏ng] nhÀn th≠Îng")
    AddStatData("pifeng_starore_open");
end
