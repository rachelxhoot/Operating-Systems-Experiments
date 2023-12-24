#include<time.h>       
#include<dos.h>     
#include"windows.h"
#include<string>
#include<iostream>

#pragma warning(disable:4996)
using namespace std;
#define MAX_FILE_NAME_LENGTH 16
#define MAX_TIME_LENGTH 30
typedef struct FCB
{
	char FileName[MAX_FILE_NAME_LENGTH];   //�ļ���
	int FirstBlock;//�ļ��׿��
	long int FileSize;    //�ļ���С,��λΪ�ֽ���
	char FileDate[MAX_TIME_LENGTH];//�ļ������޸�ʱ��
	char FileOrNot;//'1'���ļ���'0'���ļ��� !ע�⣺�������ַ�
	int FreeOrNot;//�ļ����ƿ��Ƿ�Ϊ��0:����-1:æ
	char user;
}FCB;
/*ȫ�ֱ���*/
const char* FileSavePath = "FileSystem.sys";
int BlockSize = 512 * sizeof(char);       /*ÿ���̿��С(������),Ĭ��B*/
int BlockSum = 500;   /*�̿���*/
int MaxTableItems = (BlockSize / sizeof(int));//ÿ�����̿���Դ�ŵ�FAT������
int MaxFileItems = (BlockSize / sizeof(FCB));//ÿ�����̿���Դ�ŵ��ļ����ƿ���
int TableBlocks = BlockSum / MaxTableItems + 1; //TAB��ռ�õĴ��̿�
int FcbSum = BlockSize / sizeof(FCB);//ÿ���̿������е�FCBĿ¼�����
int tempBlock;
bool IsOpen = false;
char StrTime[MAX_TIME_LENGTH] = { '\0' };
FILE* fp = NULL;      //�����ļ���ַ
int* DiskBase;    //FAT�ļ�ϵͳ���̿ռ����ַ
FCB* FileBase;//�ļ���ַ
string cmd;      //����ָ��
string CURRENT = "A:\\";   //��ǰ·��
char command[MAX_FILE_NAME_LENGTH];    //�ļ�����ʶ
int CurrentDir;    //��ǰĿ¼���̿��
char user;


int DelBlankDir(int FileItem)
{
	int i;
	int BlockNo, ParentBlock;
	FCB* pFileItem;
	FCB* tempPtr;
	tempPtr = FileBase;
	//��ɾ������ʼ��Ŀ¼����ָ�Ĵ��̿�
	BlockNo = (tempPtr + FileItem)->FirstBlock;
	do
	{
		ParentBlock = BlockNo;
		pFileItem = GetAddr(BlockNo);
		(pFileItem + 0)->FileName[0] = '\0';
		(pFileItem + 0)->FileOrNot = '0';
		(pFileItem + 0)->FileSize = 0;
		(pFileItem + 0)->FirstBlock = 0;
		(pFileItem + 0)->FreeOrNot = 0;
		strcpy((pFileItem + 0)->FileDate, GetNowDateTime());
		(pFileItem + 1)->FileName[0] = '\0';
		(pFileItem + 1)->FileOrNot = '0';
		(pFileItem + 1)->FileSize = 0;
		(pFileItem + 1)->FirstBlock = 0;
		(pFileItem + 1)->FreeOrNot = 0;
		strcpy((pFileItem + 1)->FileDate, GetNowDateTime());
		for (i = 2;i < MaxFileItems;i++)
		{//��ʼ��Ҫɾ�����ļ������ļ����ƿ�
			if ((pFileItem + i)->FreeOrNot == 0) continue;
			else
			{
				if ((pFileItem + i)->FileOrNot == '1')//������ļ�
					rm((pFileItem + i)->FileName);
				else if ((pFileItem + i)->FileOrNot == '0')//������ļ���
				{
					(pFileItem + i)->FreeOrNot = 0;
					FileBase = pFileItem;
					DelBlankDir(i);
				}
			}
		}//for
		BlockNo = *(DiskBase + ParentBlock);//ָ����������һ���̿�
		*(DiskBase + ParentBlock) = 0;
	} while (BlockNo != -1);
	//����ɾ��Ŀ¼��
	(tempPtr + FileItem)->FileName[0] = '\0';
	(tempPtr + FileItem)->FileOrNot = '0';
	(tempPtr + FileItem)->FileSize = 0;
	(tempPtr + FileItem)->FirstBlock = 0;
	(tempPtr + FileItem)->FreeOrNot = 0;
	return 0;
}

