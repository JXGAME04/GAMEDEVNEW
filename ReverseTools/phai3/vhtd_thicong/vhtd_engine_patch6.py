# -*- coding: utf-8 -*-
"""vhtd_engine_patch6.py [VHTD 02/09i] - tooltip ky nang TU PHONG (autoreplyskill 195 / autoattackskill 187 / autorescueskill 194 /
autodeathskill 184 / autocastskill 310) theo anh tooltip VLTK chu gui 02/09 ~11:40 (Doat Menh Lien Hoan Tam Tien Kiem 1364):
    Don danh co <orange>R%<color> ty le xuat <blue>TEN<color>
    So luong kiem xuat ra: N/9 kiem            (N = ChildSkillNum cua ky nang su kien tai cap; chi 1363/1368 = kiem Hoa Son)
    Thoi gian hoi chieu: CD giay               (CD = (v3 >> 8) / 18; ty le R = v3 & 0xFF; ky nang = (v1 >> 8) & 0xFFFF, cap = v1 & 0xFF)
Truoc: in dong MagicDesc "Khi bi cong kich, lay #d9-% ..." (autoreply) / trong (autocast, VLTK) -> chu: "hien thi sai 1 so thuoc tinh".
Khoi "Tang thu 2: TEN [cap N]" + thuoc tinh ky nang su kien (skill_showevent) da co san trong engine -> khong doi.
Chuoi Viet ghi TCVN3 THO (KSkills.cpp khong BOM, /source-charset = /execution-charset = 1258). Marker [VHTD 02/09i]. DUNG: python vhtd_engine_patch6.py [--kiem]
"""
import io, os, sys, re, importlib.util

KIEM = "--kiem" in sys.argv
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KSkills.cpp"
MK = "[VHTD 02/09i]"
spec = importlib.util.spec_from_file_location("vn_edit", r"C:/Users/nguye/.claude/skills/swordonline-dev/scripts/vn_edit.py")
vn = importlib.util.module_from_spec(spec); spec.loader.exec_module(vn)
def T(u):
    """Unicode -> TCVN3 tho (latin-1 str) de nhung vao nguon"""
    return vn.unicode_to_tcvn3_bytes(u).decode("latin-1")

NL = "\r\n"
HELPER = (
"// " + MK + " tooltip ky nang tu phong (Hoa Son 1364/1369/1365, Tieu Dao 2127, ...) theo tooltip client VLTK: 3 dong" + NL +
"// 'Don danh co R% ty le xuat TEN' / 'So luong kiem xuat ra: N/9 kiem' / 'Thoi gian hoi chieu: CD giay'. Ma hoa gia tri: v1 = (loai*65536 + id)*256 + cap," + NL +
"// v3 = khung_hoi_chieu*256 + ty_le_% (KNpc::HS_AutoSkill / CastAutoSkillAt). Truoc in cau MagicDesc chung chung -> chu: 'hien thi sai 1 so thuoc tinh'." + NL +
"static BOOL VhtdIsAutoSkillAttrib(int nType)" + NL +
"{" + NL +
"\treturn (nType == magic_autoreplyskill || nType == magic_autoattackskill || nType == magic_autorescueskill || nType == magic_autodeathskill || nType == magic_autocastskill);" + NL +
"}" + NL +
"static void VhtdAutoSkillDesc(const KMagicAttrib* pA, char* pszMsg)" + NL +
"{" + NL +
"\tchar szLine[256];" + NL +
"\tint nSkillId = (pA->nValue[0] >> 8) & 0xFFFF;" + NL +
"\tint nSkillLv = pA->nValue[0] & 0xFF;" + NL +
"\tint nRate = pA->nValue[2] & 0xFF;" + NL +
"\tint nCdSec = (pA->nValue[2] >> 8) / 18;" + NL +
"\tif (nSkillId <= 0 || nSkillId >= MAX_SKILL) return;" + NL +
"\tKSkill* pEv = (KSkill*)g_SkillManager.GetSkill(nSkillId, nSkillLv > 0 ? nSkillLv : 1);" + NL +
"\tif (!pEv) return;" + NL +
"\tif (pA->nAttribType == magic_autocastskill)" + NL +
"\t\tsprintf(szLine, \"" + T("Mỗi <color=orange>%d giây<color> tự thi triển <color=blue>%s<color> (tỷ lệ %d%%)") + "\\n\", nCdSec, pEv->GetSkillName(), nRate);" + NL +
"\telse if (pA->nAttribType == magic_autorescueskill)" + NL +
"\t\tsprintf(szLine, \"" + T("Sinh lực thấp có <color=orange>%d%%<color> tỷ lệ xuất <color=blue>%s<color>") + "\\n\", nRate, pEv->GetSkillName());" + NL +
"\telse if (pA->nAttribType == magic_autodeathskill)" + NL +
"\t\tsprintf(szLine, \"" + T("Khi chết xuất <color=blue>%s<color>") + "\\n\", pEv->GetSkillName());" + NL +
"\telse" + NL +
"\t\tsprintf(szLine, \"" + T("Đòn đánh có <color=orange>%d%%<color> tỷ lệ xuất <color=blue>%s<color>") + "\\n\", nRate, pEv->GetSkillName());" + NL +
"\tstrcat(pszMsg, szLine);" + NL +
"\tint nNum = pEv->GetChildSkillNum();" + NL +
"\tif (nNum > 1)" + NL +
"\t{" + NL +
"\t\tif (nSkillId == 1363 || nSkillId == 1368)" + NL +
"\t\t\tsprintf(szLine, \"" + T("Số lượng kiếm xuất ra: <color=orange>%d/9<color> kiếm") + "\\n\", nNum);" + NL +
"\t\telse" + NL +
"\t\t\tsprintf(szLine, \"" + T("Số lượng xuất ra: <color=orange>%d<color>") + "\\n\", nNum);" + NL +
"\t\tstrcat(pszMsg, szLine);" + NL +
"\t}" + NL +
"\tif (nCdSec > 0 && pA->nAttribType != magic_autocastskill)" + NL +
"\t{" + NL +
"\t\tsprintf(szLine, \"" + T("Thời gian hồi chiêu: <color=orange>%d giây<color>") + "\\n\", nCdSec);" + NL +
"\t\tstrcat(pszMsg, szLine);" + NL +
"\t}" + NL +
"}" + NL + NL)

