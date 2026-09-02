# -*- coding: utf-8 -*-
r"""vhtd_npc_spr.py -- kiem + sua bang + dong sprite NPC (NormalNpc) cho Vu Hon / Tieu Dao.

Cach engine ghep ten (Core\Src\KNpcResNode.cpp:355-420, nhanh NPC_RES_NORMAL):
  KNpc.cpp:5963  g_NpcSetting(npcs.txt client).GetString(id+2, "NpcResType") -> ten kieu
  KNpcResList::AddNpcRes -> KNpcResNode::Init(ten): npc_res_kind_file_name.txt FindRow(ten) (g_StrCmp: dung hoa/thuong,
    du do dai; lay hang DAU neu trung) -> CharacterType, ResFilePath ; NormalNpc: npc_normal_res_file.txt FindRow(ten)
    -> moi dong tac trong npc_action_name.txt (14): o cot CUNG TEN -> ComposePathAndName(ResFilePath, o)
    = '\' + ResFilePath + '\' + o ; npc_normal_spr_info_file.txt cung cot, mac dinh "16,8,0".
  Thieu hang kind/normal_res -> Init FALSE -> AddNpcRes NULL -> KNpcRes::Init FALSE (NPC khong co res).
Doi chieu bang JX1 voi bang VLTK da trich (vltk_raw\npcres\人物类型.txt / 普通npc资源.txt / 普通npc资源信息.txt):
  CHI lay phien ban VLTK cho 1 kieu khi no lam TANG so tep tim duoc trong pak (JX1 hoac VLTK) va khong mat tep nao.
Tra uid (pakdump.name2id, byte GBK giu nguyen) trong bang index that cua moi pak khai o package.ini (tru pak dich);
tep chi VLTK co -> dong sprvuhontieudao2.pak (vhtd_spr_pak.buoc_pak: header 32, nguyen blob+co) + dang ky khoa ke tiep.

  python vhtd_npc_spr.py --kiem      chay thu (khong ghi pak/ini/bang; chi ghi .tsv canh tool)
  python vhtd_npc_spr.py             thi cong (idempotent)
  python vhtd_npc_spr.py --ghi-de    thay pak dich neu da co ma khac
Bang client duoc ghi NHI PHAN (byte y nguyen, EOL cua tep), ban luu <tep>.truoc_vhtd_0209 chi tao neu chua co.
"""
import os, sys, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import vhtd_spr_pak as V
import vhtd_spr_pak2 as V2          # import da tro V.OUT_PAK/OUT_NAME sang sprvuhontieudao2.pak
P = V.P
log = V.log

NPCRES   = V2.NPCRES
NPCS_S   = os.path.join(os.path.dirname(V.CLIENT), "server", "settings", "npcs.txt")
NPCS_C   = os.path.join(V.CLIENT, "settings", "npcs.txt")
KIND     = os.path.join(NPCRES, "npc_res_kind_file_name.txt")
NRES     = os.path.join(NPCRES, "npc_normal_res_file.txt")
NINFO    = os.path.join(NPCRES, "npc_normal_spr_info_file.txt")
NACT     = os.path.join(NPCRES, "npc_action_name.txt")
VLTK_NPCRES = r"D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw\npcres"
V_KIND   = os.path.join(VLTK_NPCRES, "\u4eba\u7269\u7c7b\u578b.txt")                 # 人物类型.txt
V_NRES   = os.path.join(VLTK_NPCRES, "\u666e\u901anpc\u8d44\u6e90.txt")              # 普通npc资源.txt
V_INFO   = os.path.join(VLTK_NPCRES, "\u666e\u901anpc\u8d44\u6e90\u4fe1\u606f.txt")  # 普通npc资源信息.txt
DS_OUT   = os.path.join(V.HERE, "vhtd_npc_spr_danhsach.tsv")
UU_TIEN  = [2467, 2607, 2468, 2469, 2470, 2471, 2472, 2473, 2474, 2475, 2480, 2481, 2482, 2483, 2490, 2494, 1846, 625,
            2608, 2609, 2610, 2611, 2612, 2613, 2614]
ID_MOI   = list(range(2185, 2643))
KIND_ALIAS = {b"RightHand": b"RightHead"}     # VLTK ghi nham 'RightHead'