char* GetNowDateTime()
{//��ȡϵͳ��ǰʱ�䣬���������ַ���
	time_t   ltime;
	struct   tm* today;
	time(&ltime);
	today = localtime(&ltime);   //"%4d-%02d-%02d   %02d:%02d:%02d   
	sprintf(StrTime, "%4d-%02d-%02d  %02d:%02d:%02d\0", (today->tm_year + 1900), (today->tm_mon + 1), (today->tm_mday), (today->tm_hour), (today->tm_min), (today->tm_sec));
	return StrTime;
}
int DelFileSys(char* FileName)
{ //ɾ�������ϱ�����ļ�ϵͳ
	char   comdel[100] = "del   ";
	strcat(comdel, FileName);
	if (system(comdel) == 0)
		cout << FileName << "ɾ���ɹ���" << endl;
	return   0;
}
void CurSysPath()
{//��ʾ��ǰ�ļ�ϵͳĿ¼
	cout << "��ǰϵͳĬ�ϴ洢Ŀ¼" << FileSavePath << endl << endl;
	return;
}

void ShowCommandHelp()
{//��ʾ������Ϣ
	printf("\n***************������ʹ��˵����****************\n");
	printf("help :��ʾ������Ϣ.\n");
	printf("format :�Դ��̸�ʽ��.\n");
	printf("save :���Ǳ�����Ӳ���ϵ��ļ�ϵͳ�������˳�.\n");
	printf("cursyspath  :��ʾ��ǰϵͳĬ�ϴ洢Ŀ¼.\n");
	printf("exit   :��ȫ�˳����ļ�ϵͳ,������Ϣ.\n");
	printf("mkdir dirname :������Ŀ¼.\n");
	printf("rmdir dirname :ɾ����Ŀ¼.\n");
	printf("ls     dirname :��ʾ��ǰĿ¼����Ϣ.\n");
	printf("cd     dirname :���ĵ�ǰĿ¼.\n");
	printf("create filename :����һ�����ļ�,���Ҵ�.\n");
	printf("close filename :�ر��ļ�.\n");
	printf("write filename :ѡ��һ���򿪵��ļ�д����Ϣ,��$��ʾ����.\n");
	printf("read   filename :ѡ��һ���򿪵��ļ���ȡ��Ϣ.\n");
	printf("rm     filename :ɾ���ļ�.\n");
	cout << "***************************************************\n\n\n";
}

void FreeBlock(int BlockNo)
{//�ͷŴ��̿�ΪBlockNo�Ĵ��̿�
	if (BlockNo < 0 || BlockNo > BlockSum - 1)
	{
		cout << "Ҫ�ͷŵĴ��̿�ŷǷ�!" << endl;
		return;
	}
	*(DiskBase + BlockNo) = 0;
}

int CheckFileName(char* FileName)
{//�˲��ļ����Ƿ�Ϸ�
	if (strlen(FileName) > MAX_FILE_NAME_LENGTH)
	{
		std::cin.clear();//��ջ�����
		return -1;
	}
	return 0;
}

int GetBlock()
{//Ѱ�ҿ����̿��
	for (int i = 0;i < BlockSum;i++)
		if (*(DiskBase + i) == 0)
		{
			*(DiskBase + i) = -1;
			return i;
		}
	return -1;//-1��ʾ�Ҳ������̿�
}//GetBlock()

FCB* GetAddr(int BlockNo)
{//����BlockNo�̿��FCB��ַ
	FCB* ptr;
	ptr = (FCB*)(DiskBase + BlockNo * (BlockSize / sizeof(int)));
	return ptr;
}

int GetFileItem(int BlockNo)
{//������̿��׵�ַ���״��̿�,����µĴ��̿��׵�ַFileBase��һ����Ӧ�����ļ����ƿ�
	//FCB *pFileItem;
	int ParentBlock;
	int i;
	//	BlockNo = AddrToBlock(FileBase);
	do
	{
		ParentBlock = BlockNo;
		FileBase = GetAddr(BlockNo);
		for (i = 0;i < MaxFileItems;i++)
		{
			if ((FileBase + i)->FreeOrNot == 0)
			{
				(FileBase + i)->FreeOrNot = -1;
				return i;
			}
		}
		BlockNo = *(DiskBase + ParentBlock);//ָ����������һ���̿�
	} while (BlockNo != -1);

	BlockNo = GetBlock();//�������������̿鶼�����ã���������һ��
	if (BlockNo == -1)
	{
		cout << "���̿�����ʧ��!" << endl;
		return -1;
	}
	*(DiskBase + ParentBlock) = BlockNo;//��������Ĵ��̿���������
	FileBase = GetAddr(BlockNo);
	return 0;//������Ĵ��̿�ĵ�һ���ļ���϶������õ�
}

