"""r31_han_chet.py - danh sach tep trong thu muc/ten Han KHONG duoc tham chieu (script/settings/cpp/trap) -> chuyen ra _luutru
   python r31_han_chet.py <scratch dir> [thuc_hien]"""
import os, sys, json, shutil
sys.stdout.reconfigure(encoding='utf-8', errors='replace')
SP = sys.argv[1]
DO = len(sys.argv) > 2 and sys.argv[2] == "thuc_hien"
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
LUU = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\_luutru\0609\disan_jx"
BS = chr(92)
J = json.load(open(os.path.join(SP, "saplai_refs.json"), encoding="utf-8"))
refs, files = J["refs"], J["files"]
trap = set()
for ln in open(os.path.join(SP, "trap_ref_files.txt"), encoding="utf-8", errors="replace"):
    ln = ln.strip()
    if ln:
        trap.add(ln.replace("/", BS).lower())
def key_to_unicode(k):
    return files[k]["unicode"]
han = [k for k in files if any(ord(c) > 127 for c in k)]
giu, chet = [], []
for k in han:
    u = key_to_unicode(k)                       # duong dan unicode tren dia (dung /)
    u_bs = u.replace("/", BS)
    # trap_ref_files ghi duong dan unicode (os.walk) dang \  -> so lower
    is_trap = u_bs.lower() in trap
    if k in refs or is_trap:
        giu.append((u, sorted(refs.get(k, [])) + (["trap"] if is_trap else [])))
    else:
        chet.append(u)
print("Tep Han: %d; GIU (co tham chieu): %d; CHET (khong tham chieu tu dau): %d" % (len(han), len(giu), len(chet)))
for u, r in giu:
    print("  GIU %s  <- %s" % (u, ",".join(r)))
# nhom theo thu muc cap 2
import collections
grp = collections.Counter("/".join(u.split("/")[:2]) for u in chet)
for g, n in grp.most_common():
    print("  %4d  %s" % (n, g))
open(os.path.join(SP, "r31_danhsach_chet.txt"), "w", encoding="utf-8").write("\n".join(sorted(chet)) + "\n")
if DO:
    n = 0
    for u in chet:
        src = os.path.join(ROOT, u.replace("/", os.sep))
        dst = os.path.join(LUU, u.replace("/", os.sep))
        if not os.path.exists(src):
            continue
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.move(src, dst); n += 1
    # xoa thu muc rong con lai trong script/
    removed = 0
    for top in ("script", "scriptjx2"):
        for dp, dn, fn in os.walk(os.path.join(ROOT, top), topdown=False):
            if not os.listdir(dp) and dp != os.path.join(ROOT, top):
                os.rmdir(dp); removed += 1
    print("DA CHUYEN %d tep -> %s; xoa %d thu muc rong" % (n, LUU, removed))
else:
    print("(chua thuc hien - them tham so thuc_hien)")
