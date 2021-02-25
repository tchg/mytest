extern "C" {
#pragma once
#include <Ntddk.h>
#include <ntdef.h>
#include <basetsd.h>
/*
example
DWORD Tzm[] = { 0x48,0x8b,0x05,0x47,0x81,0xc7,0xff };
	
KdPrint(("结果地址：%llx\n", GetSystemFunByTzm(Tzm, 7)));

*/


//系统信息类
typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation = 0,						//系统的基本信息
	SystemProcessorInformation,             // obsolete...delete
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,						//系统进程信息
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation,					//系统模块信息
	SystemLocksInformation,
	SystemStackTraceInformation,
	SystemPagedPoolInformation,
	SystemNonPagedPoolInformation,
	SystemHandleInformation,
	SystemObjectInformation,
	SystemPageFileInformation,
	SystemVdmInstemulInformation,
	SystemVdmBopInformation,
	SystemFileCacheInformation,
	SystemPoolTagInformation,
	SystemInterruptInformation,
	SystemDpcBehaviorInformation,
	SystemFullMemoryInformation,
	SystemLoadGdiDriverInformation,
	SystemUnloadGdiDriverInformation,
	SystemTimeAdjustmentInformation,
	SystemSummaryMemoryInformation,
	SystemMirrorMemoryInformation,
	SystemPerformanceTraceInformation,
	SystemObsolete0,
	SystemExceptionInformation,
	SystemCrashDumpStateInformation,
	SystemKernelDebuggerInformation,
	SystemContextSwitchInformation,
	SystemRegistryQuotaInformation,
	SystemExtendServiceTableInformation,
	SystemPrioritySeperation,
	SystemVerifierAddDriverInformation,
	SystemVerifierRemoveDriverInformation,
	SystemProcessorIdleInformation,
	SystemLegacyDriverInformation,
	SystemCurrentTimeZoneInformation,
	SystemLookasideInformation,
	SystemTimeSlipNotification,
	SystemSessionCreate,
	SystemSessionDetach,
	SystemSessionInformation,
	SystemRangeStartInformation,
	SystemVerifierInformation,
	SystemVerifierThunkExtend,
	SystemSessionProcessInformation,
	SystemLoadGdiDriverInSystemSpace,
	SystemNumaProcessorMap,
	SystemPrefetcherInformation,
	SystemExtendedProcessInformation,
	SystemRecommendedSharedDataAlignment,
	SystemComPlusPackage,
	SystemNumaAvailableMemory,
	SystemProcessorPowerInformation,
	SystemEmulationBasicInformation,
	SystemEmulationProcessorInformation,
	SystemExtendedHandleInformation,
	SystemLostDelayedWriteInformation,
	SystemBigPoolInformation,
	SystemSessionPoolTagInformation,
	SystemSessionMappedViewInformation,
	SystemHotpatchInformation,
	SystemObjectSecurityMode,
	SystemWatchdogTimerHandler,
	SystemWatchdogTimerInformation,
	SystemLogicalProcessorInformation,
	SystemWow64SharedInformation,
	SystemRegisterFirmwareTableInformationHandler,
	SystemFirmwareTableInformation,
	SystemModuleInformationEx,
	SystemVerifierTriageInformation,
	SystemSuperfetchInformation,
	SystemMemoryListInformation,
	SystemFileCacheInformationEx,
	MaxSystemInfoClass  // MaxSystemInfoClass should always be the last enum
} SYSTEM_INFORMATION_CLASS;
typedef unsigned char   BYTE;
typedef BYTE           *PBYTE;

typedef struct _RTL_PROCESS_MODULE_INFORMATION {
	HANDLE Section;                 // Not filled in
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR  FullPathName[256];
} RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;
typedef struct _RTL_PROCESS_MODULES {
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, * PRTL_PROCESS_MODULES;
//检索系统信息
NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation(
	__in		SYSTEM_INFORMATION_CLASS SystemInformationClass, 
	__inout		PVOID SystemInformation, 
	__in		ULONG SystemInformationLength,
	__out_opt	PULONG ReturnLength);
 
//获取Next数组
void GetNext(long* next, long* Tzm, long TzmLength);
//通过名字获取模块基址及大小
//获取模块参考了  https://blog.csdn.net/qq125096885/article/details/52875797
VOID GetModuleBase(CHAR* ModuleName,PVOID *ModuleBase,PULONG ModuleSize);
//获取系统模块
UINT32 GetSystemModel(PRTL_PROCESS_MODULE_INFORMATION* pSystemModel);
//搜索内存
BOOLEAN SearchMemory32(long* Tzm, long* next, long TzmLength, PVOID StartAddress, ULONG size, PUINT64 resultAddr);
BOOLEAN SearchMemory64(long* Tzm, long* next, long TzmLength, PVOID StartAddress, ULONG size, PUINT64 resultAddr);
//通过特征码获取系统函数地址
UINT64 GetSystemFunByTzm64(long* Tzm, long tzmLenth);
UINT32 GetSystemFunByTzm32(long* Tzm, long tzmLenth);

}