# -*- coding: utf-8 -*-
"""v22_chep_bang_toado.py - VA "vao tran co thong bao nhung KHONG co quai".

GOC (bat tan tay trong ScriptError.log 11:46:24):
    error: bad argument #2 to `random' (interval is empty)
       2: YDBZ_add_npc      npc.lua:285   -> random(1, npc_create_point)
       3: YDBZ_create_all_npc npc.lua:453
       4: YDBZ_start_match  timer_match.lua:48
       5: OnTimer           timer_match.lua:86
  npc_create_point = YDBZ_GetTabFileHeight(file_name) (include.lua:167) -
  TabFile_Load THAT BAI thi tra 0 => random(1,0) = khoang rong => sap NGAY
  SAU cau thong bao "Tranh doat bao tang Viem De bat dau..." (giai thich dung
  anh chup cua chu game: co dong do roi im).

  Ly do TabFile_Load that bai: **thu muc settings\\maps\\yandibaozang CHUA
  DUOC CHEP sang JX1** (ban Linux co 31 tep toa do + trap\\ 34 tep + trap\\clear).
  Script trap phia JX1 da co san (script\\missions\\yandibaozang\\trap\\
  a/b/ctrapenter.lua) - chi thieu BANG DU LIEU.

MIENG VA: chep NGUYEN VAN (nhi phan, khong dich ky tu) ca cay
    D:\\ServerLinux\\server1\\settings\\maps\\yandibaozang\\**
 -> E:\\...\\bin\\server\\settings\\maps\\yandibaozang\\**
Chi chep tep CHUA CO (khong de len ban JX1 neu ai da chep truoc), va nghiem
thu tung tep: dinh dang KTabFile (dong dau la header co TAB), doc lai byte
khop 100%.

KHONG build. TabFile_Load nap theo yeu cau (khong cache boot) nen thuong an
NGAY, khong can restart - nhung neu da tung Load HONG mot lan trong phien thi
KTabFile giu ban rong => an toan nhat la khoi dong lai GameServer (bao chu).
Mac dinh DIEN TAP; --ghi moi chep that.
"""
import io
import os
import shutil
import sys

NGUON = r"D:\ServerLinux\server1\settings\maps\yandibaozang"
DICH = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\maps\yandibaozang"


def kiem_tab(duong):
    """Tep toa do phai la KTabFile: dong dau co TAB, >= 2 dong, moi dong du 2 cot."""
    try:
        raw = io.open(duong, "rb").read().decode("latin-1")
    except IOError as e:
        return "khong doc duoc: %s" % e
    dong = [l for l in raw.replace("\r\n", "\n").split("\n") if l.strip()]
    if len(dong) < 2:
        return "chi co %d dong" % len(dong)
    if "\t" not in dong[0]:
        return "dong dau khong co TAB (khong phai KTabFile)"
    for i, l in enumerate(dong[1:], 2):
        if len(l.split("\t")) < 2:
            return "dong %d thieu cot" % i
    return None


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== v22_chep_bang_toado - %s ===" % ("CHEP THAT" if ghi else "DIEN TAP"))

    if not os.path.isdir(NGUON):
        print("!!! LOI TO: khong thay nguon %s" % NGUON)
        return 1

    viec = []      # (nguon, dich)
    daco = 0
    for thumuc, _, teps in os.walk(NGUON):
        for ten in teps:
            n = os.path.join(thumuc, ten)
            d = os.path.join(DICH, os.path.relpath(n, NGUON))
            if os.path.isfile(d):
                daco += 1
                continue
            viec.append((n, d))

    print("  nguon co %d tep | ben JX1 da co %d | can chep %d"
          % (len(viec) + daco, daco, len(viec)))
    if not viec:
        print("  KHONG co gi de chep (idempotent)")
        return 0

    # nghiem thu dinh dang TRUOC khi chep
    loi = []
    for n, _ in viec:
        if n.lower().endswith(".txt"):
            r = kiem_tab(n)
            if r:
                loi.append((os.path.basename(n), r))
    if loi:
        print("!!! LOI TO: %d tep khong dung dinh dang KTabFile - DUNG LAI:" % len(loi))
        for ten, ly in loi[:10]:
            print("    %-20s %s" % (ten, ly))
        return 1
    print("  chot dinh dang: %d tep .txt deu dung KTabFile (header TAB, >=2 cot)"
          % sum(1 for n, _ in viec if n.lower().endswith(".txt")))

    if not ghi:
        for n, d in viec[:6]:
            print("    se chep: %s" % os.path.relpath(n, NGUON))
        if len(viec) > 6:
            print("    ... con %d tep nua" % (len(viec) - 6))
        print("\nDIEN TAP - chua chep. Chay lai voi --ghi de chep that.")
        return 0

    for n, d in viec:
        thu = os.path.dirname(d)
        if not os.path.isdir(thu):
            os.makedirs(thu)
        shutil.copy2(n, d)
        if io.open(n, "rb").read() != io.open(d, "rb").read():
            print("!!! LOI TO: doc lai KHONG khop: %s" % d)
            return 1
    print("  DA CHEP %d tep, doc lai khop 100%%" % len(viec))
    print("  => Khoi dong lai GameServer cho chac (bao chu), roi test lai mot minh.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
