# -*- coding: utf-8 -*-
"""[SOCK 04/09] Sua GOC lam hong luong: MultiServer/Common/SocketServer.cpp khi WSASend ghi THIEU byte.
 Ma cu (enumIO_Write_Completed): Use(dwIoSize) -> WriteCompleted() chi printf 'not all data was written' -> Release buffer.
 Phan chua gui BI VUT -> luong TCP mat mot khuc GIUA goi -> client doc truong do dai roi vao giua du lieu ->
 xu ly rac -> sap (chuoi sap 04/09 00:21-00:59).
 Vá: khi ghi thieu thi GUI NOT phan con lai ngay tren cung socket (cung hang doi ghi, dung thu tu) truoc khi tra buffer.
 Neu khong gui not duoc (socket dang dong / het bo dem) thi DONG KET NOI han hoi thay vi de client doc rac:
 mat ket noi con hon vo bo nho.
 Tep nay bien vao common.lib -> heaven.dll (GameServer.exe nap). Doc/ghi latin-1 newline=''. Neo kiem dem == 1."""
import io, os

P = r"D:\GAMEDEVNEW_wt_bc\Sources\MultiServer\Common\SocketServer.cpp"
MARK = "[SOCK 04/09]"

def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f: return f.read()
def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f: f.write(s)

s = rd(P); N = "\r\n" if "\r\n" in s else "\n"
if MARK in s:
    print("da va"); raise SystemExit

# 1. khoi hoan tat ghi: gui not phan con lai
old = ("            pBuffer->Use( dwIoSize );" + N +
       "\t\t\t" + N +
       "\t\t\tm_server.WriteCompleted( pSocket, pBuffer );" + N)
if old not in s:
    i = s.index("            pBuffer->Use( dwIoSize );")
    j = s.index("m_server.WriteCompleted( pSocket, pBuffer );", i)
    j = s.index(N, j) + len(N)
    old = s[i:j]
new = N.join([
    "            pBuffer->Use( dwIoSize );",
    "",
    "\t\t\t// %s WSASend co the ghi THIEU byte khi socket nghen. Ban cu chi in mot dong roi Release buffer," % MARK,
    "\t\t\t// tuc VUT phan chua gui -> luong TCP mat mot khuc giua goi -> client doc rac -> sap.",
    "\t\t\t// Nay gui not phan con lai ngay (cung hang doi ghi nen dung thu tu); khong gui duoc thi dong ket noi.",
    "\t\t\tif ( pBuffer->GetUsed() < pBuffer->GetWSABUF()->len )",
    "\t\t\t{",
    "\t\t\t\tconst size_t nDaGui  = pBuffer->GetUsed();",
    "\t\t\t\tconst size_t nConLai = (size_t)pBuffer->GetWSABUF()->len - nDaGui;",
    "\t\t\t\tprintf(\"--CSocketServer: ghi thieu %u/%u byte -> gui not %u byte--\\n\",",
    "\t\t\t\t\t(unsigned)nDaGui, (unsigned)pBuffer->GetWSABUF()->len, (unsigned)nConLai);",
    "\t\t\t\tif ( !pSocket->Write( pBuffer->GetBuffer() + nDaGui, nConLai ) )",
    "\t\t\t\t{",
    "\t\t\t\t\tprintf(\"--CSocketServer: khong gui not duoc -> dong ket noi de client khong doc rac--\\n\");",
    "\t\t\t\t\tpSocket->AbortiveClose();",
    "\t\t\t\t}",
    "\t\t\t}",
    "",
    "\t\t\tm_server.WriteCompleted( pSocket, pBuffer );",
    "",
])
assert s.count(old) == 1
s = s.replace(old, new, 1)

# 2. bo dong printf cu trong WriteCompleted (nay da xu ly o tren)
old2 = ("\t\t//OnError( _T(\"CSocketServer::WorkerThread::WriteCompleted - Socket write where not all data was written\") );" + N +
        "\t\tprintf(\"--CSocketServer::WorkerThread::WriteCompleted - Socket write where not all data was written--\\n\");" + N)
if old2 in s:
    s = s.replace(old2, "\t\t// %s da xu ly o enumIO_Write_Completed (gui not phan con lai), khong in nua" % MARK + N, 1)
    print("da bo printf cu")
wr(P, s)
print("SocketServer.cpp OK")
