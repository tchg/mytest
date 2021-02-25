#pragma once
#include <Windows.h>
#include <winioctl.h>
#include <bcrypt.h>
#define MY_CTL_CODE(code)  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800 + code, METHOD_BUFFERED , FILE_ANY_ACCESS)
#define CTL_GOT MY_CTL_CODE(0)
#define CTL_ENUMPROCESS MY_CTL_CODE(1)
#define CTL_READMEMORY MY_CTL_CODE(2)
#define CTL_READPRESS MY_CTL_CODE(3)
#define CTL_WRITEPRESS MY_CTL_CODE(4)
#define CTL_SSDT MY_CTL_CODE(5)
#define CTL_CALLBACK MY_CTL_CODE(6)
#pragma pack (1)
typedef struct MYGDT
{
    short limit;
    DWORD  base;
}GDT, * PGDT;

typedef struct base_addr
{
    short base_low;
    char base_mid;
    char base_high;
}BASEADDR, * PBASEADDR;


typedef struct limit_addr
{
    DWORD limit_low : 16;
    DWORD limit_high : 4;

}LIMITADDR, * PLIMITADDR;

typedef struct segment_descriptor {
    unsigned long long limit_low : 16;
    unsigned long long base_low : 16;
    unsigned long long base_mid : 8;
    unsigned long long type : 4;
    unsigned long long S : 1;
    unsigned long long DPL : 2;
    unsigned long long P : 1;
    unsigned long long limit_high : 4;
    unsigned long long AVL : 1;
    unsigned long long L : 1;
    unsigned long long D_B : 1;
    unsigned long long G : 1;
    unsigned long long base_high : 8;
}SEGMENT_DESCRIPTOR, * PSEGMENT_DESCRIPTOR;

typedef struct MyIdt
{
    unsigned low : 16;
    unsigned selector : 16;
    unsigned res : 8;
    unsigned type : 4;
    unsigned s : 1;
    unsigned dpl : 2;
    unsigned p : 1;
    unsigned hei : 16;
}MYIDT,*PMYIDT;

typedef struct MySsdt
{
    //System Service Dispatch Table的基地址
    PVOID ServiceTableBase;
    //SSDT中每个服务被调用次数的计数器。这个计数器一般由sysenter 更新。
    PVOID ServiceCounterTable;
    //由 ServiceTableBase 描述的服务的数目。
    unsigned int NumberOfServices;
    //每个系统服务参数字节数表的基地址-系统服务参数表SSPT 
    PVOID ParamTableBase;
}MYSSDT,*PMYSSDT;



#pragma pack () 
DWORD GetBaseAddr(PSEGMENT_DESCRIPTOR psd);
DWORD GetLimit(PSEGMENT_DESCRIPTOR psd);
DWORD GetIdtAddr(PMYIDT pidt);
typedef struct MyProcess
{
    DWORD eprocess;
    int   pid;
    DWORD dir_base;
    char image_name[0x10];
}MYPROCESS, * PMYPROCESS;

typedef struct MyProcessList
{
    int nNum;
    MYPROCESS m_process[1];
}MYPROCESSLIST, * PMYPROCESSLIST;

typedef struct MyPde
{
    struct {
        DWORD LowPart;
        DWORD HighPart;
    };
}MYPDE,*PMYPDE;

typedef struct MyPdeList
{
    MYPDE m_pde[1];
}MYPDELIST, * PMYPDELIST;

typedef struct MyReadWrite
{
    DWORD dwPid;
    DWORD dwAddr;
    DWORD dwSize;
    DWORD dwData;
}MYREADWRITE,*PMYREADWRITE;


typedef struct MyCallBack
{
    DWORD dwStyle;
    DWORD dwCallBack;
    DWORD dwAddr;
}MYCALLBACK, * PMYCALLBACK;

typedef struct MyCallBackList
{
    DWORD dwNum;
    MYCALLBACK callback[1];
}MYCALLBACKLIST, * PMYCALLBACKLIST;

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation,
    SystemProcessorInformation,             // obsolete...delete
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
    MaxSystemInfoClass  // MaxSystemInfoClass should always be the last enum
} SYSTEM_INFORMATION_CLASS;



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

#define PERFPOOLTAG 'masm'
#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
typedef NTSTATUS (WINAPI* PFNNtQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass, // 定义服务类型号
    PVOID SystemInformation, // 用户存储信息的缓冲区
    ULONG SystemInformationLength, // 缓冲区大小
    PULONG ReturnLength); // 返回信息长度

NTSTATUS NTAPI NtQuerySystemInformation(
    __in SYSTEM_INFORMATION_CLASS SystemInformationClass,
    __out_bcount_opt(SystemInformationLength) PVOID SystemInformation,
    __in ULONG SystemInformationLength,
    __out_opt PULONG ReturnLength);
