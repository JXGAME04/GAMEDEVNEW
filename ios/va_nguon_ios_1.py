# -*- coding: utf-8 -*-
r"""[IOS 11/09] Va nguon cho ban iOS - dot 1. Chay lai vo hai (idempotent).
Doc/ghi latin-1, giu nguyen ky tu xuong dong cua tung tep, khong doi mot byte nao > 0x7F.
MOI thay doi deu nam trong #ifdef JX_IOS -> ban Windows va ban Android khong doi hanh vi.
Chay:  python3 ios/va_nguon_ios_1.py
"""
import io, os, re, sys

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()

def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)

def dem_byte_cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)

def thay1(s, cu, moi, ten):
    if moi in s:
        print("   bo qua (da co): " + ten)
        return s, False
    n = s.count(cu)
    if n != 1:
        sys.exit("LOI: '%s' khop %d lan, phai dung 1" % (ten, n))
    return s.replace(cu, moi), True

# ---------------------------------------------------------------- 1. sys/sysinfo.h khong co tren Apple
# Thay vi sua hai ham GlobalMemoryStatus / GlobalMemoryStatusEx, ta cung cap dung ten sysinfo() va
# struct sysinfo cho Apple ngay tai cho include -> than hai ham KHONG doi mot dong nao.
P1 = os.path.join(GOC, "Sources", "Engine", "Src", "Platform", "KPosixWin32.cpp")
s = doc(P1)
truoc = dem_byte_cao(s)

CU = "#include <sys/sysinfo.h>\n"
MOI = """#ifdef JX_IOS
/* [IOS 11/09] Apple khong co <sys/sysinfo.h>. Cung cap dung ten sysinfo() + struct sysinfo o day
   de GlobalMemoryStatus / GlobalMemoryStatusEx ben duoi khong phai doi mot dong nao.
   Tong RAM lay qua sysctl(HW_MEMSIZE); phan con trong lay qua thong ke trang cua mach. */
#include <sys/sysctl.h>
#include <mach/mach.h>
struct sysinfo { unsigned long totalram; unsigned long freeram; unsigned int mem_unit; };
static int sysinfo(struct sysinfo* si)
{
\tif (!si) return -1;
\tsi->mem_unit = 1; si->totalram = 0; si->freeram = 0;
\tuint64_t nTong = 0; size_t nCo = sizeof(nTong); int mib[2] = { CTL_HW, HW_MEMSIZE };
\tif (sysctl(mib, 2, &nTong, &nCo, NULL, 0) == 0) si->totalram = (unsigned long)nTong;
\tvm_size_t nTrang = 0; mach_port_t host = mach_host_self();
\tvm_statistics64_data_t vm; mach_msg_type_number_t nDem = HOST_VM_INFO64_COUNT;
\tif (host_page_size(host, &nTrang) == KERN_SUCCESS &&
\t    host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&vm, &nDem) == KERN_SUCCESS)
\t\tsi->freeram = (unsigned long)((uint64_t)(vm.free_count + vm.inactive_count) * (uint64_t)nTrang);
\treturn 0;
}
#else
#include <sys/sysinfo.h>
#endif
"""
s, doi = thay1(s, CU, MOI, "KPosixWin32.cpp: sys/sysinfo.h -> nhanh JX_IOS")
sau = dem_byte_cao(s)
if truoc != sau:
    sys.exit("LOI: so byte >= 0x80 doi %d -> %d" % (truoc, sau))
if doi:
    ghi(P1, s)
    print("   da va: KPosixWin32.cpp (byte cao %d, khong doi)" % sau)

