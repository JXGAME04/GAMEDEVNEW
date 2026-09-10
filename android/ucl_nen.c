/* [ANDROID 12/09 NEN] Nen NRV2B (UCL) hang loat cho dong_goi_du_lieu_dien_thoai.py --nen-roi.
 * Cung thu vien UCL ma XPackFile.cpp giai nen (ucl_nrv2b_decompress_8) -> muc pak kieu TYPE_UCL (cf = 0x01000000 | co nen).
 * Dung:  ucl_nen.exe <danh_sach.txt> [muc 1..10]      (danh sach UTF-8, moi dong: <tep vao>\t<tep ra>)
 * In ra moi dong: <co goc>\t<co nen>   (co nen = 0: khong nen duoc / khong dang -> giu tho)
 * Dich (PowerShell, vcvars64): cl /O2 /nologo /I Sources/Engine/Include android/ucl_nen.c Sources/Engine/Src/ucl/{n2b_99,ucl_init,ucl_util,alloc,ucl_ptr,ucl_str,ucl_crc}.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "ucl/ucl.h"

static unsigned char* doc_tep(const wchar_t* p, size_t* n)
{
	FILE* f = _wfopen(p, L"rb");
	unsigned char* b;
	long co;
	if (!f)
		return NULL;
	fseek(f, 0, SEEK_END); co = ftell(f); fseek(f, 0, SEEK_SET);
	b = (unsigned char*)malloc(co > 0 ? co : 1);
	if (co > 0 && fread(b, 1, co, f) != (size_t)co) { free(b); fclose(f); return NULL; }
	fclose(f);
	*n = (size_t)co;
	return b;
}

static void utf8_sang_w(const char* s, wchar_t* w, int nW)
{
	int n = MultiByteToWideChar(CP_UTF8, 0, s, -1, w, nW);
	if (n <= 0)
		w[0] = 0;
}

int main(int argc, char** argv)
{
	FILE* ds;
	char dong[4096];
	int muc = 7;
	if (argc < 2)
	{
		fprintf(stderr, "dung: ucl_nen <danh_sach.txt> [muc 1..10]\n");
		return 2;
	}
	if (argc > 2)
		muc = atoi(argv[2]);
	if (muc < 1) muc = 1;
	if (muc > 10) muc = 10;
	/* ucl_init() cua ban UCL cu trong engine bao loi tren MSVC 2022 (kiem sizeof) nhung engine khong goi no;
	 * bo qua, thay bang KIEM GIAI NEN tung muc ngay duoi day (cung ham engine dung: ucl_nrv2b_decompress_8). */
	if (ucl_init() != UCL_E_OK)
		fprintf(stderr, "(ucl_init bao loi - bo qua, moi muc deu duoc giai nen kiem lai)\n");
	ds = fopen(argv[1], "rb");
	if (!ds)
	{
		fprintf(stderr, "khong mo duoc danh sach %s\n", argv[1]);
		return 2;
	}
	while (fgets(dong, sizeof(dong), ds))
	{
		char* tab; char* cuoi;
		wchar_t wVao[2048], wRa[2048];
		unsigned char* src; unsigned char* dst;
		size_t n; ucl_uint dlen; int r;
		cuoi = dong + strlen(dong);
		while (cuoi > dong && (cuoi[-1] == '\n' || cuoi[-1] == '\r')) *--cuoi = 0;
		tab = strchr(dong, '\t');
		if (!tab) continue;
		*tab = 0;
		utf8_sang_w(dong, wVao, 2048);
		utf8_sang_w(tab + 1, wRa, 2048);
		src = doc_tep(wVao, &n);
		if (!src)
		{
			printf("-1\t0\n");
			fflush(stdout);
			continue;
		}
		dlen = (ucl_uint)(n + n / 8 + 256);
		dst = (unsigned char*)malloc(dlen);
		r = ucl_nrv2b_99_compress(src, (ucl_uint)n, dst, &dlen, NULL, muc, NULL, NULL);
		if (r == UCL_E_OK && dlen > 0 && dlen < (ucl_uint)(n - n / 10))
		{
			/* kiem lai: giai nen phai ra dung byte goc (nhu XPackFile.cpp lam khi doc pak) */
			unsigned char* lai = (unsigned char*)malloc(n + 16);
			ucl_uint nLai = (ucl_uint)n;
			int r2 = ucl_nrv2b_decompress_8(dst, dlen, lai, &nLai, NULL);
			int ok = (r2 == UCL_E_OK && nLai == (ucl_uint)n && memcmp(lai, src, n) == 0);
			free(lai);
			if (!ok)
			{
				fprintf(stderr, "GIAI NEN SAI: %s\n", dong);
				printf("%u\t0\n", (unsigned)n);
				fflush(stdout);
				free(src); free(dst);
				continue;
			}
			FILE* g = _wfopen(wRa, L"wb");
			if (g && fwrite(dst, 1, dlen, g) == dlen)
			{
				fclose(g);
				printf("%u\t%u\n", (unsigned)n, (unsigned)dlen);
			}
			else
			{
				if (g) fclose(g);
				printf("%u\t0\n", (unsigned)n);
			}
		}
		else
			printf("%u\t0\n", (unsigned)n);
		fflush(stdout);
		free(src); free(dst);
	}
	fclose(ds);
	return 0;
}