int SearchFile(char* FileName)
{//�ڵ�ǰĿ¼�²���ָ���ļ�,�����򷵻ش��̿��׵�ַ���ļ����ƿ�,���򷵻�-1
	int i;
	int BlockNo;
	BlockNo = CurrentDir;
	do
	{
		FileBase = GetAddr(BlockNo);
		for (i = 0;i < MaxFileItems;i++)//���Ƿ���ͬһ���������ҵ����ļ�
		{
			if ((FileBase + i)->FreeOrNot == 0) continue;
			if ((FileBase + i)->FileOrNot == '0') continue;
			if (strcmp((FileBase + i)->FileName, FileName) == 0 && (FileBase + i)->user == user) return i;
		}
		BlockNo = *(DiskBase + BlockNo);
	} while (BlockNo != -1);
	return -1;//����-1��ʾ�Ҳ������ļ�
}

int SearchFolder(const char* Folder)
{//�ڵ�ǰĿ¼�²���ָ���ļ�,�����򷵻ش��̿��׵�ַ���ļ����ƿ�,���򷵻�-1
	int i;
	int BlockNo;
	BlockNo = CurrentDir;
	do
	{
		FileBase = GetAddr(BlockNo);
		for (i = 0;i < MaxFileItems;i++)//���Ƿ���ͬһ���������ҵ����ļ���
		{
			if ((FileBase + i)->FreeOrNot == 0) continue;
			if ((FileBase + i)->FileOrNot == '1') continue;
			if (strcmp((FileBase + i)->FileName, Folder) == 0 && (FileBase + i)->user == user) return i;
		}
		BlockNo = *(DiskBase + BlockNo);
	} while (BlockNo != -1);
	return -1;//����-1��ʾ�Ҳ������ļ���
}

void InitAllFileItem()
{//��ʼ�����е��ļ���
	int i, j;
	FCB* pFileItem;
	for (j = TableBlocks;j < BlockSum;j++)//�������д��̿�
	{
		pFileItem = GetAddr(j);
		for (i = 0;i < MaxFileItems;i++)//����ÿ�����̿�������ļ���
		{
			(pFileItem + i)->FileName[0] = '\0';
			(pFileItem + i)->FileOrNot = '0';
			(pFileItem + i)->FileSize = 0;
			(pFileItem + i)->FirstBlock = 0;
			(pFileItem + i)->FreeOrNot = 0;
			strcpy((pFileItem + i)->FileDate, GetNowDateTime());
		}
	}
}

void InitFileSys()
{
	int i;
	int BlockNo;
	int FileItem;
	CurrentDir = TableBlocks;
	//��ʼ��FAT��
	for (i = 0;i < TableBlocks;i++)
		*(DiskBase + i) = -1;//��Щ���ڴ��FAT��
	for (i = TableBlocks;i < BlockSum;i++)
		*(DiskBase + i) = 0;
	//��ʼ�����е��ļ���
	InitAllFileItem();
	BlockNo = GetBlock();//������̿�
	if (BlockNo == -1)
	{
		cout << "���̿�����ʧ��!" << endl;
		exit(0);
	}
	FileBase = GetAddr(BlockNo);//���̿���׵�ַ
	FileItem = GetFileItem(BlockNo);
	/*if(FileItem == -1)
	{//��ʼ�����տ�ʼʱ�ļ����ƿ鲻��������ʧ��
	cout<<"�ļ����ƿ�����ʧ��!"<<endl;
	exit(0);
	}*/
	strcpy((FileBase + FileItem)->FileName, ".");
	(FileBase + FileItem)->FileOrNot = '0';//���ļ���
	(FileBase + FileItem)->FirstBlock = BlockNo;
	FileItem = GetFileItem(BlockNo);
	strcpy((FileBase + FileItem)->FileName, "..");
	(FileBase + FileItem)->FileOrNot = '0';
	(FileBase + FileItem)->FirstBlock = BlockNo;
}

