extern "C" {
#include <Ntddk.h>
#include "GetModule.h"
char g_Path[200];
extern PVOID KeServiceDescriptorTable;
#define MY_CTL_CODE(code)  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800 + code, METHOD_BUFFERED , FILE_ANY_ACCESS)
#define CTL_GOT         MY_CTL_CODE(0)
#define CTL_ENUMPROCESS MY_CTL_CODE(1)
#define CTL_READMEMORY  MY_CTL_CODE(2)
#define CTL_READPRESS   MY_CTL_CODE(3)
#define CTL_WRITEPRESS  MY_CTL_CODE(4)
#define CTL_SSDT        MY_CTL_CODE(5)
#define CTL_CALLBACK    MY_CTL_CODE(6)

/*
内存池：分页内存池   非内存池

*/
//PEPROCESS=%p pid=%d image_name=%s dir_base=%p
typedef struct MyProcess
{
  int eprocess;
  int   pid;
  int dir_base;
  char image_name[0x10]; 
}MYPROCESS,*PMYPROCESS;

typedef struct MyProcessList
{
  int nNum;
  MYPROCESS m_process[1];
}MYPROCESSLIST,*PMYPROCESSLIST;

typedef struct MyCallBack
{
    ULONG dwStyle;
    ULONG dwCallBack;
    ULONG dwAddr;
}MYCALLBACK, * PMYCALLBACK;

typedef struct MyCallBackList
{
    ULONG dwNum;
    MYCALLBACK callback[1];
}MYCALLBACKLIST,*PMYCALLBACKLIST;

NTSTATUS DriverEntry( __in struct _DRIVER_OBJECT  *DriverObject, __in PUNICODE_STRING  RegistryPath);
VOID Unload(__in struct _DRIVER_OBJECT  *DriverObject);
NTSTATUS DispatchCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS DispatchClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS DispatchControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

#pragma alloc_text("PAGE", Unload)
#pragma alloc_text("INIT", DriverEntry)
#pragma alloc_text("PAGE", DispatchCreate)
#pragma alloc_text("PAGE", DispatchClose)
#pragma alloc_text("PAGE", DispatchControl)


VOID Unload(__in struct _DRIVER_OBJECT  *DriverObject)
{
    KdPrint(("Hello Unload\n"));

    //删除符号链接
    UNICODE_STRING ustrSymbolName;
    RtlInitUnicodeString(&ustrSymbolName, L"\\??\\cr36zh");
    IoDeleteSymbolicLink(&ustrSymbolName);

    //删除设备
    if (DriverObject->DeviceObject != NULL)
      IoDeleteDevice(DriverObject->DeviceObject);

    
}

NTSTATUS DispatchCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
   KdPrint(("DispatchCreate\n"));

   g_Path[0] = '\0';

   //完成请求
   Irp->IoStatus.Status = STATUS_SUCCESS;
   Irp->IoStatus.Information = 0;
   IoCompleteRequest(Irp, IO_NO_INCREMENT);

   return STATUS_SUCCESS; 
}

NTSTATUS DispatchClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{ 
  //操作硬件  异步   IRP I/O Request 请求  
   KdPrint(("DispatchClose\n"));

   //完成请求
   Irp->IoStatus.Status = STATUS_SUCCESS;
   Irp->IoStatus.Information = 0;
   IoCompleteRequest(Irp, IO_NO_INCREMENT);

   return STATUS_SUCCESS; 
}



typedef struct ShutdownCallBack
{
	LIST_ENTRY Entry;
	long       callback;
}SHUTDOWNCALLBACK,*PSHUTDOWNCALLBACK;

