# -*- coding: utf-8 -*-
"""vhtd_engine_patch2.py - [VHTD 02/09d] ĐỔI GIAO THỨC theo lệnh chủ 02/09: WeaponType BYTE -> WORD trong PLAYER_SYNC (KProtocol.h:39)
và PLAYER_NORMAL_SYNC (KProtocol.h:435); bỏ ép (BYTE) ở KNpc.cpp:6293/6498. Lý do: res vũ khí thuẫn/cầm 268/269/276 > 255 -> người chơi
khác nhận 12/13/20 (sai hình). #pragma pack(1) -> mỗi gói dài thêm 1 byte; client + server (CoreClient/CoreServer) PHẢI swap cùng lúc.
WAuto cây dự án (E:\\Src_Auto_Ngoai\\WAuto\\WAuto) dùng IPC bộ nhớ chung, không đọc gói này (grep WeaponType = 0) -> không cần build lại.
Doc/ghi latin-1. Idempotent. DUNG: python vhtd_engine_patch2.py [--kiem]
"""
import io, os, sys
ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
KIEM = "--kiem" in sys.argv
MD = "[VHTD 02/09d]"
hib = lambda t: sum(1 for c in t if ord(c) >= 0x80)

class F:
    def __init__(self, name):
        self.p = os.path.join(ROOT, name); self.name = name
        self.s = io.open(self.p, "r", encoding="latin-1", newline="").read(); self.orig = self.s; self.n = 0
    def rep(self, old, new, tag, count=1):
        c = self.s.count(old)
        if c == 0:
            if new in self.s: print("  [=] %s: da ap (%s)" % (self.name, tag)); return
            raise SystemExit("KHONG THAY anchor %s (%s):\n%r" % (self.name, tag, old[:150]))
        if c != count: raise SystemExit("anchor %s (%s) xuat hien %d lan (cho %d)" % (self.name, tag, c, count))
        self.s = self.s.replace(old, new); self.n += 1; print("  [+] %s: %s (x%d)" % (self.name, tag, c))
    def save(self):
        if self.s == self.orig: print("  (khong doi) %s" % self.name); return
        if hib(self.s) != hib(self.orig) or "\xef\xbf\xbd" in self.s: raise SystemExit("LECH BYTE CAO %s" % self.name)
        if not KIEM: io.open(self.p, "w", encoding="latin-1", newline="").write(self.s)
        print("  => ghi %s (%d cho)%s" % (self.name, self.n, " KIEM" if KIEM else ""))

h = F("KProtocol.h")
h.rep("\r\n\tBYTE\tWeaponType;\r\n", "\r\n\tWORD\tWeaponType;\t// " + MD + " BYTE -> WORD: res vu khi thuan/cam 268/269/276 > 255 (PLAYER_SYNC + PLAYER_NORMAL_SYNC, pack(1) -> +1 byte; client+server swap cung luc)\r\n", "WeaponType WORD", count=2)
h.save()
k = F("KNpc.cpp")
k.rep("\t\tPlayerSync.WeaponType\t\t= (BYTE)m_WeaponType;\r\n", "\t\tPlayerSync.WeaponType\t\t= (WORD)m_WeaponType;\t// " + MD + "\r\n", "PLAYER_SYNC ep WORD")
k.rep("\t\tPlayerSync.WeaponType\t= (BYTE)m_WeaponType;\r\n", "\t\tPlayerSync.WeaponType\t= (WORD)m_WeaponType;\t// " + MD + "\r\n", "PLAYER_NORMAL_SYNC ep WORD")
k.save()
print("XONG%s." % (" (KIEM)" if KIEM else ""))