void format()
{
	char FormatOrNot = 'n';
	cout << "��ʽ����ɾ�����е��ļ���Ϣ�����Ƿ�Ҫ��ʽ��? y or n" << endl;
	cin >> FormatOrNot;
	if (FormatOrNot == 'y' || FormatOrNot == 'Y')
	{
		free(DiskBase);
		DiskBase = NULL;
		DiskBase = (int*)malloc(BlockSize * BlockSum);
		if (DiskBase == NULL)
		{
			cout << "�����ڴ�ռ�ʧ�ܣ�" << endl;
			exit(0);
		}
		CurrentDir = TableBlocks;
		CURRENT = "A:\\";
		InitFileSys();
		cout << "��ʽ���ļ�ϵͳ��ɣ�" << endl;
		return;
	}
	return;
}

int mkdir(char* SonDirName)
{
	int FileItem;
	int BlockNo;
	if (SearchFolder(SonDirName) == -1)//���ж�Ҫ�������ļ�����û������
	{
		BlockNo = GetBlock();//��ȡһ���µĴ��̿ռ�,���ڴ�Ÿմ����ļ����µ�����
		if (BlockNo == -1)
		{
			cout << "�����ļ���" << SonDirName << "ʧ��!" << endl;
			return -1;
		}
		FileBase = GetAddr(CurrentDir);
		FileItem = GetFileItem(CurrentDir);
		if (FileItem == -1)
		{
			cout << "�����ļ���" << SonDirName << "ʧ��!" << endl;
			FreeBlock(BlockNo);
			return -1;
		}
		strcpy((FileBase + FileItem)->FileName, SonDirName);
		strcpy((FileBase + FileItem)->FileDate, GetNowDateTime());
		(FileBase + FileItem)->FileOrNot = '0';//���ļ���
		(FileBase + FileItem)->FirstBlock = BlockNo;
		(FileBase + FileItem)->user = user;
		FileBase = GetAddr(BlockNo);
		FileItem = GetFileItem(BlockNo);
		strcpy((FileBase + FileItem)->FileName, ".");//��ʾ�Լ�
		strcpy((FileBase + FileItem)->FileDate, GetNowDateTime());
		(FileBase + FileItem)->user = user;
		(FileBase + FileItem)->FileOrNot = '0';//���ļ���
		(FileBase + FileItem)->FirstBlock = BlockNo;
		FileBase = GetAddr(BlockNo);
		FileItem = GetFileItem(BlockNo);
		strcpy((FileBase + FileItem)->FileName, "..");//��ʾ�ϼ�Ŀ¼
		strcpy((FileBase + FileItem)->FileDate, GetNowDateTime());
		(FileBase + FileItem)->FileOrNot = '0';//���ļ���
		(FileBase + FileItem)->FirstBlock = CurrentDir;
		(FileBase + FileItem)->user = user;

		cout << "\n�ļ��� " << SonDirName << "  �����ɹ���" << endl;
		return 0;
	}
	cout << "�ļ���" << SonDirName << "����!" << endl;
	return -1;
}

int rmdir(char* SonDirName)
{
	int i;
	int FileItem;
	int BlockNo, ParentBlock;
	FCB* pFileItem;
	FCB* tempPtr;
	FileItem = SearchFolder(SonDirName);
	tempPtr = FileBase;
	if (FileItem == -1)
	{
		cout << "��Ҫɾ�����ļ���" << SonDirName << "�����ڣ�" << endl;
		return -1;
	}
	else
	{
		//��ɾ������ʼ��Ŀ¼����ָ�Ĵ��̿�
		BlockNo = (tempPtr + FileItem)->FirstBlock;
		do
		{
			ParentBlock = BlockNo;
			pFileItem = GetAddr(BlockNo);
			(pFileItem + 0)->FileName[0] = '\0';
			(pFileItem + 0)->FileOrNot = '0';
			(pFileItem + 0)->FileSize = 0;
			(pFileItem + 0)->FirstBlock = 0;
			(pFileItem + 0)->FreeOrNot = 0;
			strcpy((pFileItem + 0)->FileDate, GetNowDateTime());
			(pFileItem + 1)->FileName[0] = '\0';
			(pFileItem + 1)->FileOrNot = '0';
			(pFileItem + 1)->FileSize = 0;
			(pFileItem + 1)->FirstBlock = 0;
			(pFileItem + 1)->FreeOrNot = 0;
			strcpy((pFileItem + 1)->FileDate, GetNowDateTime());
			for (i = 2;i < MaxFileItems;i++)
			{//��ʼ��Ҫɾ�����ļ������ļ����ƿ�
				if ((pFileItem + i)->FreeOrNot == 0) continue;
				else
				{
					FileBase = pFileItem;
					DelBlankDir(i);
				}
			}//for
			BlockNo = *(DiskBase + ParentBlock);//ָ����������һ���̿�
			*(DiskBase + ParentBlock) = 0;
		} while (BlockNo != -1);
		//����ɾ��Ŀ¼��
		(tempPtr + FileItem)->FileName[0] = '\0';
		(tempPtr + FileItem)->FileOrNot = '0';
		(tempPtr + FileItem)->FileSize = 0;
		(tempPtr + FileItem)->FirstBlock = 0;
		(tempPtr + FileItem)->FreeOrNot = 0;

		cout << "\n�ļ��� " << SonDirName << "  ɾ���ɹ���" << endl;
		return 0;
	}
}

