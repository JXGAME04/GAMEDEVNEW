# -*- coding: utf-8 -*-
"""t71_quet_goi_nil_thanchunk.py - tim loi cung loai voi partner_reward2.lua.

BAI HOC: engine NAP SAN TUNG TEP .lua mot luc khoi dong (dong console
"Total ScriptLoaded : 2948"). Nghia la than chunk (ma nam NGOAI moi ham) cua MOI
tep deu chay DOC LAP mot lan - khong co tep nao Include no truoc.

=> Moi loi goi ham o THAN CHUNK phai TU DU: ham do phai duoc khai TRONG CHINH
   TEP, hoac trong mot tep da Include O TREN, hoac la ham ENGINE.
   Neu khong: ScriptError 4 luc khoi dong.

Toi da vap dung loi nay o partner_reward2.lua (goi BDH_CFG - ham khai trong
partner_reward.lua). Cong cu nay quet MOI tep sua hom nay xem con cho nao nua.

Cach lam:
  1. Tap ham ENGINE: doc Sources\\Core\\Src\\ScriptFuns*.cpp lay { "Ten", LuaTen }.
  2. Voi tung tep: lay cac dong o THAN CHUNK (khong nam trong function...end).
  3. Tim loi goi TEN( o cac dong do.
  4. TEN hop le neu: la ham engine, HOAC khai trong chinh tep, HOAC khai trong
     mot tep duoc Include o phia TREN dong do (de quy mot cap).

CHI DOC.
"""
import io
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lua_ham as lh  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
NGUON = r"D:\GAMEDEVNEW\Sources\Core\Src"
DUOI = (".truoc_gohecu", ".truoc_boldhc", ".truoc_dondatau", ".truoc_portlxw",
        ".truoc_cuu210", ".truoc_thutu", ".truoc_donvun", ".truoc_vacau",
        ".truoc_donchuthich", ".truoc_pb", ".truoc_donpb", ".truoc_v2",
        ".truoc_buhd", ".truoc_chanmin", ".truoc_valoi")
RE_GOI = re.compile(r"\b([A-Za-z_]\w*)\s*\(")
RE_INC = re.compile(r'Include\s*\(\s*[\["]+([^\]"]+)', re.I)
# tu khoa Lua + ham thu vien chuan - khong phai loi goi ham nguoi dung
BO = set("""if then else elseif end while for do local function return and or not
nil true false break repeat until in print type tonumber tostring format getn
tinsert tremove strlen strsub strfind strlower strupper strrep gsub abs floor
ceil mod max min random randomseed sqrt date call dofile next foreach setn
rawget rawset assert error""".split())


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def ham_engine():
    ra = set()
    for goc, _, ts in os.walk(NGUON):
        for t in ts:
            if not t.lower().endswith((".cpp", ".h")):
                continue
            try:
                d = doc(os.path.join(goc, t))
            except OSError:
                continue
            for m in re.finditer(r'\{\s*"([A-Za-z_]\w*)"\s*,\s*\w+\s*\}', d):
                ra.add(m.group(1))
            for m in re.finditer(r'lua_register\s*\([^,]+,\s*"([A-Za-z_]\w*)"',
                                 d):
                ra.add(m.group(1))
    return ra


def khai_trong(p):
    """Ten ham + bien toan cuc khai trong mot tep."""
    try:
        d = lh.sach(doc(p))
    except OSError:
        return set()
    ra = set(m.group(1) for m in
             re.finditer(r"^\s*function\s+([A-Za-z_]\w*)", d, re.M))
    ra |= set(m.group(1) for m in
              re.finditer(r"^\s*([A-Za-z_]\w*)\s*=", d, re.M))
    return ra


def main():
    he = ham_engine()
    print("=== t71 quet loi goi ham nil o THAN CHUNK ===")
    print("    ham engine doc duoc: %d" % len(he))

    tep = set()
    for goc, _, ts in os.walk(S):
        for t in ts:
            for d in DUOI:
                if t.endswith(d):
                    g = os.path.join(goc, t[:-len(d)])
                    if os.path.isfile(g):
                        tep.add(g)
    print("    tep sua hom nay: %d" % len(tep))
    print()

    n_loi = 0
    for p in sorted(tep):
        rel = os.path.relpath(p, S)
        try:
            raw = doc(p)
        except OSError:
            continue
        dong = [x.rstrip("\r") for x in raw.replace("\r\n", "\n").split("\n")]
        # xac dinh dong nao nam TRONG mot ham
        trong_ham = [False] * len(dong)
        cb = 0
        dang = False
        for i, l in enumerate(dong):
            s = lh.sach(l)
            if re.search(r"\bfunction\b", s) and not dang:
                dang = True
                cb = 0
            if dang:
                trong_ham[i] = True
                cb += lh.can_bang(l)
                if cb == 0:
                    dang = False
        # tap ten hop le
        hop_le = set(he) | khai_trong(p) | BO
        for m in RE_INC.finditer(raw):
            q = m.group(1).replace("/", "\\").lstrip("\\")
            q = os.path.join(S, q[len("script\\"):]
                             if q.lower().startswith("script\\") else q)
            if os.path.isfile(q):
                hop_le |= khai_trong(q)
        for i, l in enumerate(dong):
            if trong_ham[i] or l.lstrip().startswith("--") or not l.strip():
                continue
            s = lh.sach(l)
            for m in RE_GOI.finditer(s):
                ten = m.group(1)
                if ten in hop_le:
                    continue
                print("  !! %s:%d  goi %s()" % (rel, i + 1, ten))
                print("       %s" % l.strip()[:100])
                n_loi += 1
    print()
    print("=> %d cho goi ham co the NIL o than chunk" % n_loi)
    return 0


if __name__ == "__main__":
    sys.exit(main())
