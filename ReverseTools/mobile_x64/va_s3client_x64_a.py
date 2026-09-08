# -*- coding: utf-8 -*-
"""Dot S3Client x64 (a), byte-safe (chi doi ASCII, giu byte cao):
 a. KLVideo/rad.h: khoi hop ngu MSC chi cho x86; them ban C cho _M_X64 (KLVideo.dll van x86 -> video khong chay tren x64, chi de bien dich).
 b. CrashLog.cpp: thanh ghi/khung ngan xep theo kien truc (Rip/Rbp/Rsp, may 0x8664).
 c. NetConnectAgent.cpp: nSize size_t (IClient::GetPackFromServer(size_t&)).
 d. UiCursor.cpp: MAKEINTRESOURCE(IDC_ARROW) -> IDC_ARROW (IDC_ARROW da la MAKEINTRESOURCE).
 e. WndMessageListBox.h/.cpp: tay cam du lieu (hData) unsigned int -> KUPARAM.
 f. WndMessage.h: WND_GAMESPACE 0xFFFFFFFF -> ((KUPARAM)-1).
 g. ShortcutKey.cpp: tay cam cua so tu Lua: int -> KNPARAM.
 h. UiChatCentre.h/.cpp UpdateData(... KUPARAM uParam ...); UiShop.h/.cpp FkAutoOnBuyItem(KUPARAM).
 i. UiFaceSelector.cpp:215 hai ep con tro.
 j. KUiAuction/KUiMail/KUiChienLenh_OnCoreCmd(unsigned int, KNPARAM nParam).
 k. AntiHack/DumpMemory.cpp: (LPCVOID)(KUPARAM)offset.
"""
import io, os, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
S = ROOT + r"\Sources"
C = S + r"\S3Client"

def rd(p): return io.open(p, "rb").read()
def wr(p, b, nb):
    assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127), "byte cao doi: " + p
    io.open(p, "wb").write(nb)
def nl_of(b): return b"\r\n" if b"\r\n" in b else b"\n"
def sub_exact(p, pairs, tag):
    b = rd(p); nb = b
    for old, new, cnt in pairs:
        c = nb.count(old)
        assert (c == cnt) if cnt is not None else (c >= 1), (tag, old[:70], c, cnt)
        nb = nb.replace(old, new)
    if nb != b: wr(p, b, nb)
    print(tag, "OK" if nb != b else "da va/khong doi")

# ---- a. rad.h
p = S + r"\KLVideo\rad.h"; b = rd(p)
if b"[X64 08/09]" in b: print("a rad.h: da va")
else:
    nl = nl_of(b)
    old = b"      #ifdef _MSC_VER" + nl + nl + b"        #pragma warning( disable : 4035)"
    assert b.count(old) == 1, b.count(old)
    x64 = nl.join([
        b"      #if defined(_M_X64)   /* [X64 08/09] ban C thay khoi hop ngu x86 ben duoi (KLVideo.dll van x86 nen video khong chay tren x64) */",
        b"        #include <intrin.h>",
        b"        #undef BreakPoint",
        b"        #define BreakPoint() __debugbreak()",
        b"        typedef char* RADPCHAR;",
        b"        u32 __inline radsqr(u32 m) { return m * m; }",
        b"        u32 __inline mult64anddiv(u32 m1, u32 m2, u32 d) { return d ? (u32)(((unsigned __int64)m1 * m2) / d) : 0; }",
        b"        s32 __inline radabs(s32 ab) { return ab < 0 ? -ab : ab; }",
        b"        u8  __inline radinp(u16 p) { (void)p; return 0; }",
        b"        void __inline radoutp(u16 p, u8 v) { (void)p; (void)v; }",
        b"        RADPCHAR __inline radstpcpy(char* p1, char* p2) { while ((*p1 = *p2) != 0) { p1++; p2++; } return p1; }",
        b"        RADPCHAR __inline radstpcpyrs(char* p1, char* p2) { while ((*p1 = *p2) != 0) { p1++; p2++; } return p2; }",
        b"        void __inline radmemset16(void* dest, u16 value, u32 sizeb) { u16* d = (u16*)dest; while (sizeb--) *d++ = value; }",
        b"        void __inline radmemset32(void* dest, u32 value, u32 sizeb) { u32* d = (u32*)dest; while (sizeb--) *d++ = value; }",
        b"        u32 __inline RADsqrt(u32 sq) { u32 r = 0, bit = 1u << 30; while (bit > sq) bit >>= 2; while (bit) { if (sq >= r + bit) { sq -= r + bit; r = (r >> 1) + bit; } else r >>= 1; bit >>= 2; } return r; }",
        b"        void __inline RADCycleTimerStartAddr(u32* addr) { *addr = (u32)__rdtsc(); }",
        b"        u32 __inline RADCycleTimerDeltaAddr(u32* addr) { u32 d = (u32)__rdtsc() - *addr; *addr = d; return d; }",
        b"        #define RADCycleTimerStart(var) RADCycleTimerStartAddr(&var)",
        b"        #define RADCycleTimerDelta(var) RADCycleTimerDeltaAddr(&var)",
        b"      #endif",
        b"",
        b"      #if defined(_MSC_VER) && !defined(_M_X64)",
        b"",
        b"        #pragma warning( disable : 4035)"])
    wr(p, b, b.replace(old, x64, 1)); print("a rad.h: OK")

