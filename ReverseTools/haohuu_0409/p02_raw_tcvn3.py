# -*- coding: utf-8 -*-
"""p02_raw_tcvn3.py -- GHI BYTE THAT thay cho escape bat phan trong S3Relay (chu 04/09: "toi muon ghi dung font")

p01 chen chu Viet/chu Han bang escape bat phan (\\365, \\263...) vi S3Relay Release chi co
/source-charset:windows-1258 ma KHONG co /execution-charset -> MSVC giai ma nguon theo 1258 roi
ma hoa lai theo ACP may (1252) => byte F5 (u+moc+nga), D5, E3... bien thanh '?'. Bo va nay:
  1. them /execution-charset:windows-1258 cho Release (giong Core.vcxproj / S3Client.vcxproj -> byte go = byte trong exe)
     va sua loi go /execute-charset -> /execution-charset o Debug;
  2. doi moi literal escape bat phan sang BYTE THAT (RULE 0 cua skill swordonline-dev: raw TCVN3 moi loai tep).

Nguon byte -- KHONG go tay mot byte nao:
  - chu Viet   : unicode_to_tcvn3_bytes() cua skill (vn_to_octal.py)
  - chu Han GBK: doc thang tu Core\\Src\\KPlayerChat.cpp #define ENEMY_UNITNAME / BROTHER_UNITNAME (chinh la
                 byte GameServer gui len relay)
Chay lai duoc: chuoi cu khong con thi bo qua. Kiem: FFFD = 0, so byte cao tang dung bang so byte Viet/Han da chen.

dung: python p02_raw_tcvn3.py [<goc worktree>]
"""
import importlib.util
import io
import os
import re
import sys

ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_haohuu"
RELAY = os.path.join(ROOT, "Sources", "MultiServer", "S3Relay")
CORE_CHAT = os.path.join(ROOT, "Sources", "Core", "Src", "KPlayerChat.cpp")
SKILL_V2O = os.path.join(os.path.expanduser("~"), ".claude", "skills", "swordonline-dev", "scripts", "vn_to_octal.py")

spec = importlib.util.spec_from_file_location("v2o", SKILL_V2O)
v2o = importlib.util.module_from_spec(spec)
spec.loader.exec_module(v2o)


def tcvn3(s):
    """chu Viet -> chuoi latin-1 mang byte TCVN3 (1 ky tu = 1 byte)."""
    return v2o.unicode_to_tcvn3_bytes(s).decode("latin-1")


def octal(s):
    """dang escape bat phan ma p01 da chen (cung quy tac sinh luc do)."""
    out = []
    for ch in tcvn3(s):
        c = ord(ch)
        if 0x20 <= c < 0x7F and ch not in '"\\':
            out.append(ch)
        else:
            out.append("\\%03o" % c)
    return "".join(out)


def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f:
        return f.read()


def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f:
        f.write(s)


def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def gbk_define(name):
    """lay noi dung literal cua #define <name> "..." trong KPlayerChat.cpp (byte that, ke ca hai ky tu \\n)."""
    d = rd(CORE_CHAT)
    m = re.search(r'#define\s+%s\s+"([^"\r\n]*)"' % name, d)
    assert m, "khong thay #define %s trong KPlayerChat.cpp" % name
    lit = m.group(1)
    assert hb(lit) == 4 and lit.endswith("\\n"), "literal %s khac ky vong: %r" % (name, lit)
    return lit


def octal_of_latin1(s):
    return "".join(ch if 0x20 <= ord(ch) < 0x7F and ch not in '"\\' else "\\%03o" % ord(ch) for ch in s)


def hexs(s):
    return " ".join("%02X" % ord(c) for c in s)


def patch(name, pairs):
    """pairs: [(old, new)] -- moi old phai xuat hien dung 1 lan (hoac 0 lan neu da doi)."""
    path = os.path.join(RELAY, name)
    d = rd(path)
    before = hb(d)
    expect = 0
    # neo dai (dong chu thich chua ca literal) phai thay TRUOC literal ngan
    pairs = sorted(pairs, key=lambda t: -len(t[0]))
    done = 0
    for old, new in pairs:
        c = d.count(old)
        if c == 0:
            assert new in d, "%s: khong thay ca chuoi cu lan chuoi moi:\n%r" % (name, old)
            continue
        assert c == 1, "%s: neo khong duy nhat (%d):\n%r" % (name, c, old)
        d = d.replace(old, new)
        expect += hb(new) - hb(old)
        done += 1
    assert "\ufffd" not in d
    assert hb(d) == before + expect, "%s: so byte cao lech" % name
    if done:
        wr(path, d)
    print("  %-18s doi %d/%d chuoi, byte cao %d -> %d" % (name, done, len(pairs), before, hb(d)))


