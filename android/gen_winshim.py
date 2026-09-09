# -*- coding: utf-8 -*-
r"""[ANDROID 08/09] Sinh thu muc "winshim": cac header TEN Windows SDK / CRT MSVC (windows.h, winsock2.h, mmsystem.h, ddraw.h, d3d9.h ...)
ma ma nguon client #include truc tiep (49 tep windows.h, 16 winsock2.h, ...). Moi tep chi #include "KPosixCompat.h"
(pshpack*/poppack co #pragma pack that; initguid.h dat INITGUID). Thu muc nay CHI vao duong include cua ban Android
(CMake), ban Windows khong bao gio thay no -> khong phai sua 60+ dong #include trong nguon.
Chay:  python android/gen_winshim.py   -> Sources/Engine/Src/Platform/winshim/
"""
import io, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "Sources", "Engine", "Src", "Platform", "winshim")
NAMES = """windows.h winsock2.h winsock.h ws2tcpip.h mswsock.h wsipx.h windowsx.h winbase.h winuser.h wingdi.h winnt.h windef.h
winerror.h winreg.h winnls.h wincon.h winver.h shellapi.h shlwapi.h shlobj.h commdlg.h commctrl.h richedit.h mmsystem.h mmreg.h msacm.h
vfw.h dsound.h dinput.h ddraw.h d3d9.h d3dx9.h d3d9types.h d3d9caps.h d3d8.h d3dx8.h dxerr.h dxerr8.h dxerr9.h dxdiag.h dxsdkver.h imm.h
objbase.h ole2.h oleauto.h unknwn.h guiddef.h initguid.h basetsd.h atlbase.h atlstr.h atlconv.h atlcomcli.h crtdbg.h io.h direct.h
process.h tchar.h conio.h excpt.h eh.h intrin.h xmmintrin.h emmintrin.h mmintrin.h pmmintrin.h dbghelp.h imagehlp.h tlhelp32.h psapi.h
wininet.h winhttp.h wincrypt.h wintrust.h softpub.h iphlpapi.h ipexport.h iptypes.h icmpapi.h nb30.h lm.h lmcons.h sddl.h aclapi.h
userenv.h shfolder.h shobjidl.h comdef.h comutil.h mlang.h urlmon.h ocidl.h oaidl.h propidl.h powrprof.h setupapi.h winioctl.h winsvc.h
winspool.h dde.h ddeml.h winternl.h ntstatus.h mmdeviceapi.h audioclient.h dwmapi.h uxtheme.h olectl.h zmouse.h versionhelpers.h
fpieee.h share.h new.h iostream.h fstream.h strstrea.h minmax.h wtypes.h rpc.h rpcndr.h propkey.h
strsafe.h dshow.h vmr9.h evr.h mfapi.h wmsdk.h nspapi.h ws2spi.h af_irda.h ipmib.h netioapi.h gdiplus.h winscard.h
lmerr.h ntsecapi.h wbemidl.h activscp.h mshtml.h exdisp.h ntddscsi.h ntddndis.h
pshpack1.h pshpack2.h pshpack4.h pshpack8.h poppack.h""".split()
SPECIAL = {"pshpack1.h": "#pragma pack(push, 1)\n", "pshpack2.h": "#pragma pack(push, 2)\n", "pshpack4.h": "#pragma pack(push, 4)\n",
           "pshpack8.h": "#pragma pack(push, 8)\n", "poppack.h": "#pragma pack(pop)\n",
           "initguid.h": "#define INITGUID\n#include \"../KPosixCompat.h\"\n"}
# d3d9*.h / d3dx9.h: khi JX_D3D9MINI (Represent3 tren SDL_GPU) -> tap con d3d9mini.h (thu muc Represent3 phai co trong duong include)
for _n in ("d3d9.h", "d3dx9.h", "d3d9types.h", "d3d9caps.h"):
    SPECIAL[_n] = "#include \"../KPosixCompat.h\"\n#ifdef JX_D3D9MINI\n#include \"d3d9mini.h\"\n#endif\n"
os.makedirs(OUT, exist_ok=True)
n = 0
for name in sorted(set(NAMES)):
    p = os.path.join(OUT, name)
    body = SPECIAL.get(name, "#include \"../KPosixCompat.h\"\n")
    txt = ("/* [ANDROID 08/09] winshim: thay <%s> cua Windows SDK / CRT MSVC khi bien dich Android (JX_POSIX).\n"
           "   Sinh boi android/gen_winshim.py - KHONG sua tay. Ban Windows khong dung thu muc nay. */\n%s") % (name, body)
    io.open(p, "w", encoding="ascii", newline="\n").write(txt); n += 1
print("winshim:", n, "tep ->", OUT)
