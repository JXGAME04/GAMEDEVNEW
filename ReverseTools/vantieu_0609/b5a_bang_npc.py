# -*- coding: ascii -*-
"""
b5a_bang_npc.py -- BO VA BANG NPC + TAI NGUYEN cho Long Mon Tieu Cuc (van tieu).

Lam 3 viec:
  1) Ghi de 7 hang RONG trong settings/npcs.txt cho id 2145 2146 2147 2148 2156 2157 2158.
     LUAT DONG: dong 1-based = id + 2  (KNpcTemplate.cpp:70 "int nNpcTempRow = nNpcTemplateId + 2").
     7 hang nay o JX1 dang la hang RONG (ca 87 o deu la chuoi "0") -> chi ghi de,
     TONG SO DONG KHONG DOI.
  2) Noi dung = 87 COT DAU cua hang Linux tuong ung (npcs.txt cua D:\\ServerLinux\\server1),
     CAT 16 cot duoi (Linux 103 cot, JX1 87 cot).
     LY DO CAT: KTabFile::CreateTabOffset lay m_Width tu DONG DAU (=87) roi doc dung 87 o
     moi hang, KHONG nhay den ky tu xuong dong -> mot hang 103 o se lam LECH CA BANG.
     Rieng id 2156: hang Linux con mang ten cu "Ruong Giang Sinh" (hang bi dung lai);
     ten dung theo script la "Tan Lac Tieu Vat" (npc_lmbiaoche.lua:81 AddNpcEx) -> thay cot 0
     bang byte TCVN3 sinh tu vn_to_octal.unicode_to_tcvn3_bytes.
  3) Them 2 hang vao client/settings/NpcRes/npc_res_kind_file_name.txt: 'enemy243' (id 2147)
     va 'passerby379' (id 2158). Thieu -> KNpcResNode::Init tra FALSE -> NPC VO HINH.

An toan:
  - KHONG dung Edit/Write tool len tep du lieu: doc/ghi thuan byte (mo "rb"/"wb").
  - Idempotent: chay lai khong doi gi.
  - Sao luu <tep>.truoc_lmbc (chi tao lan dau, khong de ban luu goc).
  - Assert so dong khong doi, moi dong du 87 o, so byte CAO (>=0x80) ngoai 7 hang khong doi,
    so byte tong lech dung bang tong lech cua 7 hang.
  - Id 2230/2231/2232/2233 DA CO SAN va da Viet hoa -> kiem tra byte GIONG HET truoc/sau.
  - Sau khi ghi thi DOC LAI tep tu dia va tra TEN o dung 7 dong vua ghi de.

Da kiem chung (06/09):
  - KTabFile.cpp:159-168 BE GAY vong va nhoi o rong khi hang NGAN hon m_Width -> hang ngan
    KHONG lam lech bang (npc_res_kind_file_name.txt dong 965 'rongxanh' chi co 18 o va van
    chay duoc tu truoc). Chi hang DAI hon m_Width moi lam lech, vi doc du m_Width o roi
    KTabFile.cpp:172 khong thay CRLF nen khong nhay dong.
  - SPR cua enemy243 va passerby379 CO SAN trong client\\data\\updatejx14.pak
    (\\spr\\npcres\\enemy\\enemy243.spr, \\spr\\npcres\\passerby\\passerby379.spr ...),
    nen chi thieu moi hang trong npc_res_kind_file_name.txt.

Dung:
  python b5a_bang_npc.py --kiem              # chi kiem tra, khong ghi (cay chay that)
  python b5a_bang_npc.py                     # ap dung vao cay chay that
  python b5a_bang_npc.py --goc <THUMUC>      # ap dung vao ban sao (thu muc chua server\\ va client\\)
  python b5a_bang_npc.py --tao-thu <THUMUC>  # chep 3 tep tu cay that ra <THUMUC> de chay thu
"""

import os
import sys
import shutil
import argparse
import importlib.util

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

# ---------------------------------------------------------------- hang so

GOC_THAT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
LINUX_NPCS = r"D:\ServerLinux\server1\settings\npcs.txt"
VN2OCTAL = r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts\vn_to_octal.py"
DEC2_DIR = r"D:\GAMEDEVNEW\ReverseTools\port_3hd"

JX1_COT = 87
LINUX_COT = 103
HAUTO = ".truoc_lmbc"

