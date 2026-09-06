-- mailpoll.lua - [MAIL 03/09] do thu moi do web admin ghi thang bang `mail` (state 0) moi 30 giay,
-- quet thu het han moi 10 phut. CHI tep nay dang ky timer (AddTimer chay trong state cua tep dang ky,
-- KJx2League.cpp: ham tra so khung thi timer tiep tuc). KHONG Include tep nay o noi khac.
Include("\\script\\mail\\mailmanager.lua")

MAILPOLL_FRAMES = 30 * 18
MAILPOLL_LASTID = 0
MAILPOLL_TICKS  = 0
MAILPOLL_GLB    = 9001   -- GlbValue: chi mot state dang ky timer

function MailPoll_Tick(nParam, nTimerId)
    if MAILPOLL_LASTID == 0 then
        -- lan dau: chi bao thu ghi SAU khi may chu chay; thu cu giao luc nguoi choi dang nhap
        MAILPOLL_LASTID = MailDB_MaxId() or 0
    end
    local tb = MailDB_PollNew(MAILPOLL_LASTID, 50)
    for i = 1, getn(tb) do
        local r = tb[i]
        if r.id > MAILPOLL_LASTID then
            MAILPOLL_LASTID = r.id
        end
        MailManager_NotifyNew(r.role, r.id)
    end
    MAILPOLL_TICKS = MAILPOLL_TICKS + 1
    if MAILPOLL_TICKS >= 20 then
        MAILPOLL_TICKS = 0
        MailDB_Sweep()
    end
    return MAILPOLL_FRAMES
end

if GetGlbValue(MAILPOLL_GLB) ~= 1 then
    SetGlbValue(MAILPOLL_GLB, 1)
    AddTimer(MAILPOLL_FRAMES, "MailPoll_Tick", 0)
end
