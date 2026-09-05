import sys,re
sys.stdout.reconfigure(encoding="utf-8",errors="replace")
# dem_cu.py <cl.log>: moi TELE (loadmap/vaolandau) -> lenh chay cuoi truoc TELE, echo run cua may chu trong 1200 ms sau, KEO/NAN 8 s sau
tre=re.compile(r"^t=(\d+) pid=(\d+) (.*)$")
cmd_re=re.compile(r"\[S6-CMD\] lenh=(run|walk) npc=\d+ idx=1 ap=\d dich=\((-?\d+),(-?\d+)\)")
me_re=re.compile(r"\[S6-ME\] nhanh=(\w+) cl=\((-?\d+),(-?\d+),(-?\d+),(-?\d+)\) reg=(-?\d+) sv=\((-?\d+),(-?\d+),(-?\d+),(-?\d+)\) reg=(-?\d+) doing=(\d+)")
L=[]
with open(sys.argv[1],"r",encoding="latin-1",errors="replace") as f:
    for l in f:
        m=tre.match(l)
        if not m: continue
        s=m.group(3)
        if "[S6-CMD]" in s and " idx=1 " in s or "nhanh=loadmap" in s or "nhanh=vaolandau" in s or "S13-KEO" in s or "S8-NAN" in s or "[S6-ME]" in s:
            L.append((int(m.group(1)),s.rstrip()))
tot=0;cu=0
for i,(T,s0) in enumerate(L):
    if "nhanh=loadmap" not in s0 and "nhanh=vaolandau" not in s0: continue
    tot+=1
    # last cmd before (strictly earlier index)
    last=None; clcmd=None; lastdoing=None
    for t,s in L[max(0,i-300):i]:
        if t<T-2500: continue
        mc=cmd_re.search(s)
        if mc: last=(int(mc.group(2)),int(mc.group(3)))
        mm=me_re.search(s)
        if mm: lastdoing=mm.group(12)
    echoes=[]; keo=[]; 
    for t,s in L[i+1:i+600]:
        if t-T>8000: break
        mc=cmd_re.search(s)
        if mc and t-T<=1200: echoes.append((t-T,int(mc.group(2)),int(mc.group(3))))
        if "S13-KEO" in s or "S8-NAN" in s: keo.append((t-T,s[:60]))
    stale=[e for e in echoes if last and abs(e[1]-last[0])<=64 and abs(e[2]-last[1])<=64]
    if stale: cu+=1
    print("TELE t=%d doing_truoc=%s lenh_cuoi=%s | echo dau=%s | %s | KEO/NAN 8s: %s"%(T,lastdoing,last,echoes[:2],"LENH CU THI HANH" if stale else "-", [k[0] for k in keo][:4]))
print("TONG:",tot,"TELE; lenh cu thi hanh:",cu)
