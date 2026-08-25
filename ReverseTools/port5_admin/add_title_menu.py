# -*- coding: utf-8 -*-
r"""[24/08] Them muc "Danh hieu & vong sang" vao Lenh Bai Admin.

DA KIEM CHUNG:
 - CHI Bang Chien co danh hieu + vong sang. Ban Linux goc cua Bach Nhan / Thanh Bao / Ty Vo
   KHONG he co Title_AddTitle / AddSkillState nao (da grep ca 4 thu muc mirror) => KHONG thieu.
 - Bang Chien dung: quan ham 100-104 (match\head.lua:430) + vong sang AddSkillState(661,
   rank-1) (:432); danh hieu 105/106 (event\tongwar\head.lua:273); 199 Cao Cap DNB
   (npc_shizhe.lua:355); 3000 Vo Lam Minh Chu (kiem o head.lua:266).
 - Bang settings\playertitle_jx2.txt CO du ca 100,101,102,103,104,105,106,199,3000.
 - settings\skills.txt CO skill 661 ("trang thai trong chien truong Tong Kim"), 1485, 963, 509.
 - Chu ky engine (KJx2Title.cpp:216): Title_AddTitle(nId, nTimeType, nTime)  [nTimeType 1 =
   co han, nTime tinh FRAME, /18 = giay]; Title_ActiveTitle(nId) = DEO (tu ap AuraSkill cua
   danh hieu do); Title_RemoveTitle(nId).
"""
import io, os, shutil

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
p = os.path.join(E, r"script\item\hoatdong_admin.lua")
s = io.open(p, "r", encoding="latin-1", newline="").read()
NL = "\r\n" if "\r\n" in s else "\n"

if "HD_TT_Menu" in s:
    print("da co muc danh hieu"); raise SystemExit(0)

M = {"á":0xB8,"à":0xB5,"ả":0xB6,"ã":0xB7,"ạ":0xB9,"ă":0xA8,"ắ":0xBE,"ằ":0xBB,"ẳ":0xBC,"ẵ":0xBD,"ặ":0xC6,
     "â":0xA9,"ấ":0xCA,"ầ":0xC7,"ẩ":0xC8,"ẫ":0xC9,"ậ":0xCB,"đ":0xAE,"é":0xD0,"è":0xCC,"ẻ":0xCE,"ẽ":0xCF,
     "ẹ":0xD1,"ê":0xAA,"ế":0xD5,"ề":0xD2,"ể":0xD3,"ễ":0xD4,"ệ":0xD6,"í":0xDD,"ì":0xD7,"ỉ":0xD8,"ĩ":0xDC,
     "ị":0xDE,"ó":0xE3,"ò":0xDF,"ỏ":0xE1,"õ":0xE2,"ọ":0xE4,"ô":0xAB,"ố":0xE8,"ồ":0xE5,"ổ":0xE6,"ỗ":0xE7,
     "ộ":0xE9,"ơ":0xAC,"ớ":0xED,"ờ":0xEA,"ở":0xEB,"ỡ":0xEC,"ợ":0xEE,"ú":0xF3,"ù":0xEF,"ủ":0xF1,"ũ":0xF2,
     "ụ":0xF4,"ư":0xAD,"ứ":0xF8,"ừ":0xF5,"ử":0xF6,"ữ":0xF7,"ự":0xF9,"ý":0xFD,"ỳ":0xFA,"ỷ":0xFB,"ỹ":0xFC,"ỵ":0xFE}
for k, v in list(M.items()):
    if k.upper() != k and k.upper() not in M: M[k.upper()] = v
def V(u): return "".join(chr(M[c]) if c in M else c for c in u)

# ---- 1) them dong vao menu chinh (nhan KHONG duoc chua dau '/') ----
old = V("\t\"8. Nạp lại CONFIG (khi không có trận chạy)/HD_ReloadCfg\",")
assert s.count(old) == 1, "anchor menu chinh = %d" % s.count(old)
s = s.replace(old, old + NL + "\t" + V("\"9. Danh hiệu & vòng sáng (Bang Chiến)/HD_TT_Menu\","), 1)

