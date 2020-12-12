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
	DT_LOGIN,	 //��¼��
	
	DT_SEEDISK,  //�鿴����
	DT_SEEFILE,  //�鿴�ļ�
	DT_UPFILEOPEN,   //���ϴ��ļ�
	DT_UPFILE,		//�ϴ��ļ�
	DT_DOWNFILEOPEN, //�������ļ�
	DT_DOWNFILE,	//�����ļ�

	DT_SEEPROCESS, //�鿴����
	DT_SEETHREAD,  //�鿴�߳�
	DT_STOPPROCESS, //��������

	DT_SEEREGEDIT,     //�鿴ע���
	DT_SEEREGEDITCL,   //�鿴ע�����
	DT_CLEANREGEDITCL, //���list
	DT_ADDREGEDIT,     //���ע�����
	DT_ADDREGEDITCL,   //���ע���ֵ
	DT_DELREGEDIT,     //ɾ��ע�����
	DT_DELREGEDITVALUE, //ɾ��ע���ֵ
	DT_EDTREGEDIT,      //�޸�ע���

	DT_SEECMD,          //����cmd����

	DT_QUIT      //�˳�
};

//���ݵİ�ͷ
typedef struct  tagPackageHeader
{
	int m_nDataLen;  //���ݵĳ���
	int m_nDataType; //���ݵ�����
}PACKAGEHEADER, *PPACKAGEHEADER;

//���ݰ�
typedef struct tagDataPackage
{
	PACKAGEHEADER m_hdr;	//���ݰ��İ�ͷ
	char* m_pBuff;			//���ݰ��İ���
}DATAPACKAGE, * PDATAPACKAGE;