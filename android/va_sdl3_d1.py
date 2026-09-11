# -*- coding: utf-8 -*-
r"""[D1 11/09] Va nguon SDL3 3.2.30 (ThirdParty/SDL3-src - KHONG nam trong git; android/CMakeLists.txt tu goi luc configure, SAU va_sdl3_donhip.py):
SWAPCHAIN THEO KHUNG LOGIC tren Android. Log Fold 7 14:16 (PHUONGAN_FPS_NHIET_PIN_MOBILE_1109.md, BANGIAO_DONHIP_MOBILE_1209.md 14:50): luc dong
tren man trong 2184x1968 GPU ban 82-99 % o 650-950 MHz trong khi game chi ve khung logic 1040x936 -> GPU to gap 4,4 lan so diem can.
SDL 3.2.30 dat kich thuoc swapchain = SDL_GetWindowSizeInPixels kep trong [minImageExtent, maxImageExtent]; loader Vulkan Android cho extent
nho hon cua so (HWC/DPU phong len man, khong them pass GPU). Ba cho va, MAC DINH = hanh vi 3.2.30 (chi doi khi game dat hint):
  1. hint "JX_SWAPCHAIN_W" / "JX_SWAPCHAIN_H" (> 0) = kich thuoc swapchain mong muon (D3D9onGPUDev.cpp dat = backbuffer x Rep3SwapchainLogic/100);
     kep trong [min,max]ImageExtent; ghi lai "JX_SWAPCHAIN_THAT" = WxH da tao, "JX_SWAPCHAIN_EXTENT" = min/max/current de game ghi vao jx_rep3.log.
  2. vkCreateSwapchainKHR tu choi extent theo hint -> tat hint (JX_bSwapchainHintTat), tao lai voi kich thuoc cua so (mot lan, co log loi).
  3. acquire: hint doi so voi lan tao gan nhat -> needsSwapchainRecreate (gap/mo doi khung logic); dung lai > 8 lan trong 2 s khi dang dung hint
     -> tat hint (chong lap vo han kieu M1).
Chay lai nhieu lan khong sao (co dau [D1 11/09] thi bo qua). Can co ban va [DONHIP 12/09 b] truoc (ham JX_DemSdl).

Dung:  python android\va_sdl3_d1.py [thu muc SDL3-3.2.30]     (mac dinh ThirdParty\SDL3-src\SDL3-3.2.30 cua cay nay)
"""
import io
import os
import sys

GOC = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
SDL = sys.argv[1] if len(sys.argv) > 1 else os.path.join(GOC, "ThirdParty", "SDL3-src", "SDL3-3.2.30")
P = os.path.join(SDL, "src", "gpu", "vulkan", "SDL_gpu_vulkan.c")
DAU = "[D1 11/09]"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua: %s" % P)
    sys.exit(0)
if "[DONHIP 12/09 b]" not in s:
    raise SystemExit("chua co ban va [DONHIP 12/09 b] (chay android/va_sdl3_donhip.py truoc): " + P)
NL = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
CAO = sum(1 for c in s if ord(c) >= 0x80)


def thay(s, cu, moi, ten):
    c = NL.join(cu)
    n = s.count(c)
    if n != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (n, ten))
    return s.replace(c, NL.join(moi))


