# -*- coding: ascii -*-
# FIX-6: auto BO MUC TIEU khi ap sat MAI KHONG TOI (chua "chay ra ngoai roi huc tuong ~30s").
# Chua ap - se ap cung luc voi ban build gop cuoi phien test.
import io, sys
ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
CRLF = "\r\n"
def L(*a): return CRLF.join(a)
ok = True
def ap(fname, old, new, note):
    global ok
    p = ROOT + "\\" + fname
    c = io.open(p, 'r', encoding='latin-1', newline='').read()
    hb = sum(1 for ch in c if ord(ch) > 127)
    n = c.count(old)
    if n != 1:
        print("LOI [%s]: neo khop %d lan" % (note, n)); ok = False; return
    c = c.replace(old, new)
    if sum(1 for ch in c if ord(ch) > 127) != hb:
        print("LOI [%s]: high-byte doi" % note); ok = False; return
    io.open(p, 'w', encoding='latin-1', newline='').write(c)
    print("OK [%s]" % note)

# --- nhanh PK (Tong Kim / Lien Dau / Bang Chien): sau khi tinh nDist, truoc khi quyet dinh
ap("CoreShell.cpp",
L("\t\t\t\t\tif(pApData->bPKFollowTG)",
  "\t\t\t\t\t{",
  "\t\t\t\t\t\tif(pApData->bPKAppr && !bCastState)"),
L("\t\t\t\t\t// [FIX-6 26/08] BO MUC TIEU KHONG TOI DUOC.",
  "\t\t\t\t\t// Chu game: 'di chuyen ra ngoai roi chay vao tuong khoang 30s roi moi di chuyen",
  "\t\t\t\t\t// A* binh thuong lai'. Do that: thoi gian 'di nhieu ma khong toi dau' tang tu",
  "\t\t\t\t\t// 2,8-5,3% len 7,0% sau FIX-3 - vi FIX-3 lam may danh chon AP SAT nhieu hon,",
  "\t\t\t\t\t// ma may danh KHONG HE co buoc bo cuoc: muc tieu nam sau tuong / khong co duong",
  "\t\t\t\t\t// toi thi no dam vao tuong mai. Day la LO HONG CO SAN (khau chon muc tieu khong",
  "\t\t\t\t\t// kiem duong toi), FIX-3 chi lam lo ra.",
  "\t\t\t\t\t// Luat: dang ap sat mot muc tieu ma qua 4 giay khoang cach KHONG gan them duoc",
  "\t\t\t\t\t// (>= 1 o) thi loai muc tieu do 30 giay va chon con khac - dung khuon san co cua",
  "\t\t\t\t\t// [FIGHT-SKIPGOLD]. Chi kich hoat khi that su khong tien bo nen khong dong vao",
  "\t\t\t\t\t// truong hop danh binh thuong.",
  "\t\t\t\t\t{",
  "\t\t\t\t\t\tstatic UINT  s_uS9ApID = 0;\t// muc tieu dang ap sat",
  "\t\t\t\t\t\tstatic DWORD s_uS9ApT  = 0;\t// moc lan cuoi CO tien bo",
  "\t\t\t\t\t\tstatic int   s_nS9ApD  = 0;\t// khoang cach tot nhat da dat",
  "\t\t\t\t\t\tif (nDist < nSkillRadius)",
  "\t\t\t\t\t\t{",
  "\t\t\t\t\t\t\ts_uS9ApID = 0;\t\t\t// danh duoc roi - khong con ap sat",
  "\t\t\t\t\t\t}",
  "\t\t\t\t\t\telse if (s_uS9ApID != Npc[nTGNpcIdx].m_dwID)",
  "\t\t\t\t\t\t{",
  "\t\t\t\t\t\t\ts_uS9ApID = Npc[nTGNpcIdx].m_dwID; s_uS9ApT = uCurTime; s_nS9ApD = nDist;",
  "\t\t\t\t\t\t}",
  "\t\t\t\t\t\telse if (nDist + 32 < s_nS9ApD)",
  "\t\t\t\t\t\t{",
  "\t\t\t\t\t\t\ts_nS9ApD = nDist; s_uS9ApT = uCurTime;\t// con gan them duoc - gia han",
  "\t\t\t\t\t\t}",
  "\t\t\t\t\t\telse if (uCurTime - s_uS9ApT > 4000)",
  "\t\t\t\t\t\t{",
  "\t\t\t\t\t\t\tAUTOLOG(\"[S9-BOMUCTIEU] tgID=%u d=%d tot nhat=%d qua 4s khong gan them -> loai 30s\", Npc[nTGNpcIdx].m_dwID, nDist, s_nS9ApD);",
  "\t\t\t\t\t\t\tPlayer[nPlayerIdx].m_mAutoExcludeNpcID[Npc[nTGNpcIdx].m_dwID] = uCurTime + 30000;",
  "\t\t\t\t\t\t\tPlayer[nPlayerIdx].m_sExtAuto.uNpcID = 0;",
  "\t\t\t\t\t\t\ts_uS9ApID = 0;",
  "\t\t\t\t\t\t\treturn 0;",
  "\t\t\t\t\t\t}",
  "\t\t\t\t\t}",
  "\t\t\t\t\tif(pApData->bPKFollowTG)",
  "\t\t\t\t\t{",
  "\t\t\t\t\t\tif(pApData->bPKAppr && !bCastState)"), "FIX-6 bo muc tieu khong toi duoc (nhanh PK)")

sys.exit(0 if ok else 1)
