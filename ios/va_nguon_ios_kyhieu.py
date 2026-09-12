# -*- coding: utf-8 -*-
r"""[IOS-KYHIEU 11/09] Bang tra ky hieu TINH thay cho dlopen, chi cho iOS.

Benh: tren iOS moi thu link TINH vao MOT nhi phan, khong co libRepresent3.so / libRainbow.so de nap.
S3Client.cpp:389 goi LoadLibrary("Represent3.dll") -> JxPathPosix -> dlopen -> NULL
-> khong tao duoc tang ve -> MyApp.Init() that bai -> JxPosixMain tra ve 0 sau ~270 ms, man hinh den.

Chua: them mot bang tra tinh trong KPosixWin32.cpp. Ung dung dang ky (module, ten ham, con tro) truoc
khi chay (ios/JxIosMain.cpp). LoadLibraryA / GetModuleHandleA tra ve mot the gia cho module da dang ky;
GetProcAddress doc bang. Khong khop thi van di duong dlopen cu.

MOI thay doi la CHEN THEM KHOI, khong sua dong cu nao -> ios/kiem_rao.py so tung dong van bang nhau.
Chay lai vo hai.  python3 ios/va_nguon_ios_kyhieu.py
"""
import io, os, sys
GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
P = os.path.join(GOC, "Sources", "Engine", "Src", "Platform", "KPosixWin32.cpp")
s = io.open(P, encoding="latin-1", newline="").read()
bc = lambda t: sum(1 for c in t if ord(c) >= 0x80)
n0 = bc(s)

def chen(cu, moi, ten):
    global s
    if moi in s:
        print("   bo qua (da co): " + ten); return 0
    if s.count(cu) != 1: sys.exit("LOI: '%s' khop %d lan" % (ten, s.count(cu)))
    s = s.replace(cu, moi); print("   da va: " + ten); return 1

n = 0
# 1. bang tra + ham dang ky, dat ngay truoc LoadLibraryA
CU1 = '/*---------------------------------------------------------------- module: "Rainbow.dll" -> libRainbow.so */\n'
MOI1 = '''#ifdef JX_IOS
/*---------------------------------------------------------------- [IOS-KYHIEU 11/09] bang tra ky hieu TINH
  iOS khong nap duoc thu vien dong tu ngoai goi ung dung: moi TU deu link TINH vao mot nhi phan.
  Ung dung dang ky truoc (ios/JxIosMain.cpp) roi LoadLibrary/GetProcAddress doc tu bang nay.        */
typedef struct { char szMod[32]; char szTen[64]; void* pfn; } JxKyHieuTinh;
static JxKyHieuTinh s_KyHieu[64];
static int          s_nKyHieu = 0;
static char         s_szModTinh[8][32];       /* the gia tra cho LoadLibrary: con tro vao mang nay */
static int          s_nModTinh = 0;

/* "..\\\\Represent3.dll" -> "represent3" */
static void JxKyHieu_TenGoc(const char* pszIn, char* pszRa, size_t nRa)
{
\tconst char* p = strrchr(pszIn, '\\\\'); const char* q = strrchr(pszIn, '/');
\tconst char* f = p > q ? p + 1 : (q ? q + 1 : pszIn);
\tsize_t i = 0;
\tfor (; f[i] && i + 1 < nRa; i++) pszRa[i] = (char)tolower((unsigned char)f[i]);
\tpszRa[i] = 0;
\tif (i > 4 && strcmp(pszRa + i - 4, ".dll") == 0) pszRa[i - 4] = 0;
\telse if (i > 3 && strcmp(pszRa + i - 3, ".so") == 0) pszRa[i - 3] = 0;
}

void JxPosix_DangKyKyHieu(const char* pszMod, const char* pszTen, void* pfn)
{
\tif (!pszMod || !pszTen || s_nKyHieu >= (int)(sizeof(s_KyHieu) / sizeof(s_KyHieu[0]))) return;
\tJxKyHieu_TenGoc(pszMod, s_KyHieu[s_nKyHieu].szMod, sizeof(s_KyHieu[0].szMod));
\tstrncpy(s_KyHieu[s_nKyHieu].szTen, pszTen, sizeof(s_KyHieu[0].szTen) - 1);
\ts_KyHieu[s_nKyHieu].szTen[sizeof(s_KyHieu[0].szTen) - 1] = 0;
\ts_KyHieu[s_nKyHieu].pfn = pfn;
\ts_nKyHieu++;
}

/* tra the gia cho module da co ky hieu dang ky, NULL neu chua co */
static HMODULE JxKyHieu_TheModule(const char* pszMod)
{
\tchar szG[32]; JxKyHieu_TenGoc(pszMod, szG, sizeof(szG));
\tint i, bCo = 0;
\tfor (i = 0; i < s_nKyHieu; i++) if (strcmp(s_KyHieu[i].szMod, szG) == 0) { bCo = 1; break; }
\tif (!bCo) return NULL;
\tfor (i = 0; i < s_nModTinh; i++) if (strcmp(s_szModTinh[i], szG) == 0) return (HMODULE)s_szModTinh[i];
\tif (s_nModTinh >= 8) return NULL;
\tstrncpy(s_szModTinh[s_nModTinh], szG, sizeof(s_szModTinh[0]) - 1);
\ts_szModTinh[s_nModTinh][sizeof(s_szModTinh[0]) - 1] = 0;
\treturn (HMODULE)s_szModTinh[s_nModTinh++];
}

static int JxKyHieu_LaTheTinh(HMODULE h)
{
\treturn h && (char*)h >= (char*)s_szModTinh && (char*)h < (char*)s_szModTinh + sizeof(s_szModTinh);
}

#endif /* JX_IOS */
''' + CU1
n += chen(CU1, MOI1, "them bang tra ky hieu tinh")

