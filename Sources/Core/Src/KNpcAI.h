#ifndef	KNpcAIH
#define	KNpcAIH

#include "KMath.h"

class	KNpcAI
{
	int				m_nIndex;
	BOOL			m_bActivate;
#ifndef _SERVER
#endif
	int				m_nPosActive;

private:
public:
	KNpcAI();	
	void			Activate(int nIndex);
	void			NotActivate(int nIndex);
	void			Enable(){ m_bActivate = TRUE; }
	void			Disable(){ m_bActivate = FALSE; }	
private:	
	int				GetNearestNpc(int nRelation);
	int				GetNpcNumber(int nRelation);
	void			ProcessPlayer();
#ifdef _SERVER
	void			ProcessAIFollow();
#endif
	void			ProcessAIType1();
	void			ProcessAIType2();
	void			ProcessAIType3();
	void			ProcessAIType4();
	void			ProcessAIType5();
	void			ProcessAIType6();
	void			ProcessAIType7();
	void			ProcessAIType8();
	void			ProcessAIType9();
	void			ProcessAIType10();
	void			ProcessAIType01();		// 普通主动类1
	void			ProcessAIType02();		// 普通主动类2
	void			ProcessAIType03();		// 普通主动类3
	void			ProcessAIType04();		// 普通被动类1
	void			ProcessAIType05();		// 普通被动类2
	void			ProcessAIType06();		// 普通被动类3
#ifdef _SERVER
	// [AI710L 07/09] AI 7/8/9/10 dich tu nhi phan Linux jx_linux_y - xem PHANTICH_CAIBANG_DAN_VA_QUAI_DOT2_0609.md Phan F.3.
	// Bon ham ProcessAIType7..10 o tren la thuat toan KHAC ban Linux nen KHONG con duoc Activate goi.
	void			AI07_XungXa();			// Linux 0x08094040: xung xa tim NPC co m_dwID == p0 roi pha
	void			AI08_ThachXa();			// Linux 0x0808F1C0: dau thach xa ban vao o vuong canh p2 quanh (p0,p1)
	void			AI09_HanhQuan();		// Linux 0x08092E30: hanh quan toi (p7,p8) / bam thu linh p7 / p0% danh dich
	void			AI10_DungBan();			// Linux 0x08091EB0: dung yen, chi ra chieu khi dich trong tam
	void			AI_BoMucTieuLinux();	// nhip AI Linux 0x0808C640: bo muc tieu chet/hoi sinh/nguoi choi chua bat chien dau
	int				AI_TimDichNgauNhien();	// Linux 0x0808DBA0: gom toi da 10 dich trong tam nhin, boc 1
	int				AI_TimNpcTheoIdGan(DWORD dwID);	// Linux 0x0807A1F0: NPC co m_dwID trong vung hien tai + 8 vung ke
	void			AI_BanTaiCho(int nEnemy);	// Linux 0x0808F360: ra chieu tai cho, khong di
#endif
	void			TriggerObjectTrap();
	void			TriggerMapTrap();
	void			FollowAttack(int nIdx);
	BOOL			InEyeshot(int nIdx);
	void			CommonAction();
	BOOL			KeepActiveRange();
	void			KeepAttackRange(int nEnemy, int nRange);
	void			Flee(int nIdx);
	BOOL			CheckNpc(int nIndex);
#ifndef _SERVER
	void			FollowPeople(int nIdx);
	void			FollowObject(int nIdx);
#endif
	friend class KNpc;
#ifndef _SERVER
	
	int				ProcessShowNpc();// flying add these on Jun.4.2003
	int  			ShowNpcType11();
	int				ShowNpcType12();
	int				ShowNpcType13();
	int				ShowNpcType14();
	int				ShowNpcType15();
	int				ShowNpcType16();
	int				ShowNpcType17();
	int             GetNpcMoveOffset(int nDir, int nDistance, int *pnX, int *pnY);

	int				DoShowFlee(int nIdx);

	BOOL			CanShowNpc();
#endif

	int			IsPlayerCome();
};

#if !defined _SERVER
inline int KNpcAI::GetNpcMoveOffset(int nDir, int nDistance, int *pnX, int *pnY)
{
    _ASSERT(pnX);
    _ASSERT(pnY);

    *pnX = -nDistance * g_DirSin(nDir, 64);
    *pnY = -nDistance * g_DirCos(nDir, 64);

    return true;
}

inline BOOL KNpcAI::CanShowNpc()
{
	BOOL bResult = TRUE;
	if (Npc[m_nIndex].m_AiParam[5] < Npc[m_nIndex].m_AiParam[4])
		bResult = FALSE;
	Npc[m_nIndex].m_AiParam[5]++;
	return bResult;
}
#endif

extern KNpcAI NpcAI;
#endif
