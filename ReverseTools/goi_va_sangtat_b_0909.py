# -*- coding: utf-8 -*-
"""goi_va_sangtat_b_0909.py - [SANGTAT 09/09 b] Khong tao den dong (KLightBase) khi ban do sang tat.

Ho so tick 15:5x (DOLUOT, map cung build): KIpoTree::PluckRto 6,3 % + cac ham iterator list<KLightBase*> ~8 % = ~14 % tick.
Co che: KIpoTree::AddLeafPoint tao 'new KLightBase' cho moi NPC/dan co IPOT_RL_LIGHT_PROP va push vao m_LightList;
moi lan MoveObject (~1000/tick luc dong) PluckRto DUYET CA m_LightList de tim den cua la -> O(so den) moi lan + new/delete.
m_LightList chi dung cho RenderLightMap (da bo qua khi BanDoSang=0, [SANGTAT 09/09]) va Breathe (hoat anh den).
Sua (dung y het khi BanDoSang=0 - mac dinh): (1) chi tao den dong khi BanDoSang(); (2) dem den dong m_nDenDong; PluckRto chi duyet
khi m_nDenDong > 0 (den tinh cua map van giu, RenderLightMap khi BanDoSang=1 khong doi).
"""
import io
import sys

NL = "\r\n"
T = "\t"
D = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/Scene/"
F_C = D + "KIpoTree.cpp"
F_H = D + "KIpoTree.h"
TAG = "[SANGTAT 09/09 b]"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, s.count("\r\n"), s.count("\n") - s.count("\r\n"), sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, crlf0, lf0, h0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


def rep_sau(s, moc, old, new, ten):
    if s.count(moc) != 1:
        print("FAIL moc %s: %d" % (ten, s.count(moc))); sys.exit(1)
    a = s.find(moc); p = s.find(old, a)
    if p < 0:
        print("FAIL neo %s" % ten); sys.exit(1)
    return s[:p] + new + s[p + len(old):]


s, c0, l0, h0 = doc(F_H)
if TAG not in s:
    s = rep(s, T + "list<KLightBase*> m_LightList;" + NL,
            T + "list<KLightBase*> m_LightList;" + NL + T + "int m_nDenDong;" + T + "// " + TAG + " so den DONG (cua NPC/dan) trong m_LightList; 0 = PluckRto khong can duyet" + NL,
            "H bien")
    ghi(F_H, s, c0, l0, h0, "KIpoTree.h")
else:
    print("KIpoTree.h da co")

s, c0, l0, h0 = doc(F_C)
if TAG not in s:
    # ctor
    s = rep(s, "KIpoTree::KIpoTree()" + NL + "{" + NL + T + "m_pMainBranch = NULL;",
            "KIpoTree::KIpoTree()" + NL + "{" + NL + T + "m_pMainBranch = NULL;" + NL + T + "m_nDenDong = 0;" + T + "// " + TAG,
            "C ctor")
    # AddLeafPoint: chi tao den dong khi BanDoSang()
    s = rep_sau(s, "void KIpoTree::AddLeafPoint(KIpotLeaf* pLeaf)",
                T*2 + "if (pRtoLeaf->eLayerParam & IPOT_RL_LIGHT_PROP)",
                T*2 + "if ((pRtoLeaf->eLayerParam & IPOT_RL_LIGHT_PROP) && BanDoSang())" + T + "// " + TAG + " ban do sang tat -> khong tao den dong (chi RenderLightMap/Breathe dung)",
                "C AddLeafPoint dieu kien")
    s = rep_sau(s, "void KIpoTree::AddLeafPoint(KIpotLeaf* pLeaf)",
                T*5 + "m_LightList.push_back(pLight);",
                T*5 + "m_LightList.push_back(pLight); m_nDenDong++;" + T + "// " + TAG,
                "C AddLeafPoint dem")
    # PluckRto: bo qua khi khong co den dong
    old = NL.join([
        "void KIpoTree::PluckRto(KIpotRuntimeObj* pLeaf)",
        "{",
        T + "pLeaf->Pluck();",
        T + "list<KLightBase*>::iterator i;",
        T + "for (i = m_LightList.begin(); i != m_LightList.end(); ++i)",
        T + "{",
        T*2 + "if((*i)->m_pParent == pLeaf)",
        T*2 + "{",
        T*3 + "delete (*i);",
        T*3 + "m_LightList.erase(i);",
        T*3 + "break;",
        T*2 + "}",
        T + "}",
        "}",
    ])
    new = NL.join([
        "void KIpoTree::PluckRto(KIpotRuntimeObj* pLeaf)",
        "{",
        T + "pLeaf->Pluck();",
        T + "if (m_nDenDong <= 0)" + T + "// " + TAG + " khong co den dong nao -> khoi duyet m_LightList moi lan di chuyen (~1000 lan/tick luc dong)",
        T*2 + "return;",
        T + "list<KLightBase*>::iterator i;",
        T + "for (i = m_LightList.begin(); i != m_LightList.end(); ++i)",
        T + "{",
        T*2 + "if((*i)->m_pParent == pLeaf)",
        T*2 + "{",
        T*3 + "delete (*i);",
        T*3 + "m_LightList.erase(i);",
        T*3 + "m_nDenDong--;" + T + "// " + TAG,
        T*3 + "break;",
        T*2 + "}",
        T + "}",
        "}",
    ])
    s = rep(s, old, new, "C PluckRto")
    # Clear / Fell: dat lai bo dem khi xoa danh sach (m_LightList.clear() trong Clear)
    s = rep(s, T + "m_LightList.clear();", T + "m_LightList.clear(); m_nDenDong = 0;" + T + "// " + TAG, "C Clear")
    ghi(F_C, s, c0, l0, h0, "KIpoTree.cpp")
else:
    print("KIpoTree.cpp da co")
print("XONG SANGTAT b")
