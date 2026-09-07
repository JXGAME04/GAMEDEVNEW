r"""truy_dinh.py - TRUY THU PHAM gay dinh tick: phan bo max cua tung giai doan theo ky perf,
tuong quan SCRIPT_TIME (RunTime timerserver moi phut) voi TICK max, va tan suat dinh.
  python truy_dinh.py [log ...]
"""
import re, sys, os, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
LOGS = sys.argv[1:] or [r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\jx_perf_server.log"]
RX_HEAD = re.compile(r"^\[PERF\] (\d\d:\d\d:\d\d) tick=(\d+) tre=(\d+) \(([\d.]+)%\) online=(\d+)")
RX_ST = re.compile(r"^\s+([A-Z_]+)\s+n=(\d+)\s+tb=\s*([\d.]+)ms\s+max=\s*([\d.]+)ms\s+p95=\s*([\d.]+)ms")
ky = []; cur = None
for p in LOGS:
    if not os.path.exists(p): continue
    for ln in open(p, encoding="latin-1", errors="replace"):
        m = RX_HEAD.match(ln)
        if m:
            if cur: ky.append(cur)
            cur = {"gio": m.group(1), "tre": int(m.group(3)), "online": int(m.group(5)), "st": {}}
            continue
        if cur is None: continue
        m = RX_ST.match(ln)
        if m: cur["st"][m.group(1)] = {"tb": float(m.group(3)), "max": float(m.group(4)), "p95": float(m.group(5))}
if cur: ky.append(cur)
ky = [k for k in ky if "TICK" in k["st"] and k["online"] > 500]
print("Ky perf co >500 nguoi: %d\n" % len(ky))

# 1) phan bo TICK max
print("== PHAN BO 'TICK max' (dinh cao nhat trong tung phut) ==")
bins = [(0,20),(20,50),(50,100),(100,200),(200,400),(400,10000)]
dem = collections.Counter()
for k in ky:
    v = k["st"]["TICK"]["max"]
    for lo, hi in bins:
        if lo <= v < hi: dem[(lo,hi)] += 1; break
tong = len(ky)
for lo, hi in bins:
    c = dem[(lo,hi)]
    print("  %4d-%-5d ms : %5d ky (%5.1f%%)  %s" % (lo, hi, c, 100.0*c/tong, "#"*int(60.0*c/tong)))

# 2) dinh do dau: so sanh TICK max voi max cua tung giai doan
print("\n== KY CO TICK max > 50 ms: giai doan nao chiu trach nhiem? ==")
nang = [k for k in ky if k["st"]["TICK"]["max"] > 50]
print("So ky co dinh > 50 ms: %d / %d (%.1f%%)" % (len(nang), tong, 100.0*len(nang)/tong))
thu_pham = collections.Counter()
for k in nang:
    tmax = k["st"]["TICK"]["max"]
    best, bestv = "(khong ro)", 0.0
    for nm, v in k["st"].items():
        if nm == "TICK": continue
        if v["max"] > bestv: best, bestv = nm, v["max"]
    # giai doan giai thich duoc >= 60% dinh thi ghi ten no
    thu_pham[best if bestv >= 0.6 * tmax else "(khong giai doan nao)"] += 1
for nm, c in thu_pham.most_common():
    print("  %-18s %4d ky (%5.1f%% so ky co dinh)" % (nm, c, 100.0*c/max(1,len(nang))))

# 3) SCRIPT_TIME: RunTime timerserver moi phut
print("\n== SCRIPT_TIME (RunTime cua timerserver, goi 1 lan/phut) ==")
st = [k["st"].get("SCRIPT_TIME", {}).get("max", 0.0) for k in ky]
st_s = sorted(st)
def pct(v, q): return v[int(len(v)*q)] if v else 0
print("  max moi phut: trung vi %.1f ms | p90 %.1f ms | p99 %.1f ms | lon nhat %.1f ms"
      % (pct(st_s,0.5), pct(st_s,0.9), pct(st_s,0.99), st_s[-1] if st_s else 0))
print("  so phut RunTime > 50 ms : %d (%.1f%%)" % (sum(1 for v in st if v > 50), 100.0*sum(1 for v in st if v > 50)/tong))
print("  so phut RunTime > 200 ms: %d (%.1f%%)" % (sum(1 for v in st if v > 200), 100.0*sum(1 for v in st if v > 200)/tong))

# 4) LUA_CALL max vs SCRIPT_TIME max: dinh Lua co phai deu tu RunTime?
print("\n== DINH LUA co den tu RunTime khong? ==")
ca = sum(1 for k in ky if k["st"].get("LUA_CALL",{}).get("max",0) > 50)
trung = sum(1 for k in ky if k["st"].get("LUA_CALL",{}).get("max",0) > 50
            and abs(k["st"].get("LUA_CALL",{}).get("max",0) - k["st"].get("SCRIPT_TIME",{}).get("max",0)) < 5)
print("  ky co LUA_CALL max > 50 ms: %d; trong do TRUNG voi SCRIPT_TIME (chenh <5 ms): %d (%.0f%%)"
      % (ca, trung, 100.0*trung/max(1,ca)))
print("  -> phan con lai (%d ky) la dinh Lua NGOAI RunTime (script NPC/trap/timer JX2)" % (ca - trung))

# 5) tick tre
tre = sum(k["tre"] for k in ky)
print("\n== TICK TRE ==")
print("  tong tick tre: %d tren ~%d tick (%.3f%%)" % (tre, tong*1080, 100.0*tre/max(1,tong*1080)))
print("  so phut co tick tre: %d (%.1f%%)" % (sum(1 for k in ky if k["tre"]>0), 100.0*sum(1 for k in ky if k["tre"]>0)/tong))
