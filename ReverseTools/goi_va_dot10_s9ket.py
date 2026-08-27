# -*- coding: ascii -*-
# S9-VE r2: ghi log SAU khi doi hoat anh, va CHI khi lech VAN CON (bat nam bep that su)
import io,sys
CRLF="\r\n"
def L(*a): return CRLF.join(a)
p=r"D:\GAMEDEVNEW\Sources\Core\Src\KNpc.cpp"
c=io.open(p,'r',encoding='latin-1',newline='').read(); hb=sum(1 for ch in c if ord(ch)>127)
old=L("\t\t\tAUTOLOG(\"[S9-VE] doing=%d cdoing=%d resdoing=%d resaction=%d frame=%d/%d reg=%d t=%u\", (int)m_Doing, (int)m_ClientDoing, m_DataRes.GetResDoing(), m_DataRes.GetAction(), m_Frames.nCurrentFrame, m_Frames.nTotalFrame, m_RegionIndex, SubWorld[0].m_dwCurrentTime);",
      "\t\t}",
      "\t}",
      "#endif",
      "\tm_DataRes.SetAction(m_ClientDoing);")
new=L("\t\t\tAUTOLOG(\"[S9-VE] doing=%d cdoing=%d resdoing=%d resaction=%d frame=%d/%d reg=%d t=%u\", (int)m_Doing, (int)m_ClientDoing, m_DataRes.GetResDoing(), m_DataRes.GetAction(), m_Frames.nCurrentFrame, m_Frames.nTotalFrame, m_RegionIndex, SubWorld[0].m_dwCurrentTime);",
      "\t\t}",
      "\t}",
      "#endif",
      "\tm_DataRes.SetAction(m_ClientDoing);",
      "#ifndef _SERVER",
      "\t// [S9-KET r2 26/08] Ban tren ghi TRUOC khi doi hoat anh nen luon tre mot nhip (do that",
      "\t// 7/7 ca deu tu hoi ngay sau do => bao dong gia). Cho nay ghi SAU khi da goi SetAction:",
      "\t// neu lop ve VAN khong khop voi trang thai logic thi la KET THAT (SetAction that bai -",
      "\t// vd m_pcResNode = NULL sau khi la cay bi go). Chi ghi cho chinh nhan vat, va chi khi",
      "\t// KET LIEN TIEP 3 nhip tro len de khoi bao dong vi mot khung le.",
      "\tif (m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex)",
      "\t{",
      "\t\tstatic int s_nS9KetDem = 0;",
      "\t\tif (m_DataRes.GetResDoing() != (int)m_ClientDoing)",
      "\t\t{",
      "\t\t\ts_nS9KetDem++;",
      "\t\t\tif (s_nS9KetDem == 3 || (s_nS9KetDem > 3 && (s_nS9KetDem % 18) == 0))",
      "\t\t\t\tAUTOLOG(\"[S9-KET] LOP VE KHONG DOI DUOC: cdoing=%d resdoing=%d resaction=%d doing=%d lien tiep %d nhip t=%u\", (int)m_ClientDoing, m_DataRes.GetResDoing(), m_DataRes.GetAction(), (int)m_Doing, s_nS9KetDem, SubWorld[0].m_dwCurrentTime);",
      "\t\t}",
      "\t\telse if (s_nS9KetDem)",
      "\t\t{",
      "\t\t\tif (s_nS9KetDem >= 3)",
      "\t\t\t\tAUTOLOG(\"[S9-KET-HET] lop ve da khop lai sau %d nhip (cdoing=%d) t=%u\", s_nS9KetDem, (int)m_ClientDoing, SubWorld[0].m_dwCurrentTime);",
      "\t\t\ts_nS9KetDem = 0;",
      "\t\t}",
      "\t}",
      "#endif")
if c.count(old)!=1: print("LOI neo S9-VE r2:",c.count(old)); sys.exit(1)
c=c.replace(old,new)
if sum(1 for ch in c if ord(ch)>127)!=hb: print("LOI high-byte"); sys.exit(1)
io.open(p,'w',encoding='latin-1',newline='').write(c); print("OK S9-KET (bat nam bep THAT)")
