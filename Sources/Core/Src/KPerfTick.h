//---------------------------------------------------------------------------
// KPerfTick.h - [PerfLog 24/08] Do thoi gian TUNG GIAI DOAN trong mot tick server.
//
// Vi sao can: da do that tren may chay (24 loi) - GameServer co 73 luong nhung
// 100% CPU nam tren DUNG MOT luong (luong chinh chay Breathe/MainLoop), 72 luong
// con lai 0%. Muon biet co nen dong them cong suc vao dau thi phai biet trong
// 55ms cua mot tick (GAME_FPS 18) thoi gian di dau. Bo do nay chia tick thanh
// cac giai doan doc lap va bao cao dinh ky.
//
// Nguyen tac:
//  - TAT theo mac dinh. Bat bang config.ini muc [PerfLog] On=1 (giong [AutoLog]).
//  - Khi TAT: chi phi = doc mot bien static + so sanh, khong goi dong ho.
//  - KHONG in ra cua so GameServer theo mac dinh (Console=0) - ghi ra
//    jx_perf_server.log. Giu tep MO SAN, flush theo lo, xoay tep khi qua lon
//    (cung ky thuat da dung o g_AutoLog, KCore.cpp:806-840).
//  - Khong dung den bat ky trang thai game nao => khong the doi hanh vi game.
//
// Cach doc bao cao (mot dong cho moi giai doan, moi IntervalSec giay):
//    [PERF] tick=1080 tre=12 (1.1%) online=873
//      TICK          n=1080 tb=18.4ms max=142.7ms p95=41.0ms tong=53.2%
//      SW_ACTIVATE   n=1080 tb=15.1ms max=131.2ms p95=36.8ms tong=43.6%
//  tb  = trung binh, max = lan lau nhat, p95 = 95% so lan nhanh hon muc nay,
//  tong= phan tram thoi gian thuc te da tieu (so voi do dai khoang bao cao),
//  tre = so tick vuot TreMs (mac dinh 55ms = mot nhip 18 FPS).
//---------------------------------------------------------------------------
#ifndef KPerfTick_H
#define KPerfTick_H
//---------------------------------------------------------------------------

// Danh sach giai doan. Them muc moi thi them ca ten o KPerfTick.cpp (s_szStage).
enum KPERF_STAGE
{
	PERF_TICK = 0,			// toan bo CoreServerShell::Breathe (mot tick logic)
	PERF_SCRIPT_TIME,		// pTimeScript RunTime (script gio)
	PERF_GLBMISSION,		// KJx2GlbMission_Breathe
	PERF_CITYWAR,			// KJx2CityWar_Breathe
	PERF_SW_MSGLOOP,		// g_SubWorldSet.MessageLoop
	PERF_SW_MAINLOOP,		// g_SubWorldSet.MainLoop (tong)
	PERF_SW_ACTIVATE,		// rieng vong SubWorld[i].Activate()
	PERF_AUTOSAVE,			// PlayerSet.AutoSave
	PERF_BAUCUA,			// g_BauCua.run
	PERF_GS_MSGLOOP,		// (GameServer.exe) KSwordOnLineSever::MessageLoop
	PERF_GS_MAINLOOP,		// (GameServer.exe) KSwordOnLineSever::MainLoop
	PERF_COUNT
};

#ifdef _SERVER

// 1 = dang bat. Doc config.ini MOT LAN o lan goi dau tien.
int		g_PerfOn();
// Dong ho do phan giai cao, don vi mili-giay.
double	g_PerfNowMs();
// Cong mot lan do vao giai doan nStage.
void	g_PerfAdd(int nStage, double dMs);
// Goi DUNG MOT LAN moi tick, o cuoi tick: chot khung, in bao cao khi den han.
void	g_PerfFrame(int nOnlinePlayer);
// Khoi luong cua tick vua roi: so region dang hoat dong, so NPC nam trong cac
// region do. Co hai so nay moi biet tick cham la vi DONG hay vi mot cho nao do
// cham bat thuong (thoi gian tang ma khoi luong khong tang = loi thuat toan).
void	g_PerfCount(int nRegion, int nNpc);
// Bat/tat luc dang chay (danh cho lenh GM sau nay). nOn = 0/1.
void	g_PerfSet(int nOn);

//---------------------------------------------------------------------------
// KPerfScope - do tu luc tao den luc huy (RAII). Khi TAT thi khong goi dong ho.
//---------------------------------------------------------------------------
class KPerfScope
{
public:
	KPerfScope(int nStage)
	{
		m_nStage = nStage;
		m_dStart = g_PerfOn() ? g_PerfNowMs() : -1.0;
	}
	~KPerfScope()
	{
		if (m_dStart >= 0.0)
			g_PerfAdd(m_nStage, g_PerfNowMs() - m_dStart);
	}
private:
	int		m_nStage;
	double	m_dStart;
};

#define PERF_CAT2(a, b)	a##b
#define PERF_CAT(a, b)	PERF_CAT2(a, b)
#define PERF_SCOPE(x)	KPerfScope PERF_CAT(_perf_, __LINE__)(x)

#else	// !_SERVER (client): bo do khong ton tai, moi thu tan bien khi bien dich

#define PERF_SCOPE(x)		((void)0)
#define g_PerfCount(r, n)	((void)0)

#endif	// _SERVER
//---------------------------------------------------------------------------
#endif
