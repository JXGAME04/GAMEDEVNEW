# -*- coding: utf-8 -*-
"""t58_duong_nap_thu8_mapdata.py - DUONG NAP THU 8: script gan vao NPC/bay bang
DU LIEU BAN DO trong Pak.

VI SAO CAN: t57 quet 7 duong nap "van ban" (Include/AddNpc/settings/C++...) va
bao 1675/3065 tep la mo coi - con so do KHONG TIN DUOC. Nghi ngo dung: rat nhieu
NPC va bay duoc dat san trong DU LIEU BAN DO, va duong dan script cua chung nam
trong cac khoi DA NEN ben trong bin\\server\\Pak\\*.pak - grep van ban khong thay.

Cong cu nay giai nen TOAN BO entry cua cac pak may chu doc, roi trich moi chuoi
ket thuc .lua. Do la tap "duong nap thu 8".

Ghi chu: settings\\gamesetting.ini:259 NotAddNpcNormal=1 nghia la may chu BO QUA
npc "normal" trong du lieu map (KRegion.cpp:270,483) - nhung bay (trap), obj va
npc khac loai thi VAN nap. Nen khong the vin vao co do de coi tat ca la chet.

CHI DOC.
"""
import io
import os
import re
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
RA = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                  "duongnap_mapdata.txt")

PAK = ["Pak\\maps.pak", "Pak\\maps_client.pak",
       "Pak\\maps_banghuichengbao.pak", "Pak\\maps_tieu_bang_chien.pak",
       "Pak\\namcung.pak"]

RE_LUA = re.compile(rb"[\x20-\x7e\x80-\xff]{2,160}?\.lua", re.I)


def main():
    try:
        import pakdump
    except Exception as e:
        print("!!! khong nap duoc pakdump: %s" % e)
        return 1

    # pakdump.read_entry chi biet zlib; pak nay nen bang UCL NRV2B nen khi zlib
    # that bai no TRA VE NGUYEN KHOI DA NEN -> trich ra ten rac
    # ("-=8a\xb6\xf4\xbb\xfc\xb0.lua"). Phai giai nen bang ucl.
    import ucl

    def giai_nen(f, e):
        uid, off, size, cf = e
        csize = cf & 0xFFFFFF
        flag = cf >> 24
        f.seek(off)
        blob = f.read(csize if csize else size)
        if flag == 0 or csize in (0, size):
            return blob[:size] if size > 0 else blob
        try:
            return ucl.nrv2b_decompress_8(blob, size)
        except Exception:
            try:
                import zlib
                return zlib.decompress(blob)
            except Exception:
                return b""      # KHONG tra khoi nen thô - se sinh ten rac

    tat = set()
    for rel in PAK:
        p = os.path.join(SV, rel)
        if not os.path.isfile(p):
            print("  %-38s (khong co)" % rel)
            continue
        try:
            f, es = pakdump.entries(p)
        except Exception as e:
            print("  %-38s LOI doc chi muc: %s" % (rel, e))
            continue
        n_ok = n_loi = 0
        tim = set()
        for e in es:
            try:
                b = giai_nen(f, e)
                n_ok += 1
            except Exception:
                n_loi += 1
                continue
            if not b:
                continue
            for m in RE_LUA.finditer(b):
                s = m.group(0)
                # cat phan rac dau chuoi: lay tu dau gach cuoi cung
                k = max(s.rfind(b"\\"), s.rfind(b"/"))
                ten = s[k + 1:] if k >= 0 else s
                if 4 < len(ten) < 80:
                    tim.add(ten.decode("latin-1").lower())
        f.close()
        print("  %-38s %5d entry (%d loi giai nen) -> %d ten .lua"
              % (rel, len(es), n_loi, len(tim)))
        tat |= tim

    print()
    print("=> TONG %d ten tep .lua duoc du lieu ban do nhac toi" % len(tat))
    with io.open(RA, "w", encoding="utf-8", newline="") as f:
        f.write("DUONG NAP THU 8 - ten .lua nam trong DU LIEU BAN DO (Pak)\n")
        f.write("Sinh boi ReverseTools/cauhinh/t58_duong_nap_thu8_mapdata.py\n")
        f.write("=" * 78 + "\n\n")
        for x in sorted(tat):
            f.write(x + "\n")
    print("=> %s" % RA)
    return 0


if __name__ == "__main__":
    sys.exit(main())
