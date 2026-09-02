-- 注意：各个门派的定义，这份是lua的，C++的定义在\settings\faction_settings.ini
tbFacDef = {}
tbFacDef.nMaxFac = 13
tbFacDef.tbIds = {
    SHAOLIN     = 0,
    TIANWANG    = 1,
    TANGMEN     = 2,
    WUDU        = 3,
    EMEI        = 4,
    CUIYAN      = 5,
    GAIBANG     = 6,
    TIANREN     = 7,
    WUDANG      = 8,
    KUNLUN      = 9,
    HUASHAN     = 10,
    WUHUN       = 11,
    XIAOYAO     = 12,
}

tbFacDef.tbSchools = {
	[1] = "Thiếu Lâm quyền pháp",
	[2] = "Thiếu Lâm côn pháp",
	[3] = "Thiếu Lâm đao pháp",
	[4] = "Thiên Vương chùy pháp",
	[5] = "Thiên Vương thương pháp",
	[6] = "Thiên Vương đao pháp",
	[7] = "Nga Mi kiếm pháp",
	[8] = "Nga Mi chưởng pháp",
	[9] = "Thúy Yên đao pháp",
	[10] = "Thúy Yên song đao",
	[11] = "Ngũ Độc chưởng pháp",
	[12] = "Ngũ Độc đao pháp",
	[13] = "Đường Môn Phi Đao",
	[14] = "Đường Môn nỏ tiễn",
	[15] = "Đường Môn phi tiêu",
	[16] = "Cái Bang chưởng pháp",
	[17] = "Cái Bang côn pháp",
	[18] = "Thiên Nhẫn mâu pháp",
	[19] = "Thiên Nhẫn đao pháp",
	[20] = "Võ Đang quyền pháp",
	[21] = "Võ Đang kiếm pháp",
	[22] = "Côn Lôn đao pháp",
	[23] = "Côn Lôn kiếm pháp",
	[24] = "Hoa Sơn khí tông",
	[25] = "Hoa Sơn kiếm tông",
	[26] = "Võ Hồn Thuẫn Pháp",
	[27] = "Võ Hồn Đao Pháp",
    [28] = "Tiêu Dao Kiếm Pháp",
    [29] = "Tiêu Dao Cầm Pháp",
}

