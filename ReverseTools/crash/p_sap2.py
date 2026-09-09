# -*- coding: utf-8 -*-
"""[SAP 09/09] Va tang 2: 4 ham xoa trang thai cua KNpc deu giu san con tro nut ke roi moi goi
ModifyAttrib() -> tai nhap go mat nut do -> GetPrev() tren nut da Remove() = SAP.
Gom thanh 1 ham phu tro an toan (go nut TRUOC khi goi ra ngoai) + 1 vong co chan.
Sua bang PHAU THUAT THEO DONG (chac chan hon regex), kiem dong dau/cuoi truoc khi thay.
Doc/ghi latin-1, chi them ASCII. Dung: python p_sap2.py <goc worktree>
"""
import io, os, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_knode"
TAG = "[SAP 09/09]"

def rd(rel):
    p = os.path.join(ROOT, rel)
    d = io.open(p, "r", encoding="latin-1", newline="").read()
    return p, d, ("\r\n" if "\r\n" in d[:4000] else "\n")

def hb(d):
    return sum(1 for c in d if ord(c) >= 0x80)

# ---------------- KNpc.h : khai bao 2 ham phu tro (private, KHONG ao -> khong doi bo cuc lop)
p, d, E = rd(r"Sources\Core\Src\KNpc.h")
h0 = hb(d)
if "ClearOneStateNode" in d:
    print("[=] KNpc.h da co khai bao")
else:
    neo = "\tvoid\t\t\t\tForceClearStateSkillEffect();"
    assert d.count(neo) == 1, "KNpc.h: neo khong duy nhat"
    them = ("\t// %s Duyet danh sach trang thai AN TOAN truoc tai nhap (xem KNpc.cpp)." % TAG + E +
            "\t// Khong ao, khong them truong => bo cuc KNpc KHONG doi." + E +
            "\tbool\t\t\t\tClearOneStateNode(int nLoai, int nSkillId);" + E +
            "\tvoid\t\t\t\tClearStateNodesLoop(int nLoai, int nSkillId);" + E)
    d = d.replace(neo, them + neo, 1)
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    print("[+] KNpc.h: khai bao ClearOneStateNode / ClearStateNodesLoop")
assert hb(d) == h0
print("    KNpc.h high-byte %d (khong doi)" % hb(d))

# ---------------- KNpc.cpp : thay 4 than ham + chen 2 ham phu tro
p, d, E = rd(r"Sources\Core\Src\KNpc.cpp")
h0 = hb(d)
if "ClearOneStateNode" in d:
    print("[=] KNpc.cpp da va")
