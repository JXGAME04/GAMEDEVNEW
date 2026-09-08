// [SDL 08/09 2b-2] Harness kiem Rainbow.dll qua dung giao dien IClient ma Game.exe dung (NetConnectAgent):
//   CreateInterface -> IClientFactory -> SetEnvironment -> CreateClientInterface(IID_IESClient) -> Startup
//   -> RegisterMsgFilter -> ConnectTo -> SendPackToServer / GetPackFromServer -> (may chu dong) -> Shutdown -> Cleanup -> Release
// Dung voi may_chu_gia.py. Chay: test_rainbow.exe <thu muc chua Rainbow.dll> <port> [so goi] [giay ap luc] [ip] [lan]
//   giay ap luc > 0 : che do AP LUC (may chu chay --day N): trong N giay, moi 50 ms gui "ping" (nhu lenh di chuyen), tham
//                     GetPackFromServer moi 1 ms nhu vong khung; do RTT ping (tb/max/p99), khoang cach den cua goi PUSH (max), goi PUSH mat.
//   giay ap luc = -1 : che do CHI KET NOI: ConnectTo(ip, port) [lan] lan (do thoi gian connect + bat tay khoa ACCOUNT_BEGIN) roi
//                     Shutdown/Cleanup ngay, khong gui gi - dung do tren may chu THAT (cong dang nhap / cong game) mot cach vo hai.
// Goi timeBeginPeriod(1) nhu Game.exe de tham 1 ms. Ma thoat 0 = PASS.
#define INITGUID
#include <windows.h>
#include <objbase.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "Interface/IClient.h"
#pragma comment( lib, "winmm.lib" )

typedef HRESULT ( __stdcall *pfnCreateClientInterface )( REFIID riid, void **ppv );

static volatile long g_nCreate = 0, g_nClose = 0;
static DWORD g_tCbThread = 0;

static void __stdcall Cb( LPVOID lpParam, const unsigned long &ev )
{
	g_tCbThread = GetCurrentThreadId();
	if ( ev == enumServerConnectCreate ) InterlockedIncrement( &g_nCreate );
	else if ( ev == enumServerConnectClose ) InterlockedIncrement( &g_nClose );
	printf( "  [cb] su kien 0x%lx (luong %lu)\n", ev, GetCurrentThreadId() );
}

static double Now() { static LARGE_INTEGER f = { 0 }; if ( !f.QuadPart ) QueryPerformanceFrequency( &f ); LARGE_INTEGER c; QueryPerformanceCounter( &c ); return ( double )c.QuadPart * 1000.0 / ( double )f.QuadPart; }

static bool DoiGoi( IClient *c, const char *mong, DWORD msToiDa, DWORD *msMat )
{
	DWORD t0 = GetTickCount();
	for ( ;; )
	{
		size_t n = 0;
		const char *p = ( const char * )c->GetPackFromServer( n );
		if ( n > 0 )
		{
			*msMat = GetTickCount() - t0;
			bool ok = ( n == strlen( mong ) && 0 == memcmp( p, mong, n ) );
			printf( "  nhan %u byte sau %lu ms: \"%.*s\" %s\n", ( unsigned )n, *msMat, ( int )n, p, ok ? "OK" : "SAI" );
			return ok;
		}
		if ( GetTickCount() - t0 > msToiDa ) { printf( "  KHONG nhan duoc goi sau %lu ms\n", msToiDa ); return false; }
		Sleep( 5 );
	}
}