# ---------------------------------------------------------------- 2. jx_lua_fopen.c: fopen bi #undef roi goi lai
# -Dfopen=jx_lua_fopen lam <stdio.h> khai bao thanh jx_lua_fopen; sau "#undef fopen" thi fopen that chua duoc khai bao.
# Clang cua NDK chi canh bao (ban Android bo qua bang -w); clang cua Apple coi day la LOI (C99 bo khai bao ngam).
P2 = os.path.join(GOC, "Sources", "Library", "Lua54", "jx_lua_fopen.c")
s = doc(P2)
truoc = dem_byte_cao(s)
CU2 = "#undef fopen   /* trong tep nay goi fopen that */\n"
MOI2 = """#undef fopen   /* trong tep nay goi fopen that */
#ifdef JX_IOS
/* [IOS 11/09] khai bao lai fopen that sau khi #undef: clang cua Apple khong cho khai bao ngam.
   Ban Android di duong cu (khai bao ngam + canh bao bi -w nuot), khong doi mot dong. */
extern FILE* fopen(const char* pszPath, const char* pszMode);
#endif
"""
s, doi2 = thay1(s, CU2, MOI2, "jx_lua_fopen.c: khai bao lai fopen cho JX_IOS")
sau = dem_byte_cao(s)
if truoc != sau:
    sys.exit("LOI: so byte >= 0x80 doi %d -> %d" % (truoc, sau))
if doi2:
    ghi(P2, s)
    print("   da va: jx_lua_fopen.c (byte cao %d, khong doi)" % sau)

# ---------------------------------------------------------------- 3. JxPathPosix: goc he thong cua iOS
# Danh sach duong "he thong that, giu nguyen" chi co cac goc cua Android (/storage/, /sdcard/, /data/...).
# Tren iOS thu muc ung dung la /var/mobile/Containers/... (may that) hoac /Users/<ten>/Library/Developer/
# CoreSimulator/... (may ao) -> khong khop goc nao, bi coi la duong TUONG DOI: bi ha chu thuong va ghep vao
# thu muc du lieu -> access() luon that bai, app bao "khong thay config.ini" du tep co that.
# Chen NGUYEN MOT KHOI moi, KHONG sua dong nao cua ban cu (de kiem_rao.py so tung dong van bang nhau).
P3 = os.path.join(GOC, "Sources", "Engine", "Src", "Platform", "KPosixWin32.cpp")
s = doc(P3)
truoc = dem_byte_cao(s)
if "s_gocIos" in s:
    print("   bo qua (da co): KPosixWin32.cpp goc he thong iOS")
else:
    dong = s.splitlines(True)
    vt = [k for k, d in enumerate(dong) if d.strip() == "if (bSys) return pszOut;"]
    if len(vt) != 1:
        sys.exit("LOI: tim thay %d dong 'if (bSys) return pszOut;', phai dung 1" % len(vt))
    k = vt[0]
    le = re.match(r"[ \t]*", dong[k]).group(0)          # giu dung le thut dau cua dong cu
    nl = "\r\n" if dong[k].endswith("\r\n") else "\n"
    khoi = [
        "#ifdef JX_IOS",
        le + "/* [IOS 11/09] goc he thong cua iOS: /var/mobile/Containers (may that),",
        le + "   /Users/<ten>/Library/Developer/CoreSimulator (may ao). Khong co cho nay thi duong dan",
        le + "   tuyet doi cua iOS bi coi la tuong doi -> ha chu thuong + ghep thu muc du lieu -> mo tep hong. */",
        le + "if (!bSys)",
        le + "{",
        le + "\tstatic const char* s_gocIos[] = { \"/var/\", \"/private/\", \"/Users/\", \"/Library/\", \"/Applications/\", NULL };",
        le + "\tfor (int i3 = 0; s_gocIos[i3]; i3++) if (strncmp(pszOut, s_gocIos[i3], strlen(s_gocIos[i3])) == 0) { bSys = 1; break; }",
        le + "}",
        "#endif",
    ]
    dong = dong[:k] + [x + nl for x in khoi] + dong[k:]
    s = "".join(dong)
    if dem_byte_cao(s) != truoc:
        sys.exit("LOI: byte >= 0x80 doi")
    ghi(P3, s)
    print("   da va: KPosixWin32.cpp goc he thong iOS (chen nguyen khoi, byte cao %d)" % truoc)

print("xong va_nguon_ios_1.py")