# 7 id phai ghi de (hang dang rong o JX1)
ID_GHIDE = [2145, 2146, 2147, 2148, 2156, 2157, 2158]
# 4 id DA CO SAN, da Viet hoa -> canh gac, tuyet doi khong duoc doi
ID_CANHGAC = [2230, 2231, 2232, 2233]

# Ten dung theo script, thay cho ten cu trong hang Linux bi dung lai
# "Tan Lac Tieu Vat"
TEN_DE = {
    2156: "T\u00e1n L\u1ea1c Ti\u00eau V\u1eadt",
}

# (ten moi, ten hang khuon) trong npc_res_kind_file_name.txt
RES_THEM = [
    ("enemy243", "enemy244"),        # tai nguyen cua id 2147
    ("passerby379", "passerby378"),  # tai nguyen cua id 2158
]
RES_COT = 19


# ---------------------------------------------------------------- tien ich

class LoiVa(Exception):
    pass


def nap_vn2octal():
    if not os.path.isfile(VN2OCTAL):
        raise LoiVa("khong thay vn_to_octal.py: %s" % VN2OCTAL)
    spec = importlib.util.spec_from_file_location("vn_to_octal_lmbc", VN2OCTAL)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    if not hasattr(mod, "unicode_to_tcvn3_bytes"):
        raise LoiVa("vn_to_octal.py thieu unicode_to_tcvn3_bytes")
    return mod


def nap_dec2():
    """Bo giai ma TCVN3+GBK de IN ra ten cho de doc. Thieu cung khong sao."""
    try:
        if DEC2_DIR not in sys.path:
            sys.path.insert(0, DEC2_DIR)
        from dec2 import decline2  # type: ignore
        return decline2
    except Exception:
        return None


_DECLINE = None


def hien(b):
    """Bien byte TCVN3/GBK thanh chuoi de in ra man hinh."""
    global _DECLINE
    if _DECLINE is None:
        _DECLINE = nap_dec2() or False
    if _DECLINE:
        try:
            return _DECLINE(b)
        except Exception:
            pass
    return b.decode("latin-1")


def doc_byte(p):
    with open(p, "rb") as f:
        return f.read()


def ghi_byte(p, b):
    with open(p, "wb") as f:
        f.write(b)


def tach_dong(b, ten):
    """Tach theo CRLF. Tra (danh sach dong, duoi); duoi la phan sau CRLF cuoi cung."""
    if b.count(b"\n") != b.count(b"\r\n"):
        raise LoiVa("%s: co dong ket thuc bang LF tran (khong phai CRLF)" % ten)
    phan = b.split(b"\r\n")
    duoi = phan[-1]
    return phan[:-1], duoi


def gop_dong(dong, duoi):
    return b"\r\n".join(dong) + b"\r\n" + duoi


def dem_byte_cao(b):
    return sum(1 for x in b if x >= 0x80)


def kiem_be_rong(ds, ncot, ten, bao):
    """KTabFile::CreateTabOffset (KTabFile.cpp:143-177):
       - hang NGAN hon m_Width thi dong 159-168 BE GAY vong va nhoi o rong -> KHONG lam lech bang.
       - hang DAI hon m_Width thi KHONG co duong thoat: doc du m_Width o roi dong 172 khong
         thay CRLF nen KHONG nhay dong -> o thu m_Width tro di bi coi la hang KE TIEP
         -> LECH CA BANG tu do tro di.  => chi DAI moi la loi chet nguoi."""
    dai = [(k + 1, len(d.split(b"\t"))) for k, d in enumerate(ds)
           if len(d.split(b"\t")) > ncot]
    if dai:
        raise LoiVa("%s: co hang DAI hon %d o (se lam lech ca bang): %r"
                    % (ten, ncot, dai[:10]))
    ngan = [(k + 1, len(d.split(b"\t"))) for k, d in enumerate(ds)
            if len(d.split(b"\t")) < ncot]
    if ngan:
        bao.append("  luu y: %d hang NGAN hon %d o (co san tu truoc, KTabFile tu nhoi o rong): %r"
                   % (len(ngan), ncot, ngan[:10]))


def sao_luu(p):
    bak = p + HAUTO
    if os.path.exists(bak):
        return False, bak
    shutil.copy2(p, bak)
    return True, bak


# ---------------------------------------------------------------- npcs.txt

