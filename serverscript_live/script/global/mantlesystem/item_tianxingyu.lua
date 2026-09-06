--ÌìÐÇÓñËéÆ¬µÀ¾ß½Å±¾
Include("\\script\\activitysys\\playerfunlib.lua")

-- µÀ¾ßÈë¿Úº¯Êý
function main(nItemIndex)
    --Check cell
    if PlayerFunLib:CheckFreeBagCell(1,"") ~= 1 then
        Msg2Player("Tói ®· ®Çy, kh«ng thÓ ghÐp.")
        return 1
    end

    -- Èç¹ûÉíÉÏÓÐÊ®¸öÌìÐÇÓñËéÆ¬ÔòºÏ³ÉÒ»¸öÌìÐÇÓñ
    if CalcItemCount(3, 6, 1, 4883, -1) >= 10 then
        if ConsumeItem(3, 10, 6, 1, 4883, -1) == 1 then
            local tbItem = {szName = "Thiªn Tinh Ngäc", tbProp = {6,1,4882,1,0,0}, nBindState = -2,};
            PlayerFunLib:GetItem(tbItem, 1, "Dïng 10 M¶nh Thiªn Tinh Ngäc ghÐp Thiªn Tinh Ngäc");
        else
            Msg2Player("GhÐp Thiªn Tinh Ngäc thÊt b¹i, h·y thö l¹i.");
        end
    else
        Msg2Player("M¶nh Thiªn Tinh Ngäc kh«ng ®ñ, kh«ng thÓ ghÐp.");
    end

    return 1;
end
