# -*- coding: utf-8 -*-
"""BAN DONG HANH - G2: dang ky 51 ham PARTNER_* vao ScriptFuns.cpp.

- extern: chen sau 2 extern PARTNER cu (ScriptFuns.cpp:13987-13988)
- 2 stub cu (PARTNER_GetCurPartner/GetSettingIdx, :15484-15485) -> tro ham that
- ~49 dang ky moi chen NGAY TRUOC `#else` cuoi khoi _SERVER (sau GiftcodeIsValid).
  GetPartnerBagLevel/SetPartnerBagLevel dang ky DE (ban dang ky sau thang) vi ban
  stub o :14521-14522 nam vung chung (client can giu stub).
Idempotent + neo thich ung CRLF/LF + sao luu.
"""
import io
import shutil

P = r"D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp"
NHAN = "[BDH 27/08]"
CR = chr(13)
LF = chr(10)
TAB = chr(9)

FUNCS = [
    "PARTNER_Count", "PARTNER_GetName", "PARTNER_SetName", "PARTNER_GetLevel",
    "PARTNER_GetExp", "PARTNER_GetSeries", "PARTNER_GetCharacter",
    "PARTNER_GetEssentialFeatureID", "PARTNER_GetEmotionDegree",
    "PARTNER_SetEmotionDegree", "PARTNER_AddEmotionDegree", "PARTNER_GetBirthday",
    "PARTNER_GetGenData", "PARTNER_GetAptitudes", "PARTNER_GetAttribs",
    "PARTNER_GetAttribsInc", "PARTNER_SetAttribs", "PARTNER_GetResists",
    "PARTNER_SetResists", "PARTNER_GetTaskValue", "PARTNER_SetTaskValue",
    "PARTNER_GetSkillInfo", "PARTNER_GetAllSkill", "PARTNER_AddFightPartner",
    "PARTNER_RemovePartner", "PARTNER_SetCurPartner", "PARTNER_CallOutCurPartner",
    "PARTNER_AddExp", "PARTNER_LevelUp", "PARTNER_AddSkill", "PARTNER_RemoveSkill",
    "PARTNER_RemoveAllSkill", "PARTNER_SetStandbySkill", "PARTNER_ReGenAttribsInc",
    "PARTNER_ChangeCharacter", "PARTNER_ChangeFeature",
    "PARTNER_ChangeFeatureOfPeriod", "PARTNER_GetEndure",
    "PARTNER_SetCallOutSwitch", "PARTNER_AddState", "PARTNER_AddLifeAptitude",
    "PARTNER_AddStrengthAptitude", "PARTNER_AddHitTargetRateAptitude",
    "PARTNER_AddDefenceAptitude", "PARTNER_AddSpeedAptitude",
    "PARTNER_AddLuckAptitude",
]


def thay(s, neo, moi):
    """thu neo nguyen ban (CRLF) truoc, roi ban LF; tra (s_moi, 1/0)."""
    for n2, m2 in ((neo, moi), (neo.replace(CR, ""), moi.replace(CR, ""))):
        if s.count(n2) == 1:
            return s.replace(n2, m2, 1), 1
    return s, 0


def main():
    s = io.open(P, "r", encoding="latin-1", newline="").read()
    if NHAN in s:
        print("da co, bo qua")
        return
    shutil.copyfile(P, P + ".truoc_bdh_2708")
    eol = CR + LF

    # ---- 1. extern ----
    neo = ("extern int LuaPARTNER_GetCurPartner(Lua_State* L);" + eol +
           "extern int LuaPARTNER_GetSettingIdx(Lua_State* L);")
    ext = [neo,
           "// " + NHAN + " he ban dong hanh - than o KPlayerPartner.cpp (chi _SERVER)",
           "#ifdef _SERVER"]
    for fn in FUNCS:
        ext.append("extern int Lua" + fn + "(Lua_State* L);")
    ext.append("extern int LuaPARTNER_GetCurPartner2(Lua_State* L);")
    ext.append("extern int LuaPARTNER_GetSettingIdx2(Lua_State* L);")
    ext.append("extern int LuaGetPartnerBagLevel2(Lua_State* L);")
    ext.append("extern int LuaSetPartnerBagLevel2(Lua_State* L);")
    ext.append("#endif")
    s, ok = thay(s, neo, eol.join(ext))
    assert ok, "khong khop neo extern"

    # ---- 2. thay 2 stub -> ham that ----
    neo = (TAB + TAB + '{ "PARTNER_GetCurPartner",' + TAB + 'LuaPARTNER_GetCurPartner },' + eol +
           TAB + TAB + '{ "PARTNER_GetSettingIdx",' + TAB + 'LuaPARTNER_GetSettingIdx },')
    moi = (TAB + TAB + '{ "PARTNER_GetCurPartner",' + TAB + 'LuaPARTNER_GetCurPartner2 },' +
           TAB + '// ' + NHAN + ' ban that' + eol +
           TAB + TAB + '{ "PARTNER_GetSettingIdx",' + TAB + 'LuaPARTNER_GetSettingIdx2 },' +
           TAB + '// ' + NHAN + ' ban that')
    s, ok = thay(s, neo, moi)
    assert ok, "khong khop neo stub"

    # ---- 3. khoi dang ky moi ----
    neo = (TAB + TAB + '{ "GiftcodeIsValid", LuaGiftcodeIsValid },' + eol +
           TAB + TAB + '#else ')
    reg = [TAB + TAB + '{ "GiftcodeIsValid", LuaGiftcodeIsValid },',
           TAB + TAB + "// " + NHAN + " HE BAN DONG HANH (KPlayerPartner.cpp)",
           TAB + TAB + "// (GetCurPartner/GetSettingIdx da thay stub o tren; 2 ham tui dang",
           TAB + TAB + "//  ky DE ban stub vung chung - Lua lay ban dang ky SAU CUNG)"]
    for fn in FUNCS:
        reg.append(TAB + TAB + '{ "' + fn + '",' + TAB + 'Lua' + fn + ' },')
    reg.append(TAB + TAB + '{ "GetPartnerBagLevel",' + TAB + 'LuaGetPartnerBagLevel2 },')
    reg.append(TAB + TAB + '{ "SetPartnerBagLevel",' + TAB + 'LuaSetPartnerBagLevel2 },')
    reg.append(TAB + TAB + '#else ')
    s, ok = thay(s, neo, eol.join(reg))
    assert ok, "khong khop neo dang ky"

    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("XONG b05: extern + %d dang ky moi + 2 stub thay + 2 tui de" % len(FUNCS))


if __name__ == "__main__":
    main()