NTSTATUS GetCallBack(PVOID buf)
{
  //错误检测回调
	/*
	nt!KeRegisterBugCheckCallback+0x52:
	804fa130 8b0d58d75580    mov     ecx,dword ptr [nt!KeBugCheckCallbackListHead (8055d758)]
	*/
	PMYCALLBACKLIST pCallBackList = (PMYCALLBACKLIST)buf;
	long Tzm1[] = {0x8b,0x0d,0x58,0xd7,0x55,0x80};
	long nt_KeRegisterBugCheckCallback_0x52 = GetSystemFunByTzm32(Tzm1,6);
	KdPrint(("[Callback] DriverEntry Success nt!PspSetCreateProcessNotifyRoutine+0x52=%p\r\n",nt_KeRegisterBugCheckCallback_0x52));
	long OffsetAddr = 0;

	if (*(PUCHAR)nt_KeRegisterBugCheckCallback_0x52 == 0x8b && *(PUCHAR)(nt_KeRegisterBugCheckCallback_0x52 + 1) == 0x0d)
	{
		RtlCopyMemory(&OffsetAddr, (PUCHAR)(nt_KeRegisterBugCheckCallback_0x52 + 2), 4);
		KdPrint(("[Callback] DriverEntry Success nt!nt_KeRegisterBugCheckCallback_0x52=%p\r\n",OffsetAddr));
	}
	PSHUTDOWNCALLBACK pCallback = NULL;
	int i = 0;
	while(((int*)OffsetAddr)[i] != 0)
	{
		pCallback = (PSHUTDOWNCALLBACK)((int*)OffsetAddr)[i];
		
		KdPrint(("[Callback] KeRegisterBugCheckCallback %d callback=%p callbackobj=%p\r\n",i,pCallback->callback,pCallback));
    pCallBackList->callback[i].dwStyle = 1;
    pCallBackList->callback[i].dwCallBack = pCallback->callback;
    pCallBackList->callback[i].dwAddr = (ULONG)pCallback;
		i++;
	}

  /* 关机回调
	IoRegisterShutdownNotification
	805759cb b960be5580      mov     ecx,offset nt!IopNotifyShutdownQueueHead (8055be60)
	*/
	long Tzm2[] = {0xb9,0x60,0xbe,0x55,0x80  };
	long nt_IoRegisterShutdownNotification_0x1d = GetSystemFunByTzm32(Tzm2,5);
	KdPrint(("[Callback] DriverEntry Success nt!IoRegisterShutdownNotification+0x1d=%p\r\n",nt_IoRegisterShutdownNotification_0x1d));
	OffsetAddr = 0;

	if (*(PUCHAR)nt_IoRegisterShutdownNotification_0x1d == 0xb9 )
	{
		RtlCopyMemory(&OffsetAddr, (PUCHAR)(nt_IoRegisterShutdownNotification_0x1d + 1), 4);
		KdPrint(("[Callback] DriverEntry Success nt!nt_IoRegisterShutdownNotification_0x1d=%p\r\n",OffsetAddr));
	}

	pCallback = (PSHUTDOWNCALLBACK)OffsetAddr;
	PSHUTDOWNCALLBACK pThis = (PSHUTDOWNCALLBACK)pCallback;
	//KdBreakPoint();
	do
	{
		PDEVICE_OBJECT pDev = (PDEVICE_OBJECT)(pThis->callback);
		PDRIVER_OBJECT pObj = pDev->DriverObject;
		ULONG Addr = *((ULONG*)((char*)pObj + 0x38+0x40));
		if(Addr!=NULL)
		{
			KdPrint(("[Callback] KeRegisterBugCheckCallback %d callback=%p callbackobj=%p\r\n",i,pThis->callback,Addr));
      pCallBackList->callback[i].dwStyle = 2;
      pCallBackList->callback[i].dwCallBack = pThis->callback;
      pCallBackList->callback[i].dwAddr = Addr;
			i++;
		}
		
		pThis = (PSHUTDOWNCALLBACK)pThis->Entry.Flink;
	}while(pThis != pCallback);
  pCallBackList->dwNum = i;
  
  return STATUS_SUCCESS;
}

NTSTATUS GetMapPhysical(int PhysicalAdd, PVOID pte,int nSize)
{
  PHYSICAL_ADDRESS PhysicalAddress;
  PhysicalAddress.QuadPart = PhysicalAdd;
  PVOID p = MmMapIoSpace(PhysicalAddress, nSize, MmNonCached);
  RtlCopyMemory(pte,p,nSize);
  MmUnmapIoSpace(p,nSize);
  return STATUS_SUCCESS; 
}

NTSTATUS GetDirBase(ULONG dest_pid, PVOID* addr) {
  PEPROCESS process = NULL;
  PEPROCESS old_process = NULL;
  ULONG pid;
  UCHAR* image_name;
  PVOID dir_base = NULL;
  
  //KdBreakPoint();
  
  __asm
  {
      mov eax, fs:[124h]   //ETHREAD
      mov eax, [eax+44h] //EPROCESS
      mov process, eax
  }
  
  old_process = process;
  
  //遍历
  do 
  {
    pid = *(ULONG*)((char*)process + 0x84);
    image_name = (UCHAR*)process + 0x174;
    dir_base = *(PVOID*)((char*)process + 0x18);
    if (pid == dest_pid) {
      KdPrint(("[Process] PEPROCESS=%p pid=%d image_name=%s dir_base=%p\n", 
      process, pid, image_name, dir_base));
      *addr = dir_base;
      return STATUS_SUCCESS;
    }

    //下一个进程
    process = (PEPROCESS)(*(ULONG*)((char*)process + 0x88) - 0x88);
  } 
  while (process != old_process);
  return STATUS_UNSUCCESSFUL;
}

