import sys
WT = sys.argv[1].rstrip("/").rstrip(chr(92))
C = WT + "/Sources/Library/Lua54/lua4compat.c"
def rd(p): return open(p, "rb").read().decode("latin-1")
def wr(p, s): open(p, "wb").write(s.encode("latin-1"))
B = chr(92)
E = "\r\n"
c = rd(C)
old = ("\tsize_t i = 0, o = 0;" + E +
       "\twhile (in[i] == '" + B + B + "' || in[i] == '/' || (in[i] == '.' && (in[i + 1] == '" + B + B + "' || in[i + 1] == '/'))) i += (in[i] == '.') ? 2 : 1;" + E)
n = c.count(old)
if n != 1:
    raise SystemExit("anchor l4_alias_norm: %d" % n)
new = ("\tsize_t i = 0, o = 0;" + E +
       "\t/* bo khoang trang dau (dong \"--@ cu=moi\" co dau cach), roi bo '" + B + B + "' '/' '." + B + B + "' dau */" + E +
       "\tfor (;;)" + E + "\t{" + E +
       "\t\tif (in[i] == ' ' || in[i] == '" + B + "t') { i++; continue; }" + E +
       "\t\tif (in[i] == '" + B + B + "' || in[i] == '/') { i++; continue; }" + E +
       "\t\tif (in[i] == '.' && (in[i + 1] == '" + B + B + "' || in[i + 1] == '/')) { i += 2; continue; }" + E +
       "\t\tbreak;" + E + "\t}" + E)
c = c.replace(old, new)
wr(C, c)
print("l4_alias_norm: bo khoang trang dau OK")
