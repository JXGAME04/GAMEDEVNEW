if MODEL_GAMECLIENT ~= 1 then
	return
end

Include("\\script\\lib\\objbuffer_head.lua")

Def = 
{
    {
        "emSCRIPT_PROTOCOL_CLIENT_CMD",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:LuaExecute",
        {OBJTYPE_STRING}
    },
    {
        "emSCRIPT_PROTOCOL_STORES_REQUEST_ITEM",
        "\\script\\item\\dynamic_shop\\logic_c.lua",
        "DynamicShop:RecvItem",
        {OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_TABLE}
    },
    {
        "emSCRIPT_PROTOCOL_BATTLE",
        "\\script\\missions\\battle\\protocol_c.lua",
        "show_battle_select",
        {OBJTYPE_TABLE}
    },
    {
        "emSCRIPT_PROTOCOL_QIANCHONGLOU",
        "\\script\\missions\\qianchonglou\\ui.lua",
        "process_protocol",
        {OBJTYPE_NUMBER, OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_FANDAOMIJING",
        "\\script\\missions\\boss\\ui.lua",
        "process_gs_protocol",
        {OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCAL_SIGNIN_SYNC_DATE",
        "\\script\\ui\\signinsystem_c.lua",
        "ProcessSignInSyncDate",
        {OBJTYPE_TABLE},
    },
    {
        "emSCRIPT_PROTOCAL_SIGNIN_SYNC_AWARD",
        "\\script\\ui\\signinsystem_c.lua",
        "ProcessSignInSyncAward",
        {OBJTYPE_TABLE},
    },
    {
        "emSCRIPT_PROTOCOL_COMPOSE_OPENUI",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:OpenComposeUi",
        {OBJTYPE_TABLE},
    },
    {
        "emSCRIPT_PROTOCAL_PROCESS_CHECK",
        "\\script\\global\\process_monitor\\process_monitor_c.lua",
        "CheckSuspiciousProcess",
        {OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_MANTLE_OPENUI",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:OpenMantleInlayUi",
        {OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_MANTLE_INLAYRESULT",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:OnMantleInlayCallBack",
        {OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_TONGHYD_RETURNP1",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:TongHYDUiDataP1",
        {OBJTYPE_TABLE}
    },
    {
        "emSCRIPT_PROTOCOL_TONGHYD_RETURNP2",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:TongHYDUiDataP2",
        {OBJTYPE_TABLE}
    },
    {
        "emSCRIPT_PROTOCOL_OPENUI_SHIQIZHI",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:SetSQZVisiable",
        {OBJTYPE_TABLE}
    },
    {
        "emSCRIPT_PROTOCOL_UPDATE_SHIQIZHI",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:UpdateShiQiZhiUI",
        {OBJTYPE_TABLE}
    },

    -- 此处仅作测试用，使用客户端和服务端配置文件的话，传个任务id索引，客户端做逻辑就可以
    -- 此处由服务端计算逻辑传个客户端，做测试使用
    {
        "emSCRIPT_PROTOCOL_TOGGLE_TOWERTASKBTN",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:TaskTowerToggleBtn",
        {OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_TOGGLE_TOWERTASK",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:TaskTowerToggle",
        {OBJTYPE_NUMBER}
    },

    {
        "emSCRIPT_PROTOCOL_TOWERTASK_SETACCEPT",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:TaskTowerSetAccept",
        {OBJTYPE_STRING,OBJTYPE_STRING,OBJTYPE_STRING,OBJTYPE_NUMBER,OBJTYPE_NUMBER}
    },

    {
        "emSCRIPT_PROTOCOL_TOWERTASK_SETTASK",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:TaskTowerSetTask",
        {OBJTYPE_STRING,OBJTYPE_STRING}
    },

    {
        "emSCRIPT_PROTOCOL_TOWERTASK_SETTIMER",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:TaskTowerSetTimer",
        {OBJTYPE_STRING,OBJTYPE_NUMBER}
    },

    {
        "emSCRIPT_PROTOCOL_TOWERTASK_ADDTARGET",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:TaskTowerAddTarget",
        {OBJTYPE_STRING,OBJTYPE_NUMBER}
    },

    {
        "emSCRIPT_PROTOCOL_TOWERTASK_ADDTARGETCOUNT",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:TaskTowerAddTargetCount",
        {OBJTYPE_NUMBER}
    },

    {
        "emSCRIPT_PROTOCOL_HTLISTClOSE",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:HTListClose",
        {OBJTYPE_NUMBER}
    },

    {
        "emSCRIPT_PROTOCOL_HTLISTOPEN",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:HTListOpen",
        {OBJTYPE_TABLE}
    },
    {
        "emSCRIPT_PROTOCOL_HTLISTOLDSETDATA",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:HTListOldSetData",
        {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_HTLISTOLDdELDATA",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:HTListOldDelData",
        {OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_HTLISTOLDDELDATAS",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:HTListOldDelDatas",
        {OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_HTLISTNOWSETDATA",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:HTListNowSetData",
        {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_HTLISTNOWSETMINEDATA",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:HTListNowSetMyData",
        {OBJTYPE_NUMBER, OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_HTLISTNOWDELDATA",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:HTListNowDelData",
        {OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_HTLISTNOWDELDATAS",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:HTListNowDelDatas",
        {OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_OPENWNDFLIPS",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:OpenWndFlips",
        {OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_CLOSEWNDFLIPS",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:CloseWndFlips",
        {OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_WNDFLIPSPLAYSINGLE",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:WndFlipsPlaySingle",
        {OBJTYPE_NUMBER}
    },

    -- 剑冢迷宫
    {
        "emSCRIPT_PROTOCOL_TOGGLE_SWORDTASK",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:TaskSwordToggle",
        {OBJTYPE_NUMBER}
    },

    {
        "emSCRIPT_PROTOCOL_SWORDTASK_SETTASK",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:SetSwordTaskInfo",
        {OBJTYPE_STRING,OBJTYPE_STRING}
    },

    {
        "emSCRIPT_PROTOCOL_SWORDTASK_SETTIMER",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:SetSwordTaskTimer",
        {OBJTYPE_STRING,OBJTYPE_NUMBER}
    },

    {
        "emSCRIPT_PROTOCOL_SWORDTASK_ADDTARGET",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:AddSwordTaskTarget",
        {OBJTYPE_STRING,OBJTYPE_NUMBER}
    },

    {
        "emSCRIPT_PROTOCOL_SWORDTASK_ADDTARGETCOUNT",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:AddSwordTaskTargetCount",
        {OBJTYPE_NUMBER,OBJTYPE_NUMBER}
    },

    {
        "emSCRIPT_PROTOCOL_SWORDTASK_SETTARGETCOUNT",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:SetSwordTaskTargetCount",
        {OBJTYPE_NUMBER,OBJTYPE_NUMBER}
    },
	-- 锻造
	{
        "emSCRIPT_PROTOCOL_DUANZAO_OPENUI",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:OpenDuanZaoUi",
        {OBJTYPE_TABLE,OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCAL_GOLDBOSS_DMG_STAT",
        "\\script\\ui\\goldbossreport.lua",
        "ProcessGoldBossDmgStat",
        {OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_TABLE, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_STRING},
    },
    {
        "emSCRIPT_PROTOCAL_NEW_SEVEN_CITY_REPORT",
        "\\script\\ui\\new_seven_city_report.lua",
        "ProcessNewSevenCityReport",
        {OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_TABLE, OBJTYPE_TABLE, OBJTYPE_STRING},
    },
    {
        "emSCRIPT_PROTOCOL_RELOAD_CLIENT_SCRIPT",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:ReloadClientScript",
        {OBJTYPE_STRING}
    },
    {
        "emSCRIPT_PROTOCOL_RELOAD_GOLDBOSS_SCHEME",
        "\\script\\ui\\goldbossreport.lua",
        "ReloadScheme",
        {OBJTYPE_STRING}
    },
    {
        "emSCRIPT_PROTOCOL_MAIL_HEADERLIST",
        "\\script\\ui\\uimail.lua",
        "UIMail:HeaderListArrival",
        {OBJTYPE_NUMBER, OBJTYPE_TABLE, OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_MAIL_NEWMAIL",
        "\\script\\ui\\uimail.lua",
        "UIMail:NewMailEventArrival",
        {OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_MAIL_STATECHANGE",
        "\\script\\ui\\uimail.lua",
        "UIMail:StateChangeEventArrival",
        {OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_MAIL_DELETE",
        "\\script\\ui\\uimail.lua",
        "UIMail:DeleteEventArrival",
        {OBJTYPE_NUMBER, OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_MAIL_WHOLEMAIL",
        "\\script\\ui\\uimail.lua",
        "UIMail:WholeMailArrival",
        {OBJTYPE_TABLE},
    },
    {
        "emSCRIPT_PROTOCOL_MAIL_OPENWINDOW",
        "\\script\\ui\\uimail.lua",
        "UIMail:OpenMailWindow",
        {OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_AUCTION_OFFERPRICERET",
        "\\script\\ui\\uiauction_house.lua",
        "UIAuctionHouse:OnOfferPriceResponse",
        {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_AUCTION_REFUNDRET",
        "\\script\\ui\\uiauction_house.lua",
        "UIAuctionHouse:OnRefundResponse",
        {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_AUCTION_ACTIVITYLIST",
        "\\script\\ui\\uiauction_house.lua",
        "UIAuctionHouse:OnActivityListResponse",
        {OBJTYPE_NUMBER, OBJTYPE_TABLE},
    },
    {
        "emSCRIPT_PROTOCOL_AUCTION_ACTIVITYINFO",
        "\\script\\ui\\uiauction_house.lua",
        "UIAuctionHouse:OnActivityContentResponse",
        {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_TABLE, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_AUCTION_ITEMINFO",
        "\\script\\ui\\uiauction_house.lua",
        "UIAuctionHouse:OnItemContentResponse",
        {OBJTYPE_TABLE},
    },
    {
        "emSCRIPT_PROTOCOL_AUCTION_MEMBERLIST",
        "\\script\\ui\\uiauction_house.lua",
        "UIAuctionHouse:OnMemberListResponse",
        {OBJTYPE_NUMBER, OBJTYPE_TABLE},
    },
    {
        "emSCRIPT_PROTOCOL_AUCTION_NEWACTIVITY",
        "\\script\\ui\\uiauction_house.lua",
        "UIAuctionHouse:OnNewActivityEvent",
        {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_AUCTION_ENDACTIVITY",
        "\\script\\ui\\uiauction_house.lua",
        "UIAuctionHouse:OnEndActivityEvent",
        {OBJTYPE_NUMBER, OBJTYPE_STRING},
    },
    {
        "emSCRIPT_PROTOCOL_AUCTION_NEWITEM",
        "\\script\\ui\\uiauction_house.lua",
        "UIAuctionHouse:OnNewItemEvent",
        {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_AUCTION_ENDITEM",
        "\\script\\ui\\uiauction_house.lua",
        "UIAuctionHouse:OnEndItemEvent",
        {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_AUCTION_TPRICECHANGE",
        "\\script\\ui\\uiauction_house.lua",
        "UIAuctionHouse:OnPriceChangeEvent",
        {OBJTYPE_NUMBER, OBJTYPE_STRING, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
    },
    -- 签到活动
    {
        "emSCRIPT_PROTOCOL_SIGN_ACT_UPDATE_INFO",
        "\\script\\ui\\sign_act.lua",
        "UpdateClientInfo",
        {OBJTYPE_TABLE}
    },
    {
        "emSCRIPT_PROTOCOL_SIGN_ACT_OPEN_WINDOW",
        "\\script\\ui\\sign_act.lua",
        "OpenSignWindow",
        {OBJTYPE_NUMBER}
    },
    -- 签到活动
    -- 五行大阵
    {
        "emSCRIPT_PROTOCOL_FIVESERIESZONE_BEGIN",
        "\\script\\ui\\uifive_series_zone.lua",
        "UIFiveSeriesZone:ShowSignIcon",
        {OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_FIVESERIESZONE_END",
        "\\script\\ui\\uifive_series_zone.lua",
        "UIFiveSeriesZone:HideSignIcon",
        {OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_LINGLONGTOWER_SHOW_HELPER",
        "\\script\\ui\\uiling_long_tower.lua",
        "UILingLongTower:ShowLingLongTowerHelper",
        {OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_LINGLONGTOWER_HIDE_HELPER",
        "\\script\\ui\\uiling_long_tower.lua",
        "UILingLongTower:HideLingLongTowerHelper",
        {OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_LINGLONGTOWER_UPDATE_HELPER",
        "\\script\\ui\\uiling_long_tower.lua",
        "UILingLongTower:UpdateLingLongTowerHelper",
        {OBJTYPE_NUMBER,OBJTYPE_NUMBER,OBJTYPE_NUMBER,OBJTYPE_NUMBER,OBJTYPE_NUMBER}
    },
    -- 五行大阵
    -- 2021新商店
    {
        "emSCRIPT_PROTOCOL_SHOP_2021_SEND_SERVER_INFO",
        "\\script\\ui\\shop_2021.lua",
        "UpdateServerInfo",
        {OBJTYPE_TABLE}
    },
    {
        "emSCRIPT_PROTOCOL_SHOW_SHOP_2021_ICON",
        "\\script\\ui\\shop_2021_icon.lua",
        "ShowShopIcon",
        {OBJTYPE_NUMBER} 
    },
    -- 2021新商店
    -- 帮会联赛
    {
        "emSCRIPT_PROTOCOL_TONG_BATTLE_UPDATE_INFO",
        "\\script\\ui\\tong_battle_2023\\remind_wnd.lua",
        "TB_UpdateWndInfo",
        {OBJTYPE_TABLE}
    },
    {
        "emSCRIPT_PROTOCOL_TONG_BATTLE_CLOSE_WND",
        "\\script\\ui\\tong_battle_2023\\remind_wnd.lua",
        "TB_CloseWndInfo",
        {OBJTYPE_NUMBER}
    },
    -- 帮会联赛
    -- -- 坐骑界面
    -- {
    --     "emSCRIPT_PROTOCOL_OPEN_HORSE_OUT_LOOK_WND",
    --     "\\script\\ui\\horse\\base_manage.lua",
    --     "OpenHorseManageWnd",
    --     {OBJTYPE_NUMBER} 
    -- },
    -- -- 坐骑界面
    -- 坐骑界面
    {
        "emSCRIPT_PROTOCOL_OPEN_HORSE_OUT_LOOK_WND",
        "\\script\\ui\\horse\\base_manage.lua",
        "OpenHorseManageWnd",
        {OBJTYPE_NUMBER} 
    },
    -- 坐骑界面
    -- 2023周年庆
    {
        "emSCRIPT_PROTOCOL_SHOW_2023_ONLINE_AWARD_ICON",
        "\\script\\ui\\act_award_icon.lua",
        "ShowAwardIcon",
        {OBJTYPE_NUMBER} 
    },
    {
        "emSCRIPT_PROTOCOL_HIDE_2023_ONLINE_AWARD_ICON",
        "\\script\\ui\\act_award_icon.lua",
        "HideAwardIcon",
        {OBJTYPE_NUMBER} 
    },
    -- 2023周年庆
    -- BOSS血条
    {
        "emSCRIPT_PROTOCOL_BOSS_HP_BAR_OPEN_WND",
        "\\script\\ui\\boss_hp_bar.lua",
        "BHB_OpenWnd",
        {OBJTYPE_NUMBER, OBJTYPE_STRING}
    },
    -- BOSS血条
    -- 2026战令
    {
        "emSCRIPT_PROTOCOL_SEASON_TICKET_SHOW_ICON",
        "\\script\\ui\\season_ticket_2026_icon.lua",
        "ShowShopIcon",
        {OBJTYPE_NUMBER}
    },
    {
        "emSCRIPT_PROTOCOL_SEASON_TICKET_UPDATE_WND",
        "\\script\\ui\\season_ticket_2026.lua",
        "SeasonTicketWndUpdate",
        {OBJTYPE_NUMBER}
    },
    -- 2026战令
    {
        "emSCRIPT_PROTOCOL_TIANGANGFU_OPENUI",
        "\\script\\ui\\uitiangangfu_operators.lua",
        "UITiangangfu_Operators:OpenUI",
        {OBJTYPE_NUMBER},
    },
    
    {
        "emSCRIPT_PROTOCOL_TIANGANGFU_SELECTAGAIN",
        "\\script\\ui\\uitiangangfu_operators.lua",
        "UITiangangfu_Operators:OnOperatorSelected",
        {OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_MUTIPLEFACTION_OPEN_PAGE",
        "\\script\\ui\\uimutiple_faction.lua",
        "UIMutipleFaction:OpenMutipleFactionPage",
        {OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_NORMALAWARD_REPONSE_AWARD",
        "\\script\\ui\\uiaward.lua",
        "UIAward:AwardArrival",
        {OBJTYPE_NUMBER, OBJTYPE_TABLE},
    },
    {
        "emSCRIPT_PROTOCOL_NORMALAWARD_REPONSE_AWARDSTATE",
        "\\script\\ui\\uiaward.lua",
        "UIAward:AwardStateArrival",
        {OBJTYPE_NUMBER, OBJTYPE_NUMBER, OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_NORMALAWARD_REPONSE_RECEIVE_AWARD",
        "\\script\\ui\\uiaward.lua",
        "UIAward:AwardReceiveResultArrival",
        {OBJTYPE_NUMBER, OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_TWINKLE_SWEEPSTAKE_BOX",
        "\\script\\ui\\sweepstake.lua",
        "TwinkleSweepsTakeBox",
        {OBJTYPE_NUMBER},
    },
    {
        "emSCRIPT_PROTOCOL_CLOSE_PROGRESSBAR",
        "\\script\\ui\\uirecruit.lua",
        "UIRecruit:CloseProgressBar",
        {OBJTYPE_NUMBER},
    },
    {
        "emScript_PROTOCOL_S2C_GREATTEAMFIGHT_SYNC",
        "\\script\\missions\\greatteamfight\\client\\recvsync.lua",
        "RecvSync:Recv",
        {OBJTYPE_TABLE}
    },
    {
        "emScript_PROTOCOL_S2C_OPEN_SCRIPT_WND",
        "\\script\\ui\\manage.lua",
        "UiManage:OpenWindow",
        {OBJTYPE_STRING}
    },
    {
        "emScript_PROTOCOL_S2C_HIGHBLUE",
        "\\script\\ui\\highblue\\forge.lua",
        "s2c_highblue",
        {OBJTYPE_STRING, OBJTYPE_NUMBER}
    },
    {
        "emScript_PROTOCOL_S2C_FREEDOMRANK",
        "\\script\\global\\freedomrank\\freedomrank_c.lua",
        "FreedomRankProtocolProcessor:Process",
        {OBJTYPE_TABLE}
    },
    {
        "emSCRIPT_PROTOCOL_TITLE_SEND_TITLE_INFO",
        "\\script\\ui\\outlook\\base_manage.lua",
        "RecieveTitleInfo",
        {OBJTYPE_TABLE}  
    },
    {
        "emSCRIPT_PROTOCOL_OPEN_OUT_LOOK_MANAGE_WND",
        "\\script\\ui\\outlook\\base_manage.lua",
        "OpenOutlookWnd",
        {OBJTYPE_NUMBER}
    },
    {
        "emScript_PROTOCOL_S2C_REQ_INFO",
        "\\script\\lib\\clientcmd.lua",
        "ClientCmd:ReqModule",
        {OBJTYPE_TABLE}
    },
    {
        "emScript_PROTOCOL_S2C_SERIESSTAMP",
        "\\script\\ui\\series_stamp.lua",
        "ProcessSeriesStamp",
        {OBJTYPE_NUMBER, OBJTYPE_NUMBER}
    },
    {
        "emScript_PROTOCOL_S2C_NEWKILLER",
        "\\script\\ui\\newkiller\\main.lua",
        "NewKillerProtocolProcessor:Process",
        {OBJTYPE_TABLE}
    },
    {
        "emScript_PROTOCOL_S2C_WORLDLINE",
        "\\script\\global\\worldline\\worldline.lua",
        "WorldLine:RecvSyncLevel",
        {OBJTYPE_TABLE}
    },
    {
        "emScript_PROTOCOL_S2C_SEVENDAYMISSION",
        "\\script\\ui\\uisevendaymission.lua",
        "S2CCommond",
        {OBJTYPE_NUMBER, OBJTYPE_TABLE}
    },
    {
        "emScript_PROTOCOL_S2C_TALK_TWOOPT",
        "\\script\\ui\\talk_twoopt.lua",
        "OpenTalkTwoOpt",
        {OBJTYPE_TABLE}
    },
    {
        "emScript_PROTOCOL_S2C_CARD_ACT_BTN",
        "\\script\\ui\\card_act_btn.lua",
        "UpdateCardActBtn",
        {OBJTYPE_NUMBER}
    },

    {
         "emScript_PROTOCOL_S2C_ONLINE_ACT_BTN",
        "\\script\\ui\\online_reward_btn.lua",
        "UpdateOnlineActBtn",
        {OBJTYPE_NUMBER}
    },
    {
        "emScript_PROTOCOL_S2C_RETURN_FRIEND_ICON",
        "\\script\\ui\\return_friend_icon.lua",
        "UpdateReturnFriendIcon",
        {OBJTYPE_NUMBER}
    },
}

