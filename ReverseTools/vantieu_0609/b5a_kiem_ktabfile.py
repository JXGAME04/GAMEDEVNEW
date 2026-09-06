# -*- coding: ascii -*-
"""Mo phong DUNG KTabFile::CreateTabOffset (Engine/Src/KTabFile.cpp:81-178)
de kiem bang npcs.txt sau khi va: hang nao lech thi loi lo ra ngay."""
import os
import sys
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
from dec2 import decline2
try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass


def create_tab_offset(buf):
    n = len(buf)
    # --- do be rong tu DONG DAU (cpp:95-113)
    w = 1
    off = 0
    while buf[off] != 0x0d and buf[off] != 0x0a:
        if buf[off] == 0x09:
            w += 1
        off += 1
    off += 2 if (buf[off] == 0x0d and off + 1 < n and buf[off + 1] == 0x0a) else 1
    # --- dem so hang (cpp:114-134)
    h = 1
    while off < n:
        while off < n and buf[off] != 0x0d and buf[off] != 0x0a:
            off += 1
        h += 1
        if off + 1 < n and buf[off] == 0x0d and buf[off + 1] == 0x0a:
            off += 2
        else:
            off += 1
    # --- lap bang offset (cpp:143-177)
    tab = []
    off = 0
    for i in range(h):
        hang = []
        for j in range(w):
            start = off
            ln = 0
            while off < n and buf[off] != 0x09 and buf[off] != 0x0d and buf[off] != 0x0a:
                off += 1
                ln += 1
            off += 1                       # nhay qua 0x09 / 0x0d / 0x0a
            hang.append(buf[start:start + ln])
            prev = buf[off - 1] if 0 <= off - 1 < n else None
            if off >= n or prev == 0x0a or prev == 0x0d:
                hang.extend([b""] * (w - j - 1))
                break
        if off <= n and 0 <= off - 1 < n and buf[off - 1] == 0x0d and off < n and buf[off] == 0x0a:
            off += 1
        tab.append(hang)
    return w, h, tab


def main():
    goc = sys.argv[1]
    ids = [2145, 2146, 2147, 2148, 2156, 2157, 2158, 2230, 2231, 2232, 2233]
    for nhanh in ("server", "client"):
        p = "%s\\%s\\settings\\npcs.txt" % (goc, nhanh)
        buf = open(p, "rb").read()
        w, h, tab = create_tab_offset(buf)
        print("== %s  m_Width=%d m_Height=%d" % (p, w, h))
        # doi chieu voi ban luu goc NEU CO (chua va thi chua co .truoc_lmbc)
        if os.path.isfile(p + ".truoc_lmbc"):
            bak = open(p + ".truoc_lmbc", "rb").read()
            wb, hb, tb = create_tab_offset(bak)
            assert (w, h) == (wb, hb), "be rong/chieu cao doi: %r vs %r" % ((w, h), (wb, hb))
            cham = set(i + 1 for i in [2145, 2146, 2147, 2148, 2156, 2157, 2158])
            lech = [r for r in range(h) if r not in cham and tab[r] != tb[r]]
            assert not lech, "hang KHONG dinh toi bi doi: %r" % (lech[:10],)
            print("   %d hang, chi 7 hang muc tieu doi; %d hang con lai y het ban luu goc"
                  % (h, h - 7))
        else:
            print("   (chua co %s.truoc_lmbc -> bo qua doi chieu ban luu goc)"
                  % os.path.basename(p))
        for nid in ids:
            r = nid + 1              # 0-based trong tab; dong 1-based = nid+2
            o = tab[r]
            assert len(o) == w, "hang id %d chi co %d o" % (nid, len(o))
            print("   id=%-5d dong=%-5d Name=%-30s Kind=%-3s NpcResType=%s"
                  % (nid, nid + 2, decline2(o[0]), o[1].decode("latin-1"),
                     o[11].decode("latin-1")))
        # vai hang lan can de chac chan khong lech
        for nid in (2144, 2149, 2155, 2159, 2229, 2234, 2642):
            r = nid + 1
            if r < h:
                print("   [lan can] id=%-5d Name=%s" % (nid, decline2(tab[r][0])))

    # npc_res
    p = "%s\\client\\settings\\NpcRes\\npc_res_kind_file_name.txt" % goc
    buf = open(p, "rb").read()
    w, h, tab = create_tab_offset(buf)
    print("== %s  m_Width=%d m_Height=%d" % (p, w, h))
    can = {b"enemy243", b"passerby379", b"enemy241", b"enemy244",
           b"passerby180", b"passerby354a", b"passerby036_1"}
    thay = {}
    for r in range(h):
        if tab[r] and tab[r][0] in can:
            thay[tab[r][0]] = (r + 1, tab[r][2])
    for k in sorted(can):
        if k in thay:
            print("   OK  %-14s dong %-5d ResFilePath=%s"
                  % (k.decode(), thay[k][0], thay[k][1].decode("latin-1")))
        else:
            print("   THIEU %s" % k.decode())
    assert len(thay) == len(can), "thieu tai nguyen"

    # --- KNpcResNode::Init nhanh NPC_RES_NORMAL (KNpcResNode.cpp:356-383) con
    #     MOT FindRow BAT BUOC nua tren npc_normal_res_file.txt:
    #         if ( !NormalNpc.Load(NPC_NORMAL_RES_FILE)) return FALSE;
    #         nFindNo = NormalNpc.FindRow(lpszNpcName);
    #         if (nFindNo < 0) return FALSE;
    #     va nFindNo do duoc dung LAI tren npc_normal_spr_info_file.txt
    #     -> hai tep phai THANG HANG theo so dong.
    pn = "%s\\client\\settings\\NpcRes\\npc_normal_res_file.txt" % goc
    pi = "%s\\client\\settings\\NpcRes\\npc_normal_spr_info_file.txt" % goc
    wn, hn, tn = create_tab_offset(open(pn, "rb").read())
    wi, hi, ti = create_tab_offset(open(pi, "rb").read())
    print("== %s  m_Width=%d m_Height=%d" % (pn, wn, hn))
    print("== %s  m_Width=%d m_Height=%d" % (pi, wi, hi))
    assert hn == hi, "npc_normal_res_file / spr_info LECH SO DONG: %d vs %d" % (hn, hi)
    vt = {}
    for r in range(hn):
        if tn[r] and tn[r][0] in can:
            vt.setdefault(tn[r][0], r)
    thieu = [k.decode() for k in sorted(can) if k not in vt]
    assert not thieu, ("THIEU hang trong npc_normal_res_file.txt -> KNpcResNode::Init "
                       "tra FALSE -> NPC VO HINH: %r" % thieu)
    for k in sorted(can):
        r = vt[k]
        assert ti[r][0] == k, ("npc_normal_spr_info_file.txt dong %d la %r chu khong phai %r "
                               "(hai bang KHONG thang hang)" % (r + 1, ti[r][0], k))
        print("   OK  %-14s dong %-5d FightStand=%-22s info=%s"
              % (k.decode(), r + 1, tn[r][1].decode("latin-1"), ti[r][1].decode("latin-1")))

    print("\nMO PHONG KTabFile: KHONG LECH BANG.")


main()
