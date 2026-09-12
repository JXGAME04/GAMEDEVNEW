# -*- coding: utf-8 -*-
"""[PAK 12/09 CHISOMUC] Sua "mot so cay mat hinh, chi con bong": chi so muc trong pak bi cat con 16 bit.

NGUYEN NHAN GOC (chung minh khop den tung bit bang log may that):
  XPackFile.cpp:721  SprGetHeader cat chi so muc xuong WORD:
        *((WORD*)&pSpr->Reserved[NODE_INDEX_STORE_IN_RESERVED]) = (WORD)ElemRef.nElemIndex;
  XPackFile.cpp:745  GetSprFrame doc lai cung bang WORD.
  WORD chi chua duoc 0..65535. mobile_13.pak co 70 602 muc, nen 5 067 tep nam sau muc thu
  65 536 deu bi doc NHAM sang mot muc khac:
        cay bo de (\<game>\<my thuat>\thanh pho\Luong Ho\ngoai troi\cay\bo de 3.spr)
        o chi so 69 604 -> nhet vao 16 bit con 4 068
        muc 4 068 co co nen 0x01000376, KHONG co TYPE_FRAME (0x10000000)
        -> GetSprFrame tra NULL -> PrepareFrameData tra false -> GetImage tra NULL
        -> lop ve bo qua, chi con bong cua cay.
  Log tren iPhone bao dung 0x01000376 - khop chinh xac voi muc 4 068 tinh tu pak.
  Vi sao khong ai thay bao loi: ca ba cho tren deu tra NULL/false MA KHONG GHI LOG NAO.

VI SAO CHI "MOT SO" CAY: chi mobile_13.pak vuot 65 535 muc (70 602); 13 pak con lai deu duoi.
Nen chi tep trong pak do, va chi nhung tep nam sau muc 65 536, moi hong.

CACH SUA: cat chi so lam hai nua, giu nua thap o Reserved[2] NHU CU va them nua cao vao
Reserved[4] (o nay dang trong).
  KHONG duoc dung int 32 bit tai Reserved[2] nhu ZSPRPackFile.cpp:335 lam, vi nhu the se de len
  Reserved[3], ma Reserved[3] dang giu KHOA GIAI MA BANG MAU (TextureRes.cpp:559).
  Cac o dang dung: [0] chi so pak, [1] co m_bNew, [2] chi so muc, [3] khoa bang mau. [4] va [5] trong.

KHONG RAO THEO NEN, va day la co y: day la loi that cua ca ba ban. Voi moi pak duoi 65 536 muc
thi nua cao luon bang 0 -> gia tri doc ra Y HET truoc, khong doi mot chut nao. Ban Android dung
chung chinh mobile_13.pak nen cung dang dinh loi nay; ban PC dung bo pak khac nen chua lo ra.
"""
import os, sys

TEP = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                   'Sources', 'Engine', 'Src', 'XPackFile.cpp')
MOC = b'PAK 12/09 CHISOMUC'

CU_GHI = b'\t\t\t\t*((WORD*)&pSpr->Reserved[NODE_INDEX_STORE_IN_RESERVED]) = (WORD)ElemRef.nElemIndex;\n'
MOI_GHI = (b'\t\t\t\t// [PAK 12/09 CHISOMUC] chi so muc CO THE vuot 65535 (mobile_13.pak co 70 602 muc) nen mot WORD\n'
           b'\t\t\t\t// khong du: nua thap giu o Reserved[2] nhu cu, nua cao de o Reserved[4] (o trong).\n'
           b'\t\t\t\t// KHONG dung int 32 bit o Reserved[2] vi se de len Reserved[3] = khoa giai ma bang mau.\n'
           b'\t\t\t\t*((WORD*)&pSpr->Reserved[NODE_INDEX_STORE_IN_RESERVED]) = (WORD)(ElemRef.nElemIndex & 0xFFFF);\n'
           b'\t\t\t\tpSpr->Reserved[NODE_INDEX_HI_IN_RESERVED] = (WORD)(((unsigned int)ElemRef.nElemIndex >> 16) & 0xFFFF);\n')

CU_DOC = b'\t\tint nNodeIndex = *((WORD*)&pSprHeader->Reserved[NODE_INDEX_STORE_IN_RESERVED]);\n'
MOI_DOC = (b'\t\t// [PAK 12/09 CHISOMUC] ghep lai hai nua; pak duoi 65 536 muc thi nua cao = 0 -> y het truoc\n'
           b'\t\tint nNodeIndex = (int)(*((WORD*)&pSprHeader->Reserved[NODE_INDEX_STORE_IN_RESERVED])\n'
           b'\t\t                       | ((unsigned int)pSprHeader->Reserved[NODE_INDEX_HI_IN_RESERVED] << 16));\n')

CU_DN = b'#define\tNODE_INDEX_STORE_IN_RESERVED\t2\n'
MOI_DN = (CU_DN +
          b'// [PAK 12/09 CHISOMUC] nua CAO cua chi so muc. Reserved[3] da bi khoa giai ma bang mau chiem\n'
          b'// (TextureRes.cpp:559) nen phai dung o [4]; [4] va [5] hien khong ai dung.\n'
          b'#define\tNODE_INDEX_HI_IN_RESERVED\t4\n')

def main():
    d = open(TEP, 'rb').read()
    if MOC in d:
        print('da va roi'); return 0
    for cu, moi, ten in ((CU_DN, MOI_DN, 'dinh nghia o cao'), (CU_GHI, MOI_GHI, 'cho ghi'), (CU_DOC, MOI_DOC, 'cho doc')):
        n = d.count(cu)
        if n != 1:
            print('LOI: %s - %d cho khop, can 1' % (ten, n)); return 1
        d = d.replace(cu, moi, 1)
    open(TEP, 'wb').write(d)
    print('da va %s' % TEP)
    return 0

sys.exit(main())
