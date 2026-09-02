# -*- coding: utf-8 -*-
"""hs_port2013.py [HOASON 01/09] - port bo script Linux script\\global\\huashan2013 (79 tep) + thu vien phu + vat pham sang JX1.
  python hs_port2013.py          ap dung
  python hs_port2013.py --kiem   chi in
Quy tac: chep nguyen byte (TCVN3/GBK), bo Include thu vien JX1 khong co (thay bang hs_shim.lua), doi ten ham,
         anh xa ma vat pham (bang REMAP), them add_npc_hoason() vao startgame.lua, bo 3 dong spawn tu che.
"""
import io, os, re, sys, shutil, struct
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
from vn_to_octal import unicode_to_tcvn3_bytes
KIEM = "--kiem" in sys.argv
MARK = "[HOASON 01/09]"
LIN = r"D:\ServerLinux\server1"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
SCR = os.path.dirname(os.path.abspath(__file__))
BS = chr(92)
def V(s): return unicode_to_tcvn3_bytes(s).decode("latin-1")
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, d):
    if KIEM: print("  (kiem) ghi", p); return
    os.makedirs(os.path.dirname(p), exist_ok=True)
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    print("  ghi", p)
CRLF = "\r\n"
BAO_CAO = []

# ------------------------------------------------------------ bang anh xa ma vat pham (VLTK/Linux -> JX1)
REMAP = {4062: 4938, 4063: 4939, 4325: 4940, 4326: 4941, 4327: 4942, 4328: 4943, 2908: 4944,
         2424: 4963, 2425: 4964, 30339: 4965, 30341: 4966}
for k in range(3942, 3960): REMAP[k] = 4945 + (k - 3942)

RX_ITEM = re.compile(r"(?<![\w.])(6\s*,\s*1\s*,\s*)(\d+)\b")
def remap_items(d, ten):
    n = 0
    def f(m):
        nonlocal n
        v = int(m.group(2))
        if v in REMAP:
            n += 1
            return m.group(1) + str(REMAP[v])
        return m.group(0)
    d = RX_ITEM.sub(f, d)
    return d, n

# ------------------------------------------------------------ 1. hs_shim.lua
def shim():
    p = os.path.join(SRV, "script", "global", "huashan2013", "hs_shim.lua")
    body = [
        "-- " + MARK + " shim cho bo script Linux huashan2013 chay tren JX1 (ham/bien Linux khong co o JX1)",
        "-- G_TASK (task\\150skilltask\\g_task.lua Linux): JX1 hoc ky nang 150 bang sach/NPC -> khong them muc menu",
        "if not G_TASK then G_TASK = {} end",
        "function G_TASK:OnMessage(szFaction, tbDailog, szFn) end",
        "function G_TASK:Talk() end",
        "-- task\\lv120skill\\head.lua Linux: task id da lam nhiem vu ky nang 120 (JX1 khong dung -> GetTask = 0)",
        "LV120_SKILL_ID = 2463",
        "-- misc\\daiyitoushi\\toushi_function.lua Linux (Dai Nghe Dau Su = doi phai): JX1 doi phai bang Lenh bai tan thu",
        "function daiyitoushi_main(nFaction)",
        "\tTalk(1, \"\", \"" + V("Đại Nghệ Đầu Sư (đổi môn phái theo bản Linux) chưa mở. Hãy dùng Lệnh Bài Tân Thủ để chuyển phái.") + "\")",
        "end",
        "-- item helper Linux -> JX1 (GetItemCount(nNature,g,d,p) / ConsumeItem(nCount,nNature,g,d,p))",
        "function HaveCommonItem(g, d, p) return GetItemCount(0, g, d, p) end",
        "function DelCommonItem(g, d, p) return ConsumeItem(1, 0, g, d, p) end",
        "function IsNpcHide(nNpcIdx) return 0 end",
        "function M2g2Player(s) return Msg2Player(s) end",
        "function SetNpcAI(nNpcIdx, nType) return SetNpcAIType(nNpcIdx, nType) end",
        "function ClearFactionRecord() return ClearFactionIfnfo() end",
        "function no() end",
        ""]
    wr(p, CRLF.join(body))

