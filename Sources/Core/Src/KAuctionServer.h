// KAuctionServer.h - [DAUGIA 04/09] Kho DAU GIA tren MySQL (bang `auction_item`) + bo GIU NGUYEN VAT PHAM.
// Chu chot 04/09: dau gia ban bang XU va NGAN LUONG; lam lan luot muc A (ky gui ca nhan),
// B (dau gia toan server), C (dau gia bang hoi). Doc thiet ke: BANGIAO_DAUGIA_CHIENLENH_0409.md.
//
// Diem khac hop THU: thu tao lai vat pham tu thuoc tinh (AddItem) nen trang bi bi DOI thuoc tinh ngau nhien.
// Dau gia thi KHONG duoc phep doi: nguoi ban ky gui dung mon do do. Vi vay o day co cap ham
//   AUC_ItemToRec(nItemIdx) -> chuoi hex  |  AUC_GiveRec(szHex) -> tao lai DUNG mon do cho nguoi choi
// dung dung bo tham so ma duong luu/nap CSDL cua nhan vat dung (KPlayerDBFuns.cpp) - xem chu thich trong .cpp.
#ifndef KAUCTIONSERVER_H
#define KAUCTIONSERVER_H

typedef struct lua_State Lua_State;

#ifdef _SERVER
// ---- vat pham ----
int LuaAUC_ItemToRec(Lua_State* L);   // (nItemIdx) -> szHex, szTen, szMoTa ("g,d,p,l,s,k"), nCells, nStack, nHetHan  (KHONG xoa mon do)
int LuaAUC_RecName(Lua_State* L);
// [A16] (szHex) -> szInfo (bo so du de ve dung mon, ke ca hoang kim), nStack
int LuaAUC_RecDesc(Lua_State* L);     // (szHex) -> szTen (doc ten ma khong tao vat pham)
int LuaAUC_GiveRec(Lua_State* L);     // (szHex) -> nItemIdx (0 = loi/het cho) - tao cho PlayerIndex hien tai
int LuaAUC_RecCells(Lua_State* L);    // (szHex) -> so O hanh trang mon do chiem

// ---- bang auction_item ----
int LuaAUC_Ready(Lua_State* L);       // () -> 1 neu MySQL san sang va bang da co
int LuaAUC_PutOn(Lua_State* L);       // (bang tham so) -> id (0 = loi)
int LuaAUC_List(Lua_State* L);        // (nType, nMax, nAfterId) -> bang muc dang ban
int LuaAUC_Get(Lua_State* L);         // (nId) -> bang mot muc (nil neu khong co)
int LuaAUC_Buy(Lua_State* L);         // (nId, szBuyer, nPrice) -> 1/0 (nguyen tu: chi mot nguoi mua duoc)
int LuaAUC_SetState(Lua_State* L);    // (nId, nState, nBelow) -> 1/0 (nguyen tu)
int LuaAUC_Sweep(Lua_State* L);       // (nNow, nMax) -> bang muc HET HAN can tra lai nguoi ban
int LuaAUC_CountSeller(Lua_State* L); // (szSeller) -> so mon dang ky gui
int LuaAUC_SetPrice(Lua_State* L);    // (nId, nCur, nNextDrop, nDropLeft, nEnd) -> 1/0 (chi khi state 0)
int LuaAUC_Bid(Lua_State* L);
// [DAUGIA 04/09 B1] chong mat do
int LuaAUC_CanGiveRec(Lua_State* L);  // (szHex) -> 1 neu dat duoc vao tui NGAY BAY GIO (khoi lien tuc WxH)
int LuaAUC_Rollback(Lua_State* L);
// [DAUGIA-WEB 04/09] dau gia THE GIOI cau hinh tu web admin (BANGIAO_DAUGIA_WEB_0409.md)
int LuaAUC_MakeRec(Lua_State* L);      // (szAward) -> szHex, szTen, szMoTa, nCells, nStack, szLoi - dung mon THAT tu chuoi award, khong can nguoi choi
int LuaAUCWEB_Ready(Lua_State* L);     // () -> 1 neu hai bang auction_web_* da co
int LuaAUCWEB_Cfg(Lua_State* L);       // () -> {enabled, period, perround, next, last, roundno} / nil
int LuaAUCWEB_ClaimRound(Lua_State* L);// (nNow, nNext) -> 1 neu gianh duoc quyen mo dot (nguyen tu)
int LuaAUCWEB_Pool(Lua_State* L);      // (nMax) -> bang {id, award, currency, start, buy, weight} dang bat
int LuaAUCWEB_Drawn(Lua_State* L);     // (nId, nNow, nAucId, szName) -> 1/0
int LuaAUCWEB_Err(Lua_State* L);       // (nId, szErr) -> 1/0
int LuaAUCWEB_Msg(Lua_State* L);
int LuaAUCWEB_SetNext(Lua_State* L);   // (nNext) -> 1/0: hen lai gio mo dot ke       // (szMsg, nNow) -> 1/0    // (nId) -> tra dong ve dang ban VA xoa buyer/buy_price         // (nId, szBuyer, nPrice, nNewEnd) -> 1/0 nguyen tu: chi khi state 0 va nPrice > cur_price
// [DAUGIA-CHAT 04/09] thong bao dau gia vao kenh chat bang: id kenh "\O<tongid>" hoc tu goi notifychannelid di ngang GameServer
void AUC_GhiNhoKenhBang(const char* szKenh, unsigned long dwId);	// KNewProtocolProcess::P_ProcessPlayerCommExtend goi (chi doc goi tin)
int LuaAUC_MsgTong(Lua_State* L);      // (nTong, szMsg) -> so nguoi nhan: dong "Tin bang" toi thanh vien online, vao the "bang" khi da biet id kenh
int LuaAUC_KenhBang(Lua_State* L);     // (nTong) -> id kenh chat bang da hoc, -1 neu chua
#endif

#endif // KAUCTIONSERVER_H
