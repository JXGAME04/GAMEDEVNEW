# -*- coding: utf-8 -*-
r"""[IOS-NEN 15/09] Va tiep sau va_thoat_ios_1509.py: chua ba he qua cua viec iOS khong con thoat app.

Phan bien tim ra (15/09):
  1. UiExit() khong bao gio chay nua tren iOS -> SavePrivateConfig() khong chay -> bo cuc giao dien,
     thanh ky nang, phim tat MAT khi nguoi choi vuot tat app. SDL KHONG day su kien vao nen qua hang doi
     thuong (SDL_events.h:100: "must be handled in a callback set with SDL_AddEventWatch"), nen phai dung
     bo theo doi su kien.
  2. Con MOT loi thoat song: KSdlApp.cpp - GameLoop() tra false thi goi HandleInput(WM_CLOSE), ham nay tra 0
     (thoat, khong hoi) khi g_bScreen bat hoac chuoi thong bao rong -> bQuit -> ra khoi main -> tien trinh
     song ma khong con cua so. Khong bam nut nao toi duoc, nhung du lieu chi can dat FullScreen=1 la mo lai.
  3. Auto co the DUNG IM LANG: bon cho trong S3Client.cpp truoc kia thoat app, nay thanh lenh rong nhung VAN
     `return`, ma dieu kien thi dinh dai -> moi nhip auto bi cat ngang, khong bao gi.
     Ba trong bon cho do chi chay khi bOutWhenDis/bOutTimer/bOutWhenTP bat; tren dien thoai giao dien khong
     bat duoc chung, chi tep .dat chep tu ban PC moi bat (JxWAutoNoiBo.cpp da xoa bUseFKey vi dung ly do do).
     Cho thu tu (bLaunch == 2) khong phu thuoc may co nay nen phai xu rieng: bo qua, choi tiep.

Sua o 5 tep, deu rao #ifdef JX_IOS:
  UiShell.h / UiShell.cpp   them UiLuuKhiVaoNen() - CHI luu cau hinh, KHONG don thu muc tam
                            (don tam giua luc dang choi co the xoa nham tep dang dung)
  KSdlApp.cpp               khai bao tien + ham theo doi vong doi; dang ky SDL_AddEventWatch;
                            va khong thoat khi GameLoop() tra false
  S3Client.cpp              bLaunch == 2: khong thoat, khong return
  JxWAutoNoiBo.cpp          xoa bOutWhenDis/bOutTimer/bOutWhenTP nhu da xoa bUseFKey
                            (tep nay dung chung voi ban Android nen phai rao)

HAI BAI HOC da tra gia (15/09):
  a) MOI dong them vao, KE CA CHU THICH, phai nam trong nhanh chi-iOS. De lot mot dong chu thich ra ngoai
     la kiem rao bao HONG vi ban Android/Windows khac di. Dung dang #ifdef JX_IOS <chu thich> #else <ma cu> #endif.
  b) Moi ban va phai co DAU RIENG de nhan ra da chay. Dua vao "neo con khong" la sai: ba ban va vao KSdlApp.cpp
     deu chen SAU mot dong neo van con nguyen, nen lan chay thu hai va de len lan nua -> loi "redefinition".

Cac tep la ISO-8859-1 nen doc-ghi bang latin-1 de giu nguyen tung byte.
Chay lai nhieu lan khong sao (moi ban va tu bo qua khi thay dau rieng cua no).

Dung:  python3 ios/va_thoat_ios_1509_b.py
"""
import os
import re
import sys

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DAU = "IOS-NEN 15/09"
# [MOBILE-NEN 15/09] Phan LUU CAU HINH KHI VAO NEN dung CHUNG cho ca Android lan iOS (rao JX_MOBILE):
# Android cung mat bo cuc khi nguoi choi vuot tat app, va SDL gui dung cac su kien do tren Android.
# Cac phan con lai (chan lenh thoat, auto) van rieng iOS, vi Android duoc phep tu dong app.
DAU_M = "MOBILE-NEN 15/09"

