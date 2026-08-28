# -*- coding: ascii -*-
"""vE_reload_lenhbai.py - them 2 dong ReLoadScript cho bo test lo ren vao
`reloadfileEvent()` cua lenhbaiadmin.lua, de sua kich ban xong bam "Nap lai file
script (reload)" tren Lenh bai Admin la an ngay, khong phai khoi dong lai may chu.

Vi sao phai la patcher chu khong dung safe_edit qua dong lenh: duong dan Lua o day
la `\\\\script\\\\...` (backslash doi). Truyen qua dong lenh bash thi backslash bi
RUT MOT NUA (bay da ghi trong bo nho `bash-inline-backslash-halved`) nen moc neo
khong bao gio khop. Doc/ghi thang bang latin-1 la an toan.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_uiloren2 lan dau).
"""
import io
import os
import shutil
import sys

DICH = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\lenhbaiadmin.lua"
HAU_TO = ".truoc_uiloren2"

BS = chr(92)          # mot dau gach nguoc
D2 = BS + BS          # hai dau gach nguoc (dung Lua viet trong nguon)

NEO = 'ReLoadScript("%sscript%smissions%sfengling_ferry%sshuizeideath.lua")' % (D2, D2, D2, D2)
THEM = [
    'ReLoadScript("%sscript%sitem%stest_loren_admin.lua")' % (D2, D2, D2),
    'ReLoadScript("%sscript%sitem%scompound%scompound_ui.lua")' % (D2, D2, D2, D2),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vE_reload_lenhbai - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1

    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi_truoc = sum(1 for c in raw if ord(c) > 127)
    crlf_truoc = raw.count("\r\n")

    if THEM[0] in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    dem = raw.count(NEO)
    if dem != 1:
        print("!!! LOI TO: moc neo xuat hien %d lan (can dung 1) - KHONG ghi gi" % dem)
        return 1
    print("  moc neo trung 1 lan")

    moi = raw.replace(NEO, NEO + eol + eol.join(THEM), 1)

    hi_sau = sum(1 for c in moi if ord(c) > 127)
    if hi_sau != hi_truoc:
        print("!!! LOI TO: byte cao %d -> %d (le ra khong doi) - KHONG ghi" % (hi_truoc, hi_sau))
        return 1
    print("  byte cao %d -> %d (khong doi)   CRLF %d -> %d   EOL=%s"
          % (hi_truoc, hi_sau, crlf_truoc, moi.count("\r\n"),
             "CRLF" if eol == "\r\n" else "LF"))
    for d in THEM:
        print("  SE THEM: %s" % d)

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % sao)
    with io.open(DICH, "wb") as f:
        f.write(moi.encode("latin-1"))

    lai = io.open(DICH, "rb").read().decode("latin-1")
    if lai != moi:
        print("!!! LOI TO: doc lai tu dia KHONG khop ban vua ghi")
        return 1
    print("  DA GHI + doc lai tu dia: khop.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
