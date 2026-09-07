--========================================
-- BXH EVENT THEO TASK - LUA 3.0 VLTK
-- Dang mang, SAVE / LOAD, khong mat sau bao tri
--========================================

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_task.lua")

EVENT_RANK_TASK_SCORE = TASK_HD_EVENTNEW   -- task diem event
EVENT_RANK_MAX        = 10

EVENT_RANK_SAVE_FILE  = "script/event/data/event_rank_save.lua"
EVENT_SAVE_INTERVAL   = 300   -- 5 phut

EVENT_RANK_LIST       = EVENT_RANK_LIST or {}   -- mang: [i][1]=name, [i][2]=score
EVENT_LAST_SAVE_TIME  = EVENT_LAST_SAVE_TIME or 0

function Event_ArrayLen(tb)
    return getn(tb)
end

function Event_ArrayAdd(tb, value)
    tb[getn(tb) + 1] = value
end

function Event_ArrayPop(tb)
    if getn(tb) > 0 then
        tb[getn(tb)] = nil
    end
end

function Event_SortRank()
    local i, j

    for i = 1, getn(EVENT_RANK_LIST) - 1 do
    
        if type(EVENT_RANK_LIST[i]) ~= "table"
           or EVENT_RANK_LIST[i][1] == nil
           or EVENT_RANK_LIST[i][2] == nil then
            EVENT_RANK_LIST[i] = nil
        else
            for j = i + 1, getn(EVENT_RANK_LIST) do
                if type(EVENT_RANK_LIST[j]) == "table"
                   and EVENT_RANK_LIST[j][2] ~= nil
                   and EVENT_RANK_LIST[i][2] ~= nil
                   and EVENT_RANK_LIST[j][2] > EVENT_RANK_LIST[i][2] then
                    local t = EVENT_RANK_LIST[i]
                    EVENT_RANK_LIST[i] = EVENT_RANK_LIST[j]
                    EVENT_RANK_LIST[j] = t
                end
            end
        end
    end
end


function Event_CutRank()
    while getn(EVENT_RANK_LIST) > EVENT_RANK_MAX do
        EVENT_RANK_LIST[getn(EVENT_RANK_LIST)] = nil
    end
end

function Event_UpdateRank(szName, nScore)
    if not szName or szName == "" then return end
    if not nScore or nScore <= 0 then return end

    local i
    for i = 1, getn(EVENT_RANK_LIST) do
        if EVENT_RANK_LIST[i][1] == szName then
            EVENT_RANK_LIST[i][2] = nScore
            Event_SortRank()
            Event_CutRank()
            return
        end
    end

    -- neu chua co trong BXH -> them moi
    local row = {}
    row[1] = szName
    row[2] = nScore
    Event_ArrayAdd(EVENT_RANK_LIST, row)

    Event_SortRank()
    Event_CutRank()
end

function Event_AddScore(nAdd)
    if not nAdd or nAdd <= 0 then return end

    local nOld = GetTask(EVENT_RANK_TASK_SCORE)
    if not nOld then nOld = 0 end

    local nNew = nOld + nAdd
    SetTask(EVENT_RANK_TASK_SCORE, nNew)

    Event_UpdateRank(GetName(), nNew)
    Event_TrySaveRank()
end

function Event_OnLogin()
    local nScore = GetTask(EVENT_RANK_TASK_SCORE)
    if nScore and nScore > 0 then
        Event_UpdateRank(GetName(), nScore)
    end
end

function Event_ShowRank()
    local szMsg = "<color=yellow>B∂ng X’p Hπng Event 31 - 15<color>\n\n"

    if getn(EVENT_RANK_LIST) <= 0 then
        Msg2Player(szMsg .. "Ch≠a c„ d˜ li÷u.")
        return
    end

  Msg2Player("<color=yellow>B∂ng X’p Hπng Event 31 - 15<color>")

    local MAX_LEN = 180
    local szMsg = ""

    local i
    for i = 1, getn(EVENT_RANK_LIST) do
        local szTopColor   = "<color=red>"
        local szNameColor  = "<color=green>"
        local szPointColor = "<color=pink>"

        if i == 1 then
            szTopColor   = "<color=red>"
            szNameColor  = "<color=green>"
            szPointColor = "<color=pink>"
        elseif i == 2 then
            szTopColor   = "<color=red>"
            szNameColor  = "<color=green>"
            szPointColor = "<color=pink>"
        elseif i == 3 then
            szTopColor   = "<color=red>"
            szNameColor  = "<color=green>"
            szPointColor = "<color=pink>"
        end

        local szLine =
            szTopColor .. i .. ". <color>" ..
            szNameColor .. EVENT_RANK_LIST[i][1] .. "<color>" ..
            " - " ..
            szPointColor .. EVENT_RANK_LIST[i][2] .. " Æi”m<color>\n"

        if strlen(szMsg) + strlen(szLine) > MAX_LEN then
            Msg2Player(szMsg)
            szMsg = ""
        end

        szMsg = szMsg .. szLine
    end

    if strlen(szMsg) > 0 then
        Msg2Player(szMsg)
    end
end

function Event_SaveRank()
    local f = openfile(EVENT_RANK_SAVE_FILE, "w")
    if not f then return end

    write(f, "EVENT_RANK_LIST = {\n")

    local i
    for i = 1, getn(EVENT_RANK_LIST) do
        write(f,
            " { \"" ..
            EVENT_RANK_LIST[i][1] .. "\", " ..
            EVENT_RANK_LIST[i][2] ..
            " },\n"
        )
    end

    write(f, "}\n")
    closefile(f)
end

function Event_LoadRank()
    local f = openfile(EVENT_RANK_SAVE_FILE, "r")
    if f then
        closefile(f)
        dofile(EVENT_RANK_SAVE_FILE)
    end

    if type(EVENT_RANK_LIST) ~= "table" then
        EVENT_RANK_LIST = {}
        return
    end

    
    local i = 1
    while i <= getn(EVENT_RANK_LIST) do
        if type(EVENT_RANK_LIST[i]) ~= "table"
           or EVENT_RANK_LIST[i][1] == nil
           or EVENT_RANK_LIST[i][2] == nil then
            EVENT_RANK_LIST[i] = EVENT_RANK_LIST[getn(EVENT_RANK_LIST)]
            EVENT_RANK_LIST[getn(EVENT_RANK_LIST)] = nil
        else
            i = i + 1
        end
    end

    Event_SortRank()
    Event_CutRank()
end


function Event_TrySaveRank()
    local now = GetTimeNow()
    if now - EVENT_LAST_SAVE_TIME < EVENT_SAVE_INTERVAL then
        return
    end

    EVENT_LAST_SAVE_TIME = now
    Event_SaveRank()
end

function Event_SaveRankManual()
	Event_RebuildFromOnline()
    Event_SaveRank()
    -- Msg2Player("Da luu BXH Event.")
end

function Event_RebuildFromOnline()
    local oldIndex = PlayerIndex
    local i, n

    n = GetCountPlayerMax()
    for i = 1, n do
        PlayerIndex = i

        if GetName() ~= "" then
            local score = GetTask(EVENT_RANK_TASK_SCORE)
            if score and score > 0 then
                Event_UpdateRank(GetName(), score)
            end
        end
    end

    PlayerIndex = oldIndex
end

Event_LoadRank()
-- print(">>> EVENT RANK SYSTEM LOADED <<<")