# ------------------------------------------------------------ 2. thu vien phu: progressbar (viet lai), getrectangle_point, repute_head, dachengmiji
def libs():
    # progressconfig.txt -> bang Lua nhung (JX1 khong co TabFile_GetRowCount/WriteLog; OpenProgressBar JX1 chi dung title/frame/OnTime)
    cfg = rd(os.path.join(LIN, "settings", "progressconfig.txt")).replace(CRLF, "\n").split("\n")
    hdr = cfg[0].split("\t")
    ev = [h for h in hdr if h.startswith("EVENT_")]
    rows = []
    for l in cfg[1:]:
        c = l.split("\t")
        if len(c) < 3 or not c[0].strip().isdigit(): continue
        nid = int(c[0]); title = c[1].replace('"', '\\"'); tm = int(c[2] or 0)
        nEvent = 0
        for j, e in enumerate(ev):
            k = hdr.index(e)
            bit = int(c[k]) if k < len(c) and c[k].strip().isdigit() else 0
            if bit: nEvent = nEvent | (1 << j)   # SetBit(nEvent, j+1?, 1) - Linux tbEventType[EVENT_x] = j+1 -> bit (j+1)
        rows.append("\t[%d] = {nId = %d, szTitle = \"%s\", nTime = %d, nEvent = %d, bDesc = 0}," % (nid, nid, title, tm, nEvent))
    pb = [
        "-- " + MARK + " port tu Linux script\\lib\\progressbar.lua (bo IncludeLib TIMER/FILESYS, cau hinh settings\\progressconfig.txt nhung thang vao bang)",
        "-- JX1 OpenProgressBar (KJx2WarInfra.cpp LuaHD3_OpenProgressBar) = hop dem gio TimeBox: chi dung szTitle, nFrame va ten ham OnTime.",
        "local tbData = {}",
        "tbProgressBar = {}",
        "tbProgressBar.tbConfig = {",
    ] + rows + [
        "}",
        "function tbProgressBar:OpenByConfig(nId, fnOnTime, tbOnTimeParam, fnOnBreak, tbOnBreakParam)",
        "\tif not self.tbConfig or not self.tbConfig[nId] then",
        "\t\treturn",
        "\tend",
        "\tlocal pTemp = self.tbConfig[nId]",
        "\tself:Start(pTemp.szTitle, pTemp.nTime * 18, pTemp.nEvent, pTemp.bDesc, fnOnTime, tbOnTimeParam, fnOnBreak, tbOnBreakParam)",
        "end",
        "function tbProgressBar:Open(szTitle, nFrame, tbEvent, bDesc, fnOnTime, tbOnTimeParam, fnOnBreak, tbOnBreakParam)",
        "\tself:Start(szTitle, nFrame, 0, bDesc, fnOnTime, tbOnTimeParam, fnOnBreak, tbOnBreakParam)",
        "end",
        "function tbProgressBar:Start(szTitle, nFrame, nEvent, bDesc, fnOnTime, tbOnTimeParam, fnOnBreak, tbOnBreakParam)",
        "\tOpenProgressBar(szTitle, nFrame, nEvent, bDesc, \"tbProgressBar_OnTime\", \"tbProgressBar_OnBreak\")",
        "\t%tbData[PlayerIndex] = {}",
        "\t%tbData[PlayerIndex].fnOnTime = fnOnTime",
        "\t%tbData[PlayerIndex].tbOnTimeParam = tbOnTimeParam",
        "\t%tbData[PlayerIndex].fnOnBreak = fnOnBreak",
        "\t%tbData[PlayerIndex].tbOnBreakParam = tbOnBreakParam",
        "end",
        "function tbProgressBar_OnTime()",
        "\tlocal tbPlayerProgressData = %tbData[PlayerIndex] or {}",
        "\tif not tbPlayerProgressData.fnOnTime then",
        "\t\treturn",
        "\tend",
        "\treturn call(tbPlayerProgressData.fnOnTime, tbPlayerProgressData.tbOnTimeParam or {})",
        "end",
        "function tbProgressBar_OnBreak()",
        "\tlocal tbPlayerProgressData = %tbData[PlayerIndex] or {}",
        "\tif not tbPlayerProgressData.fnOnBreak then",
        "\t\treturn",
        "\tend",
        "\treturn call(tbPlayerProgressData.fnOnBreak, tbPlayerProgressData.tbOnBreakParam or {})",
        "end",
        ""]
    wr(os.path.join(SRV, "script", "lib", "progressbar.lua"), CRLF.join(pb))
    for rel in (r"script\lib\getrectangle_point.lua", r"script\global\repute_head.lua", r"script\item\dachengmiji_90.lua", r"script\item\dachengmiji_120.lua", r"settings\progressconfig.txt"):
        src = os.path.join(LIN, rel); dst = os.path.join(SRV, rel)
        if os.path.exists(dst) and rd(dst) == rd(src): print("  da co:", rel); continue
        d = rd(src)
        if rel.endswith(".lua"):
            d = "-- " + MARK + " chep nguyen ban Linux " + rel + CRLF + d
            d, n = remap_items(d, rel)
            if n: print("  ", rel, "remap item", n)
        wr(dst, d)
    ts = os.path.join(SRV, r"script\task\system\task_string.lua")
    if not os.path.exists(ts):
        src = os.path.join(LIN, r"script\task\system\task_string.lua")
        if os.path.exists(src): wr(ts, rd(src)); BAO_CAO.append("chep them script\\task\\system\\task_string.lua (dachengmiji can)")
        else: BAO_CAO.append("THIEU script\\task\\system\\task_string.lua (dachengmiji_90/120 Include)")