# 2. LoadLibraryA: thu bang tra truoc
CU2 = '\tif (!name) return NULL;\n\tchar base[256];'
MOI2 = '''\tif (!name) return NULL;
#ifdef JX_IOS
\t{ HMODULE hT = JxKyHieu_TheModule(name); if (hT) return hT; }\t// [IOS-KYHIEU 11/09]
#endif
\tchar base[256];'''
n += chen(CU2, MOI2, "LoadLibraryA doc bang tra")

# 3. GetProcAddress
CU3 = 'FARPROC GetProcAddress(HMODULE h, LPCSTR name) { if (!h || !name) return NULL; return (FARPROC)dlsym((void*)h, name); }\n'
# rao co nhanh #else giu NGUYEN VAN dong cu -> kiem_rao.py so tung dong van bang nhau
MOI3 = '''#ifdef JX_IOS	// [IOS-KYHIEU 11/09] doc bang tra tinh truoc, khong khop thi van dlsym
FARPROC GetProcAddress(HMODULE h, LPCSTR name)
{
\tif (!h || !name) return NULL;
\tif (JxKyHieu_LaTheTinh(h))
\t{
\t\tfor (int i = 0; i < s_nKyHieu; i++)
\t\t\tif (strcmp(s_KyHieu[i].szMod, (const char*)h) == 0 && strcmp(s_KyHieu[i].szTen, name) == 0)
\t\t\t\treturn (FARPROC)s_KyHieu[i].pfn;
\t\tjx_log("GetProcAddress(%s, %s): khong co trong bang tra tinh", (const char*)h, name);
\t\treturn NULL;
\t}
\treturn (FARPROC)dlsym((void*)h, name);
}
#else
''' + CU3 + '''#endif
'''
n += chen(CU3, MOI3, "GetProcAddress doc bang tra")

# 4. FreeLibrary + GetModuleHandleA
CU4 = 'BOOL FreeLibrary(HMODULE h) { if (!h) return FALSE; return dlclose((void*)h) == 0; }\n'
MOI4 = '''#ifdef JX_IOS	// [IOS-KYHIEU 11/09] the gia: khong co gi de tha
BOOL FreeLibrary(HMODULE h)
{
\tif (!h) return FALSE;
\tif (JxKyHieu_LaTheTinh(h)) return TRUE;
\treturn dlclose((void*)h) == 0;
}
#else
''' + CU4 + '''#endif
'''
n += chen(CU4, MOI4, "FreeLibrary hieu the gia")

CU5 = '\tif (!name) return (HMODULE)dlopen(NULL, RTLD_NOW);\n'
MOI5 = '''\tif (!name) return (HMODULE)dlopen(NULL, RTLD_NOW);
#ifdef JX_IOS
\t{ HMODULE hT = JxKyHieu_TheModule(name); if (hT) return hT; }\t// [IOS-KYHIEU 11/09]
#endif
'''
n += chen(CU5, MOI5, "GetModuleHandleA doc bang tra")

if n:
    if bc(s) != n0: sys.exit("LOI: byte >= 0x80 doi")
    io.open(P, "w", encoding="latin-1", newline="").write(s)

# 5. khai bao trong header
PH = os.path.join(GOC, "Sources", "Engine", "Src", "Platform", "KPosixWin32.h")
h = io.open(PH, encoding="latin-1", newline="").read()
CUH = "void        JxPosix_SetDataDir(const char* pszDir);"
if "JxPosix_DangKyKyHieu" not in h:
    assert h.count(CUH) == 1
    h = h.replace(CUH, "#ifdef JX_IOS\nvoid        JxPosix_DangKyKyHieu(const char* pszMod, const char* pszTen, void* pfn);  /* [IOS-KYHIEU 11/09] bang tra tinh thay dlopen */\n#endif\n" + CUH)
    io.open(PH, "w", encoding="latin-1", newline="").write(h)
    print("   da va: khai bao trong KPosixWin32.h")

print("xong va_nguon_ios_kyhieu.py (%d cho)" % n)
