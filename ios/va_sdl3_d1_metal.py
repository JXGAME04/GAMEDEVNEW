# -*- coding: utf-8 -*-
r"""[D1 13/09 Metal] Va nguon SDL3 3.2.30 (ThirdParty/SDL3-src - KHONG nam trong git; ios/CMakeLists.txt tu goi luc configure):
SWAPCHAIN (drawable Metal) THEO KHUNG LOGIC tren iOS - ban Metal cua android/va_sdl3_d1.py (Vulkan).

Vi sao: iPhone 17 Pro Max cua so 2868x1320 (3,8 Mpx) trong khi khung logic 1338x616 (0,82 Mpx) -> GPU to gap 4,6 lan so diem can
(Fold 7 sau D1: nop 1,6 -> 0,8 ms, GPU 83 % @648 -> 73 % @336 MHz, 4,61 -> 3,35 W). Metal: kich thuoc drawable = CAMetalLayer.drawableSize,
SDL dat = bounds x contentsScale trong SDL_uikitmetalview.m:updateDrawableSize (luc tao view va moi layoutSubviews). Drawable nho hon bounds
thi Core Animation phong len man khi ghep, KHONG them pass GPU. Hai cho va, MAC DINH = hanh vi 3.2.30 (chi doi khi game dat hint):
  1. SDL_uikitmetalview.m updateDrawableSize: hint "JX_SWAPCHAIN_W"/"JX_SWAPCHAIN_H" (> 0, nho hon kich thuoc tu nhien) -> drawableSize = hint;
     ghi "JX_SWAPCHAIN_THAT" = WxH (D3D9onGPUDev.cpp ghi [D1] vao jx_rep3.log). Hint 0 = nhu cu.
  2. SDL_gpu_metal.m METAL_SetSwapchainParameters: ap lai hint ngay (Reset cua game khi doi khung logic), khong doi layoutSubviews.
Game dat hint truoc SDL_ClaimWindowForGPUDevice va trong Reset (D3D9onGPUDev.cpp JxDatHintSwapchain, [Client] Rep3SwapchainLogic).
Chay lai nhieu lan khong sao (co dau [D1 13/09 Metal] thi bo qua).

Dung:  python3 ios/va_sdl3_d1_metal.py [thu muc SDL3-3.2.30]     (mac dinh ThirdParty/SDL3-src/SDL3-3.2.30 cua cay nay)
"""
import io
import os
import sys

GOC = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
SDL = sys.argv[1] if len(sys.argv) > 1 else os.path.join(GOC, "ThirdParty", "SDL3-src", "SDL3-3.2.30")
DAU = "[D1 13/09 Metal]"


def va(p, cu, moi, ten):
    s = io.open(p, encoding="latin-1", newline="").read()
    if DAU in s:
        print("da va roi, bo qua: %s" % os.path.basename(p))
        return
    NL = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    c = NL.join(cu)
    n = s.count(c)
    if n != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s trong %s" % (n, ten, p))
    s = s.replace(c, NL.join(moi))
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("da va %s: %s" % (ten, os.path.basename(p)))


# 1. view Metal: drawable theo hint
va(os.path.join(SDL, "src", "video", "uikit", "SDL_uikitmetalview.m"),
   ["- (void)updateDrawableSize",
    "{",
    "    CGSize size = self.bounds.size;",
    "    size.width *= self.layer.contentsScale;",
    "    size.height *= self.layer.contentsScale;",
    ""],
   ["- (void)updateDrawableSize",
    "{",
    "    CGSize size = self.bounds.size;",
    "    size.width *= self.layer.contentsScale;",
    "    size.height *= self.layer.contentsScale;",
    "    /* %s hint JX_SWAPCHAIN_W/H (game dat = khung logic, D3D9onGPUDev.cpp JxDatHintSwapchain) -> drawable nho hon," % DAU,
    "       Core Animation phong len man (khong them pass GPU) - ban Metal cua D1 tren Vulkan (android/va_sdl3_d1.py). Hint 0 = nhu cu. */",
    "    {",
    "        const char *jw = SDL_GetHint(\"JX_SWAPCHAIN_W\"), *jh = SDL_GetHint(\"JX_SWAPCHAIN_H\");",
    "        int iw = jw ? SDL_atoi(jw) : 0, ih = jh ? SDL_atoi(jh) : 0;",
    "        if (iw > 0 && ih > 0 && (CGFloat)iw < size.width && (CGFloat)ih < size.height) { size.width = iw; size.height = ih; }",
    "        { char jb[48]; SDL_snprintf(jb, sizeof(jb), \"%dx%d\", (int)size.width, (int)size.height); SDL_SetHint(\"JX_SWAPCHAIN_THAT\", jb); }",
    "    }",
    ""],
   "updateDrawableSize theo hint")

# 2. SetSwapchainParameters: ap lai hint ngay (Reset)
va(os.path.join(SDL, "src", "gpu", "metal", "SDL_gpu_metal.m"),
   ["        METAL_Wait(driverData);",
    "",
    "        windowData->presentMode = SDL_GPU_PRESENTMODE_VSYNC;"],
   ["        METAL_Wait(driverData);",
    "",
    "        /* %s hint doi (Reset / khung logic doi) -> ap lai drawableSize ngay, khong doi layoutSubviews */" % DAU,
    "        {",
    "            const char *jw = SDL_GetHint(\"JX_SWAPCHAIN_W\"), *jh = SDL_GetHint(\"JX_SWAPCHAIN_H\");",
    "            int iw = jw ? SDL_atoi(jw) : 0, ih = jh ? SDL_atoi(jh) : 0;",
    "            CGSize nat = windowData->layer.bounds.size;",
    "            nat.width *= windowData->layer.contentsScale;",
    "            nat.height *= windowData->layer.contentsScale;",
    "            CGSize muon = nat;",
    "            if (iw > 0 && ih > 0 && (CGFloat)iw < nat.width && (CGFloat)ih < nat.height) { muon.width = iw; muon.height = ih; }",
    "            if (muon.width > 0 && muon.height > 0 &&",
    "                (windowData->layer.drawableSize.width != muon.width || windowData->layer.drawableSize.height != muon.height)) {",
    "                windowData->layer.drawableSize = muon;",
    "                { char jb[48]; SDL_snprintf(jb, sizeof(jb), \"%dx%d\", (int)muon.width, (int)muon.height); SDL_SetHint(\"JX_SWAPCHAIN_THAT\", jb); }",
    "            }",
    "        }",
    "",
    "        windowData->presentMode = SDL_GPU_PRESENTMODE_VSYNC;"],
   "SetSwapchainParameters ap hint")
print("xong %s" % DAU)
