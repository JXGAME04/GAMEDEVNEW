# -*- coding: utf-8 -*-
"""t28_va_2loi_congthanh.py - va 2 loi DANG XAY RA trong Cong Thanh Chien.

Cong Thanh dang chay that (BAT_CTC_JX2 = 1).

=== LOI 1: dat ngoac sai lam mat muc bao danh nua tieng moi gio ===
  tinhnang\\congthanhchien\\congthanhquan.lua:24
      if (wday == 4 and hour >= 18 and (hour <= 20 and nminus < 30)) then
  Ngoac lam dieu kien thanh: gio thuoc {18,19,20} VA phut < 30.
  Tuc muc "Bao danh Loi Dai" BIEN MAT trong cac phut 30-59 cua ca ba gio.
  Y dinh ro rang la mot khoang lien tuc 18:00 -> 20:30.
  VA: (hour < 20) or (hour == 20 and nminus < 30)

  Dong 27 cung dang ngoac tuong tu:
      if (wday == 4 and (hour >= 20 and nminus >= 30) and hour <= 21) then
  => muc "Tham gia Loi Dai" chi hien khi phut >= 30, nen 21:00-21:29 bi mat.
  VA: (hour == 20 and nminus >= 30) or (hour == 21)

  ⚠️ Day la MO RONG thoi gian hien muc so voi hom nay (them nua tieng moi gio).
     Do la khoi phuc y dinh cua ma, nhung van la doi hanh vi - bao chu.

=== LOI 2: dung bien chua khai, no khi bang chu lay thuong con du ===
  missions\\citywar_global\\citywar_function.lua:194
      local msg = format("... nhan %s huyen tinh ...", ..., count)
  Bien ten `count` KHONG duoc khai o dau trong ham; ba dong lan can (190, 193,
  196) deu dung dung `nCount`. format voi nil se nem loi Lua, cat dut ham ngay
  TRUOC dong Say(...) - nen bang chu khong nhan duoc cau thong bao, va dong
  nhat ky khong duoc ghi.
  VA: count -> nCount. Khong doi hanh vi nao khac.

KHONG VA (dang ngu, bao chu de quyet khi bat):
  tinhnang\\loidai\\vebinhdautruong.lua:63-65
      function GetJoinTongTime() return 7201 end
  De cung ten voi ham cua may chu, luon tra 7201 nen phep kiem
  "phai o trong bang 2 tieng" (7200 giay) o dong 75 va 81 LUON DUNG - tuc luat
  do bi vo hieu hoan toan. Hien Loi Dai Bang Hoi dang tat (BAT_LOIDAI_BANGHOI
  = 0) nen chua gay hai.

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

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
NHAN = "[VACTC 30/08]"

VA = [
    dict(
        tep=os.path.join(S, "tinhnang", "congthanhchien", "congthanhquan.lua"),
        muc=[
            ("\tif (wday == 4 and hour >= 18 and (hour <= 20 and nminus < 30)) then",
             "\t-- " + NHAN + " dat ngoac sai: dieu kien cu la 'gio thuoc"
             " {18,19,20}\n"
             "\t-- VA phut < 30', nen muc nay BIEN MAT trong cac phut 30-59 cua"
             " ca ba\n"
             "\t-- gio. Y dinh la mot khoang lien tuc 18:00 -> 20:30.\n"
             "\tif (wday == 4 and hour >= 18 and"
             " (hour < 20 or (hour == 20 and nminus < 30))) then"),
            ("\tif (wday == 4 and (hour >= 20 and nminus >= 30) and hour <= 21) then",
             "\t-- " + NHAN + " cung loi dat ngoac: muc nay chi hien khi phut"
             " >= 30,\n"
             "\t-- nen khoang 21:00-21:29 bi mat. Y dinh la 20:30 -> het gio 21.\n"
             "\tif (wday == 4 and"
             " ((hour == 20 and nminus >= 30) or hour == 21)) then"),
        ],
    ),
    dict(
        tep=os.path.join(S, "missions", "citywar_global", "citywar_function.lua"),
        muc=[
            ('nhan %s huyen tinh cap 5 tu Kim son chuong mon nhan",'
             'GetLocalDate("[%y-%m-%d %H:%M] "),GetAccount(),GetName(),count)',
             None),   # xu ly rieng ben duoi vi dong nay co tieng Viet
        ],
    ),
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def can_bang(s):
    t = re.sub(r"--[^\n]*", "", s)
    t = re.sub(r'"[^"]*"', '""', t)
    t = re.sub(r"'[^']*'", "''", t)
    d = lambda w: len(re.findall(r"\b%s\b" % w, t))
    return (d("function") + d("then") + d("do") - d("elseif")) - d("end")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t28_va_2loi_congthanh - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    viec = []

    # ---------- LOI 1 ----------
    p1 = VA[0]["tep"]
    raw1 = doc(p1)
    if NHAN in raw1:
        print("  congthanhquan.lua DA VA - bo qua")
    else:
        eol = "\r\n" if raw1.count("\r\n") >= (raw1.count("\n")
                                              - raw1.count("\r\n")) else "\n"
        nd1 = raw1
        for cu, moi in VA[0]["muc"]:
            n = nd1.count(cu)
            if n != 1:
                print("!!! LOI TO: moc khop %d lan (can 1): %r" % (n, cu[:60]))
                return 1
            nd1 = nd1.replace(cu, moi.replace("\n", eol))
            print("  congthanhquan.lua  sua dat ngoac: %s" % cu.strip()[:52])
        if sum(1 for c in nd1 if ord(c) > 127) != sum(1 for c in raw1
                                                     if ord(c) > 127):
            print("!!! LOI TO: byte tieng Viet doi (congthanhquan)")
            return 1
        if can_bang(nd1) != can_bang(raw1):
            print("!!! LOI TO: can bang tu khoa Lua doi (congthanhquan)")
            return 1
        viec.append((p1, nd1, raw1))

    # ---------- LOI 2: doi bien `count` -> `nCount` tren DUNG mot dong ----------
    p2 = VA[1]["tep"]
    raw2 = doc(p2)
    if NHAN in raw2:
        print("  citywar_function.lua DA VA - bo qua")
    else:
        eol = "\r\n" if raw2.count("\r\n") >= (raw2.count("\n")
                                              - raw2.count("\r\n")) else "\n"
        d = raw2.split(eol)
        # CO HAI cho dung bien `count` chua khai, va moi cho can mot gia tri
        # KHAC NHAU - da doc ngu canh tung cho:
        #   dong ~157: nhanh phat DUNG MOT vien moi ngay (AddItem(...) mot lan,
        #              add_citybonus_task(..., 1)) => thay bang so 1
        #   dong ~194: co bien nCount khai ngay tren (dong 188) => thay bang nCount
        vt = [i for i, l in enumerate(d)
              if re.search(r"GetName\(\)\s*,\s*count\s*\)", l)]
        if len(vt) != 2:
            print("!!! LOI TO: tim thay %d dong dung bien `count` (mong 2)"
                  % len(vt))
            return 1
        them = 0
        for i0 in vt:
            i = i0 + them
            quanh = eol.join(d[max(0, i - 10):i])
            if "local nCount" in quanh:
                thay, mo = "nCount", "co bien nCount khai ngay tren"
            elif "add_citybonus_task" in quanh and ", 1)" in quanh:
                thay, mo = "1", "nhanh nay phat dung MOT vien moi ngay"
            else:
                print("!!! LOI TO: khong xac dinh duoc gia tri dung cho dong %d"
                      % (i + 1))
                return 1
            d[i] = re.sub(r"(GetName\(\)\s*,\s*)count(\s*\))",
                          r"\g<1>%s\g<2>" % thay, d[i])
            d.insert(i, "\t-- " + NHAN + " bien `count` khong duoc khai o dau"
                     " trong ham; cac dong")
            d.insert(i + 1, "\t-- lan can deu dung ten khac. format voi nil nem"
                     " loi Lua, cat dut ham")
            d.insert(i + 2, "\t-- ngay tai day nen nguoi choi khong nhan duoc"
                     " cau thong bao. (%s)" % mo)
            them += 3
            print("  citywar_function.lua  dong %d: count -> %s  (%s)"
                  % (i0 + 1, thay, mo))
        nd2 = eol.join(d)
        if sum(1 for c in nd2 if ord(c) > 127) != sum(1 for c in raw2
                                                     if ord(c) > 127):
            print("!!! LOI TO: byte tieng Viet doi (citywar_function)")
            return 1
        if can_bang(nd2) != can_bang(raw2):
            print("!!! LOI TO: can bang tu khoa Lua doi (citywar_function)")
            return 1


        viec.append((p2, nd2, raw2))

    if not viec:
        print("Khong co gi de lam.")
        return 0
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    for p, nd, raw in viec:
        sao = p + ".truoc_vactc"
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if doc(p) != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %s" % os.path.basename(p))
    return 0


if __name__ == "__main__":
    sys.exit(main())
