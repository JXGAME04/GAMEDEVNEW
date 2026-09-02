# -*- coding: ascii -*-
"""hs_engine_patch5.py - HOA SON dot g (02/09) theo quyet dinh chu game:
  (2) addcoldmagic_v CHUAN LINUX 0x08097AD0: chi cong min/max noi cong bang, KHONG sinh thoi gian dong bang
      (JX1 cu: bang 16 bac -> nValue[1] roi AppendSkillEffect max() vao thoi gian dong cua chieu noi cong).
      Linux 0x0807C9C0 (nhanh phi vat ly): v0 += [0x121c] + noi cong goc, v2 += [0x1224] + noi cong goc, v1 KHONG doi.
  (4) auto*skill: giu nut ty le AM nhu Linux 0x08189000 (xoa CHI khi tong ve dung 0; go ma chua co nut -> tao nut am);
      het cho thi thay cho nut tro (ty le <= 0) - Linux dung map khong gioi han.
Doc/ghi latin-1. Idempotent. DUNG: python hs_engine_patch5.py [--kiem]
"""
import io, os, sys
ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
KIEM = "--kiem" in sys.argv
MARK = "[HOASON 02/09g]"
hib = lambda t: sum(1 for c in t if ord(c) >= 0x80)

class F:
    def __init__(self, name):
        self.p = os.path.join(ROOT, name); self.name = name
        self.s = io.open(self.p, "r", encoding="latin-1", newline="").read(); self.orig = self.s; self.n = 0
    def rep(self, old, new, tag):
        c = self.s.count(old)
        if c == 0:
            if new in self.s: print("  [=] %s: da ap (%s)" % (self.name, tag)); return
            raise SystemExit("KHONG THAY anchor %s (%s)" % (self.name, tag))
        if c != 1: raise SystemExit("anchor khong duy nhat %s (%s)" % (self.name, tag))
        self.s = self.s.replace(old, new, 1); self.n += 1; print("  [+] %s: %s" % (self.name, tag))
    def rep_func_body(self, sig, new_body, tag):
        i = self.s.find(sig)
        if i < 0: raise SystemExit("khong thay ham %s" % sig)
        j = self.s.find("{", i); k = j; depth = 0
        while k < len(self.s):
            if self.s[k] == "{": depth += 1
            elif self.s[k] == "}":
                depth -= 1
                if depth == 0: break
            k += 1
        if MARK in self.s[j:k]: print("  [=] %s: da ap (%s)" % (self.name, tag)); return
        self.s = self.s[:j + 1] + new_body + self.s[k:]; self.n += 1; print("  [+] %s: %s" % (self.name, tag))
    def save(self):
        if self.s == self.orig: print("  (khong doi) %s" % self.name); return
        if hib(self.s) != hib(self.orig) or "\xef\xbf\xbd" in self.s: raise SystemExit("LECH BYTE CAO / EF BF BD %s" % self.name)
        if not KIEM: io.open(self.p, "w", encoding="latin-1", newline="").write(self.s)
        print("  => ghi %s (%d cho)%s" % (self.name, self.n, " KIEM" if KIEM else ""))

# (2) KNpcAttribModify::AddColdMagicV
m = F("KNpcAttribModify.cpp")
m.rep_func_body("void KNpcAttribModify::AddColdMagicV(KNpc* pNpc, void* pData)",
    "\t\r\n"
    "\t// " + MARK + " CHUAN LINUX 0x08097AD0 (addcoldmagic_v idx 169): CHI cong v0 vao min [0x121c] va max [0x1224] noi cong bang.\r\n"
    "\t// Ban cu JX1 con tinh nValue[1] (thoi gian dong bang) theo bang 16 bac (toi da 64 khung) va AppendSkillEffect max() vao\r\n"
    "\t// thoi gian dong cua chieu noi cong -> Huyen Nhan Van Yen 1358 (+20..315) lam Khi Tong Hoa Son dong bang lau hon Linux.\r\n"
    "\t// Linux khong co khau ve 0 khi am (khong can: du lieu deu duong). Chu game duyet 02/09 (anh huong ca phai khac dung thuoc tinh nay).\r\n"
    "\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\r\n"
    "\tpNpc->m_CurrentColdMagic.nValue[0] += pMagic->nValue[0];\r\n"
    "\tpNpc->m_CurrentColdMagic.nValue[2] += pMagic->nValue[0];\r\n",
    "AddColdMagicV chuan Linux")

