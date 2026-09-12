//---------------------------------------------------------------------------
// [IOS-DO 11/09] Do CPU / bo nho / nhiet / pin tren iOS bang API cua Apple.
//
// Ban Android doc /proc va /sys (JxPerfHudAndroid.cpp) - iOS khong co. Doi chieu:
//   CPU tien trinh   /proc/self/stat          -> task_info + thread_info (mach)
//   RAM tien trinh   /proc/self/statm         -> task_vm_info.phys_footprint  (DUNG con so ma
//                                                iOS dung de QUYET DINH GIET app - jetsam)
//   RAM con lai      /proc/meminfo            -> os_proc_available_memory()
//   Nhiet            /sys/class/thermal       -> NSProcessInfo.thermalState (0..3)
//   Pin              /sys/class/power_supply  -> UIDevice.batteryLevel
//   GPU %            /sys/class/kgsl ...      -> KHONG CO tren iOS neu khong dung API rieng tu; tra -1
//
// BAY: tep .mm nay KHONG duoc include "KWin32.h" (lop gia lap Win32 dat BOOL = int, con
// <objc/objc.h> dat BOOL = bool -> xung dot kieu). Vi vay chi bay ra mot ham C thuan.
//---------------------------------------------------------------------------
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include <mach/mach.h>
#include <os/proc.h>

extern "C" void JxIosDo_Lay(float* pCpuPhanTram, double* pRamMB, double* pRamConMB,
                            int* pNhiet, float* pPin)
{
	// ---- CPU: cong thoi gian CPU cua moi luong dang song ----
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
			*pCpuPhanTram = (float)(tong * 100.0);	// 100 % = MOT nhan chay het
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
		*pRamConMB = (double)os_proc_available_memory() / (1024.0 * 1024.0);

	// ---- nhiet: 0 = binh thuong, 1 = am, 2 = nong, 3 = nghiem trong ----
	if (pNhiet)
		*pNhiet = (int)[[NSProcessInfo processInfo] thermalState];

	// ---- pin ----
	if (pPin)
	{
		UIDevice* d = [UIDevice currentDevice];
		if (!d.batteryMonitoringEnabled) d.batteryMonitoringEnabled = YES;
		*pPin = d.batteryLevel;		// -1 neu chua doc duoc
	}
}
