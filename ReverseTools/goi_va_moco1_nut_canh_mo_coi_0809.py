# -*- coding: utf-8 -*-
"""goi_va_moco1_nut_canh_mo_coi_0809.py - [MOCOI 08/09] Chu bao: "ai dung gan mau ten binh thuong, ai dung XA thi CHU TEN
va DANH HIEU bi am do" (than nguoi van dung mau); truoc do ta la "dam toi hon"; o 144 Hz thay RO hon, 59 Hz van bi nhung mo.
GOC: KNpcRes::Init (KNpcRes.cpp:59) dat m_SceneID = 0 ma KHONG go nut khoi cay canh, trong khi KNpcRes::Remove
(KNpcRes.cpp:160) go dung cach (g_ScenePlace.RemoveObject). KNpc::Load (KNpc.cpp:6747) goi Init MA KHONG goi Remove truoc
(cho goi o KNpc.cpp:1308 thi co). Khe NPC duoc dung lai rat thuong xuyen (chu thich ngay tren Init: do that 7,5 lan/giay)
nen nut cu bi BO ROI trong cay canh, van tro dung chi so NPC do => moi khung CoreDrawGameObj duoc goi HAI LAN cho NPC ay.
 - Than nguoi ve hai lan: gan nhu khong thay (sprite dac chong len chinh no).
 - CHU ten/bang/danh hieu ve hai lan: moi lan chu da la hai luot (vien roi than chu) nen thanh BON luot; vien den dam gap
   doi va mau chu bi pha them mot lan => nhin dam va keo ve kenh manh nhat. Hai mau phe dang dung deu co kenh do bang 255
   (chinh phai 255,168,94; ta phai 255,146,255) nen ra "am do". Fps cang cao chu ve lai cang nhieu nen cang ro.
 - Chi NPC o XA bi: khe NPC bi dung lai khi nguoi choi vao/ra tam dong bo - dung la nhung nguoi dung XA; nguoi dung gan
   khe on dinh nen khong sinh nut mo coi.
SUA: Init go nut cu truoc khi xoa id, dung m_SceneID_NPCIdx da luu (dat tai KNpcRes.cpp:1301), y het Remove().
Ham dung (constructor, KNpcRes.cpp:46) KHONG sua: doi tuong moi thi m_SceneID chua co gia tri hop le.
Chi sua Core -> CoreClient.dll, khong doi bo cuc lop."""
import io, re, sys

P = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\KNpcRes.cpp"
NL = "\r\n"
s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "[MOCOI 08/09]" in s:
    print("da va roi")
    sys.exit(0)

# Neo DUY NHAT: chi khoi trong Init moi co hai dong trong roi chu thich m_pSprNode
old = ("\tm_SceneID_NPCIdx = 0;" + NL +
       "\tm_SceneID = 0;" + NL + NL + NL +
       "//\tm_pSprNode = NULL;" + NL)
n = s.count(old)
if n != 1:
    print("FAIL neo: thay %d, can 1" % n)
    sys.exit(1)

new = ("\t// [MOCOI 08/09] PHAI go nut cu khoi cay canh truoc khi xoa id. Truoc day chi gan 0: khi mot khe NPC duoc dung lai" + NL +
       "\t// (KNpc::Load goi Init MA KHONG goi Remove truoc - KNpc.cpp:6747; khe doi chu ~7,5 lan/giay) thi nut cu BI BO ROI" + NL +
       "\t// trong cay canh, van tro dung chi so NPC nay => CoreDrawGameObj duoc goi HAI LAN moi khung cho NPC do." + NL +
       "\t// Than nguoi ve chong len chinh no thi kho thay, nhung CHU ten/bang/danh hieu ve hai lan (moi lan da la hai luot" + NL +
       "\t// vien + than chu) thi dam han len va mau bi keo ve kenh manh nhat - hai mau phe deu co do = 255 nen ra 'am do'." + NL +
       "\t// Chi NPC o XA bi vi khe cua ho moi la khe hay bi dung lai (vao/ra tam dong bo)." + NL +
       "\tif (m_SceneID && m_SceneID_NPCIdx > 0)" + NL +
       "\t\tg_ScenePlace.RemoveObject(CGOG_NPC, m_SceneID_NPCIdx, m_SceneID);" + NL +
       "\tm_SceneID_NPCIdx = 0;" + NL +
       "\tm_SceneID = 0;" + NL + NL + NL +
       "//\tm_pSprNode = NULL;" + NL)
s = s.replace(old, new)

if sum(1 for ch in s if ord(ch) >= 0x80) != h0:
    print("FAIL byte cao")
    sys.exit(1)
if re.search(r"[^\r]\n", s):
    print("FAIL LF")
    sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KNpcRes.cpp")
print("XONG MOCOI")
