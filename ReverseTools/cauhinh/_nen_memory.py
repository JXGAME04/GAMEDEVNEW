# -*- coding: utf-8 -*-
"""_nen_memory.py - nen chi muc MEMORY.md ve duoi 17 KB.

Chi rut NGAN tung dong (giu MOT dong moi muc, giu nguyen ten tep dich va cac
tu khoa quan trong), khong xoa muc nao. Cach lam: voi cac dong dai qua nguong,
cat phan duoi sau dau ") - " (phan chu thich phu) va giu tron phan tieu de +
lien ket.

CHI SUA MEMORY.md. Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

P = (r"C:\Users\nguye\.claude\projects"
     r"\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\memory\MEMORY.md")
NGUONG = 140          # byte: dong dai hon thi rut
DICH = 17000          # byte muc tieu


def byte(s):
    return len(s.encode("utf-8"))


def rut(l, tran_tieu_de=None):
    """Cat phan duoi sau lien ket, va (neu can) rut ngan ca TIEU DE.

    Chi tiet day du van nam nguyen trong tung tep chu de - day chi la chi muc.
    Luon giu: dau moc/ngay o dau tieu de, va nguyen ven lien ket (...).
    """
    m = re.match(r"^- \[(.*)\]\(([^)]+)\)(.*)$", l, re.S)
    if not m:
        return l
    tieu_de, dich, duoi = m.group(1), m.group(2), m.group(3).strip()
    if tran_tieu_de and len(tieu_de) > tran_tieu_de:
        cat = tieu_de[:tran_tieu_de]
        k = cat.rfind(" ")
        if k > tran_tieu_de * 0.6:
            cat = cat[:k]
        if cat.count("**") % 2:
            cat = cat + "**"
        if cat.count("`") % 2:
            cat = cat + "`"
        tieu_de = cat.rstrip(" ,.·—-") + "…"
    dau = "- [" + tieu_de + "](" + dich + ")"
    if duoi.startswith("—") and byte(duoi) <= 80 and not tran_tieu_de:
        return dau + " " + duoi
    return dau


def main():
    ghi = "--ghi" in sys.argv[1:]
    raw = io.open(P, encoding="utf-8").read()
    L = raw.split("\n")
    print("truoc: %d byte, %d dong" % (byte(raw), len(L)))
    # rut dan tu dong dai nhat cho toi khi dat dich
    n = 0
    # Vong 1: bo phan chu thich sau lien ket o cac dong dai.
    for i in sorted(range(len(L)), key=lambda i: -byte(L[i])):
        if byte("\n".join(L)) <= DICH:
            break
        if byte(L[i]) <= NGUONG:
            continue
        m = rut(L[i])
        if m != L[i]:
            L[i] = m
            n += 1
    # Vong 2: neu van chua dat, rut ngan dan TIEU DE (chi tiet van o tep chu de).
    for tran in (220, 180, 150, 120, 100, 85):
        if byte("\n".join(L)) <= DICH:
            break
        for i in sorted(range(len(L)), key=lambda i: -byte(L[i])):
            if byte("\n".join(L)) <= DICH:
                break
            if byte(L[i]) <= NGUONG:
                continue
            m = rut(L[i], tran)
            if m != L[i]:
                L[i] = m
                n += 1
    nd = "\n".join(L)
    print("sau  : %d byte (rut %d dong)" % (byte(nd), n))
    if byte(nd) > DICH:
        print("  (chua dat %d byte - nhung da rut het dong dai)" % DICH)
    # chot: khong mat muc nao
    def so_muc(s):
        return len([x for x in s.split("\n") if x.startswith("- ")])
    if so_muc(nd) != so_muc(raw):
        print("!!! LOI TO: so muc doi (%d -> %d)" % (so_muc(raw), so_muc(nd)))
        return 1
    # chot: khong mat lien ket .md nao
    lk0 = set(re.findall(r"\(([\w-]+\.md)\)", raw))
    lk1 = set(re.findall(r"\(([\w-]+\.md)\)", nd))
    if lk0 - lk1:
        print("!!! LOI TO: mat lien ket: %s" % sorted(lk0 - lk1)[:5])
        return 1
    print("  giu nguyen %d muc va %d lien ket .md" % (so_muc(nd), len(lk1)))
    if not ghi:
        print("DIEN TAP - chua ghi.")
        return 0
    shutil.copy2(P, P + ".truoc_nen")
    io.open(P, "w", encoding="utf-8", newline="").write(nd)
    print("DA GHI MEMORY.md")
    return 0


if __name__ == "__main__":
    sys.exit(main())
