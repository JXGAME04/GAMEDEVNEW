# -*- coding: utf-8 -*-
r"""vhtd_npc_map.py [VHTD 02/09] - noi DU LIEU NPC + BAN DO cho 2 phai Vu Hon (id 11, map 1042-1046 "Vu Muc mon phai")
va Tieu Dao (id 12, map 1057) vao cay chay that E:\...\bin\server + bin\client, theo dung cach da lam cho Hoa Son
(hs_data1.py / hs_map_pak.py). Chi doc/ghi latin-1 + nhi phan (giu byte TCVN3/GBK). Idempotent, co --kiem.

  python vhtd_npc_map.py          ap dung (moi tep sua co ban luu <tep>.truoc_vhtd_0209 canh tep, chi tao 1 lan)
  python vhtd_npc_map.py --kiem   chi kiem tra / in ke hoach, KHONG ghi gi

Viec lam:
  1. npcs.txt (server + client): noi 458 hang VLTK (chi so 0-based 2186..2643 = id engine 2185..2642, id = chi so - 1
     vi KNpc::Load doc GetString(id + 2)) anh xa theo TEN COT (VLTK 103 cot -> JX1 87 cot). NpcName=id.txt +458.
  2. Kieu tai nguyen NPC (client settings\NpcRes): kieu NpcResType moi chua co trong npc_res_kind_file_name.txt ->
     lay hang tu VLTK (人物类型 / 普通npc资源 / 普通npc资源信息, rut tu slistcache.pak), CHI noi khi moi .spr da co
     trong pak client JX1 (tra jx1_client_pak_index.tsv, loc theo package.ini client); con lai chi BAO.
  3. Ban do: Linux map_publish\wumumenpai 286 + xiaoyao 385 tep _region_c.dat + 2 .wor (byte GIONG HET 673/673 muc
     \map_publish\... trong updatejx15/16 cua client JX1) dong lai duoi ten \maps\map_publish\<ten>\... vi JX1 luon ghep
     "\maps\" + gia tri MapList (KSubWorld.cpp:1884, KScenePlaceC.cpp:350):
       - server Pak\maps_vuhon_tieudao.pak: moi region 2 ten *_region_c.dat (nguyen) + *_region_s.dat (bi danh, vi
         KRegion::LoadObject server CHI mo _Region_S.dat; vat can region_c ma 0/17/33/49/65/81 dung dinh dang server);
       - client data\maps_vuhon_tieudao.pak: *_region_c.dat + .wor; them vao package.ini client.
     MapList.ini (server + client) khai <id>=map_publish\<ten>. WorldSet_GameServer.ini World927..932, package.ini server.
     KHONG dung duoc cach "<id>=..\map_publish\<ten>" (ban giao muc 6 goi y): RemoveTwoPointPath (KFilePath.cpp:37-59)
     giu lai dau '\' o cho noi (lpszAfter = lpszTarget + 3) -> "maps\..\map_publish\x" thanh "\map_publish\x", roi
     KPakList::FindElemFile (KPakList.cpp:103-108) them '\' dau nua -> bam "\\map_publish\x" != uid VLTK. Da mo phong.
  4. FactionInfo.ini + 门派设定.ini (server + client): [11] wuhun (火/正派) va [12] xiaoyao (土/中立); byte GBK lay tu
     dong chu thich muc [11]/[12] cua faction_settings.ini VLTK, kiem cheo voi mang szSeries/szCamp KFaction.cpp.
"""
import io, os, re, sys, shutil, struct, hashlib
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
from vn_to_octal import unicode_to_tcvn3_bytes
import pakdump as P

KIEM = "--kiem" in sys.argv
MARK = "[VHTD 02/09]"
BK = ".truoc_vhtd_0209"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
LIN_MAPS = r"D:\ServerLinux\server1\maps\map_publish"
VLTK = r"D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw"
PHAN = r"D:\GAMEDEVNEW\ReverseTools\phai3\phantich"
KFACTION = r"D:\GAMEDEVNEW\Sources\Core\Src\KFaction.cpp"
SCR = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(SCR, "out")
BS = chr(92)
PAK_NAME = "maps_vuhon_tieudao.pak"
MAP_DIRS = {"wumumenpai": (1042, 1043, 1044, 1045, 1046), "xiaoyao": (1057,)}
MAP_IDS = [1042, 1043, 1044, 1045, 1046, 1057]
MAP_COMMENT = {1042: "Vu Hon - Vu Muc mon phai (map_publish/wumumenpai)", 1043: "Vu Hon - Luyen Vo Truong 1 (wumumenpai)",
               1044: "Vu Hon - Luyen Vo Truong 2 (wumumenpai)", 1045: "Vu Hon - Luyen Vo Truong 3 (wumumenpai)",
               1046: "Vu Hon - Luyen Vo Truong 4 (wumumenpai)", 1057: "Tieu Dao phai (map_publish/xiaoyao)"}
