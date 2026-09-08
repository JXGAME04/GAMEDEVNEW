# -*- coding: utf-8 -*-
"""Do bo nho tien trinh Game.exe dang chay, CHI DOC (khong ghi vao tien trinh):
 - commit (PrivateUsage), working set, private working set (USS) qua psutil
 - ban do vung nho qua VirtualQueryEx: MEM_PRIVATE / MEM_MAPPED / MEM_IMAGE, theo trang thai commit,
   liet ke cac vung private >= 4 MB va cac DLL lon
Dung: python do_ram_client.py [pid]"""
import sys, ctypes, ctypes.wintypes as W, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
try:
    import psutil
except ImportError:
    psutil = None

def find_pid():
    if len(sys.argv) > 1: return int(sys.argv[1])
    if psutil:
        for p in psutil.process_iter(["name"]):
            if (p.info["name"] or "").lower() == "game.exe": return p.pid
    return None

pid = find_pid()
if not pid: print("khong thay Game.exe"); sys.exit(1)
print("pid", pid)
if psutil:
    p = psutil.Process(pid)
    mi = p.memory_full_info()
    print("psutil: rss(WS)=%.0f MB  private(commit)=%.0f MB  uss(RAM rieng THAT)=%.0f MB  vms=%.0f MB  peak_wset=%.0f MB" % (
        mi.rss / 2**20, mi.private / 2**20, mi.uss / 2**20, mi.vms / 2**20, mi.peak_wset / 2**20))

# ---- VirtualQueryEx ----
k32 = ctypes.windll.kernel32
psapi = ctypes.windll.psapi
PROCESS_QUERY_INFORMATION = 0x0400; PROCESS_VM_READ = 0x0010
h = k32.OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, False, pid)
if not h: print("OpenProcess that bai", ctypes.get_last_error()); sys.exit(1)

class MBI(ctypes.Structure):
    _fields_ = [("BaseAddress", ctypes.c_void_p), ("AllocationBase", ctypes.c_void_p), ("AllocationProtect", W.DWORD),
                ("PartitionId", W.WORD), ("RegionSize", ctypes.c_size_t), ("State", W.DWORD), ("Protect", W.DWORD), ("Type", W.DWORD)]
MEM_COMMIT = 0x1000; MEM_RESERVE = 0x2000; MEM_FREE = 0x10000
MEM_PRIVATE = 0x20000; MEM_MAPPED = 0x40000; MEM_IMAGE = 0x1000000
k32.VirtualQueryEx.argtypes = [W.HANDLE, ctypes.c_void_p, ctypes.POINTER(MBI), ctypes.c_size_t]
k32.VirtualQueryEx.restype = ctypes.c_size_t
psapi.GetMappedFileNameW.argtypes = [W.HANDLE, ctypes.c_void_p, W.LPWSTR, W.DWORD]

def mapped_name(addr):
    buf = ctypes.create_unicode_buffer(520)
    n = psapi.GetMappedFileNameW(h, ctypes.c_void_p(addr), buf, 520)
    return buf.value if n else ""

addr = 0; limit = 0x7FFFFFFF if ctypes.sizeof(ctypes.c_void_p) == 4 else 0x7FFFFFFFFFFF
tot = collections.Counter(); alloc = collections.defaultdict(int); alloc_type = {}; alloc_prot = {}
images = collections.defaultdict(int); n = 0
mbi = MBI()
# tien trinh 32-bit: quet toi 4 GB (dung khi khong con vung)
while addr < 0x100000000:
    if not k32.VirtualQueryEx(h, ctypes.c_void_p(addr), ctypes.byref(mbi), ctypes.sizeof(mbi)): break
    size = mbi.RegionSize
    if mbi.State == MEM_COMMIT:
        key = {MEM_PRIVATE: "private", MEM_MAPPED: "mapped", MEM_IMAGE: "image"}.get(mbi.Type, "khac")
        tot[key] += size
        if mbi.Type == MEM_IMAGE:
            images[mapped_name(mbi.BaseAddress)] += size
        elif mbi.Type == MEM_PRIVATE:
            alloc[mbi.AllocationBase] += size
            alloc_prot[mbi.AllocationBase] = mbi.AllocationProtect
    elif mbi.State == MEM_RESERVE:
        tot["reserve"] += size
    else:
        tot["free"] += size
    n += 1
    addr = (mbi.BaseAddress or 0) + size
    if size == 0: break
k32.CloseHandle(h)
print("vung: %d | commit private %.0f MB, mapped %.0f MB, image %.0f MB | reserve %.0f MB | free %.0f MB (khong gian dia chi 2 GB)" % (
    n, tot["private"] / 2**20, tot["mapped"] / 2**20, tot["image"] / 2**20, tot["reserve"] / 2**20, tot["free"] / 2**20))
print("--- vung private (theo AllocationBase) >= 4 MB, lon nhat truoc ---")
big = sorted(alloc.items(), key=lambda kv: -kv[1])
s_big = 0
for base, sz in big:
    if sz < 4 * 2**20: break
    s_big += sz
    print("   base 0x%08X  %7.1f MB  prot=0x%02X" % (base, sz / 2**20, alloc_prot.get(base, 0)))
print("   tong >= 4 MB: %.0f MB trong %d vung; con lai (< 4 MB moi vung) %.0f MB" % (s_big / 2**20, sum(1 for _, v in big if v >= 4 * 2**20), (tot["private"] - s_big) / 2**20))
hist = collections.Counter()
for base, sz in big:
    b = "<64K" if sz < 65536 else "<1M" if sz < 2**20 else "<4M" if sz < 4 * 2**20 else "<16M" if sz < 16 * 2**20 else "<64M" if sz < 64 * 2**20 else ">=64M"
    hist[b] += sz
print("   phan bo private theo co vung:", ", ".join("%s=%.0fMB" % (k, v / 2**20) for k, v in sorted(hist.items(), key=lambda kv: -kv[1])))
print("--- image (DLL/EXE) lon nhat ---")
for name, sz in sorted(images.items(), key=lambda kv: -kv[1])[:12]:
    print("   %6.1f MB  %s" % (sz / 2**20, name.split("\\")[-1] if name else "?"))
