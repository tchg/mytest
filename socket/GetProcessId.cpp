#include <iostream>
#include <Windows.h>
#include <ntsecapi.h> 

#define STATUS_INFO_LENGTH_MISMATCH    ((NTSTATUS)0xC0000004L)
#define NT_SUCCESS(x) ((x) >= 0)

// 结构体定义  
typedef struct _SYSTEM_PROCESS_INFORMATION {
	ULONG                   NextEntryOffset;
	ULONG                   NumberOfThreads;
	LARGE_INTEGER           Reserved[3];
	LARGE_INTEGER           CreateTime;
	LARGE_INTEGER           UserTime;
	LARGE_INTEGER           KernelTime;
	UNICODE_STRING          ImageName;
	DWORD					BasePriority;
	HANDLE                  ProcessId;
	HANDLE                  InheritedFromProcessId;
	ULONG                   HandleCount;
	ULONG                   Reserved2[2];
	ULONG                   PrivatePageCount;
	DWORD					VirtualMemoryCounters;
	IO_COUNTERS             IoCounters;
	PVOID					Threads[0];
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation,
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
	MaxSystemInfoClass
} SYSTEM_INFORMATION_CLASS;

//定义原型函数 
typedef
NTSTATUS
(WINAPI* pfnZwQuerySystemInformation)(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	IN OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength);
pfnZwQuerySystemInformation ZwQuerySystemInformation = NULL;
UINT32 PrintProcessesIDAndName();
int main()
{
	// 从Ntdll.dll中获得导出函数  
	HMODULE NtdllHmodule = GetModuleHandle(L"ntdll.dll");
	ZwQuerySystemInformation = (pfnZwQuerySystemInformation)GetProcAddress(NtdllHmodule, "ZwQuerySystemInformation");

	if (ZwQuerySystemInformation == NULL)
	{
		printf("Can't Get Address of ZwQuerySystemInformation!");
		return 0;
	}
	while (1)
	{
		PrintProcessesIDAndName();
		Sleep(2000);
	}
	

	return 0;
}

//打印进程ID和名称
UINT32 PrintProcessesIDAndName()
{
	UINT32 BufferLength = 0x1000;
	void* BufferData = NULL;

	NTSTATUS Status = STATUS_INFO_LENGTH_MISMATCH;
	HANDLE   HeapHandle = GetProcessHeap();      //获得当前进程默认堆

	UINT32 ProcessID = 0;

	BOOL   bOk = FALSE;
	while (!bOk)
	{
		BufferData = HeapAlloc(HeapHandle, HEAP_ZERO_MEMORY, BufferLength);
		if (BufferData == NULL)
		{
			return 0;
		}

		Status = ZwQuerySystemInformation(SystemProcessInformation, BufferData, BufferLength, (PULONG)&BufferLength);
		if (Status == STATUS_INFO_LENGTH_MISMATCH)
		{
			//内存不足,将内存扩大二倍重新申请
			HeapFree(HeapHandle, NULL, BufferData);
			BufferLength *= 2;
		}
		else if (!NT_SUCCESS(Status))
		{
			//不让看
			HeapFree(HeapHandle, NULL, BufferData);
			return 0;
		}
		else
		{

			PSYSTEM_PROCESS_INFORMATION SystemProcess = (PSYSTEM_PROCESS_INFORMATION)BufferData;
			while (SystemProcess)
			{
				//定义变量ProcessName接收Name
				char ProcessName[MAX_PATH];
				memset(ProcessName, 0, sizeof(ProcessName));
				WideCharToMultiByte(0, 0, SystemProcess->ImageName.Buffer, SystemProcess->ImageName.Length, ProcessName, MAX_PATH, NULL, NULL);
				ProcessID = (UINT32)(SystemProcess->ProcessId);
				if (!strcmp(ProcessName, "notepad.exe"))
				{
					printf("PID:\t%X,\tName:\t%s\r\n", ProcessID, ProcessName);
				}
				

				if (!SystemProcess->NextEntryOffset)
				{
					break;
				}
				SystemProcess = (PSYSTEM_PROCESS_INFORMATION)((unsigned char*)SystemProcess + SystemProcess->NextEntryOffset);
			}

			if (BufferData)
			{
				HeapFree(HeapHandle, NULL, BufferData);
			}

			bOk = TRUE;
		}
	}

	return ProcessID;
}
