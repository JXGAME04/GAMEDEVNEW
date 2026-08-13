import sys,io
sys.stdout=io.TextIOWrapper(sys.stdout.buffer,encoding='utf-8',errors='replace')
for p in sys.argv[1:]:
    print("########## "+p)
    d=open(p,'rb').read().decode('gbk',errors='replace')
    for i,l in enumerate(d.splitlines(),1): print(i,l)
