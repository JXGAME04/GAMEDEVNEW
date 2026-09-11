# -*- coding: utf-8 -*-
r"""[DONHIP 12/09 b] Va nguon SDL3 (ThirdParty/SDL3-src - KHONG nam trong git) cho BAN DO NHIP tren dien thoai that.
android/CMakeLists.txt tu goi kich ban nay luc configure (execute_process) -> MOI cay dung ra cung mot libSDL3.so
(10/09: APK -m dung o cay co SDL chua va -> pha 1 vo hieu, Fold 7 lai 28-45 fps).

Boi canh (BANGIAO_DONHIP_MOBILE_1209.md §8): SDL 3.2.14 tren Android dat preTransform = IDENTITY; man ngoai Fold 7 huong goc DOC,
game khoa NGANG -> vkQueuePresentKHR tra VK_SUBOPTIMAL_KHR moi khung -> SDL dung lai swapchain MOI KHUNG -> 35-61 fps, cho 13-17 ms/khung.
Commit SDL bca30aa (nhanh release-3.2.x, vao tu 3.2.26) "Ignore VK_SUBOPTIMAL_KHR on the Android platform" (issue #12950) boc cho dat
needsSwapchainRecreate trong #ifndef SDL_PLATFORM_ANDROID -> Fold 7 118-120 fps cung muc dien. Android dung SDL 3.2.30 (ban 3.2.x cuoi).

Ba cho va, MAC DINH = hanh vi 3.2.30 (chi doi khi game dat hint):
  1. dem so lan VK_SUBOPTIMAL_KHR sau present  -> hint "JX_DEM_SUBOPTIMAL"          (game doc bang SDL_GetHint)
  2. dem so lan dung lai swapchain              -> hint "JX_DEM_DUNG_LAI_SWAPCHAIN"
  3. hint "JX_DUNG_LAI_SUBOPTIMAL" = 1 (chi Android) -> dung lai swapchain moi khi SUBOPTIMAL = hanh vi 3.2.14, CHI de pha doi chung
     cua ban do nhip (JxPerfHudAndroid.cpp s_aDnPha nSdlCu), bat/tat luc chay.
Chay lai nhieu lan khong sao (co dau [DONHIP 12/09 b] thi bo qua). SDL < 3.2.26 (chua co bca30aa) -> bao loi, khong va.

Dung:  python android\va_sdl3_donhip.py [thu muc SDL3-3.2.30]     (mac dinh ThirdParty\SDL3-src\SDL3-3.2.30 cua cay nay)
"""
import io
import os
import re
import sys

GOC = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
SDL = sys.argv[1] if len(sys.argv) > 1 else os.path.join(GOC, "ThirdParty", "SDL3-src", "SDL3-3.2.30")
P = os.path.join(SDL, "src", "gpu", "vulkan", "SDL_gpu_vulkan.c")
DAU = "[DONHIP 12/09 b]"

# phien ban SDL: phai >= 3.2.26 (co bca30aa)
vh = io.open(os.path.join(SDL, "include", "SDL3", "SDL_version.h"), encoding="latin-1", newline="").read()
ver = tuple(int(re.search(r"define SDL_%s_VERSION\s+(\d+)" % k, vh).group(1)) for k in ("MAJOR", "MINOR", "MICRO"))
if ver < (3, 2, 26):
    raise SystemExit("SDL %d.%d.%d chua co bca30aa (can >= 3.2.26; Android dung 3.2.30 - chay android/tai_sdl3_src.ps1): %s" % (ver + (SDL,)))

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi (SDL %d.%d.%d), bo qua: %s" % (ver + (P,)))
    sys.exit(0)
if "[DONHIP 12/09]" in s:
    raise SystemExit("tep da mang ban va cu cho 3.2.14 - giai nen lai nguon SDL roi chay lai: " + P)
NL = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
CAO = sum(1 for c in s if ord(c) >= 0x80)


def thay(s, cu, moi, ten):
    c = NL.join(cu)
    n = s.count(c)
    if n != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (n, ten))
    return s.replace(c, NL.join(moi))


