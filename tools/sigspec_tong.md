# Dac ta chu ky 150 ham bang hoi (TONG_/TONGM_)

- **So tham so**: dem tu moi cho goi that trong 5.232 file Lua.
- **So tra ve**: doc tu ma may (gia tri `eax` tai lenh `ret` cua lua_CFunction).
- Cot rong = khong script nao goi -> phai suy tu ma may khi cai dat.

| Ham | Dia chi | Cho goi | So tham so | Kieu tham so | Tra ve |
|---|---|---:|---|---|---|
| `TONGM_ApplyAddOffer` | 0x08198720 | 1 | 3 | bien, bien, bien | 1 |
| `TONGM_ApplyAddOfferEx` | 0x081909d0 | 0 | - | - | 1 |
| `TONGM_ApplyAddRight` | 0x08198600 | 0 | - | - | 1 |
| `TONGM_ApplyAddTaskValue` | 0x081989a0 | 0 | - | - | 1 |
| `TONGM_ApplyAddUTaskValue` | 0x081988a0 | 0 | - | - | 1 |
| `TONGM_ApplyDelRight` | 0x081984e0 | 0 | - | - | 1 |
| `TONGM_ApplySetLWeekGoalOffer` | 0x08199740 | 0 | - | - | 0, 1, 3 |
| `TONGM_ApplySetLWeeklyOffer` | 0x08199b20 | 0 | - | - | 0, 1, 3 |
| `TONGM_ApplySetLastOnlineDate` | 0x08199360 | 1 | 3 | bien, bien, bien | 0, 1, 3 |
| `TONGM_ApplySetRetireDate` | 0x08198f80 | 2 | 3 | bien, bien, bien | 1 |
| `TONGM_ApplySetTaskValue` | 0x08198aa0 | 8 | 4 | bien, bien, hang so, so | 1 |
| `TONGM_CheckRight` | 0x081983b0 | 8 | 3 | bien, ket qua ham, hang so | 1 |
| `TONGM_GetFigure` | 0x08199ec0 | 27 | 2 | bien, ket qua ham | 0, 1, 3 |
| `TONGM_GetJoinDay` | 0x0819a520 | 1 | 2 | bien, ket qua ham | 0, 1, 3 |
| `TONGM_GetJoinTime` | 0x0819a660 | 0 | - | - | 0, 1, 3 |
| `TONGM_GetLWeekGoalOffer` | 0x081998c0 | 1 | 2 | bien, ket qua ham | 0, 1, 3 |
| `TONGM_GetLWeeklyOffer` | 0x08199ca0 | 0 | - | - | 0, 1, 3 |
| `TONGM_GetLastOnlineDate` | 0x081994e0 | 3 | 2 | bien, bien | 0, 1, 3 |
| `TONGM_GetMoney` | 0x0819a3e0 | 0 | - | - | 0, 1, 3 |
| `TONGM_GetName` | 0x08198d60 | 39 | 2 | bien, bien | 1 |
| `TONGM_GetOffer` | 0x08198800 | 2 | 2 | bien, bien | 1 |
| `TONGM_GetOnline` | 0x08199db0 | 3 | 2 | bien, bien | 0, 1, 3 |
| `TONGM_GetRetireDate` | 0x08199100 | 1 | 2 | bien, bien | 1 |
| `TONGM_GetSex` | 0x0819a000 | 0 | - | - | 0, 1, 3 |
| `TONGM_GetTaskValue` | 0x08198c80 | 6 | 3 | bien, bien, hang so | 1 |
| `TONGM_GetUTaskValue` | 0x08198ba0 | 0 | - | - | 1 |
| `TONGM_IsExist` | 0x08198350 | 4 | 2 | bien, bien | 1 |
| `TONG_AddTaskTemp` | 0x0818b280 | 0 | - | - | 0, 1 |
| `TONG_ApplyAddBuildFund` | 0x08194650 | 17 | 2 | bien, bien | 1 |
| `TONG_ApplyAddDay` | 0x081959d0 | 2 | 2 | bien, so | 0, 1 |
| `TONG_ApplyAddEventRecord` | 0x0818bfe0 | 52 | 2 | bien, bien | 1 |
| `TONG_ApplyAddHistoryRecord` | 0x0818c090 | 10 | 2 | bien, bien | 1 |
| `TONG_ApplyAddLWeekGoalValue` | 0x08195010 | 0 | - | - | 1 |
| `TONG_ApplyAddMoney` | 0x0818be50 | 3 | 2 | bien, bien | 1 |
| `TONG_ApplyAddPerStandFund` | 0x08193150 | 5 | 2 | bien, bien | 1 |
| `TONG_ApplyAddStoredBuildFund` | 0x08195a90 | 7 | 2 | bien, bien | 0, 1 |
| `TONG_ApplyAddStoredOffer` | 0x08195c10 | 1 | 2 | bien, bien | 0, 1 |
| `TONG_ApplyAddTaskValue` | 0x08196450 | 57 | 3 | bien, hang so, bien | 0, 1 |
| `TONG_ApplyAddTotalBuildFund` | 0x08194b90 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplyAddUTaskValue` | 0x0818ad50 | 0 | - | - | 0, 1 |
| `TONG_ApplyAddWarBuildFund` | 0x08194410 | 11 | 2 | bien, bien | 1 |
| `TONG_ApplyAddWeek` | 0x08195910 | 1 | 2 | bien, so | 0, 1 |
| `TONG_ApplyAddWeekBuildFund` | 0x08194d10 | 3 | 2 | bien, bien | 1 |
| `TONG_ApplyAddWeekGoalValue` | 0x08195550 | 2 | 2 | bien, bien | 0, 1 |
| `TONG_ApplyCreatMap` | 0x081905c0 | 2 | 2 | bien, bien | 0, 1 |
| `TONG_ApplyDegrade` | 0x0818fcc0 | 1 | 1 | bien | 0, 1 |
| `TONG_ApplyDeleteMap` | 0x08190720 | 0 | - | - | 0, 1 |
| `TONG_ApplyDeleteMember` | 0x08190510 | 0 | - | - | 0, 1 |
| `TONG_ApplyInit` | 0x0818fac0 | 0 | - | - | 0, 1 |
| `TONG_ApplyKickMember` | 0x08190310 | 1 | 3 | bien, bien, so | 0, 1 |
| `TONG_ApplyMaintain` | 0x0818fe40 | 0 | - | - | 0, 1 |
| `TONG_ApplySetAnnouncement` | 0x0818c140 | 0 | - | - | 1 |
| `TONG_ApplySetBuildFund` | 0x08192cd0 | 0 | - | - | 1 |
| `TONG_ApplySetCurWeekGoalLevel` | 0x08191ad0 | 2 | 2 | bien, so | 1 |
| `TONG_ApplySetDay` | 0x081926d0 | 0 | - | - | 1 |
| `TONG_ApplySetLWeekGoalEvent` | 0x08192010 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplySetLWeekGoalLevel` | 0x08191f50 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplySetLWeekGoalPlayer` | 0x08191dd0 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplySetLWeekGoalPricePlayer` | 0x08191b90 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplySetLWeekGoalPriceTong` | 0x08191c50 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplySetLWeekGoalTotal` | 0x08191e90 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplySetLWeekGoalValue` | 0x08191d10 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplySetMaintainFund` | 0x08192850 | 2 | 2 | bien, bien | 1 |
| `TONG_ApplySetMoney` | 0x0818fa10 | 0 | - | - | 0, 1 |
| `TONG_ApplySetOccupyCityDay` | 0x08192d90 | 2 | 2 | bien, so | 1 |
| `TONG_ApplySetPauseState` | 0x081929d0 | 3 | 2 | bien, so | 1 |
| `TONG_ApplySetPerStandFund` | 0x081917d0 | 5 | 2 | bien, bien | 1 |
| `TONG_ApplySetStoredBuildFund` | 0x08192790 | 0 | - | - | 1 |
| `TONG_ApplySetStoredOffer` | 0x08192910 | 0 | - | - | 1 |
| `TONG_ApplySetStunt` | 0x081908f0 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplySetTaskValue` | 0x0818ae40 | 85 | 3 | bien, hang so, bien | 0, 1 |
| `TONG_ApplySetTongMap` | 0x08190670 | 1 | 2 | bien, bien | 0, 1 |
| `TONG_ApplySetTongMapBan` | 0x0818af30 | 1 | 2 | bien, bien | 0, 1 |
| `TONG_ApplySetTotalBuildFund` | 0x08191890 | 0 | - | - | 1 |
| `TONG_ApplySetWarBuildFund` | 0x08192a90 | 0 | - | - | 1 |
| `TONG_ApplySetWeek` | 0x08192610 | 0 | - | - | 1 |
| `TONG_ApplySetWeekBuildFund` | 0x08191a10 | 1 | 2 | bien, so | 1 |
| `TONG_ApplySetWeekBuildUpper` | 0x08191950 | 0 | - | - | 1 |
| `TONG_ApplySetWeekGoalEvent` | 0x08192550 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplySetWeekGoalLevel` | 0x08192490 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplySetWeekGoalPlayer` | 0x08192310 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplySetWeekGoalPricePlayer` | 0x081920d0 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplySetWeekGoalPriceTong` | 0x08192190 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplySetWeekGoalTotal` | 0x081923d0 | 1 | 2 | bien, bien | 1 |
| `TONG_ApplySetWeekGoalValue` | 0x08192250 | 1 | 2 | bien, so | 1 |
| `TONG_ApplyUpgrade` | 0x0818fb40 | 0 | - | - | 0, 1 |
| `TONG_ApplyWeeklyMaintain` | 0x0818fec0 | 2 | 1 | bien | 0, 1 |
| `TONG_ChangeAllMemberFeature` | 0x081907a0 | 1 | 3 | bien, bien, so | 0, 1 |
| `TONG_ContributeOffer` | 0x081968c0 | 1 | 3 | bien, bien, bien | 0, 1 |
| `TONG_DistributeOfferToGroup` | 0x08197f70 | 1 | 3 | bien, bien, bien | 0, 1 |
| `TONG_DistributeOfferToMember` | 0x08197a70 | 1 | 3 | bien, bien, bien | 0, 1 |
| `TONG_GetBuildFund` | 0x0818d560 | 17 | 1 | bien | 1 |
| `TONG_GetBuildLevel` | 0x0818d600 | 33 | 1 | bien | 1 |
| `TONG_GetCredit` | 0x0818c7b0 | 0 | - | - | 1 |
| `TONG_GetCurCamp` | 0x0818c670 | 2 | 1 | bien | 1 |
| `TONG_GetCurWeekGoalLevel` | 0x0818f2c0 | 2 | 1 | bien | 0, 1 |
| `TONG_GetDay` | 0x0818dec0 | 17 | 1 | bien | 1 |
| `TONG_GetExp` | 0x0818c8f0 | 0 | - | - | 1 |
| `TONG_GetExpLevel` | 0x0818ca30 | 1 | 1 | bien | 1 |
| `TONG_GetFirstMember` | 0x08197880 | 5 | 2 | bien, so | 0, 1 |
| `TONG_GetFirstTong` | 0x0818a680 | 3 | 0 | - | 0, 1 |
| `TONG_GetLWeekGoalEvent` | 0x0818ea00 | 2 | 1 | bien | 1 |
| `TONG_GetLWeekGoalLevel` | 0x0818eb40 | 0 | - | - | 1 |
| `TONG_GetLWeekGoalPlayer` | 0x0818edc0 | 1 | 1 | bien | 1 |
| `TONG_GetLWeekGoalPricePlayer` | 0x0818f180 | 1 | 1 | bien | 0, 1 |
| `TONG_GetLWeekGoalPriceTong` | 0x0818f040 | 1 | 1 | bien | 1 |
| `TONG_GetLWeekGoalTotal` | 0x0818ec80 | 0 | - | - | 1 |
| `TONG_GetLWeekGoalValue` | 0x0818ef00 | 0 | - | - | 1 |
| `TONG_GetMaintainFund` | 0x0818dc40 | 3 | 1 | bien | 1 |
| `TONG_GetMaster` | 0x08197ec0 | 6 | 1 | ket qua ham | 0, 1 |
| `TONG_GetMemberCount` | 0x0818ff40 | 32 | 1, 2 | bien, so | 0, 1 |
| `TONG_GetMoney` | 0x0818f900 | 8 | 1 | bien | 0, 1 |
| `TONG_GetName` | 0x0818ac80 | 6 | 1 | bien | 0, 1 |
| `TONG_GetNextMember` | 0x08197c10 | 2 | 3 | bien, bien, so | 0, 1 |
| `TONG_GetNextTong` | 0x0818b4c0 | 3 | 1 | bien | 0, 1 |
| `TONG_GetOccupyCityDay` | 0x0818d380 | 1 | 1 | bien | 1 |
| `TONG_GetOnlineCount` | 0x0818fff0 | 2 | 2 | bien, bien | 0, 1 |
| `TONG_GetPauseState` | 0x0818d9c0 | 18 | 1, 2 | bien, bien | 1 |
| `TONG_GetPerStandFund` | 0x0818f860 | 4 | 1 | bien | 0, 1 |
| `TONG_GetPremium` | 0x0818d740 | 0 | - | - | 1 |
| `TONG_GetSelfCamp` | 0x0818c530 | 0 | - | - | 1 |
| `TONG_GetStandFund` | 0x0818cf10 | 2 | 1 | bien | 1 |
| `TONG_GetStoredBuildFund` | 0x0818dd80 | 3 | 1 | bien | 1 |
| `TONG_GetStoredOffer` | 0x0818db00 | 4 | 1 | bien | 1 |
| `TONG_GetTaskTemp` | 0x08190260 | 6 | 2 | bien, hang so | 0, 1 |
| `TONG_GetTaskValue` | 0x081900a0 | 141 | 2, 3 | bien, hang so, bien | 0, 1 |
| `TONG_GetTongByRoleName` | 0x0819a700 | 0 | - | - | 0, 3 |
| `TONG_GetTongCount` | 0x0818a6c0 | 0 | - | - | 0, 1 |
| `TONG_GetTongMap` | 0x0818d060 | 22 | 1 | bien | 1 |
| `TONG_GetTongMapBan` | 0x0818d240 | 5 | 1 | bien | 1 |
| `TONG_GetTongMapTemplate` | 0x0818d1a0 | 4 | 1 | bien | 1 |
| `TONG_GetTotalBuildFund` | 0x0818f680 | 0 | - | - | 0, 1 |
| `TONG_GetUTaskValue` | 0x08190180 | 0 | - | - | 0, 1 |
| `TONG_GetUnionID` | 0x0818cbf0 | 2 | 1 | bien | 1 |
| `TONG_GetWarBuildFund` | 0x0818d920 | 12 | 1 | bien | 1 |
| `TONG_GetWarState` | 0x0818cd30 | 0 | - | - | 1 |
| `TONG_GetWeek` | 0x0818e000 | 14 | 1 | bien | 1 |
| `TONG_GetWeekBuildFund` | 0x0818f400 | 5 | 1 | bien | 0, 1 |
| `TONG_GetWeekBuildUpper` | 0x0818f540 | 0 | - | - | 0, 1 |
| `TONG_GetWeekGoalEvent` | 0x0818e140 | 3 | 1 | bien | 1 |
| `TONG_GetWeekGoalLevel` | 0x0818e280 | 1 | 1 | bien | 1 |
| `TONG_GetWeekGoalPlayer` | 0x0818e500 | 2 | 1 | bien | 1 |
| `TONG_GetWeekGoalPricePlayer` | 0x0818e8c0 | 1 | 1 | bien | 1 |
| `TONG_GetWeekGoalPriceTong` | 0x0818e780 | 1 | 1 | bien | 1 |
| `TONG_GetWeekGoalTotal` | 0x0818e3c0 | 1 | 1 | bien | 1 |
| `TONG_GetWeekGoalValue` | 0x0818e640 | 1 | 1 | bien | 1 |
| `TONG_IsExist` | 0x08190890 | 1 | 1 | bien | 1 |
| `TONG_Name2ID` | 0x0818b600 | 16 | 1 | bien | 0, 1 |
| `TONG_SetTaskTemp` | 0x0818b350 | 6 | 3 | bien, hang so, bien | 0, 1 |
| `TONG_WriteLog` | 0x0818b5a0 | 2 | 1 | ket qua ham | 0, 1 |
