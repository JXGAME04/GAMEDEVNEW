# -*- coding: utf-8 -*-
r"""perf_read.py - doc jx_perf_server.log va tra loi thang: may chu co tut nhip khong,
va thoi gian di dau.

Dung sau buoi test Tong Kim:
    python ReverseTools\perf_read.py "E:\...\bin\server\jx_perf_server.log"
    python ReverseTools\perf_read.py <duong dan> --tu 19:30 --den 20:30

In ra:
  1. Bang theo tung khoang bao cao: gio, so tick, % tick tre, online, khoi luong.
  2. Xep hang giai doan theo % thoi gian chiem (trung binh ca phien).
  3. Tuong quan: khi so nguoi tang thi thoi gian tick tang theo kieu nao - de phan biet
     "cham vi dong" (tuyen tinh theo khoi luong) voi "cham vi loi thuat toan"
     (thoi gian tang ma khoi luong khong tang).
"""
import argparse
import io
import re
import sys

RE_HEAD = re.compile(
    r"\[PERF\]\s+(\d\d:\d\d:\d\d)\s+tick=(\d+)\s+tre=(\d+)\s+\(([\d.]+)%\)\s+online=(\d+)\s+khoang=([\d.]+)s")
RE_STAGE = re.compile(
    r"^\s+(\S+)\s+n=(\d+)\s+tb=\s*([\d.]+)ms\s+max=\s*([\d.]+)ms\s+p95=\s*(\d+)ms\s+chiem=\s*([\d.]+)%")
RE_LOAD = re.compile(
    r"^\s+KHOILUONG\s+region tb=(\d+) max=(\d+) \| NPC tb=(\d+) max=(\d+)")


def doc(path):
    khoang = []
    cur = None
    for line in io.open(path, encoding="latin-1", errors="replace"):
        m = RE_HEAD.search(line)
        if m:
            cur = {"gio": m.group(1), "tick": int(m.group(2)), "tre": int(m.group(3)),
                   "tre_pc": float(m.group(4)), "online": int(m.group(5)),
                   "khoang": float(m.group(6)), "giaidoan": {}, "region": 0, "npc": 0}
            khoang.append(cur)
            continue
        if cur is None:
            continue
        m = RE_LOAD.match(line)
        if m:
            cur["region"] = int(m.group(1))
            cur["npc"] = int(m.group(3))
            continue
        m = RE_STAGE.match(line)
        if m:
            cur["giaidoan"][m.group(1)] = {
                "n": int(m.group(2)), "tb": float(m.group(3)), "max": float(m.group(4)),
                "p95": int(m.group(5)), "chiem": float(m.group(6))}
    return khoang


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("file")
    ap.add_argument("--tu", default=None, help="chi lay tu gio nay (HH:MM)")
    ap.add_argument("--den", default=None, help="chi lay den gio nay (HH:MM)")
    a = ap.parse_args()

    ks = doc(a.file)
    if a.tu:
        ks = [k for k in ks if k["gio"][:5] >= a.tu]
    if a.den:
        ks = [k for k in ks if k["gio"][:5] <= a.den]
    if not ks:
        sys.exit("khong doc duoc khoang bao cao nao (kiem tra [PerfLog] On=1 va duong dan)")

    print("=" * 104)
    print("%-9s %6s %6s %7s %8s %8s %9s %9s %9s" %
          ("gio", "tick", "tre", "tre%", "online", "region", "NPC", "tick_tb", "tick_max"))
    print("-" * 104)
    for k in ks:
        t = k["giaidoan"].get("TICK", {})
        print("%-9s %6d %6d %6.1f%% %8d %8d %9d %8.1fms %8.1fms" %
              (k["gio"], k["tick"], k["tre"], k["tre_pc"], k["online"],
               k["region"], k["npc"], t.get("tb", 0), t.get("max", 0)))

    # --- xep hang giai doan ---
    tong = {}
    for k in ks:
        for ten, g in k["giaidoan"].items():
            d = tong.setdefault(ten, {"chiem": 0.0, "max": 0.0, "n": 0})
            d["chiem"] += g["chiem"]
            d["max"] = max(d["max"], g["max"])
            d["n"] += 1
    print()
    print("=== Giai doan nao an thoi gian (trung binh ca phien) ===")
    for ten, d in sorted(tong.items(), key=lambda x: -x[1]["chiem"] / max(1, x[1]["n"])):
        print("   %-12s chiem tb %5.1f%%   max tung thay %8.2fms" %
              (ten, d["chiem"] / max(1, d["n"]), d["max"]))

    # --- tuong quan dong <-> cham ---
    co_npc = [k for k in ks if k["npc"] > 0 and "TICK" in k["giaidoan"]]
    if len(co_npc) >= 2:
        lo = min(co_npc, key=lambda k: k["npc"])
        hi = max(co_npc, key=lambda k: k["npc"])
        tb_lo = lo["giaidoan"]["TICK"]["tb"]
        tb_hi = hi["giaidoan"]["TICK"]["tb"]
        print()
        print("=== Cham vi DONG hay vi LOI THUAT TOAN? ===")
        print("   luc vang nhat : NPC=%-7d tick tb=%6.2fms  (online=%d, %s)" %
              (lo["npc"], tb_lo, lo["online"], lo["gio"]))
        print("   luc dong nhat : NPC=%-7d tick tb=%6.2fms  (online=%d, %s)" %
              (hi["npc"], tb_hi, hi["online"], hi["gio"]))
        if lo["npc"] > 0 and tb_lo > 0:
            r_npc = hi["npc"] / float(lo["npc"])
            r_tb = tb_hi / tb_lo
            print("   NPC gap %.2f lan  ->  thoi gian gap %.2f lan" % (r_npc, r_tb))
            if r_tb > r_npc * 1.5:
                print("   => Thoi gian tang NHANH HON khoi luong: co dau hieu thuat toan xau")
                print("      (chi phi tang phi tuyen theo so doi tuong). Dang dao sau.")
            elif r_tb < r_npc * 0.8:
                print("   => Thoi gian tang CHAM HON khoi luong: duong tick chiu tai tot.")
            else:
                print("   => Thoi gian tang gan TUYEN TINH voi khoi luong: 'cham vi dong',")
                print("      muon nhanh hon thi phai giam viec moi doi tuong, khong phai doi kien truc.")

    tre_tong = sum(k["tre"] for k in ks)
    tick_tong = sum(k["tick"] for k in ks)
    print()
    print("=== Ket luan nhanh ===")
    if tick_tong:
        pc = tre_tong * 100.0 / tick_tong
        print("   Tick tre: %d/%d = %.2f%%" % (tre_tong, tick_tong, pc))
        if pc < 1:
            print("   => May chu KHONG tut nhip. Neu nguoi choi van thay giat thi nguyen nhan")
            print("      nam o CLIENT (vong ve) hoac duong truyen, khong phai CPU may chu.")
        elif pc < 10:
            print("   => Co tut nhip nhung it. Xem giai doan dung dau bang tren.")
        else:
            print("   => TUT NHIP NANG. Giai doan dung dau bang tren la noi phai chua truoc.")


if __name__ == "__main__":
    main()
