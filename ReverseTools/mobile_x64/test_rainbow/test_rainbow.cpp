// [SDL 08/09 2b-2] Harness kiem Rainbow.dll qua dung giao dien IClient ma Game.exe dung (NetConnectAgent):
//   CreateInterface -> IClientFactory -> SetEnvironment -> CreateClientInterface(IID_IESClient) -> Startup
//   -> RegisterMsgFilter -> ConnectTo -> SendPackToServer / GetPackFromServer -> (may chu dong) -> Shutdown -> Cleanup -> Release
// Dung voi may_chu_gia.py. Chay: test_rainbow.exe <thu muc chua Rainbow.dll> <port> [so goi]
// Ma thoat 0 = PASS. Dung duoc cho ca Rainbow.dll Win32 thuong (bin\client64) va ban SDL (bin\client64sdl) de so sanh.
#define INITGUID
#include <windows.h>
#include <objbase.h>
#include <stdio.h>
#include <string.h>
#include "Interface/IClient.h"

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

int main( int argc, char **argv )
{
	if ( argc < 3 ) { printf( "dung: test_rainbow.exe <thu muc Rainbow.dll> <port> [so goi]\n" ); return 2; }
	const char *dir = argv[1]; unsigned short port = ( unsigned short )atoi( argv[2] ); int nGoi = argc > 3 ? atoi( argv[3] ) : 5;
	char path[MAX_PATH]; _snprintf( path, MAX_PATH, "%s\\Rainbow.dll", dir );
	int fails = 0;

	SetDllDirectoryA( dir );
	HMODULE h = LoadLibraryExA( path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
	if ( !h ) { printf( "FAIL LoadLibrary %s (loi %lu)\n", path, GetLastError() ); return 1; }
	printf( "nap %s OK (luong chinh %lu)\n", path, GetCurrentThreadId() );
	pfnCreateClientInterface pfn = ( pfnCreateClientInterface )GetProcAddress( h, "CreateInterface" );
	if ( !pfn ) { printf( "FAIL khong co CreateInterface\n" ); return 1; }

	IClientFactory *f = NULL;
	HRESULT hr = pfn( IID_IClientFactory, ( void ** )&f );
	if ( FAILED( hr ) || !f ) { printf( "FAIL CreateInterface 0x%lx\n", hr ); return 1; }
	f->SetEnvironment( 1024 * 512 );
	IClient *c = NULL;
	hr = f->CreateClientInterface( IID_IESClient, ( void ** )&c );
	if ( FAILED( hr ) || !c ) { printf( "FAIL CreateClientInterface 0x%lx\n", hr ); return 1; }

	hr = c->Startup(); printf( "Startup -> 0x%lx\n", hr ); if ( FAILED( hr ) ) fails++;
	c->RegisterMsgFilter( NULL, Cb );

	// 1) ket noi sai cong: phai tra E_FAIL nhanh (khong treo)
	DWORD t0 = GetTickCount();
	hr = c->ConnectTo( "127.0.0.1", ( unsigned short )( port + 1 ) );
	printf( "ConnectTo cong sai -> 0x%lx sau %lu ms %s\n", hr, GetTickCount() - t0, FAILED( hr ) ? "(OK, mong that bai)" : "(SAI: le ra that bai)" );
	if ( !FAILED( hr ) ) fails++;

	// 2) ket noi dung
	t0 = GetTickCount();
	hr = c->ConnectTo( "127.0.0.1", port );
	printf( "ConnectTo 127.0.0.1:%u -> 0x%lx sau %lu ms\n", port, hr, GetTickCount() - t0 );
	if ( FAILED( hr ) ) { printf( "FAIL ket noi\n" ); return 1; }

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
	hr = c->ConnectTo( "127.0.0.1", port ); printf( "ket noi lan 2 -> 0x%lx\n", hr ); if ( FAILED( hr ) ) fails++;
	else { DWORD ms = 0; c->SendPackToServer( "lan2", 4 ); if ( !DoiGoi( c, "ECHO:lan2", 2000, &ms ) ) fails++; }
	c->Shutdown(); c->Cleanup(); c->Release(); f->Release();

	FreeLibrary( h );
	printf( "%s: %d loi, callback create=%ld close=%ld\n", fails ? "FAIL" : "PASS", fails, g_nCreate, g_nClose );
	return fails ? 1 : 0;
}