class Bang(object):
    """Bang tab: giu nguyen byte tung dong; map ten -> chi so dong DAU (nhu KTabFile::FindRow)."""
    def __init__(self, p):
        self.p = p
        raw = open(p, "rb").read()
        self.eol = b"\r\n" if b"\r\n" in raw else b"\n"
        self.lines = raw.split(b"\n")
        self.cuoi_co_eol = raw.endswith(b"\n")
        self.rows = [l.rstrip(b"\r").split(b"\t") for l in self.lines]
        while self.rows and self.rows[-1] == [b""]:
            self.rows.pop()
        self.hdr = self.rows[0] if self.rows else []
        self.idx = collections.OrderedDict()
        self.dup = collections.defaultdict(list)
        for i, r in enumerate(self.rows[1:]):
            if r and r[0]:
                self.dup[r[0]].append(i + 1)
                self.idx.setdefault(r[0], i + 1)
        self.dup = {k: v for k, v in self.dup.items() if len(v) > 1}
        self.thay_doi = []

    def row(self, name):
        i = self.idx.get(name)
        return self.rows[i] if i is not None else None

    def cell(self, row, colname):
        if row is None or colname not in self.hdr:
            return b""
        c = self.hdr.index(colname)
        return row[c] if c < len(row) else b""

    def dat_row(self, name, new_row, ghichu):
        """thay hang (hang DAU) hoac them cuoi."""
        if name in self.idx:
            i = self.idx[name]
            if self.rows[i] == new_row:
                return False
            self.rows[i] = new_row
            self.thay_doi.append("THAY dong %d %s: %s" % (i + 1, name.decode("latin-1"), ghichu))
        else:
            self.rows.append(new_row)
            self.idx[name] = len(self.rows) - 1
            self.thay_doi.append("THEM dong %d %s: %s" % (len(self.rows), name.decode("latin-1"), ghichu))
        return True

    def ghi(self):
        body = self.eol.join(b"\t".join(r) for r in self.rows) + self.eol
        V.ban_luu(self.p)
        with open(self.p, "wb") as w:
            w.write(body)


def tab_bytes(p):
    rows = [l.rstrip(b"\r").split(b"\t") for l in open(p, "rb").read().split(b"\n")]
    while rows and rows[-1] == [b""]:
        rows.pop()
    return rows


def kind_vltk_sang_jx1(vrow, vhdr, jhdr):
    out = []
    for name in jhdr:
        src = name if name in vhdr else KIND_ALIAS.get(name, name)
        c = vhdr.index(src) if src in vhdr else -1
        out.append(vrow[c] if 0 <= c < len(vrow) else b"")
    return out


