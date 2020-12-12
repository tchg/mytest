#pragma once

#define WM_ClientLOGIN WM_USER+1
#define WM_SEEDISK WM_USER+2
#define WM_SEEFILE WM_USER+3
#define WM_UPFILE  WM_USER+4
#define WM_DOWNFILE  WM_USER+5

#define WM_SEEPROCESS  WM_USER+6
#define WM_SEETHREAD  WM_USER+7
#define WM_STOPPROCESS  WM_USER+8

#define WM_SEEREGEDIT  WM_USER+9
#define WM_DELREGEDIT  WM_USER+10
#define WM_DELREGEDITVALUE  WM_USER+11
#define WM_EDTREGEDIT  WM_USER+12

#define WM_SEEREGEDITCL  WM_USER+13
#define WM_CLEANREGEDITCL  WM_USER+14

#define WM_ADDREGEDIT  WM_USER+15
#define WM_ADDREGEDITCL  WM_USER+16

#define WM_SEECMD  WM_USER+17

#define WM_DOWNFILEOPEN WM_USER+18
//#define WM_SEEPROCESS  WM_USER+13

#define WM_UPFILEOPEN WM_USER + 19


enum DataType
{
	DT_LOGIN,	 //登录包
	
	DT_SEEDISK,  //查看磁盘
	DT_SEEFILE,  //查看文件
	DT_UPFILEOPEN,   //打开上传文件
	DT_UPFILE,		//上传文件
	DT_DOWNFILEOPEN, //打开下载文件
	DT_DOWNFILE,	//下载文件

	DT_SEEPROCESS, //查看进程
	DT_SEETHREAD,  //查看线程
	DT_STOPPROCESS, //结束进程

	DT_SEEREGEDIT,     //查看注册表
	DT_SEEREGEDITCL,   //查看注册表项
	DT_CLEANREGEDITCL, //清空list
	DT_ADDREGEDIT,     //添加注册表项
	DT_ADDREGEDITCL,   //添加注册表值
	DT_DELREGEDIT,     //删除注册表项
	DT_DELREGEDITVALUE, //删除注册表值
	DT_EDTREGEDIT,      //修改注册表

	DT_SEECMD,          //发送cmd参数

	DT_QUIT      //退出
};

//数据的包头
typedef struct  tagPackageHeader
{
	int m_nDataLen;  //数据的长度
	int m_nDataType; //数据的类型
}PACKAGEHEADER, *PPACKAGEHEADER;

//数据包
typedef struct tagDataPackage
{
	PACKAGEHEADER m_hdr;	//数据包的包头
	char* m_pBuff;			//数据包的包体
}DATAPACKAGE, * PDATAPACKAGE;