def dung_7_hang(bao):
    """Doc npcs.txt Linux, tra (dict id -> bytes hang JX1 87 o, header Linux)."""
    if not os.path.isfile(LINUX_NPCS):
        raise LoiVa("khong thay npcs.txt Linux: %s" % LINUX_NPCS)
    blx = doc_byte(LINUX_NPCS)
    dong, _ = tach_dong(blx, "npcs.txt Linux")
    hdr = dong[0].split(b"\t")
    if len(hdr) != LINUX_COT:
        raise LoiVa("npcs.txt Linux: dong dau co %d cot, cho %d" % (len(hdr), LINUX_COT))

    v2o = nap_vn2octal()
    ket = {}
    for nid in ID_GHIDE:
        r = nid + 2                      # dong 1-based
        if r - 1 >= len(dong):
            raise LoiVa("npcs.txt Linux khong co dong %d (id %d)" % (r, nid))
        o = dong[r - 1].split(b"\t")
        if len(o) != LINUX_COT:
            raise LoiVa("npcs.txt Linux dong %d (id %d): %d cot, cho %d"
                        % (r, nid, len(o), LINUX_COT))
        o = o[:JX1_COT]                  # CAT 16 cot duoi
        if nid in TEN_DE:
            ten_cu = o[0]
            o[0] = v2o.unicode_to_tcvn3_bytes(TEN_DE[nid])
            bao.append("  id %d: doi ten '%s' -> '%s' (TCVN3, %d byte)"
                       % (nid, hien(ten_cu), hien(o[0]), len(o[0])))
        for k, v in enumerate(o):
            if b"\t" in v or b"\r" in v or b"\n" in v:
                raise LoiVa("id %d cot %d chua tab hoac ky tu xuong dong" % (nid, k))
        hang = b"\t".join(o)
        if hang.count(b"\t") != JX1_COT - 1:
            raise LoiVa("id %d: hang dung xong co %d tab, cho %d"
                        % (nid, hang.count(b"\t"), JX1_COT - 1))
        ket[nid] = hang
    return ket, hdr


def kiem_dau_bang(hdr_lx, hdr_jx):
    """87 ten cot dau cua Linux phai TRUNG TEN TRUNG THU TU voi JX1."""
    if len(hdr_jx) != JX1_COT:
        raise LoiVa("npcs.txt JX1: dong dau co %d cot, cho %d" % (len(hdr_jx), JX1_COT))
    lech = [(k, hdr_lx[k], hdr_jx[k]) for k in range(JX1_COT) if hdr_lx[k] != hdr_jx[k]]
    if lech:
        raise LoiVa("87 cot dau Linux/JX1 KHONG trung ten: %r" % (lech[:5],))


def byte_cao_ngoai_7(ds):
    """Dem byte >=0x80 o moi dong TRU 7 dong se ghi de."""
    bo = set(i + 1 for i in ID_GHIDE)
    tong = 0
    for k, d in enumerate(ds):
        if k in bo:
            continue
        tong += dem_byte_cao(d)
    return tong


