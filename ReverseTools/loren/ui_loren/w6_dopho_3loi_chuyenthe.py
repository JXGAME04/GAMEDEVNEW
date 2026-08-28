# -*- coding: utf-8 -*-
"""w6_dopho_3loi_chuyenthe.py - VA 3 LOI cua the Do pho vua thi cong.

CHU GAME:
  (1) "kham nam chuyen qua do pho roi do pho chuyen qua kham nam lai thi khong
       hien o kham nam"
  (2) "roi gio da bo do vao de ep do pho - hkmp chua?"

BA LOI, deu doc ra tu ma (khong doan):

LOI 1 - `ShowWindow` case 5 (Kham nam) THIEU an trang Do pho.
  Moi case khac (0,1,2,3,4) deu co du hai dong:
        m_pSelf->m_AtlasPad.Hide();
        m_pSelf->m_AtlasPadBtn.CheckButton(FALSE);
  rieng case 5 KHONG co. Vi truoc day AtlasPad chua ton tai nen khong ai thay.
  Hau qua dung nhu chu game ta: tu Do pho quay lai Kham nam thi trang Do pho
  VAN NAM DE LEN -> khong con thay o kham nam.

LOI 2 - `ShowWindow` case 6 (Do pho) THIEU `UpdateData()`.
  Moi case khac deu ket bang `<Pad>.UpdateData();` de nap lai noi dung o khi mo
  the. Case 6 khong co -> mo the Do pho khong nap lai o.

LOI 3 - `KUiCompoundItem::UpdateItem` THIEU `case UOC_ATLAS_ITEM`.
  Ham nay phan phoi tin "o vua doi" ve dung trang. Cac container COMPONE /
  COMPTWO / COMPTHREE / DISTILL / FORGE / ENCHASE deu co nhanh rieng, khong co
  nhanh cho ATLAS => KEO DO VAO O DO PHO THI DO KHONG HIEN LEN.
  => Tra loi cau hoi (2) cua chu game: CHUA bo do vao ep duoc, chinh vi loi nay.

MIENG VA: them dung nhung dong con thieu, theo dung khuon cac the khac.

Tep dich: Sources\\S3Client\\Ui\\UiCase\\UiCompoundItem.cpp => build S3Client.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_3loi lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 28/08] 3 loi the Do pho"
DICH = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiCompoundItem.cpp"
HAU_TO = ".truoc_3loi"


def K(*d):
    return list(d)


MIENG = [
    ("LOI 1: case 5 an trang Do pho",
     K(T*2 + "m_pSelf->m_EnchasePad.UpdateData();",
       T*2 + "break;",
       T + "case 6:"),
     K(T*2 + "// " + NHAN + " (1): case 5 THIEU hai dong an trang Do pho -",
       T*2 + "// moi case khac deu co. Thieu no thi tu Do pho quay lai Kham nam,",
       T*2 + "// trang Do pho VAN DE LEN => khong con thay o kham nam.",
       T*2 + "m_pSelf->m_AtlasPad.Hide();",
       T*2 + "m_pSelf->m_AtlasPadBtn.CheckButton(FALSE);",
       T*2 + "m_pSelf->m_EnchasePad.UpdateData();",
       T*2 + "break;",
       T + "case 6:")),

    ("LOI 2: case 6 nap lai o",
     K(T*2 + "m_pSelf->m_EnchasePad.Hide();",
       T*2 + "m_pSelf->m_EnchasePadBtn.CheckButton(FALSE);",
       T*2 + "break;",
       T + "}",
       "}"),
     K(T*2 + "m_pSelf->m_EnchasePad.Hide();",
       T*2 + "m_pSelf->m_EnchasePadBtn.CheckButton(FALSE);",
       T*2 + "// " + NHAN + " (2): case 6 THIEU UpdateData - moi case khac deu",
       T*2 + "// ket bang no de nap lai noi dung o khi mo the.",
       T*2 + "m_pSelf->m_AtlasPad.UpdateData();",
       T*2 + "break;",
       T + "}",
       "}")),

    ("LOI 3: UpdateItem thieu nhanh ATLAS",
     K(T*2 + "case UOC_ENCHASE_ITEM:",
       T*3 + "{",
       T*4 + "m_pSelf->m_EnchasePad.UpdateItem(pItem, bAdd);",
       T*3 + "}",
       T*3 + "break;",
       T*2 + "default:"),
     K(T*2 + "case UOC_ENCHASE_ITEM:",
       T*3 + "{",
       T*4 + "m_pSelf->m_EnchasePad.UpdateItem(pItem, bAdd);",
       T*3 + "}",
       T*3 + "break;",
       T*2 + "// " + NHAN + " (3): THIEU nhanh nay thi keo do vao o Do pho",
       T*2 + "// se KHONG hien len (tin \"o vua doi\" khong ve toi trang nao).",
       T*2 + "case UOC_ATLAS_ITEM:",
       T*3 + "{",
       T*4 + "m_pSelf->m_AtlasPad.UpdateItem(pItem, bAdd);",
       T*3 + "}",
       T*3 + "break;",
       T*2 + "default:")),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== w6_dopho_3loi_chuyenthe - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    n_crlf = raw.count("\r\n")
    n_lf = raw.count("\n") - n_crlf
    eol = "\r\n" if n_crlf >= n_lf else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    dong = raw.split(eol)
    for ten, cu, moi in MIENG:
        vt = [i for i in range(len(dong) - len(cu) + 1) if dong[i:i + len(cu)] == cu]
        if len(vt) != 1:
            print("  !!! %-34s khop %d lan (can 1)" % (ten, len(vt)))
            return 1
        i = vt[0]
        print("  ok  %-34s dong %d (+%d)" % (ten, i + 1, len(moi) - len(cu)))
        dong = dong[:i] + moi + dong[i + len(cu):]

    moi = eol.join(dong)
    if sum(1 for c in moi if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    if moi.count("{") - raw.count("{") != moi.count("}") - raw.count("}"):
        print("!!! LOI TO: ngoac lech")
        return 1
    # chot: moi Pad phai co du so lan Hide trong ShowWindow
    for pad in ("m_AtlasPad.Hide()", "m_EnchasePad.Hide()"):
        print("  chot: %-24s xuat hien %d lan" % (pad, moi.count(pad)))
    if moi.count("m_AtlasPad.Hide()") != moi.count("m_EnchasePad.Hide()"):
        print("!!! LOI TO: so lan an AtlasPad khac EnchasePad - van con the bo sot")
        return 1
    if moi.count("m_AtlasPad.UpdateData()") != 1:
        print("!!! LOI TO: m_AtlasPad.UpdateData() phai co dung 1 lan")
        return 1
    if moi.count("case UOC_ATLAS_ITEM:") != 1:
        print("!!! LOI TO: case UOC_ATLAS_ITEM phai co dung 1 lan")
        return 1
    print("  byte cao %d (khong doi) | ngoac can bang" % hi0)

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % os.path.basename(sao))
    with io.open(DICH, "wb") as f:
        f.write(moi.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != moi:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI + doc lai tu dia: khop.")
    print("\n  => build S3Client, dat lai Game.exe")
    return 0


if __name__ == "__main__":
    sys.exit(main())
