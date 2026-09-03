# -*- coding: utf-8 -*-
"""[MAIL 03/09] Dot 1 - va script Lua o cay CHAY THAT (E:\\...\\bin\\{server,client}):
  - script\\protocol.lua        : noi 12 ten MAIL vao cuoi danh sach + unpack/Require du phong (server -> chep client)
  - script\\lib\\objbuffer_head.lua : them PushByType/PopByType (ca hai cay)
  - script\\script_protocol\\protocol_def_gs.lua : ECHO tro sang echo_gs.lua
  - script\\item\\lenhbaiadmin.lua : them muc menu "Thu kenh ScriptProtocol/specho"
  - chep echo_gs.lua (server) va protocol_def_c.lua (client) tu lua_out
Doc/ghi latin-1, neo duy nhat, idempotent theo dau [MAIL 03/09]. Chay: python p1_lua_patch.py [--check]
"""
import io, os, re, shutil, sys

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "lua_out")
MARK = "[MAIL 03/09]"
CHECK = "--check" in sys.argv


def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def wr(p, s):
    if CHECK:
        return
    d = os.path.dirname(p)
    if not os.path.isdir(d):
        os.makedirs(d)
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def eol_of(s):
    return "\r\n" if "\r\n" in s else "\n"


def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)


MAIL_NAMES = [
    "emSCRIPT_PROTOCOL_MAIL_HEADERLIST",
    "emSCRIPT_PROTOCOL_MAIL_NEWMAIL",
    "emSCRIPT_PROTOCOL_MAIL_STATECHANGE",
    "emSCRIPT_PROTOCOL_MAIL_DELETE",
    "emSCRIPT_PROTOCOL_MAIL_WHOLEMAIL",
    "emSCRIPT_PROTOCOL_MAIL_OPENWINDOW",
    "emSCRIPT_PROTOCOL_MAIL_REQUEST_HEADERLIST",
    "emSCRIPT_PROTOCOL_MAIL_REQUEST_DELETE",
    "emSCRIPT_PROTOCOL_MAIL_REQUEST_WHOLEMAIL",
    "emSCRIPT_PROTOCOL_MAIL_REQUEST_STATECHANGE",
    "emSCRIPT_PROTOCOL_MAIL_REQUEST_AUTODELETE",
    "emSCRIPT_PROTOCOL_MAIL_REQUEST_OPENURL",
]


def patch_protocol(p):
    s = rd(p)
    if MARK in s:
        print("  da va:", p)
        return s
    e = eol_of(s)
    h0 = hb(s)
    anchor = '"emSCRIPT_PROTOCOL_EXCHANGE_SHOP",'
    assert s.count(anchor) == 1, "protocol.lua: neo EXCHANGE_SHOP"
    i = s.index(anchor)
    # dong '}' dong bang KE_SCRIPT_PROTOCOL ngay sau do
    j = s.index(e + "}" + e, i)
    block = e + "\t-- [MAIL 03/09] he THU (client VLTK 2.0 protocol.lua:112-124) - noi CUOI de khong doi ma cu" + e
    block += "".join("\t\"%s\"," % n + e for n in MAIL_NAMES)
    s = s[:j] + block.rstrip(e) + s[j:]
    # du phong unpack / Require o cuoi tep
    tail = e.join([
        "",
        "-- [MAIL 03/09] du phong cho ProtocolProcess: unpack (Lua 4 khong co san, common.lua co the chua Include)",
        "-- va Require (client 2.0 goi truoc DynamicExecute; o ta = EnsureScript trong KScriptProtocol.cpp).",
        "if (unpack == nil) then",
        "\tfunction unpack(tb, n)",
        "\t\tif (not n) then",
        "\t\t\tn = 1",
        "\t\tend",
        "\t\tif (not tb or tb[n] == nil) then",
        "\t\t\treturn",
        "\t\tend",
        "\t\treturn tb[n], unpack(tb, n + 1)",
        "\tend",
        "end",
        "if (Require == nil) then",
        "\tfunction Require(szFile)",
        "\t\tif (szFile and szFile ~= \"\" and EnsureScript) then",
        "\t\t\treturn EnsureScript(szFile)",
        "\t\tend",
        "\t\treturn 0",
        "\tend",
        "end",
        "",
    ])
    if not s.endswith(e):
        s += e
    s += tail
    assert hb(s) == h0
    wr(p, s)
    print("  OK:", p)
    return s