VA = []


def them(duong_dan, mo_ta, moc, mau, lam):
    """moc = chuoi chi co sau khi ban va nay da chay (dau rieng cua tung ban va)."""
    VA.append((duong_dan, mo_ta, moc, re.compile(mau), lam))


# 1. Khai bao ham luu khi vao nen.
them(
    "Sources/S3Client/Ui/UiShell.h",
    "khai bao UiLuuKhiVaoNen()",
    "void\t\tUiLuuKhiVaoNen();",
    r"(void\s+UiSetSwitchSceneStatus\(int bSwitching\);[^\r\n]*\r?\n)",
    lambda m: (
        m.group(1)
        + "#ifdef JX_MOBILE\t// [%s] luu cau hinh luc app vao nen (ca Android lan iOS).\n" % DAU_M
        + "void\t\tUiLuuKhiVaoNen();\n"
        + "#endif\n"
    ),
)

# 2. Cai dat ham do, dat ngay truoc UiExit().
them(
    "Sources/S3Client/Ui/UiShell.cpp",
    "cai dat UiLuuKhiVaoNen()",
    "void UiLuuKhiVaoNen()",
    r"(\r?\n)(void\s+UiExit\(\)\r?\n\{)",
    lambda m: (
        m.group(1)
        + "#ifdef JX_MOBILE\n"
        + "// [%s] Dien thoai: nguoi choi vuot tat app thi UiExit() khong chay, cau hinh giao dien mat.\n" % DAU_M
        + "// Tren iOS con chac chan mat, vi tu 15/09 iOS khong con duong nao goi UiExit() (ios/va_thoat_ios_1509.py).\n"
        + "// Ham nay duoc goi tu bo theo doi su kien cua SDL luc app sap vao nen.\n"
        + "// CHI luu cau hinh: KHONG goi CleanTempDataFolder() o day vi dang choi ma don thu muc tam co the xoa nham\n"
        + "// tep dang dung; viec don tam van de nguyen trong UiExit() cho cac ban khac.\n"
        + "// PHAI la UI_LIVING_S_INGAME, khong phai \"khac DEAD\": do la quy uoc san co cua ca UiExit() va nhanh mat\n"
        + "// ket noi trong UiHeartBeat(). Luu luc dang o man dang nhap / chon nhan vat la nguy hiem, vi\n"
        + "// SavePrivateConfig() xoa sach roi ghi lai tu cac cua so da bi huy -> co the xoa dung bo cuc can giu.\n"
        + "void UiLuuKhiVaoNen()\n"
        + "{\n"
        + "\tif (s_UiLiveSeed != UI_LIVING_S_INGAME)\n"
        + "\t\treturn;\n"
        + "\tg_UiBase.SavePrivateConfig();\n"
        + "}\n"
        + "\n"   # dong trong nam TRONG hang rao: de ngoai la ban Android thua mot dong -> kiem rao HONG
        + "#endif\n"
        + m.group(2)
    ),
)

