#ifndef	KRegionH
#define	KRegionH

//-----------------------------------------------------------------------
#include "KEngine.h"
#include "KWorldMsg.h"

//-----------------------------------------------------------------------
#ifdef _SERVER
// [BC 03/09] huong 1 (chu chon 03/09 dem): tran nguoi nhan cho goi MOT LAN (phat chieu, trung don, chay, doi phe)
// 100 -> 500 = bang NPC_SYNC_BROADCAST_LIMIT. Sau F4 (chi tru khi that gui) tran nay chi con cham khi > 500
// NGUOI THAT trong 9 vung; truoc do 258 bot ria da an sach 100 -> nguoi that khong thay chieu (do 04/09: 366 chieu/95 NPC
// trong khi 4.991 lenh chay/405 NPC). Chi phi: goi chieu ~30 B x so nguoi that, khong dang ke.
// [BC 03/09 c] TRA VE 100: do that cho thay cat_vi_het_ngan_sach = 0, tuc tran nay KHONG cat nguoi that.
// Nang len 500 chi go them mot van an toan chu khong sua gi. Giu 100 lam muc chan cuoi.
// [DELTA 07/09] (chu game 07/09: ban test, khong gioi han trai nghiem) tran nguoi nhan doc tu config.ini [Server]
// BroadCastMotLan (su kien mot lan) / BroadCastDongBo (goi dong bo 77/75), mac dinh 100000 = KHONG cat ai. Chi phi
// that bi chan boi so NGUOI THAT trong tam 32 o (bot khong an suat tu F4 04/09). Ban Linux tham chieu: 100 / 1200.
// Muon ve tran cu: BroadCastMotLan=100, BroadCastDongBo=500 (khong can build lai).
int BC_TranMotLan();
int BC_TranDongBo();
#define	MAX_BROADCAST_COUNT		BC_TranMotLan()
// Tran cho duong dong bo dinh ky (NormalSync). Ban goc = 100; chu game chon 500.
#define	NPC_SYNC_BROADCAST_LIMIT	BC_TranDongBo()
// [S11 26/08] Goi su kien MOT LAN (chet s2c_npcdeath, go s2c_npcremove): hiem (vai
// cu/giay ca tran) nhung MAT goi la client giu MA - danh vao khong khi, xac 0 mau
// van di, bung ve trai. Gia tri > so nguoi toi da nghia la KHONG CAT AI; chi phi
// thuc bi chan boi so node trong danh sach region, khong phai boi so nay.
#define	NPC_EVENT_BROADCAST_LIMIT	100000
#else
#define	MAX_REGION		9						//toi da load 9 region vung xung quanh
#endif
#define	REGION_GRID_WIDTH	16
#define	REGION_GRID_HEIGHT	32
// [PORT5 23/08] o trap khong co tham so JX2 (trap JX1/map-data): main(nPlayerIdx) nhu cu
#define JX2TRAP_PARAM_NONE	0x7FFFFFFF
#define CORRECT_SYNC_RANGE		30								// 同步矫正的距离

