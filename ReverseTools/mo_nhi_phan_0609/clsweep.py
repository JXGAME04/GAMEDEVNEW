import time
from cl import D, MD, BASE
t=time.time(); MD.skipdata=True
lo,hi=0x401000,0x69D000
out=open('dis_client.txt','w'); n=0
for ins in MD.disasm(D[lo-BASE:hi-BASE], lo):
    out.write("%08X %s %s\n"%(ins.address,ins.mnemonic,ins.op_str)); n+=1
out.close(); print("client", n, "lenh", round(time.time()-t,1),"s")
