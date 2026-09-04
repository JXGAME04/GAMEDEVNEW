-- test_aucchat.lua - lua4.exe: gia lap ham C++ roi nap auction_manager.lua THAT, chay moi duong bao kenh chat
-- Ket qua in ra: [CHAT-TG] = Msg2SubWorld, [CHAT-BANG:<tong>] = AUC_MsgTong (DLL moi), [MSG2TONG] = duong lui DLL cu
NOW = 1000000
CHAT_MAX_SEEN = 0
function Include(p) end
function GhiLog(a, b) print("[LOG " .. a .. "] " .. b) end
function GetCurrentTime() return NOW end
function GetLocalDate(f)
    if f == "%H" then return "15" end
    if f == "%M" then return "00" end
    if f == "%S" then return "22" end
    return "15:00 04/09"
end
function AddTimer(nFrames, szFunc, nParam) return 1 end
GLB = {}
function SetGlbValue(k, v) GLB[k] = v end
function GetGlbValue(k) return GLB[k] or 0 end
function AUC_Ready() return 1 end
function AUCWEB_Ready() return 1 end
function GetName() return "NguoiChoi" end
function GetTongName() return "BangA", 305419896 end
function GetCash() return 999999999 end
function GetTask(n) return 999999 end
function SetTask(n, v) end
function Pay(n) return 1 end
function Earn(n) end
function MailManager_SendMail(...) print("[MAIL] toi " .. arg[1] .. ": " .. arg[3] .. " | " .. arg[4] .. " | " .. arg[5]); return 1 end
function SendScriptDataToPlayer(idx, e, h) return 1 end
function OB_Create() return 1 end
function OB_Release(h) end
ObjBuffer = {PushByType = function(self, h, t, v) end}
ScriptProtocol = {}
OBJTYPE_NUMBER = 1; OBJTYPE_STRING = 2; OBJTYPE_TABLE = 3
function TONG_ApplyAddMoney(t, n) print("[QUYBANG] +" .. n) end
function TONG_ApplyAddEventRecord(t, s) print("[SUKIENBANG] " .. s) end
function AUC_RecDesc(rec) return "1,2,3", 1 end

function DoDai(sz)
    if strlen(sz) > CHAT_MAX_SEEN then CHAT_MAX_SEEN = strlen(sz) end
    return " (" .. strlen(sz) .. "b)"
end
function Msg2SubWorld(sz) print("[CHAT-TG] " .. sz .. DoDai(sz)) end
function Msg2Player(s) print("[MSG2P] " .. s) end
function Msg2Tong(idx, tong, sz, ch) print("[MSG2TONG idx=" .. idx .. " tong=" .. tong .. " ch=" .. ch .. "] " .. sz .. DoDai(sz)) end
function FindPlayer(name) if name == "ThanhVienOn" then return 7 end return 0 end
MEMBERS = {"ThanhVienOff", "ThanhVienOn"}
function TONG_GetFirstMember(t, f) return 1 end
function TONG_GetNextMember(t, m, f) if m < getn(MEMBERS) then return m + 1 end return 0 end
function TONGM_GetName(t, m) return MEMBERS[m] end
function TONGM_GetOnline(t, m) if MEMBERS[m] == "ThanhVienOn" then return 1 end return 0 end

-- kho gia lap
ROWS = {}
NEXTID = 100
function AUC_PutOn(nType, szAct, nKind, szSeller, nTong, szName, szDesc, szRec, nCells, nCur, nBase, nCurP, nGuar, nDep, nStart, nEnd, nNext, nLeft)
    NEXTID = NEXTID + 1
    ROWS[NEXTID] = {id = NEXTID, atype = nType, activity = szAct, kind = nKind, seller = szSeller, tong = nTong, name = szName,
        desc = szDesc, rec = szRec, cells = nCells, currency = nCur, base = nBase, cur = nCurP, guar = nGuar, buyer = "",
        deposit = nDep, start = nStart, endtime = nEnd, nextdrop = nNext, dropleft = nLeft, state = 0}
    return NEXTID
end
function AUC_Get(id) return ROWS[id] end
function AUC_List(t, n, after) local out = {}; for id, r in ROWS do if r.atype == t and r.state == 0 and id > after then tinsert(out, r) end end; return out end
function AUC_Buy(id, who, price) local r = ROWS[id]; if not r or r.state ~= 0 then return 0 end; r.state = 1; r.buyer = who; return 1 end
function AUC_Bid(id, who, price, nEnd) local r = ROWS[id]; if not r or r.state ~= 0 or price <= r.cur then return 0 end; r.buyer = who; r.cur = price; r.endtime = nEnd; return 1 end
function AUC_SetState(id, s, below) local r = ROWS[id]; if r then r.state = s end; return 1 end
function AUC_SetPrice(id, cur, nd, dl, e) local r = ROWS[id]; if r then r.cur = cur; r.nextdrop = nd; r.dropleft = dl; r.endtime = e end; return 1 end
function AUC_Rollback(id) return 1 end
function AUC_Sweep(now, max) local out = {}; for id, r in ROWS do if r.state == 0 and r.endtime <= now then tinsert(out, r) end end; return out end
function AUC_ItemToRec(idx) return "HEX", "Kiem Bang", "0,1,2,3,4,5", 2, 1, 0 end
function RemoveItemByIndex(i, n) return 1 end
function AUC_GiveRec(h) return 1 end
function GetItemBindState(i) return 0 end
-- web
CFG = {enabled = 1, period = 180, perround = 3, next = 0, last = 0, roundno = 0}
function AUCWEB_Cfg() return CFG end
function AUCWEB_ClaimRound(nNow, nNext) CFG.next = nNext; CFG.roundno = CFG.roundno + 1; return 1 end
POOL = {
    {id = 1, award = "item:1,1,5,1,0,0,50", currency = 1, start = 100, buy = 900, weight = 1},
    {id = 2, award = "gold:1234,1", currency = 2, start = 100, buy = 0, weight = 1},
    {id = 3, award = "item:0,1,182,3,2,10,1", currency = 1, start = 1500000, buy = 30000000, weight = 1},
}
function AUCWEB_Pool(n) return POOL end
function AUCWEB_Drawn(id, now, aid, name) return 1 end
function AUCWEB_Err(id, e) print("[ERR] " .. e); return 1 end
function AUCWEB_SetNext(n) CFG.next = n; return 1 end
function AUCWEB_Msg(m, now) print("[MSG] " .. m); return 1 end
TEN = {["item:1,1,5,1,0,0,50"] = "Chan Nguyen Don (dai)", ["gold:1234,1"] = "Vo Lam Mat Tich", ["item:0,1,182,3,2,10,1"] = "Luc Thuy Tinh"}
function AUC_MakeRec(a) return "HEX", TEN[a] or ("Mon " .. a), "0,0,0,0,0,0", 2, 1, "" end

