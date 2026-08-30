# -*- coding: utf-8 -*-
"""t08_liet_ke_thua.py - THI CONG DOT 7: LIET KE tep thua trong cay script.

CHI DOC - KHONG XOA, KHONG DOI TEN GI CA.
Xoa tep la viec khong hoi lai duoc, nen tep nay chi lam ra DANH SACH de chu
game duyet. Muon don that thi dung `--dodi` (doi vao thu muc `_dara\\` cung
cay, van hoi lai duoc bang cach chep nguoc).

BON NHOM DUOC DEM:
  1) BAN SAO LUU cua cac dot va: duoi .truoc_*, .cu, .bak, .old, .tuche_*
     (moi bo va deu tu sinh mot ban - lau ngay chat cay)
  2) TEP TRUNG BYTE: hai tep noi dung y het nhau (bam MD5)
  3) TEP CHET: .lua khong tep nao Include/dofile/DynamicExecute/ReLoadScript
     toi, va cung khong phai tep vao (script NPC/item goi theo ten trong bang
     settings). PHAI DOI CHIEU CA settings truoc khi ket luan.
  4) TEP RONG hoac chi co chu thich

!! CANH BAO VE NHOM 3: mot tep .lua co the duoc goi qua TEN trong bang settings
   (npcs.txt cot ActionScript, items o magicscript, TimerTask.txt...) chu khong
   qua Include. Tep nay CO doi chieu voi toan bo settings, nhung van co the sot
   duong goi dong (ghep chuoi luc chay). Vi vay nhom 3 chi la GOI Y, phai xem
   tay truoc khi don.
"""
import hashlib
import io
import os
import re
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
S = os.path.join(SV, "script")
SET = os.path.join(SV, "settings")
RA = os.path.join(os.path.dirname(os.path.abspath(__file__)), "tep_thua.txt")
DARA = os.path.join(SV, "_dara")

DUOI_SAO_LUU = re.compile(
    r"\.(truoc_[A-Za-z0-9_]+|cu|bak|old|orig|tuche_[A-Za-z0-9_]+|moi_[A-Za-z0-9_]+)$",
    re.I)
NAP = re.compile(
    r"(?:Include|dofile|DynamicExecute|DynamicExecuteByPlayer|ReLoadScript"
    r"|SetNpcDropScript|ExecuteScript\w*)\s*\(\s*[\"']([^\"']+)",
    re.I)


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def chuan_duong(s):
    """Chuan hoa mot duong dan script ve dang 'thumuc\\ten.lua'.

    BAY: trong tep .lua duong dan viet BACKSLASH DOI (vi Lua phai escape):
         Include("\\\\script\\\\activitysys\\\\functionlib.lua")
    Doc tho ra la  \\\\script\\\\activitysys\\\\functionlib.lua
    Neu khong gop backslash lien tiep lai thi doi chieu SAI HET - lan chay
    dau bao 2316/3104 tep "khong ai nap", trong do co ca functionlib.lua
    von duoc Include that.
    """
    s = s.replace("/", "\\").lower().strip()
    s = re.sub(r"\\+", "\\\\", s)      # gop \\\\ -> \\
    s = s.lstrip("\\")
    if s.startswith("script\\"):
        s = s[len("script\\"):]
    return s