NTSTATUS WriteProcessMemory(ULONG pid, PVOID address, PVOID buf, ULONG len) {
  
  PVOID dir_base = NULL;
  NTSTATUS status ;
  PVOID old_cr3;
  
  //获取页目录地址
  status = GetDirBase(pid, &dir_base);
  if (!NT_SUCCESS(status)) {
    return status;
  }
  
  //修改页目标表地址
  __asm
  {
      //防止线程切换
      cli  //屏蔽中断  自旋锁
 
      mov eax, cr3
      mov old_cr3, eax
      mov eax, dir_base
      mov cr3, eax

      //关闭内存保护属性检查
      mov eax, cr0
      and eax, not 10000h
      mov cr0, eax
  }
  
  //拷贝数据
  if (MmIsAddressValid(address)) {
    RtlCopyMemory(address, buf, len);
    status = STATUS_SUCCESS;
  }
  else 
    status = STATUS_UNSUCCESSFUL;
  
  //还原
  __asm
  {
    mov eax, cr0
    or  eax,  10000h
    mov cr0, eax

    mov eax, old_cr3
    mov cr3, eax

    sti  //恢复中断
  }
  
  return status;
}

NTSTATUS ReadProcessMemory(ULONG pid, PVOID address, PVOID buf, ULONG len) {

  PVOID dir_base = NULL;
  NTSTATUS status ;
  PVOID old_cr3;

  //获取页目录地址
  status = GetDirBase(pid, &dir_base);
  if (!NT_SUCCESS(status)) {
    return status;
  }

  //修改页目标表地址
  __asm
  {
    mov eax, cr3
    mov old_cr3, eax
    mov eax, dir_base
    mov cr3, eax
  }

  //拷贝数据
  if (MmIsAddressValid(address)) {
     RtlCopyMemory(buf, address, len);
     status = STATUS_SUCCESS;
  }
  else 
    status = STATUS_UNSUCCESSFUL;
  
  //还原
  __asm
  {
    mov eax, old_cr3
    mov cr3, eax
  }

  return status;
}

NTSTATUS EnumProcess(PVOID buf) {
  PEPROCESS process = NULL;
  PEPROCESS old_process = NULL;
  ULONG pid;
  UCHAR* image_name;
  PVOID dir_base = NULL;

  //KdBreakPoint();

  __asm
  {
    mov eax, fs:[124h]   //ETHREAD
    mov eax, [eax+44h] //EPROCESS
    mov process, eax
  }

  old_process = process;
  PMYPROCESSLIST pList = (PMYPROCESSLIST)buf;
  int nNum = 0;
  //遍历
  do 
  {
    pid = *(ULONG*)((char*)process + 0x84);
    image_name = (UCHAR*)process + 0x174;
    dir_base = *(PVOID*)((char*)process + 0x18);
    KdPrint(("[Process] PEPROCESS=%p pid=%d image_name=%s dir_base=%p\n", 
             process, pid, image_name, dir_base));
    if(dir_base != 0)
    {
      pList->m_process[nNum].eprocess = (int)process;
      pList->m_process[nNum].dir_base = (int)dir_base;
      pList->m_process[nNum].pid = pid;
      RtlCopyMemory(pList->m_process[nNum].image_name,image_name,0x16);
      nNum++;
    }
    
    //下一个进程
    process = (PEPROCESS)(*(ULONG*)((char*)process + 0x88) - 0x88);

  } 
  while (process != old_process);
  pList->nNum = nNum;
  return STATUS_SUCCESS;
}

