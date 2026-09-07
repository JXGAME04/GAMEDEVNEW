import re, sys, io
p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\jx_auto_server.log"
f = io.open(p, "r", encoding="latin-1", errors="replace")
dem = 0; loai_max = {}; nguoi_max = None; nguoi_top = []
dem_lines = []
combat_lines = []
for line in f:
    if "[BC-LOAI]" in line:
        parts = re.findall(r" (\d+):(\d+)/(\d+)/(\d+)", line)
        d = {int(a): (int(b), int(c), int(e)) for a, b, c, e in parts}
        for k, (g, s, c) in d.items():
            m = loai_max.get(k, (0, 0, 0))
            loai_max[k] = (max(m[0], g), max(m[1], s), max(m[2], c))
        if any(k in d for k in (91, 92, 95, 87, 88, 207, 148)):
            tot_combat = sum(d.get(k, (0, 0, 0))[1] for k in (87, 88, 91, 92, 95, 148, 207))
            combat_lines.append((tot_combat, line.strip()))
    elif "[BC-NGUOI]" in line:
        m = re.search(r"so_client=(\d+) tong_byte=(\d+) \| nhieu nhat: player=(\d+) ten=(\S+) goi=(\d+) \((\d+)/giay\) byte=(\d+) \((\d+) KB/giay\)", line)
        if m:
            nguoi_top.append((int(m.group(6)), int(m.group(8)), int(m.group(1)), m.group(4), line.strip()[:60]))
    elif "[BC-DEM]" in line:
        m = re.search(r"goi=(\d+) gui=(\d+) cat_vi_het_ngan_sach=(\d+) node_duyet=(\d+)", line)
        if m:
            dem_lines.append((int(m.group(2)), int(m.group(1)), int(m.group(3)), int(m.group(4)), line.strip()[:70]))
print("BC-LOAI windows:", len(combat_lines), "with combat codes")
print("max per-10s by code (goi, gui, cat):")
for k in sorted(loai_max): print("  ", k, loai_max[k])
combat_lines.sort(reverse=True)
print("\nTOP 6 windows by combat packets SENT:")
for t, l in combat_lines[:6]: print("  ", t, "|", l[:300])
nguoi_top.sort(reverse=True)
print("\nTOP 8 [BC-NGUOI] per-client goi/giay:")
for g, kb, nc, ten, l in nguoi_top[:8]: print("  ", g, "goi/giay", kb, "KB/giay clients=", nc, ten, l)
dem_lines.sort(reverse=True)
print("\nTOP 5 [BC-DEM] by gui:")
for gui, goi, cat, node, l in dem_lines[:5]: print("  gui=", gui, "goi=", goi, "cat=", cat, "node=", node, l)
print("\nBC-DEM windows with cat>0:", sum(1 for x in dem_lines if x[2] > 0), "of", len(dem_lines))
