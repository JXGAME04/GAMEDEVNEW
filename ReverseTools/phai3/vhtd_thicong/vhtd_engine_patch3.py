# -*- coding: utf-8 -*-
"""vhtd_engine_patch3.py [VHTD 02/09e] - sửa theo test chủ 02/09:
  1. SkillDef.h MAX_SKILL 2000 -> 2300: kỹ năng Tiêu Dao id 2114–2143 >= 2000 bị KSkillManager bỏ qua (m_SkillInfo[MAX_SKILL],
     m_pOrdinSkill[MAX_SKILL][MAX_SKILLLEVEL]) -> AddMagic thất bại, bảng kỹ năng Tiêu Dao trống. (+~150 KB con trỏ)
  2. KSkillList.cpp MAX_FIGHTSKILL_SORTLIST 50 -> 100 + S3Client UiSkillsNew.h / UiSkills.h FIGHT_SKILL_COUNT, FIGHT_SKILL_UI_MAX 50 -> 100:
     nhân vật test học nhiều phái > 50 chiêu chiến đấu -> chiêu học sau (Tiêu Dao) không vào danh sách GDI_FIGHT_SKILLS.
Doc/ghi latin-1. Idempotent. DUNG: python vhtd_engine_patch3.py [--kiem]
"""
import io, os, sys
ROOT = r"D:\GAMEDEVNEW\Sources"
KIEM = "--kiem" in sys.argv
ME = "[VHTD 02/09e]"
hib = lambda t: sum(1 for c in t if ord(c) >= 0x80)

class F:
    def __init__(self, rel):
        self.p = os.path.join(ROOT, rel); self.name = rel
        self.s = io.open(self.p, "r", encoding="latin-1", newline="").read(); self.orig = self.s; self.n = 0
    def rep(self, old, new, tag):
        c = self.s.count(old)
        if c == 0:
            if new in self.s: print("  [=] %s: da ap (%s)" % (self.name, tag)); return
            raise SystemExit("KHONG THAY anchor %s (%s):\n%r" % (self.name, tag, old[:150]))
        if c != 1: raise SystemExit("anchor khong duy nhat (%d) %s (%s)" % (c, self.name, tag))
        self.s = self.s.replace(old, new); self.n += 1; print("  [+] %s: %s" % (self.name, tag))
    def save(self):
        if self.s == self.orig: print("  (khong doi) %s" % self.name); return
        if hib(self.s) != hib(self.orig) or "\xef\xbf\xbd" in self.s: raise SystemExit("LECH BYTE CAO %s" % self.name)
        if not KIEM: io.open(self.p, "w", encoding="latin-1", newline="").write(self.s)
        print("  => ghi %s (%d cho)%s" % (self.name, self.n, " KIEM" if KIEM else ""))

f = F(r"Core\Src\SkillDef.h")
f.rep("#define MAX_SKILL\t\t\t\t\t\t\t2000\r\n", "#define MAX_SKILL\t\t\t\t\t\t\t2300\t// " + ME + " 2000 -> 2300: ky nang Tieu Dao 2114-2143 (client VLTK) - KSkillManager bo qua id >= MAX_SKILL\r\n", "MAX_SKILL 2300")
f.save()
f = F(r"Core\Src\KSkillList.cpp")
f.rep("#define MAX_FIGHTSKILL_SORTLIST 50\r\n", "#define MAX_FIGHTSKILL_SORTLIST 100\t// " + ME + " 50 -> 100 (khop FIGHT_SKILL_COUNT S3Client): nhan vat hoc nhieu phai > 50 chieu\r\n", "MAX_FIGHTSKILL_SORTLIST 100")
f.save()
for rel in (r"S3Client\Ui\UiCase\UiSkillsNew.h", r"S3Client\Ui\UiCase\UiSkills.h"):
    f = F(rel)
    f.rep("#define\tFIGHT_SKILL_COUNT\t\t\t50\r\n", "#define\tFIGHT_SKILL_COUNT\t\t\t100\t// " + ME + " khop MAX_FIGHTSKILL_SORTLIST (KSkillList.cpp)\r\n", "FIGHT_SKILL_COUNT 100")
    if "FIGHT_SKILL_UI_MAX\t\t\t50" in f.s:
        f.rep("#define FIGHT_SKILL_UI_MAX\t\t\t50\r\n", "#define FIGHT_SKILL_UI_MAX\t\t\t100\t// " + ME + "\r\n", "FIGHT_SKILL_UI_MAX 100")
    f.save()
print("XONG%s." % (" (KIEM)" if KIEM else ""))