enum MOVE_OBJ_KIND
{
	obj_npc,
	obj_object,
	obj_obstacle,
	obj_missle,
};
#ifndef TOOLVERSION
class KRegion
#else
class CORE_API KRegion
#endif
{
	friend class	KSubWorld;
public:
	int			m_nIndex;							//
	int			m_RegionID;							//
	KList		m_NpcList;							// 人物列表
	KList		m_ObjList;							// 物件列表
	KList		m_MissleList;						// 子弹列表
	KList		m_PlayerList;						// 玩家列表
	int			m_nConnectRegion[8];				// 相邻的地图索引
	int			m_nConRegionID[8];					// 相邻的地图ID
	int			m_nRegionX;							// 在世界中的位置X（象素点）
	int			m_nRegionY;							// 在世界中的位置Y（象素点）
	int			m_nWidth;
	int			m_nHeight;
	int			m_nNpcSyncCursor; // Index in list, persistent across frames
	// Diem bat dau duyet danh sach nguoi choi khi phat dong bo. Xoay moi lan phat de
	// khong ai bi bo doi khi so nguoi trong vung vuot tran NPC_SYNC_BROADCAST_LIMIT.
	int			m_nBroadCastCursor;
	DWORD		m_dwLucCoNguoiVao;	// [DELTA 07/09] GetTickCount() lan cuoi co nguoi vao vung (AddPlayer); NormalSync xem 9 vung
private:
#ifdef _SERVER
	long		m_Obstacle[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];	// 地图障碍信息表
	DWORD		m_dwTrap[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];	// 地图trap信息表
	// [PORT5 23/08] tham so trap JX2 (AddMapTrap tham so 5 - Linux KRegion trap = {scriptId, nParam})
	int			m_nTrapParam[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];
#endif
	int			m_nNpcSyncCounter;					// 同步计数器
	int			m_nObjSyncCounter;
	int			m_nActive;							// 是否激活（是否有玩家在附近）
	BYTE*		m_pNpcRef;							// 每个格子上的NPC数目
	BYTE*		m_pObjRef;							// 每个格子上的OBJ数目
	BYTE*		m_pMslRef;							// 每个格子上的MISSLE数目
	BYTE*		m_pObstacleRef;
public:
	KRegion();
	~KRegion();
	BOOL		Init(int nWidth, int nHeight);
	BOOL		Load(int nX, int nY);
	void ClearRefGrid();
#ifdef _SERVER

	// 载入服务器端地图上本region 的 object数据（包括npc、trap、box等）
	BOOL		LoadObject(int nSubWorld, int nX, int nY);
	// 载入服务器端地图上本 region 的障碍数据
	BOOL		LoadServerObstacle(KPakFile *pFile, DWORD dwDataSize);
	// 载入服务器端地图上本 region 的 trap 数据
	BOOL		LoadServerTrap(KPakFile *pFile, DWORD dwDataSize);
	// 载入服务器端地图上本 region 的 npc 数据
	BOOL		LoadServerNpc(int nSubWorld, KPakFile *pFile, DWORD dwDataSize);
	// 载入服务器端地图上本 region 的 obj 数据
	BOOL		LoadServerObj(int nSubWorld, KPakFile *pFile, DWORD dwDataSize);
	void		RenameNpc(char* &sNpcCell);//edit by phong kieu khai bao ham rename NPC
	int         DelAllNpc(int mSubWorldID = 0, char* szName = NULL);
#endif

#ifndef _SERVER
	// 载入客户端地图上本region 的 object数据（包括npc、box等）
	// 如果 bLoadNpcFlag == TRUE 需要载入 clientonly npc else 不载入
	BOOL		LoadObject(int nSubWorld, int nX, int nY, char *lpszPath);
	// 载入客户端地图上本 region 的 clientonlynpc 数据
	BOOL		LoadClientNpc(KPakFile *pFile, DWORD dwDataSize);
	// 载入客户端地图上本 region 的 clientonlyobj 数据
	BOOL		LoadClientObj(KPakFile *pFile, DWORD dwDataSize);
	// 载入障碍数据给小地图
	static void		LoadLittleMapData(int nX, int nY, char *lpszPath, BYTE *lpbtObstacle);
#endif
	void		Close();
	void		Activate();
	// [QUAICHAN 11/09] them nIdxDiChuyen (mac dinh 0 = KHONG kiem quai, moi cho goi cu giu nguyen).
	BYTE		GetBarrier(int MapX, int MapY, int nDx, int nDy, int nIdxDiChuyen = 0);	//	地图高度

	// 按 像素点坐标 * 1024 的精度判断某个位置是否障碍
	// 参数：nGridX nGirdY ：本region格子坐标
	// 参数：nOffX nOffY ：格子内的偏移量(像素点 * 1024 精度)
	// 参数：bCheckNpc ：是否判断npc形成的障碍
	// 返回值：障碍类型(if 类型 == Obstacle_NULL 无障碍)
	BYTE		GetBarrierMin(int nGridX, int nGridY, int nOffX, int nOffY, BOOL bCheckNpc);

	DWORD		GetTrap(int MapX, int MapY);						//	得到Trap编号
	// [PORT5 23/08] tham so trap JX2 theo o; JX2TRAP_PARAM_NONE khi khong dat (than co #ifdef nhu GetTrap)
	int			GetTrapParam(int nMapX, int nMapY);
	void		SetTrapParam(int nMapX, int nMapY, int nParam);
	void		ClearAllTraps();
	inline BOOL		IsActive() 
	{
#ifdef _SERVER
		return m_nActive; 
#else
		return TRUE;
#endif
	};
	inline bool		IsInBounds(int nMapX, int nMapY) const;
	int			GetRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType);
#ifdef _SERVER
	// [QUAICHAN 11/09] o (nMapX,nMapY) co QUAI KHAC (kind_normal, con song) dang dung khong?
	// Dung m_pNpcRef lam bo loc nhanh: o trong (truong hop thuong gap) thoat ngay.
	int			CoQuaiKhacTrenO(int nMapX, int nMapY, int nBoQuaIdx);
#endif
	BOOL		AddRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType);
	BOOL		DecRef(int nMapX, int nMapY, MOVE_OBJ_KIND nType);
	int			FindNpc(int nMapX, int nMapY, int nNpcIdx, int nRelation, int nPreferIdx = 0);
	int			FindNpcDuyet(int nMapX, int nMapY, int nNpcIdx, int nRelation, int nPreferIdx);	// [VUNG 09/09] duong duyet danh sach cu (may chu dung; client dung de do lech)
	int			FindEquip(int nMapX, int nMapY);
	int			FindObject(int nMapX, int nMapY, bool bAutoFind = false);
	int			FindObject(int nObjID);
	void*		GetObjNode(int nIdx);
	int			SearchNpc(DWORD dwNpcID);		// 寻找本区域内是否有某个指定 id 的 npc (zroc add)
	int			SearchNpcSettingIdx(int  nNpcSettingIdx);
	int			SearchNpcName(const char* szName);
	int			SearchNpcID(DWORD dwID);
