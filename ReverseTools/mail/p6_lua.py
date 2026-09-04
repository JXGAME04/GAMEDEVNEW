# -*- coding: utf-8 -*-
"""[MAIL 03/09 D6] protocol.lua (may chu -> chep client + guong): sua 2 loi client thay trong ScriptError.log 17:50:
  1) "UIMail:HeaderListArrival(0,nil,1)": DynamicExecute (ScriptFuns.cpp) chi chuyen SO/CHUOI, tham so BANG (danh sach thu,
     ca thu WHOLEMAIL) thanh nil -> client: neu dinh dang co OBJTYPE_TABLE thi KHONG Pop o state dieu phoi ma chuyen HANDLE
     sang state dich (uimail.lua) va Pop o do (ScriptProtocol_RecvInState - protocol.lua co trong moi state dung ScriptProtocol).
  2) "attempt to call field `SendData'": ScriptProtocol:SendData nam o protocol_def_c.lua (state dieu phoi), uimail.lua khong
     thay -> dua vao protocol.lua (guard SendScriptDataToServer de may chu khong sao).
Doc/ghi latin-1, chi chen ASCII. Chay: python p6_lua.py [--check]
"""
import io, os, sys

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIRROR_S = r"D:\GAMEDEVNEW_wt_mail\serverscript_jx2\mail\server"
MIRROR_C = r"D:\GAMEDEVNEW_wt_mail\serverscript_jx2\mail\client"
CHECK = "--check" in sys.argv
MARK = "[MAIL 03/09 D6]"


def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def wr(p, s):
    if CHECK:
        print("  (check) ghi", p, len(s))
        return
    d = os.path.dirname(p)
    if not os.path.isdir(d):
        os.makedirs(d)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("  ghi:", p, len(s))


def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def rep1(s, old, new, label):
    n = s.count(old)
    assert n == 1, "%s: neo khop %d lan" % (label, n)
    return s.replace(old, new)


def patch_protocol(s):
    if MARK in s:
        print("  da va D6")
        return s
    e = "\r\n" if "\r\n" in s else "\n"
    h0 = hb(s)
    # 1) ProtocolProcess: nhanh client co BANG -> chuyen handle sang state dich
    old = e.join([
        "\t\tlocal tbParamFormat = self.tbProtocolDef[nProtolId][3]",
        "\t\tlocal tbParam = self:HandleProcess(nHandle, tbParamFormat)",
        "\t\tif MODEL_GAMESERVER == 1 then",
    ])
    new = e.join([
        "\t\tlocal tbParamFormat = self.tbProtocolDef[nProtolId][3]",
        "\t\t-- " + MARK + " client: DynamicExecute chi chuyen so/chuoi, BANG thanh nil (HeaderListArrival(0,nil,1)) ->",
        "\t\t-- co OBJTYPE_TABLE thi khong Pop o day ma chuyen HANDLE sang state dich, Pop o do (ScriptProtocol_RecvInState).",
        "\t\tif MODEL_GAMECLIENT == 1 and ScriptProtocol_HasTable(tbParamFormat) == 1 then",
        "\t\t\tif szFile ~= \"\" then",
        "\t\t\t\tRequire(szFile);",
        "\t\t\tend",
        "\t\t\tDynamicExecute(szFile, \"ScriptProtocol_RecvInState\", nHandle, szFun, unpack(tbParamFormat))",
        "\t\t\treturn",
        "\t\tend",
        "\t\tlocal tbParam = self:HandleProcess(nHandle, tbParamFormat)",
        "\t\tif MODEL_GAMESERVER == 1 then",
    ])
    s = rep1(s, old, new, "ProtocolProcess")
    # 2) ham moi o cuoi tep
    tail = e.join([
        "",
        "-- " + MARK + " dinh dang co OBJTYPE_TABLE? (ProtocolProcess phia client)",
        "function ScriptProtocol_HasTable(tbFormat)",
        "\tif type(tbFormat) ~= \"table\" then",
        "\t\treturn 0",
        "\tend",
        "\tfor i = 1, getn(tbFormat) do",
        "\t\tif tbFormat[i] == OBJTYPE_TABLE then",
        "\t\t\treturn 1",
        "\t\tend",
        "\tend",
        "\treturn 0",
        "end",
        "",
        "-- " + MARK + " chay trong STATE DICH (uimail.lua...): Pop tham so theo dinh dang (arg) roi goi szFun",
        "-- (\"UIMail:HeaderListArrival\" = phuong thuc cua bang toan cuc, hoac ten ham toan cuc).",
        "function ScriptProtocol_RecvInState(nHandle, szFun, ...)",
        "\tlocal tbFormat = {}",
        "\tfor i = 1, arg.n do",
        "\t\ttinsert(tbFormat, arg[i])",
        "\tend",
        "\tlocal tbParam = ScriptProtocol:HandleProcess(nHandle, tbFormat)",
        "\tlocal _, _, szObj, szMethod = strfind(szFun or \"\", \"^([%w_]+):([%w_]+)$\")",
        "\tif szObj then",
        "\t\tlocal obj = getglobal(szObj)",
        "\t\tif type(obj) == \"table\" and type(obj[szMethod]) == \"function\" then",
        "\t\t\treturn obj[szMethod](obj, unpack(tbParam))",
        "\t\tend",
        "\telse",
        "\t\tlocal f = getglobal(szFun or \"\")",
        "\t\tif type(f) == \"function\" then",
        "\t\t\treturn f(unpack(tbParam))",
        "\t\tend",
        "\tend",
        "\tif Msg2Player then",
        "\t\tMsg2Player(\"ScriptProtocol_RecvInState: khong co ham \"..(szFun or \"nil\"))",
        "\tend",
        "end",
        "",
        "-- " + MARK + " client -> may chu (truoc o protocol_def_c.lua nen state uimail.lua khong thay); may chu: SendScriptDataToServer = nil -> 0",
        "function ScriptProtocol:SendData(szEnum, nHandle)",
        "\tif (type(self[szEnum]) == \"number\" and SendScriptDataToServer) then",
        "\t\treturn SendScriptDataToServer(self[szEnum], nHandle)",
        "\tend",
        "\treturn 0",
        "end",
        "",
    ])
    if not s.endswith(e):
        s += e
    s += tail
    assert hb(s) == h0
    return s


if __name__ == "__main__":
    p = os.path.join(SV, r"script\protocol.lua")
    s = patch_protocol(rd(p))
    wr(p, s)
    wr(os.path.join(CL, r"script\protocol.lua"), s)
    wr(os.path.join(MIRROR_S, r"script\protocol.lua"), s)
    wr(os.path.join(MIRROR_C, r"script\protocol.lua"), s)
    print("XONG" + (" (chi kiem tra)" if CHECK else ""))
