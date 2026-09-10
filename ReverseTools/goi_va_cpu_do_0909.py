# -*- coding: utf-8 -*-
"""goi_va_cpu_do_0909.py - [CPU 09/09 do] them CPU % tien trinh + luong chinh (va so nhan may) vao dong [REP3] moi 30 s.

Chu bao: 'RAM - CPU tang hon ban hom qua, VRAM khong thay so lieu'. Dong [REP3] da co RAM rieng/WS va vram X/Y MB (DXGI)
nhung CHUA co CPU. Do bang GetProcessTimes/GetThreadTimes trong cua so thong ke: cpu ms / thoi gian that ms x 100.
'tien trinh' co the > 100 % (nhieu luong); 'luong chinh' = luong ve/tick (luong goi RepresentEnd).
"""
import io
import sys

NL = "\r\n"
T = "\t"
P = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/KRepresentShell3.cpp"
TAG = "[CPU 09/09 do]"


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for c in s if ord(c) >= 0x80); lf0 = s.count("\n") - s.count("\r\n")
if TAG in s:
    print("da co"); sys.exit(0)

neo = T*3 + "GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));" + NL
them = NL.join([
    T*3 + "// " + TAG + " CPU % tien trinh va luong chinh trong cua so thong ke (cpu ms / ms that x 100; tien trinh co the > 100 % vi nhieu luong)",
    T*3 + "static ULONGLONG s_uCpuTt = 0, s_uCpuLc = 0; static DWORD s_dwCpuMoc = 0; static int s_nNhan = 0;",
    T*3 + "double dCpuTt = 0.0, dCpuLc = 0.0;",
    T*3 + "{",
    T*4 + "FILETIME ftT, ftX, ftK, ftU; ULONGLONG uTt = 0, uLc = 0;",
    T*4 + "if (GetProcessTimes(GetCurrentProcess(), &ftT, &ftX, &ftK, &ftU)) uTt = (((ULONGLONG)ftK.dwHighDateTime << 32) | ftK.dwLowDateTime) + (((ULONGLONG)ftU.dwHighDateTime << 32) | ftU.dwLowDateTime);",
    T*4 + "if (GetThreadTimes(GetCurrentThread(), &ftT, &ftX, &ftK, &ftU)) uLc = (((ULONGLONG)ftK.dwHighDateTime << 32) | ftK.dwLowDateTime) + (((ULONGLONG)ftU.dwHighDateTime << 32) | ftU.dwLowDateTime);",
    T*4 + "if (s_nNhan == 0) { SYSTEM_INFO si; GetSystemInfo(&si); s_nNhan = (int)si.dwNumberOfProcessors; }",
    T*4 + "if (s_dwCpuMoc != 0 && dwNow > s_dwCpuMoc) { const double dMs = (double)(dwNow - s_dwCpuMoc); dCpuTt = (double)(uTt - s_uCpuTt) / 10000.0 * 100.0 / dMs; dCpuLc = (double)(uLc - s_uCpuLc) / 10000.0 * 100.0 / dMs; }",
    T*4 + "s_uCpuTt = uTt; s_uCpuLc = uLc; s_dwCpuMoc = dwNow;",
    T*3 + "}",
    "",
])
s = rep(s, neo, neo + them, "GetProcessMemoryInfo")
s = rep(s, "\"[REP3] RAM rieng %u MB, WS %u MB | VRAM con %u MB",
        "\"[REP3] RAM rieng %u MB, WS %u MB | cpu tien trinh %.0f %% (luong chinh %.0f %%, may %d nhan) | VRAM con %u MB", "fmt")
s = rep(s, "(unsigned)(pmc.PrivateUsage >> 20), (unsigned)(pmc.WorkingSetSize >> 20), (unsigned)(PD3DDEVICE->GetAvailableTextureMem() >> 20),",
        "(unsigned)(pmc.PrivateUsage >> 20), (unsigned)(pmc.WorkingSetSize >> 20), dCpuTt, dCpuLc, s_nNhan, (unsigned)(PD3DDEVICE->GetAvailableTextureMem() >> 20),", "args")
if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s or s.count("\n") - s.count("\r\n") != lf0:
    print("FAIL ma hoa"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KRepresentShell3.cpp " + TAG)