// che do ap luc: tra ve so loi
static int ApLuc( IClient *c, int giay )
{
	std::vector<double> rtt; rtt.reserve( 4096 );
	double t0 = Now(), tPingCuoi = t0, tPushCuoi = 0, gapMax = 0; int nPing = 0, nPush = 0, nPushMat = 0, nGoiLa = 0, seqCuoi = 0; double bytes = 0;
	double gapPingMax = 0, tNhanCuoi = t0; int nGapLon = 0;
	std::vector<double> gapPush; gapPush.reserve( 4096 );
	printf( "  ap luc %d s: gui ping moi 50 ms, tham GetPackFromServer moi ~1 ms\n", giay );
	while ( Now() - t0 < giay * 1000.0 )
	{
		double now = Now();
		if ( now - tPingCuoi >= 50.0 )
		{
			char msg[64]; int len = _snprintf( msg, sizeof( msg ), "ping:%d:%.3f", ++nPing, now );
			c->SendPackToServer( msg, ( size_t )len ); tPingCuoi = now;
		}
		for ( int k = 0; k < 64; k++ )		// rut het goi dang co (nhu Breathe)
		{
			size_t n = 0; const char *p = ( const char * )c->GetPackFromServer( n );
			if ( 0 == n ) break;
			bytes += ( double )n; double tn = Now();
			if ( tn - tNhanCuoi > gapPingMax ) gapPingMax = tn - tNhanCuoi;
			tNhanCuoi = tn;
			if ( n > 10 && 0 == memcmp( p, "ECHO:ping:", 10 ) )
			{
				int id = 0; double ts = 0; if ( 2 == sscanf( p + 10, "%d:%lf", &id, &ts ) ) rtt.push_back( tn - ts );
			}
			else if ( n > 5 && 0 == memcmp( p, "PUSH:", 5 ) )
			{
				int seq = atoi( p + 5 ); nPush++;
				if ( seqCuoi && seq != seqCuoi + 1 ) nPushMat += ( seq - seqCuoi - 1 );
				seqCuoi = seq;
				if ( tPushCuoi > 0 ) { double g = tn - tPushCuoi; gapPush.push_back( g ); if ( g > gapMax ) gapMax = g; if ( g > 200.0 ) nGapLon++; }
				tPushCuoi = tn;
			}
			else nGoiLa++;
		}
		Sleep( 1 );
	}
	std::sort( rtt.begin(), rtt.end() ); std::sort( gapPush.begin(), gapPush.end() );
	double rttTb = 0; for ( size_t i = 0; i < rtt.size(); i++ ) rttTb += rtt[i]; if ( rtt.size() ) rttTb /= rtt.size();
	double rttP99 = rtt.size() ? rtt[( size_t )( rtt.size() * 0.99 )] : 0, rttMax = rtt.size() ? rtt.back() : 0;
	double gapP99 = gapPush.size() ? gapPush[( size_t )( gapPush.size() * 0.99 )] : 0;
	printf( "  KET QUA AP LUC: ping gui %d, hoi %u, RTT tb %.1f ms, p99 %.1f ms, max %.1f ms | PUSH nhan %d, mat %d, khoang cach p99 %.1f ms, max %.1f ms, >200 ms: %d lan | %.0f KB, goi la %d\n",
		nPing, ( unsigned )rtt.size(), rttTb, rttP99, rttMax, nPush, nPushMat, gapP99, gapMax, nGapLon, bytes / 1024.0, nGoiLa );
	int fails = 0;
	if ( ( int )rtt.size() < nPing - 2 ) { printf( "  FAIL: mat ping (%d/%d)\n", ( int )rtt.size(), nPing ); fails++; }
	if ( nPushMat > 0 ) { printf( "  FAIL: mat goi PUSH\n" ); fails++; }
	if ( rttMax > 1000.0 ) { printf( "  FAIL: RTT max > 1 s\n" ); fails++; }
	return fails;
}

