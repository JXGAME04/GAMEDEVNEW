//---------------------------------------------------------------------------
// [IOS 11/09] Cac ham cua BANG DO NHIP / HIEU NANG cho ban iOS.
//
// Ban Android cai o Platform/JxPerfHudAndroid.cpp, doc /proc va /sys - iOS khong co.
// Sau dot [JXMOBILE 11/09] cac cho GOI da chuyen sang rao JX_MOBILE nen iOS cung goi toi.
//
// [IOS-DO 11/09] Nay KHONG de rong nua: JxDoNhip_Vong() duoc goi MOI KHUNG tu vong lap chinh
// (S3Client.cpp), nen dung luon lam cho DEM KHUNG va cu moi 10 giay ghi mot dong vao jx_nhip.log
// canh du lieu game. So lieu lay tu ios/JxIosDoNhip.mm (mach + NSProcessInfo + UIDevice).
//
// Doc log:  moi dong = 1 lan do
//   [DO] <giay> s | fps <khung/giay> | cpu <% mot nhan> | ram <MB> (con <MB>) | nhiet <0..3> | pin <%>
// nhiet: 0 binh thuong, 1 am, 2 nong, 3 nghiem trong (iOS tu ha xung khi >= 1).
// ram  : phys_footprint - dung con so ma iOS dung de quyet dinh GIET app.
//---------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>

extern "C" void JxIosDo_Lay(float* pCpuPhanTram, double* pRamMB, double* pRamConMB,
                            int* pNhiet, float* pPin);

#define JX_DO_CHUKY_GIAY   10.0     // bao lau ghi mot dong

static double JxIos_Giay(void)
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

void JxDoNhip_Vong(void)
{
	static double s_dBatDau = 0.0, s_dLanTruoc = 0.0;
	static long   s_nKhung = 0;

	double d = JxIos_Giay();
	if (s_dBatDau == 0.0) { s_dBatDau = s_dLanTruoc = d; return; }
	s_nKhung++;
	double dTroi = d - s_dLanTruoc;
	if (dTroi < JX_DO_CHUKY_GIAY) return;

	float  fCpu = -1.0f, fPin = -1.0f;
	double dRam = -1.0, dRamCon = -1.0;
	int    nNhiet = -1;
	JxIosDo_Lay(&fCpu, &dRam, &dRamCon, &nNhiet, &fPin);

	FILE* f = fopen("jx_nhip.log", "a");	// thu muc hien hanh = thu muc du lieu (da chdir)
	if (f)
	{
		fprintf(f, "[DO] %6.0f s | fps %5.1f | cpu %5.1f %% | ram %7.1f MB (con %7.1f) | nhiet %d | pin %3.0f %%\n",
			d - s_dBatDau, (double)s_nKhung / dTroi, (double)fCpu, dRam, dRamCon, nNhiet,
			fPin >= 0.0f ? fPin * 100.0f : -1.0f);
		fclose(f);
	}
	s_nKhung = 0;
	s_dLanTruoc = d;
}

// Con lai de rong: chi phuc vu bang do VE LEN MAN cua ban Android.
void JxDoNhip_KhungVe(int nCatNgang)                       { (void)nCatNgang; }
void JxNhip_DatMuc(int nMuc)                               { (void)nMuc; }
void JxNhip_ChuMuc(int nMuc, char* sz, int n)              { (void)nMuc; if (sz && n > 0) sz[0] = 0; }
void JxNhip_VeNen(int nX, int nY, int nRong, int nCao)     { (void)nX; (void)nY; (void)nRong; (void)nCao; }
