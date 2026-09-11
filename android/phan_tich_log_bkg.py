# -*- coding: utf-8 -*-
"""Doc mot phien Fold 7 ban BKG/PALBUF: [VE-BKG] (trinh chieu/bo), [VE-GIAT] theo phan nang nhat, [VE] bang mau, [MAU] gpu/cpu_mhz/W, [REP3] cpu, [SUM] fps."""
import io, re, sys, datetime, collections
ROOT = r"D:\jx1_android_log"


def rd(name, f):
    try:
        return io.open(ROOT + "\\" + name + "\\" + f, encoding="latin-1", errors="replace").read().replace("\r", "")
    except Exception:
        return ""


def avg(a):
    a = [x for x in a if x == x]
    return sum(a) / len(a) if a else float("nan")


def f1(x): return "-" if x != x else "%.1f" % x
def f0(x): return "-" if x != x else "%.0f" % x


RX_SUM = re.compile(r"^\[SUM\] t=(\d+) .*?spikes=(\d+).*?ve: (\d+) khung, cach (\d+)/(\d+)/(\d+) ms.*?ve (\d+)/(\d+) ms tick (\d+)/(\d+) ms.*?PaintFps=(-?\d+)", re.M)
RX_WB = re.compile(r"^\[WORLD b\] t=(\d+) tick=(\d+) \| nhac [\d.]+ ms/tick \| npc ([\d.]+)/tick, tong ([\d.]+) ms/tick.*?dan ([\d.]+)/tick", re.M)
RX_BKG = re.compile(r"^\[(\d+)\] \[VE-BKG\] trinh chieu (\d+) khung, bo vi giong khung truoc (\d+), giong nhung co tai (\d+), giong nhung ep (\d+) \(cua so doi / qua (\d+) ms\), chi dem (\d+), chuoi bo dai nhat (\d+) \| bat=(-?\d+) \| bang mau kieu (\w+): lenh tai TB ([\d.]+) ms/khung \(max ([\d.]+)\)", re.M)
RX_VE = re.compile(r"^\[(\d+)\] \[VE\] \d+s trinh chieu (\d+) khung: cho lenh\+swapchain TB ([\d.]+) ms \(max ([\d.]+)\) \| chep len GPU TB ([\d.]+) \(max ([\d.]+)\): tai (\d+) texture (\d+) KB.*?ghi lenh TB ([\d.]+) \(max ([\d.]+)\): TB (\d+) lenh, (\d+) quad.*?nop TB ([\d.]+) \(max ([\d.]+)\) \| tong TB ([\d.]+) \(max ([\d.]+)\), khung viec \(khong ke cho\) >8 ms (\d+), >16 ms (\d+) \| ve CPU \(Begin->End\) TB ([\d.]+) \(max ([\d.]+)\) \| chep: bang mau (\d+) hang", re.M)
RX_GIAT = re.compile(r"^\[(\d+)\] \[VE-GIAT\] khung (\d+): ([\d.]+) ms \(viec ([\d.]+), khong ke cho\) = ve CPU ([\d.]+) \(nap ([\d.]+) ms.*?ngoai luc ve ([\d.]+)\) \+ trinh chieu ([\d.]+) \(cho ([\d.]+), chep ([\d.]+) \[tai (\d+) tex (\d+) KB.*?ghi ([\d.]+) \[(\d+) lenh.*?nop ([\d.]+)\)(?:.*?pal lenh ([\d.]+))?", re.M)
RX_MAU = re.compile(r"^\[MAU\] (\d\d):(\d\d):(\d\d)\.\d+ t=\d+ man=([\d.]+) Hz.*?nhiet=(-?\d+) headroom=([-\d.]+) \| pin=(\d+)% ([\d.]+)C (\w+) dong=(-?\d+) uA ap=(\d+) mV p=([\d.]+) W(?: \| gpu=(-|\d+)%?(?: gpu_mhz=(\d+))?(?: cpu_mhz=([\d/]+))?)?", re.M)
RX_RAM = re.compile(r"^\[(\d+)\] \[REP3\] RAM rieng (\d+) MB.*?cpu tien trinh (\d+) % \(luong chinh (\d+) %.*?fps TB (\d+)", re.M)
RX_GOP = re.compile(r"^\[(\d+)\] \[VE-GOP\] doi trang thai/khung TB: pipeline (\d+), texture/sampler (\d+) \(max (\d+)\), uniform vs (\d+), ps (\d+)", re.M)


