# -*- coding: utf-8 -*-
"""Post-build cua cau hinh x64: '..\\..\\..\\bin\\client64' (tinh tu Sources\\<du an> = D:\\bin, ngoai worktree) -> '..\\..\\bin\\client64'
(= <worktree>\\bin\\client64); Represent3 nam sau hon mot cap. 'md X' (ke ca ngay sau <Command>) -> 'if not exist X md X'."""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
S = ROOT + "\\Sources\\"
for rel, deep in (("S3Client\\S3Client.vcxproj", 3), ("Core\\Core.vcxproj", 3), ("Engine\\Engine.vcxproj", 3),
                  ("Represent\\Represent3\\Represent3.vcxproj", 4), ("FilterText\\FilterText.vcxproj", 3)):
    p = S + rel
    b = io.open(p, "rb").read()
    old = (b"..\\" * deep) + b"bin\\client64"
    new = (b"..\\" * (deep - 1)) + b"bin\\client64"
    n = b.count(old)
    nb = b.replace(old, new)
    nb, m = re.subn(rb"(<Command>|(?m:^)[ \t]*)md ([^\r\n <]*bin\\client64[^\r\n <]*)", lambda x: x.group(1) + b"if not exist " + x.group(2) + b" md " + x.group(2), nb)
    io.open(p, "wb").write(nb)
    print(rel, "duong:", n, "md:", m)
