# -*- coding: utf-8 -*-
"""t31_hoantac_va_sai.py - HOAN TAC mot cho toi da va SAI trong t28.

LOI CUA TOI:
  t28 doi `count` thanh `1` o citywar_function.lua (trong ham take_tong_award).
  Toi ket luan "bien count khong duoc khai o dau" - SAI. Ham do khai la
      function take_tong_award(count)      <- dong 134
  tuc `count` la THAM SO cua ham, hoan toan hop le. Doi thanh 1 la lam cau
  thong bao luon ghi "1 vien" ke ca khi ham duoc goi voi so khac.

  Toi da khong doc dong khai ham truoc khi ket luan - dung cai loi ma so tay
  du an canh bao: "moi khang dinh phai truy den dong da doc".

  Cho THU HAI (trong ham take_tong_resaward, dong 168) thi ket luan DUNG: ham
  do KHONG co tham so nao, nen `count` o do that su chua khai. Giu nguyen ban
  va do (count -> nCount).

BAN VA NAY: tra `1` ve `count` va bo ba dong chu thich sai kem theo.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

P = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
     r"\script\missions\citywar_global\citywar_function.lua")
NHAN = "[HOANTAC 30/08]"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t31_hoantac_va_sai - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    raw = doc(P)
    if NHAN in raw:
        print("  DA HOAN TAC - bo qua")
        return 0
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    d = raw.split(eol)

    # chot: ham take_tong_award PHAI nhan tham so `count`
    if not any(l.startswith("function take_tong_award(count)") for l in d):
        print("!!! LOI TO: khong thay `function take_tong_award(count)`"
              " - dung lai")
        return 1
    if not any(l.startswith("function take_tong_resaward()") for l in d):
        print("!!! LOI TO: khong thay `function take_tong_resaward()`"
              " - dung lai")
        return 1
    print("  chot: take_tong_award CO tham so `count`;"
          " take_tong_resaward KHONG co")

    # tim dong da bi sua nham: co GetName(),1) VA nam TRONG take_tong_award
    i_award = next(i for i, l in enumerate(d)
                   if l.startswith("function take_tong_award(count)"))
    i_res = next(i for i, l in enumerate(d)
                 if l.startswith("function take_tong_resaward()"))
    vt = [i for i, l in enumerate(d)
          if re.search(r"GetName\(\)\s*,\s*1\s*\)", l) and i_award < i < i_res]
    if len(vt) != 1:
        print("!!! LOI TO: tim thay %d dong bi sua nham (can 1)" % len(vt))
        return 1
    i = vt[0]

    # bo ba dong chu thich sai ngay tren no
    a = i
    while a > 0 and d[a - 1].strip().startswith("-- [VACTC 30/08]") is False \
            and d[a - 1].strip().startswith("--") and a > i - 4:
        a -= 1
    if a > 0 and d[a - 1].strip().startswith("-- [VACTC 30/08]"):
        a -= 1
    so_bo = i - a
    if so_bo != 3:
        print("!!! LOI TO: mong bo 3 dong chu thich, tinh ra %d" % so_bo)
        return 1

    d[i] = re.sub(r"(GetName\(\)\s*,\s*)1(\s*\))", r"\g<1>count\g<2>", d[i])
    con = d[:a] + [
        "\t-- " + NHAN + " HOAN TAC: ban va truoc doi `count` thanh 1 o day la",
        "\t-- SAI - `count` la THAM SO cua ham take_tong_award(count) o tren,",
        "\t-- hoan toan hop le. (Cho thu hai trong take_tong_resaward moi that",
        "\t-- su thieu khai, va van giu ban va do.)",
    ] + d[i:]
    nd = eol.join(con)

    if sum(1 for c in nd if ord(c) > 127) != sum(1 for c in raw if ord(c) > 127):
        print("!!! LOI TO: byte tieng Viet doi")
        return 1
    if "GetName(),count)" not in nd.replace(" ", ""):
        print("!!! LOI TO: chua tra duoc `count` ve cho cu")
        return 1
    print("  tra `1` ve `count` o dong %d, bo 3 dong chu thich sai" % (i + 1))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + ".truoc_hoantac"
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI citywar_function.lua")
    return 0


if __name__ == "__main__":
    sys.exit(main())