def doc(name):
    ts = datetime.datetime.strptime(name.split("_", 1)[1], "%Y%m%d_%H%M%S")
    paint = rd(name, "jx_paint.log"); rep3 = rd(name, "jx_rep3.log"); tb = rd(name, "jx_thietbi.log")
    S = {}
    S["sum"] = [dict(t=int(g[0]), spk=int(g[1]), khung=int(g[2]), cachmax=int(g[5]), ve=int(g[6]), vemax=int(g[7]), tick=int(g[8]), tickmax=int(g[9]), pf=int(g[10])) for g in RX_SUM.findall(paint)]
    S["wb"] = [dict(t=int(g[0]), npc=float(g[2]), ms=float(g[3]), dan=float(g[4])) for g in RX_WB.findall(paint)]
    S["bkg"] = [dict(t=int(g[0]), tc=int(g[1]), bo=int(g[2]), cotai=int(g[3]), ep=int(g[4]), ms=int(g[5]), dem=int(g[6]), chuoi=int(g[7]), bat=int(g[8]), kieu=g[9], pal=float(g[10]), palmax=float(g[11])) for g in RX_BKG.findall(rep3)]
    S["ve"] = [dict(t=int(g[0]), khung=int(g[1]), cho=float(g[2]), chep=float(g[4]), chepmax=float(g[5]), tai=int(g[6]), ghi=float(g[8]), lenh=int(g[10]), quad=int(g[11]), nop=float(g[12]), nopmax=float(g[13]), v8=int(g[16]), v16=int(g[17]), vecpu=float(g[18]), palhang=int(g[20])) for g in RX_VE.findall(rep3)]
    S["giat"] = []
    for g in RX_GIAT.findall(rep3):
        d = dict(t=int(g[0]), tong=float(g[2]), vecpu=float(g[4]), nap=float(g[5]), ngoai=float(g[6]), tc=float(g[7]), cho=float(g[8]), chep=float(g[9]), tai=int(g[10]), kb=int(g[11]), ghi=float(g[12]), lenh=int(g[13]), nop=float(g[14]), pal=float(g[15]) if g[15] else float("nan"))
        d["vekhac"] = d["vecpu"] - d["nap"] - d["ngoai"]
        S["giat"].append(d)
    mau = []
    for g in RX_MAU.findall(tb):
        wall = ts.replace(hour=int(g[0]), minute=int(g[1]), second=int(g[2]))
        if wall < ts - datetime.timedelta(hours=1):
            wall += datetime.timedelta(days=1)
        cpu = [int(x) for x in g[14].split("/")] if g[14] else []
        mau.append(dict(sec=(wall - ts).total_seconds(), wall=wall, hz=float(g[3]), nhiet=int(g[4]), head=float(g[5]), pin=int(g[6]), temp=float(g[7]), sac=g[8], w=float(g[11]),
                        gpu=(float(g[12]) if g[12] and g[12] != "-" else float("nan")), gmhz=(float(g[13]) if g[13] else float("nan")), cpu=cpu))
    S["mau"] = mau
    S["ram"] = [dict(t=int(g[0]), cpu=int(g[2]), cpumain=int(g[3]), fps=int(g[4])) for g in RX_RAM.findall(rep3)]
    S["gop"] = [dict(t=int(g[0]), pipe=int(g[1]), tex=int(g[2]), ps=int(g[5])) for g in RX_GOP.findall(rep3)]
    S["ts"] = ts; S["name"] = name
    S["loi"] = [l[:160] for l in rep3.split("\n") if "that bai" in l or "BeginGPURenderPass" in l]
    return S