NEW_ROW0 = 2186          # chi so 0-based dong VLTK dau tien can noi (id engine 2185); JX1 hien co header + id 0..2184
BAO = []                 # bao cao cuoi (gia dinh / thieu / canh bao)


def V(s): return unicode_to_tcvn3_bytes(s).decode("latin-1")
def G(s): return s.encode("gbk").decode("latin-1")
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def nl_of(d): return "\r\n" if "\r\n" in d else "\n"
def bao(s):
    if s not in BAO:
        BAO.append(s)
    print("   BAO:", s)


def wr(p, d):
    if KIEM:
        print("  (kiem) se ghi", p); return
    if os.path.exists(p) and not os.path.exists(p + BK):
        shutil.copyfile(p, p + BK)
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    print("  ghi", p)


def wrb(p, b, backup=True):
    if KIEM:
        print("  (kiem) se ghi (nhi phan %d byte)" % len(b), p); return
    if backup and os.path.exists(p) and not os.path.exists(p + BK):
        shutil.copyfile(p, p + BK)
    open(p, "wb").write(b)
    print("  ghi", p)


def tab_rows(d):
    """tach bang tab: tra (rows, nl); bo dong trong cuoi"""
    nl = nl_of(d)
    lines = d.split(nl)
    while lines and lines[-1].strip() == "":
        lines.pop()
    return [l.split("\t") for l in lines], nl


# ============================================================ nguon VLTK npcs.txt
_VL = None
def vltk_npcs():
    global _VL
    if _VL is None:
        d = rd(os.path.join(VLTK, "settings__npcs.txt"))
        assert nl_of(d) == "\r\n"
        lines = d.split("\r\n")
        while lines and lines[-1].strip() == "":
            lines.pop()
        hdr = lines[0].split("\t")
        assert len(hdr) == 103, len(hdr)
        assert len(lines) == 2644, "VLTK npcs.txt: %d dong (can 2644 = header + id 0..2642)" % len(lines)
        rows = [l.split("\t") for l in lines]
        for i in range(1, len(rows)):
            assert len(rows[i]) == 103, "VLTK npcs dong %d co %d cot" % (i, len(rows[i]))
        _VL = (hdr, rows)
    return _VL


def new_npc_rows():
    """[(id_engine, row_vltk)] cac hang can noi"""
    hdr, rows = vltk_npcs()
    out = []
    for i in range(NEW_ROW0, len(rows)):
        assert rows[i][0].strip() not in ("", "0"), "VLTK dong %d ten rong" % i
        out.append((i - 1, rows[i]))
    assert len(out) == 458, len(out)
    return out


# ============================================================ 1. npcs.txt (server + client)
def npcs_txt():
    hdr, rows = vltk_npcs()
    new = new_npc_rows()
    for p in (os.path.join(SRV, "settings", "npcs.txt"), os.path.join(CLI, "settings", "npcs.txt")):
        d = rd(p); nl = nl_of(d)
        jr, _ = tab_rows(d)
        hj = jr[0]
        thieu = [h for h in hj if h not in hdr]
        assert not thieu, "npcs: cot JX1 khong co o VLTK: %r" % thieu
        idx = [hdr.index(h) for h in hj]               # anh xa theo TEN cot
        exp = ["\t".join(r[k] for k in idx) for _, r in new]
        # JX1 phai dang la header + id 0..2184 (2186 dong) hoac da noi roi
        if len(jr) == NEW_ROW0 + len(exp):
            cur = ["\t".join(r) for r in jr[NEW_ROW0:]]
            if cur == exp:
                print("  npcs.txt da noi (%d dong), khop 100%%:" % len(exp), p)
            else:
                nd = sum(1 for a, b in zip(cur, exp) if a != b)
                bao("npcs.txt %s da co %d dong cuoi nhung %d dong KHAC ban VLTK -> GIU NGUYEN, khong ghi" % (p, len(exp), nd))
            kiem_npcs(jr, hj, p)
            continue
        assert len(jr) == NEW_ROW0, "npcs.txt %s co %d dong (can %d = header + id 0..2184) - dung tay" % (p, len(jr), NEW_ROW0)
        body = [("\t".join(r)) for r in jr] + exp
        kiem_npcs([l.split("\t") for l in body], hj, p)
        print("  npcs.txt %s: noi %d dong (id %d..%d), %d cot" % (p, len(exp), new[0][0], new[-1][0], len(hj)))
        wr(p, nl.join(body) + nl)