tbFacDef.tbWholeInfoList =
{
--      [tbFacDef.tbIds.SHAOLIN] = {-- 
--          门派名, 门派ID, 阵营,   头衔ID, 属性
--          任务ID, 
--          137的变量,  137变量的值,        // 就这一项来说，搞不明白是做什么的，入门的时候设置
--          技能Tab,
--          入门公告,
--      },
    [tbFacDef.tbIds.SHAOLIN] = {-- 少林
        szFacName       = "shaolin",
        szFacChName     = "Thiếu Lâm phái",
        szFacShortName  = "Thiếu Lâm",
        szFacShortEng   = "sl",
        nFacNumb        = tbFacDef.tbIds.SHAOLIN,
        nCamp           = 1,
        nRankId         = 72,
        nSeries         = 0,
        nTaskId_Fact    = 7,
        nTaskId_90Skill = 122,
        nTaskId_137     = 137,
        nValue_137      = 67,
        n150TaskStep    = 6,
        nJinjieSkillID  = 1220,
        nFactionMapId   = 103,
        tbFactionMapPos = {1845,2983},
        szMsg           = "Kể từ hôm nay %s gia nhập môn phái Thiếu Lâm, sau này nhờ các huynh chỉ giáo!",
        tbSkillID       =
        {
            [10]    = {
                14,     -- 行龙不雨[14]
                10,     -- 金刚伏魔[10]（入门）
            },
            [20]    = {
                8,      -- 少林拳法[8]
                4,      -- 少林棍法[4]（10级）
                6,      -- 少林刀法[6]（10级）
            },
            [30]    = {
                15,     -- 不动明王[15]（20级）
            },
            [40]    = {
                16,     -- 罗汉阵[16]（30级）
            },
            [50]    = {
                20,     -- 狮子吼 [20]（40级）
            },
            [60]    = {
                271,    -- 龙爪虎抓[271]
                11,     -- 横扫六合 [11]（50级）
                19,     -- 摩诃无量 [19]（50级）
            },
            [70]    = {
                273,    -- 如来千叶[273]
                21,     -- 易筋经[21]（60级）
            },
            [90]    = {
                {318, 1, "Quyền pháp", "Đạt Ma Độ Giang"},   -- 达摩渡江[318]            添加熟练度技能 至少添加为1级
                {319, 1, "Côn pháp", "Hoành Tảo Thiên Quân"},   -- 横扫千军[319]（90级）
                {321, 1, "Đao pháp", "Vô Tướng Trảm "},     -- 无相斩  [321]（90级）
            },
            [120]   = {
                {709, 1, "Thông dụng", "Đại Thừa Như Lai Chú"}
            },
            [150]   = {
                {1055, 1, "Quyền pháp", "Đại Lực Kim Cang Chưởng"},  -- 大力金刚掌[318]          添加熟练度技能 至少添加为1级
                {1056, 1, "Côn pháp", "Vi Đà Hiến Xử"},    -- 韦陀献杵[319]（90级）
                {1057, 1, "Đao pháp", "Tam Giới Quy Thiền"},    -- 叁戒归禅[321]（90级）
            },
        },
        tbItemInfo      =
        {
            tbSkillBook90 = {
                {"Thiếu Lâm Quyền Pháp. Quyển 1",56},
                {"Thiếu Lâm Côn pháp. Quyển 2",57},
                {"Thiếu Lâm Đao pháp. Quyển 3",58}
            },
        },
    },
    
    [tbFacDef.tbIds.TIANWANG] = {-- 天王
        szFacName       = "tianwang",
        szFacChName     = "Thiên Vương Bang",
        szFacShortName  = "Thiên Vương",
        szFacShortEng   = "tw",
        nFacNumb        = tbFacDef.tbIds.TIANWANG,
        nCamp           = 3,
        nRankId         = 69,
        nSeries         = 0,
        nTaskId_Fact    = 3,
        nTaskId_90Skill = 121,
        nTaskId_137     = 137,
        nValue_137      = 63,
        n150TaskStep    = 4,
        nJinjieSkillID  = 1221,
        nFactionMapId   = 59,
        tbFactionMapPos = {1552,3188},
        szMsg           = "Kể từ hôm nay %s gia nhập Thiên Vương, nhờ các sư huynh chỉ giáo!",
        tbSkillID       =
        {
            [10]    = {
                34,     -- 惊雷斩[34]  （入门）
                30,     -- 回风落雁[30]（入门）
                29,     -- 斩龙诀[29]  （入门）
            },
            [20]    = {
                26,     -- 天王锤法[26]（10级）
                23,     -- 天王枪法[23]（10级）
                24,     -- 天王刀法[24]（10级）
            },
            [30]    = {
                33,     -- 静心诀[33]  （20级）
            },
            [40]    = {
                37,     -- 泼风斩  [37]（30级）
                35,     -- 阳关三叠[35]（30级）
                31,     -- 行云诀  [31]（30级）
            },
            [50]    = {
                40,     -- 断魂刺  [40]（40级）
            },
            [60]    = {
                42,     -- 金钟罩  [42]（50级）
            },
            [70]    = {
                32,     -- 无心斩  [32] （60级）
                36,     -- 天王战意[36]（镇派）
                41,     -- 血战八方[41] （60级）
                324,    -- 乘龙诀  [324]（60级）
            },
            [90]    = {
                {325, 1, "Chùy pháp", "Truy Phong Quyết "},     -- 追风诀  （90级）
                {323, 1, "Thương pháp", "Truy Tinh Trục Nguyệt "},   -- 追星逐月（90级）
                {322, 1, "Đao pháp", "Phá Thiên Trảm "},     -- 破天斩  （90级）
            },
            [120]   = {
                {708, 1, "Thông dụng", "Đảo Hư Thiên"}    --
            },
            [150]   = {
                {1059, 1, "Chùy pháp", "Tung Hoành Bát Hoang"},  -- 纵横八荒
                {1060, 1, "Thương pháp", "Bá Vương Tạm Kim"},  -- 霸王錾金
                {1058, 1, "Đao pháp", "Hào Hùng Trảm"},    -- 豪雄斩
            },
        },
        tbItemInfo      =
        {
            tbSkillBook90 = {
                {"Thiên Vương Chùy Pháp. Quyển 1",37},
                {"Thiên Vương Thương pháp. Quyển 2",38},
                {"Thiên Vương Đao pháp.Quyển 3",39}
            },
        },
    },
    
    [tbFacDef.tbIds.TANGMEN] = {-- 唐门
        szFacName       = "tangmen",
        szFacChName     = "Đường Môn",
        szFacShortName  = "Đường Môn",
        szFacShortEng   = "tm",
        nFacNumb        = tbFacDef.tbIds.TANGMEN,
        nCamp           = 3,
        nRankId         = 76,
        nSeries         = 1,
        nTaskId_Fact    = 2,
        nTaskId_90Skill = 123,
        nTaskId_137     = 137,
        nValue_137      = 62,
        n150TaskStep    = 7,
        nJinjieSkillID  = 1223,
        nFactionMapId   = 25,
        tbFactionMapPos = {3982,5235},
        szMsg           = "Kể từ hôm nay, %s gia nhập Đường môn, cảm phiền các sư huynh sư tỷ chỉ giáo!",
        tbSkillID       =
        {
            [10]    = {
                45,     -- 霹雳弹[45 ]（入门）
            },
            [20]    = {
                43,     -- 唐门暗器[43 ]（10级）
                347,    -- 地焰火[347]（10级）
            },
            [30]    = {
                303,    -- 毒刺骨  [303]（20级）
            },
            [40]    = {
                50,     -- 追心箭  [50 ]（30级）
                54,     -- 漫天花雨[54 ]（30级）
                47,     -- 夺魂镖  [47 ]（30级）
                343,    -- 穿心刺  [343]（30级）
            },
            [50]    = {
                345,    -- 寒冰刺  [345]（40级）
            },
            [60]    = {
                349,    -- 雷击术[349]（50级）
            },
            [70]    = {
                249,    -- 小李飞刀[249]（60级）
                48,     -- 心眼  [48 ]（镇派）
                58,     -- 天罗地网[58 ]（60级）
                341,    -- 散花镖  [341]（60级）
            },
            [90]    = {
                {339, 1, "Phi đao", "Nhiếp Hồn Nguyệt ảnh"},   -- 摄魂月影[339]（90级）
                {302, 1, "Nỏ tiễn", "Bạo Vũ Lê Hoa"},   -- 暴雨梨花[302]（90级）
                {342, 1, "Phi Tiêu", "Cửu Cung Phi Tinh"},   -- 九宫飞星[342]（90级）
                {351, 0, "Thông dụng", "Loạn Hoàn Kích"},     -- 乱环击  [351]（90级）
            },
            [120]   = {
                {710, 1, "Thông dụng", "Mê ảnh Tung"}
            },
            [150]   = {
                {1069, 1, "Phi đao", "Vô ảnh Xuyên"},     -- 无影穿
                {1070, 1, "Nỏ tiễn", "Thiết Liên Tứ Sát"},    -- 铁莲四杀
                {1071, 1, "Phi Tiêu", "Càn Khôn Nhất Trịch"},    -- 乾坤一掷
                {1110, 0, "Thông dụng", "Tích Lịch Loạn Hoàn Kích"},  -- 霹雳乱环击
            },
        },
        tbItemInfo      =
        {
            tbSkillBook90 = {
                {"Phi Đao thuật. Nhiếp Hồn Nguyệt ảnh",45},
                {"Tụ Tiễn thuật. Bạo Vũ Lê Hoa",27},
                {"Phi Tiêu thuật. Cửu Cung Phi Tinh",46},
                {"Hãm Tĩnh thuật.Loạn Hoàn Kích",28}
            },
        },
    },
    
    [tbFacDef.tbIds.WUDU] = {-- 五毒
        szFacName       = "wudu",
        szFacChName     = "Ngũ Độc Giáo",
        szFacShortName  = "Ngũ Độc",
        szFacShortEng   = "wu",
        nFacNumb        = tbFacDef.tbIds.WUDU,
        nCamp           = 2,
        nRankId         = 80,
        nSeries         = 1,
        nTaskId_Fact    = 10,
        nTaskId_90Skill = 124,
        nTaskId_137     = 137,
        nValue_137      = 70,
        n150TaskStep    = 5,
        nJinjieSkillID  = 1222,
        nFactionMapId   = 183,
        tbFactionMapPos = {1746,2673},
        szMsg           = "Kể từ hôm nay, %s gia nhập môn Ngũ Độc giáo, cảm phiền các sư huynh sư tỷ chỉ giáo!",
        tbSkillID       =
        {
            [10]    = {
                63,     -- 毒砂掌  [63]（入门）
                65,     -- 血刀毒杀[65]（入门）
            },
            [20]    = {
                62,     -- 五毒掌法[62]（10级）
                60,     -- 五毒刀法[60]（10级）
                67,     -- 九天狂雷[67]（10级）
            },
            [30]    = {
                70,     -- 赤焰蚀天[70]（20级）
                66,     -- 杂难药经[66]（20级）
            },
            [40]    = {
                68,     -- 幽冥骷髅[68 ]（30级）
                384,    -- 百毒穿心[384]（30级）
                64,     -- 冰蓝玄晶[64 ]（30级）
                69,     -- 无形蛊  [69]（30级）
            },
            [50]    = {
                356,    -- 穿衣破甲  [356]（40级）
                73,     -- 万蛊蚀心[73 ]（40级）
            },
            [60]    = {
                72,     -- 穿心毒刺[72]（50级）
                                        },
            [70]    = {
                71,     -- 天罡地煞手[71 ]（60级）
                75,     -- 五毒奇经[75]（镇派）
                74,     -- 朱蛤青冥  [74 ]（60级）
            },
            [90]    = {
                {353, 1, "Chưởng pháp", "Âm Phong Thực Cốt "},   -- 阴风蚀骨[353]（90级）
                {355, 1, "Đao pháp", "Huyền Âm Trảm "},     -- 玄阴斩  [355]（90级）
                {390, 0, "Thông dụng", "Đoạn Cân Hủ Cốt "},   -- 断筋腐骨[390]（90级）
            },
            [120]   = {
                {711, 1, "Thông dụng", "Hấp Tinh Yểm"}
            },
            [150]   = {
                {1066, 1, "Chưởng pháp", ""},  -- 形销骨立
                {1067, 1, "Đao pháp", ""},  -- 幽魂噬影
            },
        },
        tbItemInfo      =
        {
            tbSkillBook90 = {
                {"Ngũ Độc Chưởng Pháp. Quyển 1",47},
                {"Ngũ Độc Đao pháp. Quyển 2",48},
                {"Ngũ Độc Nhiếp Tâm thuật. Quyển 3",49}
            },
        },
    },
    
    [tbFacDef.tbIds.EMEI] = {-- 峨眉
        szFacName       = "emei",
        szFacChName     = "Nga My phái",
        szFacShortName  = "Nga Mi",
        szFacShortEng   = "em",
        nFacNumb        = tbFacDef.tbIds.EMEI,
        nCamp           = 1,
        nRankId         = 64,
        nSeries         = 2,
        nTaskId_Fact    = 1,
        nTaskId_90Skill = 125,
        nTaskId_137     = 137,
        nValue_137      = 61,
        n150TaskStep    = 7,
        nJinjieSkillID  = 1224,
        nFactionMapId   = 13,
        tbFactionMapPos = {1898,4978},
        szMsg           = "Kể từ hôm nay, %s gia nhập Nga My phái, cảm phiền các sư tỷ chỉ giáo",
        tbSkillID       =
        {
            [10]    = {
                85,     -- 一叶知秋[85]（入门）
                80,     -- 飘雪穿云[80]（入门）
            },
            [20]    = {
                77,     -- 峨嵋剑法[77]（10级）
                79,     -- 峨嵋掌法[79]（10级）
            },
            [30]    = {
                93,     -- 慈航普渡[93]（20级）
            },
            [40]    = {
                385,    -- 推窗望月[385]（30级）
                82,     -- 四象同归[82]（30级）
                89,     -- 梦蝶    [89]（30级）
            },
            [50]    = {
                86,     -- 流水    [86 ]（40级）
            },
            [60]    = {
                92,     -- 佛心慈佑[92]（50级）
            },
            [70]    = {
                88,     -- 不灭不绝[88]（60级）
                252,    -- 佛法无边[252]（镇派）
                91,     -- 佛光普照[91]（60级）
                282,    -- 清音梵唱[282]（60级）                          
            },
            [90]    = {
                {328, 1, "Kiếm pháp", "Tam Nga Tề Tuyết "},   -- 三峨霁雪[328]（90级）
                {380, 1, "Chưởng pháp", "Phong Sương Toái ảnh "},   -- 风霜碎影[380]（90级）
                {332, 0, "Thông dụng", "Phổ Độ Chúng Sinh "},   -- 普渡众生[332]（90级）
            },
            [120]   = {
                {712, 1, "Thông dụng", "Bế Nguyệt Phất Trần"}
            },
            [150]   = {
                {1061, 1, "Kiếm pháp", "Kiếm Hoa Vãn Tinh"},  -- 剑花挽晶
                {1062, 1, "Chưởng pháp", "Băng Vũ Lạc Tinh"},  -- 冰雨珞晶
                {1114, 1, "Hỗ trợ", "Ngọc Tuyền Tâm Kinh"},  -- 玉泉心经
            },
        },
        tbItemInfo      =
        {
            tbSkillBook90 = {
                {"Diệt Kiếm Mật Tịch",42},
                {"Nga Mi  Phật Quang Chưởng Mật Tịch",43},
                {"Phổ Độ Mật Tịch",59}
            },
        },
    },
    
    [tbFacDef.tbIds.CUIYAN] = {-- 翠烟
        szFacName       = "cuiyan", 
        szFacChName     = "Thúy Yên môn",
        szFacShortName  = "Thúy Yên",
        szFacShortEng   = "cy",
        nFacNumb        = tbFacDef.tbIds.CUIYAN,
        nCamp           = 3,
        nRankId         = 67,
        nSeries         = 2,
        nTaskId_Fact    = 6,
        nTaskId_90Skill = 126,
        nTaskId_137     = 137,
        nValue_137      = 66,
        n150TaskStep    = 6,
        nJinjieSkillID  = 1225,
        nFactionMapId   = 154,
        tbFactionMapPos = {403,1361},
        szMsg           = "Kể từ hôm nay, %s gia nhập Thúy Yên môn, cảm phiền các sư tỷ chỉ giáo!",
        tbSkillID       =
        {
            [10]    = {
                99,     -- 风花雪月[99 ]（入门）
                102,    -- 风卷残雪[102]（入门）
            },
            [20]    = {
                95,     -- 翠烟刀法[95 ]（10级）
                97,     -- 翠烟双刀[97 ]（10级）
            },
            [30]    = {
                269,    -- 冰心倩影[269]（20级）
            },
            [40]    = {
                105,    -- 雨打梨花[105]（30级）
                113,    -- 浮云散雪[113]（30级）
            },
            [50]    = {
                100,    -- 护体寒冰[100]（30级）
            },
            [60]    = {
                109,    -- 雪影    [109]（40级）
            },
            [70]    = {
                108,    -- 牧野流星[108]（60级）
                114,    -- 冰骨雪心[114]（镇派）
                111,    -- 碧海潮生[111]（60级）
            },
            [90]    = {
                {336, 1, "Đao pháp", "Băng Tung Vô ảnh "},   -- 冰踪无影[336]（90级）
                {337, 1, "Song đao", "Băng Tâm Tiên Tử  "},   -- 冰心仙子[337]（90级）
            },
            [120]   = {
                {713, 1, "Thông dụng", "Ngự Tuyết ẩn"}
            },
            [150]   = {
                {1063, 1, "Đao pháp", "Băng Tước Hoạt Kỳ"},  -- 冰雀越枝
                {1065, 1, "Song đao", "Thủy Anh Man Tú"},  -- 水映曼秀
            },
        },
        tbItemInfo      =
        {
            tbSkillBook90 = {
                {"Thúy Yên đao pháp",40},
                {"Thúy Yên song đao",41}
            },
        },
    },
    
    [tbFacDef.tbIds.GAIBANG] = {-- 丐帮
        szFacName       = "gaibang",
        szFacChName     = "Cái Bang",
        szFacShortName  = "Cái Bang",
        szFacShortEng   = "gb",
        nFacNumb        = tbFacDef.tbIds.GAIBANG,
        nCamp           = 1,
        nRankId         = 78,
        nSeries         = 3,
        nTaskId_Fact    = 8,
        nTaskId_90Skill = 128,
        nTaskId_137     = 137,
        nValue_137      = 68,
        n150TaskStep    = 9,
        nJinjieSkillID  = 1227,
        nFactionMapId   = 115,
        tbFactionMapPos = {1501,3672},
        szMsg           = "Kể từ hôm nay, %s gia nhập Cái Bang, cảm phiền các tiền bối chỉ giáo!",
        tbSkillID       =
        {
            [10]    = {
                122,    -- 见人伸手[122]（入门）
                119,    -- 沿门托钵[119]（入门）
            },
            [20]    = {
                116,    -- 丐帮掌法[116]（10级）
                115,    -- 丐帮棒法[115]（10级）
            },
            [30]    = {
                129,    -- 化险为夷[129]（20级）
            },
            [40]    = {
                274,    -- 降龙掌[274]（30级）
                124,    -- 打狗阵[124]（30级）
            },
            [50]    = {
                277,    -- 滑不留手[277]（40级
            },
            [60]    = {
                128,    -- 亢龙有悔[128]（50级）
                125,    -- 棒打恶狗[125]（50级）
            },
            [70]    = {
                130,    -- 醉蝶狂舞[130]（镇派）
                360,    -- 逍遥功[360]（60级）
            },
            [90]    = {
                {357, 1, "Chưởng pháp", "Phi Long Tại Thiên "},   -- 飞龙在天[357]（90级）
                {359, 1, "Bổng pháp", "Thiên Hạ Vô Cẩu "},   -- 天下无狗[359]（90级）
            },
            [120]   = {
                {714, 1, "Thông dụng", "Hỗn Thiên Khí Công"}
            },
            [150]   = {
                {1073, 1, "Chưởng pháp", "Thời Thặng Lục Long"},  -- 时乘六龙
                {1074, 1, "Bổng pháp", "Bổng Huýnh Lược Địa"},  -- 棒迥掠地
            },
        },
        tbItemInfo      =
        {
            tbSkillBook90 = {
                {"Cái Bang chưởng pháp",54},
                {"Cái Bang côn pháp",55}
            },
        },
    },
    
    [tbFacDef.tbIds.TIANREN] = {-- 天忍
        szFacName       = "tianren",
        szFacChName     = "Thiên Nhẫn Giáo",
        szFacShortName  = "Thiên Nhẫn",
        szFacShortEng   = "tr",
        nFacNumb        = tbFacDef.tbIds.TIANREN,
        nCamp           = 2,
        nRankId         = 81,
        nSeries         = 3,
        nTaskId_Fact    = 4,
        nTaskId_90Skill = 127,
        nTaskId_137     = 137,
        nValue_137      = 64,
        n150TaskStep    = 9,
        nJinjieSkillID  = 1226,
        nFactionMapId   = 49,
        tbFactionMapPos = {1644,3215},
        szMsg           = "Kể từ hôm nay, %s gia nhập Thiên Nhẫn giáo, bái kiến các sư huynh sư tỷ!",
        tbSkillID       =
        {
            [10]    = {
                135,    -- 残阳如血[135]（入门）
                145,    -- 弹指烈焰[145]（入门）
            },
            [20]    = {
                132,    -- 天忍矛法[132]（10级）
                131,    -- 天忍刀法[131]（10级）
                136,    -- 火焚莲华[136]（10级）
            },
            [30]    = {
                137,    --幻影飞狐[137]（20级）
            },
            [40]    = {
                141,    -- 烈火情天[141]（30级）
                138,    -- 推山填海[138]（30级）
                140,    -- 飞鸿无迹[140]（30级）
            },
            [50]    = {
                364,    -- 悲酥清风[364]（40级）
            },
            [60]    = {
                143,    -- 厉魔夺魂[143]（50级）
            },
            [70]    = {
                142,    -- 偷天换日[142]（60级）
                150,    -- 天魔解体[150]（镇派）
                148,    -- 魔焰七杀[148]（60级）
            },
            [90]    = {
                {361, 1, "Thương pháp", "Vân Long Kích"},     -- 云龙击  [361]（90级）
                {362, 1, "Đao pháp", "Thiên Ngoại Lưu Tinh"},   -- 天外流星[362]（90级）
                {391, 0, "Thông dụng", "Nhiếp Hồn Loạn Tâm "},   -- 慑魂乱心[391]（90级）
            },
            [120]   = {
                {715, 1, "Thông dụng", "Ma Âm Phệ Phách"}
            },
            [150]   = {
                {1075, 1, "Thương pháp", "Giang Hải Nộ Lan"},  -- 江海怒澜
                {1076, 1, "Đao pháp", "Tật Hỏa Liệu Nguyên"},  -- 疾火燎原
            },
        },
        tbItemInfo      =
        {
            tbSkillBook90 = {
                {"Vân Long Kích. Mâu pháp",35},
                {"Lưu Tinh. Đao pháp",36},
                {"Nhiếp Hồn. Chú thuật",53}
            },
        },
    },
    
    [tbFacDef.tbIds.WUDANG] = {-- 武当
        szFacName       = "wudang",
        szFacChName     = "Võ Đang phái",
        szFacShortName  = "Võ Đang",
        szFacShortEng   = "wd",
        nFacNumb        = tbFacDef.tbIds.WUDANG,
        nCamp           = 1,
        nRankId         = 73,
        nSeries         = 4,
        nTaskId_Fact    = 5,
        nTaskId_90Skill = 129,
        nTaskId_137     = 137,
        nValue_137      = 65,
        n150TaskStep    = 5,
        nJinjieSkillID  = 1228,
        nFactionMapId   = 81,
        tbFactionMapPos = {1574,3224},
        szMsg           = "Kể từ hôm nay, %s gia nhập Võ Đang phái, tham kiến các sư huynh sư tỷ!",
        tbSkillID       =
        {
            [10]    = {
                153,    -- 怒雷指  [153]（入门）
                155,    -- 沧海明月[155]（入门）
            },
            [20]    = {
                152,    -- 武当拳法[152]（10级）
                151,    -- 武当剑法[151]（10级）
            },
            [30]    = {
                159,    -- 七星阵  [159]（30级）
            },
            [40]    = {
                164,    -- 剥及而复[164]（30级）
                158,    -- 剑飞惊天[158]（30级）
            },
            [50]    = {
                160,    -- 梯云纵  [160]（40级）
            },
            [60]    = {
                157,    -- 坐望无我[157]（50级）
            },
            [70]    = {
                165,    -- 无我无剑[165]（60级）
                166,    -- 太极神功[166]（镇派）
                267,    -- 三环套月[267]（60级）
            },
            [90]    = {
                {365, 1, "Quyền pháp", "Thiên Địa Vô Cực "},   -- 天地无极[365]（90级）
                {368, 1, "Kiếm pháp", "Nhân Kiếm Hợp Nhất  "},   -- 人剑合一[368]（90级）
            },
            [120]   = {
                {716, 1, "Thông dụng", "Xuất ứ Bất Nhiễm"}
            },
            [150]   = {
                {1078, 1, "Quyền pháp", "Tạo Hóa Thái Thanh"},  -- 造化太清
                {1079, 1, "Kiếm pháp", "Kiếm Thùy Tinh Hà"},  -- 剑垂星河
            },
        },
        tbItemInfo      =
        {
            tbSkillBook90 = {
                {"Thái Cực Quyền Phổ. Quyển 3",33},
                {"Thái Cực Kiếm Phổ. Quyển 2",34}
            },
        },
    },
    
    [tbFacDef.tbIds.KUNLUN] = {-- 昆仑
        szFacName       = "kunlun",
        szFacChName     = "Côn Lôn phái",
        szFacShortName  = "Côn Lôn",
        szFacShortEng   = "kl",
        nFacNumb        = tbFacDef.tbIds.KUNLUN,
        nCamp           = 3,
        nRankId         = 75,
        nSeries         = 4,
        nTaskId_Fact    = 9,
        nTaskId_90Skill = 130,
        nTaskId_137     = 137,
        nValue_137      = 69,
        n150TaskStep    = 5,
        nJinjieSkillID  = 1229,
        nFactionMapId   = 131,
        tbFactionMapPos = {1582,3175},
        szMsg           = "Kể từ hôm nay, %s gia nhập Côn Lôn phái, bái kiến các vị đạo huynh!",
        tbSkillID       =
        {
            [10]    = {
                169,    -- 呼风法  [169]（入门）
                179,    -- 狂雷震地[179]（入门）
            },
            [20]    = {
                167,    -- 昆仑刀法[167]（10级）
                168,    -- 昆仑剑法[168]（10级）
                392,    -- 缠字诀[392]（10级）
                171,    -- 清风符  [171]（10级）
            },
            [30]    = {
                174,    -- 羁绊符  [174]（20级）
            },
            [40]    = {
                178,    -- 一气三清[178]（30级）
                172,    -- 迅雷咒  [172]（30级）
                393,    -- 缺少ID（30级）
                173,    -- 天清地浊[173]（30级）
            },
            [50]    = {
                175,    -- 欺寒傲雪[175]（40级）
                181,    -- 弃心符  [181]（40级）
            },
            [60]    = {
                176,    -- 狂风骤电[176]（50级）
                90,     -- 迷踪幻影[90 ]（50级）
            },
            [70]    = {
                275,    -- 霜傲昆仑[275]（镇派）
                182,    -- 五雷正法[182]（60级）
                630,   -- 第三资料片，增加昆仑新技能"玄天无极"
            },
            [90]    = {
                {372, 1, "Đao pháp", "Ngạo Tuyết Tiêu Phong "},   -- 傲雪啸风[372]（90级）
                {375, 1, "Kiếm pháp", "Lôi động Cửu Thiên "},   -- 雷动九天[375]（90级）
                {394, 0, "Thông dụng", "Túy Tiên Tá Cốt"},   -- 醉仙错骨[394]（90级）
            },
            [120]   = {
                {717, 1, "Thông dụng", "Lưỡng Nghi Chân Khí"}
            },
            [150]   = {
                {1080, 1, "Đao pháp", "Cửu Thiên Cương Phong"},  -- 九天罡风
                {1081, 1, "Kiếm pháp", "Thiên Lôi Chấn Nhạc"},  -- 天雷震岳
            },
        },
        tbItemInfo      =
        {
            tbSkillBook90 = {
                {"Ngự Phong thuật",50},
                {"Ngự Lôi thuật",51},
                {"Ngự Tâm thuật",52}
            },
        },
    },
    [tbFacDef.tbIds.HUASHAN] = {-- 华山
        szFacName       = "huashan",
        szFacChName     = "Hoa Sơn phái",
        szFacShortName  = " Hoa Sơn",
        szFacShortEng   = "hs",
        nFacNumb        = tbFacDef.tbIds.HUASHAN,
        nCamp           = 3,
        nRankId         = 89,
        nSeries         = 2,
        nTaskId_Fact    = 3481, -- 门派任务变量
        nTaskId_90Skill = 3486, -- 90技能任务变量
        nTaskId_137     = 137 , -- 出师任务变量
        nValue_137      = 71  , -- 出师任务变量值
        n150TaskStep    = 9,    -- 150技能任务步骤
        nJinjieSkillID  = 1370, -- 进阶技能(浩然之气)
        nFactionMapId   = 987,
        tbFactionMapPos = {1346,3128},
        szMsg           = "%s bắt đầu từ hôm nay gia nhập phái Hoa Sơn, đến gặp và bái kiến các vị sư huynh, xin các vị chiếu cố!",
        tbSkillID       =
        {
            [10] = {
                1347,   -- 白虹贯日(入门)
                1372,   -- 清风送爽(入门)
            },
            [20] = {
                1349,   -- 剑宗总诀(10级)
                1374,   -- 龙扰身  (10级)
            },
            [30] = {
                1350,   -- 养吾剑法(20级)
                1375,   -- 海纳百川(20级)
            },
            [40] = {
                1351,   -- 金雁横空(30级)
                1376,   -- 龙旋剑气(30级)
            },
            [50] = {
                1354,   -- 希夷剑法(40级)
                1378,   -- 气震山河(40级)
            },
            [60] = {
                1355,   -- 天坤倒悬(50级)
                1379,   -- 气贯长虹(50级)
            },
            [70] = {
                1358,   -- 幻眼云湮(镇派)
                1360,   -- 苍松迎客(60级)
                1380,   -- 摩云剑气(60级)
            },
            [90] = {
                {1364, 1, "Hoa Sơn kiếm tông", "Đoạt Mệnh Liên Hoàn Tam Tiên Kiếm"},  -- 夺命连环三仙剑(90级)
                {1382, 1, "Hoa Sơn khí tông", "Phách Thạch Phá Ngọc"},       -- 劈石破玉(90级)
            },
            [120] = {
                {1365, 1, "Thông dụng", "Tử Hà Kiếm Khí"}                   -- 紫霞剑气(120级)
            },
            [150] = {
                {1369, 1, "Hoa Sơn kiếm tông", "Cửu kiếm hợp nhất"},  -- 九剑合一
                {1384, 1, "Hoa Sơn khí tông", "Thần Quang Toàn Nhiễu"},  -- 神光旋绕
            },
        },
        tbItemInfo      =
        {
            tbSkillBook90 = {
                {"Hoa Sơn Kiếm Tông-Đoạt mệnh liên hoàn tam tiên kiếm", 4062},
                {"Hoa Sơn Khí Tông-Bích Thạch Phá Ngọc", 4063}
            },
        },
    },
    [tbFacDef.tbIds.WUHUN] = {-- 武魂
        szFacName       = "wuhun",
        szFacChName     = "Vũ Hồn ",
        szFacShortName  = "Vũ Hồn",
        szFacShortEng   = "wh",
        nFacNumb        = tbFacDef.tbIds.WUHUN,
        nCamp           = 1,
        nRankId         = 90,
        nSeries         = 3,
        nTaskId_Fact    = 4406, -- 门派任务变量
        nTaskId_JMDZ    = 4405,
        nTaskValue_JMDZ = 30*256,
        nTaskId_90Skill = 4407, -- 90技能任务变量
        nTaskId_137     = 137 , -- 出师任务变量
        nValue_137      = 71  , -- 出师任务变量值
        n150TaskStep    = 3,
        nJinjieSkillID  = 1986, -- 进阶技能(精忠报国)
        nFactionMapId   = 1042,
        tbFactionMapPos = {1530,3251},
        szMsg           = "%s từ này gia nhập Vũ Hồn, đến bái kiến các vị sư huynh, xin các vị chiếu cố!",
        tbSkillID       =
        {
            [10] = {
                1972,   -- 夺寨扬旗(入门)
                1974,   -- 长缨挥斥(入门)
            },
            [20] = {
                1964,   -- 武魂堂盾法(10级)
                1975,   -- 武魂堂刀法  (10级)
            },
            [30] = {
                1976,   -- 公忠秉性(20级)
            },
            [40] = {
                1965,   -- 请缨提锐旅(30级)
                1977,   -- 撼山击(30级)
            },
            [50] = {
                1963,   -- 朝天阙(40级)
                1979,   -- 怒劈天涯(40级)
            },
            [60] = {
                1980,   -- 孤将百战酣(50级)
            },
            [70] = {
                1982,   -- 武穆遗志(镇派)
                1971,   -- 智勇超伦(60级)
                1981,   -- 饥餐胡虏肉(60级)
            },
            [90] = {
                {1967, 1, "Vũ Hồn hệ Thuẫn", "Trấn Biên Thùy"},  -- 镇边陲(90级)
                {1983, 1, "Vũ Hồn hệ Đao", "Khát ẩm Hung Nô Huyết"},  -- 渴饮匈奴血(90级)
            },
            [120] = {
                {1984, 1, "Thông dụng", "Trung Vu Lưu Phong"}                   -- 忠武流风(120级)
            },
            [150] = {
                {1969, 1, "Vũ Hồn hệ Thuẫn", "Huy Sư Diệt Lỗ"},  -- 挥师灭虏
                {1985, 1, "Vũ Hồn hệ Đao", "Trừ Gian Diệt Nịnh"},  -- 铲奸除佞
            },
        },
        tbItemInfo      =
        {
            tbSkillBook90 = {   -- 90级技能书道具名和ID,注意！要和技能表中90级技能的顺序一一对应！
                {"Vũ Hồn Thuẫn Pháp#Trấn Biên Thùy",5106},
                {"Vũ Hồn Đao Pháp#Khát ẩm Hung Nô Huyết",5107},
            },
        },
    },
    [tbFacDef.tbIds.XIAOYAO] = {-- 逍遥派
        bFactionIsOpen  = 1,
        szFacName       = "xiaoyao",
        szFacChName     = "Phái Tiêu Dao",
        szFacShortName  = "Tiêu Dao",
        szFacShortEng   = "xy",
        nFacNumb        = tbFacDef.tbIds.XIAOYAO,
        nCamp           = 3,
        nRankId         = 98,
        nSeries         = 4,
        nTaskId_Fact    = 4464, -- 门派任务变量
        nTaskId_JMDZ    = 4463,
        nTaskValue_JMDZ = 40*256,
        nTaskId_90Skill = 4465, -- 90技能任务变量
        nTaskId_137     = 137 , -- 出师任务变量
        nValue_137      = 71  , -- 出师任务变量值
        n150TaskStep    = 6,
        nJinjieSkillID  = 2132,  --深藏身名
        nFactionMapId   = 1057,
        tbFactionMapPos = {1584,3251},
        tbSchools       = {28,29},
        szMsg           = "%s từ hôm nay vào Phái Tiêu Dao, nay đến bái kiến các vị sư huynh, mong hãy chiếu cố!",
        szSelfCall      = "Bổn Môn",
        tbSkillID       =
        {
            [10] = {
                2114,   -- 吴钩霜雪(入门)
                2136,   -- 随意曲(入门)
            },
            [20] = {
                2115,   -- 逍遥剑法(10级)
                2137,   -- 逍遥琴法(10级)
            },
            [30] = {
                2116,   -- 移宫换羽(20级)
            },
            [40] = {
                2118,   -- 飒沓流星(30级)
                2138,   -- 落雁平沙(30级)
            },
            [50] = {
                2120,   -- 倚剑拂天(40级)
                2140,   -- 广陵散(40级)
            },
            [60] = {
                2121,   -- 银鞍白马(50级)
            },
            [70] = {
                2123,   -- 千里独行(镇派)
                2122,   -- 侠骨柔情(出师)
            },
            [90] = {
                {2124, 1, "Tiêu Dao Kiếm Tông", "Kiếm Quyết Phù Vân"},  -- 剑决浮云(90级)
                {2141, 1, "Tiêu Dao Cầm Tông", "Cao Sơn Lưu Thủy "},  -- 高山流水(90级)
            },
            [120] = {
                {2127, 1, "Thông dụng", "Ăn Uống No Say"}                   -- 啖炙持觞(120级)
            },
            [150] = {
                {2129, 1, "Tiêu Dao Kiếm Tông", "Thập Bộ Nhất Sát"},  -- 十步一杀
                {2142, 1, "Tiêu Dao Cầm Tông", "Mai Hoa Tam Lộng"},  -- 梅花三弄
            },
        },
        tbItemInfo      =
        {
            tbSkillBook90 = {   -- 90级技能书道具名和ID,注意！要和技能表中90级技能的顺序一一对应！
                {"Tiêu Dao Kiếm Pháp-Kiếm Quyết Phù Vân",5231},
                {"Tiêu Dao Cầm Pháp-Cao Sơn Lưu Thủy",5232},
            },
            tbVipMaskIndex = {
                [1] = 8767, -- 涅槃·炎凰面具
                [2] = 8768, -- 无尽·炎凰面具
                [3] = 8769, -- 天骄·炎凰面具
                [4] = 8781, -- 无双·炎凰面具
            },
            tbVipWuXingYin = {
                [0] = 8770, -- 至尊印鉴
                [1] = 8771, -- 1★至尊印鉴
                [2] = 8772, -- 2★至尊印鉴
                [3] = 8773, -- 3★至尊印鉴
                [4] = 8774, -- 4★至尊印鉴
                [5] = 8775, -- 5★至尊印鉴
                [6] = 8776, -- 6★至尊印鉴
                [7] = 8777, -- 7★至尊印鉴
                [8] = 8778, -- 8★至尊印鉴
                [9] = 8779, -- 9★至尊印鉴
                [10] = 8780, -- 10★至尊印鉴
            }
        },
    },
}