def bang(S):
    print("\n######## %s (%s) ########" % (S["name"], S["ts"].strftime("%H:%M")))
    if S["loi"]:
        print("LOI:", S["loi"][:5])
    print("   t | fps  spk | paint | npc  dan | trchieu    bo  %bo cotai   ep chuoi | pal ms | veCPU  cho chep(max)  ghi lenh  nop | cpu  lc |   W gpu% gmhz cpumax | nh    C")
    marks = [x["t"] for x in S["bkg"]] or [x["t"] for x in S["ve"]]
    tot = dict(tc=0, bo=0, cotai=0, ep=0)
    for T in marks:
        a = T - 30000
        su = [x for x in S["sum"] if a < x["t"] <= T + 1000]; wb = [x for x in S["wb"] if a < x["t"] <= T + 1000]
        b = [x for x in S["bkg"] if x["t"] == T]; b = b[0] if b else None
        ve = [x for x in S["ve"] if abs(x["t"] - T) < 3000]; ve = ve[0] if ve else None
        ram = [x for x in S["ram"] if abs(x["t"] - T) < 3000]; ram = ram[0] if ram else None
        mau = [x for x in S["mau"] if a / 1000.0 < x["sec"] <= T / 1000.0 + 1]
        if b:
            for k in tot:
                tot[k] += b[k]
        fps = sum(x["khung"] for x in su) / (10.0 * len(su)) if su else float("nan")
        print("%4d | %4s %4s | %5s | %4s %4s | %7s %5s %4s %5s %4s %5s | %6s | %5s %4s %4s(%4s) %4s %4s %4s | %3s %3s | %4s %4s %4s %6s | %2s %4s" % (
            T // 1000, f0(fps), sum(x["spk"] for x in su) if su else "-", f1(avg([x["ve"] for x in su])), f0(avg([x["npc"] for x in wb])), f0(avg([x["dan"] for x in wb])),
            b["tc"] if b else "-", b["bo"] if b else "-", f0(100.0 * b["bo"] / max(1, b["tc"] + b["bo"])) if b else "-", b["cotai"] if b else "-", b["ep"] if b else "-", b["chuoi"] if b else "-",
            ("%.3f" % b["pal"]) if b else "-",
            f1(ve["vecpu"]) if ve else "-", f1(ve["cho"]) if ve else "-", f1(ve["chep"]) if ve else "-", f1(ve["chepmax"]) if ve else "-", f1(ve["ghi"]) if ve else "-", ve["lenh"] if ve else "-", f1(ve["nop"]) if ve else "-",
            ram["cpu"] if ram else "-", ram["cpumain"] if ram else "-",
            f1(avg([x["w"] for x in mau if x["sac"] == "khong_sac"])), f0(avg([x["gpu"] for x in mau])), f0(avg([x["gmhz"] for x in mau])), f0(avg([max(x["cpu"]) for x in mau if x["cpu"]])),
            max([x["nhiet"] for x in mau], default=-1), f1(avg([x["temp"] for x in mau]))))
    n = tot["tc"] + tot["bo"]
    if n:
        print("TONG: paint %d, trinh chieu %d (%.0f %%), bo %d (%.0f %%), giong-co-tai %d, ep %d" % (n, tot["tc"], 100.0 * tot["tc"] / n, tot["bo"], 100.0 * tot["bo"] / n, tot["cotai"], tot["ep"]))
    g = [x for x in S["giat"] if x["t"] > 60000]; mins = (S["sum"][-1]["t"] / 60000.0 - 1) if S["sum"] else 1
    cnt = collections.Counter(max({"nap": d["nap"], "ngoai": d["ngoai"], "vekhac": d["vekhac"], "cho": d["cho"], "chep": d["chep"], "ghi": d["ghi"], "nop": d["nop"]}.items(), key=lambda kv: kv[1])[0] for d in g)
    ch = [d for d in g if d["chep"] >= 10]
    print("[VE-GIAT] sau 60 s: %d dong (%.1f/phut); phan lon nhat: %s; chep>=10 ms: %d (%.1f/phut), co bang mau: %d" % (len(g), len(g) / max(0.1, mins), dict(cnt), len(ch), len(ch) / max(0.1, mins), sum(1 for d in ch if d["pal"] == d["pal"] and d["pal"] > 0)))
    if ch:
        print("   chep>=10: ", ", ".join("%ds:%.0f(tai %d/%dKB pal %s)" % (d["t"] // 1000, d["chep"], d["tai"], d["kb"], f1(d["pal"])) for d in ch[:12]))
    vk = [d for d in g if d["vekhac"] >= 20]
    if vk:
        print("   ve khac>=20 ms: %d dong, vi du: %s" % (len(vk), ", ".join("%ds:%.0f(lenh %d)" % (d["t"] // 1000, d["vekhac"], d["lenh"]) for d in vk[:8])))
    su = [x for x in S["sum"] if x["t"] <= 60000]
    print("[SUM] 60 s dau (dang nhap/tai map):", " ".join("t=%d:%d khung pf=%d" % (x["t"] // 1000, x["khung"], x["pf"]) for x in su))
    mau = [x for x in S["mau"] if x["sec"] > 60 and x["sac"] == "khong_sac"]
    if mau:
        ws = sorted(x["w"] for x in mau)
        print("[MAU] sau 60 s: W TB %.2f (trung vi %.2f, p90 %.2f, max %.2f), gpu%% TB %s (max %s), gpu_mhz TB %s, nhiet max %d, pin %d -> %d %% trong %.0f phut, temp %.1f -> %.1f" % (
            avg(ws), ws[len(ws) // 2], ws[int(len(ws) * 0.9)], ws[-1], f0(avg([x["gpu"] for x in mau])), f0(max([x["gpu"] for x in mau if x["gpu"] == x["gpu"]], default=float("nan"))), f0(avg([x["gmhz"] for x in mau])),
            max(x["nhiet"] for x in mau), mau[0]["pin"], mau[-1]["pin"], (mau[-1]["sec"] - mau[0]["sec"]) / 60.0, mau[0]["temp"], mau[-1]["temp"]))
        c = [x["cpu"] for x in mau if x["cpu"]]
        if c:
            print("   cpu_mhz TB tung nhan:", "/".join("%.0f" % avg([r[i] for r in c if len(r) > i]) for i in range(max(len(r) for r in c))))
    if S["gop"]:
        print("[VE-GOP] doi/khung TB: pipeline %.0f, texture %.0f, ps %.0f" % (avg([x["pipe"] for x in S["gop"]]), avg([x["tex"] for x in S["gop"]]), avg([x["ps"] for x in S["gop"]])))


for name in sys.argv[1:]:
    bang(doc(name))
