# -*- coding: utf-8 -*-
"""Phan loai dong goldequip.txt can mo dung luyen (HKMP + An Bang + Dinh Quoc).
Doc/ghi latin-1 (anh xa 1:1 byte) -> khong the pha byte TCVN3."""
import io, sys, re, collections
sys.path.insert(0, "C:/Users/nguye/.claude/skills/swordonline-dev/scripts")
from vn_edit import doc_vn

SV = "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/settings/item/goldequip.txt"

def strip_brackets(nm):
    return re.sub(r'\[[^\]]*\]', ' ', nm)

def load(p):
    return [r.rstrip('\r').split('\t')
            for r in io.open(p, 'r', encoding='latin-1', newline='').read().split('\n') if r.strip()]

def base_sets(rows):
    """31 bo HKMP goc = 2 tu dau cua ten (da bo ngoac) o dong 2..153."""
    out = []
    for i in range(1, 153):
        b = strip_brackets(doc_vn(rows[i][0])).strip()
        p = ' '.join(b.split()[:2])
        if p and p not in out:
            out.append(p)
    return out

def classify(rows):
    """-> {filerow: (cap, qual, nhan)}  theo dung 4 muc chu chot 02/09."""
    pref = base_sets(rows)
    res = {}
    for i in range(1, len(rows)):
        nm = doc_vn(rows[i][0])
        low = nm.lower()
        filerow = i + 1
        if not nm.strip() or nm.strip() == doc_vn('Chi\xd5m v\xde'):
            continue
        if rows[i][57].strip():            # da co san gia tri -> khong dung toi
            continue
        # 0. loai do thue theo gio
        if 'định thời' in low or 'hạn chế thời gian' in low:
            continue
        b2 = ' '.join(strip_brackets(nm).strip().split()[:2])
        thuoc_hkmp = b2 in pref
        la_ab = 'an bang' in low
        la_dq = 'định quốc' in low
        if not (thuoc_hkmp or la_ab or la_dq):
            continue
        # 1. cuc pham / hoan my thang truoc
        if 'cực phẩm' in low or 'hoàn mỹ' in low:
            res[filerow] = (3, 9, 'CucPham/HoanMy')
        elif la_ab:
            res[filerow] = (3, 8, 'AnBang')
        elif la_dq:
            res[filerow] = (2, 7, 'DinhQuoc')
        else:
            res[filerow] = (4, 10, 'HKMP')
    return res

if __name__ == '__main__':
    sys.stdout.reconfigure(encoding='utf-8')
    rows = load(SV)
    res = classify(rows)
    c = collections.Counter(v[2] for v in res.values())
    print('TONG SO DONG SE SUA:', len(res))
    for k in ('HKMP', 'AnBang', 'DinhQuoc', 'CucPham/HoanMy'):
        print('  %-16s %4d dong' % (k, c[k]))
    print()
    for k in ('HKMP', 'AnBang', 'DinhQuoc', 'CucPham/HoanMy'):
        ds = [r for r in sorted(res) if res[r][2] == k]
        cap, qual, _ = res[ds[0]]
        print('=== %s : cot58=%d cot59=%d | %d dong ===' % (k, cap, qual, len(ds)))
        for r in ds[:6]:
            print('   %5d  %s' % (r, doc_vn(rows[r - 1][0])))
        if len(ds) > 6:
            print('   ... (%d dong nua)' % (len(ds) - 6))