else:
    lines = d.split(E)          # lines[i] = dong i+1

    def chk(no, txt):
        got = lines[no - 1].strip()
        assert got.startswith(txt), "dong %d mong '%s', thay '%s'" % (no, txt, got[:70])

    # kiem moc truoc khi dung dao
    chk(12110, "void KNpc::ForceClearStateSkillEffect() {")
    chk(12141, "}")
    chk(12157, "void KNpc::ForceClearStateSkillEffect(int nSkillId)")
    chk(12192, "}")
    chk(12194, "void KNpc::ClearStateSkillEffect()")
    chk(12234, "}")
    chk(12237, "void KNpc::ClearStateSkillEffect(int nSkillId)")
    chk(12270, "}")
    print("    8 moc dong deu khop")

    HELP = [
        "//---------------------------------------------------------------------------",
        "// %s GOC SAP (dump 09/09 14:49, AV doc 0x10 tai KNpc.cpp ClearStateSkillEffect):" % TAG,
        "// 4 ham xoa trang thai duoi day deu giu san con tro nut KE (pNode) roi MOI goi",
        "// ModifyAttrib(). ModifyAttrib di vong ra ngoai va co the GO dung cai nut dang giu",
        "// (KNode::Remove() dat m_pPrev/m_pNext = NULL); nhip sau pNode->GetPrev() doc",
        "// [NULL+0x10] -> sap may chu. Nut nan nhan trong dump: skill 36 Thien Vuong Chien Y",
        "// (m_LeftTime = -1, magic_lifemax_p -21), ca hai con tro NULL ma vptr con nguyen.",
        "//",
        "// Cach sua: GO nut ra khoi danh sach TRUOC roi moi goi ra ngoai.",
        "//  - vong tim ben duoi KHONG goi ham ngoai nao => danh sach khong the doi giua chung;",
        "//  - nut da go la RIENG cua ta => moi duong go khac (deu duyet danh sach) khong cham",
        "//    toi duoc => khong the go/xoa hai lan.",
        "// Khac biet duy nhat so voi ban cu: nut duoc go TRUOC khi ModifyAttrib chay (truoc day",
        "// go sau) - dang neu co thuoc tinh nao doc lai chinh danh sach nay trong luc do.",
        "//---------------------------------------------------------------------------",
        "// nLoai: 0 = moi nut | 1 = dung m_SkillID == nSkillId | 2 = het han (!OverLook, LeftTime > 0)",
        "// Tra ve TRUE neu da go+xu ly duoc mot nut (goi lai de xu ly tiep).",
        "bool KNpc::ClearOneStateNode(int nLoai, int nSkillId)",
        "{",
        "\tKStateNode* pKill = NULL;",
        "\tKStateNode* pNode = (KStateNode*)m_StateSkillList.GetTail();",
        "\twhile (pNode)",
        "\t{",
        "\t\tKStateNode* pPrev = (KStateNode*)pNode->GetPrev();",
        "\t\tbool bHop;",
        "\t\tif (nLoai == 0)",
        "\t\t\tbHop = true;",
        "\t\telse if (nLoai == 1)",
        "\t\t\tbHop = (pNode->m_SkillID == nSkillId);",
        "\t\telse",
        "\t\t\tbHop = (!pNode->m_bOverLook && pNode->m_LeftTime != -1 && pNode->m_LeftTime > 0);",
        "\t\tif (bHop)",
        "\t\t{",
        "\t\t\tpKill = pNode;",
        "\t\t\tbreak;",
        "\t\t}",
        "\t\tpNode = pPrev;",
        "\t}",
        "\tif (pKill == NULL)",
        "\t\treturn false;",
        "\tpKill->Remove();\t\t// GO TRUOC khi goi ra ngoai",
        "\tfor (int i = 0; i < MAX_SKILL_STATE; i++)",
        "\t{",
        "\t\tif (pKill->m_State[i].nAttribType)",
        "\t\t\tModifyAttrib(m_Index, &pKill->m_State[i]);",
        "\t}",
        "\tdelete pKill;",
        "\treturn true;",
        "}",
        "",
        "void KNpc::ClearStateNodesLoop(int nLoai, int nSkillId)",
        "{",
        "#ifdef _SERVER",
        "\tbool bStateRemove = false;",
        "#endif",
        "\t// Chan 256 vong: ban cu duyet DUNG mot luot nen khong the lap vo tan; ban nay tim lai",
        "\t// tu duoi moi lan nen phai tu chan phong khi ModifyAttrib lai SINH them trang thai hop dieu kien.",
        "\tfor (int nBaoVe = 0; nBaoVe < 256; nBaoVe++)",
        "\t{",
        "\t\tif (!ClearOneStateNode(nLoai, nSkillId))",
        "\t\t\tbreak;",
        "#ifdef _SERVER",
        "\t\tbStateRemove = true;",
        "#endif",
        "\t}",
        "#ifdef _SERVER",
        "\tif (bStateRemove)",
        "\t\tUpdateNpcStateInfo();",
        "#endif",
        "}",
        "",
    ]

    def than(sig, loai, skill):
        return [sig, "{", "\t// %s xem ClearOneStateNode / ClearStateNodesLoop o tren." % TAG,
                "\tClearStateNodesLoop(%s, %s);" % (loai, skill), "}"]

    # thay tu DUOI len tren de so dong phia tren khong xe dich
    lines[12237 - 1:12270] = than("void KNpc::ClearStateSkillEffect(int nSkillId)", "1", "nSkillId")
    lines[12194 - 1:12234] = than("void KNpc::ClearStateSkillEffect()", "2", "0")
    lines[12157 - 1:12192] = than("void KNpc::ForceClearStateSkillEffect(int nSkillId)", "1", "nSkillId")
    lines[12110 - 1:12141] = HELP + than("void KNpc::ForceClearStateSkillEffect()", "0", "0")

    d = E.join(lines)
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    print("[+] KNpc.cpp: 2 ham phu tro + 4 than ham goi chung")
assert hb(d) == h0, "KNpc.cpp: high-byte doi!"
print("    KNpc.cpp high-byte %d (khong doi)" % hb(d))
print("XONG.")
