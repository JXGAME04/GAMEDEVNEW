MAILDEF                     = {}
MAILDEF.DEBUG               = 0
MAILDEF.CACHE_DURATION      = 60*60*24*30 -- 缓存时间30天
MAILDEF.EXPIRED_DURATION    = 60*60*24*30 -- 过期时间30天
MAILDEF.PLAYER_MAX_MAIL     = 100         -- 玩家最多存100封
MAILDEF.REMOTE_SCRIPT       = "\\script\\mail\\MailManager.lua" -- 远程脚本路径

-- 邮件状态定义
MAILDEF.tbState = 
{
    UNRECEIVE   = 0,    -- 未收取
    UNREAD      = 1,    -- 未读
    READED      = 2,    -- 已读
    DRAWED      = 3,    -- 已领取
    DELETEED    = 4,    -- 已删除
}

-- 邮件删除原因定义
MAILDEF.tbDeleteReson =
{
    REQUEST     = 0,
    EXPIRED     = 1,
    OVERFLOW    = 2,
}

function MAILDEF:WriteLog(szLog)
    szLog = format("[MailManager] %s", szLog)
    -- WriteLog(szLog)
    if MAILDEF.DEBUG == 1 then
        print(szLog)
    end
end

function MAILDEF:Debug(...)
    if MAILDEF.DEBUG == 1 then
        print("[MAIL]", unpack(arg or {"nil"}))
    end
end