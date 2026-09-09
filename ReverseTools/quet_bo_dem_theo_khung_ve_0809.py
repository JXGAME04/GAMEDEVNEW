# -*- coding: utf-8 -*-
"""quet_bo_dem_theo_khung_ve_0809.py - [MAU 08/09] RA SOAT: moi bo dem thoi gian nam trong ham VE se chay theo so khung ve.
O PaintFps=120 no nhanh gap 6,7 lan nhip goc 18/s (da dinh 3 lan: lam muot huong m_ResDir, alpha bong mo, so sat thuong m_nBloodTime).
Quet cac tep client cua Core + S3Client: tim ham co ten Paint/Draw/Render/Show (hoac duoc goi tu duong ve) co ++/--/+= tren thanh vien
m_* hoac bien tinh, VA khong co chot theo tick (SubWorld[0].m_dwCurrentTime / GetTickCount / timeGetTime) trong cung ham."""
import io, os, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources"
TEPS = [r"Core\Src", r"S3Client", r"S3Client\Ui", r"S3Client\Ui\UiCase", r"Represent\Represent3"]
VE = re.compile(r"\b(Paint|Draw|Render|Show|Blt|Flush)\w*\s*\(", re.I)
DEM = re.compile(r"(\b(?:m_|s_|g_)\w+|\w+\[\w+\])\s*(\+\+|--|\+=\s*1\b|-=\s*1\b)|(\+\+|--)\s*(\b(?:m_|s_|g_)\w+)")
CHOT = re.compile(r"m_dwCurrentTime|GetTickCount|timeGetTime|m_dwLastTick|m_dwBloodTick|m_dwLastDirTick|QueryPerformance")
BO_QUA = re.compile(r"^\s*(//|/\*|\*)")
def ham(sig):
    m = re.search(r"([A-Za-z_]\w*)::([A-Za-z_]\w*)\s*\(", sig)
    return (m.group(1) + "::" + m.group(2)) if m else None
ket = []
for d in TEPS:
    dd = os.path.join(ROOT, d)
    if not os.path.isdir(dd): continue
    for ten in sorted(os.listdir(dd)):
        if not ten.lower().endswith(".cpp"): continue
        p = os.path.join(dd, ten)
        try: L = io.open(p, "r", encoding="latin-1", newline="").read().split("\r\n")
        except Exception: continue
        i = 0; n = len(L)
        while i < n:
            s = L[i]
            nm = ham(s) if ("::" in s and "(" in s and not s.strip().startswith(("//", "*", "/*"))) else None
            if nm and VE.search(nm + "("):
                # than ham: tu dau { den } cung cot
                j = i
                while j < n and "{" not in L[j] and j - i < 6: j += 1
                sau = 0; k = j; than = []
                while k < n:
                    sau += L[k].count("{") - L[k].count("}")
                    than.append((k + 1, L[k]))
                    k += 1
                    if sau <= 0 and k > j: break
                    if k - j > 900: break
                txt = "\n".join(x[1] for x in than)
                if CHOT.search(txt):
                    i = k; continue
                for ln, dong in than:
                    if BO_QUA.match(dong): continue
                    if "for" in dong or "while" in dong: continue
                    m = DEM.search(dong)
                    if m:
                        bien = m.group(1) or m.group(4) or ""
                        if bien.startswith(("m_", "s_", "g_")):
                            ket.append((os.path.join(d, ten), ln, nm, bien, dong.strip()[:110]))
                i = k; continue
            i += 1
for f, ln, nm, bien, dong in ket:
    print("%-34s %6d  %-32s %-24s %s" % (f, ln, nm, bien, dong))
print("\nTONG: %d cho nghi ngo" % len(ket))
