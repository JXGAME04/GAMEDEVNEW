import sys,re
# cuaso.py <log> <T> <truoc_ms> <sau_ms> [regex loc]
p,T,a,b=sys.argv[1],int(sys.argv[2]),int(sys.argv[3]),int(sys.argv[4])
rx=re.compile(sys.argv[5]) if len(sys.argv)>5 else None
tre=re.compile(r"^t=(\d+)")
n=0
with open(p,"r",encoding="latin-1",errors="replace") as f:
    for ln,l in enumerate(f,1):
        m=tre.match(l)
        if not m: continue
        t=int(m.group(1))
        if t<T-a: continue
        if t>T+b: break
        if rx and not rx.search(l): continue
        print("%d\t%+d\t%s"%(ln,t-T,l.rstrip()[:260]))
        n+=1
        if n>400: print("...(cat)"); break
