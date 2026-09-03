MAILDEF                     = {}
MAILDEF.DEBUG               = 0
MAILDEF.CACHE_DURATION      = 60*60*24*30 -- 缓存时间30天
MAILDEF.CACHE_DURATION_SEVEN      = 60*60*24*7 -- 缓存时间7天
MAILDEF.EXPIRED_DURATION    = 60*60*24*30 -- 过期时间30天
MAILDEF.EXPIRED_DURATION_SEVEN    = 60*60*24*7 -- 过期时间7天
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
    WriteLog(szLog)
    if MAILDEF.DEBUG == 1 then
        print(szLog)
    end
end

function MAILDEF:Debug(...)
    if MAILDEF.DEBUG == 1 then
        print("[MAIL]", unpack(arg or {"nil"}))
    end
end

-- [MAIL 03/09 JX1] shim cho client JX1 (2.0 lay tu common.lua / C++)
if (tblen == nil) then
    function tblen(tb)
        local n = 0
        if type(tb) == "table" then
            for _, _ in tb do
                n = n + 1
            end
        end
        return n
    end
end
if (dump2str == nil) then
    function dump2str(v)
        if type(v) == "table" then
            local s = "{"
            for k, x in v do
                s = s..tostring(k).."="..dump2str(x)..","
            end
            return s.."}"
        end
        return tostring(v)
    end
end
if (WriteLog == nil) then
    function WriteLog(szLog)
        if (GhiLog) then
            GhiLog("MAIL", szLog)
        end
    end
end
