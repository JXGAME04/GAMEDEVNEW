# giai_ma.py - doi (module, offset) -> ham + tep:dong bang dbghelp.dll (ctypes), khong can cdb/windbg.
# Dung: python giai_ma.py <CoreServer.dll thuc te> <base hex> <size hex> <GameServer.exe> <base> <size> off1 off2 ... (off dang 'C:0x234317' hoac 'G:0x9963')
import ctypes, ctypes.wintypes as W, sys, os
sys.stdout.reconfigure(encoding='utf-8', errors='replace')
dbg = ctypes.WinDLL('dbghelp')
HPROC = ctypes.c_void_p(0x4444)
SYMOPT_UNDNAME = 0x2; SYMOPT_DEFERRED_LOADS = 0x4; SYMOPT_LOAD_LINES = 0x10; SYMOPT_NO_PROMPTS = 0x80000
dbg.SymSetOptions.restype = W.DWORD
dbg.SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_NO_PROMPTS)
dbg.SymInitializeW.argtypes = [ctypes.c_void_p, ctypes.c_wchar_p, W.BOOL]; dbg.SymInitializeW.restype = W.BOOL
dbg.SymLoadModuleExW.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_ulonglong, W.DWORD, ctypes.c_void_p, W.DWORD]
dbg.SymLoadModuleExW.restype = ctypes.c_ulonglong

class SYMBOL_INFOW(ctypes.Structure):
    _fields_ = [("SizeOfStruct", W.DWORD), ("TypeIndex", W.DWORD), ("Reserved", ctypes.c_ulonglong * 2), ("Index", W.DWORD), ("Size", W.DWORD),
                ("ModBase", ctypes.c_ulonglong), ("Flags", W.DWORD), ("Value", ctypes.c_ulonglong), ("Address", ctypes.c_ulonglong),
                ("Register", W.DWORD), ("Scope", W.DWORD), ("Tag", W.DWORD), ("NameLen", W.DWORD), ("MaxNameLen", W.DWORD), ("Name", ctypes.c_wchar * 1024)]
class IMAGEHLP_LINEW64(ctypes.Structure):
    _fields_ = [("SizeOfStruct", W.DWORD), ("Key", ctypes.c_void_p), ("LineNumber", W.DWORD), ("FileName", ctypes.c_wchar_p), ("Address", ctypes.c_ulonglong)]
dbg.SymFromAddrW.argtypes = [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.POINTER(ctypes.c_ulonglong), ctypes.POINTER(SYMBOL_INFOW)]; dbg.SymFromAddrW.restype = W.BOOL
dbg.SymGetLineFromAddrW64.argtypes = [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.POINTER(W.DWORD), ctypes.POINTER(IMAGEHLP_LINEW64)]; dbg.SymGetLineFromAddrW64.restype = W.BOOL

def main():
    a = sys.argv[1:]
    dll, dbase, dsize, exe, ebase, esize = a[0], int(a[1], 16), int(a[2], 16), a[3], int(a[4], 16), int(a[5], 16)
    offs = a[6:]
    search = ";".join(os.path.dirname(p) for p in (dll, exe)) + r";D:\GAMEDEVNEW_wt_mail\Sources\Core\x64\Server Release;E:\SourceTuanLe\SourceVs22\bin\server"
    if not dbg.SymInitializeW(HPROC, search, False):
        print("SymInitialize loi", ctypes.GetLastError()); return
    bases = {}
    for tag, path, base, size in (("C", dll, dbase, dsize), ("G", exe, ebase, esize)):
        r = dbg.SymLoadModuleExW(HPROC, None, path, None, base, size, None, 0)
        print("nap %s %s -> base %X (loi %d)" % (tag, os.path.basename(path), r, ctypes.GetLastError() if not r else 0))
        bases[tag] = base
    for o in offs:
        tag, hx = o.split(":"); addr = bases[tag] + int(hx, 16)
        si = SYMBOL_INFOW(); si.SizeOfStruct = 88; si.MaxNameLen = 1024
        disp = ctypes.c_ulonglong(0)
        ok = dbg.SymFromAddrW(HPROC, addr, ctypes.byref(disp), ctypes.byref(si))
        ln = IMAGEHLP_LINEW64(); ln.SizeOfStruct = ctypes.sizeof(IMAGEHLP_LINEW64); d2 = W.DWORD(0)
        ok2 = dbg.SymGetLineFromAddrW64(HPROC, addr, ctypes.byref(d2), ctypes.byref(ln))
        print("%s+%-8s %-60s %s" % (tag, hx, (si.Name + "+0x%X" % disp.value) if ok else "(khong co ky hieu, loi %d)" % ctypes.GetLastError(),
                                    ("%s:%d" % (os.path.basename(ln.FileName or ""), ln.LineNumber)) if ok2 else ""))

if __name__ == "__main__":
    main()
