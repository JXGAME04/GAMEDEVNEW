import sys,re
sys.stdout.reconfigure(encoding="utf-8",errors="replace")
# quet.py <log>: moi su kien ha canh (loadnew=1 | vaolandau | loadmap | S6-ADD idx=1) -> dem KEO/NAN/S12-THEO trong 8 s sau
tre=re.compile(r"^t=(\d+) pid=(\d+) (.*)$")
L=[]
with open(sys.argv[1],"r",encoding="latin-1",errors="replace") as f:
    for l in f:
        m=tre.match(l)
        if not m: continue
        s=m.group(3)
        if ("S6-LOADMAP" in s and "loadnew=1" in s) or "nhanh=vaolandau" in s or "nhanh=loadmap" in s or ("[S6-ADD]" in s and " idx=1 " in s) or "S13-KEO" in s or "S8-NAN" in s or "S12-THEO" in s or "S12-CUA" in s or "S13-XA]" in s:
            L.append((int(m.group(1)),m.group(2),s.rstrip()))
ev=[x for x in L if not any(k in x[2] for k in ("S13-KEO","S8-NAN","S12-THEO","S12-CUA","S13-XA]"))]
print("so su kien ha canh:",len(ev))
for t,pid,s in ev:
    after=[(t2-t,s2) for t2,p2,s2 in L if p2==pid and 0<t2-t<=8000 and any(k in s2 for k in ("S13-KEO","S8-NAN","S12-THEO","S13-XA]"))]
    tag="  <-- CO KEO/NAN TRONG 8s" if any(("S13-KEO" in a[1] or "S8-NAN" in a[1]) for a in after) else ""
    print("t=%d pid=%s %s%s"%(t,pid,s[:110],tag))
    for d,s2 in after[:8]:
        print("     %+6d %s"%(d,s2[:150]))