def patch_objbuffer(p):
    s = rd(p)
    if MARK in s:
        print("  da va:", p)
        return
    e = eol_of(s)
    h0 = hb(s)
    tail = e.join([
        "",
        "-- [MAIL 03/09] PushByType/PopByType (uimail.lua va ScriptProtocol:HandleProcess cua VLTK 2.0 dung).",
        "-- Dinh dang tren day GIONG HET PushObject/PopObject: [1 byte loai][du lieu]; nType chi de ep loai khi ghi.",
        "function ObjBuffer:PushByType(handle, nType, obj)",
        "\tif (nType == OBJTYPE_NUMBER) then",
        "\t\tOB_PushByte(handle, OBJTYPE_NUMBER)",
        "\t\tObjBuffer:PushNumber(handle, obj or 0)",
        "\telseif (nType == OBJTYPE_STRING) then",
        "\t\tOB_PushByte(handle, OBJTYPE_STRING)",
        "\t\tObjBuffer:PushString(handle, obj or \"\")",
        "\telseif (nType == OBJTYPE_TABLE) then",
        "\t\tOB_PushByte(handle, OBJTYPE_TABLE)",
        "\t\tObjBuffer:PushTable(handle, obj or {})",
        "\telse",
        "\t\tOB_PushByte(handle, OBJTYPE_NIL)",
        "\tend",
        "end",
        "",
        "function ObjBuffer:PopByType(handle, nType)",
        "\treturn ObjBuffer:PopObject(handle)",
        "end",
        "",
    ])
    if not s.endswith(e):
        s += e
    s += tail
    assert hb(s) == h0
    wr(p, s)
    print("  OK:", p)


def patch_def_gs(p):
    s = rd(p)
    if MARK in s:
        print("  da va:", p)
        return
    e = eol_of(s)
    h0 = hb(s)
    # khoi ECHO: 4 dong sau ten, moi dong co chu thich GBK -> thay theo regex tren tung dong
    m = re.search(r'"emSCRIPT_PROTOCOL_ECHO",[^\r\n]*' + e + r'(\t\t"",[^\r\n]*' + e + r')(\t\t"ScriptProtocol:Echo",[^\r\n]*' + e + r')(\t\tnil,[^\r\n]*' + e + ')', s)
    assert m, "protocol_def_gs.lua: khong thay khoi ECHO"
    new = ('\t\t"\\\\script\\\\script_protocol\\\\echo_gs.lua",\t-- [MAIL 03/09] thu kenh: chuoi tu client' + e +
           '\t\t"EchoBack",' + e +
           '\t\t{OBJTYPE_STRING},' + e)
    s = s[:m.start(1)] + new + s[m.end(3):]
    assert hb(s) == h0 - (hb(m.group(1)) + hb(m.group(2)) + hb(m.group(3)))
    wr(p, s)
    print("  OK:", p)


def patch_lenhbai(p):
    s = rd(p)
    if MARK in s:
        print("  da va:", p)
        return
    e = eol_of(s)
    h0 = hb(s)
    # them muc menu sau dong chua /system"
    lines = s.split(e)
    # nhieu dong chua /system" (co dong bi chu thich) -> lay dong SONG dau tien trong menu main
    idx = [i for i, l in enumerate(lines) if '/system"' in l and not l.lstrip().startswith("--")]
    assert len(idx) >= 1, "lenhbaiadmin.lua: khong thay dong /system con song"
    if len(idx) > 1:
        print("  luu y: %d dong /system con song, chen sau dong %d" % (len(idx), idx[0] + 1))
    lines.insert(idx[0] + 1, '\t\t"Thu kenh ScriptProtocol (ECHO)/specho",\t-- [MAIL 03/09]')
    s = e.join(lines)
    tail = e.join([
        "",
        "-- [MAIL 03/09] thu kenh ScriptProtocol: may chu gui ECHO xuong client, client tra lai",
        "function specho()",
        "\tInclude(\"\\\\script\\\\script_protocol\\\\echo_gs.lua\")",
        "\tEchoTest()",
        "end",
        "",
    ])
    if not s.endswith(e):
        s += e
    s += tail
    assert hb(s) == h0
    wr(p, s)
    print("  OK:", p)


def copy_new(src, dst):
    if CHECK:
        print("  (check) chep", src, "->", dst)
        return
    d = os.path.dirname(dst)
    if not os.path.isdir(d):
        os.makedirs(d)
    shutil.copyfile(src, dst)
    print("  chep:", dst)


if __name__ == "__main__":
    s_proto = patch_protocol(os.path.join(SV, r"script\protocol.lua"))
    # client dung CUNG mot protocol.lua (danh sach ten = hop dong hai dau)
    wr(os.path.join(CL, r"script\protocol.lua"), s_proto)
    print("  chep protocol.lua -> client")
    patch_objbuffer(os.path.join(SV, r"script\lib\objbuffer_head.lua"))
    patch_objbuffer(os.path.join(CL, r"script\lib\objbuffer_head.lua"))
    patch_def_gs(os.path.join(SV, r"script\script_protocol\protocol_def_gs.lua"))
    patch_lenhbai(os.path.join(SV, r"script\item\lenhbaiadmin.lua"))
    copy_new(os.path.join(OUT, "echo_gs.lua"), os.path.join(SV, r"script\script_protocol\echo_gs.lua"))
    copy_new(os.path.join(OUT, "protocol_def_c.lua"), os.path.join(CL, r"script\script_protocol\protocol_def_c.lua"))
    print("XONG" + (" (chi kiem tra)" if CHECK else ""))
