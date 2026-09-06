#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""so_sanh_perf.py - [LUA54] SO SANH HIEU NANG Lua 4 vs Lua 5.4 tu jx_perf_server.log (khong sua gi).

Log da co san: CoreServerShell ghi mot khoi [PERF] moi 60 s, trong do PERF_SCRIPT_TIME
(KPerfTick.h:34) do dung phan chay script moi tick. Cong cu nay:
  * tach log thanh cac khoi [PERF] (gio, tre, online) + tung giai doan (n, tb, max, p95, chiem%)
  * chia hai thoi ky theo MOC GIO chuyen sang 5.4 (--moc "05/09 12:39")
  * chi so sanh cac khoi CUNG MUC ONLINE (mac dinh >= 900) de cong bang
  * in bang truoc/sau cho SCRIPT_TIME va TICK: trung binh, p95, max, % tre

Chay:
  python so_sanh_perf.py <jx_perf_server.log> --moc "2026-09-05 12:39" [--online 900] [--giaidoan SCRIPT_TIME]
"""
import argparse, io, re, sys, statistics
from datetime import datetime

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

RE_HEAD = re.compile(r"\[PERF\] (\d\d):(\d\d):(\d\d) tick=(\d+) tre=(\d+) \(([\d.]+)%\) online=(\d+)")


def lay_giai_doan(khoi, ten):
    m = re.search(re.escape(ten) + r"\s+n=\s*(\d+)\s+tb=\s*([\d.]+)ms max=\s*([\d.]+)ms p95=\s*(\d+)ms chiem=\s*([\d.]+)%", khoi)
    if not m:
        return None
    return dict(n=int(m.group(1)), tb=float(m.group(2)), max=float(m.group(3)), p95=int(m.group(4)), chiem=float(m.group(5)))


def doc(p):
    t = io.open(p, "r", encoding="latin-1", newline="").read()
    ra = []
    phan = t.split("[PERF] ")
    for b in phan[1:]:
        m = RE_HEAD.match("[PERF] " + b.split("\n", 1)[0])
        if not m:
            continue
        ra.append(dict(gio=m.group(1) + ":" + m.group(2), h=int(m.group(1)), phut=int(m.group(2)),
                       tick=int(m.group(4)), tre=int(m.group(5)), tre_pc=float(m.group(6)),
                       online=int(m.group(7)), than=b))
    return ra


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("log")
    ap.add_argument("--moc", required=True, help='moc chuyen sang 5.4, dang "HH:MM" (theo gio trong log)')
    ap.add_argument("--online", type=int, default=900)
    ap.add_argument("--giaidoan", default="SCRIPT_TIME,TICK,SW_MAINLOOP,GS_MAINLOOP")
    ap.add_argument("--dau", type=int, default=0, help="chi lay N khoi DAU (online du) cua MOI lan chay - so cong bang boot vs boot")
    a = ap.parse_args()
    moc = a.moc.split()[-1][:5]
    mh, mm = int(moc[:2]), int(moc[3:5])
    rows = doc(a.log)
    # Log KHONG co ngay -> cat theo LAN CHAY: khoi cach nhau > 3 phut hoac dong ho lui = lan chay moi.
    lan = []
    for r in rows:
        p_ = r["h"] * 60 + r["phut"]
        if lan and (p_ - lan[-1][-1]["h"] * 60 - lan[-1][-1]["phut"]) in range(1, 4):
            lan[-1].append(r)
        elif lan and p_ == lan[-1][-1]["h"] * 60 + lan[-1][-1]["phut"]:
            lan[-1].append(r)
        else:
            lan.append([r])
    # lan chay cua ban 5.4 = cac lan co khoi DAU tu moc tro di, va nam o CUOI log
    i_moc = len(rows)
    dem = 0
    for L in lan:
        if (L[0]["h"], L[0]["phut"]) >= (mh, mm) and L is lan[-1]:
            i_moc = dem
            break
        dem += len(L)
    print("Log co %d lan chay may chu (cat theo khoang trong > 3 phut)." % len(lan))
    print("Lan cuoi: %s -> %s, %d khoi, online cao nhat = %d" % (
        lan[-1][0]["gio"], lan[-1][-1]["gio"], len(lan[-1]), max(x["online"] for x in lan[-1])))
    truoc = [r for r in rows[:i_moc] if r["online"] >= a.online]
    sau = [r for r in rows[i_moc:] if r["online"] >= a.online]
    if a.dau > 0:
        # cong bang: N khoi dau tien (du online) cua TUNG lan chay Lua 4, so voi N khoi dau cua lan 5.4
        truoc = []
        dem2 = 0
        for L in lan:
            if dem2 >= i_moc: break
            du = [r for r in L if r["online"] >= a.online][:a.dau]
            truoc.extend(du); dem2 += len(L)
        sau = sau[:a.dau]
        print("Che do --dau %d: Lua 4 = %d khoi dau cua %d lan chay; Lua 5.4 = %d khoi dau" % (a.dau, len(truoc), sum(1 for L in lan[:-1] if any(r["online"] >= a.online for r in L)), len(sau)))
    print("jx_perf_server.log: %d khoi; moc 5.4 = %s" % (len(rows), moc))
    print("Loc online >= %d:  Lua 4 = %d khoi, Lua 5.4 = %d khoi" % (a.online, len(truoc), len(sau)))
    if not sau:
        print("\nCHUA DU DU LIEU BEN 5.4. Can may chu chay lai voi ~%d nguoi va doi vai phut," % a.online)
        print("roi chay lai lenh nay - bang so sanh se hien ra.")
    print()
    hang = "%-14s | %-28s | %-28s | %s" % ("giai doan", "Lua 4 (tb/p95/max ms)", "Lua 5.4 (tb/p95/max ms)", "doi")
    print(hang); print("-" * len(hang))
    for ten in a.giaidoan.split(","):
        ten = ten.strip()
        A = [lay_giai_doan(r["than"], ten) for r in truoc]
        B = [lay_giai_doan(r["than"], ten) for r in sau]
        A = [x for x in A if x]; B = [x for x in B if x]
        if not A and not B:
            continue
        def gom(X):
            if not X: return None
            return (statistics.mean(x["tb"] for x in X), statistics.mean(x["p95"] for x in X), max(x["max"] for x in X))
        ga, gb = gom(A), gom(B)
        sa = "%.3f / %.1f / %.1f" % ga if ga else "(khong co)"
        sb = "%.3f / %.1f / %.1f" % gb if gb else "(chua co)"
        doi = ""
        if ga and gb and ga[0] > 0:
            doi = "tb %+.0f%%" % ((gb[0] - ga[0]) / ga[0] * 100)
            if ga[2] > 0:
                doi += ", max %+.0f%%" % ((gb[2] - ga[2]) / ga[2] * 100)
        print("%-14s | %-28s | %-28s | %s" % (ten, sa, sb, doi))
    # do tre tick
    def tre(X):
        return (statistics.mean(x["tre"] for x in X), statistics.mean(x["tre_pc"] for x in X)) if X else None
    ta, tb2 = tre(truoc), tre(sau)
    print()
    if ta: print("Tick tre  Lua 4  : tb %.0f lan/phut (%.2f%%)" % ta)
    if tb2: print("Tick tre  Lua 5.4: tb %.0f lan/phut (%.2f%%)" % tb2)
    # dinh nhon script
    if truoc:
        dn = sorted(((lay_giai_doan(r["than"], "SCRIPT_TIME") or {}).get("max", 0), r["gio"], r["online"]) for r in truoc)[-5:]
        print("\n5 dinh nhon SCRIPT_TIME cao nhat ben Lua 4 (giat khung hinh):")
        for mx, g, on in reversed(dn): print("   %s  online=%d  max=%.1f ms" % (g, on, mx))
    if sau:
        dn = sorted(((lay_giai_doan(r["than"], "SCRIPT_TIME") or {}).get("max", 0), r["gio"], r["online"]) for r in sau)[-5:]
        print("\n5 dinh nhon SCRIPT_TIME cao nhat ben Lua 5.4:")
        for mx, g, on in reversed(dn): print("   %s  online=%d  max=%.1f ms" % (g, on, mx))
    return 0


if __name__ == "__main__":
    sys.exit(main())
