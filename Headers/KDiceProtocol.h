#ifndef KDICEPROTOCOL_H
#define KDICEPROTOCOL_H

//---------------------------------------------------------------------------
// He XUC XAC chia do (DICEITEM) - cau truc goi
// Dac ta: D:\GAMEDEVNEW\DACTA_HE_XUCXAC_DICEITEM.md
//
// VI SAO TACH RA MOT TEP RIENG (doc ky truoc khi gop nguoc lai):
//
// 1. Trong cay nay co HAI tep KProtocol.h khac nhau:
//        Sources\Core\Src\KProtocol.h   (ban that, Core + phan lon S3Client dung)
//        Headers\KProtocol.h            (ban ton dong, cu)
//    CA HAI cung dung mot include guard "KPROTOCOL_H", nen trong mot don vi
//    bien dich, ban nao vao TRUOC thi ban kia bi NUOT IM LANG.
//
// 2. Mot so tep cua client nap thang ban Headers bang duong dan tuong minh:
//        Sources\S3Client\NetConnect\NetConnectAgent.h:8
//        Sources\S3Client\Ui\UiCase\UiItem.cpp:29
//        Sources\S3Client\Ui\UiCase\UiPlayerBar.cpp:39
//
// 3. Hai ban dinh nghia SViewSellItemInfo LECH NHAU that su:
//        Headers  : short m_btMagicLevel[6]        , KHONG co m_nNature
//        Core\Src : int   m_btMagicLevel[MAX_ITEM_MAGICLEVEL] (=16), CO m_nNature
//    Lech 40 + 4 = 44 byte.
//
// => Cau truc goi KHONG duoc nhung bat cu kieu nao dinh nghia trong KProtocol.h.
//
// Vat pham dem ra chia dung 'ChatItem' - dinh nghia o Sources\Core\src\GameDataDef.h
// (dong 613), tep ma CA HAI ban KProtocol.h deu nap, nen bo cuc dong nhat o moi noi.
// Chon ChatItem con vi client DA CO san duong dung lai vat pham tu no:
// CoreShell.cpp GDI_ITEM_CHAT (dong 2226) goi ItemSet.Add / AddGoldItem / AddItemSet2
// roi ve bieu tuong + chu giai nhu vat pham binh thuong. Khong phai viet dinh dang moi.
//
// Tep nay co guard rieng (KDICEPROTOCOL_H) nen khong bi cuon vao vu va cham guard
// o tren, va duoc nap tu CA HAI ban KProtocol.h (sau GameDataDef.h).
//---------------------------------------------------------------------------

#pragma pack(push, enter_dice_protocol)
#pragma pack(1)

#define MAX_DICE_NAME_LEN       32      // do dai ten vat pham gui kem

// hanh dong ma goi s2c_diceitem yeu cau client lam
enum DICE_ACTION
{
    DICE_ACT_OPEN   = 0,        // mo mot o xuc xac moi
    DICE_ACT_CLOSE  = 1,        // dong o (het gio / da chot)
    DICE_ACT_NUMBER = 2,        // bao diem mot nguoi vua gieo
};

// lua chon nguoi choi gui len trong goi c2s_diceitem
enum DICE_CHOICE
{
    DICE_CHOICE_GIVEUP = 0,     // "Huy bo nhan"
    DICE_CHOICE_NEED   = 1,     // "Tham du nhan"
};

//---------------------------------------------------------------------------
// may chu -> client
//---------------------------------------------------------------------------
typedef struct
{
    BYTE        ProtocolType;                   // s2c_diceitem
    BYTE        m_btAction;                     // xem DICE_ACTION
    int         m_nDiceId;                      // ma phien xuc xac
    int         m_nTimeLeft;                    // so giay con lai
    int         m_nNumber;                      // diem vua gieo (khi m_btAction = DICE_ACT_NUMBER)
    char        m_szName[MAX_DICE_NAME_LEN];    // ten nguoi vua gieo (DICE_ACT_NUMBER)
    ChatItem    m_Item;                         // vat pham dem ra chia
} DICE_ITEM_SYNC;

//---------------------------------------------------------------------------
// client -> may chu
//---------------------------------------------------------------------------
typedef struct
{
    BYTE    ProtocolType;           // c2s_diceitem
    int     m_nDiceId;              // ma phien xuc xac
    BYTE    m_btChoice;             // xem DICE_CHOICE
} DICE_CHOICE_DATA;

#pragma pack(pop, enter_dice_protocol)

#endif  // KDICEPROTOCOL_H