void ls()
{
	FCB* pFileItem;
	int BlockNo, ParentBlock;
	int SumFile, SumDir;
	SumFile = SumDir = 0;
	//������ǰĿ¼
	BlockNo = CurrentDir;
	pFileItem = GetAddr(BlockNo);
	cout << "*******************��ǰĿ¼����������********************" << endl;
	do
	{
		ParentBlock = BlockNo;
		pFileItem = GetAddr(BlockNo);
		for (int i = 0;i < MaxFileItems;i++)
		{
			if ((pFileItem + i)->FreeOrNot && (pFileItem + i)->user == user)//�Ƿ�����ļ����ļ���
			{
				if ((pFileItem + i)->FileOrNot == '1')//���ļ�
				{
					cout << (pFileItem + i)->FileDate << "	�ı��ļ� " << (pFileItem + i)->FileName << "	��С" << (pFileItem + i)->FileSize << "�ֽ�" << endl;
					SumFile++;
				}
				else if ((pFileItem + i)->FileOrNot == '0')
				{
					if (strcmp((pFileItem + i)->FileName, "..") == 0 || strcmp((pFileItem + i)->FileName, ".") == 0)
						continue;
					cout << (pFileItem + i)->FileDate << "	�ļ���	" << (pFileItem + i)->FileName << endl;
					SumDir++;
				}
			}
		}//for
		BlockNo = *(DiskBase + ParentBlock);//ָ����������һ���̿�
	} while (BlockNo != -1);
	cout << "\n�ڸ�Ŀ¼�¹���" << SumFile << " ���ı��ļ�, " << SumDir << " ���ļ���\n\n";
	cout << "*********************************************************" << endl;
	return;
}

int cd(char* SonDirName)
{
	int BlockNo, ParentBlock;
	int FileItem, i;
	if (strcmp(SonDirName, "..") == 0)
	{/*�ص���Ŀ¼*/
		if (CurrentDir == TableBlocks)//����ǰĿ¼�Ǹ�Ŀ¼
		{
			printf("���Ѿ��ڸ�Ŀ¼����!\n");
			return 0;
		}
		BlockNo = CurrentDir;
		FileBase = GetAddr(BlockNo);
		FileItem = SearchFolder("..");
		ParentBlock = (FileBase + FileItem)->FirstBlock;//��ȡ�ϼ�Ŀ¼���̿�
		CurrentDir = ParentBlock;
		//���ϼ�Ŀ¼�в����׿��ΪBlockNo���ļ�����
		do
		{
			FileItem = -1;
			FileBase = GetAddr(ParentBlock);
			for (i = 0;i < MaxFileItems;i++)
			{
				if ((FileBase + i)->FreeOrNot == 0) continue;
				if ((FileBase + i)->FirstBlock == BlockNo && (FileBase + i)->user == user)
				{
					FileItem = i;//�ҵ����ϼ�Ŀ¼�ļ���
					break;
				}
			}
			ParentBlock = *(DiskBase + ParentBlock);
		} while (ParentBlock != -1);
		/*if(FileItem == -1)
		{//��һ������ǲ������е���Ϊ�����ļ��гɹ���".."��ָ���ϼ�Ŀ¼Ҳ�϶���ȷ
		cout<<"�Ҳ����ϼ�Ŀ¼�ļ���!"<<endl;
		return -1;
		}*/
		CURRENT = CURRENT.substr(0, CURRENT.length() - strlen((FileBase + FileItem)->FileName) - 1);
		//CURRENT = CURRENT + '>';
		return 0;
	}
	else
	{//���cd�����Ϊ�ļ���
		BlockNo = CurrentDir;
		FileBase = GetAddr(BlockNo);
		FileItem = SearchFolder(SonDirName);
		if (FileItem == -1)
		{
			cout << "�ļ���" << SonDirName << "������!" << endl;
			return -1;
		}
		BlockNo = (FileBase + FileItem)->FirstBlock;//��ȡ�ϼ�Ŀ¼���̿�
		CurrentDir = BlockNo;

		CURRENT = CURRENT + (FileBase + FileItem)->FileName + "\\";
		return 0;
	}
}

