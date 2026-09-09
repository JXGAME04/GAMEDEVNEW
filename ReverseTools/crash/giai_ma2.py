# -*- coding: utf-8 -*-
"""Nhu giai_ma.py nhung BAT SYMOPT_LOAD_ANYTHING (0x40) de nap PDB du GUID KHONG khop
(PDB da bi ban build sau ghi de). PHAI kiem chung bang moc da biet truoc khi tin ten ham.
Dung: python giai_ma2.py <pdb> <dll> <base> <size> <rva>...
"""
import ctypes, ctypes.wintypes as W, sys, os
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
dbg = ctypes.WinDLL("dbghelp")
H = ctypes.c_void_p(0x7777)
SYMOPT = 0x2 | 0x10 | 0x80000 | 0x40 | 0x4000000     # UNDNAME|LOAD_LINES|NO_PROMPTS|LOAD_ANYTHING|DEBUG(0)
dbg.SymSetOptions.restype = W.DWORD
dbg.SymSetOptions(0x2 | 0x10 | 0x80000 | 0x40)
dbg.SymInitializeW.argtypes = [ctypes.c_void_p, ctypes.c_wchar_p, W.BOOL]; dbg.SymInitializeW.restype = W.BOOL
dbg.SymLoadModuleExW.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_wchar_p, ctypes.c_wchar_p,
                                 ctypes.c_ulonglong, W.DWORD, ctypes.c_void_p, W.DWORD]
dbg.SymLoadModuleExW.restype = ctypes.c_ulonglong

class SI(ctypes.Structure):
    _fields_ = [("SizeOfStruct", W.DWORD), ("TypeIndex", W.DWORD), ("Reserved", ctypes.c_ulonglong * 2),
                ("Index", W.DWORD), ("Size", W.DWORD), ("ModBase", ctypes.c_ulonglong), ("Flags", W.DWORD),
                ("Value", ctypes.c_ulonglong), ("Address", ctypes.c_ulonglong), ("Register", W.DWORD),
                ("Scope", W.DWORD), ("Tag", W.DWORD), ("NameLen", W.DWORD), ("MaxNameLen", W.DWORD),
                ("Name", ctypes.c_wchar * 2048)]
class LN(ctypes.Structure):
    _fields_ = [("SizeOfStruct", W.DWORD), ("Key", ctypes.c_void_p), ("LineNumber", W.DWORD),
                ("FileName", ctypes.c_wchar_p), ("Address", ctypes.c_ulonglong)]
dbg.SymFromAddrW.argtypes = [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.POINTER(ctypes.c_ulonglong), ctypes.POINTER(SI)]
dbg.SymFromAddrW.restype = W.BOOL
dbg.SymGetLineFromAddrW64.argtypes = [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.POINTER(W.DWORD), ctypes.POINTER(LN)]
dbg.SymGetLineFromAddrW64.restype = W.BOOL

pdb, dll, base, size = sys.argv[1], sys.argv[2], int(sys.argv[3], 16), int(sys.argv[4], 16)
dbg.SymInitializeW(H, os.path.dirname(pdb), False)
r = dbg.SymLoadModuleExW(H, None, pdb, None, base, size, None, 0)
print("nap PDB %s -> base %X\n" % (os.path.basename(pdb), r))
for a in sys.argv[5:]:
    rva = int(a, 16); addr = base + rva
    si = SI(); si.SizeOfStruct = 88; si.MaxNameLen = 2048
    d = ctypes.c_ulonglong(0)
    ok = dbg.SymFromAddrW(H, addr, ctypes.byref(d), ctypes.byref(si))
    ln = LN(); ln.SizeOfStruct = ctypes.sizeof(LN); d2 = W.DWORD(0)
    ok2 = dbg.SymGetLineFromAddrW64(H, addr, ctypes.byref(d2), ctypes.byref(ln))
    nm = ("%s+0x%X" % (si.Name, d.value)) if ok else "(khong co ky hieu)"
    src = ("  %s:%d" % (os.path.basename(ln.FileName or ""), ln.LineNumber)) if ok2 else ""
    print("  rva %-9s %-64s%s" % (hex(rva), nm[:64], src))