def kiem_npcs(jr, hj, p):
    bad = [i for i, r in enumerate(jr) if len(r) != len(hj)]
    cu = [i for i in bad if i < NEW_ROW0]
    if cu:
        # loi CO SAN cua JX1 (vd chi so 2185 'nangongjue' 103 cot) - ngoai pham vi, KTabFile bo qua cot du -> chi bao
        bao("npcs.txt %s: %d dong CU co so cot != 87 (co san truoc khi noi, khong sua): chi so %r" % (p, len(cu), cu[:10]))
    bad = [i for i in bad if i >= NEW_ROW0]
    assert not bad, "%s: dong MOI so cot != header: %r" % (p, bad[:10])
    def ten(idx0): return jr[idx0][0] if idx0 < len(jr) else None
    ok1 = ten(2469) == V("Nhạc Lôi"); ok2 = ten(2609) == V("Văn Bán Sơn"); ok3 = ten(2470) == V("Ngưu Thông")
    print("  kiem %s: %d dong; chi so 2469 (id engine 2468) = Nhac Loi: %s; 2609 (id 2608) = Van Ban Son: %s; 2470 (id 2469) = Nguu Thong: %s"
          % (os.path.basename(os.path.dirname(os.path.dirname(p))), len(jr), ok1, ok2, ok3))
    assert ok1 and ok2 and ok3


# ============================================================ 1b. NpcName=id.txt (server + client)
def npcname_id():
    new = new_npc_rows()
    for p in (os.path.join(SRV, "settings", "NpcName=id.txt"), os.path.join(CLI, "settings", "NpcName=id.txt")):
        d = rd(p); nl = nl_of(d)
        lines = d.split(nl)
        while lines and lines[-1].strip() == "":
            lines.pop()
        assert lines[0].split("\t")[0].strip() == "Id", lines[0][:20]
        have = set(l.split("\t")[0].strip() for l in lines[1:])
        add = ["%d\t%s" % (i, r[0]) for i, r in new if str(i) not in have]
        if not add:
            print("  NpcName=id.txt da du:", p); continue
        print("  NpcName=id.txt %s: +%d dong" % (p, len(add)))
        wr(p, nl.join(lines + add) + nl)


# ============================================================ 2. kieu tai nguyen NPC (client NpcRes)
def pak_index_client():
    """uid cac pak client DANG khai trong package.ini client"""
    pk = rd(os.path.join(CLI, "package.ini"))
    paks = set(x.strip().lower() for x in re.findall(r"^\d+=(.*)$", pk, re.M))
    uids = set(); dem = 0
    for l in io.open(os.path.join(PHAN, "jx1_client_pak_index.tsv"), "r", encoding="utf-8", errors="replace"):
        c = l.rstrip("\n").split("\t")
        if len(c) >= 2 and c[0].strip().lower() in paks:
            try:
                uids.add(int(c[1], 16)); dem += 1
            except ValueError:
                pass
    return uids, paks