int create(char* FileName)
{
	int FileItem;
	int BlockNo;
	FCB* pFileItem;
	if (SearchFile(FileName) == -1)//���ж�Ҫ�������ļ���û������
	{
		BlockNo = GetBlock();//��ȡһ���µĴ��̿ռ�,���ڴ�Ÿմ����ļ��µ�����
		if (BlockNo == -1)
		{
			cout << "�����ļ�" << FileName << "ʧ��!" << endl;
			return -1;
		}
		pFileItem = GetAddr(CurrentDir);
		FileItem = GetFileItem(CurrentDir);
		if (FileItem == -1)
		{
			cout << "�����ļ�" << FileName << "ʧ��!" << endl;
			FreeBlock(BlockNo);
			return -1;
		}
		strcpy((pFileItem + FileItem)->FileName, FileName);
		strcpy((pFileItem + FileItem)->FileDate, GetNowDateTime());
		(pFileItem + FileItem)->FileOrNot = '1';//���ļ�
		(pFileItem + FileItem)->FileSize = 0;
		(pFileItem + FileItem)->FirstBlock = BlockNo;
		(pFileItem + FileItem)->user = user;
		cout << "\n�ļ� " << FileName << "  �����ɹ���" << endl;
		return 0;
	}
	cout << "��Ҫ�������ļ��Ѵ���!" << endl;
	return -1;
}

int read(char* FileName)
{
	int BlockNo;
	int FileItem;
	int j, i;
	int FileFirstBlock = -1;//���ڴ���ļ����׿�������
	char* FilePtr;
	FileItem = SearchFile(FileName);
	if (FileItem == -1)
	{
		cout << "��Ҫ��ȡ���ļ�" << "������!" << endl;
		return -1;

	}
	//�ҵ����ļ��������ļ�����
	BlockNo = (FileBase + FileItem)->FirstBlock;
	FilePtr = (char*)GetAddr(BlockNo);//��ȡ���ݿ���ײ���ַ
	//����ļ��е�����
	if ((FileBase + FileItem)->FileSize == 0)
	{
		cout << "�ļ�" << FileName << "Ϊ��!" << endl;
		return 0;
	}
	cout << "\n********************************�ļ���������**********************************" << endl;
	i = -1;
	for (j = 0;j < (FileBase + FileItem)->FileSize;j++)
	{
		if (i > BlockSize - 2)//�����̿������,ע�⣺i=511ʱ�������������
		{//��ȡ��һ����̿�
			i = 0;
			BlockNo = *(DiskBase + BlockNo);
			if (BlockNo == -1)
			{
				cout << "����!�ļ��Ѿ���������" << endl;
				return -1;
			}
			FilePtr = (char*)GetAddr(BlockNo);
		}
		else i++;

		putchar(*(FilePtr + i));


	}
	cout << "\n********************************�ļ���ȡ���!**********************************" << endl;
	//	IsOpen == false;
	return 0;
}

int write(char* FileName)
{
	int BlockNo, ParentBlock;
	char input;
	int FileItem;
	char* FilePtr;
	int j = -1;
	FileItem = SearchFile(FileName);
	if (FileItem == -1)
	{
		cout << "��Ҫ��д���ļ�" << FileName << "������!" << endl;
		return -1;

	}
	ClearFileBlock(FileName);//����ļ�����
	//д���ļ�����
	strcpy((FileBase + FileItem)->FileDate, GetNowDateTime());
	BlockNo = (FileBase + FileItem)->FirstBlock;
	FilePtr = (char*)GetAddr(BlockNo);//��ȡ�����ݿ���ײ���ַ
	cout << "�������ļ����ݣ�" << endl;
	getchar();
	ParentBlock = BlockNo;
	while ((input = getchar()) != '$')
	{
		if (j > BlockSize - 2)//�����̿�����,ע�⣺i=511ʱ�������������
		{//����һ����̿�
			j = 0;
			BlockNo = GetBlock();
			*(DiskBase + ParentBlock) = BlockNo;
			if (BlockNo == -1)
			{
				cout << "���̿ռ�������!" << endl;
				cout << "�����ɾ��һЩ�ļ����ļ��к����ԣ�" << endl;
				return -1;
			}
			FilePtr = (char*)GetAddr(BlockNo);
			ParentBlock = BlockNo;
		}
		else j++;
		*(FilePtr + j) = input;
		(FileBase + FileItem)->FileSize++;
	}
	return 0;
}