# (4) HS_AutoSkillModify: giu nut am, xoa khi dung 0, tao nut am khi go ma chua co
m.rep(
    "\tif (nFound >= 0)\r\n"
    "\t{\r\n"
    "\t\tpList[nFound].nRate += nRate;\r\n"
    "\t\tif (pList[nFound].nRate <= 0)\r\n"
    "\t\t{\r\n"
    "\t\t\tmemset(&pList[nFound], 0, sizeof(KMagicAutoSkill));\t// Linux xoa nut khi ty le ve 0\r\n"
    "\t\t\treturn;\r\n"
    "\t\t}\r\n"
    "\t\tif (nV2 > 0)\r\n"
    "\t\t{\r\n"
    "\t\t\tpList[nFound].nType = nType;\r\n"
    "\t\t\tpList[nFound].nWaitCastTime = nWait;\r\n"
    "\t\t}\r\n"
    "\t\treturn;\r\n"
    "\t}\r\n"
    "\tif (nV2 <= 0 || nFree < 0)\r\n"
    "\t\treturn;\t\t// go ma khong co muc (Linux tao nut ty le am - vo tac dung) hoac het cho\r\n",
    "\tif (nFound >= 0)\r\n"
    "\t{\r\n"
    "\t\tpList[nFound].nRate += nRate;\r\n"
    "\t\tif (pList[nFound].nRate == 0)\r\n"
    "\t\t{\r\n"
    "\t\t\tmemset(&pList[nFound], 0, sizeof(KMagicAutoSkill));\t// Linux 0x0818908F: xoa nut CHI khi tong ve dung 0; nut am giu lai (vo tac dung)\r\n"
    "\t\t\treturn;\r\n"
    "\t\t}\r\n"
    "\t\tif (nV2 > 0)\r\n"
    "\t\t{\r\n"
    "\t\t\tpList[nFound].nType = nType;\r\n"
    "\t\t\tpList[nFound].nWaitCastTime = nWait;\r\n"
    "\t\t}\r\n"
    "\t\treturn;\r\n"
    "\t}\r\n"
    "\t// " + MARK + " Linux 0x08189169: go ma chua co nut -> van tao nut (ty le am, khong ban) - chu game duyet lam dung Linux.\r\n"
    "\t// Linux dung map khong gioi han; JX1 co MAX_AUTOSKILL o -> het cho thi thay cho nut tro (ty le <= 0).\r\n"
    "\tif (nFree < 0)\r\n"
    "\t{\r\n"
    "\t\tfor (int j = 0; j < MAX_AUTOSKILL; j++)\r\n"
    "\t\t\tif (pList[j].nRate <= 0) { nFree = j; break; }\r\n"
    "\t\tif (nFree < 0 || nV2 <= 0)\r\n"
    "\t\t\treturn;\r\n"
    "\t}\r\n",
    "HS_AutoSkillModify giu nut am (Linux)")
m.save()

# (2) KNpc::AppendSkillEffect nhanh noi cong: bo max() thoi gian tu m_CurrentColdMagic
c = F("KNpc.cpp")
c.rep("\t\t\tpDes->nValue[0] += m_PhysicsMagic.nValue[0] + m_CurrentColdMagic.nValue[0];\r\n"
      "\t\t\tpDes->nValue[1] = max(pDes->nValue[1], m_CurrentColdMagic.nValue[1] + m_CurrentColdEnhance);\r\n"
      "\t\t\tpDes->nValue[2] += m_PhysicsMagic.nValue[0] + m_CurrentColdMagic.nValue[2];\r\n",
      "\t\t\tpDes->nValue[0] += m_PhysicsMagic.nValue[0] + m_CurrentColdMagic.nValue[0];\r\n"
      "\t\t\t// " + MARK + " Linux 0x0807C9C0 (nhanh phi vat ly): chi cong v0/v2, KHONG doi v1 (thoi gian dong) - addcoldmagic_v khong sinh dong bang.\r\n"
      "\t\t\t// v1 = v1 goc + coldenhance da dat o tren (Linux 0x0807CA27) - giu.\r\n"
      "\t\t\tpDes->nValue[2] += m_PhysicsMagic.nValue[0] + m_CurrentColdMagic.nValue[2];\r\n",
      "AppendSkillEffect bo thoi gian dong tu addcoldmagic_v")
c.save()
print("XONG%s." % (" (KIEM)" if KIEM else ""))