# 1. bien + ham doc hint, ngay sau JX_DemSdl cua [DONHIP 12/09 b]
s = thay(s, ["static void JX_DemSdl(const char *ten, int *pDem)",
             "{",
             "    char buf[24];",
             "    *pDem += 1;",
             "    SDL_snprintf(buf, sizeof(buf), \"%d\", *pDem);",
             "    SDL_SetHint(ten, buf);",
             "}"],
         ["static void JX_DemSdl(const char *ten, int *pDem)",
          "{",
          "    char buf[24];",
          "    *pDem += 1;",
          "    SDL_snprintf(buf, sizeof(buf), \"%d\", *pDem);",
          "    SDL_SetHint(ten, buf);",
          "}",
          "",
          "// " + DAU + " JX1 mobile: swapchain theo KHUNG LOGIC (hint JX_SWAPCHAIN_W / JX_SWAPCHAIN_H > 0 do game dat) - HWC/DPU phong len man, GPU to it diem hon.",
          "// Extent ngoai [min,max]ImageExtent thi kep; vkCreateSwapchainKHR tu choi -> tat hint va tao lai voi kich thuoc cua so. Xem android/va_sdl3_d1.py",
          "static Uint32 JX_uSwapchainHintW = 0, JX_uSwapchainHintH = 0; // hint da dung o lan tao swapchain gan nhat (0 = khong dung)",
          "static bool JX_bSwapchainHintTat = false;",
          "static Uint64 JX_uDungLaiLuc = 0; static int JX_nDungLaiNhanh = 0;",
          "static bool JX_LayHintSwapchain(Uint32 *pW, Uint32 *pH)",
          "{",
          "    const char *hw = SDL_GetHint(\"JX_SWAPCHAIN_W\"), *hh = SDL_GetHint(\"JX_SWAPCHAIN_H\");",
          "    int w = hw ? SDL_atoi(hw) : 0, h = hh ? SDL_atoi(hh) : 0;",
          "    if (JX_bSwapchainHintTat || w <= 0 || h <= 0) { *pW = 0; *pH = 0; return false; }",
          "    *pW = (Uint32)w; *pH = (Uint32)h;",
          "    return true;",
          "}"], "ham doc hint")

# 2. kich thuoc swapchain (non-Apple): hint thay cho cua so, kep trong [min,max]; ghi lai hint JX_SWAPCHAIN_THAT / JX_SWAPCHAIN_EXTENT
s = thay(s, ["#else",
             "    windowData->width = SDL_clamp(",
             "        windowData->swapchainCreateWidth,",
             "        swapchainSupportDetails.capabilities.minImageExtent.width,",
             "        swapchainSupportDetails.capabilities.maxImageExtent.width);",
             "    windowData->height = SDL_clamp(windowData->swapchainCreateHeight,",
             "        swapchainSupportDetails.capabilities.minImageExtent.height,",
             "        swapchainSupportDetails.capabilities.maxImageExtent.height);",
             "#endif"],
         ["#else",
          "    {   // " + DAU + " hint JX_SWAPCHAIN_W/H (JX1 mobile) thay cho kich thuoc cua so; kep trong [min,max]ImageExtent",
          "        Uint32 jxW = windowData->swapchainCreateWidth, jxH = windowData->swapchainCreateHeight, jxHW = 0, jxHH = 0;",
          "        const bool jxHint = JX_LayHintSwapchain(&jxHW, &jxHH);",
          "        char jxBuf[96];",
          "        if (jxHint) { jxW = jxHW; jxH = jxHH; }",
          "        JX_uSwapchainHintW = jxHint ? jxHW : 0; JX_uSwapchainHintH = jxHint ? jxHH : 0;",
          "        windowData->width = SDL_clamp(jxW,",
          "            swapchainSupportDetails.capabilities.minImageExtent.width,",
          "            swapchainSupportDetails.capabilities.maxImageExtent.width);",
          "        windowData->height = SDL_clamp(jxH,",
          "            swapchainSupportDetails.capabilities.minImageExtent.height,",
          "            swapchainSupportDetails.capabilities.maxImageExtent.height);",
          "        SDL_snprintf(jxBuf, sizeof(jxBuf), \"%ux%u\", windowData->width, windowData->height);",
          "        SDL_SetHint(\"JX_SWAPCHAIN_THAT\", jxBuf);",
          "        SDL_snprintf(jxBuf, sizeof(jxBuf), \"min %ux%u max %ux%u cua so %ux%u hint %ux%u\",",
          "            swapchainSupportDetails.capabilities.minImageExtent.width, swapchainSupportDetails.capabilities.minImageExtent.height,",
          "            swapchainSupportDetails.capabilities.maxImageExtent.width, swapchainSupportDetails.capabilities.maxImageExtent.height,",
          "            windowData->swapchainCreateWidth, windowData->swapchainCreateHeight, jxHW, jxHH);",
          "        SDL_SetHint(\"JX_SWAPCHAIN_EXTENT\", jxBuf);",
          "    }",
          "#endif"], "kich thuoc swapchain")