# ---- b. CrashLog.cpp
p = C + r"\CrashLog.cpp"; b = rd(p)
if b"CL_CTX_IP" in b: print("b CrashLog.cpp: da va")
else:
    nl = nl_of(b)
    anchor = b"static void CL_DumpStack(CONTEXT* pCtx)"
    assert b.count(anchor) == 1
    macros = nl.join([
        b"// [X64 08/09] thanh ghi / khung ngan xep theo kien truc",
        b"#ifdef _WIN64",
        b"\ttypedef DWORD64 CL_ADDR;",
        b"\t#define CL_CTX_IP(c) ((c)->Rip)",
        b"\t#define CL_CTX_BP(c) ((c)->Rbp)",
        b"\t#define CL_CTX_SP(c) ((c)->Rsp)",
        b"\t#define CL_MACHINE   0x8664",
        b"#else",
        b"\ttypedef DWORD CL_ADDR;",
        b"\t#define CL_CTX_IP(c) ((c)->Eip)",
        b"\t#define CL_CTX_BP(c) ((c)->Ebp)",
        b"\t#define CL_CTX_SP(c) ((c)->Esp)",
        b"\t#define CL_MACHINE   0x014c",
        b"#endif",
        b"", anchor])
    nb = b.replace(anchor, macros, 1)
    pairs = [
        (b"DWORD* pFrame = (DWORD*)pCtx->Ebp;", b"CL_ADDR* pFrame = (CL_ADDR*)CL_CTX_BP(pCtx);", 1),
        (b"if (IsBadReadPtr(pFrame, sizeof(DWORD) * 2))", b"if (IsBadReadPtr(pFrame, sizeof(CL_ADDR) * 2))", 1),
        (b"DWORD dwRet = pFrame[1];", b"CL_ADDR dwRet = pFrame[1];", 1),
        (b"pFrame = (DWORD*)pFrame[0];", b"pFrame = (CL_ADDR*)pFrame[0];", 1),
        (b"sf.AddrPC.Offset    = pCtx->Eip;", b"sf.AddrPC.Offset    = CL_CTX_IP(pCtx);", 1),
        (b"sf.AddrFrame.Offset = pCtx->Ebp;", b"sf.AddrFrame.Offset = CL_CTX_BP(pCtx);", 1),
        (b"sf.AddrStack.Offset = pCtx->Esp;", b"sf.AddrStack.Offset = CL_CTX_SP(pCtx);", 1),
        (b"if (!s_pStackWalk64(0x014c, GetCurrentProcess(), GetCurrentThread(),", b"if (!s_pStackWalk64(CL_MACHINE, GetCurrentProcess(), GetCurrentThread(),", 1),
    ]
    for old, new, cnt in pairs:
        assert nb.count(old) == cnt, (old, nb.count(old)); nb = nb.replace(old, new)
    old_regs = nl.join([
        b'        _snprintf(szLine, sizeof(szLine) - 1,',
        b'                  "  Thanh ghi: EAX=%08X EBX=%08X ECX=%08X EDX=%08X",',
        b'                  (unsigned)pCtx->Eax, (unsigned)pCtx->Ebx,',
        b'                  (unsigned)pCtx->Ecx, (unsigned)pCtx->Edx);',
        b'        szLine[sizeof(szLine) - 1] = 0;',
        b'        CL_WriteLine(szLine);',
        b'',
        b'        _snprintf(szLine, sizeof(szLine) - 1,',
        b'                  "             ESI=%08X EDI=%08X EBP=%08X ESP=%08X EIP=%08X",',
        b'                  (unsigned)pCtx->Esi, (unsigned)pCtx->Edi,',
        b'                  (unsigned)pCtx->Ebp, (unsigned)pCtx->Esp, (unsigned)pCtx->Eip);',
        b'        szLine[sizeof(szLine) - 1] = 0;',
        b'        CL_WriteLine(szLine);'])
    assert nb.count(old_regs) == 1, nb.count(old_regs)
    new_regs = nl.join([
        b'#ifdef _WIN64',
        b'        _snprintf(szLine, sizeof(szLine) - 1,',
        b'                  "  Thanh ghi: RAX=%016llX RBX=%016llX RCX=%016llX RDX=%016llX",',
        b'                  (unsigned long long)pCtx->Rax, (unsigned long long)pCtx->Rbx,',
        b'                  (unsigned long long)pCtx->Rcx, (unsigned long long)pCtx->Rdx);',
        b'        szLine[sizeof(szLine) - 1] = 0;',
        b'        CL_WriteLine(szLine);',
        b'',
        b'        _snprintf(szLine, sizeof(szLine) - 1,',
        b'                  "             RSI=%016llX RDI=%016llX RBP=%016llX RSP=%016llX RIP=%016llX",',
        b'                  (unsigned long long)pCtx->Rsi, (unsigned long long)pCtx->Rdi,',
        b'                  (unsigned long long)pCtx->Rbp, (unsigned long long)pCtx->Rsp, (unsigned long long)pCtx->Rip);',
        b'        szLine[sizeof(szLine) - 1] = 0;',
        b'        CL_WriteLine(szLine);',
        b'#else', old_regs, b'#endif'])
    nb = nb.replace(old_regs, new_regs, 1)
    wr(p, b, nb); print("b CrashLog.cpp: OK")