-- Include("\\script\\faction_def.lua")
-- 注意：全部下标是从0开始
function tbFacDef:MakeDefine()
    self.tbFacNames             = {}
    self.tbFacChNames           = {}
    self.tbFacShortName         = {}
    self.tbCamps                = {}
    self.tbRankIds              = {}
    self.tbSeriess              = {}
    self.tbFacTaskIds           = {}
    self.tbTaskId_90Skills      = {}
    self.tbTaskId_137s          = {}
    self.tbValue_137s           = {}
    self.tb150TaskSteps         = {}
    self.tbJinjieSkillIDs       = {}
    self.tbFacMapIds            = {}
    self.tbJoinMsgs             = {}
    self.tbSkills               = {}
    self.tbItemInfos            = {}
    self.tbFacName2TaskId       = {}
    self.tbFacName2RankId       = {}
    self.tbFacName2FacId        = {}
    self.tbFacChName2FacId      = {}
    self.tbFacMapPos            = {}
    self.tb90Skills             = {}
    self.tb120Skills            = {}
    self.tb150Skills            = {}
    self.tbVipMaskIndexs        = {}
    self.tbVipWuXingYins        = {}
    for _, tbFactionInfo in self.tbWholeInfoList do
        self.tbFacNames[tbFactionInfo.nFacNumb]             = tbFactionInfo.szFacName
        self.tbFacChNames[tbFactionInfo.nFacNumb]           = tbFactionInfo.szFacChName
        self.tbFacShortName[tbFactionInfo.nFacNumb]         = tbFactionInfo.szFacShortName
        self.tbCamps[tbFactionInfo.nFacNumb]                = tbFactionInfo.nCamp
        self.tbRankIds[tbFactionInfo.nFacNumb]              = tbFactionInfo.nRankId
        self.tbSeriess[tbFactionInfo.nFacNumb]              = tbFactionInfo.nSeries
        self.tbFacTaskIds[tbFactionInfo.nFacNumb]           = tbFactionInfo.nTaskId_Fact
        self.tbTaskId_90Skills[tbFactionInfo.nFacNumb]      = tbFactionInfo.nTaskId_90Skill
        self.tbTaskId_137s[tbFactionInfo.nFacNumb]          = tbFactionInfo.nTaskId_137
        self.tbValue_137s[tbFactionInfo.nFacNumb]           = tbFactionInfo.nValue_137
        self.tb150TaskSteps[tbFactionInfo.nFacNumb]         = tbFactionInfo.n150TaskStep
        self.tbJinjieSkillIDs[tbFactionInfo.nFacNumb]       = tbFactionInfo.nJinjieSkillID
        self.tbFacMapIds[tbFactionInfo.nFacNumb]            = tbFactionInfo.nFactionMapId
        self.tbJoinMsgs[tbFactionInfo.nFacNumb]             = tbFactionInfo.szMsg

        self.tbSkills[tbFactionInfo.nFacNumb]               = tbFactionInfo.tbSkillID

        self.tbItemInfos[tbFactionInfo.nFacNumb]            = tbFactionInfo.tbItemInfo

        self.tbFacName2TaskId[tbFactionInfo.szFacName]      = tbFactionInfo.nTaskId_Fact
        self.tbFacName2RankId[tbFactionInfo.szFacName]      = tbFactionInfo.nRankId
        self.tbFacName2FacId[tbFactionInfo.szFacName]       = tbFactionInfo.nFacNumb
        self.tbFacChName2FacId[tbFactionInfo.szFacChName]   = tbFactionInfo.nFacNumb

        self.tbFacMapPos[tbFactionInfo.nFacNumb]            = {tbFactionInfo.nFactionMapId, tbFactionInfo.tbFactionMapPos[1], tbFactionInfo.tbFactionMapPos[2]}

        self.tb90Skills[tbFactionInfo.nFacNumb]             = {}
        for _,tbSkillInfo in tbFactionInfo.tbSkillID[90] do
            tinsert(self.tb90Skills[tbFactionInfo.nFacNumb], tbSkillInfo[1])
        end
        self.tb120Skills[tbFactionInfo.nFacNumb]            = {}
        for _,tbSkillInfo in tbFactionInfo.tbSkillID[120] do
            tinsert(self.tb120Skills[tbFactionInfo.nFacNumb], tbSkillInfo[1])
        end
        self.tb150Skills[tbFactionInfo.nFacNumb]            = {}
        for _,tbSkillInfo in tbFactionInfo.tbSkillID[150] do
            tinsert(self.tb150Skills[tbFactionInfo.nFacNumb], tbSkillInfo[1])
        end

        -- self.tbVipMaskIndexs[tbFactionInfo.nFacNumb]        = tbFactionInfo.tbItemInfo.tbVipMaskIndex
        -- self.tbVipWuXingYins[tbFactionInfo.nFacNumb]        = tbFactionInfo.tbItemInfo.tbVipWuXingYin
    end
