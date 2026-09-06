#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""doi_lua54.py - [LUA54 05/09] DOI CAY SCRIPT sang ban Lua 5.4 co kiem tra tung buoc (thay cho logic bat).

Chay tu thu muc bin\\server hoac bin\\client (bat goi:  python tools\\doi_lua54.py server|client ).
Chi lam khi CO dau hieu LUA54.moi. Thu tu:
  1. Binary chinh (CoreServer.dll / CoreClient.dll + Game.exe + Engine.dll) PHAI la ban dung Lua54Dll.dll
     (doc bang import table: tim chuoi trong file). Thieu -> KHONG doi, thoat ma 2.
  2. Lua54Dll.dll phai co. Thieu -> thoat ma 3.
  3. Da co script.lua4 (da doi roi) -> chi doi dau hieu, thoat 0.
  4. Chuyen script -> script54 (converter trong tools\\), kiem dau hieu LUA54_DA_CHUYEN.txt va tep chot
     (timerserver.lua / protocol.lua). Loi -> thoat ma 4, cay cu giu nguyen.
  5. Doi ten: script -> script.lua4, script54 -> script (co lui lai neu buoc 2 that bai). LUA54.moi -> LUA54.da_doi.
Ma thoat 0 = xong; khac 0 = KHONG DUOC MO GAME/SERVER (bat kiem errorlevel).
"""
import os, sys, subprocess, io

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

CHE_DO = sys.argv[1] if len(sys.argv) > 1 else "server"
if CHE_DO == "server":
    BINARY = ["CoreServer.dll", "engine.dll"]
    TEP_CHOT = "timerserver.lua"
else:
    BINARY = ["CoreClient.dll", "Game.exe", "Engine.dll"]
    TEP_CHOT = "protocol.lua"


def loi(ma, msg):
    print("[LUA54] LOI: " + msg)
    print("[LUA54] KHONG doi cay script, KHONG mo game/server. Bao Claude (wauto-c9).")
    sys.exit(ma)


def dung_lua54(p):
    try:
        b = open(p, "rb").read()
    except OSError:
        return False
    return b"Lua54Dll.dll" in b


def main():
    if not os.path.exists("LUA54.moi"):
        print("[LUA54] khong co LUA54.moi - khong co gi de doi")
        return 0
    for b in BINARY:
        if not os.path.exists(b):
            loi(2, "thieu " + b)
        if not dung_lua54(b):
            loi(2, b + " KHONG phai ban dung Lua54Dll (van la ban Lua 4). Kiem lai .moi/.truoc")
    if not os.path.exists("Lua54Dll.dll"):
        loi(3, "thieu Lua54Dll.dll")
    if os.path.isdir("script.lua4"):
        if os.path.exists(os.path.join("script", "LUA54_DA_CHUYEN.txt")):
            os.rename("LUA54.moi", "LUA54.da_doi")
            print("[LUA54] cay script da la ban 5.4 tu truoc - chi doi dau hieu")
            return 0
        loi(5, "co script.lua4 nhung script hien tai thieu dau hieu - trang thai la, xem tay")
    if not os.path.isdir("script") or not os.path.exists(os.path.join("script", TEP_CHOT)):
        loi(4, "thu muc script hoac " + TEP_CHOT + " khong co")
    conv = os.path.join("tools", "chuyen_lua4_54.py")
    if not os.path.exists(conv):
        loi(4, "thieu " + conv)
    print("[LUA54] dang chuyen script -> script54 (Lua 4 -> 5.4) ...")
    r = subprocess.run([sys.executable, conv, "script", "script54", "--baocao", os.path.join("tools", "chuyen_baocao.txt")])
    if r.returncode != 0:
        loi(4, "converter tra ma %d" % r.returncode)
    if not os.path.exists(os.path.join("script54", "LUA54_DA_CHUYEN.txt")) or not os.path.exists(os.path.join("script54", TEP_CHOT)):
        loi(4, "script54 thieu dau hieu hoac " + TEP_CHOT)
    # dem tep de chac khong mat gi
    def dem(d):
        n = 0
        for _, _, fs in os.walk(d):
            n += sum(1 for f in fs if f.lower().endswith(".lua"))
        return n
    n1, n2 = dem("script"), dem("script54")
    if n2 < n1:
        loi(4, "script54 co %d tep .lua, it hon script (%d)" % (n2, n1))
    try:
        os.rename("script", "script.lua4")
    except OSError as e:
        loi(6, "khong doi ten duoc script -> script.lua4: %s (tep dang bi mo?)" % e)
    try:
        os.rename("script54", "script")
    except OSError as e:
        try:
            os.rename("script.lua4", "script")
        except OSError:
            pass
        loi(6, "khong doi ten duoc script54 -> script: %s" % e)
    os.rename("LUA54.moi", "LUA54.da_doi")
    print("[LUA54] XONG: script = ban 5.4 (%d tep), ban cu giu o script.lua4" % n2)
    return 0


if __name__ == "__main__":
    sys.exit(main())
