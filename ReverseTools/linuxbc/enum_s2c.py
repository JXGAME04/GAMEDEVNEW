import re, io
p = r"D:\GAMEDEVNEW\Headers\KProtocolDef.h"
src = io.open(p, encoding="latin-1").read()
# strip comments
src = re.sub(r"/\*.*?\*/", "", src, flags=re.S)
src = re.sub(r"//[^\n]*", "", src)
m = re.search(r"enum\s+s2c_PROTOCOL\s*\{(.*?)\};", src, re.S) or re.search(r"enum\s+\w*\s*\{([^}]*s2c_syncnpcmin[^}]*)\};", src, re.S)
body = m.group(1)
consts = {"g_nGlobalProtocolType": 31}
val = -1; table = {}
for tok in body.split(","):
    tok = tok.strip()
    if not tok: continue
    if "=" in tok:
        name, rhs = [x.strip() for x in tok.split("=")]
        val = consts.get(rhs, None)
        if val is None: val = int(rhs)
    else:
        name = tok; val += 1
    table[name] = val
want = ["s2c_clientbegin","s2c_syncplayer","s2c_syncplayermin","s2c_syncnpc","s2c_syncnpcmin","s2c_syncnpcminplayer","s2c_objadd","s2c_syncobjstate","s2c_objremove","s2c_npcremove","s2c_npcwalk","s2c_npcrun","s2c_npcattack","s2c_npcmagic","s2c_npcjump","s2c_npchurt","s2c_npcdeath","s2c_skillcast","s2c_playertalk","s2c_npcsit","s2c_castskilldirectly","s2c_playerrevive","s2c_show_damage","s2c_scriptdata","s2c_extend","s2c_end"]
for w in want: print(f"{table.get(w)}\t{w}")
used = set(table.values())
free = sorted(set(range(0, 255)) - used)
# compress ranges
rng = []; s = None; prev = None
for v in free:
    if s is None: s = v; prev = v; continue
    if v == prev + 1: prev = v; continue
    rng.append((s, prev)); s = v; prev = v
if s is not None: rng.append((s, prev))
print("entries:", len(table), "free:", len(free), rng)
# which value is 93
inv = {v: k for k, v in table.items()}
for v in (89, 90, 93, 94, 96, 97): print(v, inv.get(v))