def main():
    dodi = "--dodi" in sys.argv[1:]
    print("=== t08_liet_ke_thua - %s ==="
          % ("DOI VAO _dara" if dodi else "CHI LIET KE"))

    tat_ca = []
    for goc, _, tep in os.walk(S):
        for t in tep:
            tat_ca.append(os.path.join(goc, t))
    print("  tong %d tep trong cay script" % len(tat_ca))

    # ---- nhom 1: ban sao luu ----
    sao_luu = [p for p in tat_ca if DUOI_SAO_LUU.search(p)]

    lua = [p for p in tat_ca if p.lower().endswith(".lua")
           and not DUOI_SAO_LUU.search(p)]
    print("  trong do %d tep .lua that su" % len(lua))

    # ---- nhom 2: trung byte ----
    theo_bam = {}
    for p in lua:
        try:
            b = hashlib.md5(io.open(p, "rb").read()).hexdigest()
        except OSError:
            continue
        theo_bam.setdefault(b, []).append(p)
    trung = {k: v for k, v in theo_bam.items() if len(v) > 1}

    # ---- nhom 4: rong / chi chu thich ----
    rong = []
    for p in lua:
        try:
            t = doc(p)
        except OSError:
            continue
        con = re.sub(r"--[^\n]*", "", t).strip()
        if con == "":
            rong.append(p)

    # ---- nhom 3: khong ai nap ----
    duoc_nap = set()
    for p in tat_ca:
        if not p.lower().endswith((".lua", ".txt", ".ini")):
            continue
        try:
            t = doc(p)
        except OSError:
            continue
        for m in NAP.finditer(t):
            duoc_nap.add(chuan_duong(m.group(1)))
        # ten tep tho trong chuoi (duong goi ghep)
        for m in re.finditer(r"[\w\\/]+\.lua", t):
            duoc_nap.add(chuan_duong(m.group(0)))

    # doi chieu ca cay settings (npcs.txt ActionScript, TimerTask.txt...)
    if os.path.isdir(SET):
        for goc, _, tep in os.walk(SET):
            for t in tep:
                if not t.lower().endswith((".txt", ".ini")):
                    continue
                try:
                    tt = doc(os.path.join(goc, t))
                except OSError:
                    continue
                for m in re.finditer(r"[\w\\/]+\.lua", tt):
                    duoc_nap.add(chuan_duong(m.group(0)))

    chet = []
    for p in lua:
        rel = chuan_duong(os.path.relpath(p, S))
        ten = os.path.basename(p).lower()
        if rel in duoc_nap or ten in duoc_nap:
            continue
        chet.append(p)

    print("  1) ban sao luu cua cac dot va : %d tep" % len(sao_luu))
    print("  2) nhom tep TRUNG BYTE        : %d nhom (%d tep du)"
          % (len(trung), sum(len(v) - 1 for v in trung.values())))
    print("  3) .lua khong ai nap (GOI Y)  : %d tep" % len(chet))
    print("  4) tep rong / chi chu thich   : %d tep" % len(rong))

    with io.open(RA, "w", encoding="utf-8", newline="") as f:
        f.write("DANH SACH TEP THUA TRONG CAY SCRIPT\n")
        f.write("Sinh boi ReverseTools/cauhinh/t08_liet_ke_thua.py - CHI DOC\n")
        f.write("=" * 78 + "\n\n")

        f.write("## 1) BAN SAO LUU CUA CAC DOT VA (%d tep)\n" % len(sao_luu))
        f.write("Xoa duoc khi da nghiem thu xong ban va tuong ung.\n\n")
        for p in sorted(sao_luu):
            f.write("    %s\n" % os.path.relpath(p, SV))

        f.write("\n\n## 2) TEP TRUNG BYTE (%d nhom)\n" % len(trung))
        f.write("Noi dung y het nhau. Giu mot ban, cac ban con lai la du -\n")
        f.write("NHUNG phai kiem xem duong goi tro toi ban nao truoc khi don.\n\n")
        for _b, v in sorted(trung.items(), key=lambda x: -len(x[1])):
            f.write("    [%d ban giong nhau]\n" % len(v))
            for p in sorted(v):
                f.write("        %s\n" % os.path.relpath(p, SV))
            f.write("\n")

        f.write("\n## 3) .LUA KHONG THAY AI NAP - CHI LA GOI Y (%d tep)\n"
                % len(chet))
        f.write("Da doi chieu ca cay script LAN cay settings. Van co the sot\n")
        f.write("duong goi ghep chuoi luc chay => PHAI XEM TAY truoc khi don.\n\n")
        for p in sorted(chet):
            f.write("    %s\n" % os.path.relpath(p, SV))

        f.write("\n\n## 4) TEP RONG / CHI CO CHU THICH (%d tep)\n" % len(rong))
        for p in sorted(rong):
            f.write("    %s\n" % os.path.relpath(p, SV))

    print("  => da ghi %s" % RA)

    if not dodi:
        print("\nCHI LIET KE - khong dong toi tep nao.")
        print("Muon don ban sao luu (nhom 1) thi chay lai voi --dodi:")
        print("  chung se duoc DOI vao %s (van hoi lai duoc)." % DARA)
        return 0

    # chi doi nhom 1 - nhom an toan nhat
    n = 0
    for p in sao_luu:
        rel = os.path.relpath(p, SV)
        dich = os.path.join(DARA, rel)
        thu_muc = os.path.dirname(dich)
        if not os.path.isdir(thu_muc):
            os.makedirs(thu_muc)
        shutil.move(p, dich)
        n += 1
    print("  DA DOI %d ban sao luu vao %s" % (n, DARA))
    print("  (nhom 2/3/4 KHONG dong toi - phai chu duyet tay)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
