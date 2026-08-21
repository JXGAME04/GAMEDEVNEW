//---------------------------------------------------------------------------
// File     : JxReplay.h
// Muc dich : Tang dieu phoi he GHI / PHAT LAI ban dien .jxr phia client.
//            Nap JXReplay.dll, giu trang thai, bom nhip, va hien thuc 8 dong tu
//            cua ham Lua Replay(). Dung theo dung ban tham chieu.
//---------------------------------------------------------------------------
#ifndef _JXREPLAY_CLIENT_H_
#define _JXREPLAY_CLIENT_H_

struct IJXReplay;

// Nhip dong ho replay cua ban tham chieu (g_ReplayFPS = 30 tick/giay).
// Dinh dang .jxr co luoi thoi gian 3 tick, nen hinh hoc chi duoc ghi 10 lan/giay.
#define JXR_REPLAY_FPS		30

// Nguong dia trong bat buoc truoc khi bat dau ghi: 100 MiB.
#define JXR_MIN_FREE_BYTES	0x06400000

//--- Vong doi ------------------------------------------------------------
// Goi NGAY SAU khi g_pRepresentShell duoc tao xong (trong InitRepresentShell).
bool		JxReplay_Init();
// Goi TRUOC khi giai phong g_pRepresentShell.
void		JxReplay_Exit();

//--- Truy van ------------------------------------------------------------
IJXReplay*	JxReplay_Get();
int			JxReplay_GetState();		// tra JXREPLAY_STATUS, -1 neu chua nap DLL
bool		JxReplay_IsPlaying();		// state == 4
bool		JxReplay_IsRecording();		// state == 1 hoac 5

//--- Nhip ----------------------------------------------------------------
// Goi MOI VONG BOM: hoi lai trang thai tu DLL (tuong duong 0x0056E3BD).
void		JxReplay_Breathe();
// Goi MOI KHUNG LOGIC: tang bo dem khung 30Hz roi day (nTime, nStatus) xuong shell.
// KHONG goi ham nay thi se KHONG co gi duoc ghi va cung khong he bao loi.
void		JxReplay_OnGameFrame();

//--- Tam ngan dong tu (dung cho ham Lua Replay) --------------------------
// pszVerb: rec | endrec | play | stop | pause | speedup | slowdown | pauserec
void		JxReplay_DoVerb(const char* pszVerb);

// Mo hop thoai chon tep *.jxr roi phat. Tra true neu bat dau phat duoc.
bool		JxReplay_OpenFileAndPlay();

// Ban tham chieu tu ket thuc phien ghi khi nguoi choi bam thoat game.
// Tra true neu da NUOT lenh thoat (dang ghi hoac dang phat).
bool		JxReplay_OnQuitRequest();

#endif // _JXREPLAY_CLIENT_H_
