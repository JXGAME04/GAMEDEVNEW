# -*- coding: utf-8 -*-
"""[BIEN 04/09] GOC SAP CUA CLIENT: cac ham xu ly goi TIN TUONG do dai lay tu goi, khong kiem bien.
 Bang chung (jx_crash.log 04/09):
   00:32:40  memcpy + KPlayerChat::GetChat (KPlayerChat.cpp:678)  ECX=0x5080 = 20.608 byte chep vao szBuf[256]
   00:21:13  memcpy + KPlayerTeam::ReceiveInvite (KPlayerTeam.cpp:112) ECX=0xDB84 = 56.196 byte chep vao szName[32]
   00:08:06  strcat  + nhanh chat/extend
 Ca ba deu: do dai la truong trong goi (m_wSentenceLen WORD toi 65535, m_btNameLen/m_btChatPrefixLen BYTE toi 255,
 hoac bieu thuc 'sizeof(struct) - m_wLength - 1' bi TRAN NGUOC khi m_wLength lon) -> ghi tran ngan xep/vung nho dong.
 Mot goi hong (do luong TCP lech vi may chu vut byte khi ghi thieu) la du lam sap.
 Vá: KIEM BIEN truoc khi chep. Sai thi BO GOI + ghi [BIEN-XAU] mot lan/giay. Khong doi hanh vi khi goi hop le.
 Doc/ghi latin-1 newline='' (tep TCVN3/GBK). Neo kiem dem == 1."""
import io, os

ROOT = r"D:\GAMEDEVNEW_wt_net\Sources\Core\Src"
MARK = "[BIEN 04/09]"

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)
def rep1(s, old, new, name):
    n = s.count(old)
    assert n == 1, "%s: found %d (expected 1)" % (name, n)
    return s.replace(old, new)

# ---------------- KPlayerChat.cpp : GetChat
p = os.path.join(ROOT, "KPlayerChat.cpp"); s = rd(p); N = "\r\n" if "\r\n" in s else "\n"
if MARK not in s:
    old = ("\tchar	szBuf[MAX_SENTENCE_LENGTH];" + N +
           "\tKUiMsgParam	Param;" + N + N +
           "\tParam.cChatPrefixLen = pChat->m_btChatPrefixLen;" + N +
           "\tParam.nMsgLength = pChat->m_wSentenceLen;" + N +
           "\tmemcpy(szBuf, &pChat->m_szSentence[pChat->m_btNameLen + pChat->m_btChatPrefixLen], pChat->m_wSentenceLen);" + N +
           "\tszBuf[pChat->m_wSentenceLen] = 0;" + N)
    if old not in s:
        i = s.index("void	KPlayerChat::GetChat(PLAYER_SEND_CHAT_SYNC *pChat)")
        j = s.index("\tszBuf[pChat->m_wSentenceLen] = 0;", i)
        j = s.index(N, j) + len(N)
        k = s.index("\tchar	szBuf[MAX_SENTENCE_LENGTH];", i)
        old = s[k:j]
    new = N.join([
        "\tchar	szBuf[MAX_SENTENCE_LENGTH];",
        "\tKUiMsgParam	Param;",
        "",
        "\t// %s KIEM BIEN: ba do dai duoi day deu lay TU GOI. Truoc day chep thang -> mot goi hong" % MARK,
        "\t// (luong lech vi may chu vut byte khi ghi thieu) la ghi tran szBuf[256] / szName[32] / cChatPrefix[16]",
        "\t// va de bep ngan xep -> sap (jx_crash.log 04/09 00:32:40: memcpy 20.608 byte vao szBuf[256]).",
        "\tif (pChat->m_wSentenceLen >= MAX_SENTENCE_LENGTH ||",
        "\t\tpChat->m_btNameLen >= sizeof(Param.szName) ||",
        "\t\tpChat->m_btChatPrefixLen > CHAT_MSG_PREFIX_MAX_LEN ||",
        "\t\t(int)pChat->m_btNameLen + (int)pChat->m_btChatPrefixLen + (int)pChat->m_wSentenceLen > (int)sizeof(pChat->m_szSentence))",
        "\t{",
        "\t\tAUTOLOG_EVERY(1000, \"[BIEN-XAU] goi chat hong: ten=%u tien to=%u cau=%u (toi da %u/%u/%u) - BO GOI\",",
        "\t\t\t(unsigned)pChat->m_btNameLen, (unsigned)pChat->m_btChatPrefixLen, (unsigned)pChat->m_wSentenceLen,",
        "\t\t\t(unsigned)sizeof(Param.szName), (unsigned)CHAT_MSG_PREFIX_MAX_LEN, (unsigned)MAX_SENTENCE_LENGTH);",
        "\t\treturn;",
        "\t}",
        "",
        "\tParam.cChatPrefixLen = pChat->m_btChatPrefixLen;",
        "\tParam.nMsgLength = pChat->m_wSentenceLen;",
        "\tmemcpy(szBuf, &pChat->m_szSentence[pChat->m_btNameLen + pChat->m_btChatPrefixLen], pChat->m_wSentenceLen);",
        "\tszBuf[pChat->m_wSentenceLen] = 0;",
        "",
    ])
    s = rep1(s, old, new, "GetChat kiem bien")
    wr(p, s); print("KPlayerChat.cpp OK")
else:
    print("KPlayerChat.cpp da va")

# ---------------- KPlayerTeam.cpp : ReceiveInvite
p = os.path.join(ROOT, "KPlayerTeam.cpp"); s = rd(p); N = "\r\n" if "\r\n" in s else "\n"
if MARK not in s:
    old = "\tmemcpy(szName, pInvite->m_szName, sizeof(pInvite->m_szName) - (sizeof(TEAM_INVITE_ADD_SYNC) - pInvite->m_wLength - 1));" + N
    new = N.join([
        "\t// %s KIEM BIEN: bieu thuc cu 'sizeof(struct) - m_wLength - 1' TRAN NGUOC khi m_wLength lon (m_wLength lay tu goi)" % MARK,
        "\t// -> so byte chep thanh khong lo, de bep ngan xep (jx_crash.log 04/09 00:21:13: 56.196 byte vao szName[32]).",
        "\t{",
        "\t\tint nChep = (int)sizeof(pInvite->m_szName) - ((int)sizeof(TEAM_INVITE_ADD_SYNC) - (int)pInvite->m_wLength - 1);",
        "\t\tif (nChep < 0 || nChep > (int)sizeof(szName) - 1 || nChep > (int)sizeof(pInvite->m_szName))",
        "\t\t{",
        "\t\t\tAUTOLOG_EVERY(1000, \"[BIEN-XAU] goi moi to doi hong: m_wLength=%u -> chep=%d (toi da %u) - BO GOI\",",
        "\t\t\t\t(unsigned)pInvite->m_wLength, nChep, (unsigned)sizeof(szName) - 1);",
        "\t\t\treturn;",
        "\t\t}",
        "\t\tmemcpy(szName, pInvite->m_szName, nChep);",
        "\t}",
        "",
    ])
    s = rep1(s, old, new, "ReceiveInvite kiem bien")
    wr(p, s); print("KPlayerTeam.cpp OK")
else:
    print("KPlayerTeam.cpp da va")
print("DONE")