# ---- 2) khoi ham ----
block = NL.join([
"",
"-- ================= 9) DANH HIEU & VONG SANG =================",
"-- CHI Bang Chien co danh hieu/vong sang. Da doi chieu ban Linux: Bach Nhan, Thanh Bao,",
"-- Ty Vo KHONG he co Title_AddTitle/AddSkillState nao => khong phai thieu khi port.",
"--   quan ham 1..5 = title 100..104  (missions\\tongwar\\match\\head.lua:430)",
"--   vong sang quan ham = skill 661 cap (rank-1)          (match\\head.lua:432)",
"--   105/106 Vo Lam De Nhat Bang     (event\\tongwar\\head.lua:273, han 30 ngay)",
"--   199 Cao Cap De Nhat Bang        (npc_shizhe.lua:355, han 90 ngay)",
"--   3000 Vo Lam Minh Chu            (head.lua:266)",
"-- Chu ky: Title_AddTitle(nId, nTimeType, nTime) - nTimeType 1 = co han, nTime tinh FRAME",
"-- (18 frame = 1 giay); Title_ActiveTitle(nId) = DEO len nguoi.",
"TT_QUANHAM = {" + V("\"Binh Sĩ\", \"Hiệu Úy\", \"Thống Lĩnh\", \"Phó Tướng\", \"Đại Tướng\"") + "}",
"",
"function HD_TT_Menu()",
"\tlocal nCur = Title_GetActiveTitle()",
"\tSayEx({" + V("\"<color=yellow>Danh hiệu & vòng sáng<color> - danh hiệu đang đeo: \"") + ".. tostring(nCur),",
"\t" + V("\"Quân hàm 1 - Binh Sĩ (kèm vòng sáng)/HD_TT_R1\","),
"\t" + V("\"Quân hàm 2 - Hiệu Úy (kèm vòng sáng)/HD_TT_R2\","),
"\t" + V("\"Quân hàm 3 - Thống Lĩnh (kèm vòng sáng)/HD_TT_R3\","),
"\t" + V("\"Quân hàm 4 - Phó Tướng (kèm vòng sáng)/HD_TT_R4\","),
"\t" + V("\"Quân hàm 5 - Đại Tướng (kèm vòng sáng)/HD_TT_R5\","),
"\t" + V("\"Danh hiệu 105 - Võ Lâm Đệ Nhất Bang/HD_TT_105\","),
"\t" + V("\"Danh hiệu 106 - Võ Lâm Đệ Nhất Bang (bang viên)/HD_TT_106\","),
"\t" + V("\"Danh hiệu 199 - Cao Cấp Đệ Nhất Bang/HD_TT_199\","),
"\t" + V("\"Danh hiệu 3000 - Võ Lâm Minh Chủ/HD_TT_3000\","),
"\t" + V("\"Gỡ hết danh hiệu và vòng sáng/HD_TT_Clear\","),
"\t" + V("\"Quay lại/HD_AdminMenu\"") + "})",
"end",
"",
"function HD_TT_Rank(nRank)",
"\tlocal nId = 100 + nRank - 1",
"\tTitle_AddTitle(nId, 0, 9999999)",
"\tTitle_ActiveTitle(nId)",
"\tAddSkillState(661, nRank - 1, 0, 999999)",
"\tMsg2Player(" + V("\"Đã nhận quân hàm \"") + ".. TT_QUANHAM[nRank] ..",
"\t\t" + V("\" (danh hiệu \"") + ".. nId .. " + V("\", vòng sáng kỹ năng 661 cấp \"") + ".. (nRank - 1) .. \")\")",
"end",
"function HD_TT_R1() HD_TT_Rank(1) end",
"function HD_TT_R2() HD_TT_Rank(2) end",
"function HD_TT_R3() HD_TT_Rank(3) end",
"function HD_TT_R4() HD_TT_Rank(4) end",
"function HD_TT_R5() HD_TT_Rank(5) end",
"",
"-- han dung y het ban goc: 105/106 = 30 ngay, 199 = 90 ngay (tinh bang FRAME)",
"function HD_TT_Cap(nId, nNgay, szTen)",
"\tTitle_AddTitle(nId, 1, nNgay * 24 * 60 * 60 * 18)",
"\tTitle_ActiveTitle(nId)",
"\tMsg2Player(" + V("\"Đã nhận danh hiệu \"") + ".. szTen .. " + V("\" (id \"") + ".. nId ..",
"\t\t" + V("\", hạn \"") + ".. nNgay .. " + V("\" ngày)\"") + ")",
"end",
"function HD_TT_105()  HD_TT_Cap(105,  30, " + V("\"Võ Lâm Đệ Nhất Bang\"") + ") end",
"function HD_TT_106()  HD_TT_Cap(106,  30, " + V("\"Võ Lâm Đệ Nhất Bang\"") + ") end",
"function HD_TT_199()  HD_TT_Cap(199,  90, " + V("\"Cao Cấp Đệ Nhất Bang\"") + ") end",
"function HD_TT_3000() HD_TT_Cap(3000, 30, " + V("\"Võ Lâm Minh Chủ\"") + ") end",
"",
"function HD_TT_Clear()",
"\tTitle_ActiveTitle(0)",
"\tlocal tb = {100, 101, 102, 103, 104, 105, 106, 199, 3000}",
"\tfor i = 1, getn(tb) do",
"\t\tTitle_RemoveTitle(tb[i])",
"\tend",
"\tRemoveSkillState(661)",
"\tRemoveSkillState(1485)",
"\tMsg2Player(" + V("\"Đã gỡ hết danh hiệu Bang Chiến và vòng sáng quân hàm.\"") + ")",
"end",
""])

s = s.rstrip() + NL + block + NL
if not os.path.isfile(p + ".truoc_title_2408"):
    shutil.copyfile(p, p + ".truoc_title_2408")
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("da them muc 9 (danh hieu & vong sang) + 12 ham")

d = r"D:\GAMEDEVNEW\serverscript_jx2\port5_admin"
if os.path.isdir(d):
    shutil.copyfile(p, os.path.join(d, "hoatdong_admin.lua"))
print("XONG")
