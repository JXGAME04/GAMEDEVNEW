import re, io, sys, statistics
p = "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/jx_perf_server.log"
t = io.open(p, "r", encoding="latin-1", newline="").read()
RE_HEAD = re.compile(r"(\d\d):(\d\d):(\d\d) tick=(\d+) tre=(\d+) \(([\d.]+)%\) online=(\d+)")
RE_ST = re.compile(r"^\s+([A-Z_]+)\s+n=\s*(\d+)\s+tb=\s*([\d.]+)ms max=\s*([\d.]+)ms p95=\s*(\d+)ms chiem=\s*([\d.]+)%", re.M)
rows = []
for b in t.split("[PERF] ")[1:]:
    m = RE_HEAD.match(b)
    if not m: continue
    st = {}
    for s in RE_ST.finditer(b):
        st[s.group(1)] = dict(tb=float(s.group(3)), mx=float(s.group(4)), p95=int(s.group(5)), chiem=float(s.group(6)))
    rows.append(dict(h=int(m.group(1)), mi=int(m.group(2)), tre=int(m.group(5)), tre_pc=float(m.group(6)), online=int(m.group(7)), st=st))
runs = []
for r in rows:
    mm = r["h"] * 60 + r["mi"]
    if runs:
        prev = runs[-1][-1]["h"] * 60 + runs[-1][-1]["mi"]
        if 0 <= mm - prev <= 3:
            runs[-1].append(r); continue
    runs.append([r])
print("Khoi [PERF]: %d; lan chay: %d (cat khi cach > 3 phut hoac lui gio)" % (len(rows), len(runs)))
print("%-3s %-13s %5s %6s | %-22s | %-20s | %-13s | %s" % ("#", "gio", "khoi", "online", "TICK tb/p95/max ms", "SCRIPT tb/max/chiem%", "SW_ACT tb/max", "tre%"))
def mean(xs): return statistics.mean(xs) if xs else 0
for i, R in enumerate(runs):
    du = [r for r in R if r["online"] >= 900]
    if not du: du = [r for r in R if r["online"] >= 500]
    if not du:
        print("%-3d %s-%s %5d %6d | (online thap, bo)" % (i, "%02d:%02d" % (R[0]["h"], R[0]["mi"]), "%02d:%02d" % (R[-1]["h"], R[-1]["mi"]), len(R), max(r["online"] for r in R)))
        continue
    def g(k, f):
        return [r["st"][k][f] for r in du if k in r["st"]]
    print("%-3d %s-%s %5d %6d | %6.2f / %4.1f / %6.1f | %5.3f / %6.1f / %4.2f | %5.2f / %6.1f | %.2f" % (
        i, "%02d:%02d" % (R[0]["h"], R[0]["mi"]), "%02d:%02d" % (R[-1]["h"], R[-1]["mi"]), len(R), max(r["online"] for r in R),
        mean(g("TICK", "tb")), mean(g("TICK", "p95")), max(g("TICK", "mx") or [0]),
        mean(g("SCRIPT_TIME", "tb")), max(g("SCRIPT_TIME", "mx") or [0]), mean(g("SCRIPT_TIME", "chiem")),
        mean(g("SW_ACTIVATE", "tb")), max(g("SW_ACTIVATE", "mx") or [0]), mean(r["tre_pc"] for r in du)))
# phan phoi SCRIPT max cua lan chay cuoi
R = runs[-1]
mx = sorted([r["st"]["SCRIPT_TIME"]["mx"] for r in R if "SCRIPT_TIME" in r["st"] and r["online"] >= 900])
if mx:
    print("Lan cuoi: SCRIPT_TIME max moi phut: median %.1f ms, p90 %.1f, max %.1f (n=%d)" % (mx[len(mx)//2], mx[int(len(mx)*0.9)], mx[-1], len(mx)))
