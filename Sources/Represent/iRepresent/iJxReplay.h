//---------------------------------------------------------------------------
// File     : iJxReplay.h
// Muc dich : Giao dien he GHI / PHAT LAI ban dien .jxr (thu vien ngoai jxreplay.dll)
//
// Day KHONG phai he quay video. jxreplay.dll ghi lai chinh cac LOI GOI PHUONG THUC
// cua iRepresentShell (lenh ve 2D + su kien am thanh) vao tep .jxr nen zlib,
// roi khi phat lai no GOI NGUOC vao mot giao dien ve co bo cuc vtable co dinh.
//
// Bo cuc vtable duoi day KHOP TUNG O voi jxreplay.dll (imagebase 0x10000000,
// vtable KJxReplay @ 0x10010174, 19 o). KHONG duoc them / bot / doi thu tu.
//
// LUU Y ABI:
//   - O 0 (Rec) la ham BIEN THAM SO nen MSVC dung __cdecl: con tro this duoc
//     DAY LEN NGAN XEP lam tham so trai nhat, va NGUOI GOI don ngan xep.
//     Doi chieu ban tham chieu: `push eax` (this) roi `call eax` roi `add esp,N`.
//   - 18 o con lai la __thiscall (this trong ecx, ham duoc goi don ngan xep),
//     khop voi `ret N` doc duoc trong tung than ham cua DLL.
//---------------------------------------------------------------------------
#ifndef _IJXREPLAY_H_
#define _IJXREPLAY_H_

//---------------------------------------------------------------------------
// Trang thai replay. jxreplay.dll chan cung gia tri > 5 trong SetStatus.
//---------------------------------------------------------------------------
enum JXREPLAY_STATUS
{
    JXR_STATUS_IDLE         = 0,    // ranh
    JXR_STATUS_RECORDING    = 1,    // dang ghi
    JXR_STATUS_ENDRECORDING = 2,    // xin ket thuc ghi (dang xa + nen)
    JXR_STATUS_LOADING      = 3,    // dang nap tep de phat
    JXR_STATUS_PLAYING      = 4,    // dang phat lai
    JXR_STATUS_PAUSEREC     = 5,    // tam dung ghi
};

//---------------------------------------------------------------------------
// Ma loai ban ghi. Chi 12 ma co bo ma hoa THAT o duong ghi trong jxreplay.dll;
// cac ma khac bi bo qua IM LANG (khong bao loi).
// Rieng JXRT_CLEARIMAGEDATA (4) duoc represent2 ban tham chieu phat ra nhung
// bo ghi VUT BO - giu lai de trung thanh voi ban tham chieu.
//---------------------------------------------------------------------------
enum JXREPLAY_RECTYPE
{
    JXRT_CREATEIMAGE            = 1,    // fmt "pnwh"
    JXRT_CLEARIMAGEDATA         = 4,    // fmt "pun"      (bo ghi vut bo)
    JXRT_DRAWPRIMITIVES         = 11,   // fmt "npub"
    JXRT_DRAWPRIMITIVESONIMAGE  = 12,   // fmt "npusim"
    JXRT_OUTPUTRICHTEXT         = 13,   // fmt "npscl"
    JXRT_OUTPUTTEXT             = 14,   // fmt "npcxyolzb"
    JXRT_LOOKAT                 = 17,   // fmt "xyz"
    JXRT_SETOPTION              = 20,
    JXRT_SETLIGHTINFO           = 21,
};

//---------------------------------------------------------------------------
// Ma tham so cho SetParam. Ban tham chieu goi SetParam TRUOC StartRec de bat
// che do chup trang thai ban dau; ca hai ma nay chi co tac dung khi status == 0.
//---------------------------------------------------------------------------
#define JXR_PARAM_TRACE_CREATEIMAGE     1
#define JXR_PARAM_TRACE_UNKNOWN11       11
#define JXR_PARAM_TRACE_DRAWPRIMONIMAGE 12

//---------------------------------------------------------------------------
// Giao dien chinh. 19 o, KHOP TUNG O voi jxreplay.dll.
//---------------------------------------------------------------------------
struct IJXReplay
{
    // +0x00  __cdecl bien tham so. pszFormat dung CHU CAI DON de chon truong;
    //        moi loai co tap chu cai rieng - KHONG the viet bo ma hoa tong quat.
    virtual void            Rec(int nTime, int nType, const char* pszFormat, ...) = 0;

    // +0x04  Bat dau ghi. szName toi da 31 ky tu (truong header chi 32 byte,
    //        DLL dung strcpy KHONG gioi han -> dai hon se de len cac truong sau).
    virtual void            StartRec(const char* szName, unsigned int dwParam) = 0;

    virtual void            PauseRec() = 0;                             // +0x08
    virtual void            EndRec(const char* szDestPath) = 0;         // +0x0C

    virtual int             Play(const char* szFile) = 0;               // +0x10
    virtual int             PlayTick() = 0;                             // +0x14  goi moi khung khi dang phat
    virtual int             GetTotalSize() = 0;                         // +0x18  mac dinh tra 1
    virtual int             GetLoadedSize() = 0;                        // +0x1C  mac dinh tra 0
    virtual void            RequestStopPlay() = 0;                      // +0x20
    virtual void            Pause() = 0;                                // +0x24
    virtual void            Resume() = 0;                               // +0x28

    // +0x2C / +0x30: HAI O NAY LA HAM RONG (than chi co 1 byte 0xC3 = ret)
    // trong jxreplay.dll ban tham chieu. Giu lai cho dung bo cuc; goi khong co tac dung.
    virtual void            SpeedUp() = 0;                              // +0x2C
    virtual void            SlowDown() = 0;                             // +0x30

    virtual int             GetStatus() = 0;                            // +0x34
    virtual void            SetParam(int nID, int nValue) = 0;          // +0x38

    // +0x3C  Cai giao dien VE. Bo giai ma khi phat kiem con tro nay TRUOC TIEN,
    //        nen khong cai no thi CA am thanh cung cam.
    virtual void            SetDrawInterface(void* pDrawInterface) = 0;

    virtual void            SetSoundInterface(void* pSoundInterface) = 0;   // +0x40
    virtual void            ReleaseInterface() = 0;                         // +0x44
    virtual void            SetStatus(int nStatus) = 0;                     // +0x48
};

//---------------------------------------------------------------------------
// Ham nha may duy nhat ma jxreplay.dll xuat khau (rva 0x1380).
// __cdecl, KHONG tham so, tra ve con tro singleton.
//---------------------------------------------------------------------------
typedef IJXReplay* (__cdecl *fnCreateJxReplayInterface)(void);

#define JXREPLAY_MODULE_NAME        "JXReplay.dll"
#define JXREPLAY_CREATE_FUN_NAME    "CreateJxReplayInterface"

// Thu muc tep tam BAT BUOC ton tai, cho CA chieu ghi LAN chieu phat.
// jxreplay.dll ghep cung: <thu muc chua EXE> + JXREPLAY_TEMP_SUBDIR + <timestamp>.jxr
// Thieu thu muc nay thi fopen hong va ca hai chieu that bai IM LANG.
#define JXREPLAY_TEMP_SUBDIR        "UserData\\Temp\\"

// Thu muc chua tep .jxr thanh pham (ban tham chieu: <thu muc EXE>\JxRep\).
#define JXREPLAY_SAVE_SUBDIR        "JxRep\\"

#endif // _IJXREPLAY_H_
