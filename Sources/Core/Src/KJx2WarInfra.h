// KJx2WarInfra.h - DOT E (cong thanh JX2, E4): nhom ham ha tang tran con thieu.
// Danh muc + bang chung tung ham: PHULUC_HAM_CONGTHANH.md (muc 2e).
// 4 ten ForbidChangePK/DisabledUseTownP/PushString/PopString la LO THUNG CO SAN
// cua binary goc (khong dang ky dau ca) - ta dang ky that de script chay tron
// (deviation co chu dich, xem PHULUC muc 4).
#ifndef KJX2WARINFRA_H
#define KJX2WARINFRA_H

#ifdef _SERVER

typedef struct lua_State Lua_State;

// --- npc / map ---
int LuaSetNpcDeathScript(Lua_State* L);   // (nNpcIdx, szScript) - ghi ActionScript; KNpc::OnDeath tu chay OnDeath()
int LuaClearMapNpc(Lua_State* L);         // (nMapId) - xoa moi NPC thuong tren MOI instance map
int LuaAddObstacleObj(Lua_State* L);      // (nObjId, nMapId, nMpsX, nMpsY) - chan o (khong ve hinh - ghi nhan)
int LuaClearObstacleObj(Lua_State* L);    // (nMapId, nMpsX, nMpsY) - bo chan
int LuaGetLoop(Lua_State* L);             // () -> tick game (log)
int LuaGetNpcSettingIdx(Lua_State* L);    // (nNpcIdx) -> npcs.txt idx / 0
int LuaGetLastDiagNpc(Lua_State* L);      // () -> npc idx dang thoai / 0

// --- nguoi choi ---
int LuaSetPKFlag(Lua_State* L);           // (0/1) = SetFightState (JX2 goi ca hai canh nhau)
int LuaForbidChangePK(Lua_State* L);      // no-op nhan doi (lo goc; chua co co cam doi PK tay)
int LuaDisabledUseTownP(Lua_State* L);    // no-op nhan doi (lo goc)
int LuaRestoreOwnFeature(Lua_State* L);   // no-op (JX1 khong doi feature trong citywar)
int LuaGetPlayerRev(Lua_State* L);        // () -> nSubWorldId, nRevId (2 gia tri - cho SetRevPos)
int LuaSearchPlayer(Lua_State* L);        // (szName) -> nPlayerIdx / 0
int LuaGetJoinTongTime(Lua_State* L);     // stub 0 (chi arena idle goi)
int LuaGetTongMaster(Lua_State* L);       // () -> ten bang chu cua bang MINH / ""
int LuaAddTongExp(Lua_State* L);          // stub 0 (chi arena idle goi)

// --- item theo EVENT id (quest item: genre item_task, detail = id; bang
//     questkey.txt cay ta DA co san row 343 + 354-367) ---
int LuaGetItemCountEx(Lua_State* L);      // (nEventId) -> so luong trong tui
int LuaDelItemEx(Lua_State* L);           // (nEventId) - xoa 1
int LuaGetItemLife(Lua_State* L);         // (nEventId) -> 0 khi co (luon "con han"), -1 khi khong co
int LuaGetItemProp(Lua_State* L);         // (nItemIdx) -> 3 gia tri: genre, detail, particular

// --- giao vat pham (map len GiveBox/pos_affairitem co san cua JX1) ---
int LuaGiveItemUI(Lua_State* L);          // (szTitle, szDesc, szOkCb, szCancelCb) - mo hop giao do;
                                          // xac nhan -> KProtocolProcess goi cb(nCount) qua GiveBoxCollect
int LuaGetGiveItemUnit(Lua_State* L);     // (i 1-based) -> item idx (stack N = N muc lien tiep)
int LuaRemoveItemByIndex(Lua_State* L);   // (nItemIdx) - tru 1 don vi (stack-- / xoa khi het)
int KJx2WarInfra_GiveBoxCollect(int nPlayerIdx);  // -1 = khong phai phien JX2; >=0 = so don vi da gom

// --- bo dung chuoi engine (PushString/AppendString/ReplaceString/PopString) ---
int LuaPushString(Lua_State* L);          // (sz) nap dem
int LuaAppendString(Lua_State* L);        // (sz) noi duoi
int LuaReplaceString(Lua_State* L);       // (szFrom, szTo) thay MOI lan xuat hien trong dem
int LuaPopString(Lua_State* L);           // () -> chuoi + xoa dem

// --- khac ---
int LuaSetSiegeVoitureParam(Lua_State* L);// (nToolNpcIdx, nDoorNpcIdx) - luu tham so xe (chua co AI xe)
int LuaSetMangonelParam(Lua_State* L);    // (nToolNpcIdx, nX, nY, nParam) - luu tham so may ban da
int LuaNW_GetSealInfo(Lua_State* L);      // stub "" (quoc chien chua port)
int LuaPARTNER_GetCurPartner(Lua_State* L);   // stub 0 (task_string can ten ham)
int LuaPARTNER_GetSettingIdx(Lua_State* L);   // stub 0

#endif // _SERVER
#endif // KJX2WARINFRA_H
