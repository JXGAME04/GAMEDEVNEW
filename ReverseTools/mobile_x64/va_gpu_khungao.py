# -*- coding: utf-8 -*-
"""[GPU 08/09 khung ao] phan 2: KSdlApp.cpp - toa do chuot cua so -> khung logic (SCREEN_WIDTH x SCREEN_HEIGHT) cung cong thuc letterbox
voi D3D9onGPU (scale = min, can giua). ASCII-only, giu CRLF cua tep, chay lai an toan."""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
TAG = "[GPU 08/09 khung ao]"
p = ROOT + r"\Sources\S3Client\Platform\KSdlApp.cpp"
b = io.open(p, "rb").read(); t = b.decode("latin-1"); o = t
CR = chr(13); LF = chr(10)
NL = CR + LF if (CR + LF) in t else LF
if TAG in t:
    print("KSdlApp.cpp: da va"); sys.exit(0)
HELP_LINES = [
    "// " + TAG + " cua so (toan man hinh / dien thoai) co the khac khung logic SCREEN_WIDTH x SCREEN_HEIGHT: bo ve (D3D9onGPU) phong",
    "// khung logic len cua so voi letterbox (scale = min, can giua) -> toa do chuot phai doi nguoc lai cung cong thuc.",
    "static void SdlToLogical(SDL_Window* pWin, float& x, float& y)",
    "{",
    "\tint w = 0, h = 0; SDL_GetWindowSize(pWin, &w, &h);",
    "\tif (w <= 0 || h <= 0 || SCREEN_WIDTH <= 0 || SCREEN_HEIGHT <= 0 || (w == SCREEN_WIDTH && h == SCREEN_HEIGHT)) return;",
    "\tfloat sx = (float)w / (float)SCREEN_WIDTH, sy = (float)h / (float)SCREEN_HEIGHT;",
    "\tfloat sc = (sx < sy) ? sx : sy;",
    "\tfloat ox = ((float)w - (float)SCREEN_WIDTH * sc) * 0.5f, oy = ((float)h - (float)SCREEN_HEIGHT * sc) * 0.5f;",
    "\tx = (x - ox) / sc; y = (y - oy) / sc;",
    "\tif (x < 0.0f) x = 0.0f; if (y < 0.0f) y = 0.0f;",
    "\tif (x > (float)(SCREEN_WIDTH - 1)) x = (float)(SCREEN_WIDTH - 1); if (y > (float)(SCREEN_HEIGHT - 1)) y = (float)(SCREEN_HEIGHT - 1);",
    "}",
    "",
]
HELP = NL.join(HELP_LINES) + NL
i = t.find("bool KSdlApp::TranslateEvent(const SDL_Event& ev)"); assert i > 0, "TranslateEvent"
t = t[:i] + HELP + t[i:]
def sub1(pat, repl, name):
    global t
    t2, n = re.subn(pat, repl, t, count=1); assert n == 1, "khong thay: " + name; t = t2
sub1(r"WPARAM w = SdlMouseFlags\(ev\.motion\.state\); LPARAM l = MAKELPARAM\(\(int\)ev\.motion\.x, \(int\)ev\.motion\.y\);",
     lambda m: "float mx = ev.motion.x, my = ev.motion.y; SdlToLogical(m_pWindow, mx, my);" + NL + "\t\tWPARAM w = SdlMouseFlags(ev.motion.state); LPARAM l = MAKELPARAM((int)mx, (int)my);", "motion")
sub1(r"WPARAM w = SdlMouseFlags\(SDL_GetMouseState\(NULL, NULL\)\); LPARAM l = MAKELPARAM\(\(int\)ev\.button\.x, \(int\)ev\.button\.y\);",
     lambda m: "float bx = ev.button.x, by = ev.button.y; SdlToLogical(m_pWindow, bx, by);" + NL + "\t\t\tWPARAM w = SdlMouseFlags(SDL_GetMouseState(NULL, NULL)); LPARAM l = MAKELPARAM((int)bx, (int)by);", "button")
sub1(r"([ \t]*)GhiChuot\(w, MAKELPARAM\(\(int\)ev\.wheel\.mouse_x, \(int\)ev\.wheel\.mouse_y\)\);\r?\n([ \t]*)MsgProc\(hWnd, WM_MOUSEWHEEL, w, MAKELPARAM\(\(int\)ev\.wheel\.mouse_x, \(int\)ev\.wheel\.mouse_y\)\);",
     lambda m: m.group(1) + "float wx = ev.wheel.mouse_x, wy = ev.wheel.mouse_y; SdlToLogical(m_pWindow, wx, wy);" + NL + m.group(1) + "GhiChuot(w, MAKELPARAM((int)wx, (int)wy));" + NL + m.group(2) + "MsgProc(hWnd, WM_MOUSEWHEEL, w, MAKELPARAM((int)wx, (int)wy));", "wheel")
assert sum(1 for c in o if ord(c) > 127) == sum(1 for c in t if ord(c) > 127)
io.open(p, "wb").write(t.encode("latin-1"))
print("KSdlApp.cpp: + SdlToLogical (motion/button/wheel), NL=%s" % ("CRLF" if NL == CR + LF else "LF"))
