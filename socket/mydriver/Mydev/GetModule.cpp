#include "GetModule.h"

UINT32 GetSystemModel(PRTL_PROCESS_MODULE_INFORMATION *pSystemModel)
{
	//定义变量
	ULONG i = 0;
	RTL_PROCESS_MODULES* ProcessModules = NULL;
	ULONG ReturnLength = 0;
	ZwQuerySystemInformation(SystemModuleInformation, &ProcessModules, 4, &ReturnLength);
	if (ReturnLength)
	{
		ProcessModules = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag((POOL_TYPE)SystemModuleInformation, 2 * ReturnLength, ' kdD');
		if (ProcessModules)
		{
			if (NT_SUCCESS(ZwQuerySystemInformation(SystemModuleInformation, ProcessModules, 2 * ReturnLength, NULL)))
			{
				*pSystemModel = (PRTL_PROCESS_MODULE_INFORMATION)(ProcessModules->Modules);
				return ProcessModules->NumberOfModules;
			}
		}
	}
 
	return 0;
}
 
 
// 获取模块基址
VOID GetModuleBase(CHAR* ModuleName,PVOID* ModuleBase, PULONG ModuleSize)
{
	//参数效验
	if (ModuleName == NULL || ModuleBase==0 || ModuleSize==0)return;
 
 
	//定义变量
	ULONG i = 0;
	RTL_PROCESS_MODULES* ProcessModules = NULL;
	PVOID ImageBase = NULL;
	ULONG ReturnLength = 0;
	RTL_PROCESS_MODULE_INFORMATION* ModuleInformation = NULL;
 
	ZwQuerySystemInformation(SystemModuleInformation, &ProcessModules, 4, &ReturnLength);
	if (ReturnLength)
	{
		ProcessModules = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag((POOL_TYPE)SystemModuleInformation, 2 * ReturnLength, ' kdD');
		if (ProcessModules)
		{
			if (NT_SUCCESS(ZwQuerySystemInformation(SystemModuleInformation, ProcessModules, 2 * ReturnLength, NULL)))
			{
				ModuleInformation = (RTL_PROCESS_MODULE_INFORMATION*)(ProcessModules->Modules);
				for (i = 0; i < ProcessModules->NumberOfModules; i++)
				{
					//KdPrint(("%s\n", (CHAR*)&ModuleInformation[i].FullPathName[ModuleInformation[i].OffsetToFileName]));
					if (!_stricmp(ModuleName, (CHAR*)&ModuleInformation[i].FullPathName[ModuleInformation[i].OffsetToFileName]))
					{
						ImageBase = ModuleInformation[i].ImageBase;
						*ModuleBase = ImageBase;
						*ModuleSize = ModuleInformation[i].ImageSize;
						KdPrint(("Find module:%llx,size:%llx\n", *ModuleBase, *ModuleSize));
						break;
					}
				}
			}
			ExFreePoolWithTag(ProcessModules, 0);
		}
	}
}
 
 
//获取Next数组
void GetNext(long* next, long* Tzm, long TzmLength)
{
	//特征码（字节集）的每个字节的范围在0-255（0-FF）之间，256用来表示问号，到260是为了防止越界
	for (long i = 0; i < 260; i++)
		next[i] = -1;
	for (long i = 0; i < TzmLength; i++)
		next[Tzm[i]] = i;
}
 
//搜索一块内存
BOOLEAN SearchMemory64(long* Tzm, long* next, long TzmLength, PVOID StartAddress, ULONG size, PUINT64 resultAddr)
{
	long i = 0, j, k,num;
	if (!MmIsAddressValid(StartAddress))
	{
		KdPrint(("Error address:%llu",StartAddress));
		return FALSE;
	}
 
	while (i < (long)size)
	{
		j = i; k = 0;
		for (; k < TzmLength && j < (long)size && (Tzm[k] == ((PBYTE)StartAddress)[j] || Tzm[k] == 256); k++, j++);
 
		if (k == TzmLength)
		{
			*resultAddr = (UINT64)StartAddress + i;
			return TRUE;
		}
 
		if ((i + TzmLength) >= (long)size)
		{
			return FALSE;
		}
		num = next[((BYTE*)StartAddress)[i + TzmLength]];
		if (num == -1)
			i += (TzmLength - next[256]);//如果特征码有问号，就从问号处开始匹配，如果没有就i+=-1
		else
			i += (TzmLength - num);
	}
	return FALSE;
}

BOOLEAN SearchMemory32(long* Tzm, long* next, long TzmLength, PVOID StartAddress, ULONG size, PUINT32 resultAddr)
{
	long i = 0, j, k,num;
	if (!MmIsAddressValid(StartAddress))
	{
		KdPrint(("Error address:%u",StartAddress));
		return FALSE;
	}
 
	while (i < (long)size)
	{
		j = i; k = 0;
		for (; k < TzmLength && j < (long)size && (Tzm[k] == ((PBYTE)StartAddress)[j] || Tzm[k] == 256); k++, j++);
 
		if (k == TzmLength)
		{
			*resultAddr = (UINT32)StartAddress + i;
			return TRUE;
		}
 
		if ((i + TzmLength) >= (long)size)
		{
			return FALSE;
		}
		num = next[((BYTE*)StartAddress)[i + TzmLength]];
		if (num == -1)
			i += (TzmLength - next[256]);//如果特征码有问号，就从问号处开始匹配，如果没有就i+=-1
		else
			i += (TzmLength - num);
	}
	return FALSE;
}
 
UINT64 GetSystemFunByTzm64(long* Tzm,long tzmLenth)
{
	PVOID modualAddr;
	UINT64 reAddr = 0;
	ULONG size;
	long next[260] = { 0 };
 
	GetModuleBase("ntoskrnl.exe", &modualAddr, &size);
	GetNext(next, Tzm, tzmLenth);
	SearchMemory64(Tzm, next, tzmLenth, modualAddr, size, &reAddr);
	
	return reAddr;
}

UINT32 GetSystemFunByTzm32(long* Tzm,long tzmLenth)
{
	PVOID modualAddr;
	UINT32 reAddr = 0;
	ULONG size;
	long next[260] = { 0 };
 
	GetModuleBase("ntkrnlpa.exe", &modualAddr, &size);
	GetNext(next, Tzm, tzmLenth);
	SearchMemory32(Tzm, next, tzmLenth, modualAddr, size, &reAddr);
	
	return reAddr;
}