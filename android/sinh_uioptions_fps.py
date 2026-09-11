# -*- coding: utf-8 -*-
r"""[FPS 12/09] Sinh ui\ui3\uioptions.ini cho ban Android = ini goc cua PC + muc [Fps] / [Fps_Btn] (thanh khung hinh/giay,
6 nac: tu dong / 30 / 45 / 60 / 90 / 120) dat vao hang "Bo phim tat" (KUiOptions an hang do tren dien thoai).
Chep BYTE (ini co ten anh GBK + chu Viet TCVN3), chi noi them phan ASCII. Ghi vao: lop ghi de (git), D:\jx1_android_data (may ao)
va tuy chon goi may chu tai (tham so 1). Chay lai nhieu lan khong sao.

Dung:  python android\sinh_uioptions_fps.py [D:\jx1_android_data_dt_v4]
"""
import io
import os
import sys

GOC = os.path.dirname(os.path.abspath(__file__))
NGUON = [os.path.join(r"D:\jx1_android_data", "ui", "Ui3", "uioptions.ini"),
         r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\ui\Ui3\UiOptions.ini"]
DICH = [os.path.join(GOC, "du_lieu_ghi_de", "ui", "ui3", "uioptions.ini"),
        os.path.join(r"D:\jx1_android_data", "ui", "ui3", "uioptions.ini")]
if len(sys.argv) > 1:
    DICH.append(os.path.join(sys.argv[1], "ui", "ui3", "uioptions.ini"))

src = next((p for p in NGUON if os.path.isfile(p)), None)
if not src:
    raise SystemExit("khong thay ini goc: " + " | ".join(NGUON))
s = io.open(src, encoding="latin-1", newline="").read()
if "[Fps]" in s:
    raise SystemExit("ini goc da co [Fps]?? " + src)
nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
# anh nut keo: lay dung dong Image= cua [Sound_Btn] (ten GBK, khong go lai)
i = s.index("[Sound_Btn]")
j = s.index("[ShortcutSet]", i)
anh = next(ln for ln in s[i:j].split(nl) if ln.startswith("Image="))
them = nl.join([
    "; [FPS 12/09] thanh khung hinh/giay (Android): 0 tu dong theo man hinh, 1..5 = 30/45/60/90/120; chu ve bang ma (UiOptions.cpp)",
    "[Fps]", "Left=77", "Top=134", "Width=70", "Height=13", "Type=0", "Min=0", "Max=5", "PageSize=1", "SlideBegin=14", "SlideEnd=56", "",
    "[Fps_Btn]", "Left=0", "Top=0", "Width=27", "Height=13", "Up=0", "Down=0", "Over=0", "OverFrame=0", anh, "Trans=0", "", ""])
s2 = s[:j] + them + s[j:]
cao = lambda t: sum(1 for c in t if ord(c) >= 0x80)
if cao(s2) - cao(s) != cao(anh):
    raise SystemExit("so byte cao lech")
for p in DICH:
    os.makedirs(os.path.dirname(p), exist_ok=True)
    io.open(p, "w", encoding="latin-1", newline="").write(s2)
    print("da ghi:", p)
