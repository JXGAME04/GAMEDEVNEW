import sys,re,io
sys.stdout.reconfigure(encoding="utf-8",errors="replace")
# hacanh.py <log...> : moi su kien loadnew=1 -> dong thoi gian [-1500,+15000]
tre=re.compile(r"^t=(\d+) pid=(\d+) (.*)$")
me_re=re.compile(r"\[S6-ME\] nhanh=(\w+) cl=\((-?\d+),(-?\d+),(-?\d+),(-?\d+)\) reg=(-?\d+) sv=\((-?\d+),(-?\d+),(-?\d+),(-?\d+)\) reg=(-?\d+) doing=(\d+)")
keep=re.compile(r"S8-NAN|S13-KEO|S12-|S13-|SYNCME-FIRSTREGION|S6-LOADMAP|\[S6-CMD\][^\n]*\bidx=1\b|E4_MOVE_PATH\] npc=1 |NPCSYNC|S6-ADD[^\n]*\bidx=1\b|SYNCWORLD|WORLD-SYNC|\[S6-SYNCFULL|PLAYERSYNC|ME-FIGHTMODE|\[WA|\[TK|\[DT")
for p in sys.argv[1:]:
    lines=[]
    with open(p,"r",encoding="latin-1",errors="replace") as f:
        for ln,l in enumerate(f,1):
            m=tre.match(l)
            if not m: continue
            lines.append((int(m.group(1)),m.group(2),m.group(3).rstrip(),ln))
    ev=[i for i,(t,pid,s,ln) in enumerate(lines) if "S6-LOADMAP" in s and "loadnew=1" in s]
    print("#### %s : %d lan doi map that"%(p,len(ev)))
    for i in ev:
        T0,pid0,s0,ln0=lines[i]
        print("\n=== DOI MAP t=%d pid=%s dong %d : %s"%(T0,pid0,ln0,s0))
        j=i
        while j>0 and lines[j-1][0]>=T0-1500: j-=1
        lastme=None; n=0
        while j<len(lines) and lines[j][0]<=T0+15000:
            t,pid,s,ln=lines[j]; j+=1
            if pid!=pid0: continue
            m=me_re.search(s)
            if m:
                nh,cx,cy,ox,oy,rc,sx,sy,sox,soy,rs,doing=m.groups()
                cx,cy,ox,oy,sx,sy,sox,soy=map(int,(cx,cy,ox,oy,sx,sy,sox,soy))
                if rc==rs and rc!="-1":
                    dx=((cx-sx)*32768+(ox-sox))/1024.0; dy=((cy-sy)*32768+(oy-soy))/1024.0
                    lech="lech=%.0f mps (dx=%.0f dy=%.0f)"%(max(abs(dx),abs(dy)),dx,dy)
                else:
                    lech="khac reg cl=%s sv=%s"%(rc,rs)
                key=(nh,cx,cy,ox,oy,sx,sy,sox,soy,doing)
                if key==lastme: continue
                lastme=key
                print("%+7d  ME %-10s doing=%-2s cl=(%d,%d,%d,%d) sv=(%d,%d,%d,%d) %s"%(t-T0,nh,doing,cx,cy,ox,oy,sx,sy,sox,soy,lech))
                n+=1
            elif keep.search(s):
                print("%+7d  %s"%(t-T0,s[:200])); n+=1
            if n>140: print("...(cat)"); break