int main( int argc, char **argv )
{
	if ( argc < 3 ) { printf( "dung: test_rainbow.exe <thu muc Rainbow.dll> <port> [so goi] [giay ap luc|-1 chi ket noi] [ip] [lan]\n" ); return 2; }
	const char *dir = argv[1]; unsigned short port = ( unsigned short )atoi( argv[2] ); int nGoi = argc > 3 ? atoi( argv[3] ) : 5; int giayApLuc = argc > 4 ? atoi( argv[4] ) : 0;
	const char *ip = argc > 5 ? argv[5] : "127.0.0.1"; int nLan = argc > 6 ? atoi( argv[6] ) : 1;
	char path[MAX_PATH]; _snprintf( path, MAX_PATH, "%s\\Rainbow.dll", dir );
	int fails = 0;
	timeBeginPeriod( 1 );

	SetDllDirectoryA( dir );
	HMODULE h = LoadLibraryExA( path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
	if ( !h ) { printf( "FAIL LoadLibrary %s (loi %lu)\n", path, GetLastError() ); return 1; }
	printf( "nap %s OK (luong chinh %lu)\n", path, GetCurrentThreadId() );
	pfnCreateClientInterface pfn = ( pfnCreateClientInterface )GetProcAddress( h, "CreateInterface" );
	if ( !pfn ) { printf( "FAIL khong co CreateInterface\n" ); return 1; }

	if ( giayApLuc < 0 )
	{
		// CHI KET NOI: nhu NetConnectAgent::ClientConnectByNumericIp / ConnectToGameSvr, roi ngat ngay
		for ( int lan = 1; lan <= nLan; lan++ )
		{
			IClientFactory *f = NULL; IClient *c = NULL;
			pfn( IID_IClientFactory, ( void ** )&f ); f->SetEnvironment( 1024 * 512 ); f->CreateClientInterface( IID_IESClient, ( void ** )&c );
			c->Startup(); c->RegisterMsgFilter( NULL, Cb );
			double t0 = Now();
			HRESULT hr = c->ConnectTo( ip, port );
			double t1 = Now();
			// cho toi da 300 ms xem may chu co gui gi them ngay sau bat tay khong (chi dem, khong xu ly)
			size_t nTong = 0; int nGoiNhan = 0; double t2 = t1;
			while ( Now() - t1 < 300.0 ) { size_t n = 0; c->GetPackFromServer( n ); if ( n ) { nTong += n; nGoiNhan++; t2 = Now(); } Sleep( 1 ); }
			printf( "  lan %d: ConnectTo %s:%u -> 0x%lx sau %.1f ms (connect + bat tay khoa); 300 ms sau nhan %d goi / %u byte (goi cuoi luc +%.1f ms)\n",
				lan, ip, port, hr, t1 - t0, nGoiNhan, ( unsigned )nTong, t2 - t1 );
			if ( FAILED( hr ) ) fails++;
			if ( t1 - t0 > 2000.0 ) { printf( "  CHAM: ket noi qua 2 s\n" ); fails++; }
			double t3 = Now(); c->Shutdown(); double t4 = Now(); c->Cleanup(); double t5 = Now(); c->Release(); f->Release(); double t6 = Now();
			printf( "  lan %d: Shutdown %.1f ms, Cleanup %.1f ms, Release %.1f ms\n", lan, t4 - t3, t5 - t4, t6 - t5 );
			Sleep( 500 );
		}
		FreeLibrary( h );
		printf( "%s (chi ket noi): %d loi\n", fails ? "FAIL" : "PASS", fails );
		return fails ? 1 : 0;
	}

	IClientFactory *f = NULL;
	HRESULT hr = pfn( IID_IClientFactory, ( void ** )&f );
	if ( FAILED( hr ) || !f ) { printf( "FAIL CreateInterface 0x%lx\n", hr ); return 1; }
	f->SetEnvironment( 1024 * 512 );
	IClient *c = NULL;
	hr = f->CreateClientInterface( IID_IESClient, ( void ** )&c );
	if ( FAILED( hr ) || !c ) { printf( "FAIL CreateClientInterface 0x%lx\n", hr ); return 1; }

	hr = c->Startup(); printf( "Startup -> 0x%lx\n", hr ); if ( FAILED( hr ) ) fails++;
	c->RegisterMsgFilter( NULL, Cb );

	if ( giayApLuc <= 0 )
	{
		// 1) ket noi sai cong: phai tra E_FAIL nhanh (khong treo)
		DWORD t0 = GetTickCount();
		hr = c->ConnectTo( "127.0.0.1", ( unsigned short )( port + 1 ) );
		printf( "ConnectTo cong sai -> 0x%lx sau %lu ms %s\n", hr, GetTickCount() - t0, FAILED( hr ) ? "(OK, mong that bai)" : "(SAI: le ra that bai)" );
		if ( !FAILED( hr ) ) fails++;
	}

	// 2) ket noi dung
	DWORD t0 = GetTickCount();
	hr = c->ConnectTo( ip, port );
	printf( "ConnectTo %s:%u -> 0x%lx sau %lu ms\n", ip, port, hr, GetTickCount() - t0 );
	if ( FAILED( hr ) ) { printf( "FAIL ket noi\n" ); return 1; }

	if ( giayApLuc > 0 )
	{
		fails += ApLuc( c, giayApLuc );
		c->Shutdown(); c->Cleanup(); c->Release(); f->Release(); FreeLibrary( h );
		printf( "%s (ap luc): %d loi\n", fails ? "FAIL" : "PASS", fails );
		return fails ? 1 : 0;
	}

	// 3) gui / nhan
	DWORD msTong = 0, msMax = 0;
	for ( int k = 1; k <= nGoi; k++ )
	{
		char msg[64]; int len = _snprintf( msg, sizeof( msg ), "xin chao %d", k );
		char mong[80]; _snprintf( mong, sizeof( mong ), "ECHO:%s", msg );
		hr = c->SendPackToServer( msg, ( size_t )len );
		if ( FAILED( hr ) ) { printf( "  FAIL SendPackToServer %d\n", k ); fails++; continue; }
		DWORD ms = 0;
		if ( !DoiGoi( c, mong, 2000, &ms ) ) fails++;
		msTong += ms; if ( ms > msMax ) msMax = ms;
	}
	// 3b) goi lon 9.000 byte (bo dem doc 10 KB) + goi 1 byte
	{
		static char big[9000]; for ( int i = 0; i < 9000; i++ ) big[i] = ( char )( 'A' + ( i % 26 ) );
		static char mong[9010]; memcpy( mong, "ECHO:", 5 ); memcpy( mong + 5, big, 9000 ); mong[9005] = 0;
		hr = c->SendPackToServer( big, 9000 );
		DWORD ms = 0; if ( !DoiGoi( c, mong, 3000, &ms ) ) fails++;
		hr = c->SendPackToServer( "Z", 1 );
		if ( !DoiGoi( c, "ECHO:Z", 2000, &ms ) ) fails++;
	}
	printf( "tre trung binh %lu ms, toi da %lu ms (%d goi)\n", nGoi ? msTong / nGoi : 0, msMax, nGoi );

	// 4) may chu dong -> callback enumServerConnectClose
	long closeTruoc = g_nClose;
	c->SendPackToServer( "BYE", 3 );
	t0 = GetTickCount();
	while ( g_nClose == closeTruoc && GetTickCount() - t0 < 4000 ) { size_t n = 0; c->GetPackFromServer( n ); Sleep( 10 ); }
	printf( "may chu dong -> callback Close %s sau %lu ms (luong callback %lu)\n", g_nClose > closeTruoc ? "CO" : "KHONG", GetTickCount() - t0, g_tCbThread );
	if ( g_nClose == closeTruoc ) fails++;

	// 5) don dep: Shutdown + Cleanup (join luong I/O) + Release
	t0 = GetTickCount();
	hr = c->Shutdown(); printf( "Shutdown -> 0x%lx\n", hr );
	hr = c->Cleanup();  printf( "Cleanup -> 0x%lx sau %lu ms\n", hr, GetTickCount() - t0 ); if ( FAILED( hr ) ) fails++;
	c->Release(); f->Release();

	// 6) tao lai client, ket noi lan 2 (kiem dung/hoi sinh luong + khoa)
	pfn( IID_IClientFactory, ( void ** )&f ); f->SetEnvironment( 1024 * 512 ); f->CreateClientInterface( IID_IESClient, ( void ** )&c );
	c->Startup(); c->RegisterMsgFilter( NULL, Cb );
	hr = c->ConnectTo( ip, port ); printf( "ket noi lan 2 -> 0x%lx\n", hr ); if ( FAILED( hr ) ) fails++;
	else { DWORD ms = 0; c->SendPackToServer( "lan2", 4 ); if ( !DoiGoi( c, "ECHO:lan2", 2000, &ms ) ) fails++; }
	c->Shutdown(); c->Cleanup(); c->Release(); f->Release();

	FreeLibrary( h );
	printf( "%s: %d loi, callback create=%ld close=%ld\n", fails ? "FAIL" : "PASS", fails, g_nCreate, g_nClose );
	return fails ? 1 : 0;
}