# 1. ham dem + hai bo dem, ngay sau macro clamp (truoc moi ham dung toi)
s = thay(s, ["#define VULKAN_INTERNAL_clamp(val, min, max) SDL_max(min, SDL_min(val, max))"],
         ["#define VULKAN_INTERNAL_clamp(val, min, max) SDL_max(min, SDL_min(val, max))",
          "",
          "// " + DAU + " JX1 mobile - ban do nhip: dem su kien swapchain qua hint (game doc bang SDL_GetHint). Xem android/va_sdl3_donhip.py",
          "static int JX_nDemSuboptimal = 0, JX_nDemDungLai = 0;",
          "static void JX_DemSdl(const char *ten, int *pDem)",
          "{",
          "    char buf[24];",
          "    *pDem += 1;",
          "    SDL_snprintf(buf, sizeof(buf), \"%d\", *pDem);",
          "    SDL_SetHint(ten, buf);",
          "}"], "ham dem")

# 2. dem so lan dung lai swapchain
s = thay(s, ["static Uint32 VULKAN_INTERNAL_RecreateSwapchain(",
             "    VulkanRenderer *renderer,",
             "    WindowData *windowData)",
             "{",
             "    Uint32 i;",
             ""],
         ["static Uint32 VULKAN_INTERNAL_RecreateSwapchain(",
          "    VulkanRenderer *renderer,",
          "    WindowData *windowData)",
          "{",
          "    Uint32 i;",
          "",
          "    JX_DemSdl(\"JX_DEM_DUNG_LAI_SWAPCHAIN\", &JX_nDemDungLai); // " + DAU,
          ""], "dem dung lai swapchain")

# 3. SUBOPTIMAL sau vkQueuePresentKHR (doan bca30aa): dem; Android + hint JX_DUNG_LAI_SUBOPTIMAL=1 -> dung lai nhu 3.2.14 (doi chung)
s = thay(s, ["// On the Android platform, VK_SUBOPTIMAL_KHR is returned whenever the device is rotated. We'll just ignore this for now.",
             "#ifndef SDL_PLATFORM_ANDROID",
             "            if (presentResult == VK_SUBOPTIMAL_KHR) {",
             "                presentData->windowData->needsSwapchainRecreate = true;",
             "            }",
             "#endif"],
         ["// On the Android platform, VK_SUBOPTIMAL_KHR is returned whenever the device is rotated. We'll just ignore this for now.",
          "            // " + DAU + " dem SUBOPTIMAL; tren Android hint JX_DUNG_LAI_SUBOPTIMAL=1 = hanh vi 3.2.14 (dung lai swapchain",
          "            // moi khi SUBOPTIMAL) chi de pha doi chung cua ban do nhip. Xem android/va_sdl3_donhip.py",
          "            if (presentResult == VK_SUBOPTIMAL_KHR) {",
          "                JX_DemSdl(\"JX_DEM_SUBOPTIMAL\", &JX_nDemSuboptimal);",
          "            }",
          "#ifndef SDL_PLATFORM_ANDROID",
          "            if (presentResult == VK_SUBOPTIMAL_KHR) {",
          "                presentData->windowData->needsSwapchainRecreate = true;",
          "            }",
          "#else",
          "            if (presentResult == VK_SUBOPTIMAL_KHR && SDL_GetHintBoolean(\"JX_DUNG_LAI_SUBOPTIMAL\", false)) {",
          "                presentData->windowData->needsSwapchainRecreate = true;",
          "            }",
          "#endif"], "suboptimal")

if sum(1 for c in s if ord(c) >= 0x80) != CAO:
    raise SystemExit("so byte cao doi - khong ghi")
# ghi nguyen tu (CMake cua hai ABI co the goi cung luc)
tmp = P + ".donhip.tmp"
io.open(tmp, "w", encoding="latin-1", newline="").write(s)
os.replace(tmp, P)
print("da va (SDL %d.%d.%d): %s" % (ver + (P,)))
