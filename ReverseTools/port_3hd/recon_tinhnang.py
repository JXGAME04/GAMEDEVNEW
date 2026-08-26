# -*- coding: utf-8 -*-
"""RECON - do "mat phu thuoc" cua MOT tinh nang ban Linux truoc khi port sang JX1.

Dung: python recon_tinhnang.py <thu muc con trong script cua Linux> [them...]
Vi du: python recon_tinhnang.py event\beidoulingpai vng_event\viminhlenh

In ra 5 muc:
  1. Danh sach tep + so dong + so dong CON SONG (khong bi chu thich)
  2. Ham Lua GOI RA NGOAI ma JX1 CHUA co (doi chieu bang dang ky trong
     Sources/Core/Src/ScriptFuns.cpp + KJx2WarInfra.cpp)
  3. Cac tep Include ra ngoai thu muc (phu thuoc script khac)
  4. Item {6,1,xxx} / {0,x,y} dung trong tinh nang: co trong bang item JX1 chua,
     ten JX1 co trung ten Linux khong
  5. Task id (SetTask/GetTask/SetBitTask/GetBitTask) - co dung trung ai o JX1 khong
"""
import io, os, re, sys, importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
spec = importlib.util.spec_from_file_location("dec2", os.path.join(os.path.dirname(os.path.abspath(__file__)), "dec2.py"))
dec2 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(dec2)

LNX = r"D:\ServerLinux\server1"
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"

LUA_CHUAN = set("""and break do else elseif end false for function if in local nil not or repeat
return then true until while print format strsub strlen strfind gsub tinsert tremove getn sort
type tonumber tostring random floor ceil abs mod max min dofile setglobal getglobal next call
rawget rawset assert error strrep strupper strlower date time sqrt""".split())


RE_CHUOI = re.compile('"[^"]*"' + "|'[^']*'")


def bo_chuoi(l):
    """bo noi dung trong nhay kep/don, de chu Viet trong cau thoai khong bi nham
    la ten ham (vd "Long Phung Huyet Ngoc Trac (He Hoa)" -> Trac()"""
    return RE_CHUOI.sub('""', l)


_TCVN3 = None


def tcvn3(s):
    """chuoi doc bang latin-1 tu tep JX1 -> chuoi unicode doc duoc.
    Dung bang NGUOC sinh tu chinh vn_to_octal cua skill (khong go tay byte nao)."""
    global _TCVN3
    if _TCVN3 is None:
        sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
        from vn_to_octal import unicode_to_tcvn3_bytes
        _TCVN3 = {}
        for cp in range(0x20, 0x2000):
            ch = chr(cp)
            try:
                b = unicode_to_tcvn3_bytes(ch)
            except Exception:
                continue
            if len(b) == 1 and b[0] >= 0x80:
                _TCVN3.setdefault(b[0], ch)
    return "".join(_TCVN3.get(ord(c), c) for c in s)


def dec_lines(p):
    return [dec2.decline2(l.rstrip(b"\r")) for l in open(p, "rb").read().split(b"\n")]


def dang_ky_jx1():
    """moi ten ham Lua da dang ky trong engine JX1"""
    ten = set()
    for f in os.listdir(CORE):
        if not f.endswith(".cpp"):
            continue
        try:
            d = io.open(os.path.join(CORE, f), encoding="latin-1", newline="").read()
        except Exception:
            continue
        for m in re.finditer(r'\{\s*"([A-Za-z_]\w*)"\s*,\s*[A-Za-z_]\w*\s*\}', d):
            ten.add(m.group(1))
    return ten


def ham_lua_jx1():
    """moi 'function ten(' dinh nghia trong cay script JX1"""
    ten = set()
    for dp, dn, fs in os.walk(os.path.join(JX1, "script")):
        for f in fs:
            if not f.endswith(".lua"):
                continue
            try:
                d = io.open(os.path.join(dp, f), encoding="latin-1", newline="").read()
            except Exception:
                continue
            for m in re.finditer(r"^\s*function\s+([A-Za-z_][\w:.]*)", d, re.M):
                ten.add(m.group(1).split(":")[-1].split(".")[-1])
                ten.add(m.group(1))
    return ten


def bang_item_jx1():
    """(genre,detail,particular) -> ten, tu magicscript.txt"""
    tb = {}
    for name in ["magicscript.txt", "questkey.txt", "otheritem.txt"]:
        for dp, dn, fs in os.walk(os.path.join(JX1, "settings", "item")):
            if name not in fs:
                continue
            p = os.path.join(dp, name)
            for l in io.open(p, encoding="latin-1", newline="").read().split("\n"):
                c = l.split("\t")
                if len(c) > 4 and c[1].isdigit() and c[2].lstrip("-").isdigit() and c[3].lstrip("-").isdigit():
                    tb.setdefault((c[1], c[2], c[3]), tcvn3(c[0]))
    return tb