def va_npcs(duong, hang_moi, hdr_lx, chi_kiem, bao):
    ten = duong
    b0 = doc_byte(duong)
    dong, duoi = tach_dong(b0, ten)
    if duoi != b"":
        raise LoiVa("%s: khong ket thuc bang CRLF (duoi=%r)" % (ten, duoi[:20]))
    so_dong_truoc = len(dong)
    hdr_jx = dong[0].split(b"\t")
    kiem_dau_bang(hdr_lx, hdr_jx)

    # canh gac: chup byte 4 hang da co san
    gac_truoc = {}
    for i in ID_CANHGAC:
        if i + 1 >= len(dong):
            raise LoiVa("%s: thieu dong %d cho id canh gac %d" % (ten, i + 2, i))
        gac_truoc[i] = dong[i + 1]
        o = gac_truoc[i].split(b"\t")
        if len(o) != JX1_COT or all(x == b"0" for x in o):
            raise LoiVa("%s: id %d (dong %d) khong phai hang da co san" % (ten, i, i + 2))

    trang_thai = {}
    can_ghi = False
    for nid in ID_GHIDE:
        r = nid + 2
        if r - 1 >= len(dong):
            raise LoiVa("%s: khong co dong %d (id %d)" % (ten, r, nid))
        cu = dong[r - 1]
        o = cu.split(b"\t")
        if len(o) != JX1_COT:
            raise LoiVa("%s dong %d (id %d): %d o, cho %d" % (ten, r, nid, len(o), JX1_COT))
        if cu == hang_moi[nid]:
            trang_thai[nid] = "da-va-roi"
        elif all(x == b"0" for x in o):
            trang_thai[nid] = "hang-rong"
            can_ghi = True
        else:
            raise LoiVa("%s dong %d (id %d): hang KHONG rong va KHONG khop ban va "
                        "(ten hien tai=%r) -> dung lai, khong de bua"
                        % (ten, r, nid, hien(o[0])))

    cao_truoc = byte_cao_ngoai_7(dong)
    lech_byte = 0
    for nid in ID_GHIDE:
        lech_byte += len(hang_moi[nid]) - len(dong[nid + 1])

    bao.append("  trang thai: %s" % ", ".join("%d=%s" % (i, trang_thai[i]) for i in ID_GHIDE))

    if chi_kiem or not can_ghi:
        bao.append("  khong ghi (%s)" % ("che do --kiem" if chi_kiem else "da du, idempotent"))
        if not can_ghi:
            doc_lai_npcs(duong, hang_moi, bao)
        return trang_thai, False

    moi, bak = sao_luu(duong)
    bao.append("  sao luu: %s%s" % (bak, "" if moi else "  (da co tu truoc, giu nguyen)"))

    for nid in ID_GHIDE:
        dong[nid + 1] = hang_moi[nid]

    b1 = gop_dong(dong, duoi)

    # ---- assert TRUOC khi ghi
    d2, du2 = tach_dong(b1, ten)
    if len(d2) != so_dong_truoc:
        raise LoiVa("%s: so dong doi %d -> %d" % (ten, so_dong_truoc, len(d2)))
    if du2 != b"":
        raise LoiVa("%s: mat CRLF cuoi tep" % ten)
    kiem_be_rong(d2, JX1_COT, ten, bao)
    for nid in ID_GHIDE:
        if len(d2[nid + 1].split(b"\t")) != JX1_COT:
            raise LoiVa("%s: hang id %d khong dung %d o" % (ten, nid, JX1_COT))
    cao_sau = byte_cao_ngoai_7(d2)
    if cao_sau != cao_truoc:
        raise LoiVa("%s: so byte cao (>=0x80) NGOAI 7 hang bi doi %d -> %d"
                    % (ten, cao_truoc, cao_sau))
    if len(b1) - len(b0) != lech_byte:
        raise LoiVa("%s: lech byte tong %d != tong lech 7 hang %d"
                    % (ten, len(b1) - len(b0), lech_byte))
    for i in ID_CANHGAC:
        if d2[i + 1] != gac_truoc[i]:
            raise LoiVa("%s: id canh gac %d (dong %d) BI DOI" % (ten, i, i + 2))

    ghi_byte(duong, b1)
    bao.append("  GHI XONG: %d byte -> %d byte; so dong giu nguyen %d; byte cao ngoai 7 hang %d"
               % (len(b0), len(b1), so_dong_truoc, cao_sau))
    doc_lai_npcs(duong, hang_moi, bao)
    return trang_thai, True


def doc_lai_npcs(duong, hang_moi, bao):
    """Doc lai tu dia va tra TEN o dung 7 dong -- khong tin so dong trong bo nho."""
    dong, duoi = tach_dong(doc_byte(duong), duong)
    if duoi != b"":
        raise LoiVa("%s: doc lai -> khong ket thuc bang CRLF" % duong)
    for nid in ID_GHIDE:
        r = nid + 2
        cu = dong[r - 1]
        if cu != hang_moi[nid]:
            raise LoiVa("%s: doc lai dong %d (id %d) KHONG khop ban va" % (duong, r, nid))
        o = cu.split(b"\t")
        bao.append("    doc lai  dong %-5d id=%-5d ten=%-30s res=%s"
                   % (r, nid, hien(o[0]), o[11].decode("latin-1")))
    for i in ID_CANHGAC:
        o = dong[i + 1].split(b"\t")
        bao.append("    canh gac dong %-5d id=%-5d ten=%-30s (khong dung toi)"
                   % (i + 2, i, hien(o[0])))


# ------------------------------------------------ npc_res_kind_file_name.txt

