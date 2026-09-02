# -*- coding: utf-8 -*-
r"""vhtd_spr_pak2.py -- sprite DONG TAC vu khi + than cho Vu Hon / Tieu Dao.

Lap danh sach ten tep engine se hoi, dung nhu KNpcResNode::Init (Core\Src\KNpcResNode.cpp:186-232):
  o (dong equipNo+2, cot k+2) cua bang part (k theo THU TU action_file_name.txt, 75 dong tac)
  -> ComposePathAndName(ResFilePath cua npc_res_kind_file_name.txt, o)  = '\' + 'spr\npcres\man' + '\' + o
  -> KPakList::FindElemFile -> FileNameToId == pakdump.name2id. (vu khi khong co *_effect.txt -> khong hoi spr hieu ung)
Tra uid trong MOI pak khai o bin\client\package.ini (thu tu uu tien; TRU pak dich de idempotent); ten thieu -> tra
pak client VLTK theo thu tu package.ini cua VLTK (+ pak le trong data) -> dong vao sprvuhontieudao2.pak
(header 32 byte, index cuoi tep, chep nguyen blob+co -- dung lai vhtd_spr_pak.ghi_pak/doi_chung_pak),
dang ky khoa ke tiep (max+1) trong package.ini (ban luu .truoc_vhtd_0209 da co -> khong tao lai).

  python vhtd_spr_pak2.py --kiem      chay thu: khong ghi pak/ini (chi ghi danh sach .tsv canh tool)
  python vhtd_spr_pak2.py             thi cong (idempotent)
  python vhtd_spr_pak2.py --ghi-de    thay pak dich neu da co ma khac noi dung
Pham vi: hang NpcRes 268,269,276 cua Man/Lady RightWeapon + LeftWeapon ; hang than 156,157 cua ManBody/LadyBody.
"""
import os, sys, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import vhtd_spr_pak as V
P = V.P
log = V.log

OUT2_NAME   = "sprvuhontieudao2.pak"
V.OUT_NAME  = OUT2_NAME                                   # buoc_ini dang ky ten nay
V.OUT_PAK   = os.path.join(V.CLIENT, "data", OUT2_NAME)   # buoc_pak / pak_da_dung / ghi_index_tsv dung
V.INDEX_OUT = os.path.join(V.HERE, "vhtd_spr_pak2_index.tsv")
NPCRES      = os.path.join(V.CLIENT, "settings", "NpcRes")
KIND        = os.path.join(NPCRES, "npc_res_kind_file_name.txt")
ACTION      = os.path.join(NPCRES, "action_file_name.txt")
DS_OUT      = os.path.join(V.HERE, "vhtd_spr_pak2_danhsach.tsv")
VLTK_ROOT   = os.path.dirname(V.VLTK_DATA)
VLTK_INI    = os.path.join(VLTK_ROOT, "package.ini")

# (CharacterName trong kind file, cot part, danh sach equipNo)
YEUCAU = [
    ("MainMan",  "RightWeapon", [268, 269, 276]),
    ("MainMan",  "LeftWeapon",  [268, 269, 276]),
    ("MainLady", "RightWeapon", [268, 269, 276]),
    ("MainLady", "LeftWeapon",  [268, 269, 276]),
    ("MainMan",  "Body",        [156, 157]),
    ("MainLady", "Body",        [156, 157]),
]


def doc_tab(p):
    """KTabFile: dong 1 = header; tra list dong, moi dong list o (str latin-1), bo CR va dong trong cuoi."""
    raw = open(p, "rb").read()
    rows = [l.rstrip(b"\r").decode("latin-1").split("\t") for l in raw.split(b"\n")]
    while rows and rows[-1] == [""]:
        rows.pop()
    return rows


def o(rows, r, c):
    """GetString(r, c) 1-based, ngoai bang -> '' (KTabFile tra chuoi rong)."""
    if r < 1 or r > len(rows) or c < 1 or c > len(rows[r - 1]):
        return ""
    return rows[r - 1][c - 1]


def compose(res_path, name):
    """KNpcResNode::ComposePathAndName (KNpcResNode.cpp:422)."""
    if not name:
        return ""
    if not res_path:
        return name
    s = "" if res_path.startswith("\\") else "\\"
    s += res_path
    if not s.endswith("\\"):
        s += "\\"
    return s + name


