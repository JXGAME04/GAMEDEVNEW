//---------------------------------------------------------------------------
// [IOS-DO 11/09] Do CPU / bo nho / nhiet / pin tren iOS bang API cua Apple.
//
// Ban Android doc /proc va /sys (JxPerfHudAndroid.cpp) - iOS khong co. Doi chieu:
//   CPU tien trinh   /proc/self/stat          -> task_info + thread_info (mach)
//   RAM tien trinh   /proc/self/statm         -> task_vm_info.phys_footprint  (DUNG con so ma
//                                                iOS dung de QUYET DINH GIET app - jetsam)
//   RAM con lai      /proc/meminfo            -> os_proc_available_memory()
//   Nhiet            /sys/class/thermal       -> NSProcessInfo.thermalState (0..3)
//   Pin              /sys/class/power_supply  -> SDL_GetPowerInfo (chay ca iOS lan macOS)
//   GPU %            /sys/class/kgsl ...      -> KHONG CO tren iOS neu khong dung API rieng tu; tra -1
//
// BAY: tep .mm nay KHONG duoc include "KWin32.h" (lop gia lap Win32 dat BOOL = int, con
// <objc/objc.h> dat BOOL = bool -> xung dot kieu). Vi vay chi bay ra mot ham C thuan.
//---------------------------------------------------------------------------
#import <Foundation/Foundation.h>
#include <TargetConditionals.h>
#include <mach/mach.h>
#include <SDL3/SDL.h>
#include <unistd.h>		// sysconf: so nhan
#if TARGET_OS_IPHONE
#include <os/proc.h>		// os_proc_available_memory: CHI co tren iOS
#endif

// [DONHIP 11/09 CPU] So nhan logic, lay MOT LAN. Dung de quy CPU ve thang 0..100 % cua CA MAY,
// giong het ban Android (JxPerfHudAndroid.cpp:113 nhan thoi gian thuc voi s_nCores).
// Thieu buoc chia nay thi con so la "% cua MOT nhan": iPhone 6 nhan co the len toi 600 %,
// nen chu thay "CPU 130 %" va tuong bo do hong.
static int JxIosDo_SoNhan(void)
{
	static int s_nNhan = 0;
	if (s_nNhan == 0)
	{
		long n = sysconf(_SC_NPROCESSORS_ONLN);
		s_nNhan = (n > 0) ? (int)n : 1;
	}
	return s_nNhan;
}

extern "C" void JxIosDo_Lay(float* pCpuPhanTram, double* pRamMB, double* pRamConMB,
                            int* pNhiet, float* pPin)
{
	// ---- CPU: cong thoi gian CPU cua moi luong dang song, roi chia so nhan ----
	if (pCpuPhanTram)
	{
		*pCpuPhanTram = -1.0f;
		thread_array_t     ds = NULL;
		mach_msg_type_number_t nDs = 0;
		if (task_threads(mach_task_self(), &ds, &nDs) == KERN_SUCCESS)
		{
			double tong = 0.0;
			for (mach_msg_type_number_t i = 0; i < nDs; i++)
			{
				thread_basic_info_data_t ti;
				mach_msg_type_number_t nTi = THREAD_BASIC_INFO_COUNT;
				if (thread_info(ds[i], THREAD_BASIC_INFO, (thread_info_t)&ti, &nTi) == KERN_SUCCESS &&
				    !(ti.flags & TH_FLAGS_IDLE))
					tong += ti.cpu_usage / (double)TH_USAGE_SCALE;
			}
			vm_deallocate(mach_task_self(), (vm_offset_t)ds, nDs * sizeof(thread_t));
			double dPt = tong * 100.0 / (double)JxIosDo_SoNhan();	// 100 % = CA MAY chay het (nhu ban Android)
			if (dPt > 100.0) dPt = 100.0;	// ban Android cung kep o 100 % (nPm > 1000 -> 1000)
			*pCpuPhanTram = (float)dPt;
		}
	}

	// ---- RAM: phys_footprint la con so jetsam dung de quyet dinh giet app ----
	if (pRamMB)
	{
		*pRamMB = -1.0;
		task_vm_info_data_t vi;
		mach_msg_type_number_t n = TASK_VM_INFO_COUNT;
		if (task_info(mach_task_self(), TASK_VM_INFO, (task_info_t)&vi, &n) == KERN_SUCCESS)
			*pRamMB = (double)vi.phys_footprint / (1024.0 * 1024.0);
	}
	if (pRamConMB)
	{
#if TARGET_OS_IPHONE
		*pRamConMB = (double)os_proc_available_memory() / (1024.0 * 1024.0);
#else
		// macOS: khong co os_proc_available_memory -> lay RAM tu do cua may
		*pRamConMB = -1.0;
		vm_size_t nTrang = 0; mach_port_t host = mach_host_self();
		vm_statistics64_data_t vm; mach_msg_type_number_t nDem = HOST_VM_INFO64_COUNT;
		if (host_page_size(host, &nTrang) == KERN_SUCCESS &&
		    host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&vm, &nDem) == KERN_SUCCESS)
			*pRamConMB = (double)((uint64_t)(vm.free_count + vm.inactive_count) * (uint64_t)nTrang) / (1024.0 * 1024.0);
#endif
	}

	// ---- nhiet: 0 = binh thuong, 1 = am, 2 = nong, 3 = nghiem trong ----
	if (pNhiet)
		*pNhiet = (int)[[NSProcessInfo processInfo] thermalState];

	// ---- pin: dung SDL cho ca iOS lan macOS, khoi phai keo UIKit vao ----
	if (pPin)
	{
		int nPhanTram = -1;
		SDL_GetPowerInfo(NULL, &nPhanTram);
		*pPin = (nPhanTram >= 0) ? (float)nPhanTram / 100.0f : -1.0f;
	}
}
