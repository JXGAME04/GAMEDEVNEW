# -*- coding: utf-8 -*-
"""
gen_tk_ruong.py - SINH BANG RUONG theo THANH x HUONG cho KTongKimTables.h.

Nguon (doc thang tu may chu dang chay, khong go tay):
  1. bin/server/script/item/ib/shenxingfu.lua
       RUONG_ARRAY  = toa do (o) cac ruong cua 7 thanh - DANH SACH THO, KHONG ghi huong
       THANH_ARRAY  = menu Than Hanh Phu: moi thanh co nhung HUONG nao, kem revId
  2. bin/server/settings/RevivePos.ini
       revId -> toa do MPS diem hoi sinh cua chinh huong do
GHEP: huong X cua thanh Y = ruong GAN NHAT diem hoi sinh revId cua huong X.
      (chinh xac vi moi cua thanh chi co mot ruong; script tu kiem khoang cach + trung lap)

Chay: python gen_tk_ruong.py [--ghi]   (--ghi = chen thang vao KTongKimTables.h, idempotent)
"""
import io, os, re, sys

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
HDR = r"D:\GAMEDEVNEW\Sources\Core\Src\KTongKimTables.h"
GHI = "--ghi" in sys.argv

# thu tu 7 thanh PHAI trung g_LDVeMap trong KLienDauTables.h
LD_VE_MAP = [1, 11, 162, 37, 78, 80, 176]
HUONG = ["Trung Tam", "Dong", "Tay", "Nam", "Bac"]
# tu khoa nhan dien huong trong ten muc (TCVN3 -> so sanh bang byte)
KHOA = {
    0: [b"Trung T\xa9m"],
    1: [b"\xa7\xabng"],
    2: [b"T\xa9y"],
    3: [b"Nam"],
    4: [b"B\xbec"],   # 'Bac' TCVN3: B + 0xbe ('a mu nguoc sac') + c
}


def doc_lua():
    p = os.path.join(SRV, r"script\item\ib\shenxingfu.lua")
    s = io.open(p, encoding="latin-1", newline="").read()
    # ---- RUONG_ARRAY ----
    i = s.index("RUONG_ARRAY")
    j = s.index("};", i)
    than = s[i:j]
    ruong = []
    for dong in re.findall(r"\{((?:\s*\{\s*\d+\s*,\s*\d+\s*\}\s*,?)+)\s*\}\s*(?:,|--)", than):
        cap = [(int(a), int(b)) for a, b in re.findall(r"\{\s*(\d+)\s*,\s*(\d+)\s*\}", dong)]
        if cap:
            ruong.append(cap)
    # ---- THANH_ARRAY ----
    i = s.index("THANH_ARRAY")
    j = s.index("\nTHON_ARRAY", i)
    khoi = s[i:j]
    thanh = []
    for m in re.finditer(r"\{\s*(\d+)\s*,\s*\"([^\"]*)\"\s*,\s*\{(.*?)\}\s*\}", khoi, re.S):
        mapid = int(m.group(1))
        ten = m.group(2)
        # BAY: '(.*?)\}\s*\}' an mat dau '}' cua MUC CUOI -> phai bu lai truoc khi tach,
        # khong thi moi thanh rung mat huong cuoi cung (Bac).
        muc = [(int(a), b) for a, b in re.findall(r"\{\s*(\d+)\s*,\s*\"([^\"]*)\"\s*\}", m.group(3) + "}")]
        thanh.append((mapid, ten, muc))
    return ruong, thanh


def doc_revive():
    p = os.path.join(SRV, r"settings\RevivePos.ini")
    s = io.open(p, encoding="latin-1", newline="").read()
    rev = {}          # mapid -> {revId: (mpsx, mpsy)}
    cur = None
    for l in s.replace("\r", "").split("\n"):
        l = l.strip()
        m = re.match(r"^\[(\d+)\]$", l)
        if m:
            cur = int(m.group(1)); rev[cur] = {}; continue
        m = re.match(r"^(\d+)\s*=\s*(\d+)\s*,\s*(\d+)\s*$", l)
        if m and cur is not None:
            rev[cur][int(m.group(1))] = (int(m.group(2)), int(m.group(3)))
    return rev


def huong_cua(ten_byte):
    """ten muc (TCVN3 bytes) -> chi so huong 0..4, None neu khong ro"""
    for k, ds in KHOA.items():
        for kw in ds:
            if kw in ten_byte:
                # 'Nam' de trung 'Trung Tam'? khong: 'Trung T\xa9m' bat truoc va return
                return k
    return None