def npcres_client():
    C = os.path.join(CLI, "settings", "NpcRes")
    pk = os.path.join(C, "npc_res_kind_file_name.txt"); pr = os.path.join(C, "npc_normal_res_file.txt"); pi = os.path.join(C, "npc_normal_spr_info_file.txt")
    dk = rd(pk); dr = rd(pr); di = rd(pi)
    K, nlk = tab_rows(dk); R, nlr = tab_rows(dr); I, nli = tab_rows(di)
    hk, hr, hi = K[0], R[0], I[0]
    assert len(hk) == 19 and len(hr) == 15 and len(hi) == 15, (len(hk), len(hr), len(hi))
    KD = dict((r[0].strip(), r) for r in K[1:]); RD = dict((r[0].strip(), r) for r in R[1:]); ID = dict((r[0].strip(), r) for r in I[1:])
    VR_ = os.path.join(VLTK, "npcres")
    vk, _ = tab_rows(rd(os.path.join(VR_, "人物类型.txt")))        # rut tu slistcache.pak VLTK (uid 64D8690E), ten tep Unicode
    vr, _ = tab_rows(rd(os.path.join(VR_, "普通npc资源.txt")))
    vi, _ = tab_rows(rd(os.path.join(VR_, "普通npc资源信息.txt")))
    hvk, hvr, hvi = vk[0], vr[0], vi[0]
    assert hvr == hr and hvi == hi, "header 普通npc资源 khac JX1"
    alias = {"RightHand": "RightHead"}          # JX1 'RightHand' <-> VLTK go nham 'RightHead'
    kidx = []
    for h in hk:
        hv = alias.get(h, h)
        assert hv in hvk, "cot kind JX1 %s khong co o VLTK" % h
        kidx.append(hvk.index(hv))
    dropped = [i for i, h in enumerate(hvk) if h not in hk and h not in alias.values()]   # *Special
    VK = dict((r[0].strip(), r) for r in vk[1:]); VRD = dict((r[0].strip(), r) for r in vr[1:]); VID = dict((r[0].strip(), r) for r in vi[1:])
    uids, paks = pak_index_client()
    print("  pak client dang khai: %d, uid tra duoc: %d" % (len(paks), len(uids)))
    hdr, rows = vltk_npcs(); ci = hdr.index("NpcResType")
    kinds = []
    for _, r in new_npc_rows():
        k = r[ci].strip()
        if k and k not in kinds:
            kinds.append(k)
    print("  NPC moi dung %d kieu NpcResType" % len(kinds))
    # kieu da co o JX1: kiem hang res/info va lech duong dan
    for k in kinds:
        if k in KD:
            if KD[k][1].strip() == "NormalNpc" and (k not in RD or k not in ID):
                bao("kieu %s co trong npc_res_kind_file_name nhung THIEU hang o %s (khong tu them)" % (k, "npc_normal_res_file" if k not in RD else "npc_normal_spr_info_file"))
            if k in VK and (KD[k][1].strip() != VK[k][1].strip() or KD[k][2].strip().lower() != VK[k][2].strip().lower()):
                bao("kieu %s: JX1 (%s, %s) KHAC VLTK (%s, %s) -> giu ban JX1" % (k, KD[k][1].strip(), KD[k][2].strip(), VK[k][1].strip(), VK[k][2].strip()))
    missing = [k for k in kinds if k not in KD]
    print("  kieu chua co trong JX1: %d" % len(missing))
    addK, addR, addI, skip = [], [], [], []
    for k in missing:
        if k not in VK:
            skip.append((k, "KHONG co trong 人物类型.txt VLTK")); continue
        r = VK[k]
        if r[1].strip() != "NormalNpc":
            skip.append((k, "CharacterType=%s (khong phai NormalNpc)" % r[1].strip())); continue
        if any(len(r) > i and r[i].strip() for i in dropped):
            skip.append((k, "co cot *Special khac rong")); continue
        if k not in VRD or k not in VID:
            skip.append((k, "thieu hang 普通npc资源%s" % ("" if k not in VRD else "信息"))); continue
        path = r[2].strip()
        rr = VRD[k] + [""] * (15 - len(VRD[k])); ri = VID[k] + [""] * (15 - len(VID[k]))
        sprs = sorted(set(s.strip() for s in rr[1:15] if s.strip()))
        if not sprs:
            skip.append((k, "hang 普通npc资源 khong co .spr nao")); continue
        lack = [s for s in sprs if P.name2id(BS + path + BS + s) not in uids]
        if lack:
            skip.append((k, "thieu %d/%d .spr trong pak client JX1: %s (%s)" % (len(lack), len(sprs), ", ".join(lack), path))); continue
        rowk = [r[j] if j < len(r) else "" for j in kidx]
        rowk = [x.strip() if n < 3 else "" for n, x in enumerate(rowk)]
        addK.append(rowk); addR.append([x.strip() for x in rr[:15]]); addI.append([x.strip() for x in ri[:15]])
    for k, why in skip:
        bao("kieu NPC %s KHONG noi: %s" % (k, why))
    if not addK:
        print("  khong co kieu nao can/duoc noi"); return
    names = [r[0] for r in addK]
    print("  noi %d kieu (du .spr): %s" % (len(names), ", ".join(names)))
    K2 = ["\t".join(r) for r in K] + ["\t".join(r) for r in addK]
    R2 = ["\t".join(r) for r in R] + ["\t".join(r) for r in addR if r[0] not in RD]
    I2 = ["\t".join(r) for r in I] + ["\t".join(r) for r in addI if r[0] not in ID]
    coR = sum(1 for r in addR if r[0] in RD); coI = sum(1 for r in addI if r[0] in ID)
    print("  hang 普通npc资源/信息 da co san o JX1: %d/%d va %d/%d (chi them hang thieu)" % (coR, len(addR), coI, len(addI)))
    for p, txt, nl, old in ((pk, K2, nlk, dk), (pr, R2, nlr, dr), (pi, I2, nli, di)):
        new = nl.join(txt) + nl
        if new == old:
            print("  khong doi:", p)
        else:
            wr(p, new)


# ============================================================ 3a. pak ban do server
def collect_map_files():
    """tra (items_server, items_client): [(ten trong pak, bytes)] - ten \\maps\\map_publish\\<nm>\\..."""
    srv, cli = [], []
    for nm in MAP_DIRS:
        wor = os.path.join(LIN_MAPS, nm + ".wor")
        w = (BS + "maps" + BS + "map_publish" + BS + nm + ".wor", open(wor, "rb").read())
        srv.append(w); cli.append(w)
        base = os.path.join(LIN_MAPS, nm)
        for v in sorted(os.listdir(base)):
            assert re.match(r"^v_\d{3}$", v), v
            for f in sorted(os.listdir(os.path.join(base, v))):
                m = re.match(r"^(\d{3})_region_c\.dat$", f, re.I)
                assert m, "tep la trong map Linux: " + f
                d = open(os.path.join(base, v, f), "rb").read()
                n = struct.unpack_from("<I", d, 0)[0]
                assert n == 6, "region %s/%s/%s khong phai combin 6 muc" % (nm, v, f)
                rel = BS + "maps" + BS + "map_publish" + BS + nm + BS + v + BS + m.group(1)
                srv.append((rel + "_region_c.dat", d))       # ten nguyen (ProcLoadPathGrid lui _Region_C)
                srv.append((rel + "_region_s.dat", d))       # bi danh: KRegion::LoadObject server chi mo _Region_S.dat
                cli.append((rel + "_region_c.dat", d))
    return srv, cli


