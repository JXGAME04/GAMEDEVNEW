-- auctionpoll.lua - [DAUGIA 04/09] quet dau gia moi 30 giay (giam gia Ha Lan, ket thuc kieu Anh, het han ky gui).
-- CHI tep nay dang ky timer (khuon mailpoll.lua). KHONG Include tep nay o noi khac.
Include("\\script\\auction_house\\auction_manager.lua")
AUCPOLL_FRAMES = 30 * 18
AUCPOLL_GLB    = 9002   -- GlbValue: chi mot state dang ky timer
function AucPoll_Tick(nParam, nTimerId)
    if AUC_Ready() == 1 then
        AUC_Tick()
    end
    return AUCPOLL_FRAMES
end
if GetGlbValue(AUCPOLL_GLB) ~= 1 then
    SetGlbValue(AUCPOLL_GLB, 1)
    AddTimer(AUCPOLL_FRAMES, "AucPoll_Tick", 0)
end