def main():
    gbk_enemy = gbk_define("ENEMY_UNITNAME")      # GBK "Cuu nhan" + \n  (B3 F0 C8 CB 5C 6E)
    gbk_brother = gbk_define("BROTHER_UNITNAME")  # GBK "Than nhan" + \n (C7 D7 C8 CB 5C 6E)
    cli_enemy = tcvn3("Cừu Nhân")                 # = ENEMY_UNITNAME cua client UiChatCentre.cpp
    cli_brother = tcvn3("Thân Nhân")              # = BROTHER_UNITNAME cua client
    notice = tcvn3("[Hệ thống] %s hiện không trực tuyến, lời nhắn đã được lưu và sẽ chuyển khi %s đăng nhập.")
    prefix = tcvn3("[Lời nhắn lúc %02d/%02d %02d:%02d] ")
    print("GBK enemy  :", hexs(gbk_enemy[:-2]), "| client:", hexs(cli_enemy))
    print("GBK brother:", hexs(gbk_brother[:-2]), "| client:", hexs(cli_brother))

    # --- FriendMgr.cpp: 4 literal + 2 dong chu thich con chua octal -------------------------------
    friend = [
        ('"' + octal_of_latin1(gbk_enemy[:-2]) + '\\n"', '"' + gbk_enemy + '"'),
        ('"' + octal_of_latin1(gbk_brother[:-2]) + '\\n"', '"' + gbk_brother + '"'),
        ('"' + octal("Cừu Nhân") + '\\n"', '"' + cli_enemy + '\\n"'),
        ('"' + octal("Thân Nhân") + '\\n"', '"' + cli_brother + '\\n"'),
        # chu thich: bo octal, ghi hex ASCII de doc duoc o moi trinh soan thao
        ('//   "' + octal_of_latin1(gbk_enemy[:-2]) + '\\n" (Cuu nhan) va "' + octal_of_latin1(gbk_brother[:-2]) + '\\n" (Than nhan)',
         '//   ' + hexs(gbk_enemy[:-2]) + ' 0A (Cuu nhan) va ' + hexs(gbk_brother[:-2]) + ' 0A (Than nhan) -- chep tu KPlayerChat.cpp'),
        ('//   "' + octal("Cừu Nhân") + '" va "' + octal("Thân Nhân") + '" (TCVN3)',
         '//   ' + hexs(cli_enemy) + ' va ' + hexs(cli_brother) + ' (TCVN3 "Cuu Nhan" / "Than Nhan")'),
    ]
    patch("FriendMgr.cpp", friend)

    # --- ChannelMgr.cpp: thong bao cho nguoi gui + tien to moc thoi gian -------------------------
    chann = [
        ('"' + octal("[Hệ thống] %s hiện không trực tuyến, lời nhắn đã được lưu và sẽ chuyển khi %s đăng nhập.") + '"',
         '"' + notice + '"'),
        ('"' + octal("[Lời nhắn lúc %02d/%02d %02d:%02d] ") + '"', '"' + prefix + '"'),
    ]
    patch("ChannelMgr.cpp", chann)

    # --- S3Relay.vcxproj: execution charset ------------------------------------------------------
    vcx = [
        ("/source-charset:windows-1258 %(AdditionalOptions)",
         "/source-charset:windows-1258 /execution-charset:windows-1258 %(AdditionalOptions)"),
        ("/source-charset:windows-1258 /execute-charset:windows-1258 %(AdditionalOptions)",
         "/source-charset:windows-1258 /execution-charset:windows-1258 %(AdditionalOptions)"),
    ]
    patch("S3Relay.vcxproj", vcx)
    print("XONG")


if __name__ == "__main__":
    main()
