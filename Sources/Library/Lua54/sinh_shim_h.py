#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""sinh_shim_h.py - [LUA54] nhung lua4compat.lua vao lua4compat_lua.h (chuoi C) de Lua54Dll nap
vao moi state luc lua4_baselibopen. CHAY LAI moi khi sua lua4compat.lua, roi build lai Lua54Dll.

    python sinh_shim_h.py
"""
import io, os

HERE = os.path.dirname(os.path.abspath(__file__))
src = io.open(os.path.join(HERE, "lua4compat.lua"), "r", encoding="latin-1").read()
assert all(ord(c) < 128 for c in src), "lua4compat.lua phai la ASCII thuan"
out = ["/* SINH TU DONG boi sinh_shim_h.py tu lua4compat.lua - DUNG SUA TAY */",
       "static const char lua4compat_lua[] ="]
for line in src.split("\n"):
    s = line.rstrip("\r").replace("\\", "\\\\").replace('"', '\\"')
    out.append('"' + s + '\\n"')
out.append(";")
io.open(os.path.join(HERE, "lua4compat_lua.h"), "w", encoding="latin-1", newline="\n").write("\n".join(out) + "\n")
print("da ghi lua4compat_lua.h (%d dong Lua)" % src.count("\n"))