def va_res(duong, chi_kiem, bao):
    b0 = doc_byte(duong)
    dong, duoi = tach_dong(b0, duong)
    if duoi != b"":
        raise LoiVa("%s: khong ket thuc bang CRLF" % duong)
    hdr = dong[0].split(b"\t")
    if len(hdr) != RES_COT:
        raise LoiVa("%s: dong dau %d o, cho %d" % (duong, len(hdr), RES_COT))

    chi_muc = {}
    for k, d in enumerate(dong):
        c = d.split(b"\t")
        if c:
            chi_muc.setdefault(c[0], k)

    them = []
    for ten_moi, ten_khuon in RES_THEM:
        kb = ten_moi.encode("ascii")
        if kb in chi_muc:
            bao.append("  '%s' DA CO o dong %d -> bo qua" % (ten_moi, chi_muc[kb] + 1))
            continue
        kk = ten_khuon.encode("ascii")
        if kk not in chi_muc:
            raise LoiVa("%s: khong thay hang khuon '%s'" % (duong, ten_khuon))
        khuon = dong[chi_muc[kk]].split(b"\t")
        if len(khuon) != RES_COT:
            raise LoiVa("%s: hang khuon '%s' co %d o, cho %d"
                        % (duong, ten_khuon, len(khuon), RES_COT))
        moi = list(khuon)
        moi[0] = kb
        # cot 2 = ResFilePath: chi doi doan cuoi (ten thu muc tai nguyen)
        p = khuon[2].split(b"\\")
        if p[-1] != kk:
            raise LoiVa("%s: ResFilePath cua '%s' khong ket thuc bang chinh ten (%r)"
                        % (duong, ten_khuon, khuon[2]))
        p[-1] = kb
        moi[2] = b"\\".join(p)
        them.append((ten_moi, ten_khuon, b"\t".join(moi)))

    if not them:
        bao.append("  du ca 2 hang, khong phai ghi (idempotent)")
        doc_lai_res(duong, bao)
        return False
    if chi_kiem:
        for ten_moi, ten_khuon, hang in them:
            bao.append("  SE THEM '%s' (khuon '%s'): %s"
                       % (ten_moi, ten_khuon, hang.rstrip(b"\t").decode("latin-1")))
        return False

    moi_bak, bak = sao_luu(duong)
    bao.append("  sao luu: %s%s" % (bak, "" if moi_bak else "  (da co tu truoc, giu nguyen)"))

    so_truoc = len(dong)
    cu_chup = list(dong)
    for ten_moi, ten_khuon, hang in them:
        dong.append(hang)
    b1 = gop_dong(dong, duoi)

    d2, du2 = tach_dong(b1, duong)
    if len(d2) != so_truoc + len(them):
        raise LoiVa("%s: so dong %d -> %d, cho %d"
                    % (duong, so_truoc, len(d2), so_truoc + len(them)))
    if du2 != b"":
        raise LoiVa("%s: mat CRLF cuoi tep" % duong)
    kiem_be_rong(d2, RES_COT, duong, bao)
    for k in range(so_truoc, len(d2)):
        if len(d2[k].split(b"\t")) != RES_COT:
            raise LoiVa("%s: hang moi them (dong %d) khong dung %d o" % (duong, k + 1, RES_COT))
    if dem_byte_cao(b1) != dem_byte_cao(b0):
        raise LoiVa("%s: so byte cao (>=0x80) bi doi %d -> %d"
                    % (duong, dem_byte_cao(b0), dem_byte_cao(b1)))
    if d2[:so_truoc] != cu_chup:
        raise LoiVa("%s: phan cu bi dong toi" % duong)

    ghi_byte(duong, b1)
    bao.append("  GHI XONG: %d dong -> %d dong (+%d); %d byte -> %d byte"
               % (so_truoc, len(d2), len(them), len(b0), len(b1)))
    doc_lai_res(duong, bao)
    return True


def doc_lai_res(duong, bao):
    dong, _ = tach_dong(doc_byte(duong), duong)
    con = {}
    for k, d in enumerate(dong):
        c = d.split(b"\t")
        if c:
            con.setdefault(c[0], (k + 1, d))
    for ten_moi, _kh in RES_THEM:
        kb = ten_moi.encode("ascii")
        if kb not in con:
            raise LoiVa("%s: doc lai KHONG thay '%s'" % (duong, ten_moi))
        k, d = con[kb]
        bao.append("    doc lai  res dong %-5d %s" % (k, d.rstrip(b"\t").decode("latin-1")))


# ---------------------------------------------------------------- chay

