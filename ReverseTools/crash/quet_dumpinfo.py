# quet_dumpinfo.py - kiem ke thu muc DumpInfo cua GameServer va mo N dump moi nhat.
#
# Dung:
#   python quet_dumpinfo.py                      -> thu muc mac dinh, mo 3 dump moi nhat
#   python quet_dumpinfo.py <thu_muc> [so_dump]  -> chi dinh thu muc / so luong
#
# Ten dump do GameServer.cpp GenerateMiniDump() sinh ra:
#   DumpInfo\g-v1.0-YYYYMMDD-HHMMSS-<pid><tid>.dmp   (kieu MiniDumpWithFullMemory -> vai GB moi tep)
# Script chi doc vai KB dau + vung stack cua thread loi, KHONG doc het tep.
#
# Ket qua: in ra man hinh + ghi bao_cao_dumpinfo.txt trong thu muc dang chay.
# Muon co ten ham + so dong: chay tiep lenh giai_ma.py ma script in san o cuoi moi dump
# (can PDB dung ban build: Sources\Core\x64\Server Release\CoreServer.pdb).
import os, sys, re, datetime, traceback

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from doc_dump import phan_tich, ten_ngan

THU_MUC_MAC_DINH = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\DumpInfo"
RE_TEN = re.compile(r"-(\d{8})-(\d{6})-(\d+)\.dmp$", re.I)

class Ghi(object):
    """In ra man hinh va ghi vao tep bao cao cung luc."""
    def __init__(self, duong_dan):
        self.f = open(duong_dan, "w", encoding="utf-8", errors="replace")
        self.cu = sys.stdout
    def write(self, s):
        self.cu.write(s); self.f.write(s)
    def flush(self):
        self.cu.flush(); self.f.flush()

def co_nguoi(n):
    for don in ("B", "KB", "MB", "GB", "TB"):
        if n < 1024 or don == "TB":
            return "%.1f %s" % (n, don)
        n /= 1024.0

def gio_trong_ten(ten):
    m = RE_TEN.search(ten)
    if not m:
        return None
    try:
        return datetime.datetime.strptime(m.group(1) + m.group(2), "%Y%m%d%H%M%S")
    except ValueError:
        return None

def lenh_giai_ma(thu_muc_bin, info):
    """Dung san dong lenh giai_ma.py tu base/size va cac khung doc duoc."""
    base = {}
    for b, sz, ten in info["mods"]:
        bn = ten_ngan(ten).lower()
        if bn in ("coreserver.dll", "gameserver.exe"):
            base[bn] = (b, sz)
    if "coreserver.dll" not in base or "gameserver.exe" not in base:
        return "   (khong thay du coreserver.dll + gameserver.exe trong dump -> bo qua giai_ma)"
    offs, da_co = [], set()
    for ten, off in info["khung"]:
        tag = "C" if ten.lower() == "coreserver.dll" else "G" if ten.lower() == "gameserver.exe" else None
        if tag and (tag, off) not in da_co:
            da_co.add((tag, off)); offs.append("%s:0x%X" % (tag, off))
        if len(offs) >= 30:
            break
    cb, cs = base["coreserver.dll"]
    gb, gs = base["gameserver.exe"]
    giai_ma = os.path.join(os.path.dirname(os.path.abspath(__file__)), "giai_ma.py")
    return ('   python "%s" "%s" %X %X "%s" %X %X %s'
            % (giai_ma, os.path.join(thu_muc_bin, "CoreServer.dll"), cb, cs,
               os.path.join(thu_muc_bin, "GameServer.exe"), gb, gs, " ".join(offs)))

def main():
    thu_muc = sys.argv[1] if len(sys.argv) > 1 else THU_MUC_MAC_DINH
    so_dump = int(sys.argv[2]) if len(sys.argv) > 2 else 3
    bao_cao = os.path.abspath("bao_cao_dumpinfo.txt")
    sys.stdout = Ghi(bao_cao)

    print("=" * 78)
    print("QUET DUMPINFO: %s" % thu_muc)
    print("=" * 78)
    if not os.path.isdir(thu_muc):
        print("[LOI] khong co thu muc nay. Truyen duong dan dung lam tham so 1.")
        return 2

    tep = []
    for ten in os.listdir(thu_muc):
        if ten.lower().endswith(".dmp"):
            dd = os.path.join(thu_muc, ten)
            try:
                st = os.stat(dd)
            except OSError:
                continue
            tep.append((dd, ten, st.st_size, st.st_mtime))
    if not tep:
        print("Khong co tep .dmp nao -> may chu chua sap ke tu lan don gan nhat.")
        return 0

    # sap theo GIO SAP trong ten tep; tep khong theo mau thi lay ngay sua doi
    # (dump da tung duoc chuyen sang bin\_luutru nen mtime co the khong con dung)
    def khoa(x):
        g = gio_trong_ten(x[1])
        return g.timestamp() if g else x[3]
    tep.sort(key=khoa, reverse=True)
    tong = sum(x[2] for x in tep)
    print("So dump: %d   tong dung luong: %s" % (len(tep), co_nguoi(tong)))
    print()
    print("%-52s %10s  %s" % ("TEN TEP", "CO", "GIO SAP (theo ten tep)"))
    theo_ngay = {}
    for dd, ten, sz, mt in tep:
        g = gio_trong_ten(ten)
        print("%-52s %10s  %s" % (ten[:52], co_nguoi(sz),
                                  g.strftime("%d/%m/%Y %H:%M:%S") if g else "(ten khong theo mau)"))
        if g:
            theo_ngay.setdefault(g.strftime("%d/%m/%Y"), []).append(g)
    print()
    print("--- so lan sap theo ngay ---")
    for ngay in sorted(theo_ngay, key=lambda d: datetime.datetime.strptime(d, "%d/%m/%Y")):
        gio = sorted(theo_ngay[ngay])
        print("  %s : %2d lan  (%s)" % (ngay, len(gio), ", ".join(x.strftime("%H:%M:%S") for x in gio[:12])))

    thu_muc_bin = os.path.dirname(os.path.abspath(os.path.normpath(thu_muc)))
    for dd, ten, sz, mt in tep[:so_dump]:
        print()
        print("=" * 78)
        print("MO DUMP: %s  (%s)" % (ten, co_nguoi(sz)))
        print("=" * 78)
        try:
            info = phan_tich(dd)
            print()
            print("   [lenh lay ten ham + so dong cho dump nay]")
            print(lenh_giai_ma(thu_muc_bin, info))
        except Exception:
            print("[LOI] khong doc duoc dump nay:")
            traceback.print_exc(file=sys.stdout)

    print()
    print("Bao cao da ghi: %s" % bao_cao)
    return 0

if __name__ == "__main__":
    sys.exit(main())
