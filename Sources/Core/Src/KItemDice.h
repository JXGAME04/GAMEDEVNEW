// KItemDice.h - he XUC XAC chia do (DICEITEM), dich nguoc tu ban Linux JX2.
//
// Dac ta day du (kem dia chi ma may lam bang chung cho tung khang dinh):
//     D:\GAMEDEVNEW\DACTA_HE_XUCXAC_DICEITEM.md
//
// Tom tat vong doi:
//   1. Lua: nId = ApplyItemDice(nType, nMaxRoll, nTime, szFile, szDone, szCall, nCount)
//   2. Lua: nItemIdx = AddDiceItemInfo(nId, nQuality, nG, nD, nP, nLevel, nSeries,
//                                      nRandomSeed, nLucky)      -- tao vat pham THAT
//   3. Lua: voi tung nguoi -> PlayerIndex = X; RollItem(nId)     -- mo cua so tren may X
//   4. Nguoi choi bam "Tham du nhan" / "Huy bo nhan" -> goi c2s_diceitem
//        -> may chu gieo 1..nMaxRoll
//        -> goi szCallFunc(nId, nNumber) trong szFile, PlayerIndex = nguoi vua gieo
//   5. Chot khi MOI NGUOI DA CHON hoac HET GIO (ban Linux chot som, ta giu y het)
//        -> chon nguoi diem cao nhat -> TRAO VAT PHAM
//        -> goi szDoneFunc(nId, nWinner, nNumber)
//
// BA CHO CO Y LECH BAN LINUX (deu la sua loi, deu tuong thich nguoc voi chinh
// kich ban goc - xem muc 7 cua dac ta):
//   1. GetItemDiceRollInfo tra 3 gia tri (t, nSize, nWinner). Ban Linux day 3 thu
//      len ngan xep nhung `return 2` (byte tho `b8 02 00 00 00` @ 0x81c0c65) nen
//      BANG bi roi mat -> `for index, value in t` bao loi.
//   2. szDoneFunc nhan 3 tham so. Ban Linux chi truyen 1 (dung "%u" @ 0x824fb8f),
//      trong khi duong goi lai truyen 2 bang chuoi kieu "dd" @ 0x825597c - chung to
//      engine thua suc truyen nhieu, nen 1 tham so la LOI chu khong phai gioi han.
//   3. Engine TRAO vat pham cho nguoi thang. Khong kich ban Linux nao trao ca
//      (roll_item.lua:Done chi doi trang thai khoa + nhan tin; saizi.lua chi in tin),
//      nen engine phai la ben trao.
#ifndef KITEMDICE_H
#define KITEMDICE_H

#include "GameDataDef.h"      // ChatItem - phai co TRUOC KDiceProtocol.h
#include "KDiceProtocol.h"

#ifdef _SERVER

typedef struct lua_State Lua_State;

#define MAX_ITEM_DICE           64      // so phien xuc xac chay dong thoi
#define MAX_DICE_PLAYER         32      // so nguoi moi phien (To doi JX1 toi da 10)
#define DICE_FUNC_LEN           32      // = ban Linux: strncpy(..., 31)
#define DICE_FILE_LEN           80      // = ban Linux: strncpy(..., 79)

// trang thai phien - GetItemDiceState tra ra so nay
enum KDICE_STATE
{
    DICE_ST_FREE = 0,       // khe trong
    DICE_ST_RUN  = 1,       // dang cho nguoi choi chon
    DICE_ST_DONE = 2,       // da chot (ban Linux: DiceLootItem so [+0x34] == 2)
};

// trang thai tung nguoi - dung lam cot 3 cua GetItemDiceRollInfo
// (saizi.lua doc: 0 = huy bo, 2 = da gieo)
enum KDICE_ROLL
{
    DICE_ROLL_GIVEUP  = 0,
    DICE_ROLL_WAITING = 1,
    DICE_ROLL_DONE    = 2,
};

struct KDicePlayer
{
    int     nPlayerIndex;
    DWORD   dwPlayerId;             // chot danh tinh: khe nguoi choi co the bi tai dung
    int     nNumber;                // diem gieo
    int     nRollState;             // xem KDICE_ROLL
    char    szName[MAX_DICE_NAME_LEN];  // chep lai de con hien ten khi ho thoat
                                        // (dung hang cua KDiceProtocol.h, KHONG dung
                                        //  _NAME_LEN - hang do chi co o KProtocol.h)
};

class KItemDice
{
public:
    void    Reset();
    BOOL    Setup(int nType, int nMaxRoll, int nTime, int nPlayerCount);
    BOOL    AddPlayer(int nPlayerIndex);
    int     FindPlayer(int nPlayerIndex);
    BOOL    SetChoice(int nPlayerIndex, int nChoice);
    void    Breathe();                          // goi moi GIAY
    void    Finish();
    void    FillItemDesc(ChatItem* pDesc);
    void    SendTo(int nPlayerIndex, BYTE btAction, int nNumber,
                   const char* szWho = 0);
    void    SendAll(BYTE btAction, int nNumber, const char* szWho = 0);
    void    CallScript(const char* szFunc, const char* szTypes,
                       int nPlayerIndex, int nA, int nB, int nC);

    int     m_nId;
    int     m_nState;
    int     m_nType;
    int     m_nMaxRoll;
    int     m_nSize;                // so nguoi DU KIEN (= nSize cua saizi.lua)
    int     m_nDecided;             // so nguoi da chon xong
    int     m_nTimeLeft;            // giay con lai
    int     m_nItemIndex;           // vat pham dem ra chia (0 = chua co)
    int     m_nWinner;              // PlayerIndex nguoi thang (0 = khong ai)
    int     m_nWinNumber;
    int     m_nPlayerCount;
    KDicePlayer m_Players[MAX_DICE_PLAYER];
    char    m_szFile[DICE_FILE_LEN];
    char    m_szDoneFunc[DICE_FUNC_LEN];
    char    m_szCallFunc[DICE_FUNC_LEN];
};

class KItemDiceSet
{
public:
    void        Init();
    KItemDice*  Alloc();
    KItemDice*  Find(int nId);
    void        Breathe();

    KItemDice   m_Dice[MAX_ITEM_DICE];
    int         m_nNextId;
};

extern KItemDiceSet g_ItemDiceSet;

// goi tu CoreServerShell::Breathe() - tu do nhip 1 giay ben trong
void KItemDice_Breathe();

// 8 ham Lua - dang ky trong ScriptFuns.cpp
int LuaApplyItemDice(Lua_State* L);
int LuaAddDiceItemInfo(Lua_State* L);
int LuaRollItem(Lua_State* L);
int LuaGetItemDiceState(Lua_State* L);
int LuaDiceLootItem(Lua_State* L);
int LuaGetItemDiceRollInfo(Lua_State* L);
int LuaGetItemDiceItemInfo(Lua_State* L);
int LuaGetItemDicePlayerList(Lua_State* L);

#endif  // _SERVER

#endif  // KITEMDICE_H
