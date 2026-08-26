# -*- coding: ascii -*-
import io, sys
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KNpc.cpp"
CRLF="\r\n"
def L(*a): return CRLF.join(a)
c = io.open(p,'r',encoding='latin-1',newline='').read()
hb = sum(1 for ch in c if ord(ch)>127)
ok=True
pairs=[]
# A2b
pairs.append((
L("\t\tm_Doing = do_stand;",
  "\t\tm_Frames.nCurrentFrame = 0;",
  "\t\tGetMpsPos(&m_DesX, &m_DesY);"),
L("\t\tm_Doing = do_stand;",
  "\t\t// [FIX-1 26/08] Dung han thi khong con 'dang bi chan' nua - xoa bo dem, neu khong no",
  "\t\t// giu quyen ghi de toa do cua goi dong bo mai mai (do that: co leo toi 304 qua 53 giay).",
  "\t\tm_nNeedFixPos = 0;",
  "\t\tm_Frames.nCurrentFrame = 0;",
  "\t\tGetMpsPos(&m_DesX, &m_DesY);")))
# D1
pairs.append((
L("\t}",
  "",
  "\tm_DataRes.SetAction(m_ClientDoing);",
  "\tm_DataRes.SetRideHorse(m_bRideHorse);"),
L("\t}",
  "",
  "#ifndef _SERVER",
  "\t// [S9-VE 26/08] Nhan DUY NHAT nhin thay LOP VE. Toan bo he log truoc day chi chup trang",
  "\t// thai logic (m_Doing/m_ClientDoing) nen khi nguoi choi bao 'nam bep duoi dat' thi khong",
  "\t// the phan biet loi o KNpc, o KNpcRes hay o Represent. Chi ghi cho CHINH NHAN VAT, va chi",
  "\t// khi tu the DOI (hoac 2 giay mot lan) nen rat nhe.",
  "\t// Doc: resdoing=8 => loi o lop ve; cdoing=8 => loi o KNpc; ca ba deu 1 ma van nam => Represent.",
  "\tif (m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex)",
  "\t{",
  "\t\tstatic DWORD s_uS9VeT = 0;",
  "\t\tstatic int s_nS9VeCu = -999;",
  "\t\tDWORD uS9Now = timeGetTime();",
  "\t\tif (s_nS9VeCu != (int)m_ClientDoing || (DWORD)(uS9Now - s_uS9VeT) >= 2000)",
  "\t\t{",
  "\t\t\ts_uS9VeT = uS9Now;",
  "\t\t\ts_nS9VeCu = (int)m_ClientDoing;",
  "\t\t\tAUTOLOG(\"[S9-VE] doing=%d cdoing=%d resdoing=%d resaction=%d frame=%d/%d reg=%d t=%u\", (int)m_Doing, (int)m_ClientDoing, m_DataRes.m_nDoing, m_DataRes.m_nAction, m_Frames.nCurrentFrame, m_Frames.nTotalFrame, m_RegionIndex, SubWorld[0].m_dwCurrentTime);",
  "\t\t}",
  "\t}",
  "#endif",
  "\tm_DataRes.SetAction(m_ClientDoing);",
  "\tm_DataRes.SetRideHorse(m_bRideHorse);")))
for old,new in pairs:
    n=c.count(old)
    if n!=1:
        print("LOI: neo khop %d lan"%n); ok=False; break
    c=c.replace(old,new)
if ok:
    if sum(1 for ch in c if ord(ch)>127)!=hb:
        print("LOI high-byte doi"); sys.exit(1)
    io.open(p,'w',encoding='latin-1',newline='').write(c)
    print("OK KNpc.cpp (2 mieng, high-byte %d giu nguyen)"%hb)
sys.exit(0 if ok else 1)
