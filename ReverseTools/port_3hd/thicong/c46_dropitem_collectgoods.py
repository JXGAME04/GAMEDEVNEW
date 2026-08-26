# -*- coding: utf-8 -*-
"""C46 - chu game: "van bao loi item nhu tren hinh"
    KItemSet::AddItemSet2 khong tim thay nItemGenre=51584  (lap 3 lan)

TRUY RA GOC: 51584 = 1612 * 32. Chinh la "x * 32" bi doc thanh GENRE.
  script\\missions\\tong\\collectgoods\\schedule\\bombout.lua:59
      DropItem(world, x * 32, y * 32, -1, 6, 1, COLLG_GOODSIDX, 1, 0, 0, 0)
  Day la CHU KY LINUX (world,x,y,belong,genre,...) con sot lai. JX1:
      DropItem(nNpcIndex, genre, detail, particular, level, series, luck)
  => tham so 2 (= x*32) bi hieu la genre. Dung 3 lan vi nCaseCount = random(1,5).

KHONG PHAI 3 hoat dong dot nay: 3 tep duoi day duoc chep tu Linux hom 21/08
(dot Tin Su, moc sua 21/08 21:22) va chua ai doi chu ky. Cung mot lop loi.

CACH VA (giu dung y ban goc):
  * bombout.lua:59        - roi NGAY TAI CHO NGUOI CHOI (w,x,y = GetWorldPos()).
      JX1: DropItem(0, ...) - nIndex <= 0 thi engine tu lay Player[..].m_nIndex
      (ScriptFuns.cpp LuaDropItem) => dung vi tri nguoi choi. Khop 1-1.
  * fairy_smalltimer.lua:126 - roi TAI VI TRI NPC (x,y,subworld = GetNpcPos(idx)).
      JX1: DropItem(npcidxtab[i], ...) => dung vi tri NPC do. Khop 1-1.
  * npcpoint.lua:123      - roi tai TOA DO TU DO doc tu tep pos. Khong ep duoc
      vao DropItem cua JX1, dung DropItemEx da them cho dot nay (nhan MPS,
      cung khuon KNpc::DropItemFromLuaScript). GIU nguyen thu tu tham so goc.
      (Luu y: ca JX1 lan Linux deu THIEU settings\\maps\\dragonboatfestival_06\\
      elf_*.txt nen getndata tra nil => nhanh nay hien la ma chet; van sua cho
      dung de sau nay them tep pos vao la chay.)
"""
import io, os, sys, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"

SUA = [
    (r"script\missions\tong\collectgoods\schedule\bombout.lua",
     "\t\tDropItem(world, x * 32, y * 32, -1, 6, 1, COLLG_GOODSIDX, 1, 0, 0, 0);",
     [
         "\t\t-- [3HD 25/08 C46] chu ky Linux (world,x,y,belong,genre,...) => JX1 doc",
         "\t\t-- tham so 2 (x*32) LA GENRE: 'AddItemSet2 khong tim thay nItemGenre=51584'.",
         "\t\t-- JX1 la DropItem(nNpcIndex, genre, detail, particular, level, series, luck);",
         "\t\t-- nNpcIndex <= 0 thi engine lay chinh nguoi choi => dung y ban goc (roi",
         "\t\t-- ngay tai cho nguoi choi, vi w,x,y o tren la GetWorldPos cua nguoi choi).",
         "\t\tDropItem(0, 6, 1, COLLG_GOODSIDX, 1, 0, 0);",
     ]),
    (r"script\missions\tong\tong_springfestival\schedule\fairy_smalltimer.lua",
     "        nItemIdx = DropItem(subworld, x, y, -1, 6, 1, 1050, 1, 0, 0, 0);",
     [
         "        -- [3HD 25/08 C46] chu ky Linux => JX1 doc tham so 2 la GENRE. Ban goc",
         "        -- roi tai vi tri NPC (x,y,subworld lay tu GetNpcPos ngay tren) nen chi",
         "        -- can truyen thang chi so NPC do cho DropItem cua JX1.",
         "        nItemIdx = DropItem(npcidxtab[i], 6, 1, 1050, 1, 0, 0);",
     ]),
    (r"script\missions\tong\collectgoods\npcpoint.lua",
     "\t\tnItemIdx = DropItem(SubWorld, x, y, -1, 6, 1, 1086, 1, 0, 0, 0);",
     [
         "\t\t-- [3HD 25/08 C46] chu ky Linux => JX1 doc tham so 2 la GENRE. O day roi",
         "\t\t-- theo TOA DO TU DO (doc tu tep pos) nen khong ep vao DropItem cua JX1",
         "\t\t-- duoc; dung DropItemEx (nhan MPS) da them cho dot 3 hoat dong.",
         "\t\tnItemIdx = DropItemEx(SubWorld, x, y, -1, 0, 0, 0, 6, 1, 1086, 1, 0, 0);",
     ]),
]

for rel, cu, moi in SUA:
    p = os.path.join(JX1, rel)
    d = io.open(p, encoding="latin-1", newline="").read()
    if "[3HD 25/08 C46]" in d:
        print("  (da va)", rel.rsplit("\\", 1)[-1])
        continue
    if d.count(cu) != 1:
        print("  !! KHONG khop duy nhat (%d):" % d.count(cu), rel)
        continue
    nl = "\r\n" if "\r\n" in d else "\n"
    d = d.replace(cu, nl.join(moi))
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(p, dst)
    print("  OK", rel)
print("xong C46")
