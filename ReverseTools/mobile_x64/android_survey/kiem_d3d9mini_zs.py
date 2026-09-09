# -*- coding: utf-8 -*-
"""[GPU 08/09] Kiem cu phap (cl /Zs) cac tep Represent3 (logic + D3D9onGPU, tru D3D9on11) voi /DJX_D3D9MINI - lay dong lenh CL tu log build ReleaseSDL."""
import io, sys, re, subprocess, glob, os
LOGS = r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\f7fe3690-d1f7-481d-b4f6-845330c7c14f\scratchpad\logs"
REP3 = r"D:\GAMEDEVNEW_wt_mobile\Sources\Represent\Represent3"
VC = r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
log = sorted(glob.glob(os.path.join(LOGS, "rep3_sdl_*.log")), key=os.path.getmtime)[-1]
L = io.open(log, encoding="utf-8", errors="replace").read().splitlines()
cl = [l for l in L if "CL.exe" in l and " /c " in l][0].strip()
i = cl.find("CL.exe") + 6
toks = re.findall(r'"[^"]*"|\S+', cl[i:])
toks = [t for t in toks if all(ord(c) < 128 for c in t)]
toks = [t for t in toks if not t.lower().rstrip('"').endswith((".cpp", ".c")) and t != "/c" and not t.startswith(("/Fo", "/Fd", "/Fp", "/Yu", "/Yc", "/FS", "/MP", "/Fa"))]
files = sys.argv[1:] or ["KRepresentShell3.cpp", "TextureRes.cpp", "TextureResMgr.cpp", "D3D_Device.cpp", "D3D_Shell.cpp", "D3D_Utils.cpp", "D3D9onGPU.cpp", "D3D9onGPUDev.cpp", "D3D9onGPURes.cpp"]
cmd = 'call "' + VC + '" >nul && cd /d ' + REP3 + ' && "' + cl[:i] + '" ' + " ".join(toks) + ' /Zs /DJX_D3D9MINI ' + " ".join(files)
cmdfile = os.path.join(REP3, "x64", "ReleaseSDL", "zs_mini.cmd")
io.open(cmdfile, "w", encoding="ascii", errors="replace", newline="\r\n").write(cmd + "\n")
r = subprocess.run(["cmd", "/c", cmdfile], capture_output=True, text=True, errors="replace")
out = (r.stdout + r.stderr)
errs = [l for l in out.splitlines() if re.search(r"error [A-Z]\d+|fatal error", l)]
print("rc", r.returncode, "| dong loi:", len(errs))
seen = set()
for e in errs:
    k = re.sub(r"\(\d+,\d+\)", "", e)[:170]
    if k in seen: continue
    seen.add(k); print("  ", e.replace(REP3 + "\\", "")[:230])
    if len(seen) >= 60: break
if not errs: print(out[-600:])
