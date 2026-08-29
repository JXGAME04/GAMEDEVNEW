# -*- coding: utf-8 -*-
import io

BS = chr(92)
p = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()

hong = '_snprintf(szOut, nOutLen - 1, "' + BS + 'spr' + BS + 'npcres' + BS + \
    '%s' + BS + '%s' + BS + '%s_st01.spr",'
dung = '_snprintf(szOut, nOutLen - 1, "' + BS*2 + 'spr' + BS*2 + 'npcres' + BS*2 + \
    '%s' + BS*2 + '%s' + BS*2 + '%s_st01.spr",'
assert hong in s, "khong thay chuoi hong"
s = s.replace(hong, dung, 1)

# kiem luon dong Load npcs.txt (cung nghi bi nuot)
hong2 = 's_NpcTab.Load((LPSTR)"' + BS*2 + 'settings' + BS*2 + 'npcs.txt");'
if hong2 not in s:
    hong2b = 's_NpcTab.Load((LPSTR)"' + BS + 'settings' + BS + 'npcs.txt");'
    if hong2b in s:
        s = s.replace(hong2b, hong2, 1)
        print("sua ca dong Load npcs")
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("da sua chuoi path (backslash doi)")