def build_pak(items):
    got = {}
    for n, d in items:
        u = P.name2id(n)
        assert u not in got, "trung uid: %s ~ %s" % (n, got[u][0])
        got[u] = (n, d)
    uids = sorted(got); n = len(uids)
    hdr = 16; isz = 16 * n; off = hdr + isz
    index = bytearray(); data = bytearray()
    for u in uids:
        d = got[u][1]
        index += struct.pack("<IIiI", u, off, len(d), len(d))     # flag 0 (RAW), csize = size
        data += d; off += len(d)
    return struct.pack("<4sIII", b"PACK", n, hdr, hdr + isz) + bytes(index) + bytes(data), got


def verify_pak(path, got):
    f, es = P.entries(path)
    ok = 0; cnt = {"_region_c.dat": 0, "_region_s.dat": 0, ".wor": 0}
    for e in es:
        f.seek(e[1]); raw = f.read(e[2])
        if e[0] in got and raw == got[e[0]][1]:
            ok += 1
            for suf in cnt:
                if got[e[0]][0].endswith(suf):
                    cnt[suf] += 1
    f.close()
    return len(es), ok, cnt


def kiem_client_vltk(items_cli):
    """client JX1 da co san 673 tep nay duoi ten VLTK \\map_publish\\... (khong co \\maps\\) trong updatejx15/16 - chi bao"""
    uids, _ = pak_index_client()
    co = sum(1 for n, _ in items_cli if P.name2id(n.replace(BS + "maps" + BS, BS, 1)) in uids)
    print("  client JX1 (updatejx15/16) co san %d/%d tep nay duoi ten VLTK \\map_publish\\... (khong dung duoc vi JX1 ghep \\maps\\)" % (co, len(items_cli)))


def dong_pak(items, dst, nhan, want):
    blob, got = build_pak(items)
    if not os.path.isdir(OUT):
        os.makedirs(OUT)
    tmp = os.path.join(OUT, nhan + "_" + PAK_NAME)
    open(tmp, "wb").write(blob)
    n, ok, cnt = verify_pak(tmp, got)
    print("  %s: %d muc, %d byte; doc lai khop %d/%d; region_c %d, region_s %d, wor %d" % (nhan, len(got), len(blob), ok, n, cnt["_region_c.dat"], cnt["_region_s.dat"], cnt[".wor"]))
    assert n == ok == len(got) and cnt == want, cnt
    if os.path.exists(dst) and open(dst, "rb").read() == blob:
        print("  pak %s da co va giong het:" % nhan, dst); return got
    wrb(dst, blob, backup=False)          # tep cua chinh tool (khong co du lieu cu cua chu) -> khong tao .truoc
    if not KIEM:
        n, ok, cnt = verify_pak(dst, got)
        print("  doc lai %s: %d muc, khop %d/%d" % (dst, n, ok, len(got)))
        assert n == ok == len(got)
    return got


def map_pak():
    srv, cli = collect_map_files()
    nreg = sum(1 for n, _ in cli if n.endswith("_region_c.dat"))
    print("  thu thap: %d region (wumumenpai %d + xiaoyao %d) + %d .wor" % (
        nreg, sum(1 for n, _ in cli if "wumumenpai" in n and n.endswith("_region_c.dat")),
        sum(1 for n, _ in cli if "xiaoyao" in n and n.endswith("_region_c.dat")), sum(1 for n, _ in cli if n.endswith(".wor"))))
    assert nreg == 286 + 385
    kiem_client_vltk(cli)
    dong_pak(srv, os.path.join(SRV, "Pak", PAK_NAME), "server", {"_region_c.dat": 671, "_region_s.dat": 671, ".wor": 2})
    dong_pak(cli, os.path.join(CLI, "data", PAK_NAME), "client", {"_region_c.dat": 671, "_region_s.dat": 0, ".wor": 2})
    # package.ini client
    p = os.path.join(CLI, "package.ini")
    d = rd(p); nl = nl_of(d)
    if PAK_NAME.lower() in d.lower():
        print("  package.ini client da co", PAK_NAME)
    else:
        keys = [int(x) for x in re.findall(r"^(\d+)=", d, re.M)]
        assert len(keys) + 1 < 64, "KPakList MAX_PAK 64"
        wr(p, d.rstrip("\r\n") + nl + "%d=%s" % (max(keys) + 1, PAK_NAME) + nl)


