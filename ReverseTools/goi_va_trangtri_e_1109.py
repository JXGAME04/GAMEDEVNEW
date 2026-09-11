# -*- coding: utf-8 -*-
r"""goi_va_trangtri_e_1109.py - [TRANGTRI 11/09 e] chua goc "hong 3" trong dong do:

Do duoc tren may dang chay (jx_paint.log):
  [TRANGTRI] 10s: npc them 1 hong 3 thieu_anh 3 xoa 0 | obj them 2 | khoi ve 1426 bo qua 0
  -> khoi DA VE THAT (1.426 lan/10 s, bo qua 0; chu xac nhan "khoi da co")
  -> nhung 3 tren 7 ban ghi NPC bi "hong", tuc KNpcSet::AddClientNpc tra 0.

AddClientNpc tra 0 chi o mot cho: `SubWorld[0].Mps2Map(...)` cho `nRegion < 0`, nghia la
toa do cua NPC KHONG roi vao vung nao DANG NAP. Nguyen nhan la THU TU trong
KSubWorld::LoadMap: vung tam duoc Load + LoadObject NGAY (dong 2416), roi moi den vong
nap 8 vung lan can (2428-2453). Ban ghi NPC nam sat mep vung co the tinh ra vung ben
canh - luc do vung do CHUA NAP -> FindRegion tra -1 -> mat con vat.

Sua: ghi nho cac o vua nap roi goi LoadObject SAU KHI du cua so 3x3. Goi lai tren vung
da nap la vo hai (SearchClientID thay ban cu nen bo qua).
"""
import io
import sys

NL = "\r\n"
T = "\t"
TAG = "[TRANGTRI 11/09 e]"
P = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/KSubWorld.cpp"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, sum(1 for c in s if ord(c) >= 0x80), s.count("\n") - s.count("\r\n")


def ghi(p, s, h0, lf0, ten):
    if (s.count("\n") - s.count("\r\n")) != lf0 or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten)
        sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten, n_mong=1):
    n = s.count(old)
    if n != n_mong:
        print("FAIL neo %s: %d (mong %d)" % (ten, n, n_mong))
        sys.exit(1)
    return s.replace(old, new)


s, h0, lf0 = doc(P)
if TAG in s:
    print("da co")
    sys.exit(0)

# 1. khai bao bang ghi nho, dat ngay truoc `if (nIdx < 0)`
s = rep(s, T + "if (nIdx < 0)" + NL + T + "{" + NL + T*2 + "nIdx = m_ClientRegionIdx[0];" + NL,
        T + "// " + TAG + " hoan viec nap NPC/vat the trang tri den khi du cua so 3x3:" + NL +
        T + "// ban ghi sat mep vung co the tinh ra vung ben canh, ma luc nap vung tam thi" + NL +
        T + "// 8 vung lan can CHUA co -> Mps2Map tra -1 -> AddClientNpc bo (dong do: hong 3)." + NL +
        T + "int nTTO[9], nTTX[9], nTTY[9], nTTSo = 0;" + NL +
        T + "if (nIdx < 0)" + NL + T + "{" + NL + T*2 + "nIdx = m_ClientRegionIdx[0];" + NL, "khai bao")

# 2. vung tam: ghi nho thay vi goi ngay
s = rep(s, T*3 + "m_Region[nIdx].LoadObject(0, nX, nY, m_szMapPath);" + T + "// [TRANGTRI 11/09] nap NPC+OBJ trang tri cua vung" + NL,
        T*3 + "nTTO[nTTSo] = nIdx; nTTX[nTTSo] = nX; nTTY[nTTSo] = nY; nTTSo++;" + T + "// " + TAG + NL, "vung tam")

# 3. vung lan can: ghi nho thay vi goi ngay
s = rep(s, T*4 + "m_Region[nConIdx].LoadObject(0, nX + nXOff[i], nY + nYOff[i], m_szMapPath);" + T + "// [TRANGTRI 11/09]" + NL,
        T*4 + "nTTO[nTTSo] = nConIdx; nTTX[nTTSo] = nX + nXOff[i]; nTTY[nTTSo] = nY + nYOff[i]; nTTSo++;" + T + "// " + TAG + NL, "vung lan can")

# 4. goi that sau khi du cua so
neo = T*2 + "m_ClientRegionIdx[i + 1] = nConIdx;" + NL + T*2 + "m_Region[nIdx].m_nConnectRegion[i] = nConIdx;" + NL + T + "}" + NL
s = rep(s, neo, neo + NL +
        T + "// " + TAG + " gio moi nap vat trang tri: ca 9 vung deu co mat nen Mps2Map khong con tra -1." + NL +
        T + "for (int nT11 = 0; nT11 < nTTSo; nT11++)" + NL +
        T*2 + "m_Region[nTTO[nT11]].LoadObject(0, nTTX[nT11], nTTY[nT11], m_szMapPath);" + NL, "goi sau")

ghi(P, s, h0, lf0, "KSubWorld.cpp")
print("XONG " + TAG)
