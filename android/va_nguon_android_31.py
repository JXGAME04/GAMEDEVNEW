# -*- coding: utf-8 -*-
#
# [LUACLIENT 09/09 I] Go cac phep do tam (ban va 27/28/29) sau khi da tim ra goc.
#
# Goc da tim duoc va sua o ban va 30 (hai mang trung ten GameScriptFuns).
# Nhung phep do dat trong sClientLoad chi de truy vet, khong con ly do o lai:
# no chay mot lan cho MOI script client, ton mot lan bien dich chunk vo ich.
#
# GIU LAI (vi co ich lau dai, khong phai do tam):
#   - ban va 23: dang ky _ERRORMESSAGE ve ham C ghi nhat ky;
#   - ban va 24: lua4_outerrmsg ghi ra ca tep, khong chi stderr;
#   - ban va 25: lua4_execute noi ro vi sao thoat thay vi im lang;
#   - ban va 26: l4_report ghi thang cau loi ra nhat ky.
# Bon cai nay chinh la thu da bien "loi cam" thanh "loi doc duoc".
#
# GIU LAI ca dong log so ham dang ky - no bat dung benh vua roi (80 thay vi 1351),
# nen de lai thi lan sau lech bang la thay ngay.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")

P = "Sources/Core/Src/KScriptProtocol.cpp"
DAU = "[LUACLIENT 09/09 I]"
NL = "\r\n"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "\t// [LUACLIENT 09/09 F] do: sau khi dang ky, chunk moi trong CHINH state nay",
        "\t// thay duoc nhung gi. Loi (neu co) tu vao ScriptError.log qua l4_report.",
        "\t// [LUACLIENT 09/09 G] ba du kien de chot xem 80 ham dang ky di dau",
        "\tSP_ClientLog(\"[SP] dang ky %d ham C cho %s | mucImpl[0]=%s | mot_state=%d | state=%p\",",
        "\t\tg_GetGameScriptFunNum(), szLow,",
        "\t\tGameScriptFuns[0].name ? GameScriptFuns[0].name : \"(rong)\",",
        "\t\tlua4_mot_state(), (void*)p->m_LuaState);",
        "\tlua_dostring(p->m_LuaState,",
        "\t\t\"local t = '' \"",
        "\t\t\"if Include == nil then t = t .. 'Include ' end \"",
        "\t\t\"if OB_Create == nil then t = t .. 'OB_Create ' end \"",
        "\t\t\"if _ALERT == nil then t = t .. '_ALERT ' end \"",
        "\t\t\"if type == nil then t = t .. 'type ' end \"",
        "\t\t\"if table == nil then t = t .. 'table ' end \"",
        "\t\t\"if string == nil then t = t .. 'string ' end \"",
        "\t\t\"if t ~= '' then error('[chan doan] thieu trong bang toan cuc: ' .. t) end\");",
    ])
    assert s.count(CU) == 1, "khong tim thay khoi do tam (thay %d)" % s.count(CU)
    MOI = NL.join([
        "\t// [LUACLIENT 09/09 I] Giu lai dong nay: no la thu bat duoc benh 09/09 - bang ham C",
        "\t// dang ky cho script client bi lech sang bang khac (80 ham phim tat cua S3Client",
        "\t// thay vi 1351 ham cua Core, xem ban va 30). Lech bang la thay ngay o so nay.",
        "\tSP_ClientLog(\"[SP] dang ky %d ham C (%s...) cho %s\", g_GetGameScriptFunNum(),",
        "\t\tGameScriptFuns[0].name ? GameScriptFuns[0].name : \"(rong)\", szLow);",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
