# -*- coding: latin-1 -*-
"""p02_giao_thuc_rpc.py  [RELAYHT 06/09]

Them giao thuc goi kich ban qua lai GameServer <-> S3Relay:
  Headers/KProtocolDef.h    : s2s_script = 98
  Headers/KRelayProtocol.h  : struct RELAY_SCRIPT_CALL

Doc/ghi latin-1 de KHONG lam hong byte GBK trong hai tep do.
"""
import io
import os
import sys

ROOT = r"D:\GAMEDEVNEW_wt_relay"
H = os.path.join(ROOT, "Headers")
MARK = "[RELAYHT 06/09]"


def read(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def write(p, d):
    io.open(p, "w", encoding="latin-1", newline="").write(d)


# ---------------------------------------------------------------- KProtocolDef.h
p = os.path.join(H, "KProtocolDef.h")
d = read(p)
if "s2s_script" in d:
    print("  = KProtocolDef.h: da co s2s_script")
else:
    old = "\ts2s_execute = 97,\t\t//Relay"
    assert d.count(old) == 1, "KProtocolDef.h: neo s2s_execute khong duy nhat"
    new = old + "\n\ts2s_script = 98,\t\t//Relay -- " + MARK + " goi kich ban GS<->relay (RELAY_SCRIPT_CALL)"
    d = d.replace(old, new, 1)
    write(p, d)
    print("  + KProtocolDef.h: s2s_script = 98")

# ---------------------------------------------------------------- KRelayProtocol.h
p = os.path.join(H, "KRelayProtocol.h")
d = read(p)
if "RELAY_SCRIPT_CALL" in d:
    print("  = KRelayProtocol.h: da co RELAY_SCRIPT_CALL")
else:
    old = "\n\n////////////////////////////////////////////////////////\n//pf_playercommunity"
    assert d.count(old) == 1, "KRelayProtocol.h: neo pf_playercommunity khong duy nhat"
    block = """

// """ + MARK + """ GOI KICH BAN QUA LAI GameServer <-> S3Relay (kieu RemoteExecute ban Linux)
//
// Goi nam TRONG RELAY_DATA, byte dau tien cua phan than la s2s_script, roi toi
// cau truc nay, roi toi 3 khoi byte noi duoi nhau:
//     char szScript[wScriptLen]   duong dan kich ban, ke ca byte 0 cuoi
//     char szFunc  [wFuncLen]     ten ham,            ke ca byte 0 cuoi
//     BYTE data    [wDataLen]     noi dung ObjBuffer (co the rong)
//
// Chieu di  (byIsResult = 0): ben goi nho lai dwCallId de doi ket qua.
// Chieu ve  (byIsResult = 1): dwCallId lay nguyen tu goi di; szFunc de trong.
// dwCallId = 0 nghia la KHONG can tra ket qua.
struct RELAY_SCRIPT_CALL : EXTEND_HEADER
{
\tDWORD\tdwCallId;
\tDWORD\tdwGameSvrId;\t\t\t\t\t\t\t// 0 = khong chi dinh may chu nao
\tBYTE\tbyIsResult;
\tWORD\twScriptLen;
\tWORD\twFuncLen;
\tWORD\twDataLen;
};

////////////////////////////////////////////////////////
//pf_playercommunity"""
    d = d.replace(old, block, 1)
    write(p, d)
    print("  + KRelayProtocol.h: struct RELAY_SCRIPT_CALL")

print("XONG p02.")