# 3. vkCreateSwapchainKHR tu choi extent theo hint -> tat hint, tao lai (ham tu tao lai surface o dau)
s = thay(s, ["    if (vulkanResult != VK_SUCCESS) {",
             "        renderer->vkDestroySurfaceKHR(",
             "            renderer->instance,",
             "            windowData->surface,",
             "            NULL);",
             "        windowData->surface = VK_NULL_HANDLE;",
             "        CHECK_VULKAN_ERROR_AND_RETURN(vulkanResult, vkCreateSwapchainKHR, false);",
             "    }"],
         ["    if (vulkanResult != VK_SUCCESS) {",
          "        renderer->vkDestroySurfaceKHR(",
          "            renderer->instance,",
          "            windowData->surface,",
          "            NULL);",
          "        windowData->surface = VK_NULL_HANDLE;",
          "        if (JX_uSwapchainHintW && !JX_bSwapchainHintTat) { // " + DAU + " extent theo hint bi tu choi -> thu lai mot lan voi kich thuoc cua so",
          "            SDL_LogError(SDL_LOG_CATEGORY_GPU, \"[D1] vkCreateSwapchainKHR %s voi hint %ux%u -> tat hint, dung kich thuoc cua so\", VkErrorMessages(vulkanResult), JX_uSwapchainHintW, JX_uSwapchainHintH);",
          "            JX_bSwapchainHintTat = true;",
          "            SDL_SetHint(\"JX_SWAPCHAIN_THAT\", \"tat (bi tu choi)\");",
          "            return VULKAN_INTERNAL_CreateSwapchain(renderer, windowData);",
          "        }",
          "        CHECK_VULKAN_ERROR_AND_RETURN(vulkanResult, vkCreateSwapchainKHR, false);",
          "    }"], "tu choi extent")

# 4. acquire: hint doi so voi lan tao gan nhat -> dung lai swapchain
s = thay(s, ["    // If window data marked as needing swapchain recreate, try to recreate",
             "    if (windowData->needsSwapchainRecreate) {"],
         ["    {   // " + DAU + " hint JX_SWAPCHAIN_W/H doi so voi lan tao gan nhat (gap/mo doi khung logic, doi Rep3SwapchainLogic) -> dung lai swapchain",
          "        Uint32 jxW = 0, jxH = 0;",
          "        JX_LayHintSwapchain(&jxW, &jxH);",
          "        if (jxW != JX_uSwapchainHintW || jxH != JX_uSwapchainHintH) {",
          "            windowData->needsSwapchainRecreate = true;",
          "        }",
          "    }",
          "    // If window data marked as needing swapchain recreate, try to recreate",
          "    if (windowData->needsSwapchainRecreate) {"], "acquire")

# 5. chong lap dung lai vo han khi dang dung hint (kieu M1): > 8 lan trong 2 s -> tat hint
s = thay(s, ["    JX_DemSdl(\"JX_DEM_DUNG_LAI_SWAPCHAIN\", &JX_nDemDungLai); // [DONHIP 12/09 b]"],
         ["    JX_DemSdl(\"JX_DEM_DUNG_LAI_SWAPCHAIN\", &JX_nDemDungLai); // [DONHIP 12/09 b]",
          "    {   // " + DAU + " dung lai lien tuc khi dang dung hint kich thuoc -> tat hint (chong lap vo han)",
          "        const Uint64 jxNow = SDL_GetTicks();",
          "        if (jxNow - JX_uDungLaiLuc < 2000) { JX_nDungLaiNhanh += 1; } else { JX_nDungLaiNhanh = 0; }",
          "        JX_uDungLaiLuc = jxNow;",
          "        if (JX_nDungLaiNhanh >= 8 && JX_uSwapchainHintW && !JX_bSwapchainHintTat) {",
          "            SDL_LogError(SDL_LOG_CATEGORY_GPU, \"[D1] dung lai swapchain %d lan trong 2 s voi hint %ux%u -> tat hint\", JX_nDungLaiNhanh, JX_uSwapchainHintW, JX_uSwapchainHintH);",
          "            JX_bSwapchainHintTat = true;",
          "            SDL_SetHint(\"JX_SWAPCHAIN_THAT\", \"tat (dung lai lien tuc)\");",
          "        }",
          "    }"], "chong lap")

if sum(1 for c in s if ord(c) >= 0x80) != CAO:
    raise SystemExit("so byte cao doi - khong ghi")
tmp = P + ".d1.tmp"
io.open(tmp, "w", encoding="latin-1", newline="").write(s)
os.replace(tmp, P)
print("da va %s: %s" % (DAU, P))
