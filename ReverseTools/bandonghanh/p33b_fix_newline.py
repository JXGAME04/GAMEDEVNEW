# -*- coding: utf-8 -*-
r"""Sua chuoi fprintf trong KPlayer.cpp bi heredoc rut \\n thanh newline that."""
import io

CR = chr(13)
BS = chr(92)
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayer.cpp"
s = io.open(p, "r", encoding="latin-1", newline="").read()
lf = s.replace(CR + "\n", "\n")
hong = '"nhan id=%d val=%d\n", nTaskId, nTaskVal); fclose(fDo); }'
dung = '"nhan id=%d val=%d' + BS + 'n", nTaskId, nTaskVal); fclose(fDo); }'
assert lf.count(hong) == 1, lf.count(hong)
lf = lf.replace(hong, dung, 1)
io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
print("da sua newline trong chuoi C")