# ============================================================ 3b. WorldSet_GameServer.ini + package.ini
def worldset():
    p = os.path.join(SRV, "Maps", "WorldSet_GameServer.ini")
    d = rd(p); nl = nl_of(d)
    ws = re.findall(r"^World(\d+)=(\d+)", d, re.M)
    have = set(int(b) for a, b in ws); nxt = max(int(a) for a, b in ws) + 1
    m = re.search(r"Count=(\d+)", d); cnt = int(m.group(1))
    assert cnt == nxt, "WorldSet: Count=%d nhung World cuoi = %d" % (cnt, nxt - 1)
    add = []
    for mid in MAP_IDS:
        if mid in have:
            print("  WorldSet da co", mid); continue
        add.append("World%03d=%d --- %s %s" % (nxt, mid, MAP_COMMENT[mid], MARK)); nxt += 1
    if not add:
        return
    d = d.replace("Count=%d" % cnt, "Count=%d" % nxt, 1)
    d = d.rstrip("\r\n") + nl + nl.join(add) + nl
    print("  WorldSet: +%d world, Count %d -> %d" % (len(add), cnt, nxt))
    wr(p, d)


def package_ini():
    p = os.path.join(SRV, "package.ini")
    d = rd(p); nl = nl_of(d)
    if PAK_NAME.lower() in d.lower():
        print("  package.ini da co", PAK_NAME); return
    keys = [int(x) for x in re.findall(r"^(\d+)=", d, re.M)]
    d = d.rstrip("\r\n") + nl + "%d=%s" % (max(keys) + 1, PAK_NAME) + nl
    wr(p, d)


# ============================================================ 3c. MapList.ini (server + client)
def vltk_maplist_block(mid):
    v = rd(os.path.join(VLTK, "settings__maplist.ini")).replace("\r\n", "\n").split("\n")
    blk = [l for l in v if re.match(r"^%d(=|_)" % mid, l)]
    assert blk and blk[0].startswith("%d=" % mid), "VLTK maplist khong co %d" % mid
    out = []
    for l in blk:
        k, val = l.split("=", 1)
        val = val.rstrip()
        if k == str(mid):
            m = re.match(r"^\\map_publish\\(\w+)$", val)
            assert m, "duong dan la: %r" % val
            val = "map_publish" + BS + m.group(1)      # JX1 ghep \maps\ + val -> \maps\map_publish\<nm> = ten trong 2 pak moi
        elif k.endswith("_NewWorldScript") and "wuhun2020" in val.lower():
            bao("%s: VLTK tro %s (JX1 khong co) -> dung \\script\\maps\\newworldscript.lua nhu 987/1042" % (k, val))
            val = BS + "script" + BS + "maps" + BS + "newworldscript.lua"
        out.append(k + "=" + val)
    return out


def maplist():
    for p in (os.path.join(SRV, "settings", "MapList.ini"), os.path.join(CLI, "settings", "MapList.ini")):
        d = rd(p); nl = nl_of(d)
        lines = d.split(nl)
        blocks = []; sua = 0
        for mid in MAP_IDS:
            blk = vltk_maplist_block(mid)
            if re.search(r"^%d=" % mid, d, re.M):
                # da co: kiem/sua tung khoa theo ban chuan (vd ban ghi truoc dung '..\map_publish' - da chung minh sai)
                for kv in blk:
                    k, val = kv.split("=", 1)
                    hit = [i for i, l in enumerate(lines) if l.startswith(k + "=")]
                    assert len(hit) == 1, "MapList %s: khoa %s xuat hien %d lan" % (p, k, len(hit))
                    if lines[hit[0]] != kv:
                        print("  MapList sua %s: %r -> %r" % (p, lines[hit[0]], kv)); lines[hit[0]] = kv; sua += 1
                continue
            blocks.append(blk)
        if sua:
            wr(p, nl.join(lines))
        if not blocks:
            if not sua:
                print("  MapList da du 6 ban do:", p)
            continue
        di = [i for i, l in enumerate(lines) if l.startswith("Default_NewWorldScript=")]
        assert len(di) == 1, "MapList %s: Default_NewWorldScript %d lan" % (p, len(di))
        j = di[0]
        while j > 0 and (lines[j - 1].strip() == "" or lines[j - 1].startswith(";")):
            j -= 1                                   # chen sau khoi ban do cuoi, truoc chu thich/Default
        ins = []
        for b in blocks:
            ins += [""] + b
        lines = lines[:j] + ins + lines[j:]
        print("  MapList %s: +%d ban do (%s)" % (p, len(blocks), ", ".join(b[0].split("=")[0] for b in blocks)))
        wr(p, nl.join(lines))


