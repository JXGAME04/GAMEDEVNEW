#!/usr/bin/env python3
# -*- coding: ascii -*-
"""
goi_va_ai_7_10_0609.py

[AI710 06/09] Bat lai bon kieu AI quai 7/8/9/10 trong KNpcAI::Activate.

Ly do (do tu nhi phan D:/ServerLinux/server1/jx_linux_y):
  KNpcAI::Activate 0x08095780, bang nhay 0x08255340 co du 25 muc (AIMode 0..24).
  Ban Linux chay du AI 1..10 va 21..24:
     AI7  0x08094040 | AI8  0x0808F1C0 | AI9  0x08092E30 | AI10 0x08091EB0
  JX1 chu thich 'case 7..10' nen nhung con quai khai AIMode 7-10 KHONG CHAY AI NAO.
  Do trong settings/npcs.txt cua JX1: AIMode 9 = 50 con, AIMode 10 = 62 con
  => 112 con quai dung im tai diem sinh.

Bon ham ProcessAIType7/8/9/10 DA CO SAN va day du trong KNpcAI.cpp
(dong 2481 / 2549 / 2619 / 2687). Chi thieu duong goi.
Ca bon deu chia cho m_CurrentLifeMax, va Activate da chan
'if (m_CurrentLifeMax == 0) return;' o dau ham nen khong co chia 0.

CHI THEM duong goi - khong sua mot dong nao cua bon ham do.
Chay lai duoc (idempotent). Doc/ghi latin-1, giu nguyen EOL va byte TCVN3.
"""
import io
import os
import shutil

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KNpcAI.cpp"
NHAN = "[AI710 06/09]"


def main():
    d0 = io.open(P, "r", encoding="latin-1", newline="").read()
    if NHAN in d0:
        print("KNpcAI.cpp: da va roi, bo qua")
        return
    nl = "\r\n" if "\r\n" in d0 else "\n"

    cu_mo = "/*\t\tcase 7:"
    cu_dong = "\t\t\tbreak;*/"
    assert d0.count(cu_mo) == 1, "neo mo khong duy nhat: %d" % d0.count(cu_mo)
    assert d0.count(cu_dong) == 1, "neo dong khong duy nhat: %d" % d0.count(cu_dong)

    b = P + ".truoc_ai710_0609"
    if not os.path.exists(b):
        shutil.copyfile(P, b)
        print("  sao luu -> " + os.path.basename(b))

    moi_mo = ("\t\t// " + NHAN + " ban Linux chay du AI 1-10 (bang nhay 0x08255340)." + nl +
              "\t\t// JX1 chu thich 4 case nay nen 112 con quai khai AIMode 9/10 khong co AI." + nl +
              "\t\t// Bon ham duoi day da co san trong tep nay, chi thieu duong goi." + nl +
              "\t\tcase 7:")
    d = d0.replace(cu_mo, moi_mo).replace(cu_dong, "\t\t\tbreak;")

    hb_t = sum(1 for c in d0 if ord(c) >= 0x80)
    hb_s = sum(1 for c in d if ord(c) >= 0x80)
    assert hb_t == hb_s, "so byte cao doi %d -> %d" % (hb_t, hb_s)
    assert "\xef\xbf\xbd" not in d, "xuat hien FFFD"

    io.open(P, "w", encoding="latin-1", newline="").write(d)
    print("KNpcAI.cpp: da bat case 7/8/9/10")


if __name__ == "__main__":
    main()
