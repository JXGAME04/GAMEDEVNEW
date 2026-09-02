# fix4: hs_port2013.py - (1) name2id chi ha chu ASCII (khong pha byte GBK); (2) pak VLTK tim theo basename duoi ROOT data
import io, os
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), "hs_port2013.py")
s = io.open(p, "r", encoding="utf-8").read()
old = '''    import pakdump as P
    idx = {}'''
new = '''    import pakdump as P
    VROOT = r"C:\\Users\\nguye\\Level Up Games\\Vo Lam Truyen Ky\\data"
    vfiles = {}
    for dp, ds, fs in os.walk(VROOT):
        for f in fs:
            if f.lower().endswith((".pak", ".mps")): vfiles[f.lower()] = os.path.join(dp, f)
    def alow(s):  # ha chu ASCII, giu nguyen byte >= 0x80 (GBK)
        return "".join(ch.lower() if ord(ch) < 128 else ch for ch in s)
    idx = {}'''
assert old in s; s = s.replace(old, new)
old = '''        u = P.name2id(img.lower())
        if u not in idx:'''
new = '''        u = P.name2id(alow(img))
        if u not in idx:'''
assert old in s; s = s.replace(old, new)
old = '''                    pk = vidx[u]
                    f, es = P.entries(pk)'''
new = '''                    pk = vfiles.get(vidx[u].lower(), vidx[u])
                    f, es = P.entries(pk)'''
assert old in s; s = s.replace(old, new)
io.open(p, "w", encoding="utf-8").write(s)
print("fix4 ok")