def bang_item_linux():
    tb = {}
    p = os.path.join(LNX, "settings", "item", "004", "magicscript.txt")
    if not os.path.exists(p):
        return tb
    for l in open(p, "rb").read().split(b"\n"):
        s = dec2.decline2(l.rstrip(b"\r"))
        c = s.split("\t")
        if len(c) > 4 and c[1].isdigit() and c[2].lstrip("-").isdigit() and c[3].lstrip("-").isdigit():
            tb.setdefault((c[1], c[2], c[3]), c[0])
    return tb


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return
    thumucs = sys.argv[1:]
    dk = dang_ky_jx1()
    hl = ham_lua_jx1()
    itjx = bang_item_jx1()
    itlx = bang_item_linux()

    for tm in thumucs:
        goc = os.path.join(LNX, "script", tm)
        print("=" * 78)
        print("TINH NANG:  script\\%s" % tm)
        print("=" * 78)
        if not os.path.isdir(goc):
            print("  !! khong co thu muc nay trong ban Linux")
            continue

        tep = []
        for dp, dn, fs in os.walk(goc):
            for f in sorted(fs):
                if f.endswith(".lua"):
                    tep.append(os.path.join(dp, f))

        # --- 1) tep + ty le dong song
        print("\n[1] TEP (%d)" % len(tep))
        tong_song = 0
        noidung = {}
        for p in tep:
            out = dec_lines(p)
            song = [l for l in out if l.strip() and not l.strip().startswith("--")]
            tong_song += len(song)
            noidung[p] = (out, song)
            canh = "  <== TAT HET (ban Linux cung khong chay)" if len(song) == 0 else ""
            print("    %-32s %5d dong / %5d dong song%s"
                  % (os.path.basename(p), len(out), len(song), canh))
        print("    TONG dong song: %d" % tong_song)

        # --- 2) ham goi ra ngoai
        goi = {}
        tu_dinh_nghia = set()
        for p in tep:
            out, song = noidung[p]
            for l in song:
                for m in re.finditer(r"^\s*function\s+([A-Za-z_][\w:.]*)", l):
                    tu_dinh_nghia.add(m.group(1).split(":")[-1].split(".")[-1])
                    tu_dinh_nghia.add(m.group(1))
            for l in song:
                for m in re.finditer(r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(", bo_chuoi(l)):
                    goi.setdefault(m.group(1), 0)
                    goi[m.group(1)] += 1
        thieu = []
        for t, n in sorted(goi.items(), key=lambda kv: -kv[1]):
            if t in LUA_CHUAN or t in tu_dinh_nghia or t in dk or t in hl:
                continue
            thieu.append((t, n))
        print("\n[2] HAM JX1 CHUA CO (%d) - phai them vao engine hoac viet bang Lua" % len(thieu))
        for t, n in thieu:
            print("    %-34s goi %d lan" % (t, n))

        # --- 3) include ra ngoai
        inc = set()
        for p in tep:
            out, song = noidung[p]
            for l in song:
                for m in re.finditer(r'Include\w*\(\s*"([^"]+)"', l):
                    inc.add(m.group(1).replace("\\\\", "\\"))
        ngoai = sorted(x for x in inc if tm.lower().replace("\\", "/") not in x.lower().replace("\\", "/"))
        print("\n[3] INCLUDE RA NGOAI THU MUC (%d)" % len(ngoai))
        for x in ngoai:
            rel = x.lstrip("\\")
            co = os.path.exists(os.path.join(JX1, rel))
            print("    %-58s %s" % (x, "co san o JX1" if co else "<== JX1 THIEU"))

        # --- 4) item
        it = {}
        for p in tep:
            out, song = noidung[p]
            for l in song:
                for m in re.finditer(r"\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*[,}]", l):
                    g, d2, pt = m.groups()
                    if g == "6":   # genre 0 = trang bi, khong nam trong magicscript
                        it.setdefault((g, d2, pt), 0)
                        it[(g, d2, pt)] += 1
        print("\n[4] ITEM DUNG TRONG TINH NANG (%d ma)" % len(it))
        for k in sorted(it, key=lambda k: (int(k[0]), int(k[1]), int(k[2]))):
            tenl = itlx.get(k, "?")
            tenj = itjx.get(k, None)
            if tenj is None:
                trang = "<== JX1 KHONG CO MA NAY"
            elif tenl != "?" and tenj.strip() != tenl.strip():
                trang = "<== JX1 la '%s' (KHAC NGHIA)" % tenj[:34]
            else:
                trang = "ok"
            print("    %s,%s,%-6s Linux='%-34s' %s" % (k[0], k[1], k[2], tenl[:34], trang))

        # --- 5) task id
        tsk = {}
        for p in tep:
            out, song = noidung[p]
            for l in song:
                for m in re.finditer(r"(?:Set|Get)(?:Bit)?Task\w*\s*\(\s*(\d{3,5})", l):
                    tsk.setdefault(m.group(1), 0)
                    tsk[m.group(1)] += 1
                for m in re.finditer(r"TSK_\w*\s*=\s*(\d{3,5})", l):
                    tsk.setdefault(m.group(1), 0)
                    tsk[m.group(1)] += 1
                for m in re.finditer(r"nTaskID\s*=\s*(\d{3,5})", l):
                    tsk.setdefault(m.group(1), 0)
                    tsk[m.group(1)] += 1
        print("\n[5] TASK ID (%d) - kiem tra JX1 co ai dung chua" % len(tsk))
        for t in sorted(tsk, key=int):
            dung = []
            for dp, dn, fs in os.walk(os.path.join(JX1, "script")):
                for f in fs:
                    if not f.endswith(".lua"):
                        continue
                    try:
                        d = io.open(os.path.join(dp, f), encoding="latin-1", newline="").read()
                    except Exception:
                        continue
                    if re.search(r"(?:Set|Get)(?:Bit)?Task\w*\s*\(\s*%s\b" % t, d):
                        dung.append(os.path.relpath(os.path.join(dp, f), JX1))
                        if len(dung) >= 3:
                            break
                if len(dung) >= 3:
                    break
            print("    task %-6s %s" % (t, ("TRONG - dung duoc" if not dung else "<== DA DUNG: " + ", ".join(dung))))
        print()


main()
