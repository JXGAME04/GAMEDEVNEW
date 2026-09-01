/*******************************************************************************
File        : UiDiceItem.h
create data : 26-08-2026
Description : He XUC XAC chia do (DICEITEM) - cua so ben client.

  Bo cuc lay nguyen tu ban Linux, hai tep .ini da chep nguyen van:
    "\xCD\xB6\xC9\xAB\xD7\xD3-\xB9\xAB\xD3\xC3.ini"  (312x340) - khung chua 4 O,
        cac muc [Img3] y=0, [Img2] y=86, [Img1] y=172, [Img0] y=259, moi o 312x83
    "\xCD\xB6\xC9\xAB\xD7\xD3.ini"      (312x83)  - noi dung MOT o:
        [objItem] 38x38 @(16,16) · [TimeInfo] @(80,28) · [BtnGreed] "Tham du nhan"
        63x19 @(100,52) · [BtnClose] "Huy bo nhan" @(200,52)

  Toa do trong tep thu hai la TUONG DOI so voi mot o, nen moi o duoc lam thanh mot
  KWndImage rieng va cac dieu khien la con cua no - khoi phai tu cong offset.

  Dac ta day du: D:\GAMEDEVNEW\DACTA_HE_XUCXAC_DICEITEM.md
*******************************************************************************/

#if !defined(AFX_UIDICEITEM_H__7F3A1C42_9D18_4E77_B6A1_2C55E9D0A311__INCLUDED_)
#define AFX_UIDICEITEM_H__7F3A1C42_9D18_4E77_B6A1_2C55E9D0A311__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "../elem/wndobjcontainer.h"
#include "../elem/wndlabeledbutton.h"
#include "../elem/wndimage.h"
#include "../elem/WndPage.h"
#include "../elem/wndtext.h"

#define DICE_ROW_COUNT      4       // khung chua dung 4 o (Img0..Img3)

class KUiDiceItem : public KWndImage
{
public:
    KUiDiceItem();
    virtual ~KUiDiceItem();

    static KUiDiceItem* OpenWindow();
    static KUiDiceItem* GetIfVisible();
    static void         CloseWindow(bool bDestory = TRUE);
    static void         LoadScheme(const char* pScheme);

    // Nhan goi s2c_diceitem. Tham so la DICE_ITEM_SYNC* - de kieu void* cho khoi
    // phai keo KProtocol.h vao tep .h nay (cay co HAI ban KProtocol.h dung chung
    // include guard, xem ghi chu dau Headers\KDiceProtocol.h).
    static void         OnDiceMsg(void* pSync);

private:
    static KUiDiceItem* ms_pSelf;

    void          Initialize();
    virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
    virtual void  Breathe();

    int           FindRow(int nDiceId);     // -1 neu khong co
    int           FindFreeRow();            // -1 neu day
    void          ClearRow(int nRow);       // nha vat pham + an o
    void          ShowRow(int nRow, int nDiceId, void* pChatItem, int nTimeLeft);
    void          RefreshTime(int nRow);
    void          SendChoice(int nRow, int nChoice);
    int           VisibleRowCount();

private:
    // [DICECLICK 01/09] PHAI la KWndPage chu KHONG phai KWndImage: KWndButton
    // chi bao WND_N_BUTTON_CLICK cho CHA TRUC TIEP (WndButton.cpp:326), ma hai
    // nut Can/Bo qua la CON cua m_Row[i] - de KWndImage thi thong bao chet tai
    // o hang, SendChoice khong bao gio chay, goi c2s_diceitem khong bao gio
    // duoc gui => bam "Can" cam lang, het 20 giay thanh "0 diem". KWndPage
    // (WndPage.cpp:20-28) sinh ra dung cho viec nay: chuyen tiep
    // WND_N_BUTTON_CLICK (va chuot) len cha - khop khuon PropertiePage goc.
    KWndPage          m_Row[DICE_ROW_COUNT];
    KWndObjectBox     m_ItemBox[DICE_ROW_COUNT];
    // PHAI la KWndText80 chu KHONG phai KWndText: KWndText de m_pText = NULL
    // va SetText() co chot `if (m_pText && ...)` nen KHONG LAM GI CA. Chi cac
    // lop dan xuat co san bo dem (KWndText32/80/256) moi dat duoc chu.
    // KUiTimeBox dung KWndText32; chuoi "Thoi gian con lai %d giay" ~25 byte
    // TCVN3 nen lay 80 cho rong rai.
    KWndText80        m_Time[DICE_ROW_COUNT];
    KWndLabeledButton m_BtnNeed[DICE_ROW_COUNT];
    KWndLabeledButton m_BtnGiveUp[DICE_ROW_COUNT];

    int   m_nDiceId[DICE_ROW_COUNT];    // 0 = o trong
    int   m_nItemIdx[DICE_ROW_COUNT];   // chi so vat pham DUNG TAM o mang Item[]
    int   m_nFrame[DICE_ROW_COUNT];     // so KHUNG HINH con lai (18 khung = 1 giay)
    char  m_szTimeFmt[128];             // chuoi dinh dang lay tu [TimeInfo] Text=
};

#endif