def lap_ten():
    """-> (danh sach o, actions). Moi o: dict(nv, part, bang, dong, equip, k, action, cell, path, uid, cham)."""
    kind = doc_tab(KIND)
    hdr = kind[0]
    def kcol(name):
        return hdr.index(name) + 1
    actions = [r[0] for r in doc_tab(ACTION)[1:]]          # CActionName: 75 dong tac theo thu tu
    n_act = len(actions)
    act_no = {a: i for i, a in enumerate(actions)}
    out = []
    for nv, part, equips in YEUCAU:
        krow = next(i + 1 for i, r in enumerate(kind) if r and r[0] == nv)
        res_path = o(kind, krow, kcol("ResFilePath"))
        bang = o(kind, krow, kcol(part))
        tab1 = o(kind, krow, kcol("WeaponActionTab1"))
        tab2 = o(kind, krow, kcol("WeaponActionTab2"))
        rows = doc_tab(os.path.join(NPCRES, bang))
        n_equip = len(rows) - 1
        eff = os.path.join(NPCRES, bang[:-4] + V.__dict__.get("EFFECT_INFO_NAME", "_effect") + ".txt")
        log("  %s/%s: %s (%d hang equip, %d cot), ResFilePath=%r, %s" % (
            nv, part, bang, n_equip, len(rows[0]), res_path, "CO _effect.txt -> engine hoi them spr hieu ung!" if os.path.exists(eff) else "khong co _effect.txt"))
        # dong tac cham toi qua WeaponActionTab1/2 (m_NoHorseTable/m_OnHorseTable: hang = equipNo, cot = viec dang lam)
        t1 = doc_tab(os.path.join(NPCRES, tab1)) if tab1 else []
        t2 = doc_tab(os.path.join(NPCRES, tab2)) if tab2 else []
        for e in equips:
            if e >= n_equip:
                raise SystemExit("%s hang %d vuot %d hang" % (bang, e, n_equip))
            cham = set()
            if part.endswith("Weapon"):
                for t in (t1, t2):
                    if t and e + 1 < len(t):
                        for name in t[e + 1][1:]:
                            if name in act_no:
                                cham.add(act_no[name])
            else:
                cham = set(range(n_act))                    # than: moi dong tac deu co the dung
            for k in range(n_act):
                cell = o(rows, e + 2, k + 2)
                path = compose(res_path, cell)
                out.append(dict(nv=nv, part=part, bang=bang, dong=e + 2, equip=e, k=k, action=actions[k], cell=cell,
                                path=path, uid=P.name2id(path) if path else 0, cham=1 if k in cham else 0,
                                eqname=o(rows, e + 2, 1)))
    return out, actions


def doc_ini_paks(ini):
    """[Package] Path + khoa 0..N lien tuc (KPakList::Open dung o khoa thieu) -> (path, [pak])."""
    raw = open(ini, "rb").read().decode("latin-1")
    sec, path, kv = False, "", {}
    for l in raw.splitlines():
        s = l.strip()
        if s.startswith("[") and s.endswith("]"):
            sec = s.lower() == "[package]"
            continue
        if not sec or "=" not in s:
            continue
        k, v = [x.strip() for x in s.split("=", 1)]
        if k.lower() == "path":
            path = v
        elif k.isdigit() and v:
            kv[int(k)] = v
    order = []
    i = 0
    while i in kv:
        order.append(kv[i])
        i += 1
    return path, order


def quet_pak(root, order, bo=()):
    """Doc BANG INDEX (khong doc du lieu) moi pak theo thu tu -> {uid: [(pak, entry)]} (pak dau = uu tien)."""
    idx, co = {}, []
    for pak in order:
        if pak.lower() in [b.lower() for b in bo]:
            continue
        p = os.path.join(root, pak)
        if not os.path.exists(p):
            continue
        try:
            f, es = P.entries(p)
            f.close()
        except Exception as ex:
            log("  bo pak %s: %s" % (pak, ex))
            continue
        co.append(pak)
        for e in es:
            idx.setdefault(e[0], []).append((pak, e))
    return idx, co


