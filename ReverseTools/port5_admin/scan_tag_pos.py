# -*- coding: utf-8 -*-
r"""Mo phong TEncodeText de tim CHINH XAC nhung the '<' bi nuot (roi vao vi tri byte-duoi
cua mot cap 2 byte). Chi nhung cho DO moi thuc su hong."""
import io, re

P = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiTaskGuideStr.h"
s = io.open(P, "r", encoding="latin-1", newline="").read()

def bad_tags(text):
    """tra ve danh sach offset cua '<' bi engine nuot"""
    b = text.encode("latin-1", "replace")
    out, i = [], 0
    while i < len(b):
        c = b[i]
        if c > 0x80:
            if i + 1 < len(b):
                if b[i+1] == 0x3C:      # '<' roi vao byte duoi -> HONG
                    out.append(i + 1)
                i += 2
            else:
                break
        else:
            i += 1
    return out

n_line = 0
n_bad = 0
for m in re.finditer(r'^#define\s+(\w+)\s+"((?:[^"\\]|\\.)*)"', s, re.M):
    name, body = m.group(1), m.group(2)
    if "<" not in body: continue
    n_line += 1
    bads = bad_tags(body)
    if bads:
        n_bad += 1
        # in ngu canh quanh moi cho hong
        b = body.encode("latin-1", "replace")
        ctx = []
        for off in bads:
            a = max(0, off - 12); z = min(len(b), off + 14)
            ctx.append(repr(b[a:z]))
        print("HONG  %-16s %d cho: %s" % (name, len(bads), " | ".join(ctx)))

print("\nTong: %d chuoi co the, %d chuoi HONG" % (n_line, n_bad))