int rm(char* FileName)
{
	int i;
	int FileItem;
	int BlockNo, ParentBlock;
	FCB* pFileItem;
	FileItem = SearchFile(FileName);
	if (FileItem == -1)
	{
		cout << "��Ҫɾ�����ļ�" << FileName << "�����ڣ�" << endl;
		return -1;
	}
	else
	{
		//��ɾ������ʼ���ļ�����ָ�Ĵ��̿�
		BlockNo = (FileBase + FileItem)->FirstBlock;
		do
		{
			ParentBlock = BlockNo;
			pFileItem = GetAddr(BlockNo);
			for (i = 0;i < MaxFileItems;i++)
			{//��ʼ��Ҫɾ�����ļ����ļ����ƿ�
				if ((pFileItem + i)->FreeOrNot == 0) continue;
				else
				{
					(pFileItem + i)->FileName[0] = '\0';
					(pFileItem + i)->FileOrNot = '0';
					(pFileItem + i)->FileSize = 0;
					(pFileItem + i)->FirstBlock = 0;
					(pFileItem + i)->FreeOrNot = 0;
					strcpy((pFileItem + i)->FileDate, GetNowDateTime());
				}
			}//for
			BlockNo = *(DiskBase + ParentBlock);//ָ����������һ���̿�
			*(DiskBase + ParentBlock) = 0;
		} while (BlockNo != -1);
		//����ɾ���ļ�Ŀ¼��
		(FileBase + FileItem)->FileName[0] = '\0';
		(FileBase + FileItem)->FileOrNot = '0';
		(FileBase + FileItem)->FileSize = 0;
		(FileBase + FileItem)->FirstBlock = 0;
		(FileBase + FileItem)->FreeOrNot = 0;
		cout << "\n�ļ� " << FileName << "  ɾ���ɹ���" << endl;
		return 0;
	}
}