# ------------------------------------------------------------ 3. huashan2013 (79 tep)
BO_INCLUDE = [r"\\script\\missions\\freshman_match\\head.lua", r"\\script\\task\\newtask\\education\\knowmagic.lua",
              r"\\script\\task\\lv120skill\\head.lua", r"\\script\\task\\150skilltask\\g_task.lua", r"\\script\\misc\\daiyitoushi\\toushi_function.lua"]
def huashan2013():
    root = os.path.join(LIN, "script", "global", "huashan2013")
    dst_root = os.path.join(SRV, "script", "global", "huashan2013")
    n_tep = 0; n_item = 0
    for dp, ds, fs in os.walk(root):
        for f in sorted(fs):
            if not f.endswith(".lua"): continue
            src = os.path.join(dp, f); rel = os.path.relpath(src, root)
            d = rd(src)
            nl = CRLF if CRLF in d else "\n"
            lines = d.split(nl)
            out = []
            for l in lines:
                skip = False
                for inc in BO_INCLUDE:
                    if inc in l and "Include" in l:
                        out.append("-- " + MARK + " bo (JX1 khong co, xem hs_shim.lua): " + l.strip()); skip = True; break
                if skip: continue
                out.append(l)
            d2 = nl.join(out)
            # ham doi ten (an toan hon shim vi shim cung co)
            d2 = d2.replace("M2g2Player(", "Msg2Player(")
            d2, n = remap_items(d2, rel); n_item += n
            if f == "npc_hoason.lua":
                assert "add_npc(npc_hoason)" in d2
                d2 = d2.replace("add_npc(npc_hoason)", "hs_add_npc(npc_hoason)")
                d2 += nl + nl.join([
                    "-- " + MARK + " hs_add_npc = ban sao Linux script\\global\\vng\\add_npc.lua:15 (JX1 khong co add_npc toan cuc)",
                    "function hs_add_npc(tbnpc)",
                    "\tfor i = 1 , getn(tbnpc) do",
                    "\t\tlocal Mid = SubWorldID2Idx(tbnpc[i][4]);",
                    "\t\tif (Mid >= 0 ) then",
                    "\t\t\tlocal nNpcIdx = AddNpc(tbnpc[i][1],tbnpc[i][2],Mid,tbnpc[i][5] * 32,tbnpc[i][6] * 32,tbnpc[i][7],tbnpc[i][8]);",
                    "\t\t\tif (nNpcIdx > 0) then SetNpcScript(nNpcIdx, tbnpc[i][10]); end",
                    "\t\tend;",
                    "\tend;",
                    "end", ""])
                # NPC box.lua (Linux changefeature) -> JX1 ruong chua do ; npcdailog.lua activitysys giu neu co
                if not os.path.exists(os.path.join(SRV, r"script\changefeature\npc\box.lua")):
                    d2 = d2.replace("\\\\script\\\\changefeature\\\\npc\\\\box.lua", "\\\\script\\\\global\\\\npcchucnang\\\\ruongchua.lua")
                    BAO_CAO.append("npc_hoason: Ruong chua do 987 dung \\script\\global\\npcchucnang\\ruongchua.lua (JX1)")
                if not os.path.exists(os.path.join(SRV, r"script\activitysys\npcdailog.lua")):
                    BAO_CAO.append("npc_hoason: \\script\\activitysys\\npcdailog.lua (Phong Thai Linh) KHONG co o JX1 - NPC van spawn, click se loi script")
            # them Include shim o dau (sau dong dau tien neu la comment header)
            if f != "hs_shim.lua":
                d2 = "Include(\"\\\\script\\\\global\\\\huashan2013\\\\hs_shim.lua\")\t-- " + MARK + nl + d2
            dst = os.path.join(dst_root, rel)
            if os.path.exists(dst) and rd(dst) == d2: continue
            wr(dst, d2); n_tep += 1
    print("  huashan2013: ghi %d tep, remap %d cho item" % (n_tep, n_item))