# 3. KSdlApp: khai bao tien + ham theo doi vong doi.
#    KHONG include ../Ui/UiShell.h: header do keo theo ca lop cua so (ComWindow.h, WndButton.h), de sinh loi
#    bien dich khong dang co. Chi can mot khai bao tien la du.
them(
    "Sources/S3Client/Platform/KSdlApp.cpp",
    "khai bao tien + ham theo doi vong doi",
    "static bool SDLCALL JxTheoDoiVongDoi",
    r"(#include \"\.\./Ui/Elem/UiToaDo\.h\"[^\r\n]*\r?\n)",
    lambda m: (
        m.group(1)
        + "// KHONG mo #ifdef JX_MOBILE o day: doan nay da nam TRONG khoi JX_MOBILE mo o tren, lop rao thu hai la thua.\n"
        + "void UiLuuKhiVaoNen();\t// [%s] dinh nghia o Ui/UiShell.cpp\n" % DAU_M
        + "// [%s] Bo theo doi su kien. Cac su kien vong doi KHONG vao hang doi: SDL_events.c ghi ro \"We won't\n" % DAU_M
        + "// actually queue this event\" roi goi thang danh sach theo doi, nen SDL_PollEvent khong bao gio thay chung.\n"
        + "// Gia tri tra ve bi BO QUA voi bo theo doi (khac bo loc su kien); tra true chi cho dung quy uoc.\n"
        + "// Tren iOS ham chay luc bom su kien nen khong chen ngang giua khung; tren Android no CO THE chay giua khung\n"
        + "// (Android_WaitActiveAndLockActivity). Ham chi ghi mot tep ini, khong goi lai SDL, khong giu khoa -> an toan ca hai.\n"
        + "static bool SDLCALL JxTheoDoiVongDoi(void* pRieng, SDL_Event* pSuKien)\n"
        + "{\n"
        + "\t(void)pRieng;\n"
        + "\tbool bVaoNen = (pSuKien && pSuKien->type == SDL_EVENT_WILL_ENTER_BACKGROUND);\n"
        + "#ifdef JX_IOS\n"
        + "\t// CHI iOS moi bat them TERMINATING. Tren Android, luc vuot tat app da co duong thoat thuong lo viec luu\n"
        + "\t// (nativeSendQuit -> WM_CLOSE -> UiExit), them nhanh nay la ghi de tep ini LAN HAI ngay trong cua so 1 giay\n"
        + "\t// cua luc bi huy; ma KIniFile::Save ghi de toan bo tep nen de dut nua chung.\n"
        + "\tif (pSuKien && pSuKien->type == SDL_EVENT_TERMINATING)\n"
        + "\t\tbVaoNen = true;\n"
        + "#endif\n"
        + "\tif (bVaoNen)\n"
        + "\t\tUiLuuKhiVaoNen();\n"
        + "\treturn true;\n"
        + "}\n"
    ),
)

# 4. KSdlApp: dang ky ngay sau SDL_Init.
them(
    "Sources/S3Client/Platform/KSdlApp.cpp",
    "SDL_AddEventWatch luc vao nen",
    "SDL_AddEventWatch(JxTheoDoiVongDoi",
    r"(\tif \(!SDL_Init\(SDL_INIT_VIDEO \| SDL_INIT_EVENTS\)\)\r?\n\t\{\r?\n"
    r"\t\tg_DebugLog\(\"\[SDL\] SDL_Init loi: %s\", SDL_GetError\(\)\);\r?\n"
    r"\t\treturn FALSE;\r?\n\t\}\r?\n)",
    lambda m: (
        m.group(1)
        + "#ifdef JX_MOBILE\n"
        + "\t// [%s] SDL KHONG day cac su kien vong doi vao hang doi thuong (SDL_events.h: \"must be handled in a\n" % DAU_M
        + "\t// callback set with SDL_AddEventWatch\"), nen SDL_PollEvent khong bao gio thay chung. Bat o day de\n"
        + "\t// con kip luu cau hinh truoc khi iOS treo app.\n"
        + "\tSDL_AddEventWatch(JxTheoDoiVongDoi, NULL);\n"
        + "#endif\n"
    ),
)

# 5. KSdlApp: khong thoat khi GameLoop() tra false.
them(
    "Sources/S3Client/Platform/KSdlApp.cpp",
    "GameLoop() tra false: iOS khong thoat",
    "iOS de he dieu hanh tat app",
    r"(\t\t\t\tif \(!GameLoop\(\)\)\r?\n\t\t\t\t\{\r?\n)"
    r"(\t\t\t\t\t// KWin32App[^\r\n]*\r?\n)"
    r"(\t\t\t\t\tif \(HandleInput\(WM_CLOSE, 0, 0\) == 0\)\r?\n\t\t\t\t\t\tbQuit = true;\r?\n)",
    lambda m: (
        m.group(1)
        + "#ifdef JX_IOS\n"
        + "\t\t\t\t\t// [%s] iOS: khong thoat. HandleInput(WM_CLOSE) tra 0 khi g_bScreen bat hoac chuoi thong bao\n" % DAU
        + "\t\t\t\t\t// rong -> se ra khoi main va de lai tien trinh khong con cua so. iOS de he dieu hanh tat app.\n"
        + "#else\n"
        + m.group(2)
        + m.group(3)
        + "#endif\n"
    ),
)

