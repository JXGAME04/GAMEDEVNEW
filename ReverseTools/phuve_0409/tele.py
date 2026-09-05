import sys,re
sys.stdout.reconfigure(encoding="utf-8",errors="replace")
# tele.py <cl.log> <sv.log> : moi lan nhanh=loadmap/vaolandau -> vi tri truoc, diem den (S12-TELE), dich run truoc/sau, ME 3 s sau
tre=re.compile(r"^t=(\d+) pid=(\d+) (.*)$")
me_re=re.compile(r"\[S6-ME\] nhanh=(\w+) cl=\((-?\d+),(-?\d+),(-?\d+),(-?\d+)\) reg=(-?\d+) sv=\((-?\d+),(-?\d+),(-?\d+),(-?\d+)\) reg=(-?\d+) doing=(\d+)")
cmd_re=re.compile(r"\[S6-CMD\] lenh=(\w+) npc=\d+ idx=1 ap=(\d) dich=\((-?\d+),(-?\d+)\)")
fr_re=re.compile(r"SYNCME-FIRSTREGION me idx=1 regcu=(-?\d+) svreg=(-?\d+) cell=\((-?\d+),(-?\d+)\) mps=\((-?\d+),(-?\d+)\)")
lm_re=re.compile(r"SYNCME-LOADMAP me idx=1 cellcu=\((-?\d+),(-?\d+)\) offcu=\((-?\d+),(-?\d+)\) regcu=(-?\d+) svmps=\((-?\d+),(-?\d+)\)")
tele_re=re.compile(r"\[S12-TELE\] \w+ setpos cung map -> bao chinh chu \((-?\d+),(-?\d+)\)")
sv=[]
with open(sys.argv[2],"r",encoding="latin-1",errors="replace") as f:
    for l in f:
        if "[S12-TELE]" in l or "E4_POS_CHANGEWORLD] npc=91477" in l or ("[E4_CMD_ACCEPT] npc=91477" in l) :
            m=tre.match(l)
            if m: sv.append((int(m.group(1)),m.group(3).rstrip()))
L=[]
with open(sys.argv[1],"r",encoding="latin-1",errors="replace") as f:
    for l in f:
        m=tre.match(l)
        if not m: continue
        s=m.group(3)
        if "[S6-ME]" in s or "[S6-CMD]" in s and " idx=1 " in s or "S13-KEO" in s or "S8-NAN" in s or "S12-THEO" in s or "SYNCME-LOADMAP" in s or "E4_MOVE_PATH] npc=1 " in s or "[TK-" in s:
            L.append((int(m.group(1)),s.rstrip()))
ev=[i for i,(t,s) in enumerate(L) if "nhanh=loadmap" in s or "nhanh=vaolandau" in s]
for i in ev:
    T,s0=L[i]
    # truoc: dich run cuoi + ME cuoi
    lastcmd=None; lastme=None; lastmv=None
    for t,s in L[max(0,i-400):i]:
        if t<T-3000: continue
        mc=cmd_re.search(s)
        if mc and mc.group(1) in ("run","walk"): lastcmd=(t-T,mc.group(1),int(mc.group(3)),int(mc.group(4)))
        mm=me_re.search(s)
        if mm: lastme=(t-T,mm.group(12))
        if "E4_MOVE_PATH] npc=1 " in s: lastmv=(t-T,re.search(r"des=\((-?\d+),(-?\d+)\)",s).groups())
    dest=None
    for t,s in sv:
        if abs(t-T)<=400 and "S12-TELE" in s:
            mt=tele_re.search(s); dest=(int(mt.group(1)),int(mt.group(2))); break
    print("\n=== TELE t=%d %s"%(T, "vaolandau" if "vaolandau" in s0 else "loadmap"),"den=",dest,"| truoc: doing=%s, lenh cuoi=%s, path cuoi=%s"%(lastme and lastme[1], lastcmd, lastmv))
    # sau
    n=0
    for t,s in L[i+1:i+400]:
        if t-T>3500: break
        mc=cmd_re.search(s)
        if mc:
            dx=dy=None
            if dest: dx=int(mc.group(3))-dest[0]; dy=int(mc.group(4))-dest[1]
            near_old = (lastcmd and abs(int(mc.group(3))-lastcmd[2])<=64 and abs(int(mc.group(4))-lastcmd[3])<=64)
            print("  %+6d SV-ECHO %s dich=(%s,%s) cach diem den=(%s,%s)%s"%(t-T,mc.group(1),mc.group(3),mc.group(4),dx,dy,"  <== TRUNG DICH CU (lenh ton dong)" if near_old else ""))
            n+=1; continue
        mm=me_re.search(s)
        if mm:
            nh,cx,cy,ox,oy,rc,sx,sy,sox,soy,rs,doing=mm.groups()
            cx,cy,ox,oy,sx,sy,sox,soy=map(int,(cx,cy,ox,oy,sx,sy,sox,soy))
            if rc==rs and rc!="-1":
                ddx=((cx-sx)*32768+(ox-sox))/1024.0; ddy=((cy-sy)*32768+(oy-soy))/1024.0
                print("  %+6d ME d=%s lech=%.0f (dx=%.0f dy=%.0f)"%(t-T,doing,max(abs(ddx),abs(ddy)),ddx,ddy))
            else:
                print("  %+6d ME d=%s khac reg"%(t-T,doing))
            n+=1; continue
        if "S13-KEO" in s or "S8-NAN" in s or "S12-THEO" in s or "TK-" in s:
            print("  %+6d %s"%(t-T,s[:140])); n+=1
        if n>40: print("  ..."); break
