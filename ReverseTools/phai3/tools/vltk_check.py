# -*- coding: utf-8 -*-
"""vltk_check.py : gop index 3 dot quet (vltk_scan, vltk_scan15, vltk_scan16) + 5 pak nho da dump,
roi (1) kiem moi nhom tai san can cho 3 phai co trong pak VLTK nao, (2) tra ten tep quan tam trong names_all."""
import os, sys, io
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump as P
BS = chr(92)
def gb(s): return s.encode("gbk").decode("latin-1")
vl = {}
for d in ["vltk_scan", "vltk_scan15", "vltk_scan16"]:
    p = os.path.join(d, "index_all.tsv")
    if not os.path.exists(p): continue
    for l in open(p, encoding="utf-8").read().split("\n")[1:]:
        c = l.split("\t")
        if len(c) > 3: vl.setdefault(c[1], set()).add(c[0])
for pk in ["slistcache", "serverlist", "slistfree", "1024", "updatejx17"]:
    for l in open("vltk_dump/%s/index.tsv" % pk, encoding="utf-8").read().split("\n")[1:]:
        c = l.split("\t")
        if len(c) > 2: vl.setdefault(c[0], set()).add(pk + ".pak")
print("uid VLTK:", len(vl))
jx = {}
for l in open("jx1_client_pak_index.tsv", encoding="utf-8"):
    c = l.rstrip("\n").split("\t")
    if len(c) > 1: jx.setdefault(c[1], []).append(c[0])
def norm(p):
    p = p.strip().replace("/", BS)
    return p if p.startswith(BS) else BS + p
def u8_to_l1(s):  # duong dan unicode (tu tep utf-8) -> chuoi latin-1 mang byte GBK
    return s.encode("gbk", "replace").decode("latin-1")
groups = {}
groups["kynang_icon_cast_sound"] = set()
d = open("out/vltk/slistcache__settings__skills.txt", "rb").read().split(b"\n")
hdr = d[0].rstrip(b"\r").split(b"\t"); ix = {h.decode(): i for i, h in enumerate(hdr)}
want = set(range(1347, 1385)) | set(range(1963, 1993)) | set(range(2114, 2144)) | {1733, 1734}
for l in d[1:]:
    c = l.rstrip(b"\r").split(b"\t")
    if len(c) < 100: continue
    try: sid = int(c[2])
    except: continue
    if sid in want:
        for k in ("SkillIcon", "PreCastSpr", "ManCastSnd", "FMCastSnd"):
            v = c[ix[k]].decode("latin-1").strip()
            if v and v != "0": groups["kynang_icon_cast_sound"].add(norm(v))
def load_list(fn, prefix=""):
    return set(norm(u8_to_l1(prefix + l)) for l in open(fn, encoding="utf-8").read().split("\n") if l.strip())
groups["dan_missile"] = load_list("out/missle_paths.txt")
groups["trangbi_obj"] = load_list("out/equip_paths.txt")
newspr = [l.strip() for l in open("out/npcres_new_sprites_vltk.txt", encoding="utf-8").read().split("\n") if l.strip()]
groups["dongtac_moi_nam"] = set(norm("spr/npcres/man/" + f) for f in newspr if not f.startswith("F"))
groups["dongtac_moi_nu"] = set(norm("spr/npcres/woman/" + f) for f in newspr if not f.startswith("M"))
groups["hang_bang_moi"] = load_list("out/npcres_newrow_sprites.txt")
groups["o_doi"] = load_list("out/npcres_changed_sprites.txt")
groups["npc_3phai"] = load_list("out/npc_faction_sprites.txt")
print("%-24s %6s %8s %8s %8s  %s" % ("nhom", "tong", "JX1 co", "VLTK co", "khong ai", "pak VLTK chua (top)"))
report = {}
for g, ps in groups.items():
    jh = vh = none = 0; paks = {}; missing_vltk = []; only_vltk = []
    for p in ps:
        u = "%08X" % P.name2id(p)
        inj = u in jx; inv = u in vl
        if inj: jh += 1
        if inv:
            vh += 1
            for pk in vl[u]: paks[pk] = paks.get(pk, 0) + 1
            if not inj: only_vltk.append((p, sorted(vl[u])))
        if not inj and not inv: none += 1; missing_vltk.append(p)
    top = ", ".join("%s:%d" % kv for kv in sorted(paks.items(), key=lambda x: -x[1])[:5])
    print("%-24s %6d %8d %8d %8d  %s" % (g, len(ps), jh, vh, none, top))
    report[g] = (only_vltk, missing_vltk)
with io.open("out/vltk_vitri_tai_san.txt", "w", encoding="utf-8") as f:
    for g, (only_vltk, missing) in report.items():
        f.write("## %s — chi VLTK co (%d), khong ai co (%d)\n" % (g, len(only_vltk), len(missing)))
        for p, pk in only_vltk: f.write("%s\t%s\n" % (p.encode("latin-1").decode("gbk", "replace"), ",".join(pk)))
        for p in missing: f.write("%s\tKHONG-CO-O-DAU\n" % p.encode("latin-1").decode("gbk", "replace"))
# tra ten tep quan tam
names = {}
for d in ["vltk_scan", "vltk_scan15", "vltk_scan16"]:
    p = os.path.join(d, "names_all.txt")
    if not os.path.exists(p): continue
    for l in open(p, encoding="utf-8", errors="replace").read().split("\n")[1:]:
        c = l.split("\t")
        if len(c) >= 3: names.setdefault(c[1].lower(), (c[0], c[2]))
print("ten tep kham pha:", len(names))
kws = ["ui3_1024" + BS + "技能", "技能主窗口", "战斗技能", "技能选择树", "meleeres", "goldequipres", "platinaequipres", "clientweaponskill", "advancedskill", "objdata", "icon_zd_", "map_publish" + BS + "wumumenpai", "map_publish" + BS + "xiaoyao", "华山派2013", "faction", "wuhun", "xiaoyao" + BS, "huashan", "门派"]
with io.open("out/vltk_ten_tep_quan_tam.txt", "w", encoding="utf-8") as f:
    for kw in kws:
        hits = [(n, v) for n, v in names.items() if kw.lower() in n]
        f.write("## %s (%d)\n" % (kw, len(hits)))
        print("##", kw, len(hits))
        for n, (u, pk) in sorted(hits)[:400]:
            line = "%s\t%s\t%s" % (u, n, pk)
            f.write(line + "\n")
            if len(hits) <= 40: print("   ", line)