def main():
    args = sys.argv[1:]
    kiem = "--kiem" in args
    ghi_de = "--ghi-de" in args
    n_mau = 5
    for a in args:
        if a.startswith("--mau="):
            n_mau = int(a.split("=", 1)[1])
    log("=== vhtd_spr_pak2 %s ===" % ("CHAY THU (--kiem)" if kiem else "THI CONG"))

    log("[TEN] lap ten theo KNpcResNode::Init / ComposePathAndName")
    cells, actions = lap_ten()
    co_ten = [c for c in cells if c["path"]]
    paths = collections.OrderedDict()
    for c in co_ten:
        paths.setdefault(c["path"], []).append(c)
    log("  %d o (%d hang x %d dong tac), %d o rong, %d ten tep khac nhau" % (
        len(cells), len(cells) // len(actions), len(actions), len(cells) - len(co_ten), len(paths)))

    # JX1: moi pak khai o package.ini (tru pak dich de idempotent)
    jpath, jorder = doc_ini_paks(V.PKG_INI)
    jroot = os.path.join(V.CLIENT, jpath.strip("\\/"))
    jidx, jco = quet_pak(jroot, jorder, bo=(OUT2_NAME,))
    log("[JX1] %s: %d khoa lien tuc, %d pak mo duoc (bo %s), %d uid" % (V.PKG_INI, len(jorder), len(jco), OUT2_NAME, len(jidx)))
    # pak dich neu da co (lan chay truoc)
    pak2_uid = set()
    if os.path.exists(V.OUT_PAK):
        f, es = P.entries(V.OUT_PAK)
        f.close()
        pak2_uid = set(e[0] for e in es)
        log("  pak dich da ton tai: %d muc" % len(pak2_uid))

    # VLTK: theo package.ini cua VLTK + pak le trong data
    vpath, vorder = doc_ini_paks(VLTK_INI) if os.path.exists(VLTK_INI) else ("\\data", [])
    le = sorted(x for x in os.listdir(V.VLTK_DATA) if x.lower().endswith(".pak") and x not in vorder)
    vidx, vco = quet_pak(V.VLTK_DATA, vorder + le)
    log("[VLTK] %s: %d pak theo ini + %d pak le, %d pak mo duoc, %d uid" % (VLTK_INI, len(vorder), len(le), len(vco), len(vidx)))

    co_jx1, tu_vltk, thieu = {}, {}, []
    for path, occ in paths.items():
        u = occ[0]["uid"]
        if u in jidx:
            co_jx1[path] = jidx[u][0][0]
        elif u in vidx:
            tu_vltk[path] = vidx[u][0]              # (pak, entry) uu tien pak dau theo ini VLTK
        else:
            thieu.append(path)
    for c in co_ten:
        p = c["path"]
        if p in co_jx1:
            c["tt"] = "JX1:" + co_jx1[p]
        elif p in tu_vltk:
            c["tt"] = "VLTK:%s -> %s" % (tu_vltk[p][0], OUT2_NAME)
        else:
            c["tt"] = "THIEU CA HAI"
        if c["uid"] in pak2_uid:
            c["tt"] += " (da o pak dich)"
    for c in cells:
        c.setdefault("tt", "(o rong)")

    # tom tat theo hang
    log("[KET QUA] theo hang (ten khac nhau trong hang | JX1 co | them tu VLTK | thieu ca hai | cham toi qua ActionTab):")
    for key, grp in collections.OrderedDict((k, [c for c in co_ten if (c["bang"], c["dong"]) == k])
                                            for k in collections.OrderedDict(((c["bang"], c["dong"]), 1) for c in co_ten)).items():
        ps = collections.OrderedDict((c["path"], c) for c in grp)
        nj = sum(1 for p in ps if p in co_jx1)
        nv = sum(1 for p in ps if p in tu_vltk)
        nt = sum(1 for p in ps if p in thieu)
        nc = len(set(c["path"] for c in grp if c["cham"]))
        log("  %-20s dong %3d equip %3d %-24s : %3d ten | %3d | %3d | %3d | %3d cham toi" % (
            key[0], key[1], grp[0]["equip"], grp[0]["eqname"].encode("latin-1").decode("cp1258", "replace")[:24], len(ps), nj, nv, nt, nc))
    log("  TONG: %d ten khac nhau: JX1 co %d, them tu VLTK %d, THIEU CA HAI %d" % (len(paths), len(co_jx1), len(tu_vltk), len(thieu)))
    if thieu:
        log("  THIEU CA HAI (ten | hang dung):")
        for p in thieu:
            occ = paths[p]
            hang = sorted(set("%s:%d" % (c["bang"], c["dong"]) for c in occ))
            cham = any(c["cham"] for c in occ)
            log("   %-46s uid %08X | %s | %s" % (p, occ[0]["uid"], ", ".join(hang), "CHAM TOI" if cham else "khong cham toi qua ActionTab"))
    nguon = collections.Counter(v[0] for v in tu_vltk.values())
    if nguon:
        log("  pak nguon VLTK: %s" % dict(nguon))

    # danh sach day du
    with open(DS_OUT, "wb") as w:
        w.write(b"nhanvat\tpart\tbang\tdong_tep\tequipNo\tk\taction\to\tpath\tuid\tcham_toi\ttrang_thai\r\n")
        for c in cells:
            w.write(("%s\t%s\t%s\t%d\t%d\t%d\t%s\t%s\t%s\t%s\t%d\t%s\r\n" % (
                c["nv"], c["part"], c["bang"], c["dong"], c["equip"], c["k"], c["action"], c["cell"], c["path"],
                "%08X" % c["uid"] if c["path"] else "", c["cham"], c["tt"])).encode("latin-1", "replace"))
    log("  danh sach: %s (%d dong)" % (DS_OUT, len(cells)))

    # dong pak
    md5 = None
    if not tu_vltk:
        log("[PAK] khong co ten nao can them tu VLTK -> khong tao/ghi %s" % OUT2_NAME)
        if os.path.exists(V.OUT_PAK):
            log("  (pak dich da ton tai tu lan truoc: giu nguyen)")
    else:
        items = [(path, pe[0], occ_uid) for path, pe in tu_vltk.items() for occ_uid in [paths[path][0]["uid"]]]
        plan, thieu2, warn = V.lap_kehoach(items)
        log("[PAK] ke hoach %d muc (thieu %d, canh bao %s)" % (len(plan), len(thieu2), dict(warn) or "-"))
        md5 = V.buoc_pak(plan, kiem, ghi_de, n_mau)
    if os.path.exists(V.OUT_PAK) or (kiem and tu_vltk):
        V.buoc_ini(kiem)
    log("=== TOM TAT: %d ten can / JX1 co %d / them tu VLTK %d / thieu ca hai %d ; pak %s md5 %s ===" % (
        len(paths), len(co_jx1), len(tu_vltk), len(thieu), OUT2_NAME, md5 or "-"))


if __name__ == "__main__":
    main()
