# -*- coding: utf-8 -*-
"""z5_xepdo_syncfix.py - VA "xep do xong item xuat hien lai / doi hinh / mat Tho Dia Phu".

CHUOI NHAN QUA (moi mat xich deu doc tan ma, khong doan):
  1. Moi lan SO LUONG CHONG cua mot item doi (tru stack: KItemList.cpp:5104
     RemoveItemIdx; gop chong tran: KItemList.cpp:203 AddKIL), server goi
     SyncItem(nIdx) MOT tham so => goi ITEM_SYNC voi m_bIsNew=false, place=0.
     (Da thong ke: moi loi goi SyncItem nhieu tham so deu bIsNew=true; goi
     bIsNew=false CHI mang y nghia "cap nhat thuoc tinh".)
  2. Client KProtocolProcess.cpp s2cSyncItem nhanh bIsNew=false (:1644-1648):
        nIndex = ItemSet.SearchID(...)        // CHI SO ITEMSET
        AddKIL(nIndex, m_Items[nIndex].nPlace, m_Items[nIndex].nX, ...)
     - m_Items[] la mang SLOT cua KItemList, tra bang chi so ItemSet la SAI
       KHONG GIAN chi so => doc du lieu RAC (dung chu cua chu game);
     - item DA co entry trong list ma AddKIL THEM LAN NUA: slot rac tung dung
       thuong mang pos_equiproom + (x,y) cu -> PlaceItem vao o dang TRONG la
       an => item co HAI entry + chiem HAI vung grid CLIENT.
  3. He auto (ban rac DT_TimMonRac, cat do buoc 5 Hau can, dem thuoc...) quet
     GRID CLIENT bang FindItem(x,y) -> gap o ma -> doc Item[idx] (ruot hien
     hanh cua slot, co the DA TAI CAP cho mon khac sau chuoi remove/add cua
     lo ren) -> gui lenh theo GetID() -> server thi hanh len MON THAT KHAC:
     ban mat "Tho Dia Phu vo han" (2 lan, mat that), cat nham, "doi hinh".
  4. Bam XEP DO: server AutoArrangeItem Remove+AddKIL ca tui (dwID moi);
     entry ma client khong bi xoa (s2c_removeitem SearchID xoa entry dau)
     => sau xep do item ma "xuat hien lai".

MIENG VA:
  A. (GOC) client s2cSyncItem: nhanh bIsNew=false KHONG AddKIL nua - goi cap
     nhat chi duoc phep doi thuoc tinh (StackNum...). UI so chong van dung vi
     ve tu Item[].GetStackNum (xua nay AddKIL-rac da phan fail som ma so chong
     van cap nhat binh thuong).
  B. (PHONG THU) server KPlayer::AutoArrangeItem: khu trung nIdx trong vItems/
     vSpecIt truoc khi xep - grid rach (mot item o 2 o roi) se duoc xep ve MOT
     cho thay vi khuech dai thanh 2 entry that. Du lieu sach thi khong doi gi.

Tep dich:
  - Sources/Core/Src/KProtocolProcess.cpp  (ham client s2cSyncItem)
  - Sources/Core/Src/KPlayer.cpp           (AutoArrangeItem, #ifdef _SERVER)
File CHUNG Core => build CA HAI cau hinh (client Win32 + server x64), dat canh
cho chu swap; CAM tu restart (luat 28/08).
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_xepdo lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[XEPDO 28/08]"
HAU_TO = ".truoc_xepdo"
KPP = r"D:\GAMEDEVNEW\Sources\Core\Src\KProtocolProcess.cpp"
KPL = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayer.cpp"

# ---------------- hunk A: client s2cSyncItem ----------------
A_CU = [
    T*2 + "if (pItemSync->m_bIsNew)",
    T*3 + "Player[CLIENT_PLAYER_INDEX].m_ItemList.AddKIL(nIndex, pItemSync->m_btPlace, pItemSync->m_btX, pItemSync->m_btY);",
    T*2 + "else",
    T*3 + "Player[CLIENT_PLAYER_INDEX].m_ItemList.AddKIL(nIndex,",
    T*4 + "Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nIndex].nPlace,",
    T*4 + "Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nIndex].nX,",
    T*4 + "Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nIndex].nY);",
]
A_MOI = [
    T*2 + "if (pItemSync->m_bIsNew)",
    T*3 + "Player[CLIENT_PLAYER_INDEX].m_ItemList.AddKIL(nIndex, pItemSync->m_btPlace, pItemSync->m_btX, pItemSync->m_btY);",
    T*2 + "// " + NHAN + " Goi bIsNew=false la CAP NHAT thuoc tinh (server chi gui no",
    T*2 + "// tu SyncItem(nIdx) mot tham so khi so luong chong doi - place/x/y trong",
    T*2 + "// goi la 0). Ban cu AddKIL them lan nua voi m_Items[nIndex] - nIndex la",
    T*2 + "// chi so ITEMSET tra vao mang SLOT cua KItemList = doc du lieu RAC, lam",
    T*2 + "// item co 2 entry + 2 vung grid client. Auto (ban rac / cat ruong / xep",
    T*2 + "// do) quet grid gap o ma roi gui lenh theo dwID cua ruot hien hanh =>",
    T*2 + "// ban/cat nham mon that (su co 28/08: mat Tho Dia Phu vo han x2, item",
    T*2 + "// 'doi hinh', xep do xong item 'xuat hien lai'). Thuoc tinh da duoc gan",
    T*2 + "// het o khoi tren; item van dung nguyen cho cu - KHONG AddKIL gi them.",
]

# ---------------- hunk B: server AutoArrangeItem dedupe ----------------
B_CU = [
    "	memset(pnCmpArray, 0, sizeof(int)*EQUIPMENT_ROOM_WIDTH*EQUIPMENT_ROOM_HEIGHT);",
    "	int i;",
    "	int itemCount = (int)vItems.size();",
]
B_MOI = [
    "	memset(pnCmpArray, 0, sizeof(int)*EQUIPMENT_ROOM_WIDTH*EQUIPMENT_ROOM_HEIGHT);",
    "	int i;",
    "	// " + NHAN + " KHU TRUNG: grid rach (mot item ghi o 2 o roi - FindItem khong",
    "	// nhan ra vi hai o khong ke nhau) lam item vao danh sach xep 2 lan =>",
    "	// Remove x2 (lan 2 fail) + AddKIL x2 = item co 2 entry THAT tren server.",
    "	// Xep do phai tu chua grid rach chu khong duoc khuech dai no.",
    "	{",
    "		size_t nGiu = 0;",
    "		for (size_t k1 = 0; k1 < vItems.size(); ++k1)",
    "		{",
    "			bool bTrung = false;",
    "			for (size_t k2 = 0; k2 < nGiu && !bTrung; ++k2)",
    "				bTrung = (vItems[k2].nIdx == vItems[k1].nIdx);",
    "			if (!bTrung)",
    "				vItems[nGiu++] = vItems[k1];",
    "		}",
    "		if (nGiu < vItems.size())",
    "			vItems.resize(nGiu);",
    "		nGiu = 0;",
    "		for (size_t k1 = 0; k1 < vSpecIt.size(); ++k1)",
    "		{",
    "			bool bTrung = false;",
    "			for (size_t k2 = 0; k2 < nGiu && !bTrung; ++k2)",
    "				bTrung = (vSpecIt[k2].nIdx == vSpecIt[k1].nIdx);",
    "			if (!bTrung)",
    "				vSpecIt[nGiu++] = vSpecIt[k1];",
    "		}",
    "		if (nGiu < vSpecIt.size())",
    "			vSpecIt.resize(nGiu);",
    "	}",
    "	int itemCount = (int)vItems.size();",
]


def va(p, cu, moi):
    raw = io.open(p, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    if NHAN in raw:
        print("  %s: DA VA - bo qua (idempotent)" % os.path.basename(p))
        return None
    d = raw.split(eol)
    vt = [i for i in range(len(d) - len(cu) + 1)
          if [x.rstrip() for x in d[i:i + len(cu)]] == [x.rstrip() for x in cu]]
    if len(vt) != 1:
        print("!!! LOI TO: %s neo khop %d lan (can 1)" % (os.path.basename(p), len(vt)))
        return False
    i = vt[0]
    d[i:i + len(cu)] = moi
    nd = eol.join(d)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi (%s)" % os.path.basename(p))
        return False
    if (nd.count("{") - raw.count("{")) != (nd.count("}") - raw.count("}")):
        print("!!! LOI TO: ngoac lech (%s)" % os.path.basename(p))
        return False
    print("  %s: dong %d, %+d dong | byte cao %d (khong doi)"
          % (os.path.basename(p), i + 1, len(moi) - len(cu), hi0))
    return nd


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== z5_xepdo_syncfix - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    viec = []
    for p, cu, moi in ((KPP, A_CU, A_MOI), (KPL, B_CU, B_MOI)):
        r = va(p, cu, moi)
        if r is False:
            return 1
        if r:
            viec.append((p, r))
    if not viec:
        print("Khong co gi de lam.")
        return 0
    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    for p, nd in viec:
        sao = p + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
            print("  sao luu -> %s" % os.path.basename(sao))
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if io.open(p, "rb").read().decode("latin-1") != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %s" % os.path.basename(p))
    print("\n=> BUILD CA HAI cau hinh (client Win32 + server x64), dat canh cho chu swap.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