dofile("E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/script/auction_house/auction_def.lua")
dofile("E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/script/auction_house/auction_manager.lua")
local W = AUCTION_DEF.tbAuctionTypeEnum.eType_WORLD
local T = AUCTION_DEF.tbAuctionTypeEnum.eType_TONG
local ENG = AUCTION_DEF.tbItemTypeEnum.eType_ENGLISH
local DUT = AUCTION_DEF.tbItemTypeEnum.eType_DUTCH

print("==== A) DLL MOI co AUC_MsgTong ====")
function AUC_MsgTong(tong, sz) print("[CHAT-BANG:" .. tong .. "] " .. sz .. DoDai(sz)); return 2 end

print("---- 1) dot web mo (3 mon: id 101 khoi diem 100 mua ngay 900 luong; 102; 103 xu)")
print("body=" .. tostring(call(AucWeb_Body, {}, "x", AucWeb_LoiLua)))
print("---- 2) bang chu dua mon len san bang (kieu Anh, khoi diem 50 van, mua ngay 100 van)")
local idB = AUC_PutOnItem(T, "BangA", ENG, 1, 1000000, 5, 305419896, 500000)
print("idB=" .. idB)
print("---- 3) GM dua mon len san the gioi kieu Ha Lan")
local idD = AUC_PutOnItem(W, "Phien 15:00", DUT, 2, 100, 5, 0, 0)
print("---- 4) thanh vien tra gia mon bang (khoi diem 50 van)")
function GetName() return "ThanhVienOn" end
AUC_OnRequestOfferEnglish(T, "BangA", idB, 500000)
print("---- 5) tra gia mon web id 101 (khoi diem 100, mua ngay 900): tra 100 roi nguoi khac tra 900 = mua ngay")
function GetName() return "CaiBang" end
AUC_OnRequestOfferEnglish(W, "Dot 1 15:00 04/09", 101, 100)
function GetName() return "LyMinh" end
AUC_OnRequestOfferEnglish(W, "Dot 1 15:00 04/09", 101, 900)
print("---- 5b) mua ngay mon web 103 (100 xu, khong mua ngay -> bi tu choi) va tra gia 103")
AUC_OnRequestOfferDutch(W, "Dot 1 15:00 04/09", 103, 100)
AUC_OnRequestOfferEnglish(W, "Dot 1 15:00 04/09", 103, 100)
print("---- 6) ha gia Ha Lan (AUC_Tick sau 5 phut)")
NOW = NOW + 5 * 60 + 1
print("tick=" .. tostring(call(AUC_Tick, {}, "x", AucWeb_LoiLua)))
print("---- 7) het gio: mon bang co nguoi thang; mon web 102 khong ai tra")
NOW = NOW + 4 * 60 * 60
print("tick=" .. tostring(call(AUC_Tick, {}, "x", AucWeb_LoiLua)))
print("---- 8) rut mon (bang chu rut mon chua ai tra)")
function GetName() return "NguoiChoi" end
local idR = AUC_PutOnItem(T, "BangA", ENG, 1, 2000000, 5, 305419896, 1000000)
AUC_OnRequestGetBack(T, "BangA", idR)
print("---- 9) ky gui ca nhan: KHONG duoc bao")
local idP = AUC_PutOnItem(AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL, "", ENG, 1, 300000, 5, 0, 100000)
function GetName() return "KhachMua" end
AUC_OnRequestOfferEnglish(AUCTION_DEF.tbAuctionTypeEnum.eType_PERSONAL, AUCTION_DEF.szPersonalActivity, idP, 100000)

print("==== B) DLL CU: khong co AUC_MsgTong -> Msg2Tong qua thanh vien online ====")
AUC_MsgTong = nil
function GetName() return "NguoiChoi" end
local idC = AUC_PutOnItem(T, "BangA", ENG, 2, 500, 5, 305419896, 100)
print("---- B2) bang khong ai online -> im lang (tra 0)")
MEMBERS = {"ThanhVienOff"}
AUC_OnRequestGetBack(T, "BangA", idC)
print("---- C) cau qua dai bi cat <= 200")
AUC_ChatGui(W, 0, strrep("x", 300))
print("MAX BYTE MOT DONG = " .. CHAT_MAX_SEEN)
print("---- D) loi trong ham bao khong pha giao dich")
AUC_ChatThu(function(r) return r.khong_co.gi end, {{}})
print("XONG")
