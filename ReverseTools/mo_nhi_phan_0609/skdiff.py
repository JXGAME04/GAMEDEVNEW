import importlib.util,sys
spec=importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py"); dec2=importlib.util.module_from_spec(spec); spec.loader.exec_module(dec2)
sys.stdout.reconfigure(encoding='utf-8',errors='replace')
def load(p):
    rows=[dec2.decline2(l.rstrip(b"\r")).split("\t") for l in open(p,'rb').read().split(b"\n") if l.strip()]
    return rows[0], rows[1:]
hA,A=load(r"D:\ServerLinux\server1\settings\skills.txt")
hB,B=load(r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\skills.txt")
print("cot Linux",len(hA),"cot JX1",len(hB))
print("cot chi Linux:", [c for c in hA if c not in hB]); print("cot chi JX1:", [c for c in hB if c not in hA])
iA={h:i for i,h in enumerate(hA)}; iB={h:i for i,h in enumerate(hB)}
def row(rows,idx,sid):
    for r in rows:
        if r[idx["SkillId"]]==sid: return r
ids=sys.argv[1:] or ["711","714","715","719","720","723","1223","1236"]
for sid in ids:
    a=row(A,iA,sid); b=row(B,iB,sid)
    print("\n##### SkillId",sid, "| Linux:", a[iA["SkillName"]] if a else None, "| JX1:", b[iB["SkillName"]] if b else None)
    if not a or not b: continue
    for c in hA:
        if c not in iB: continue
        va=a[iA[c]] if iA[c]<len(a) else ""; vb=b[iB[c]] if iB[c]<len(b) else ""
        if va!=vb: print("   %-24s Linux=%-28s JX1=%s"%(c,va,vb))
    # also print full key columns
    keys=["SkillStyle","ReqLevel","MinTimePerCast","LevelDataFile","ScriptFile","LvlSetting1","LvlSetting2","LvlSetting3","LvlSetting4","LvlSetting5","LvlSetting6","EventSkillLevel","AutoDeathSkill"]
    print("   [chung] "+" ".join("%s=%s"%(k,a[iA[k]]) for k in keys if k in iA and iA[k]<len(a)))