NTSTATUS DispatchControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
  //获取irp堆栈
  PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(Irp);
  
  //获取缓冲区
  PVOID pOutBuffer =  Irp->AssociatedIrp.SystemBuffer;
  PVOID pInBuffer = Irp->AssociatedIrp.SystemBuffer;


  ULONG nInLength = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
  ULONG nOutLength = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
  ULONG nIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;
  NTSTATUS status = STATUS_SUCCESS;
  FILE_DIRECTORY_FILE ;
  KdPrint(("DispatchControl nIoControlCode=%p pInBuffer=%p nInLength=%p" 
           "pOutBuffer=%p nOutLength=%p\n", 
           nIoControlCode, 
           pInBuffer, 
           nInLength,
           pOutBuffer,
           nOutLength));

  switch(nIoControlCode) 
  {
  //遍历GDT表
  case CTL_GOT:
  {
    PVOID pGdt = (PVOID)(*(int *)pInBuffer);
    KdPrint(("DispatchControl CTL_GOT pGdt=%p\n", pGdt));
    if (MmIsAddressValid(pGdt)) 
    {
      RtlMoveMemory(pOutBuffer,pGdt,nOutLength);
    }
    break;
  }
  //遍历进程
  case CTL_ENUMPROCESS:  
  {
    if (MmIsAddressValid(pOutBuffer)) 
    {
      EnumProcess(pOutBuffer);
      nOutLength = ((PMYPROCESSLIST)pOutBuffer)->nNum * sizeof(MYPROCESS) + 4;
    }
    
    break;
  }
  //读取真实物理内存地址
  case CTL_READMEMORY:
  {
    int PhysicalAdd = *(int *)pInBuffer;
    //KdBreakPoint();
    GetMapPhysical(PhysicalAdd, pOutBuffer,nOutLength);
    break;
  }
  //读指定进程
  case CTL_READPRESS:
  {
    int pid = *(int *)pInBuffer;
    PVOID address = (PVOID)(*((int *)pInBuffer+1));
    ReadProcessMemory(pid, address, pOutBuffer, nOutLength);
    break;
  }
  //写指定进程
  case CTL_WRITEPRESS:
  {
    int pid = *(int *)pInBuffer;
    PVOID address = (PVOID)(*((int *)pInBuffer+1));
    PVOID buf = (PVOID)((int *)pInBuffer+3);
    WriteProcessMemory(pid, address, buf, nOutLength);
    break;
  }
  //获取ssdt
  case CTL_SSDT:
  {
    if (MmIsAddressValid(pOutBuffer)) 
    {
      RtlMoveMemory(pOutBuffer,KeServiceDescriptorTable,0x10);
      nOutLength = 0x10;

    }
    break;
  }

  //获取callback
  case CTL_CALLBACK:
  {
    
    if (MmIsAddressValid(pOutBuffer)) 
    {
      GetCallBack(pOutBuffer);
      nOutLength = ((PMYCALLBACKLIST)pOutBuffer)->dwNum * sizeof(MYCALLBACK) +4;
    }
    break;
  }
    
  }
   //完成请求
   Irp->IoStatus.Status = STATUS_SUCCESS;
   Irp->IoStatus.Information = nOutLength;
   IoCompleteRequest(Irp, IO_NO_INCREMENT);
   return STATUS_SUCCESS; 
}


//驱动入口
NTSTATUS DriverEntry( 
    __in struct _DRIVER_OBJECT  *DriverObject,
    __in PUNICODE_STRING  RegistryPath)
{
  
  //AddDevice  创建设备
  UNICODE_STRING ustrDevName;
  RtlInitUnicodeString(&ustrDevName, L"\\Device\\cr36zh");
  PDEVICE_OBJECT pDevObj = NULL;
  NTSTATUS  status = IoCreateDevice(
                                  DriverObject,                         //DriverObject
                                  0,                                    //DeviceExtensionSize
                                  &ustrDevName,                         //DeviceName
                                  FILE_DEVICE_UNKNOWN,                  //DeviceType
                                  0,                                    //DeviceCharacteristics
                                  FALSE,                                //Exclusive
                                  &pDevObj);                            //DeviceObject
  if (!NT_SUCCESS(status))
  {
      KdPrint(("IoCreateDevice Error status=%p\n", status));
      return status;
  }
  KdPrint(("IoCreateDevice OK pDevObj=%p\n", pDevObj));

  //注册派遣函数 DispatchXxx (routines beginning with "Dispatch" such as DispatchCreate)
  DriverObject->MajorFunction[IRP_MJ_CREATE]         = DispatchCreate;
  DriverObject->MajorFunction[IRP_MJ_CLOSE]          = DispatchClose;
  DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchControl;


  //注册卸载函数 DriverUnload
  DriverObject->DriverUnload  = Unload;

  //缓冲区通讯方式  1.缓冲区通讯方式 2.直接方式  3.其它方式(ring3)
  pDevObj->Flags |=  DO_BUFFERED_IO;
  //pDevObj->Flags |=  DO_DIRECT_IO;

  //符号链接
  UNICODE_STRING ustrSymbolName;
  RtlInitUnicodeString(&ustrSymbolName, L"\\??\\cr36zh");
  
  IoCreateSymbolicLink(&ustrSymbolName, &ustrDevName);
  if (!NT_SUCCESS(status))
  {
    KdPrint(("IoCreateSymbolicLink Error status=%p\n", status));
    return status;
  }
  KdPrint(("IoCreateSymbolicLink OK\n"));

  return STATUS_SUCCESS; 
} 



    
}

