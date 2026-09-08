# -*- coding: utf-8 -*-
"""[X64 08/09 NGOAIHINH FIX] KUiOptions2::LoadSetting: khi chua co tep cau hinh tai khoan (GetAutoSettingFile() == NULL vi chua biet tai khoan:
luc vua khoi dong UiInit.cpp:69 va WM_ACTIVATEAPP UiShell.cpp:430) thi KHONG day mac dinh {true,true,true,true} xuong Core.
Bang chung jx_auto.log 13:5x (x64): luc dang nhap m_MaskType = 94 = ManTypeNameIdx (chi gan o nhanh Option.GetLow(LowPlayer) trong SyncPlayer)
-> Draw self: moi bo phan rong, part5 = \\spr\\npcres\\enemy\\enemy067\\enemy067_st.spr ('nhu dang mang mat na'); 19 s sau low=0 va mac lai do moi het.
Byte-safe (chi chen ASCII), neo duy nhat, chay lai an toan."""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
p = ROOT + r"\Sources\S3Client\Ui\UiCase\UiOptions2.cpp"
TAG = b"[X64 08/09 NGOAIHINH FIX]"
b = io.open(p, "rb").read()
if TAG in b:
    print("UiOptions2.cpp: da va"); sys.exit(0)
nl = b"\r\n" if b"\r\n" in b else b"\n"
pat = re.compile(
    rb"(\t\tKIniFile\* pSetting = g_UiBase\.GetAutoSettingFile\(\);[ \t]*\r?\n"
    rb"\t\tif \(pSetting\)[ \t]*\r?\n"
    rb"\t\t\{[ \t]*\r?\n"
    rb"\t\t\tfor \(i = 0; i < OPTION_INDEX_COUNT2; i\+\+\)[ \t]*\r?\n"
    rb"\t\t\t\tpSetting->GetInteger\(OPTIONS_SAVE_SECTION2, ls_ToggleOptionName2\[i\], false, &bOptionsEnable\[i\]\);[ \t]*\r?\n"
    rb"\t\t\tg_UiBase\.CloseAutoSettingFile\(true\);[ \t]*\r?\n"
    rb"\t\t\}[ \t]*\r?\n)")
ms = list(pat.finditer(b)); assert len(ms) == 1, len(ms)
m = ms[0]
ins = nl.join([
    b"\t\telse",
    b"\t\t{",
    b"\t\t\t// " + TAG + b" Chua biet tai khoan (vua khoi dong UiInit.cpp:69, WM_ACTIVATEAPP UiShell.cpp:430 truoc dang nhap)",
    b"\t\t\t// -> khong co tep cau hinh -> KHONG day mac dinh {true,true,true,true} (giam chi tiet nguoi choi / an NPC / an nguoi choi /",
    b"\t\t\t// giam skill) xuong Core. Truoc day no lam Option.GetLow(LowPlayer)=1 -> SyncPlayer gan m_MaskType = ManTypeNameIdx (94)",
    b"\t\t\t// cho CHINH MINH -> ve bang mau NPC enemy067 ('nhu dang mang mat na') toi lan dong bo sau (mac lai do). Do that x64 13:5x 08/09.",
    b"\t\t\treturn;",
    b"\t\t}"]) + nl
nb = b[:m.end(1)] + ins + b[m.end(1):]
assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127)
io.open(p, "wb").write(nb)
print("UiOptions2.cpp: FIX OK (chen sau dong", b.count(b"\n", 0, m.end(1)) + 1, ")")
