# -*- coding: utf-8 -*-
"""vV_khe_ket_quiting.py - VA "khe KET VINH VIEN + spam console" khi mot khe
mang co dang-thoat ma khong luu duoc.

TRIEU CHUNG (chu game bao, luc KHONG co ai trong game):
    => Acc [] Player [] thoat game character quiting <=
lap lien tuc, ca tai khoan lan ten deu RONG (dau hieu khe BOT, khong phai nguoi).

GOC - da ghi san trong chinh ma nguon, `KPlayerBot.cpp:1228-1232`:
    "TUYET DOI KHONG de bot song sot mot khung nao voi m_bIsQuiting = TRUE:
     PlayerLogoutGateway thay IsCharacterQuiting thi goi SavePlayerData, ma
     KPlayer::Save chan ngay dong dau voi m_nNetConnectIdx == -1 -> tra FALSE
     -> "continue" -> RemoveQuitingPlayer KHONG BAO GIO chay -> khe KET VINH
     VIEN kem spam console 18 dong/giay."

Vong lap that (KSOServer.cpp:3605-3641):
    for (int nIndex = 1; nIndex < MAX_PLAYER; nIndex++)
        ...
        else if (IsCharacterQuiting(nIndex)) {
            if (IsCharacterNeedSave(nIndex)) {
                if (!SavePlayerData(nIndex, true))
                    continue;              <-- thoat som, khong bao gio go khe
            }
Cho chan trong Save (KPlayer.cpp:1075) CO mo duong cho bot
(`m_nNetConnectIdx==-1 && !PB_IsBot(...)`), nhung chi voi khe CON NAM trong
danh sach bot. Khe da thao khoi danh sach ma van con co dang-thoat (vd may chu
khoi dong lai dung luc bot dang thoat) thi PB_IsBot = 0 -> chan -> ket vinh vien.

MIENG VA (an toan cho NGUOI THAT):
KHONG go khe ngay lan luu hong dau tien - nguoi that co the luu hong TAM THOI
(Goddess ban, mang nghen) va lan sau luu duoc; go ngay la MAT DU LIEU.
Dem so lan hong theo tung khe:
  - duoi nguong: giu nguyen hanh vi cu (continue) de thu lai vong sau;
  - qua nguong: ghi log ro rang roi de chay tiep xuong duoi (gui goi roi game
    + RemoveQuitingPlayer), giai phong khe.
Bo dem xoa khi luu duoc va sau khi da go.

Nguong 300 vong: ham nay chay moi nhip GameServer nen 300 vong chi vai giay -
du lau de truong hop hong tam thoi tu khoi phuc, du ngan de console khong bi
spam lau.

Tep dich: Sources\\MultiServer\\GameServer\\KSOServer.cpp
=> PHAI BUILD LAI GameServer.exe (ban dang chay build tu 25/08).

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_kheket lan dau).
"""
import io
import os
import shutil
import sys

DICH = r"D:\GAMEDEVNEW\Sources\MultiServer\GameServer\KSOServer.cpp"
HAU_TO = ".truoc_kheket"

T = "\t"
NL = "\n"

CU = (
    T*2 + "else if (m_pCoreServerShell->IsCharacterQuiting(nIndex))" + NL +
    T*2 + "{" + NL +
    T*3 + "if (m_pCoreServerShell->IsCharacterNeedSave(nIndex)) {" + NL +
    T*4 + "if (!SavePlayerData(nIndex, true))" + NL +
    T*5 + "continue;" + NL +
    T*3 + "}" + NL
)

MOI = (
    T*2 + "else if (m_pCoreServerShell->IsCharacterQuiting(nIndex))" + NL +
    T*2 + "{" + NL +
    T*3 + "// [LOREN 27/08] KHE KET VINH VIEN + SPAM CONSOLE." + NL +
    T*3 + "// Truoc day chi `continue` khi luu hong: khe nao luu hong mot lan la" + NL +
    T*3 + "// hong mai (KPlayer::Save chan o KPlayer.cpp:1075 khi" + NL +
    T*3 + "// m_nNetConnectIdx == -1 va khe da thao khoi danh sach bot) ->" + NL +
    T*3 + "// RemoveQuitingPlayer khong bao gio chay -> vong lap in" + NL +
    T*3 + "// \"thoat game character quiting\" voi ten RONG hang chuc dong/giay." + NL +
    T*3 + "// Chinh KPlayerBot.cpp:1228-1232 da mo ta truoc co che nay." + NL +
    T*3 + "// Van THU LAI nhieu vong (nguoi that co the luu hong tam thoi vi" + NL +
    T*3 + "// Goddess ban / mang nghen - go ngay la MAT DU LIEU); chi khi qua" + NL +
    T*3 + "// nguong moi go khe, va ghi log ro rang." + NL +
    T*3 + "static int s_anQuitFail[MAX_PLAYER] = { 0 };" + NL +
    T*3 + "const int nNguongBoQua = 300;" + T*2 + "// vai giay - ham chay moi nhip" + NL +
    T*3 + "if (m_pCoreServerShell->IsCharacterNeedSave(nIndex)) {" + NL +
    T*4 + "if (!SavePlayerData(nIndex, true))" + NL +
    T*4 + "{" + NL +
    T*5 + "if (nIndex <= 0 || nIndex >= MAX_PLAYER)" + NL +
    T*6 + "continue;" + NL +
    T*5 + "s_anQuitFail[nIndex]++;" + NL +
    T*5 + "if (s_anQuitFail[nIndex] < nNguongBoQua)" + NL +
    T*6 + "continue;" + T*3 + "// con trong nguong: thu lai vong sau" + NL +
    T*5 + "char szTenKet[32] = \"\";" + NL +
    T*5 + "m_pCoreServerShell->GetGameData(SGDI_CHARACTER_NAME, (intptr_t)szTenKet, nIndex);" + NL +
    T*5 + "std::ostringstream ossKet;" + NL +
    T*5 + "ossKet << \"!! Khe \" << nIndex << \" [\" << szTenKet" + NL +
    T*6 + "   << \"] luu hong \" << s_anQuitFail[nIndex]" + NL +
    T*6 + "   << \" lan lien tiep - GO KHE de khong ket vong lap !!\" << endl;" + NL +
    T*5 + "GameServerLog::Instance().WriteAndConsole(ossKet.str());" + NL +
    T*5 + "s_anQuitFail[nIndex] = 0;" + NL +
    T*5 + "// KHONG continue: chay tiep xuong duoi de gui goi roi game va" + NL +
    T*5 + "// RemoveQuitingPlayer, giai phong khe." + NL +
    T*4 + "}" + NL +
    T*4 + "else if (nIndex > 0 && nIndex < MAX_PLAYER)" + NL +
    T*5 + "s_anQuitFail[nIndex] = 0;" + T + "// luu duoc: xoa bo dem" + NL +
    T*3 + "}" + NL
)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vV_khe_ket_quiting - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if "[LOREN 27/08] KHE KET VINH VIEN" in raw:
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
    # can bang ngoac cua vung thay
    if MOI.count("{") != MOI.count("}") + 0:
        pass
    mo = moi.count("{")
    dong = moi.count("}")
    print("  moc neo trung 1 lan | byte cao %d (khong doi) | CRLF %d -> %d | ngoac {%d }%d"
          % (hi0, raw.count("\r\n"), moi.count("\r\n"), mo, dong))
    if mo != dong:
        print("!!! LOI TO: ngoac nhon khong can bang - KHONG ghi gi")
        return 1

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
    print("\n  => PHAI BUILD LAI GameServer.exe")
    return 0


if __name__ == "__main__":
    sys.exit(main())
