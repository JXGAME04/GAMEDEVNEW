# -*- coding: utf-8 -*-
"""[SAP 09/09] Va nguyen nhan dump GameServer 14:49:
  1) Engine\\Src\\KNode.h  : GetPrev() them kiem m_pPrev NULL (doi xung voi GetNext) -> het SAP o ca 25 cho goi.
  2) Core\\Src\\KNpc.cpp   : ClearStateSkillEffect GO nut TRUOC khi goi ModifyAttrib -> het goc tai nhap.
Doc/ghi latin-1, chi THEM chu thich ASCII (KNpc.cpp la TCVN3 boc UTF-8: khong duoc dung byte cao).
Chay lai duoc. Dung: python p_sap_getprev.py <goc worktree>
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_knode"
TAG = "[SAP 09/09]"

def rd(rel):
    p = os.path.join(ROOT, rel)
    d = io.open(p, "r", encoding="latin-1", newline="").read()
    return p, d, ("\r\n" if "\r\n" in d[:4000] else "\n")

def wr(p, d):
    io.open(p, "w", encoding="latin-1", newline="").write(d)

def hb(d):
    return sum(1 for c in d if ord(c) >= 0x80)

def once(d, pat, repl, name, flags=re.M):
    ms = list(re.finditer(pat, d, flags))
    assert len(ms) == 1, "neo '%s': mong 1, thay %d" % (name, len(ms))
    m = ms[0]
    return d[:m.start()] + (repl(m) if callable(repl) else repl) + d[m.end():]

# ============================================================ 1) KNode.h
p, d, E = rd(r"Sources\Engine\Src\KNode.h")
h0 = hb(d)
if TAG in d:
    print("[=] KNode.h da va")
else:
    NEW = ("inline KNode* KNode::GetPrev(void)" + E +
           "{" + E +
           "\t// %s Them kiem m_pPrev NULL cho DOI XUNG voi GetNext() o tren (GetNext da kiem," % TAG + E +
           "\t// GetPrev thi quen). Remove() dat CA m_pPrev lan m_pNext = NULL, nen goi GetPrev()" + E +
           "\t// tren mot nut DA BI GO (vd: bi go boi ma tai nhap trong luc dang duyet danh sach)" + E +
           "\t// se doc [NULL+0x10] va lam SAP MAY CHU." + E +
           "\t// Dung vu dump 09/09 14:49 (KNpc::ClearStateSkillEffect, KNpc.cpp): AV doc dia chi 0x10." + E +
           "\t// Danh sach lanh manh thi hanh vi khong doi; danh sach da hong thi dung duyet som thay vi no." + E +
           "\tif (m_pPrev && m_pPrev->m_pPrev)" + E +
           "\t\treturn m_pPrev;" + E +
           "\treturn NULL;" + E +
           "}" + E)
    d = once(d, r"^inline KNode\* KNode::GetPrev\(void\)\r?\n\{\r?\n\tif \(m_pPrev->m_pPrev\)\r?\n\t\treturn m_pPrev;\r?\n\treturn NULL;\r?\n\}\r?\n",
             NEW, "KNode::GetPrev")
    wr(p, d)
    print("[+] KNode.h: GetPrev() kiem NULL")
assert hb(d) == h0, "KNode.h: high-byte doi!"
print("    KNode.h high-byte %d (khong doi)" % hb(d))

# ============================================================ 2) KNpc.cpp
p, d, E = rd(r"Sources\Core\Src\KNpc.cpp")
h0 = hb(d)
if TAG in d:
    print("[=] KNpc.cpp da va")
else:
    OLD = (r"\tKStateNode\* pNode;\r?\n"
           r"\tpNode = \(KStateNode \*\)m_StateSkillList\.GetTail\(\);\r?\n"
           r"\twhile\(pNode\)\r?\n"
           r"\t\{\r?\n"
           r"\t\tKStateNode\* pTempNode = pNode;\r?\n"
           r"\t\tpNode = \(KStateNode \*\)pNode->GetPrev\(\);\r?\n"
           r".*?"
           r"\t\t\tpTempNode = NULL;\r?\n"
           r"\t\t\tcontinue;\r?\n"
           r"\t\t\}\r?\n"
           r"\t\}\r?\n")
    NEW = (
        "\t// %s GOC SAP (dump 09/09 14:49): vong lap cu giu san con tro nut ke (pNode) ROI moi" % TAG + E +
        "\t// goi ModifyAttrib(). ModifyAttrib di vong ra ngoai va co the GO dung cai nut dang" + E +
        "\t// giu do (Remove() dat m_pPrev/m_pNext = NULL); nhip sau pNode->GetPrev() doc" + E +
        "\t// [NULL+0x10] -> AV. Nut nan nhan trong dump: skill 36 Thien Vuong Chien Y," + E +
        "\t// m_LeftTime = -1, magic_lifemax_p -21, ca hai con tro = NULL nhung vptr con nguyen." + E +
        "\t//" + E +
        "\t// Sua: GO nut ra khoi danh sach TRUOC roi moi goi ra ngoai." + E +
        "\t//  - vong tim ben duoi KHONG goi ham ngoai nao => danh sach khong the doi giua chung;" + E +
        "\t//  - nut da go la RIENG cua ta => moi duong go khac (deu duyet danh sach) khong" + E +
        "\t//    cham toi duoc, nen khong the go/xoa hai lan." + E +
        "\t// Danh sach xu ly va thu tu tu duoi len giu NGUYEN nhu ban cu. Khac biet duy nhat:" + E +
        "\t// nut duoc go truoc khi ModifyAttrib chay (truoc day go sau) - can neu co thuoc tinh" + E +
        "\t// nao doc lai chinh danh sach nay trong luc do (vd magic_reset_bufftime)." + E +
        "\tfor (;;)" + E +
        "\t{" + E +
        "\t\tKStateNode* pKill = NULL;" + E +
        "\t\tKStateNode* pNode = (KStateNode *)m_StateSkillList.GetTail();" + E +
        "\t\twhile (pNode)" + E +
        "\t\t{" + E +
        "\t\t\tKStateNode* pPrev = (KStateNode *)pNode->GetPrev();" + E +
        "\t\t\t// giu dung 3 dieu kien cu: bo qua OverLook va trang thai vinh vien (-1)" + E +
        "\t\t\tif (!pNode->m_bOverLook && pNode->m_LeftTime != -1 && pNode->m_LeftTime > 0)" + E +
        "\t\t\t{" + E +
        "\t\t\t\tpKill = pNode;" + E +
        "\t\t\t\tbreak;" + E +
        "\t\t\t}" + E +
        "\t\t\tpNode = pPrev;" + E +
        "\t\t}" + E +
        "\t\tif (pKill == NULL)" + E +
        "\t\t\tbreak;" + E +
        "\t\tpKill->Remove();\t\t// GO TRUOC khi goi ra ngoai" + E +
        "\t\tfor (int i = 0; i < MAX_SKILL_STATE; i++)" + E +
        "\t\t{" + E +
        "\t\t\tif (pKill->m_State[i].nAttribType)" + E +
        "\t\t\t\tModifyAttrib(m_Index, &pKill->m_State[i]);" + E +
        "\t\t}" + E +
        "\t\tdelete pKill;" + E +
        "#ifdef _SERVER" + E +
        "\t\tbStateRemove = true;" + E +
        "#endif" + E +
        "\t}" + E)
    d = once(d, OLD, NEW, "ClearStateSkillEffect", flags=re.M | re.S)
    wr(p, d)
    print("[+] KNpc.cpp: ClearStateSkillEffect go nut truoc khi goi ModifyAttrib")
assert hb(d) == h0, "KNpc.cpp: high-byte doi!"
print("    KNpc.cpp high-byte %d (khong doi)" % hb(d))
print("XONG.")
