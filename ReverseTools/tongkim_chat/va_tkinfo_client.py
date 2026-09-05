# -*- coding: ascii -*-
# [TKINFO 05/09] Hook client: GameSpaceChangedNotify (kind 7/8/9 + doi map), UiShell (mo/dong), S3Client.vcxproj + .filters.
import io, os, re

S3 = r"D:\GAMEDEVNEW_wt_mail\Sources\S3Client"
T = "\t"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) >= 128)
def crlf(x): return x.replace("\r\n", "\n").replace("\n", "\r\n")
def rep(s, old, new, n=1):
    if s.count(old) != n and s.count(crlf(old)) == n:
        old, new = crlf(old), crlf(new)
    assert s.count(old) == n, ("anchor", old[:70], s.count(old))
    assert all(ord(c) < 128 for c in new), "new khong ASCII"
    return s.replace(old, new)

# ---- GameSpaceChangedNotify.cpp
p = os.path.join(S3, "Ui", "GameSpaceChangedNotify.cpp"); s = rd(p); h0 = hi(s)
s = rep(s, '#include "UiCase/UiTongKimScore.h"\n', '#include "UiCase/UiTongKimScore.h"\n#include "UiCase/UiTongKimInfo.h"\t// [TKINFO 05/09]\n')
s = rep(s, T*2 + "KUiTongKimScore::OnBattleBox((char*)uParam, nParam);\n",
           T*2 + "KUiTongKimScore::OnBattleBox((char*)uParam, nParam);\n"
         + T*2 + "KUiTongKimInfo::OnBattleBox((char*)uParam, nParam);\t// [TKINFO 05/09] kind 7/8 thong tin tran, 9 an\n")
s = rep(s, T*2 + "KUiTongKimScore::OnSwitchMap(1);\n",
           T*2 + "KUiTongKimScore::OnSwitchMap(1);\n" + T*2 + "KUiTongKimInfo::OnSwitchMap(1);\t// [TKINFO 05/09]\n")
assert hi(s) == h0; wr(p, s); print("GameSpaceChangedNotify ok")

# ---- UiShell.cpp
p = os.path.join(S3, "Ui", "UiShell.cpp"); s = rd(p); h0 = hi(s)
s = rep(s, '#include "UiCase/UiTongKimScore.h"\n', '#include "UiCase/UiTongKimScore.h"\n#include "UiCase/UiTongKimInfo.h"\t// [TKINFO 05/09]\n')
m = re.search(r"([ \t]*)KUiTongKimScore::OpenWindow\(\);[^\r\n]*(\r?\n)", s)
assert m, "OpenWindow anchor"
s = s[:m.end()] + m.group(1) + "KUiTongKimInfo::OpenWindow();" + T + "// [TKINFO 05/09]" + m.group(2) + s[m.end():]
m = re.search(r"([ \t]*)KUiTongKimScore::CloseWindow\(TRUE\);[^\r\n]*(\r?\n)", s)
assert m, "CloseWindow anchor"
s = s[:m.end()] + m.group(1) + "KUiTongKimInfo::CloseWindow(TRUE);" + T + "// [TKINFO 05/09]" + m.group(2) + s[m.end():]
assert hi(s) == h0; wr(p, s); print("UiShell ok")

# ---- vcxproj + filters: nhan doi dong UiTongKimScore -> UiTongKimInfo
for fn in ["S3Client.vcxproj", "S3Client.vcxproj.filters"]:
    p = os.path.join(S3, fn); s = rd(p); h0 = hi(s)
    n = 0
    for ext in ["cpp", "h"]:
        pat = re.compile(r"([ \t]*<Cl(?:Compile|Include) Include=\"[^\"]*UiTongKimScore\." + ext + r"\"(?: />|>[\s\S]*?</Cl(?:Compile|Include)>)[ \t]*\r?\n)")
        for m in list(pat.finditer(s))[:1]:
            blk = m.group(1)
            if "UiTongKimInfo." + ext in s: continue
            s = s[:m.end()] + blk.replace("UiTongKimScore." + ext, "UiTongKimInfo." + ext) + s[m.end():]
            n += 1
    assert hi(s) == h0; wr(p, s); print(fn, "them", n, "muc")
print("XONG")
