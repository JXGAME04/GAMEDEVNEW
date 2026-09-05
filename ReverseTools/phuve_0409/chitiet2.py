import sys,re
sys.stdout.reconfigure(encoding="utf-8",errors="replace")
p,T0,a,b=sys.argv[1],int(sys.argv[2]),int(sys.argv[3]),int(sys.argv[4])
pidf=sys.argv[5] if len(sys.argv)>5 else None
tre=re.compile(r"^t=(\d+) pid=(\d+) (.*)$")
me_re=re.compile(r"\[S6-ME\] nhanh=(\w+) cl=\((-?\d+),(-?\d+),(-?\d+),(-?\d+)\) reg=(-?\d+) sv=\((-?\d+),(-?\d+),(-?\d+),(-?\d+)\) reg=(-?\d+) doing=(\d+)")
keep=re.compile(r"S8-NAN|S13-KEO|S12-|S13-|S6-LOADMAP|\[S6-CMD\][^\n]*\bidx=1\b|E4_MOVE_PATH\] npc=1 |\[S6-ADD\][^\n]*\bidx=1\b|\[DT-|\[TK|\[WA|FIGHT-EMIT|CANCAST|SKILL_ABORT|ME-FIGHTMODE me idx=1|SYNCME-FIRSTREGION")
last=None;lastcmd=None;n=0
with open(p,"r",encoding="latin-1",errors="replace") as f:
    for l in f:
        m=tre.match(l)
        if not m: continue
        t=int(m.group(1))
        if pidf and m.group(2)!=pidf: continue
        if t<T0+a: continue
        if t>T0+b: break
        s=m.group(3).rstrip()
        mm=me_re.search(s)
        if mm:
            nh,cx,cy,ox,oy,rc,sx,sy,sox,soy,rs,doing=mm.groups()
            cx,cy,ox,oy,sx,sy,sox,soy=map(int,(cx,cy,ox,oy,sx,sy,sox,soy))
            if rc==rs and rc!="-1":
                dx=((cx-sx)*32768+(ox-sox))/1024.0; dy=((cy-sy)*32768+(oy-soy))/1024.0
                lech="lech=%.0f (dx=%.0f dy=%.0f)"%(max(abs(dx),abs(dy)),dx,dy)
            else: lech="khac reg cl=%s sv=%s"%(rc,rs)
            key=(cx,cy,ox,oy,sx,sy,sox,soy,doing)
            if key==last: continue
            last=key
            print("%+7d ME d=%-2s cl=(%2d,%2d,%5d,%5d) sv=(%2d,%2d,%5d,%5d) %s"%(t-T0,doing,cx,cy,ox,oy,sx,sy,sox,soy,lech))
        elif keep.search(s):
            if "[S6-CMD]" in s:
                k=re.sub(r" t=\d+$","",s)
                if k==lastcmd: continue
                lastcmd=k
            print("%+7d %s"%(t-T0,s[:200]))
        else: continue
        n+=1
        if n>420: print("...(cat)"); break
