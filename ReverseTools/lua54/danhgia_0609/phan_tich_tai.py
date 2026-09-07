r"""phan_tich_tai.py - phan tich TAI may chu tu jx_perf_server.log: hoi quy thoi gian tick theo so nguoi truc tuyen
va theo so NPC/region, de suy ra chi phi MOI DAU NGUOI va tran cua MOT nhan.
  python phan_tich_tai.py [duong_dan_log ...]
"""
import re, sys, os, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
LOGS = sys.argv[1:] or [r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\jx_perf_server.log"]

RX_HEAD = re.compile(r"^\[PERF\] (\d\d:\d\d:\d\d) tick=(\d+) tre=(\d+) \(([\d.]+)%\) online=(\d+) khoang=([\d.]+)s")
RX_KL = re.compile(r"KHOILUONG\s+region tb=(\d+) max=(\d+) \| NPC tb=(\d+) max=(\d+)")
RX_ST = re.compile(r"^\s+([A-Z_]+)\s+n=(\d+)\s+tb=\s*([\d.]+)ms\s+max=\s*([\d.]+)ms\s+p95=\s*([\d.]+)ms\s+chiem=\s*([\d.]+)%")

ky = []          # moi ky: dict
cur = None
for p in LOGS:
    if not os.path.exists(p): continue
    for ln in open(p, encoding="latin-1", errors="replace"):
        m = RX_HEAD.match(ln)
        if m:
            if cur: ky.append(cur)
            cur = {"gio": m.group(1), "tick": int(m.group(2)), "tre": int(m.group(3)),
                   "online": int(m.group(5)), "khoang": float(m.group(6)), "st": {}}
            continue
        if cur is None: continue
        m = RX_KL.search(ln)
        if m:
            cur["region"] = int(m.group(1)); cur["npc"] = int(m.group(3)); continue
        m = RX_ST.match(ln)
        if m:
            cur["st"][m.group(1)] = (int(m.group(2)), float(m.group(3)), float(m.group(4)), float(m.group(6)))
if cur: ky.append(cur)
ky = [k for k in ky if "TICK" in k["st"]]
print("Doc %d ky perf (moi ky ~60 s)" % len(ky))

def hoi_quy(xs, ys):
    """binh phuong toi thieu y = a + b*x -> (a, b, r2)"""
    n = len(xs)
    if n < 3: return (0.0, 0.0, 0.0)
    mx = sum(xs) / n; my = sum(ys) / n
    sxx = sum((x - mx) ** 2 for x in xs); sxy = sum((x - mx) * (y - my) for x, y in zip(xs, ys))
    if sxx == 0: return (my, 0.0, 0.0)
    b = sxy / sxx; a = my - b * mx
    ss_tot = sum((y - my) ** 2 for y in ys)
    ss_res = sum((y - (a + b * x)) ** 2 for x, y in zip(xs, ys))
    r2 = 1 - ss_res / ss_tot if ss_tot > 0 else 0.0
    return (a, b, r2)

# --- bang theo bac online (bo ky co tick tre bat thuong de lay nen on dinh)
print("\n== TICK trung binh theo so nguoi truc tuyen (median cua tung bac) ==")
bac = collections.defaultdict(list)
for k in ky:
    b = (k["online"] // 100) * 100
    bac[b].append(k)
print("%8s %6s %9s %9s %9s %9s %9s" % ("online", "so ky", "TICK tb", "SW_ACT", "LUA", "NPC", "region"))
for b in sorted(bac):
    ks = bac[b]
    def med(f):
        v = sorted(f(k) for k in ks if f(k) is not None)
        return v[len(v) // 2] if v else 0.0
    print("%8d %6d %8.2fms %8.2fms %8.2fms %9.0f %9.0f" % (
        b, len(ks), med(lambda k: k["st"]["TICK"][1]),
        med(lambda k: k["st"].get("SW_ACTIVATE", (0, 0, 0, 0))[1]),
        med(lambda k: k["st"].get("LUA_CALL", (0, 0, 0, 0))[1]),
        med(lambda k: k.get("npc", 0)), med(lambda k: k.get("region", 0))))

# --- hoi quy: tick theo online (chi lay ky "sach": tre it, tick tb < 30 ms)
sach = [k for k in ky if k["tre"] <= 2 and k["st"]["TICK"][1] < 30 and k["online"] > 0]
xs = [k["online"] for k in sach]; ys = [k["st"]["TICK"][1] for k in sach]
a, b, r2 = hoi_quy(xs, ys)
print("\n== HOI QUY tren %d ky sach ==" % len(sach))
print("TICK(ms) = %.3f + %.5f * online     (r2 = %.3f)" % (a, b, r2))
print("  -> chi phi co dinh %.2f ms/tick; moi 100 nguoi them %.2f ms/tick" % (a, b * 100))
for nm in ("SW_ACTIVATE", "LUA_CALL", "SW_MSGLOOP"):
    ys2 = [k["st"].get(nm, (0, 0, 0, 0))[1] for k in sach]
    a2, b2, r22 = hoi_quy(xs, ys2)
    print("  %-12s = %.3f + %.5f*online (r2=%.2f) -> +%.2f ms moi 100 nguoi" % (nm, a2, b2, r22, b2 * 100))

# --- hoi quy theo so NPC (bot + quai + npc chuc nang)
xs3 = [k.get("npc", 0) for k in sach if k.get("npc")]
ys3 = [k["st"]["TICK"][1] for k in sach if k.get("npc")]
a3, b3, r23 = hoi_quy(xs3, ys3)
print("TICK(ms) = %.3f + %.5f * soNPC        (r2 = %.3f) -> +%.2f ms moi 100 NPC" % (a3, b3, r23, b3 * 100))

# --- tran mot nhan
NGAN_SACH = 1000.0 / 18.0
print("\n== TRAN CUA MOT NHAN (ngan sach %.1f ms/tick, 18 tick/giay) ==" % NGAN_SACH)
if b > 0:
    for he_so, ten in ((1.0, "may nay (i7-13700K)"), (0.75, "cham hon 25%"), (0.55, "cham hon 45%"), (0.40, "cham hon 60%")):
        # tick tren may cham = tick_may_nay / he_so
        tran_80 = (0.8 * NGAN_SACH * he_so - a) / b     # nguong an toan 80% ngan sach
        tran_100 = (NGAN_SACH * he_so - a) / b
        print("  %-22s: %5.0f nguoi (80%% ngan sach)   |  %5.0f nguoi (100%%, da lag)" % (ten, max(0, tran_80), max(0, tran_100)))
print("\nLuu y: so nguoi o day la BOT (khong co goi mang vao/ra). Nguoi that them chi phi giai ma goi + phat tan.")
