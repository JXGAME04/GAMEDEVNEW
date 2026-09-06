//===========================================================================
// KBiaoChe.h - [LMBC 06/09] Long Mon Tieu Cuc: xe tieu bam theo chu.
//
// LUAT KIEN TRUC - DOC TRUOC KHI SUA:
//   NPC chi duoc Activate khi VUNG cua no dang co nguoi choi (KRegion::IsActive).
//   Vi vay:
//     * AI cua xe (KNpcAI::ProcessAIFollow) chi lo NGAN HAN: bam sat, di chuyen.
//     * VONG DOI (het han, lac chu, keo qua ban do, don xac) BAT BUOC chay o
//       BC_Breathe - nhip toan cuc, KHONG phu thuoc vung co nguoi hay khong.
//   Moi thiet ke dat dong ho 30 phut / 5 phut trong KNpcAI deu CHET CUNG khi
//   chu di sang ban do khac va vung cua xe vang nguoi.
//
// CAC HANG SO LAY TU NHI PHAN BAN LINUX (jx_linux_y), giu dung kieu so sanh.
//===========================================================================
#ifndef _KBIAOCHE_H_
#define _KBIAOCHE_H_

#ifdef _SERVER

#define BC_MAX_CART         256         // so xe song cung luc
#define BC_FOLLOW_DIST2     46224       // Linux 0xb490 - so sanh CO DAU (jg)
#define BC_FAR_DIST2        262143u     // Linux 0x3ffff - so sanh KHONG DAU (ja)
#define BC_LOST_TICK        5400        // 300 giay o 18 tick/giay
#define BC_DEFAULT_LIVE     36000       // = MAX_FIND_PATH_NPC_TIME cua KNpcAI.cpp
#define BC_CORPSE_TICK      1200        // ~66 giay: xac chua duoc go thi ta tu don
#define BC_RETRY_TICK       18          // 1 giay: gian cach thu lai ChangeWorld
#define BC_AI_TICK          3           // ~6 lan/giay (ban goc chay 18 lan/giay)
#define BC_SCRIPT_DEFAULT   "\\script\\event\\lmbiaoche\\npc_lmbiaoche.lua"

extern int  g_nBiaoCheOn;           // cong tat nong (BC_SetEnable)
extern int  g_nBiaoCheGiuKhiThoat;  // 1 = giu xe 5 phut sau khi chu dang xuat (nhu Linux)

int     BC_GetCart(int nPlayerIdx);         // -> npc idx, 0 = khong co
int     BC_GetOwnerIdx(int nNpcIdx);        // -> player idx, 0 = chu khong online
BOOL    BC_Attach(int nPlayerIdx, int nNpcIdx);
void    BC_UnlinkPlayer(int nNpcIdx);       // chi cat the ben KPlayer
void    BC_HardRemove(int nNpcIdx);         // go NPC theo khuon nha (chong DecRef kep)
void    BC_FireCartScript(int nNpcIdx, char* szFun, char* szOwnerName);
void    BC_Breathe();                       // CoreServerShell.cpp nhip chinh
void    BC_OnPlayerLogout(int nPlayerIdx);  // KPlayerSet::PrepareRemove

#endif  // _SERVER
#endif  // _KBIAOCHE_H_
