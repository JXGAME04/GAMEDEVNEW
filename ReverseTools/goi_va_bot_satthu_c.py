# -*- coding: utf-8 -*-
r"""goi_va_bot_satthu_c.py - [SATTHU dot c 14/09] bit not duong bot VAN KEO TOI CHO BOSS SAT THU.

Chu game hoi lai: "toi noi khong cho bot danh boss sat thu ban da lam dung chua?" -> soat lai thay dot a/b
moi chan duong DANH (pb_FindTarget khong chon + pb_Fight nha muc tieu), CON duong DI TOI thi chua:

  pb_NapMotNpc (bang cum diem sinh quai) chi loc kind_normal + m_OriginX > 0 - boss Sat Thu thoa CA HAI
  nen duoc nap vao bang cum; boss lai thuong dung XA cac bai quai nen no TU TAO MOT CUM RIENG.
  pb_FindRoamSpot gom MOI tam cum lam ung vien diem di hoang roi boc theo chi so bot => bot van chon
  cho boss lam diem den, toi noi thi khong co gi de danh (boss da bi chan) -> dung i quanh boss.
  Mat chu game nhin vao van la "bot bu quanh boss sat thu".

Va (hai cho, cung dung pb_LaBossSatThu da co tu dot a):
  1. pb_NapMotNpc      : boss Sat Thu KHONG duoc vao bang cum (khong tao cum, khong tinh vao aDem).
  2. pb_FindRoamSpot   : nhanh DU PHONG (quet quai dang song khi ban do chua co bang cum) cung bo qua boss.

Khong dong toi quai thuong: pb_LaBossSatThu loc mau >= 20.000 truoc roi moi so chuoi ActionScript.
Idempotent (dau "[SATTHU dot c]"); --thu = chi kiem neo. Doc/ghi latin-1, giu kieu xuong dong.
"""
import io
import os
import re
import sys

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerBot.cpp"
THU = "--thu" in sys.argv


def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)


class Hunks(object):
    def __init__(self, path):
        self.path = path
        self.s = io.open(path, "r", encoding="latin-1", newline="").read()
        self.crlf = "\r\n" in self.s
        self.n = 0
        self.err = 0

    def sub(self, pat, new, count=1):
        p = "\r?\n".join(re.escape(x) for x in pat.split("\n"))
        found = len(re.findall(p, self.s, re.M))
        if found != count:
            print("  NEO KHONG KHOP (%d thay vi %d): %s" % (found, count, pat[:70].replace("\n", "|")))
            self.err += 1
            return
        nl = (lambda t: t.replace("\n", "\r\n")) if self.crlf else (lambda t: t)
        self.s = re.sub(p, lambda m: nl(new), self.s, count=0, flags=re.M)
        self.n += found

    def done(self, name):
        if self.err:
            print("%s: %d neo hong -> KHONG GHI" % (name, self.err))
            return False
        if THU:
            print("%s: %d hunk OK (thu)" % (name, self.n))
            return True
        s0 = io.open(self.path, "r", encoding="latin-1", newline="").read()
        if hb(s0) != hb(self.s):
            print("%s: SO BYTE CAO DOI %d -> %d -> KHONG GHI" % (name, hb(s0), hb(self.s)))
            return False
        io.open(self.path, "w", encoding="latin-1", newline="").write(self.s)
        print("%s: AP %d hunk, high-byte %d, %d -> %d byte" % (name, self.n, hb(self.s), len(s0), len(self.s)))
        return True


# ------------------------------------------------------------------ 1. bang cum diem sinh quai
C1_NEO = """	if (Npc[i].m_Kind != kind_normal)            return;
	const int ox = Npc[i].m_OriginX, oy = Npc[i].m_OriginY;
"""
C1_NEW = """	if (Npc[i].m_Kind != kind_normal)            return;
	// [SATTHU dot c 14/09] boss nhiem vu Sat Thu KHONG duoc vao bang cum. Bang cum vua la "san choi"
	// cua pb_FindTarget vua la DANH SACH DIEM DI HOANG cua pb_FindRoamSpot; boss dung xa cac bai quai
	// nen no tu tao MOT CUM RIENG, va bot van boc cum do lam diem den roi dung i quanh boss (khong
	// danh duoc vi da bi chan o pb_FindTarget). Chu game: "khong cho bot danh boss sat thu" - vay thi
	// cung dung keo bot toi do.
	if (pb_LaBossSatThu(i))                     return;
	const int ox = Npc[i].m_OriginX, oy = Npc[i].m_OriginY;
"""

# ------------------------------------------------------------------ 2. nhanh du phong cua di hoang
C2_NEO = """			if (Npc[i].m_Kind == kind_player)                continue;
			if (!(NpcSet.GetRelation(nNpcIdx, i) & relation_enemy)) continue;
"""
C2_NEW = """			if (Npc[i].m_Kind == kind_player)                continue;
			// [SATTHU dot c 14/09] khong lay boss Sat Thu lam diem di hoang (xem pb_NapMotNpc)
			if (pb_LaBossSatThu(i))                          continue;
			if (!(NpcSet.GetRelation(nNpcIdx, i) & relation_enemy)) continue;
"""


def main():
    if not os.path.isfile(SRC):
        print("KHONG THAY: " + SRC)
        return 2
    h = Hunks(SRC)
    if "[SATTHU dot c" in h.s:
        print("KPlayerBot.cpp: DA AP")
        return 0
    if "pb_LaBossSatThu" not in h.s:
        print("CHUA AP dot a - chay goi_va_bot_satthu_tklac.py truoc")
        return 3
    h.sub(C1_NEO, C1_NEW)
    h.sub(C2_NEO, C2_NEW)
    ok = h.done("KPlayerBot.cpp")
    print("KET QUA: %s" % ("OK" if ok else "CO LOI"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
