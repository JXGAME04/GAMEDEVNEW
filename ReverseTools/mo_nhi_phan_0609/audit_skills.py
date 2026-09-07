import importlib.util,sys,json
spec=importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py"); dec2=importlib.util.module_from_spec(spec); spec.loader.exec_module(dec2)
sys.stdout.reconfigure(encoding='utf-8',errors='replace')
def load(p):
    rows=[dec2.decline2(l.rstrip(b"\r")).split("\t") for l in open(p,'rb').read().split(b"\n") if l.strip()]
    return rows[0], rows[1:]
hA,A=load(r"D:\ServerLinux\server1\settings\skills.txt"); hB,B=load(r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\skills.txt")
iA={h:i for i,h in enumerate(hA)}; iB={h:i for i,h in enumerate(hB)}
g=lambda r,idx,c: (r[idx[c]] if idx[c]<len(r) else "").strip()
COSMETIC={"SkillName","Property","SkillDesc","SkillIcon","PreCastSpr","ManCastSnd","FMCastSnd","Param1Memo","Param2Memo","Attrib"}
dA={g(r,iA,"SkillId"):r for r in A}; dB={g(r,iB,"SkillId"):r for r in B}
only_linux=sorted(set(dA)-set(dB),key=int); only_jx1=sorted(set(dB)-set(dA),key=int)
print("Linux",len(dA),"JX1",len(dB),"| chi Linux:",len(only_linux),"| chi JX1:",len(only_jx1))
print("  chi Linux ids:", only_linux[:40]); print("  chi JX1 ids:", only_jx1[:60])
diffs={}
from collections import Counter
colcount=Counter()
for sid in dA:
    if sid not in dB: continue
    a=dA[sid]; b=dB[sid]; d=[]
    for c in hA:
        if c in COSMETIC or c not in iB: continue
        va=g(a,iA,c); vb=g(b,iB,c)
        if va!=vb: d.append((c,va,vb)); colcount[c]+=1
    if d: diffs[sid]=d
print("dong co lech co che:",len(diffs))
print("cot hay lech:",colcount.most_common(25))
json.dump(diffs,open('audit_skills_diff.json','w',encoding='utf-8'),ensure_ascii=False,indent=0)
# print compact listing
with open('audit_skills_diff.txt','w',encoding='utf-8') as f:
    for sid in sorted(diffs,key=int):
        f.write("%s %s | %s\n"%(sid, g(dA[sid],iA,"SkillName")[:30], " ; ".join("%s: L=%s J=%s"%(c,va,vb) for c,va,vb in diffs[sid])))
print(open('audit_skills_diff.txt',encoding='utf-8').read()[:6000])
