# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] Do kenh sync task value 3 diem (GO SAU NGHIEM THU):
1. SERVER  KPlayerTask.cpp SyncTaskValueToClient: id 5100..5199 -> petsync_sv.log
2. CLIENT  KPlayer.cpp case UI_TASKVALUE:        id 5100..5199 -> petsync_cl.log
3. GAME    UiPet.cpp UpdateData:                 gia tri 4 diem -> petsync_ui.log
"""
import io

CR = chr(13)


def va(p, cu, moi, dau, nhan):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    if dau in s:
        print("da co:", nhan)
        return
    lf = s.replace(CR + "\n", "\n")
    if lf.count(cu) != 1:
        print("!! anchor (%d):" % lf.count(cu), nhan)
        return
    lf = lf.replace(cu, moi, 1)
    if CR + "\n" in s:
        io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))
    else:
        io.open(p, "w", encoding="latin-1", newline="").write(lf)
    print("VA:", nhan)


va(r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerTask.cpp",
   "void KPlayer::SyncTaskValueToClient(int nTaskId, int nValue)\n{\n#ifdef _SERVER\n\tif (m_nNetConnectIdx < 0)\n\t\treturn;\t// bot / chua co ket noi\n",
   "void KPlayer::SyncTaskValueToClient(int nTaskId, int nValue)\n{\n#ifdef _SERVER\n"
   "\t// [PETSYS DO 29/08 - GO SAU NGHIEM THU] soi kenh sync o pet\n"
   "\tif (nTaskId >= 5100 && nTaskId < 5200)\n"
   "\t{\n"
   "\t\tFILE* fDo = fopen(\"petsync_sv.log\", \"a\");\n"
   "\t\tif (fDo) { fprintf(fDo, \"gui id=%d val=%d conn=%d\\n\", nTaskId, nValue, m_nNetConnectIdx); fclose(fDo); }\n"
   "\t}\n"
   "\tif (m_nNetConnectIdx < 0)\n\t\treturn;\t// bot / chua co ket noi\n",
   "petsync_sv", "1. server gui")

va(r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayer.cpp",
   "\t\t\t\t\tPlayer[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(nTaskId, nTaskVal);\n\t\t\t\t}\n\t\t\t\tCoreDataChanged(GDCNI_TASK_VALUE_UPDATE, (unsigned int)nTaskId, nTaskVal);",
   "\t\t\t\t\tPlayer[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(nTaskId, nTaskVal);\n\t\t\t\t}\n"
   "\t\t\t\t// [PETSYS DO 29/08 - GO SAU NGHIEM THU]\n"
   "\t\t\t\tif (nTaskId >= 5100 && nTaskId < 5200)\n"
   "\t\t\t\t{\n"
   "\t\t\t\t\tFILE* fDo = fopen(\"petsync_cl.log\", \"a\");\n"
   "\t\t\t\t\tif (fDo) { fprintf(fDo, \"nhan id=%d val=%d\\n\", nTaskId, nTaskVal); fclose(fDo); }\n"
   "\t\t\t\t}\n"
   "\t\t\t\tCoreDataChanged(GDCNI_TASK_VALUE_UPDATE, (unsigned int)nTaskId, nTaskVal);",
   "petsync_cl", "2. client nhan")

va(r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiPet.cpp",
   "void KUiPet::UpdateData()\n{\n    char szBuf[64];\n    int bCo = (sPetTV(PET_TV_CREATE) == 1);\n",
   "void KUiPet::UpdateData()\n{\n    char szBuf[64];\n    int bCo = (sPetTV(PET_TV_CREATE) == 1);\n"
   "    // [PETSYS DO 29/08 - GO SAU NGHIEM THU]\n"
   "    {\n"
   "        FILE* fDo = fopen(\"petsync_ui.log\", \"a\");\n"
   "        if (fDo) { fprintf(fDo, \"updatedata co=%d diem=%d/%d/%d/%d\\n\", bCo,\n"
   "            sPetTV(PET_TV_UPGRADE), sPetTV(PET_TV_GROWN), sPetTV(PET_TV_TAME), sPetTV(PET_TV_XIUZHEN)); fclose(fDo); }\n"
   "    }\n",
   "petsync_ui", "3. UI ve")
print("XONG p33")