# ------------------------------------------------------------ 4. startgame.lua + bo 3 dong spawn tu che
def startgame():
    p = os.path.join(SRV, "script", "startgame.lua")
    d = rd(p)
    if MARK not in d:
        old = "Include(\"\\\\script\\\\startgame\\\\thon\\\\namnhactran.lua\");\r\n"
        assert d.count(old) == 1, "startgame: neo Include namnhactran"
        d = d.replace(old, old + "Include(\"\\\\script\\\\global\\\\huashan2013\\\\npc_hoason.lua\");\t-- " + MARK + " NPC/quai Hoa Son 2013 (Linux)\r\n")
        old = "\taddfullnpc()"
        assert d.count(old) == 1, "startgame: neo addfullnpc"
        d = d.replace(old, "\tadd_npc_hoason()\t-- " + MARK + " Linux npc_hoason.lua: quai + NPC map 987 + Hoa Son Kiem Khach 8 thon\r\n" + old)
        wr(p, d)
    else: print("  startgame da co")
    for fn in ("balanghuyen.lua", "giangtanthon.lua", "longmontran.lua"):
        p = os.path.join(SRV, "script", "startgame", "thon", fn)
        d = rd(p)
        lines = d.split("\r\n")
        new = [l for l in lines if not (MARK in l and "hoason.lua" in l)]
        if len(new) != len(lines):
            wr(p, "\r\n".join(new)); print("   bo spawn tu che:", fn)