int exit()
{
	char QuitOrNot = 'n';
	cout << "���Ƿ�Ҫ�����ļ�ϵͳ��Ӳ�̲��˳�? y or n" << endl;
	cin >> QuitOrNot;
	if (QuitOrNot == 'y' || QuitOrNot == 'Y')
	{
		fp = fopen(FileSavePath, "w+");
		if (fp == NULL)
		{
			cout << "���ļ�" << FileSavePath << "ʧ�ܣ�" << endl;
			cout << "Sorry! -_- �ļ�ϵͳ�޷������˳��ͱ��浽Ӳ���ϣ�" << endl;
			return -1;
		}
		fwrite(DiskBase, sizeof(int), (BlockSize / sizeof(int)) * BlockSum, fp);
		cout << "�ļ�ϵͳ����ɹ���" << endl;
		fclose(fp);
		free(DiskBase);
		exit(0);
	}
	cout << "��û�б����ļ�ϵͳ��Ӳ��!" << endl;
	return -1;
}
int SaveFileSystem()
{
	char SaveOrNot = 'n';
	cout << "���Ƿ�Ҫ�����ļ�ϵͳ��Ӳ��? y or n" << endl;
	cin >> SaveOrNot;
	if (SaveOrNot == 'y' || SaveOrNot == 'Y')
	{
		fp = fopen(FileSavePath, "w+");
		if (fp == NULL)
		{
			cout << "���ļ�" << FileSavePath << "ʧ�ܣ�" << endl;
			cout << "Sorry! -_- �ļ�ϵͳ�޷������˳��ͱ��浽Ӳ���ϣ�" << endl;
			return -1;
		}
		fwrite(DiskBase, sizeof(int), (BlockSize / sizeof(int)) * BlockSum, fp);
		cout << "�ļ�ϵͳ����ɹ���" << endl;
		fclose(fp);
		//free(DiskBase);
		return 0;
	}
	return -1;
}
int main()
{
	CurrentDir = TableBlocks;
	//����FAT�ļ�ϵͳ���̲��ҳ�ʼ��
	DiskBase = (int*)malloc(BlockSize * BlockSum);
	fp = fopen(FileSavePath, "r");
	if (fp == NULL)
	{
		InitFileSys();
	}
	else
	{
		cout << "����ϵͳ.";
		::Sleep(500);
		cout << ".";
		::Sleep(500);
		cout << ".";
		::Sleep(500);
		cout << ".";
		::Sleep(500);
		cout << ".";
		::Sleep(500);
		fread(DiskBase, sizeof(int), (BlockSize / sizeof(int)) * BlockSum, fp);
		if (DiskBase == NULL)
		{
			cout << "FileSystem�ļ���ȡʧ��!" << endl;
			fclose(fp);
			exit(0);
		}
		fclose(fp);
	}
	printf("         ��ӭʹ���ļ�ϵͳ!            \n");
	char SaveOrNot = 'n';
	cout << "�Ƿ�Ҫ��ʽ��ϵͳ? y or n" << endl;
	cin >> SaveOrNot;
	if (SaveOrNot == 'y' || SaveOrNot == 'Y')
	{
		format();
	}
	cout << endl << endl;
	cout << "��ѡ���û���½!: ";
	getchar();
	char in_char;
	std::cin.clear();
	in_char = getchar();
	user = in_char;
	if (in_char == '1' || in_char == '2')
	{
		cout << endl << "�����û�" << in_char << "������!" << endl;
	}
	ShowCommandHelp();
	while (1)
	{//��ʼʹ�ù���
		cout << CURRENT << ">";
		std::cin.clear();//��ջ�����
		cin >> cmd;
		for (int i = 0;i < cmd.length();i++)
			cmd[i] = tolower(cmd[i]);//���Դ�Сд
		if (cmd == "format") {
			format();
		}
		else if (cmd == "cursyspath") {
			CurSysPath();
		}
		else if (cmd == "delete")
		{
			cin >> command;
			if (CheckFileName(command) == -1) //�ļ������Ϸ�
			{
				cout << "������ļ���̫��! ������!" << endl;
				continue;
			}
			DelFileSys(command);
		}
		else if (cmd == "mkdir") {
			cin >> command;
			if (CheckFileName(command) == -1) //�ļ������Ϸ�
			{
				cout << "������ļ���̫��! ������!" << endl;
				continue;
			}
			mkdir(command);
		}
		else if (cmd == "rmdir") {
			cin >> command;
			if (CheckFileName(command) == -1) //�ļ������Ϸ�
			{
				cout << "������ļ���̫��! ������!" << endl;
				continue;
			}
			rmdir(command);
		}
		else if (cmd == "ls") {
			ls();
		}
		else if (cmd == "cd") {
			cin >> command;
			if (CheckFileName(command) == -1) //�ļ������Ϸ�
			{
				cout << "������ļ���̫��! ������!" << endl;
				continue;
			}
			cd(command);
		}
		else if (cmd == "create") {
			cin >> command;
			if (CheckFileName(command) == -1) //�ļ������Ϸ�
			{
				cout << "������ļ���̫��! ������!" << endl;
				continue;
			}
			create(command);
		}
		else if (cmd == "saveas") {
			cin >> command;
			if (CheckFileName(command) == -1) //�ļ������Ϸ�
			{
				cout << "������ļ���̫��! ������!" << endl;
				continue;
			}
			//SaveAs(command);
		}
		else if (cmd == "close") {
			cin >> command;
			if (CheckFileName(command) == -1) //�ļ������Ϸ�
			{
				cout << "������ļ���̫��! ������!" << endl;
				continue;
			}
			close(command);
		}
		else if (cmd == "open") {
			cin >> command;
			if (CheckFileName(command) == -1) //�ļ������Ϸ�
			{
				cout << "������ļ���̫��! ������!" << endl;
				continue;
			}
			open(command);
		}
		else if (cmd == "write") {
			cin >> command;
			if (CheckFileName(command) == -1) //�ļ������Ϸ�
			{
				cout << "������ļ���̫��! ������!" << endl;
				continue;
			}
			write(command);
		}
		else if (cmd == "read") {
			cin >> command;
			if (CheckFileName(command) == -1) //�ļ������Ϸ�
			{
				cout << "������ļ���̫��! ������!" << endl;
				continue;
			}
			read(command);
		}
		else if (cmd == "rm") {
			cin >> command;
			if (CheckFileName(command) == -1) //�ļ������Ϸ�
			{
				cout << "������ļ���̫��! ������!" << endl;
				continue;
			}
			rm(command);
		}
		else if (cmd == "save") SaveFileSystem();
		else if (cmd == "help") ShowCommandHelp();
		else if (cmd == "exit") {
			exit();
			break;
		}
		else
		{
			std::cin.clear();//��ջ�����
			cout << "��Чָ��,����������!" << endl;
			std::cin.clear();//��ջ�����
		}
	}
	cout << "Thank you for using DaiBin's file system! ^o^\n\n\n" << endl;
	return 0;
}