# ============================================================ 4. FactionInfo.ini + 门派设定.ini (4 tep)
def faction_bytes():
    src = rd(KFACTION)
    ser = re.search(r'szSeries\[series_num\]\[16\]\s*=\s*\{([^}]*)\}', src).group(1)
    cam = re.search(r'szCamp\[camp_num\]\[16\]\s*=\s*\{([^}]*)\}', src).group(1)
    S = [x.strip().strip('"') for x in ser.split(",")]
    C = [x.strip().strip('"') for x in cam.split(",")]
    assert len(S) == 5 and len(C) == 7
    fs = rd(os.path.join(VLTK, "settings__faction_settings.ini")).replace("\r\n", "\n").split("\n")
    cm = {}
    for i, l in enumerate(fs):
        if l.strip() in ("[10]", "[11]", "[12]"):
            parts = fs[i - 1].lstrip(";").strip().split(" ")
            assert len(parts) == 3, parts
            cm[int(l.strip()[1:-1])] = parts                # [ten GBK, he GBK, camp GBK]
    # kiem cheo voi mang engine: Hoa Son = Thuy/Trung lap, Vu Hon = Hoa/Chinh phai, Tieu Dao = Tho/Trung lap
    assert cm[10][1] == S[2] and cm[10][2] == C[3], "faction_settings [10] khong khop szSeries[2]/szCamp[3]"
    assert cm[11][1] == S[3] and cm[11][2] == C[1], "faction_settings [11] khong khop szSeries[3]/szCamp[1]"
    assert cm[12][1] == S[4] and cm[12][2] == C[3], "faction_settings [12] khong khop szSeries[4]/szCamp[3]"
    # ValueName theo faction_settings.ini VLTK
    txt = "\n".join(fs)
    assert re.search(r"\[11\]\nName=wuhun\n", txt) and re.search(r"\[12\]\nName=xiaoyao\n", txt)
    return cm


def faction_ini():
    cm = faction_bytes()
    spec = {11: (cm[11], "wuhun", V("Vũ Hồn")), 12: (cm[12], "xiaoyao", V("Tiêu Dao"))}
    for root in (SRV, CLI):
        for fn in ("FactionInfo.ini", G("门派设定.ini")):
            p = os.path.join(root, "settings", "faction", fn)
            if not os.path.exists(p):
                bao("khong co tep %s (bo qua)" % p); continue
            d = rd(p); nl = nl_of(d)
            m = re.search(r"\[10\]\r?\nName=([^\r\n]*)\r?\nSeries=([^\r\n]*)\r?\nCamp=([^\r\n]*)", d)
            assert m, "%s: khong thay muc [10]" % p
            assert m.group(1) == cm[10][0] and m.group(2) == cm[10][1] and m.group(3) == cm[10][2], "%s: byte [10] khac VLTK" % p
            add = []
            for fid in (11, 12):
                if re.search(r"^\[%d\]" % fid, d, re.M):
                    print("  da co [%d]:" % fid, p); continue
                (name, ser, camp), vn, show = spec[fid]
                add += ["", "[%d]" % fid, "Name=" + name, "Series=" + ser, "Camp=" + camp, "ValueName=" + vn, "ShowName=" + show]
            if not add:
                continue
            d = d.rstrip("\r\n") + nl + nl.join(add + [""])
            wr(p, d)


# ============================================================ 4b. mo phong duong dan engine -> uid phai co trong pak
def _rm2(s):
    """RemoveTwoPointPath (KFilePath.cpp:37-59): giu dau '\\' o cho noi (lpszAfter = lpszTarget + 3)"""
    while True:
        i = s.find(BS + ".." + BS)
        if i < 0:
            return s
        after = i + 3; t = i
        while t > 0:
            t -= 1
            if s[t] == BS:
                break
        s = s[:t] + s[after:]


def _rm1(s):
    while True:
        i = s.find(BS + "." + BS)
        if i < 0:
            return s
        s = s[:i] + s[i + 2:]


def _lower(s):
    return "".join(chr(ord(c) + 32) if "A" <= c <= "Z" else c for c in s)     # g_StrLower/FileNameToId: chi A-Z


def _set_file_path(p):
    cur = p[1:] if p[:1] in (BS, "/") else p
    if cur and cur[-1] not in (BS, "/"):
        cur += BS
    return _rm1(_rm2(cur))


def _pak_uid(cur, name):
    """KPakList::FindElemFile: '\\' + g_GetPackPath(cur, name)"""
    s = name[1:] if name[:1] in (BS, "/") else cur + name
    return P.name2id(BS + _lower(_rm1(_rm2(s))))