# ---- c. NetConnectAgent.cpp
sub_exact(C + r"\NetConnect\NetConnectAgent.cpp", [(b"\tunsigned int nSize;", b"\tsize_t nSize;\t// [X64 08/09] IClient::GetPackFromServer(size_t&)", None)], "c NetConnectAgent.cpp")
# ---- d. UiCursor.cpp
sub_exact(C + r"\Ui\Elem\UiCursor.cpp", [(b"MAKEINTRESOURCE(IDC_ARROW)", b"IDC_ARROW", None)], "d UiCursor.cpp")
# ---- e. WndMessageListBox
p = C + r"\Ui\Elem\WndMessageListBox.h"; b = rd(p)
nb = re.sub(rb"unsigned int(\s+)SplitData\(\)", lambda m: b"KUPARAM" + m.group(1) + b"SplitData()", b)
nb = re.sub(rb"unsigned int(\s+)BindData\(unsigned int(\s+)hData\)", lambda m: b"KUPARAM" + m.group(1) + b"BindData(KUPARAM" + m.group(2) + b"hData)", nb)
nb = re.sub(rb"FreeData\(unsigned int(\s+)hData\)", lambda m: b"FreeData(KUPARAM" + m.group(1) + b"hData)", nb)
assert nb.count(b"KUPARAM") >= 3, nb.count(b"KUPARAM"); wr(p, b, nb); print("e WndMessageListBox.h: OK")
p = C + r"\Ui\Elem\WndMessageListBox.cpp"; b = rd(p)
nb = b.replace(b"unsigned int KWndMessageListBox::SplitData()", b"KUPARAM KWndMessageListBox::SplitData()")
nb = nb.replace(b"unsigned int KWndMessageListBox::BindData(unsigned int hData)", b"KUPARAM KWndMessageListBox::BindData(KUPARAM hData)")
nb = re.sub(rb"void KWndMessageListBox::FreeData\(unsigned int(\s+)hData\)", lambda m: b"void KWndMessageListBox::FreeData(KUPARAM" + m.group(1) + b"hData)", nb)
nb = re.sub(rb"\(unsigned int\)\s*(pNew\w*|pData|pList\w*)\b", lambda m: b"(KUPARAM)" + m.group(1), nb)
assert nb.count(b"KUPARAM") >= 3; wr(p, b, nb); print("e WndMessageListBox.cpp: OK", nb.count(b"KUPARAM"))
# ---- f. WndMessage.h
p = C + r"\Ui\Elem\WndMessage.h"; b = rd(p)
nb, n = re.subn(rb"(#define\s+WND_GAMESPACE\s+)0xFFFFFFFF", lambda m: m.group(1) + b"((KUPARAM)-1)\t// [X64 08/09] rong bang con tro; Win32 van 0xFFFFFFFF", b)
assert n == 1 or b"((KUPARAM)-1)" in b; wr(p, b, nb) if n else None; print("f WndMessage.h:", "OK" if n else "da va")
# ---- g. ShortcutKey.cpp
sub_exact(C + r"\Ui\ShortcutKey.cpp", [(b"\t\tint hWnd = (int)Lua_ValueToNumber(L, 2);", b"\t\tKNPARAM hWnd = (KNPARAM)Lua_ValueToNumber(L, 2);\t// [X64 08/09]", None)], "g ShortcutKey.cpp")
# ---- h. UiChatCentre / UiShop
sub_exact(C + r"\Ui\UiCase\UiChatCentre.h", [(b"unsigned int uParam, int nGroupIndex);", b"KUPARAM uParam, int nGroupIndex);", 1)], "h UiChatCentre.h")
sub_exact(C + r"\Ui\UiCase\UiChatCentre.cpp", [(b"void KUiChatCentre::UpdateData(UICHATCENTRE_UPDATE eFlag, unsigned int uParam, int nGroupIndex)", b"void KUiChatCentre::UpdateData(UICHATCENTRE_UPDATE eFlag, KUPARAM uParam, int nGroupIndex)", 1)], "h UiChatCentre.cpp")
sub_exact(C + r"\Ui\UiCase\UiShop.h", [(b"FkAutoOnBuyItem(unsigned int szItemName);", b"FkAutoOnBuyItem(KUPARAM szItemName);", 1)], "h UiShop.h")
sub_exact(C + r"\Ui\UiCase\UiShop.cpp", [(b"void KUiShop::FkAutoOnBuyItem(unsigned int szItemName)", b"void KUiShop::FkAutoOnBuyItem(KUPARAM szItemName)", 1)], "h UiShop.cpp")
# ---- i. UiFaceSelector.cpp
sub_exact(C + r"\Ui\UiCase\UiFaceSelector.cpp", [(b"(unsigned int)m_pvCallerParam, (int)ms_pFaceList[m_nCurrIndex].szFaceText", b"(KUPARAM)m_pvCallerParam, (KNPARAM)ms_pFaceList[m_nCurrIndex].szFaceText", 1)], "i UiFaceSelector.cpp")
# ---- j. OnCoreCmd
for f in (r"\Ui\UiCase\UiAuction.h", r"\Ui\UiCase\UiAuction.cpp", r"\Ui\UiCase\UiMail.h", r"\Ui\UiCase\UiMail.cpp", r"\Ui\UiCase\UiChienLenh.h", r"\Ui\UiCase\UiChienLenh.cpp"):
    p = C + f; b = rd(p)
    nb, n = re.subn(rb"(KUi(?:Auction|Mail|ChienLenh)_OnCoreCmd\(unsigned int uCmd, )int nParam\)", lambda m: m.group(1) + b"KNPARAM nParam)", b)
    if n: wr(p, b, nb)
    print("j", f, n)
# ---- k. DumpMemory.cpp
sub_exact(C + r"\AntiHack\DumpMemory\DumpMemory.cpp", [(b"(LPCVOID)g_ProcessesDumps[i].m_aOffset", b"(LPCVOID)(KUPARAM)g_ProcessesDumps[i].m_aOffset", 1)], "k DumpMemory.cpp")
