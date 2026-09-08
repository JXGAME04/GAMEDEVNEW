# -*- coding: utf-8 -*-
"""Phan tich jx_gui_server.log ([GUI-DO]/[GUI-NB]) theo pid va khoang t (GetTickCount ms).
Dung: python ptich_gui.py <pid> <t0> <t1>   (0 = khong gioi han)"""
import re, io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
PID = sys.argv[1] if len(sys.argv) > 1 else "0"
T0 = int(sys.argv[2]) if len(sys.argv) > 2 else 0
T1 = int(sys.argv[3]) if len(sys.argv) > 3 else 0
P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\jx_gui_server.log"
rows = []; nb = []
for line in io.open(P, encoding="latin-1", errors="replace"):
    m = re.match(r"^t=(\d+) pid=(\d+) \[(GUI-DO|GUI-NB)\] (.*)$", line.rstrip())
    if not m: continue
    t = int(m.group(1))
    if PID != "0" and m.group(2) != PID: continue
    if T0 and t < T0: continue
    if T1 and t > T1: continue
    body = m.group(4)
    if m.group(3) == "GUI-DO":
        mm = re.search(r"goi=(\d+) \(dem day gui ngay=(\d+)\) byte=(\d+) t_goi=([\d.]+) ms \| xa=(\d+) nhip, client_xa=(\d+) byte_xa=(\d+) \(([\d.]+) KB/s\) t_xa=([\d.]+) ms \(Write (\d+) lan, ([\d.]+) us/lan\) \| moi nhip: goi ([\d.]+) ms \(max ([\d.]+)\) \+ xa ([\d.]+) ms \(max ([\d.]+)\) \| khoa_rieng=(\d+) client=(\d+)", body)
        if mm:
            g = mm.groups()
            rows.append(dict(t=t, goi=int(g[0]), tran=int(g[1]), byte=int(g[2]), tgoi=float(g[3]), xa=int(g[4]), cxa=int(g[5]), bxa=int(g[6]), kbs=float(g[7]), txa=float(g[8]), nw=int(g[9]), usw=float(g[10]), ngoi=float(g[11]), mgoi=float(g[12]), nxa=float(g[13]), mxa=float(g[14]), kr=int(g[15]), cl=int(g[16])))
    else:
        mm = re.search(r"nhan ban x(\d+) .*goi=(\d+) t=([\d.]+) ms \| xa=(\d+) byte=(\d+) \(([\d.]+) KB/s\) t=([\d.]+) ms \| moi nhip: goi ([\d.]+) ms \(max ([\d.]+)\) \+ xa ([\d.]+) ms \(max ([\d.]+)\) => duong gui uoc tinh moi nhip ([\d.]+) ms", body)
        if mm:
            g = mm.groups()
            nb.append(dict(t=t, n=int(g[0]), goi=int(g[1]), tgoi=float(g[2]), xa=int(g[3]), byte=int(g[4]), kbs=float(g[5]), txa=float(g[6]), ngoi=float(g[7]), mgoi=float(g[8]), nxa=float(g[9]), mxa=float(g[10]), tong=float(g[11])))
if not rows:
    print("khong co dong GUI-DO"); sys.exit(0)
t0 = rows[0]["t"]
print("pid %s: %d cua so 10 s, %.1f phut, khoa_rieng=%d" % (PID, len(rows), (rows[-1]["t"] - t0) / 60000.0, rows[0]["kr"]))
print("%-6s %6s %5s %7s %6s | %6s %7s %7s %6s | %6s %6s %6s %6s" % ("phut", "goi/s", "tran", "KB/s", "cl_xa", "tgoi", "ngoi", "mgoi", "usW", "txa", "nxa", "mxa", "cl"))
for i, r in enumerate(rows):
    if i % 3 and i != len(rows) - 1: continue
    print("%-6.1f %6d %5d %7.1f %6d | %6.2f %7.3f %7.3f %6.1f | %6.2f %6.3f %6.3f %6d" % ((r["t"] - t0) / 60000.0, r["goi"] // 10, r["tran"], r["kbs"], r["cxa"], r["tgoi"], r["ngoi"], r["mgoi"], r["usw"], r["txa"], r["nxa"], r["mxa"], r["cl"]))
n = len(rows)
print("\nTONG: goi/s TB %d max %d | KB/s TB %.1f max %.1f | moi nhip goi TB %.3f ms max %.3f | xa TB %.3f ms max %.3f | Write us TB %.1f max %.1f | dem day gui ngay tong %d" % (
    sum(r["goi"] for r in rows) / n / 10, max(r["goi"] for r in rows) / 10,
    sum(r["kbs"] for r in rows) / n, max(r["kbs"] for r in rows),
    sum(r["ngoi"] for r in rows) / n, max(r["mgoi"] for r in rows),
    sum(r["nxa"] for r in rows) / n, max(r["mxa"] for r in rows),
    sum(r["usw"] for r in rows) / n, max(r["usw"] for r in rows), sum(r["tran"] for r in rows)))
if nb:
    print("\nNHAN BAN x%d: %d cua so" % (nb[0]["n"], len(nb)))
    print("%-6s %8s %7s %8s %7s | %7s %7s %7s %7s | %7s" % ("phut", "goi/10s", "t_goi", "KB/s", "t_xa", "ngoi", "mgoi", "nxa", "mxa", "TONG/nhip"))
    for r in nb:
        print("%-6.1f %8d %7.2f %8.1f %7.2f | %7.3f %7.3f %7.3f %7.3f | %7.3f" % ((r["t"] - t0) / 60000.0, r["goi"], r["tgoi"], r["kbs"], r["txa"], r["ngoi"], r["mgoi"], r["nxa"], r["mxa"], r["tong"]))
    print("max TONG/nhip %.3f ms; max (mgoi+mxa) %.3f ms; KB/s max %.1f" % (max(r["tong"] for r in nb), max(r["mgoi"] + r["mxa"] for r in nb), max(r["kbs"] for r in nb)))