def main():
    args = sys.argv[1:]
    kiem = "--kiem" in args
    ghi_de = "--ghi-de" in args
    n_mau = 5
    for a in args:
        if a.startswith("--mau="):
            n_mau = int(a.split("=", 1)[1])
    log("=== vhtd_npc_spr %s ===" % ("CHAY THU (--kiem)" if kiem else "THI CONG"))

    # 1. NpcResType theo id (client ve hinh; server phai trung)
    S = tab_bytes(NPCS_S)
    C = tab_bytes(NPCS_C)
    ci = C[0].index(b"NpcResType")
    ids = list(dict.fromkeys(UU_TIEN + ID_MOI))
    kieu_ids = collections.OrderedDict()
    lech = []
    for i in ids:
        rc = C[i + 1] if i + 1 < len(C) else None
        rs = S[i + 1] if i + 1 < len(S) else None
        if rc is None:
            lech.append((i, "client thieu hang"))
            continue
        if rs is None or rs[ci] != rc[ci]:
            lech.append((i, "server %r != client %r" % (rs[ci] if rs else None, rc[ci])))
        kieu_ids.setdefault(rc[ci], []).append(i)
    log("[NPC] %d id (25 uu tien + %d hang moi 2185..2642) -> %d kieu ; server/client lech %d %s" % (
        len(ids), len(ID_MOI), len(kieu_ids), len(lech), lech[:5]))

    # 2. bang JX1 + VLTK
    jk, jr, ji = Bang(KIND), Bang(NRES), Bang(NINFO)
    vk, vr, vi = Bang(V_KIND), Bang(V_NRES), Bang(V_INFO)
    acts = [r[0] for r in tab_bytes(NACT)[1:] if r and r[0]]
    log("[BANG] JX1 kind %d | normal_res %d (trung %s) | spr_info %d ; VLTK kind %d | normal_res %d | spr_info %d ; %d dong tac" % (
        len(jk.idx), len(jr.idx), {k.decode("latin-1"): v for k, v in jr.dup.items()} or "-", len(ji.idx), len(vk.idx), len(vr.idx), len(vi.idx), len(acts)))
    if jr.hdr != vr.hdr[:len(jr.hdr)]:
        raise SystemExit("header normal_res JX1 != VLTK: %r / %r" % (jr.hdr, vr.hdr))
    thieu_cot = [a for a in acts if a not in jr.hdr]
    if thieu_cot:
        raise SystemExit("normal_res thieu cot %s" % thieu_cot)
    kind_khong_res = [k for k in jk.idx if jk.cell(jk.row(k), b"CharacterType") == b"NormalNpc" and k not in jr.idx]
    log("  FindNpcRes: kind NormalNpc khong co hang normal_res: %d %s" % (len(kind_khong_res), [x.decode("latin-1") for x in kind_khong_res]))
    def gan(t, bang):
        tl = t.strip().lower()
        return [k.decode("latin-1") for k in bang.idx if k != t and k.strip().lower() == tl]

    # 3. index pak
    jpath, jorder = V2.doc_ini_paks(V.PKG_INI)
    jidx, jco = V2.quet_pak(os.path.join(V.CLIENT, jpath.strip("\\/")), jorder, bo=(V2.OUT2_NAME,))
    log("[JX1] %d khoa, %d pak mo duoc (bo %s), %d uid" % (len(jorder), len(jco), V2.OUT2_NAME, len(jidx)))
    vpath, vorder = V2.doc_ini_paks(V2.VLTK_INI) if os.path.exists(V2.VLTK_INI) else ("", [])
    le = sorted(x for x in os.listdir(V.VLTK_DATA) if x.lower().endswith(".pak") and x not in vorder)
    vidx, vco = V2.quet_pak(V.VLTK_DATA, vorder + le)
    log("[VLTK] %d pak mo duoc, %d uid" % (len(vco), len(vidx)))

    def duong_dan(kind_bang, kind_row, res_bang, res_row):
        """{path: [action]} theo dung KNpcResNode::Init."""
        out = collections.OrderedDict()
        rp = kind_bang.cell(kind_row, b"ResFilePath").decode("latin-1")
        for a in acts:
            cell = res_bang.cell(res_row, a)
            if cell:
                out.setdefault(V2.compose(rp, cell.decode("latin-1")), []).append(a)
        return out

    def o_dau(pth):
        u = P.name2id(pth)
        if u in jidx:
            return "JX1", jidx[u][0][0]
        if u in vidx:
            return "VLTK", vidx[u][0][0]
        return "KHONG", ""

    # 4. tung kieu
    ket = collections.OrderedDict()
    cells = []
    for t, idl in kieu_ids.items():
        r = dict(kieu=t, ids=idl, ghichu=[], sua=[], paths=collections.OrderedDict(), nguon="JX1")
        jkr, jrr, jir = jk.row(t), jr.row(t), ji.row(t)
        vkr, vrr, vir = vk.row(t), vr.row(t), vi.row(t)
        r["kind"], r["nres"], r["info"] = jkr is not None, jrr is not None, jir is not None
        r["ctype"] = jk.cell(jkr, b"CharacterType")
        r["respath"] = jk.cell(jkr, b"ResFilePath")
        for nm, bang in ((b"kind", jk), (b"normal_res", jr)):
            g = gan(t, bang)
            if g:
                r["ghichu"].append("%s co ten gan giong %s" % (nm.decode(), g))
        pj = duong_dan(jk, jkr, jr, jrr) if (jkr is not None and jrr is not None and r["ctype"] == b"NormalNpc") else collections.OrderedDict()
        pv = duong_dan(vk, vkr, vr, vrr) if (vkr is not None and vrr is not None and vk.cell(vkr, b"CharacterType") == b"NormalNpc") else collections.OrderedDict()
        tim_j = {p: o_dau(p) for p in pj}
        tim_v = {p: o_dau(p) for p in pv}
        co_j = sum(1 for x in tim_j.values() if x[0] != "KHONG")
        co_v = sum(1 for x in tim_v.values() if x[0] != "KHONG")
        # co lay phien ban VLTK khong? (chi khi tang so tep tim duoc, khong mat tep nao)
        dung_vltk = False
        if pv and (co_v > co_j or (jkr is None or jrr is None)) and co_v >= co_j and co_v > 0:
            mat = [p for p, x in tim_j.items() if x[0] != "KHONG" and (p not in tim_v or tim_v[p][0] == "KHONG")]
            if not mat:
                dung_vltk = True
        if dung_vltk:
            r["nguon"] = "VLTK"
            if jkr is None:
                r["sua"].append(("kind", "them", kind_vltk_sang_jx1(vkr, vk.hdr, jk.hdr)))
            elif jk.cell(jkr, b"ResFilePath") != vk.cell(vkr, b"ResFilePath"):
                nr = list(jkr) + [b""] * (len(jk.hdr) - len(jkr))
                nr[jk.hdr.index(b"ResFilePath")] = vk.cell(vkr, b"ResFilePath")
                r["sua"].append(("kind", "ResFilePath %r -> %r" % (jk.cell(jkr, b"ResFilePath"), vk.cell(vkr, b"ResFilePath")), nr))
            if jrr is None or jrr[:len(vrr)] != vrr[:len(jr.hdr)] and jrr[1:] != vrr[1:len(jrr)]:
                r["sua"].append(("normal_res", "hang theo VLTK", vrr[:len(jr.hdr)] + [b""] * max(0, len(jr.hdr) - len(vrr))))
            if vir is not None and (jir is None or jir[1:] != vir[1:len(jir)]):
                r["sua"].append(("spr_info", "hang theo VLTK", vir[:len(ji.hdr)] + [b""] * max(0, len(ji.hdr) - len(vir))))
            r["paths"] = pv
            r["tim"] = tim_v
            r["respath"] = vk.cell(vkr, b"ResFilePath")
        else:
            r["paths"] = pj
            r["tim"] = tim_j
        for pth, al in r["paths"].items():
            noi, pak = r["tim"][pth]
            cells.append(dict(kieu=t, path=pth, actions=al, noi=noi, pak=pak, nguon=r["nguon"]))
        r["n_j"] = sum(1 for x in r["tim"].values() if x[0] == "JX1")
        r["n_v"] = sum(1 for x in r["tim"].values() if x[0] == "VLTK")
        r["n_k"] = sum(1 for x in r["tim"].values() if x[0] == "KHONG")
        if jkr is None and vkr is None:
            r["ghichu"].append("kind THIEU o ca JX1 va VLTK")
        elif jkr is None and not dung_vltk:
            r["ghichu"].append("kind THIEU (VLTK co, nhung tep VLTK cung khong pak nao co)")
        if r["ctype"] == b"NormalNpc" and jrr is None and vrr is None:
            r["ghichu"].append("normal_res THIEU o ca JX1 va VLTK")
        if r["nres"] and not r["info"]:
            r["ghichu"].append("spr_info THIEU -> mac dinh 16,8,0")
        ket[t] = r

    # 5. bao
    log("[KIEU] chi in kieu uu tien hoac co van de: kieu | id | kind | ResFilePath | nres | info | nguon hang | o | JX1 co | them VLTK | thieu ca hai")
    n_loi = 0
    for t, r in ket.items():
        uu = any(i in UU_TIEN for i in r["ids"])
        loi = (not r["kind"]) or (r["ctype"] == b"NormalNpc" and not r["nres"]) or r["n_v"] or r["n_k"] or r["sua"] or r["ghichu"] or (r["ctype"] != b"NormalNpc")
        if loi:
            n_loi += 1
        if not (uu or loi):
            continue
        log("  %-17s ids %-20s kind=%-5s %-42s nres=%-5s info=%-5s %-4s | %2d o | %2d | %2d | %2d %s" % (
            t.decode("latin-1"), ",".join(map(str, r["ids"]))[:20], "co" if r["kind"] else "THIEU", r["respath"].decode("latin-1"),
            "co" if r["nres"] else "THIEU", "co" if r["info"] else "THIEU", r["nguon"], len(r["paths"]), r["n_j"], r["n_v"], r["n_k"],
            "<- UU TIEN" if uu else ""))
        for g in r["ghichu"]:
            log("      ghi chu: %s" % g)
        for bang, mota, row in r["sua"]:
            log("      SUA %s: %s" % (bang, mota if isinstance(mota, str) else mota))
        for pth, al in r["paths"].items():
            noi, pak = r["tim"][pth]
            if uu or noi != "JX1":
                log("      %-58s %-30s %s" % (pth.encode("latin-1").decode("gbk", "replace"), ",".join(a.decode() for a in al)[:30],
                                             ("JX1:" + pak) if noi == "JX1" else (("VLTK:%s -> %s" % (pak, V2.OUT2_NAME)) if noi == "VLTK" else "THIEU CA HAI")))
    paths_all = collections.OrderedDict()
    for c in cells:
        paths_all.setdefault(c["path"], c)
    n_j = sum(1 for c in paths_all.values() if c["noi"] == "JX1")
    tu_vltk = collections.OrderedDict((p, c) for p, c in paths_all.items() if c["noi"] == "VLTK")
    thieu = [p for p, c in paths_all.items() if c["noi"] == "KHONG"]
    log("  TONG: %d kieu kiem, %d kieu OK, %d kieu co van de ; %d ten tep: JX1 co %d, them tu VLTK %d, THIEU CA HAI %d" % (
        len(ket), len(ket) - n_loi, n_loi, len(paths_all), n_j, len(tu_vltk), len(thieu)))

    with open(DS_OUT, "wb") as w:
        w.write(b"kieu\tids\tkind\tCharacterType\tResFilePath\tnormal_res\tspr_info\tnguon_hang\tactions\tpath\tuid\ttrang_thai\tsua\tghi_chu\r\n")
        for t, r in ket.items():
            base = [t, ",".join(map(str, r["ids"])).encode(), b"co" if r["kind"] else b"THIEU", r["ctype"], r["respath"],
                    b"co" if r["nres"] else b"THIEU", b"co" if r["info"] else b"THIEU", r["nguon"].encode()]
            sua = b"; ".join(("%s:%s" % (b, m if isinstance(m, str) else "")).encode("latin-1", "replace") for b, m, _ in r["sua"])
            gc = b"; ".join(g.encode("latin-1", "replace") for g in r["ghichu"])
            if not r["paths"]:
                w.write(b"\t".join(base + [b"", b"", b"", b"(khong co o)", sua, gc]) + b"\r\n")
            for pth, al in r["paths"].items():
                noi, pak = r["tim"][pth]
                tt = ("JX1:" + pak) if noi == "JX1" else (("VLTK:" + pak) if noi == "VLTK" else "THIEU CA HAI")
                w.write(b"\t".join(base + [b",".join(al), pth.encode("latin-1"), ("%08X" % P.name2id(pth)).encode(), tt.encode(), sua, gc]) + b"\r\n")
    log("  danh sach: %s" % DS_OUT)

    # 6. sua bang
    sua_bang = [(t, s) for t, r in ket.items() for s in r["sua"]]
    if sua_bang:
        log("[BANG] %d thay doi bang (theo VLTK, chi kieu tang so tep tim duoc):" % len(sua_bang))
        for t, (bang, mota, row) in sua_bang:
            b = {"kind": jk, "normal_res": jr, "spr_info": ji}[bang]
            b.dat_row(t, row, mota if isinstance(mota, str) else "")
        for b in (jk, jr, ji):
            for td in b.thay_doi:
                log("   %s: %s" % (os.path.basename(b.p), td))
            if b.thay_doi:
                if kiem:
                    log("   [KIEM] khong ghi %s" % b.p)
                else:
                    b.ghi()
                    log("   DA GHI %s (%d dong)" % (b.p, len(b.rows)))
    else:
        log("[BANG] khong can sua bang")

    # 7. dong pak
    md5 = None
    if tu_vltk:
        items = [(pth, c["pak"], P.name2id(pth)) for pth, c in tu_vltk.items()]
        plan, thieu2, warn = V.lap_kehoach(items)
        log("[PAK] ke hoach %d muc (thieu %d, canh bao %s) -> %s" % (len(plan), len(thieu2), dict(warn) or "-", V.OUT_PAK))
        md5 = V.buoc_pak(plan, kiem, ghi_de, n_mau)
    else:
        log("[PAK] khong co tep nao can them tu VLTK -> khong tao %s" % V2.OUT2_NAME)
    if os.path.exists(V.OUT_PAK) or (kiem and tu_vltk):
        V.buoc_ini(kiem)
    log("=== TOM TAT: %d kieu / van de %d / sua bang %d / tep them tu VLTK %d / thieu ca hai %d ; md5 %s ===" % (
        len(ket), n_loi, len(sua_bang), len(tu_vltk), len(thieu), md5 or "-"))


if __name__ == "__main__":
    main()
