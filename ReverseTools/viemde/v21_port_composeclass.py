# -*- coding: utf-8 -*-
"""VIEM DE - port \\script\\lib\\composeclass.lua tu ban Linux sang JX1.

Ai can: npc\\yandituteng.lua:8 Include no, roi dung tbComposeClass:CheckMaterial
(dong 80) va :ConsumeMaterial (dong 86) de doi Viem De Do Dang -> Viem De Lenh Ky.

=================== VI SAO KHONG CHEP CA string.lua ==========================
composeclass.lua cua ban Linux Include "\\script\\lib\\string.lua". Tep do dinh
nghia 9 ham TOAN CUC: replace, split, join, trim, toColor, toSeriesColorText,
toSeries, toLevel, transferDigit2CnNum.
Da soat cay JX1: replace / split / join DA CO trong \\script\\lib\\common.lua,
trim da co o 2 noi khac. Chep ca tep vao se CHE MAT ban cua JX1 - rui ro khong
can thiet cho mot tinh nang.
Da soat tiep: composeclass.lua CHI dung DUNG MOT ham cua string.lua la
transferDigit2CnNum (o ComposeDailog - nhanh ma Viem De khong dung toi).
=> Bo Include string.lua, mang theo DUNG mot ham do, chep nguyen van tu ban Linux.

Cac ham khac ma composeclass goi ra ngoai da soat: AskClientForNumber (JX1 CO),
CalcEquiproomItemCount / ConsumeEquiproomItem / CalcFreeItemCellCount / GetCash /
Pay / Say / Talk / Msg2Player / WriteLog / GetName / GetAccount / GetLocalDate (CO);
dostring / getglobal / setglobal la ham CHUAN cua Lua 4.0; CreateTaskSay chi nam
trong ComposeDailog - nhanh Viem De khong goi.
=============================================================================
"""
import io
import os
import re
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bangtxt import uni2tcvn, tcvn2uni

spec = importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(dec2)

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
GUONG = r"D:\GAMEDEVNEW\serverscript_jx2\viemde"
NGUON = r"D:\ServerLinux\server1\script\lib\composeclass.lua"
NGUON_STR = r"D:\ServerLinux\server1\script\lib\string.lua"
DICH = r"\script\lib\composeclass.lua"


def doc_lnx(p):
    return [dec2.decline2(l.rstrip(b"\r")) for l in open(p, "rb").read().split(b"\n")]


def lay_ham(dong, ten):
    """cat nguyen van mot ham tu tep Linux"""
    ra = []
    trong = False
    sau = 0
    for l in dong:
        if not trong:
            if re.match(r"\s*function\s+%s\s*\(" % ten, l):
                trong = True
                sau = 1
                ra.append(l)
            continue
        ra.append(l)
        s = re.sub(r"--.*", "", l)
        sau += len(re.findall(r"\b(?:function|if|for|while|do)\b", s))
        sau -= len(re.findall(r"\bend\b", s))
        # 'do' trong 'for ... do' da dem o tren nen tru bot
        sau -= len(re.findall(r"\bfor\b[^\n]*\bdo\b", s))
        sau -= len(re.findall(r"\bwhile\b[^\n]*\bdo\b", s))
        if sau <= 0:
            break
    return ra


def main():
    dong = doc_lnx(NGUON)
    dstr = doc_lnx(NGUON_STR)
    ham = lay_ham(dstr, "transferDigit2CnNum")
    if not ham:
        print("!! khong cat duoc transferDigit2CnNum tu string.lua - DUNG LAI")
        return
    print("da cat transferDigit2CnNum: %d dong" % len(ham))

    ra = []
    ra.append("-- ==========================================================================")
    ra.append("-- composeclass.lua - PORT tu ban Linux sang JX1  (Viem De, 26/08)")
    ra.append("-- Sinh boi ReverseTools\\viemde\\v21_port_composeclass.py - DUNG SUA TAY")
    ra.append("--")
    ra.append("-- Nguyen van ban Linux, CHI khac mot cho: bo Include \\script\\lib\\string.lua")
    ra.append("-- va mang theo DUNG mot ham cua no ma tep nay can (transferDigit2CnNum).")
    ra.append("-- Ly do: string.lua dinh nghia replace/split/join/trim - JX1 DA CO cac ham")
    ra.append("-- do trong \\script\\lib\\common.lua, chep ca tep vao se che mat ban cua JX1.")
    ra.append("-- ==========================================================================")
    ra.append("")
    da_bo = 0
    for l in dong:
        if re.search(r'Include\s*\(\s*"\\\\script\\\\lib\\\\string\.lua"', l):
            ra.append("-- [JX1 26/08] BO Include string.lua (xem dau tep). Mang theo dung 1 ham:")
            ra.extend(ham)
            ra.append("")
            da_bo += 1
            continue
        ra.append(l)
    if da_bo != 1:
        print("!! so dong Include string.lua = %d (can dung 1) - DUNG LAI" % da_bo)
        return

    noi_dung = "\r\n".join(uni2tcvn(x) for x in ra)
    for goc in (SRV, GUONG):
        p = goc + DICH
        os.makedirs(os.path.dirname(p), exist_ok=True)
        tam = p + ".dangghi"
        with open(tam, "wb") as f:
            f.write(noi_dung.encode("latin-1"))
        os.replace(tam, p)
        print("da ghi %s (%d dong)" % (p, len(ra)))

    # kiem lai
    d = io.open(SRV + DICH, encoding="latin-1", newline="").read()
    print()
    print("KIEM LAI:")
    print("   con Include string.lua : %s" % ("CON - SAI!" if "string.lua" in d and "BO Include" not in d else "khong (dung)"))
    print("   co tbComposeClass      : %s" % ("co" if "tbComposeClass" in d else "KHONG - SAI!"))
    print("   co CheckMaterial       : %s" % ("co" if "function tbComposeClass:CheckMaterial" in d else "KHONG - SAI!"))
    print("   co ConsumeMaterial     : %s" % ("co" if "function tbComposeClass:ConsumeMaterial" in d else "KHONG - SAI!"))
    print("   co transferDigit2CnNum : %s" % ("co" if "function transferDigit2CnNum" in d else "KHONG - SAI!"))
    b = open(SRV + DICH, "rb").read()
    print("   FFFD=%d, LF-don=%d" % (b.count(b"\xef\xbf\xbd"), b.count(b"\n") - b.count(b"\r\n")))


main()