def kiem_duongdan():
    """mo phong KSubWorld/KRegion/KScenePlaceC ghep duong dan -> uid phai co trong pak server/client va MapList dung gia tri"""
    f, es = P.entries(os.path.join(SRV, "Pak", PAK_NAME)); spak = set(e[0] for e in es); f.close()
    f, es = P.entries(os.path.join(CLI, "data", PAK_NAME)); cpak = set(e[0] for e in es); f.close()
    ml = rd(os.path.join(SRV, "settings", "MapList.ini"))
    tot = ok = 0
    for nm, ids in MAP_DIRS.items():
        for mid in ids:
            m = re.search(r"^%d=(.*)$" % mid, ml, re.M)
            assert m, "MapList server chua co %d" % mid
            mp = m.group(1).strip()
            assert mp == "map_publish" + BS + nm, "MapList %d=%r" % (mid, mp)
        # server .wor: g_SetFilePath("\\maps"); IniFile.Load("%s.wor" % szPathName)
        cur = _set_file_path(BS + "maps"); tot += 1; ok += _pak_uid(cur, mp + ".wor") in spak
        # client .wor: KScenePlaceC "\\maps\\%s.wor"
        cur = _set_file_path(BS + "settings"); tot += 1; ok += _pak_uid(cur, BS + "maps" + BS + mp + ".wor") in cpak
        base = os.path.join(LIN_MAPS, nm)
        for v in os.listdir(base):
            for fn in os.listdir(os.path.join(base, v)):
                x = fn[:3]
                # server KRegion::LoadObject: g_SetFilePath("\\maps\\%s"); "\\%sv_%03d" % g_GetFilePath + "\\%03d_Region_S.dat"
                cur = _set_file_path(BS + "maps" + BS + mp)
                tot += 1; ok += _pak_uid(cur, BS + cur + v + BS + x + "_Region_S.dat") in spak
                # server ProcLoadPathGrid: "%s\\v_%03d\\%03d_Region_S.dat" % m_szPathName (cur = maps\)
                cur = _set_file_path(BS + "maps")
                tot += 1; ok += _pak_uid(cur, mp + BS + v + BS + x + "_Region_S.dat") in spak
                # client: "\\maps\\%s\\v_%03d\\%03d_Region_C.dat"
                cur = _set_file_path(BS + "settings")
                tot += 1; ok += _pak_uid(cur, BS + "maps" + BS + mp + BS + v + BS + x + "_Region_C.dat") in cpak
    print("  mo phong ghep duong dan engine -> uid co trong pak: %d/%d" % (ok, tot))
    assert ok == tot
    # doi chung: cach '..\map_publish' (ban giao muc 6) that bai
    cur = _set_file_path(BS + "maps")
    uids, _ = pak_index_client()
    sai = _pak_uid(cur, ".." + BS + "map_publish" + BS + "wumumenpai.wor")
    print("  doi chung '..\\map_publish\\wumumenpai.wor' -> uid %08X co trong pak client: %s (VLTK uid %08X) => cach '..' KHONG dung duoc" % (
        sai, sai in uids, P.name2id(BS + "map_publish" + BS + "wumumenpai.wor")))
    assert sai not in uids


# ============================================================ 5. kiem phu (chi bao)
def kiem_phu():
    hdr, rows = vltk_npcs(); new = new_npc_rows()
    # level script
    ci = hdr.index("LevelScript"); ca = hdr.index("ActionScript")
    scr = {}
    for i, r in new:
        for c in (ci, ca):
            s = r[c].strip()
            if s:
                scr.setdefault(s, []).append(i)
    for s, ids in sorted(scr.items()):
        p = os.path.join(SRV, s.lstrip(BS).replace(BS, os.sep))
        if not os.path.exists(p):
            bao("script NPC %s KHONG co tren server (dung boi %d NPC, vd id %d)" % (s, len(ids), ids[0]))
    # skill id
    sk = rd(os.path.join(SRV, "settings", "skills.txt")).replace("\r\n", "\n").split("\n")
    have = set()
    for l in sk[1:]:
        c = l.split("\t")
        if len(c) > 2 and c[2].strip().isdigit():
            have.add(int(c[2]))
    need = {}
    for i, r in new:
        for col in ("Skill1", "Skill2", "Skill3", "Skill4"):
            v = r[hdr.index(col)].strip()
            if v.isdigit() and int(v) > 0 and int(v) not in have:
                need.setdefault(int(v), []).append(i)
    if need:
        bao("%d ma ky nang NPC moi dung ma skills.txt server CHUA co (nguoi lam skills xu ly): %s" % (
            len(need), ", ".join("%d(%d npc)" % (k, len(v)) for k, v in sorted(need.items()))))


if __name__ == "__main__":
    steps = (npcs_txt, npcname_id, npcres_client, map_pak, worldset, package_ini, maplist, faction_ini, kiem_duongdan, kiem_phu)
    loi = []
    for f in steps:
        print("==", f.__name__)
        if KIEM:
            try:
                f()
            except Exception as e:
                print("   LOI:", repr(e)); loi.append(f.__name__)
        else:
            f()
    print("== BAO CAO (%d muc)" % len(BAO))
    for b in BAO:
        print(" -", b)
    if loi:
        print("HAM LOI:", loi)
    print("XONG" + (" (kiem)" if KIEM else ""))