def main():
    b = io.open(P, "rb").read()
    assert b[:3] != b"\xef\xbb\xbf", "KSkills.cpp co BOM - dung tool bo_bom_tcvn3.py truoc"
    s = b.decode("latin-1")
    if MK in s:
        print("  [=] KSkills.cpp da co %s" % MK); return
    # H1 helper truoc GetDescAboutLevel
    a3 = "void KSkill::GetDescAboutLevel(unsigned long ulSkillId, char * pszMsg, BOOL bNextLevel/* = FALSE*/, BOOL bAddSkillDamage/* = FALSE*/, BOOL bEventSkill/* = FALSE*/)" + NL + "{" + NL
    assert s.count(a3) == 1, "neo a3"
    s = s.replace(a3, HELPER + a3)
    # H2 sau m_szMagicSkillDesc: in 3 dong cho moi thuoc tinh tu phong (state + immediate)
    m = re.search(r"\tif \(m_szMagicSkillDesc\[0\]\)\r\n\t\tstrcat\(pszMsg, m_szMagicSkillDesc\);[ \t]*\r\n", s)
    assert m and s.count(m.group(0)) == 1, "neo a2"
    blk = (m.group(0) +
           "\t// " + MK + " ky nang tu phong: 3 dong theo tooltip VLTK (thay cau MagicDesc chung chung)" + NL +
           "\tfor (i = 0; i < m_nStateAttribsNum; i++)" + NL +
           "\t\tif (VhtdIsAutoSkillAttrib(m_StateAttribs[i].nAttribType)) VhtdAutoSkillDesc(&m_StateAttribs[i], pszMsg);" + NL +
           "\tfor (i = 0; i < m_nImmediateAttribsNum; i++)" + NL +
           "\t\tif (VhtdIsAutoSkillAttrib(m_ImmediateAttribs[i].nAttribType)) VhtdAutoSkillDesc(&m_ImmediateAttribs[i], pszMsg);" + NL)
    s = s.replace(m.group(0), blk)
    # H3 vong StateAttribs: bo qua thuoc tinh tu phong (da in o tren)
    a1 = "\tfor (i  = 0; i < m_nStateAttribsNum; i ++)" + NL + "\t{" + NL + "\t\tif (!m_StateAttribs[i].nAttribType || "
    assert s.count(a1) == 1, "neo a1"
    s = s.replace(a1, "\tfor (i  = 0; i < m_nStateAttribsNum; i ++)" + NL + "\t{" + NL +
                  "\t\tif (VhtdIsAutoSkillAttrib(m_StateAttribs[i].nAttribType)) continue;\t// " + MK + NL +
                  "\t\tif (!m_StateAttribs[i].nAttribType || ")
    nb = s.encode("latin-1")
    assert b"\xef\xbf\xbd" not in nb
    hi = sum(1 for c in nb if c >= 0x80) - sum(1 for c in b if c >= 0x80)
    print("  [+] KSkills.cpp: helper + 2 vong + skip (them %d byte TCVN3)" % hi)
    if not KIEM: io.open(P, "wb").write(nb)
    print("  => %s %s" % ("KIEM" if KIEM else "ghi", P))

if __name__ == "__main__":
    print("vhtd_engine_patch6 %s%s" % (MK, " (KIEM)" if KIEM else ""))
    main()
    print("XONG.")