#ifdef _SERVER
	void		SendSyncData(int nClient);
	void		BroadCast(const void *pBuffer, DWORD dwSize, int &nMaxCount, int nX, int nY, int nBoNguoi1 = -1, int nBoNguoi2 = -1);	// [DELTA 07/09 g] nBoNguoi = chi so Player KHONG gui (ho nhan goi rieng)
	int			FindPlayer(DWORD dwId);
	BOOL		CheckPlayerIn(int nPlayerIdx);
	void		SetTrap(DWORD nTrapId, int nMapX, int nMapY);
	void		SetObstacle(long value, int nSubWorld, int nMapX, int nMapY);
	// DOT E (E4 fix CHAN-1): doc/ghi TRUC TIEP mot o vat can cho vat can dong
	// cong thanh - SetObstacle o tren chi ghi khi o == 0 nen KHONG BAO GIO
	// go duoc (value 0 khong de len 1); di kem luu-gia-tri-cu o KJx2WarInfra.
#ifdef _SERVER
	// [DECHONG 04/09] so NPC dang dung tren mot o. Bo dem m_pNpcRef van duoc AddRef/DecRef
	// day du ngay ca khi g_nPbNpcChan = 0 (nguoi/bot khong chan duong nhau).
	int		GetNpcCell(int nMapX, int nMapY)
	{
		if (!m_pNpcRef || nMapX < 0 || nMapY < 0 || nMapX >= m_nWidth || nMapY >= m_nHeight)
			return 0;
		return (int)m_pNpcRef[nMapY * m_nWidth + nMapX];
	}
	long		GetObstacleCell(int nMapX, int nMapY)
	{
		if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
			return 0;
		return m_Obstacle[nMapX][nMapY];
	}
	void		SetObstacleCell(int nMapX, int nMapY, long nValue)
	{
		if (nMapX < 0 || nMapY < 0 || nMapX >= REGION_GRID_WIDTH || nMapY >= REGION_GRID_HEIGHT)
			return;
		m_Obstacle[nMapX][nMapY] = nValue;
	}
#endif //#Set V藅 C秐
#endif

#ifndef _SERVER
	void		Paint();
	// [VUNG 09/09] chi muc o -> NPC cho FindNpc: dan (CheckNearestCollision 9 o, ProcessCollision (2R+1)^2 o) hoi hang nghin
	// o moi tick, moi lan duyet ca m_NpcList (130 NPC) = 1,2 trieu lan doc Npc[] / tick (do [WORLD-TICK] 14:28: dan 20-35 ms).
	// Xay lai khi m_uNpcDoi != m_uNpcChiMuc (NPC vao/ra vung, doi o). Thu tu trong o = thu tu m_NpcList => y het duyet.
	unsigned	m_uNpcDoi;		// tang o AddNpc/RemoveNpc/AddRef/DecRef(obj_npc)/Init/Close
	unsigned	m_uNpcChiMuc;	// phien ban chi muc dang giu
	int		m_nChiMucO;		// so o + 1 cua m_pChiMucODau (de biet khi kich thuoc vung doi)
	int*		m_pChiMucODau;	// [so o + 1]: o k giu m_pChiMucNpc[ODau[k] .. ODau[k+1])
	int*		m_pChiMucKe;		// [so o] con tro dien tam khi xay
	int*		m_pChiMucNpc;		// [so NPC] chi so NPC, theo o roi theo thu tu danh sach
	int		m_nChiMucNpcCap;
	void		XayChiMucNpc();
#endif
	void		AddNpc(int nIdx);
	void		RemoveNpc(int nIdx);
	void		AddMissle(int nIdx);
	void		RemoveMissle(int nIdx);
	void		AddObj(int nIdx);
	void		RemoveObj(int nIdx);
	BOOL		AddPlayer(int nIdx);
	BOOL		RemovePlayer(int nIdx);
    
};