def tao_thu(dich):
    """Chep 3 tep tu cay chay that ra <dich> de chay thu."""
    cap = [
        (os.path.join(GOC_THAT, "server", "settings", "npcs.txt"),
         os.path.join(dich, "server", "settings", "npcs.txt")),
        (os.path.join(GOC_THAT, "client", "settings", "npcs.txt"),
         os.path.join(dich, "client", "settings", "npcs.txt")),
        (os.path.join(GOC_THAT, "client", "settings", "NpcRes", "npc_res_kind_file_name.txt"),
         os.path.join(dich, "client", "settings", "NpcRes", "npc_res_kind_file_name.txt")),
        # hai bang duoi KHONG bi va, chep de b5a_kiem_ktabfile.py kiem duoc
        # (KNpcResNode::Init nhanh NormalNpc con mot FindRow bat buoc tren chung)
        (os.path.join(GOC_THAT, "client", "settings", "NpcRes", "npc_normal_res_file.txt"),
         os.path.join(dich, "client", "settings", "NpcRes", "npc_normal_res_file.txt")),
        (os.path.join(GOC_THAT, "client", "settings", "NpcRes", "npc_normal_spr_info_file.txt"),
         os.path.join(dich, "client", "settings", "NpcRes", "npc_normal_spr_info_file.txt")),
    ]
    for a, b in cap:
        os.makedirs(os.path.dirname(b), exist_ok=True)
        shutil.copy2(a, b)
        print("chep %s\n  -> %s" % (a, b))
    return 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--goc", default=GOC_THAT,
                    help="thu muc chua server\\ va client\\ (mac dinh: cay chay that)")
    ap.add_argument("--kiem", action="store_true", help="chi kiem tra, khong ghi")
    ap.add_argument("--tao-thu", dest="tao_thu", default=None,
                    help="chep 3 tep tu cay that ra thu muc nay roi thoat")
    ns = ap.parse_args()

    if ns.tao_thu:
        return tao_thu(ns.tao_thu)

    bao = []
    try:
        bao.append("GOC = %s%s" % (ns.goc, "   [--kiem]" if ns.kiem else ""))
        bao.append("")
        bao.append("[1] Dung 7 hang tu npcs.txt Linux (cat %d -> %d cot)" % (LINUX_COT, JX1_COT))
        hang_moi, hdr_lx = dung_7_hang(bao)
        for nid in ID_GHIDE:
            o = hang_moi[nid].split(b"\t")
            bao.append("  id %-5d dong %-5d ten=%-30s Kind=%-3s Camp=%-3s res=%s"
                       % (nid, nid + 2, hien(o[0]), o[1].decode("latin-1"),
                          o[2].decode("latin-1"), o[11].decode("latin-1")))

        bao.append("")
        bao.append("[2] npcs.txt (server + client)")
        for nhanh in ("server", "client"):
            p = os.path.join(ns.goc, nhanh, "settings", "npcs.txt")
            if not os.path.isfile(p):
                raise LoiVa("khong thay %s" % p)
            bao.append("  --- %s" % p)
            va_npcs(p, hang_moi, hdr_lx, ns.kiem, bao)

        # server va client phai giong nhau o 11 hang quan tam
        ps = os.path.join(ns.goc, "server", "settings", "npcs.txt")
        pc = os.path.join(ns.goc, "client", "settings", "npcs.txt")
        ds, _ = tach_dong(doc_byte(ps), ps)
        dc, _ = tach_dong(doc_byte(pc), pc)
        for i in ID_GHIDE + ID_CANHGAC:
            if ds[i + 1] != dc[i + 1]:
                raise LoiVa("server/client lech o id %d (dong %d)" % (i, i + 2))
        bao.append("  server va client GIONG HET o ca 11 hang %s" % (ID_GHIDE + ID_CANHGAC,))

        bao.append("")
        bao.append("[3] client/settings/NpcRes/npc_res_kind_file_name.txt")
        pr = os.path.join(ns.goc, "client", "settings", "NpcRes", "npc_res_kind_file_name.txt")
        if not os.path.isfile(pr):
            raise LoiVa("khong thay %s" % pr)
        va_res(pr, ns.kiem, bao)

    except LoiVa as e:
        for d in bao:
            print(d)
        print("")
        print("*** LOI VA: %s" % e)
        return 2

    for d in bao:
        print(d)
    print("")
    print("XONG%s." % (" (che do --kiem, khong ghi gi)" if ns.kiem else ""))
    return 0


if __name__ == "__main__":
    sys.exit(main())