# 6. S3Client: bLaunch == 2 -> khong thoat, khong return.
them(
    "Sources/S3Client/S3Client.cpp",
    "bLaunch == 2: bo qua thay vi thoat",
    "Bo qua thi auto choi tiep",
    r"(\t\t\t\t\tif\(bLaunch == 2\)\r?\n\t\t\t\t\t\{\r?\n)"
    r"(\t\t\t\t\t\tPostQuitMessage\(0\);\r?\n\t\t\t\t\t\treturn;\r?\n)",
    lambda m: (
        m.group(1)
        + "#ifdef JX_IOS\n"
        + "\t\t\t\t\t\t// [%s] iOS: khong thoat app, va KHONG return - return se cat ngang moi nhip auto khi\n" % DAU
        + "\t\t\t\t\t\t// dieu kien con dung, lam auto dung im lang. Bo qua thi auto choi tiep.\n"
        + "#else\n"
        + m.group(2)
        + "#endif\n"
    ),
)

# 7. JxWAutoNoiBo: xoa ba co tu thoat cho ban iOS.
them(
    "Sources/S3Client/Platform/JxWAutoNoiBo.cpp",
    "xoa bOutWhenDis/bOutTimer/bOutWhenTP",
    "pGL->setting.bOutWhenDis = 0;",
    r"(\tpGL->setting\.bUseFKey = 0;\r?\n)",
    lambda m: (
        m.group(1)
        + "#ifdef JX_IOS\n"
        + "\t// [%s] Ba co tu thoat nay tren iOS chi lam auto dung im lang (app khong con thoat duoc), va giao dien\n" % DAU
        + "\t// mobile khong co cho bat chung - chi tep .dat chep tu ban PC moi bat, dung ly do da xoa bUseFKey o tren.\n"
        + "\tpGL->setting.bOutWhenDis = 0;\n"
        + "\tpGL->setting.bOutTimer = 0;\n"
        + "\tpGL->setting.bOutWhenTP = 0;\n"
        + "#endif\n"
    ),
)


def main():
    da_sua = 0
    for duong_dan, mo_ta, moc, mau, lam in VA:
        tep = os.path.join(GOC, duong_dan)
        if not os.path.isfile(tep):
            sys.exit("HONG: khong thay %s" % duong_dan)
        with open(tep, "rb") as f:
            goc = f.read().decode("latin-1")

        # Dau rieng cua tung ban va: dua vao neo la sai, vi neo van con sau khi chen.
        if moc in goc:
            print("bo qua (da va): %-46s %s" % (duong_dan, mo_ta))
            continue

        so = len(mau.findall(goc))
        if so != 1:
            sys.exit("HONG: %s (%s) - mau tim thay %d lan, phai dung 1 lan" % (duong_dan, mo_ta, so))

        moi = mau.sub(lam, goc, count=1)
        if moi == goc:
            sys.exit("HONG: %s - thay the khong doi gi" % duong_dan)

        with open(tep, "wb") as f:
            f.write(moi.encode("latin-1"))
        print("da va: %-46s %s" % (duong_dan, mo_ta))
        da_sua += 1

    print("---")
    print("so cho da va: %d / %d" % (da_sua, len(VA)))
    print("Nho chay: python3 ios/kiem_rao.py  va  python3 ios/kiem_rao.py --pc")
    print("LUU Y: JxWAutoNoiBo.cpp khong nam trong tep du an Windows nen che do --pc BO QUA no.")


if __name__ == "__main__":
    main()