//--------------------------------------------------------------------------
//	Find Npc
//--------------------------------------------------------------------------
#ifndef _SERVER
extern int g_nCorePaintLog;
extern unsigned g_uVungSo, g_uVungLech, g_uVungXay;	// [VUNG 09/09] bo do: so lan hoi, so lan chi muc khac duyet, so lan xay
#endif
inline int KRegion::FindNpc(int nMapX, int nMapY, int nNpcIdx, int nRelation, int nPreferIdx)
{
#ifndef _SERVER
	// [VUNG 09/09] client: tra chi muc o thay vi duyet ca danh sach (cung thu tu, cung phep chon)
	if (nMapX < 0 || nMapY < 0 || nMapX >= m_nWidth || nMapY >= m_nHeight)
		return 0;
	if (m_pNpcRef[nMapY * m_nWidth + nMapX] == 0)
		return 0;
	if (m_uNpcChiMuc != m_uNpcDoi || !m_pChiMucODau)
		XayChiMucNpc();
	if (m_pChiMucODau)
	{
		const int nO = nMapY * m_nWidth + nMapX;
		int nFallback = 0, nKq = 0;
		bool bXong = false;
		for (int k = m_pChiMucODau[nO]; k < m_pChiMucODau[nO + 1] && !bXong; k++)
		{
			const int nIdx = m_pChiMucNpc[k];
			if (nIdx <= 0 || nIdx >= MAX_NPC || Npc[nIdx].m_MapX != nMapX || Npc[nIdx].m_MapY != nMapY)
				continue;	// phong ho: NPC doi o ma khong qua AddRef/DecRef (chi muc cu) -> bo qua nhu m_pNpcRef
			if (NpcSet.GetRelation(nNpcIdx, nIdx) & nRelation)
			{
				if (nPreferIdx <= 0) { nKq = nIdx; bXong = true; break; }	// con dau tien theo thu tu danh sach
				if (nIdx == nPreferIdx) { nKq = nIdx; bXong = true; break; }
				if (nFallback == 0)
					nFallback = nIdx;
				else if ((Npc[nFallback].m_Doing == do_death || Npc[nFallback].m_Doing == do_revive) &&
					Npc[nIdx].m_Doing != do_death && Npc[nIdx].m_Doing != do_revive)
					nFallback = nIdx;
			}
		}
		if (!bXong) nKq = nFallback;
		if (g_nCorePaintLog > 0)	// bo do: moi 64 lan so voi duyet cu
		{
			if ((++g_uVungSo & 63) == 0 && FindNpcDuyet(nMapX, nMapY, nNpcIdx, nRelation, nPreferIdx) != nKq)
				g_uVungLech++;
		}
		return nKq;
	}
#endif
	return FindNpcDuyet(nMapX, nMapY, nNpcIdx, nRelation, nPreferIdx);
}
inline int KRegion::FindNpcDuyet(int nMapX, int nMapY, int nNpcIdx, int nRelation, int nPreferIdx)
{
	// FIX 24/08: thieu chan CAN TREN => cac ham quet vung tam nhin lon co the doc tran m_pNpcRef.
	if (nMapX < 0 || nMapY < 0 || nMapX >= m_nWidth || nMapY >= m_nHeight)
		return 0;

	if (m_pNpcRef[nMapY * m_nWidth + nMapX] == 0)
		return 0;

	KIndexNode *pNode = NULL;
	// FIX 25/08: ham nay von tra ve NPC DAU TIEN trong danh sach vung nam o o do. Khi nhieu con
	// dung CHUNG mot o, nguoi goi chi thay DUY NHAT con dau tien; KMissle::CheckNearestCollision
	// xet con do that bai la BO LUON CA O, nen muc tieu that dung ngay canh khong bao gio duoc xet.
	// Do that 25/08: 1064/1102 vien dan hut la vi le nay (81 ca CUNG O, nDX=nDY=0, khong phep so
	// nao co the loai duoc - chi co the do con dau tien la XAC). nPreferIdx = muc tieu ma nguoi goi
	// DANG NHAM; neu no o trong o nay thi tra ve no. nPreferIdx = 0 (mac dinh) giu hanh vi CU Y NGUYEN.
	int nFallback = 0;
	
	pNode = (KIndexNode *)m_NpcList.GetHead();
	
	while(pNode)
	{
		if (Npc[pNode->m_nIndex].m_MapX == nMapX && Npc[pNode->m_nIndex].m_MapY == nMapY)
		{
			if (NpcSet.GetRelation(nNpcIdx, pNode->m_nIndex) & nRelation)
			{
				if (nPreferIdx <= 0)
					return pNode->m_nIndex;		// duong CU: con dau tien, khong doi mot bit nao
				if (pNode->m_nIndex == nPreferIdx)
					return pNode->m_nIndex;		// dung muc tieu dang nham
				if (nFallback == 0)
					nFallback = pNode->m_nIndex;
				else if ((Npc[nFallback].m_Doing == do_death || Npc[nFallback].m_Doing == do_revive) &&
					 Npc[pNode->m_nIndex].m_Doing != do_death && Npc[pNode->m_nIndex].m_Doing != do_revive)
					nFallback = pNode->m_nIndex;	// XAC khong duoc CHE mat con con song cung o
			}
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}	
	return nFallback;
}
#endif
