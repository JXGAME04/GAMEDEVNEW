"""hieunang_sau.py - do hieu nang sau khi test: perf log tu gio HH:MM (lan chay hien tai), bo nho GameServer, thong ke cache.
   python hieunang_sau.py [HH:MM]  (mac dinh: lan chay cuoi trong log)"""
import re, io, sys, statistics, subprocess
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\jx_perf_server.log"
moc = sys.argv[1] if len(sys.argv) > 1 else None
t = io.open(P, "r", encoding="latin-1", newline="").read()
RE_HEAD = re.compile(r"(\d\d):(\d\d):(\d\d) tick=(\d+) tre=(\d+) \(([\d.]+)%\) online=(\d+)")
RE_ST = re.compile(r"^\s+([A-Z_]+)\s+n=\s*(\d+)\s+tb=\s*([\d.]+)ms max=\s*([\d.]+)ms p95=\s*(\d+)ms chiem=\s*([\d.]+)%", re.M)
rows = []
for b in t.split("[PERF] ")[1:]:
    m = RE_HEAD.match(b)
    if not m: continue
    st = {s.group(1): dict(tb=float(s.group(3)), mx=float(s.group(4)), p95=int(s.group(5)), chiem=float(s.group(6))) for s in RE_ST.finditer(b)}
    rows.append(dict(gio="%s:%s" % (m.group(1), m.group(2)), h=int(m.group(1)), mi=int(m.group(2)), tre_pc=float(m.group(6)), online=int(m.group(7)), st=st))
# cat lan chay cuoi
runs = []
for r in rows:
    mm = r["h"] * 60 + r["mi"]
    if runs and 0 <= mm - (runs[-1][-1]["h"] * 60 + runs[-1][-1]["mi"]) <= 3:
        runs[-1].append(r)
    else:
        runs.append([r])
R = runs[-1]
if moc:
    hh, mi = map(int, moc.split(":"))
    R = [r for r in R if r["h"] * 60 + r["mi"] >= hh * 60 + mi]
du = [r for r in R if r["online"] >= 500] or R
print("Lan chay cuoi: %s -> %s, %d khoi (online>=500: %d, max online %d)" % (R[0]["gio"], R[-1]["gio"], len(R), len(du), max(r["online"] for r in R)))
def g(k, f): return [r["st"][k][f] for r in du if k in r["st"]]
def mean(x): return statistics.mean(x) if x else 0.0
print("%-12s %8s %8s %8s %8s" % ("giai doan", "tb ms", "p95 ms", "max ms", "chiem %"))
for k in ("TICK", "SW_ACTIVATE", "SCRIPT_TIME", "LUA_CALL", "SW_MSGLOOP", "GLBMISSION"):
    if g(k, "tb"):
        print("%-12s %8.3f %8.1f %8.1f %8.2f" % (k, mean(g(k, "tb")), mean(g(k, "p95")), max(g(k, "mx")), mean(g(k, "chiem"))))
tick = mean(g("TICK", "tb")); lua = mean(g("LUA_CALL", "tb"))
if tick and lua:
    print("=> Lua chiem %.1f %% cua tick (LUA_CALL/TICK), %.2f %% thoi gian thuc" % (lua / tick * 100, mean(g("LUA_CALL", "chiem"))))
print("Tick tre: %.2f %%" % mean(r["tre_pc"] for r in du))
try:
    out = subprocess.run(["powershell", "-NoProfile", "-Command",
        "Get-Process GameServer -ErrorAction SilentlyContinue | Select-Object Id,StartTime,@{n='WS_MB';e={[int]($_.WorkingSet64/1MB)}},@{n='CPU_s';e={[int]$_.CPU}} | Format-Table -AutoSize | Out-String"],
        capture_output=True, text=True, timeout=20).stdout.strip()
    print(out)
except Exception as e:
    print("(khong doc duoc tien trinh)", e)