end

tbFacDef:MakeDefine()

function FacName2Id(szFacName)
    for key, value in tbFacDef.tbWholeInfoList do
        if value.szFacChName == szFacName then
            return key
        end
    end

    return -1;
end

function AddFacSkill(nFacId, nLv)
    if nFacId < 0 or nFacId >= tbFacDef.nMaxFac then
        return
    end
    if not tbFacDef.tbSkills[nFacId][nLv] then
        return
    end

    for _,tbSkillInfo in tbFacDef.tbSkills[nFacId][nLv] do
        local nSkillId = 0
        local nSkillLv = 0
        if type(tbSkillInfo) == "table" then
            nSkillId = tbSkillInfo[1]
            nSkillLv = tbSkillInfo[2]
        elseif type(tbSkillInfo) == "number" then
            nSkillId = tbSkillInfo
            nSkillLv = 0
        end
        if HaveMagic(nSkillId) == -1 then
            AddMagic(nSkillId,nSkillLv)
            AddSkillEvent(nFacId, nLv, nSkillId, nSkillLv)
        end
    end

	DynamicExecute("\\script\\misc\\eventsys\\eventsys.lua", "EventSys:OnPlayerEvent", "OnLearnSkill", PlayerIndex, nLv)
end

function AddSkillEvent(nFacId, nLv, nSkillId, nSkillLv)
    if nLv == 90 then
        WriteLog("Player Get 90 Skill. szAccount = %s, szName = %s, nFacId = %d, nLv = %d, nSkillId = %d, nSkillLv = %d")
    end
end