# ------------------------------------------------------------ 5. magicscript: 29 vat pham moi (server + client), icon thieu -> rut tu pak VLTK ra dia client
def items():
    rows = {}
    for l in open(os.path.join(SCR, "..", "out", "hs_items_vltk.tsv"), "rb").read().split(b"\n"):
        c = l.rstrip(b"\r").split(b"\t")
        if len(c) > 20 and c[1] == b"6" and c[2] == b"1" and c[3].isdigit():
            rows[int(c[3])] = c
    thieu = [k for k in REMAP if k not in rows]
    assert not thieu, "hs_items_vltk.tsv thieu dong: %r" % thieu
    # icon: co trong pak JX1?
    import pakdump as P
    VROOT = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"
    vfiles = {}
    for dp, ds, fs in os.walk(VROOT):
        for f in fs:
            if f.lower().endswith((".pak", ".mps")): vfiles[f.lower()] = os.path.join(dp, f)
    def alow(s):  # ha chu ASCII, giu nguyen byte >= 0x80 (GBK)
        return "".join(ch.lower() if ord(ch) < 128 else ch for ch in s)
    idx = {}
    for l in open(os.path.join(SCR, "..", "jx1_client_pak_index.tsv"), "rb").read().split(b"\n"):
        c = l.split(b"\t")
        if len(c) >= 2:
            try: idx[int(c[1].strip(), 16)] = c[0].decode()
            except: pass
    vidx = {}
    vp = r"D:\GAMEDEVNEW\ReverseTools\phai3\phantich\vltk_pak_index_all.tsv"
    if os.path.exists(vp):
        for l in open(vp, "rb").read().split(b"\n"):
            c = l.split(b"\t")
            if len(c) >= 2:
                try: vidx[int(c[1].strip(), 16)] = c[0].decode("latin-1")
                except: pass
    new_lines = []
    for old in sorted(REMAP, key=lambda k: REMAP[k]):
        c = rows[old]; nid = REMAP[old]
        img = c[4].decode("latin-1")
        u = P.name2id(alow(img))
        if u not in idx:
            # thu rut tu pak VLTK ra dia client
            ok = False
            if u in vidx:
                try:
                    import ucl
                    pk = vfiles.get(vidx[u].lower(), vidx[u])
                    f, es = P.entries(pk)
                    for e in es:
                        if e[0] == u:
                            cs = e[3] & 0xFFFFFF; fl = e[3] >> 24
                            f.seek(e[1]); raw = f.read(cs if cs else e[2])
                            blob = raw if fl == 0 else ucl.nrv2b_decompress_8(raw, e[2])
                            dst = os.path.join(CLI, img.strip(BS))
                            if not KIEM:
                                os.makedirs(os.path.dirname(dst), exist_ok=True)
                                open(dst, "wb").write(blob)
                            ok = True; BAO_CAO.append("icon %s rut tu VLTK %s -> dia client" % (img, os.path.basename(pk)))
                            break
                    f.close()
                except Exception as ex:
                    BAO_CAO.append("icon %s: loi rut %r" % (img, ex))
            if not ok:
                BAO_CAO.append("icon %s KHONG co (pak JX1 + VLTK) -> dung obj_item_lection.spr cho item %d" % (img, nid))
                img = BS + "spr" + BS + "item" + BS + "questkey" + BS + "obj_item_lection.spr"
        script = c[13].decode("latin-1")
        if script.lower().endswith("noscript.lua") or script.strip() == "":
            script = "0"
        else:
            sp = os.path.join(SRV, script.strip(BS))
            if not os.path.exists(sp):
                BAO_CAO.append("item %d: script %s KHONG co o JX1" % (nid, script))
        obj = c[5].decode(); w = c[6].decode(); h = c[7].decode()
        price = c[10].decode() if c[10].strip().isdigit() else "0"
        maxstack = c[20].decode() if c[20].strip().isdigit() else "0"
        name = c[0].decode("latin-1"); intro = c[8].decode("latin-1")
        row = "\t".join([name, "6", "1", str(nid), img, obj, w, h, intro, script, price, "1", maxstack, "0"] + [""] * 16)
        new_lines.append((nid, row))
    for p in (os.path.join(SRV, r"settings\item\magicscript.txt"), os.path.join(CLI, r"settings\item\magicscript.txt")):
        d = rd(p)
        lines = d.split(CRLF)
        if lines[-1] == "": lines = lines[:-1]
        last = lines[-1].split("\t")
        assert last[3].isdigit(), "magicscript dong cuoi la?"
        last_id = int(last[3])
        assert len(lines) == last_id + 2, "magicscript: so dong %d != id cuoi %d + 2 (bay ParticularType = so dong - 1)" % (len(lines), last_id)
        if last_id >= 4966:
            print("  magicscript da co toi", last_id, p); continue
        assert last_id == 4937, "magicscript id cuoi %d != 4937" % last_id
        for nid, row in new_lines:
            assert nid == len(lines) - 1, "lech dong: id %d vs dong %d" % (nid, len(lines))
            lines.append(row)
        wr(p, CRLF.join(lines) + CRLF)
    print("  magicscript: +%d vat pham (4938..4966)" % len(new_lines))

if __name__ == "__main__":
    for f in (shim, libs, huashan2013, startgame, items):
        print("==", f.__name__); f()
    print("== BAO CAO")
    for b in BAO_CAO: print("  -", b)
    print("XONG" + (" (kiem)" if KIEM else ""))
