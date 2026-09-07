#!/usr/bin/env python3
# -*- coding: ascii -*-
"""
goi_va_npcs_aimaxtime_0609.py

[AIMAX 06/09] Them cot AIMaxTime con thieu vao settings/npcs.txt (may chu + khach).

Vi sao thieu:
  Engine JX1 DA doc cot nay - KNpcTemplate.cpp:136
      g_NpcSetting.GetInteger(nNpcTempRow, "AIMaxTime", 25, (int*)&nAIMaxTime);
  nhung npcs.txt cua du an KHONG CO cot do => moi con quai roi ve mac dinh 25.
  m_NextAITime = m_dwCurrentTime + m_AIMAXTime, ma m_dwCurrentTime la BO DEM
  KHUNG HINH (KSubWorld.cpp:1107, 18 khung/giay) => moi quai chi ra quyet dinh
  1 lan moi 25/18 = 1,39 giay.
  Ban Linux (D:/ServerLinux/server1/settings/npcs.txt) co cot AIMaxTime va dat
  rieng tung con: 3 / 6 / 9 / 10 / 12 / 18 / 24 / 36 khung.

Cach lay so - THAN TRONG:
  Hai bang xep CUNG THU TU DONG (da kiem: dong 727 'Dau ong dia' o ca hai ban).
  Chi chep AIMaxTime cho dong nao co DAU VAN TAY AI TRUNG KHIT giua hai ban:
      AIMode, AIParam1..9, VisionRadius, ActiveRadius, WalkSpeed, RunSpeed,
      Kind, Camp, Series
  Dong khong trung (du an tu chinh, hoac NPC rieng cua JX1) -> de TRONG
  => engine dung mac dinh 25 nhu cu, KHONG doi hanh vi.

Chay lai duoc (idempotent). Doc/ghi latin-1, giu nguyen EOL CRLF va byte TCVN3.

LUI: xoa tep dich va doi ten <ten>.truoc_aimaxtime_0609 tro lai.
"""
import io
import os
import shutil

LINUX = r"D:\ServerLinux\server1\settings\npcs.txt"
DICH = [
    r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\npcs.txt",
    r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\settings\npcs.txt",
]
COT = "AIMaxTime"
VANTAY = ["AIMode", "AIParam1", "AIParam2", "AIParam3", "AIParam4", "AIParam5",
          "AIParam6", "AIParam7", "AIParam8", "AIParam9",
          "VisionRadius", "ActiveRadius", "WalkSpeed", "RunSpeed",
          "Kind", "Camp", "Series"]


def tach(p):
    raw = io.open(p, "r", encoding="latin-1", newline="").read()
    nl = "\r\n" if "\r\n" in raw else "\n"
    lines = raw.split(nl)
    return raw, nl, lines


def o(row, idx, c):
    j = idx.get(c, -1)
    return (row[j] if 0 <= j < len(row) else "").strip()


def main():
    rawL, nlL, linesL = tach(LINUX)
    hdrL = linesL[0].split("\t")
    idxL = {h: i for i, h in enumerate(hdrL)}
    assert COT in idxL, "ban Linux khong co cot " + COT
    rowsL = [l.split("\t") for l in linesL[1:] if l.strip()]
    print("ban Linux: %d cot, %d dong" % (len(hdrL), len(rowsL)))

    for p in DICH:
        raw, nl, lines = tach(p)
        hdr = lines[0].split("\t")
        if COT in hdr:
            print("%s: da co cot %s, bo qua" % (os.path.basename(os.path.dirname(os.path.dirname(p))), COT))
            continue
        idx = {h: i for i, h in enumerate(hdr)}
        ncot = len(hdr)

        b = p + ".truoc_aimaxtime_0609"
        if not os.path.exists(b):
            shutil.copyfile(p, b)

        ra = [lines[0] + "\t" + COT]
        chep = 0
        bo = 0
        for k, l in enumerate(lines[1:]):
            if not l.strip():
                ra.append(l)
                continue
            row = l.split("\t")
            assert len(row) == ncot, "dong %d co %d o, header %d o" % (k + 2, len(row), ncot)
            v = ""
            if k < len(rowsL):
                a = [o(rowsL[k], idxL, c) for c in VANTAY]
                bb = [o(row, idx, c) for c in VANTAY]
                if a == bb:
                    lv = o(rowsL[k], idxL, COT)
                    if lv:
                        v = lv
                        chep += 1
                    else:
                        bo += 1
                else:
                    bo += 1
            else:
                bo += 1
            ra.append(l + "\t" + v)

        moi = nl.join(ra)
        # kiem byte cao khong doi (chi them ky tu ASCII)
        hb_t = sum(1 for c in raw if ord(c) >= 0x80)
        hb_s = sum(1 for c in moi if ord(c) >= 0x80)
        assert hb_t == hb_s, "so byte cao doi %d -> %d" % (hb_t, hb_s)
        assert "\xef\xbf\xbd" not in moi, "xuat hien FFFD"
        io.open(p, "w", encoding="latin-1", newline="").write(moi)
        print("%s : them cot %s | chep %d dong | de trong %d dong (dung mac dinh 25)"
              % (p, COT, chep, bo))


if __name__ == "__main__":
    main()
