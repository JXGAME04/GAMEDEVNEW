# -*- coding: utf-8 -*-
"""vW_removequiting_bot.py - VA GOC THAT cua "spam => Acc [] Player [] thoat game
character quiting" vo han.

DUONG TRUY (moi buoc deu doc ma, khong doan):
  1. Vong lap KSOServer.cpp:3605-3675 moi nhip:
        else if (IsCharacterQuiting(nIndex)) { ... RemoveQuitingPlayer(nIndex); ... in log }
  2. `CoreServerShell::IsCharacterQuiting` (CoreServerShell.cpp:1292-1300)
        return Player[nIndex].IsWaitingRemove();
     `KPlayer::IsWaitingRemove` (KPlayer.cpp:999-1004)
        if (!m_dwID) return FALSE;  return m_bIsQuiting;
  3. `CoreServerShell::RemoveQuitingPlayer` (CoreServerShell.cpp:252-261)
        if (Player[nIndex].IsWaitingRemove()) PlayerSet.RemoveQuiting(nIndex);
  4. >>> CHO HONG <<< `KPlayerSet::RemoveQuiting` (KPlayerSet.cpp:452-453):
        if (!Player[nIndex].m_bForeQuit &&
            (Player[nIndex].m_nNetConnectIdx == -1 || Player[nIndex].m_dwID == 0))
            return;                       // THOAT SOM, KHONG GO GI CA
     Khe BOT khong co ket noi mang nen m_nNetConnectIdx == -1, va m_bForeQuit = FALSE
     => return ngay => co m_bIsQuiting KHONG bao gio duoc go
     => IsWaitingRemove() van TRUE => nhip sau lai vao => in lai => VO HAN.
     Ten rong trong log vi day la khe bot (khong co ten/tai khoan).

BANG CHUNG PHU (bac bo gia thuyet truoc do cua chinh toi):
  Mieng va vV (dem so lan luu hong roi go khe) DA CHAY DUNG ban tren dia
  (bam GameServer.exe khop), nhung `GameServer.log` co 2077 dong spam va
  0 dong "GO KHE" => nhanh do KHONG he chay => `SavePlayerData` KHONG thất bại.
  Vay goc khong nam o duong luu, ma o duong GO KHE. Chu thich
  KPlayerBot.cpp:1228-1232 mo ta mot duong ket KHAC (qua Save) - dung ve co che
  nhung khong phai truong hop dang gap.

MIENG VA (mot dieu kien, toi thieu):
  Cho phep GO khi khe DANG THOAT that su. `IsWaitingRemove()` da bao dam
  `m_dwID != 0` VA `m_bIsQuiting == TRUE` - tuc khe co that va dang thoat, chinh
  la truong hop dang bi ket. Khe rong (m_dwID == 0) van bi chan nhu cu vi
  IsWaitingRemove() tra FALSE.
  Nguoi that KHONG doi hanh vi: ho co m_nNetConnectIdx >= 0 nen von da khong roi
  vao nhanh return nay.

Tep dich: Sources\\Core\\Src\\KPlayerSet.cpp  => build lai CoreServer.dll
(KPlayerSet.cpp dung chung client/server nen build CA HAI cau hinh cho chac).

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_goket lan dau).
"""
import io
import os
import shutil
import sys

DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerSet.cpp"
HAU_TO = ".truoc_goket"

T = "\t"
NL = "\n"

CU = (
    T + "if (!Player[nIndex].m_bForeQuit && (Player[nIndex].m_nNetConnectIdx == -1 || Player[nIndex].m_dwID == 0))" + NL +
    T*2 + "return;" + NL
)

MOI = (
    T + "// [LOREN 27/08] Them `&& !IsWaitingRemove()`: khe DANG THOAT that su thi" + NL +
    T + "// PHAI go duoc, khong duoc thoat som." + NL +
    T + "// Truoc day khe BOT (m_nNetConnectIdx == -1, m_bForeQuit = FALSE) roi vao" + NL +
    T + "// nhanh return nay => co m_bIsQuiting khong bao gio duoc go =>" + NL +
    T + "// IsCharacterQuiting van TRUE => vong lap KSOServer.cpp:3637 vao lai moi" + NL +
    T + "// nhip => spam \"=> Acc [] Player [] thoat game character quiting <=\"" + NL +
    T + "// vo han (ten rong vi la khe bot)." + NL +
    T + "// An toan: IsWaitingRemove() da doi m_dwID != 0 VA m_bIsQuiting == TRUE," + NL +
    T + "// nen khe rong van bi chan nhu cu; nguoi that co m_nNetConnectIdx >= 0" + NL +
    T + "// nen von khong di qua nhanh nay - hanh vi khong doi." + NL +
    T + "if (!Player[nIndex].m_bForeQuit && !Player[nIndex].IsWaitingRemove()" + NL +
    T*2 + "&& (Player[nIndex].m_nNetConnectIdx == -1 || Player[nIndex].m_dwID == 0))" + NL +
    T*2 + "return;" + NL
)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vW_removequiting_bot - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if "[LOREN 27/08] Them `&& !IsWaitingRemove()`" in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    cu = CU.replace(NL, eol)
    dem = raw.count(cu)
    if dem != 1:
        print("!!! LOI TO: moc neo xuat hien %d lan (can dung 1) - KHONG ghi gi" % dem)
        return 1
    moi = raw.replace(cu, MOI.replace(NL, eol), 1)

    hi1 = sum(1 for c in moi if ord(c) > 127)
    if hi1 != hi0:
        print("!!! LOI TO: byte cao %d -> %d" % (hi0, hi1))
        return 1
    if moi.count("{") != raw.count("{") or moi.count("}") != raw.count("}"):
        print("!!! LOI TO: so ngoac nhon doi - KHONG ghi gi")
        return 1
    print("  moc neo trung 1 lan | byte cao %d (khong doi) | CRLF %d -> %d | ngoac khong doi"
          % (hi0, raw.count("\r\n"), moi.count("\r\n")))

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % sao)
    with io.open(DICH, "wb") as f:
        f.write(moi.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != moi:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI + doc lai tu dia: khop.")
    print("\n  => build lai Core CA HAI cau hinh, dat CoreServer.dll")
    return 0


if __name__ == "__main__":
    sys.exit(main())
