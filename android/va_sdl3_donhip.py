# -*- coding: utf-8 -*-
r"""[DONHIP 12/09] Va ban SDL3 3.2.14 (ThirdParty/SDL3-src - KHONG nam trong git) cho BAN DO NHIP tren dien thoai that.

Muc dich: do xem tren may that (Galaxy Z Fold 7: man ngoai huong goc DOC, game khoa NGANG) swapchain Vulkan co bi dung lai MOI
KHUNG khong. SDL 3.2.14 tren Android dat preTransform = IDENTITY; man xoay khac huong goc -> vkQueuePresentKHR tra
VK_SUBOPTIMAL_KHR moi lan -> SDL dat needsSwapchainRecreate -> lan lay anh sau dung lai swapchain (cho GPU roi + tao lai).
LDPlayer co huong goc NGANG nen khong bao gio lo. Ban sua chinh thuc cua SDL: commit bca30aa (09/10/2025, nhanh release-3.2.x)
"Ignore VK_SUBOPTIMAL_KHR on the Android platform" (issue #12950).

Ba cho va, MAC DINH GIU NGUYEN hanh vi 3.2.14 (chi doi khi game dat hint):
  1. dem so lan VK_SUBOPTIMAL_KHR     -> hint "JX_DEM_SUBOPTIMAL"         (game doc bang SDL_GetHint)
  2. dem so lan dung lai swapchain     -> hint "JX_DEM_DUNG_LAI_SWAPCHAIN"
  3. hint "JX_BO_QUA_SUBOPTIMAL" = 1 (chi Android) -> KHONG dung lai swapchain vi SUBOPTIMAL (= ban sua bca30aa), bat/tat luc chay
Chay lai nhieu lan khong sao (co dau [DONHIP 12/09] thi bo qua).

Dung:  python android\va_sdl3_donhip.py [thu muc SDL3-3.2.14]     (mac dinh ThirdParty\SDL3-src\SDL3-3.2.14 cua cay nay)
"""
import io
import os
import sys

GOC = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
SDL = sys.argv[1] if len(sys.argv) > 1 else os.path.join(GOC, "ThirdParty", "SDL3-src", "SDL3-3.2.14")
P = os.path.join(SDL, "src", "gpu", "vulkan", "SDL_gpu_vulkan.c")
DAU = "[DONHIP 12/09]"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
    sys.exit(0)
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
          "// [DONHIP 12/09] JX1 mobile - ban do nhip: dem su kien swapchain qua hint (game doc bang SDL_GetHint). Xem android/va_sdl3_donhip.py",
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
          "    JX_DemSdl(\"JX_DEM_DUNG_LAI_SWAPCHAIN\", &JX_nDemDungLai); // [DONHIP 12/09]",
          ""], "dem dung lai swapchain")

# 3. SUBOPTIMAL sau vkQueuePresentKHR: dem; hint JX_BO_QUA_SUBOPTIMAL=1 (Android) = ban sua bca30aa
s = thay(s, ["            if (presentResult == VK_SUBOPTIMAL_KHR || presentResult == VK_ERROR_OUT_OF_DATE_KHR) {",
             "                presentData->windowData->needsSwapchainRecreate = true;",
             "            }"],
         ["            // [DONHIP 12/09] dem SUBOPTIMAL; hint JX_BO_QUA_SUBOPTIMAL=1 tren Android = ban sua bca30aa (khong dung lai swapchain vi man xoay)",
          "            if (presentResult == VK_SUBOPTIMAL_KHR) {",
          "                JX_DemSdl(\"JX_DEM_SUBOPTIMAL\", &JX_nDemSuboptimal);",
          "            }",
          "            if (presentResult == VK_ERROR_OUT_OF_DATE_KHR ||",
          "#ifdef SDL_PLATFORM_ANDROID",
          "                (presentResult == VK_SUBOPTIMAL_KHR && !SDL_GetHintBoolean(\"JX_BO_QUA_SUBOPTIMAL\", false))) {",
          "#else",
          "                presentResult == VK_SUBOPTIMAL_KHR) {",
          "#endif",
          "                presentData->windowData->needsSwapchainRecreate = true;",
          "            }"], "suboptimal")

if sum(1 for c in s if ord(c) >= 0x80) != CAO:
    raise SystemExit("so byte cao doi - khong ghi")
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va:", P)