def main():
    ruong, thanh = doc_lua()
    rev = doc_revive()
    print("RUONG_ARRAY: %d thanh, so ruong tung thanh = %s" % (len(ruong), [len(x) for x in ruong]))
    print("THANH_ARRAY: %d thanh = %s" % (len(thanh), [t[0] for t in thanh]))
    if [t[0] for t in thanh] != LD_VE_MAP:
        print("!! THU TU THANH_ARRAY KHAC g_LDVeMap %s -> PHAI sua tay truoc khi dung" % LD_VE_MAP)
        sys.exit(1)
    if len(ruong) != len(thanh):
        print("!! so hang RUONG_ARRAY (%d) khac THANH_ARRAY (%d)" % (len(ruong), len(thanh)))
        sys.exit(1)

    bang = []      # [thanh][huong] = (x, y) hoac None
    bangrev = []
    loi = []
    for ti, (mapid, ten, muc) in enumerate(thanh):
        hang = [None] * 5
        hangrev = [-1] * 5
        dung = set()
        for revid, tenmuc in muc:
            tb = tenmuc.encode("latin-1")
            # 'Trung Tam' phai xet TRUOC 'Nam' vi chuoi 'Trung T\xa9m' khong chua 'Nam' -> an toan
            h = huong_cua(tb)
            if h is None:
                loi.append("thanh %d: khong ro huong cua muc %r" % (mapid, tenmuc)); continue
            rp = rev.get(mapid, {}).get(revid)
            if not rp:
                loi.append("thanh %d: revId %d khong co trong RevivePos.ini" % (mapid, revid)); continue
            rx, ry = rp[0] // 32, rp[1] // 32
            # ruong gan diem hoi sinh nhat
            best, bd = None, 10 ** 9
            for k, (cx, cy) in enumerate(ruong[ti]):
                d = (cx - rx) ** 2 + (cy - ry) ** 2
                if d < bd:
                    bd, best = d, k
            d = int(bd ** 0.5)
            if best is None or d > 60:
                loi.append("thanh %d huong %s: ruong gan nhat cach %d o (>60) - dang ngo" % (mapid, HUONG[h], d))
            if best in dung:
                loi.append("thanh %d: ruong #%d bi gan cho HAI huong" % (mapid, best))
            dung.add(best)
            hang[h] = ruong[ti][best]
            hangrev[h] = revid
            print("  map %3d %-22s rev %2d hoi sinh o (%d,%d) -> ruong %-14s cach %2d o  [%s]"
                  % (mapid, tenmuc.encode("latin-1").decode("latin-1"), revid, rx, ry,
                     str(ruong[ti][best]), d, HUONG[h]))
        thua = [k for k in range(len(ruong[ti])) if k not in dung]
        if thua:
            print("  (thanh %d: %d ruong KHONG mang huong nao: %s - bo qua)"
                  % (mapid, len(thua), [ruong[ti][k] for k in thua]))
        bang.append(hang)
        bangrev.append(hangrev)

    if loi:
        print("\n!! CANH BAO:")
        for l in loi: print("   " + l)

    dong = ["// ===== RUONG CHUA DO theo THANH x HUONG (04/09/2026) - SINH TU DONG boi",
            "// ReverseTools/gen_tk_ruong.py tu shenxingfu.lua (RUONG_ARRAY + THANH_ARRAY) va",
            "// settings/RevivePos.ini. Huong duoc gan bang cach lay ruong GAN NHAT diem hoi sinh",
            "// cua chinh huong do - vi RUONG_ARRAY la danh sach THO, thu tu KHAC menu Than Hanh Phu.",
            "// Hang = thu tu g_LDVeMap {1,11,162,37,78,80,176} (dung thang o 'Het tran ve' cua tab TK).",
            "// Cot = 0 Trung Tam / 1 Dong / 2 Tay / 3 Nam / 4 Bac.  {-1,-1} = thanh KHONG co huong do.",
            "#define TK_RUONG_THANH\t7",
            "#define TK_RUONG_HUONG\t5",
            "static const TKPoint g_TKRuong[TK_RUONG_THANH][TK_RUONG_HUONG] = {"]
    for ti, hang in enumerate(bang):
        o = ", ".join("{ %4d, %4d }" % (c[0], c[1]) if c else "{   -1,   -1 }" for c in hang)
        dong.append("\t{ %s },\t// %-3d %s" % (o, thanh[ti][0], thanh[ti][1].encode("latin-1").decode("latin-1")))
    dong.append("};")
    dong.append("// revId cua RevivePos.ini tuong ung (chi de doi chieu khi go loi), -1 = khong co")
    dong.append("static const short g_TKRuongRev[TK_RUONG_THANH][TK_RUONG_HUONG] = {")
    for hangrev in bangrev:
        dong.append("\t{ %s }," % ", ".join("%3d" % r for r in hangrev))
    dong.append("};")
    khoi = "\n".join(dong)
    print("\n" + khoi)

    if GHI:
        s = io.open(HDR, encoding="latin-1", newline="").read()
        nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
        k = khoi.replace("\n", nl)
        if "g_TKRuong[TK_RUONG_THANH]" in s:
            a = s.index("// ===== RUONG CHUA DO theo THANH x HUONG")
            b = s.index("};", s.index("g_TKRuongRev")) + 2
            s = s[:a] + k + s[b:]
            print("\n-> da THAY khoi cu trong KTongKimTables.h")
        else:
            # PHAI chen SAU 'struct TKPoint' (dong 44) -> neo o cuoi tep, truoc #endif
            neo = "#endif // KTONGKIMTABLES_H"
            if neo not in s:
                print("!! khong thay neo trong KTongKimTables.h"); sys.exit(1)
            s = s.replace(neo, k + nl + nl + neo, 1)
            print("\n-> da CHEN khoi moi vao KTongKimTables.h")
        io.open(HDR, "w", encoding="latin-1", newline="").write(s)


if __name__ == "__main__":
    main()
