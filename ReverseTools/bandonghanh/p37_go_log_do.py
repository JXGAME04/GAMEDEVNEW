# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] Nghiem thu xong ("da oke") - GO 3 diem log C cua p33
(petsync_sv/cl/ui) de khoi phinh file khi chay that. Giu PLOG lua (nhe,
chi khi bam nut pet) - go not o dot trang bi."""
import io

CR = chr(13)


def go(p, dau, cuoi, nhan):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    lf = s.replace(CR + "\n", "\n")
    i = lf.find(dau)
    if i < 0:
        print("da go / khong thay:", nhan)
        return
    j = lf.find(cuoi, i)
    assert j > 0, nhan
    j += len(cuoi)
    lf = lf[:i] + lf[j:]
    if CR + "\n" in s:
        io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    else:
        io.open(p, "w", encoding="latin-1", newline="").write(lf)
    print("GO:", nhan)


go(r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerTask.cpp",
   "\t// [PETSYS DO 29/08 - GO SAU NGHIEM THU] soi kenh sync o pet\n",
   'fclose(fDo); }\n\t}\n',
   "1. server gui")

go(r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayer.cpp",
   "\t\t\t\t\t// [PETSYS DO 29/08 - GO SAU NGHIEM THU]\n",
   "fclose(fDo); }\n\t\t\t\t\t}\n",
   "2. client nhan")

go(r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp",
   "    // [PETSYS DO 29/08 - GO SAU NGHIEM THU]\n",
   "fclose(fDo); }\n    }\n",
   "3. UI ve")
print("XONG p37")
