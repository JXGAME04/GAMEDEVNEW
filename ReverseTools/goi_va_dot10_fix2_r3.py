# -*- coding: ascii -*-
# FIX-2 r3: BO NGOAI SUY. Do that: ban sao client dao chieu 2,0% so voi may chu 0,2%
# (gap 10 lan) => zigzag la do ngoai suy P+V vot qua da moi khi bot dung/re.
import io,sys
CRLF="\r\n"
def L(*a): return CRLF.join(a)
p=r"D:\GAMEDEVNEW\Sources\Core\Src\KProtocolProcess.cpp"
c=io.open(p,'r',encoding='latin-1',newline='').read(); hb=sum(1 for ch in c if ord(ch)>127)
old=L("\t\t\t\tint nS9DesX = NpcSync->MapX + nS9Vx;\t// ngoai suy mot nhip ve PHIA TRUOC",
      "\t\t\t\tint nS9DesY = NpcSync->MapY + nS9Vy;")
new=L("\t\t\t\t// [r3 26/08] BO NGOAI SUY. Ban r2 giao dich = P + V (mot nhip phia truoc) de bu",
      "\t\t\t\t// tre mang, nhung do that cho thay no VOT QUA DA moi khi bot dung lai hoac re:",
      "\t\t\t\t// ban sao client dao chieu (>120 do) 2,0% so buoc trong khi VI TRI MAY CHU chi",
      "\t\t\t\t// 0,2% - gap 10 lan; ca biet co bot 39% so buoc bi dao chieu trong khi may chu 0%.",
      "\t\t\t\t// May chu di rat thang (95,7% buoc re duoi 30 do) nen ngoai suy khong loi gi ma",
      "\t\t\t\t// chi tao zigzag. Giao DUNG vi tri may chu: ban sao luon chay THEO SAU mot nhip",
      "\t\t\t\t// (khong bao gio vuot len nen khong phai quay lai) va van lien tuc, vi moi goi",
      "\t\t\t\t// dong bo lai day dich len dung mot nhip nua.",
      "\t\t\t\tint nS9DesX = NpcSync->MapX;",
      "\t\t\t\tint nS9DesY = NpcSync->MapY;")
if c.count(old)!=1: print("LOI neo:",c.count(old)); sys.exit(1)
c=c.replace(old,new)
if sum(1 for ch in c if ord(ch)>127)!=hb: print("LOI high-byte"); sys.exit(1)
io.open(p,'w',encoding='latin-1',newline='').write(c); print("OK FIX-2 r3: bo ngoai suy")
