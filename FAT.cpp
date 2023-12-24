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
	char FileName[MAX_FILE_NAME_LENGTH];   //文件名
	int FirstBlock;//文件首块号
	long int FileSize;    //文件大小,单位为字节数
	char FileDate[MAX_TIME_LENGTH];//文件创建修改时间
	char FileOrNot;//'1'：文件；'0'：文件夹 !注意：这里是字符
	int FreeOrNot;//文件控制块是否为空0:空闲-1:忙
	char user;
}FCB;
/*全局变量*/
const char* FileSavePath = "FileSystem.sys";
int BlockSize = 512 * sizeof(char);       /*每个盘块大小(可配置),默认B*/
int BlockSum = 500;   /*盘块数*/
int MaxTableItems = (BlockSize / sizeof(int));//每个磁盘块可以存放的FAT表项数
int MaxFileItems = (BlockSize / sizeof(FCB));//每个磁盘块可以存放的文件控制块数
int TableBlocks = BlockSum / MaxTableItems + 1; //TAB表占用的磁盘块
int FcbSum = BlockSize / sizeof(FCB);//每个盘块中所有的FCB目录项个数
int tempBlock;
bool IsOpen = false;
char StrTime[MAX_TIME_LENGTH] = { '\0' };
FILE* fp = NULL;      //磁盘文件地址
int* DiskBase;    //FAT文件系统磁盘空间基地址
FCB* FileBase;//文件地址
string cmd;      //输入指令
string CURRENT = "A:\\";   //当前路径
char command[MAX_FILE_NAME_LENGTH];    //文件名标识
int CurrentDir;    //当前目录的盘块号
char user;


int DelBlankDir(int FileItem)
{
	int i;
	int BlockNo, ParentBlock;
	FCB* pFileItem;
	FCB* tempPtr;
	tempPtr = FileBase;
	//先删除并初始化目录项所指的磁盘块
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
		{//初始化要删掉的文件夹中文件控制块
			if ((pFileItem + i)->FreeOrNot == 0) continue;
			else
			{
				if ((pFileItem + i)->FileOrNot == '1')//如果是文件
					rm((pFileItem + i)->FileName);
				else if ((pFileItem + i)->FileOrNot == '0')//如果是文件夹
				{
					(pFileItem + i)->FreeOrNot = 0;
					FileBase = pFileItem;
					DelBlankDir(i);
				}
			}
		}//for
		BlockNo = *(DiskBase + ParentBlock);//指向相连的另一磁盘块
		*(DiskBase + ParentBlock) = 0;
	} while (BlockNo != -1);
	//接着删除目录项
	(tempPtr + FileItem)->FileName[0] = '\0';
	(tempPtr + FileItem)->FileOrNot = '0';
	(tempPtr + FileItem)->FileSize = 0;
	(tempPtr + FileItem)->FirstBlock = 0;
	(tempPtr + FileItem)->FreeOrNot = 0;
	return 0;
}

char* GetNowDateTime()
{//获取系统当前时间，并返回其字符串
	time_t   ltime;
	struct   tm* today;
	time(&ltime);
	today = localtime(&ltime);   //"%4d-%02d-%02d   %02d:%02d:%02d   
	sprintf(StrTime, "%4d-%02d-%02d  %02d:%02d:%02d\0", (today->tm_year + 1900), (today->tm_mon + 1), (today->tm_mday), (today->tm_hour), (today->tm_min), (today->tm_sec));
	return StrTime;
}
int DelFileSys(char* FileName)
{ //删除磁盘上保存的文件系统
	char   comdel[100] = "del   ";
	strcat(comdel, FileName);
	if (system(comdel) == 0)
		cout << FileName << "删除成功！" << endl;
	return   0;
}
void CurSysPath()
{//显示当前文件系统目录
	cout << "当前系统默认存储目录" << FileSavePath << endl << endl;
	return;
}

void ShowCommandHelp()
{//显示帮助信息
	printf("\n***************以下是使用说明书****************\n");
	printf("help :显示帮助信息.\n");
	printf("format :对磁盘格式化.\n");
	printf("save :覆盖保存在硬盘上的文件系统，但不退出.\n");
	printf("cursyspath  :显示当前系统默认存储目录.\n");
	printf("exit   :安全退出该文件系统,保存信息.\n");
	printf("mkdir dirname :创建子目录.\n");
	printf("rmdir dirname :删除子目录.\n");
	printf("ls     dirname :显示当前目录下信息.\n");
	printf("cd     dirname :更改当前目录.\n");
	printf("create filename :创建一个新文件,并且打开.\n");
	printf("close filename :关闭文件.\n");
	printf("write filename :选择一个打开的文件写入信息,以$表示结束.\n");
	printf("read   filename :选择一个打开的文件读取信息.\n");
	printf("rm     filename :删除文件.\n");
	cout << "***************************************************\n\n\n";
}

void FreeBlock(int BlockNo)
{//释放磁盘块为BlockNo的磁盘块
	if (BlockNo < 0 || BlockNo > BlockSum - 1)
	{
		cout << "要释放的磁盘块号非法!" << endl;
		return;
	}
	*(DiskBase + BlockNo) = 0;
}

int CheckFileName(char* FileName)
{//核查文件名是否合法
	if (strlen(FileName) > MAX_FILE_NAME_LENGTH)
	{
		std::cin.clear();//清空缓冲区
		return -1;
	}
	return 0;
}

int GetBlock()
{//寻找空闲盘块号
	for (int i = 0;i < BlockSum;i++)
		if (*(DiskBase + i) == 0)
		{
			*(DiskBase + i) = -1;
			return i;
		}
	return -1;//-1表示找不到磁盘块
}//GetBlock()

FCB* GetAddr(int BlockNo)
{//返回BlockNo盘块的FCB地址
	FCB* ptr;
	ptr = (FCB*)(DiskBase + BlockNo * (BlockSize / sizeof(int)));
	return ptr;
}

int GetFileItem(int BlockNo)
{//输入磁盘块首地址和首磁盘块,输出新的磁盘块首地址FileBase和一个对应的新文件控制块
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
		BlockNo = *(DiskBase + ParentBlock);//指向相连的另一磁盘块
	} while (BlockNo != -1);

	BlockNo = GetBlock();//若所有相连磁盘块都不够用，则再申请一块
	if (BlockNo == -1)
	{
		cout << "磁盘块申请失败!" << endl;
		return -1;
	}
	*(DiskBase + ParentBlock) = BlockNo;//将新申请的磁盘块连接起来
	FileBase = GetAddr(BlockNo);
	return 0;//新申请的磁盘块的第一个文件项肯定可以用的
}

int SearchFile(char* FileName)
{//在当前目录下查找指定文件,存在则返回磁盘块首地址和文件控制块,否则返回-1
	int i;
	int BlockNo;
	BlockNo = CurrentDir;
	do
	{
		FileBase = GetAddr(BlockNo);
		for (i = 0;i < MaxFileItems;i++)//看是否在同一个块中能找到该文件
		{
			if ((FileBase + i)->FreeOrNot == 0) continue;
			if ((FileBase + i)->FileOrNot == '0') continue;
			if (strcmp((FileBase + i)->FileName, FileName) == 0 && (FileBase + i)->user == user) return i;
		}
		BlockNo = *(DiskBase + BlockNo);
	} while (BlockNo != -1);
	return -1;//返回-1表示找不到该文件
}

int SearchFolder(const char* Folder)
{//在当前目录下查找指定文件,存在则返回磁盘块首地址和文件控制块,否则返回-1
	int i;
	int BlockNo;
	BlockNo = CurrentDir;
	do
	{
		FileBase = GetAddr(BlockNo);
		for (i = 0;i < MaxFileItems;i++)//看是否在同一个块中能找到该文件夹
		{
			if ((FileBase + i)->FreeOrNot == 0) continue;
			if ((FileBase + i)->FileOrNot == '1') continue;
			if (strcmp((FileBase + i)->FileName, Folder) == 0 && (FileBase + i)->user == user) return i;
		}
		BlockNo = *(DiskBase + BlockNo);
	} while (BlockNo != -1);
	return -1;//返回-1表示找不到该文件夹
}

void InitAllFileItem()
{//初始化所有的文件项
	int i, j;
	FCB* pFileItem;
	for (j = TableBlocks;j < BlockSum;j++)//对于所有磁盘块
	{
		pFileItem = GetAddr(j);
		for (i = 0;i < MaxFileItems;i++)//对于每个磁盘块的所有文件项
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
	//初始化FAT表
	for (i = 0;i < TableBlocks;i++)
		*(DiskBase + i) = -1;//这些用于存放FAT表
	for (i = TableBlocks;i < BlockSum;i++)
		*(DiskBase + i) = 0;
	//初始化所有的文件项
	InitAllFileItem();
	BlockNo = GetBlock();//申请磁盘块
	if (BlockNo == -1)
	{
		cout << "磁盘块申请失败!" << endl;
		exit(0);
	}
	FileBase = GetAddr(BlockNo);//磁盘块的首地址
	FileItem = GetFileItem(BlockNo);
	/*if(FileItem == -1)
	{//初始化，刚开始时文件控制块不可能申请失败
	cout<<"文件控制块申请失败!"<<endl;
	exit(0);
	}*/
	strcpy((FileBase + FileItem)->FileName, ".");
	(FileBase + FileItem)->FileOrNot = '0';//表文件夹
	(FileBase + FileItem)->FirstBlock = BlockNo;
	FileItem = GetFileItem(BlockNo);
	strcpy((FileBase + FileItem)->FileName, "..");
	(FileBase + FileItem)->FileOrNot = '0';
	(FileBase + FileItem)->FirstBlock = BlockNo;
}

void format()
{
	char FormatOrNot = 'n';
	cout << "格式化会删除所有的文件信息，你是否要格式化? y or n" << endl;
	cin >> FormatOrNot;
	if (FormatOrNot == 'y' || FormatOrNot == 'Y')
	{
		free(DiskBase);
		DiskBase = NULL;
		DiskBase = (int*)malloc(BlockSize * BlockSum);
		if (DiskBase == NULL)
		{
			cout << "申请内存空间失败！" << endl;
			exit(0);
		}
		CurrentDir = TableBlocks;
		CURRENT = "A:\\";
		InitFileSys();
		cout << "格式化文件系统完成！" << endl;
		return;
	}
	return;
}

int mkdir(char* SonDirName)
{
	int FileItem;
	int BlockNo;
	if (SearchFolder(SonDirName) == -1)//先判断要创建的文件夹有没有重名
	{
		BlockNo = GetBlock();//获取一个新的磁盘空间,用于存放刚创建文件夹下的内容
		if (BlockNo == -1)
		{
			cout << "创建文件夹" << SonDirName << "失败!" << endl;
			return -1;
		}
		FileBase = GetAddr(CurrentDir);
		FileItem = GetFileItem(CurrentDir);
		if (FileItem == -1)
		{
			cout << "创建文件夹" << SonDirName << "失败!" << endl;
			FreeBlock(BlockNo);
			return -1;
		}
		strcpy((FileBase + FileItem)->FileName, SonDirName);
		strcpy((FileBase + FileItem)->FileDate, GetNowDateTime());
		(FileBase + FileItem)->FileOrNot = '0';//表文件夹
		(FileBase + FileItem)->FirstBlock = BlockNo;
		(FileBase + FileItem)->user = user;
		FileBase = GetAddr(BlockNo);
		FileItem = GetFileItem(BlockNo);
		strcpy((FileBase + FileItem)->FileName, ".");//表示自己
		strcpy((FileBase + FileItem)->FileDate, GetNowDateTime());
		(FileBase + FileItem)->user = user;
		(FileBase + FileItem)->FileOrNot = '0';//表文件夹
		(FileBase + FileItem)->FirstBlock = BlockNo;
		FileBase = GetAddr(BlockNo);
		FileItem = GetFileItem(BlockNo);
		strcpy((FileBase + FileItem)->FileName, "..");//表示上级目录
		strcpy((FileBase + FileItem)->FileDate, GetNowDateTime());
		(FileBase + FileItem)->FileOrNot = '0';//表文件夹
		(FileBase + FileItem)->FirstBlock = CurrentDir;
		(FileBase + FileItem)->user = user;

		cout << "\n文件夹 " << SonDirName << "  创建成功！" << endl;
		return 0;
	}
	cout << "文件夹" << SonDirName << "重名!" << endl;
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
		cout << "你要删除的文件夹" << SonDirName << "不存在！" << endl;
		return -1;
	}
	else
	{
		//先删除并初始化目录项所指的磁盘块
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
			{//初始化要删掉的文件夹中文件控制块
				if ((pFileItem + i)->FreeOrNot == 0) continue;
				else
				{
					FileBase = pFileItem;
					DelBlankDir(i);
				}
			}//for
			BlockNo = *(DiskBase + ParentBlock);//指向相连的另一磁盘块
			*(DiskBase + ParentBlock) = 0;
		} while (BlockNo != -1);
		//接着删除目录项
		(tempPtr + FileItem)->FileName[0] = '\0';
		(tempPtr + FileItem)->FileOrNot = '0';
		(tempPtr + FileItem)->FileSize = 0;
		(tempPtr + FileItem)->FirstBlock = 0;
		(tempPtr + FileItem)->FreeOrNot = 0;

		cout << "\n文件夹 " << SonDirName << "  删除成功！" << endl;
		return 0;
	}
}

void ls()
{
	FCB* pFileItem;
	int BlockNo, ParentBlock;
	int SumFile, SumDir;
	SumFile = SumDir = 0;
	//搜索当前目录
	BlockNo = CurrentDir;
	pFileItem = GetAddr(BlockNo);
	cout << "*******************当前目录有下列内容********************" << endl;
	do
	{
		ParentBlock = BlockNo;
		pFileItem = GetAddr(BlockNo);
		for (int i = 0;i < MaxFileItems;i++)
		{
			if ((pFileItem + i)->FreeOrNot && (pFileItem + i)->user == user)//是否存在文件或文件夹
			{
				if ((pFileItem + i)->FileOrNot == '1')//是文件
				{
					cout << (pFileItem + i)->FileDate << "	文本文件 " << (pFileItem + i)->FileName << "	大小" << (pFileItem + i)->FileSize << "字节" << endl;
					SumFile++;
				}
				else if ((pFileItem + i)->FileOrNot == '0')
				{
					if (strcmp((pFileItem + i)->FileName, "..") == 0 || strcmp((pFileItem + i)->FileName, ".") == 0)
						continue;
					cout << (pFileItem + i)->FileDate << "	文件夹	" << (pFileItem + i)->FileName << endl;
					SumDir++;
				}
			}
		}//for
		BlockNo = *(DiskBase + ParentBlock);//指向相连的另一磁盘块
	} while (BlockNo != -1);
	cout << "\n在该目录下共有" << SumFile << " 个文本文件, " << SumDir << " 个文件夹\n\n";
	cout << "*********************************************************" << endl;
	return;
}

int cd(char* SonDirName)
{
	int BlockNo, ParentBlock;
	int FileItem, i;
	if (strcmp(SonDirName, "..") == 0)
	{/*回到父目录*/
		if (CurrentDir == TableBlocks)//若当前目录是根目录
		{
			printf("你已经在根目录下了!\n");
			return 0;
		}
		BlockNo = CurrentDir;
		FileBase = GetAddr(BlockNo);
		FileItem = SearchFolder("..");
		ParentBlock = (FileBase + FileItem)->FirstBlock;//获取上级目录磁盘块
		CurrentDir = ParentBlock;
		//在上级目录中查找首块号为BlockNo的文件夹名
		do
		{
			FileItem = -1;
			FileBase = GetAddr(ParentBlock);
			for (i = 0;i < MaxFileItems;i++)
			{
				if ((FileBase + i)->FreeOrNot == 0) continue;
				if ((FileBase + i)->FirstBlock == BlockNo && (FileBase + i)->user == user)
				{
					FileItem = i;//找到了上级目录文件夹
					break;
				}
			}
			ParentBlock = *(DiskBase + ParentBlock);
		} while (ParentBlock != -1);
		/*if(FileItem == -1)
		{//这一句程序是不会运行的因为建立文件夹成功，".."所指的上级目录也肯定正确
		cout<<"找不到上级目录文件夹!"<<endl;
		return -1;
		}*/
		CURRENT = CURRENT.substr(0, CURRENT.length() - strlen((FileBase + FileItem)->FileName) - 1);
		//CURRENT = CURRENT + '>';
		return 0;
	}
	else
	{//如果cd命令后为文件夹
		BlockNo = CurrentDir;
		FileBase = GetAddr(BlockNo);
		FileItem = SearchFolder(SonDirName);
		if (FileItem == -1)
		{
			cout << "文件夹" << SonDirName << "不存在!" << endl;
			return -1;
		}
		BlockNo = (FileBase + FileItem)->FirstBlock;//获取上级目录磁盘块
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
	if (SearchFile(FileName) == -1)//先判断要创建的文件有没有重名
	{
		BlockNo = GetBlock();//获取一个新的磁盘空间,用于存放刚创建文件下的内容
		if (BlockNo == -1)
		{
			cout << "创建文件" << FileName << "失败!" << endl;
			return -1;
		}
		pFileItem = GetAddr(CurrentDir);
		FileItem = GetFileItem(CurrentDir);
		if (FileItem == -1)
		{
			cout << "创建文件" << FileName << "失败!" << endl;
			FreeBlock(BlockNo);
			return -1;
		}
		strcpy((pFileItem + FileItem)->FileName, FileName);
		strcpy((pFileItem + FileItem)->FileDate, GetNowDateTime());
		(pFileItem + FileItem)->FileOrNot = '1';//表文件
		(pFileItem + FileItem)->FileSize = 0;
		(pFileItem + FileItem)->FirstBlock = BlockNo;
		(pFileItem + FileItem)->user = user;
		cout << "\n文件 " << FileName << "  创建成功！" << endl;
		return 0;
	}
	cout << "你要创建的文件已存在!" << endl;
	return -1;
}

int read(char* FileName)
{
	int BlockNo;
	int FileItem;
	int j, i;
	int FileFirstBlock = -1;//用于存放文件的首块数据区
	char* FilePtr;
	FileItem = SearchFile(FileName);
	if (FileItem == -1)
	{
		cout << "你要读取的文件" << "不存在!" << endl;
		return -1;

	}
	//找到了文件并读出文件内容
	BlockNo = (FileBase + FileItem)->FirstBlock;
	FilePtr = (char*)GetAddr(BlockNo);//获取数据块的首部地址
	//输出文件中的内容
	if ((FileBase + FileItem)->FileSize == 0)
	{
		cout << "文件" << FileName << "为空!" << endl;
		return 0;
	}
	cout << "\n********************************文件内容如下**********************************" << endl;
	i = -1;
	for (j = 0;j < (FileBase + FileItem)->FileSize;j++)
	{
		if (i > BlockSize - 2)//本磁盘块读完了,注意：i=511时，本块就用完了
		{//获取下一块磁盘块
			i = 0;
			BlockNo = *(DiskBase + BlockNo);
			if (BlockNo == -1)
			{
				cout << "错误!文件已经读到底了" << endl;
				return -1;
			}
			FilePtr = (char*)GetAddr(BlockNo);
		}
		else i++;

		putchar(*(FilePtr + i));


	}
	cout << "\n********************************文件读取完毕!**********************************" << endl;
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
		cout << "你要编写的文件" << FileName << "不存在!" << endl;
		return -1;

	}
	ClearFileBlock(FileName);//清除文件内容
	//写入文件内容
	strcpy((FileBase + FileItem)->FileDate, GetNowDateTime());
	BlockNo = (FileBase + FileItem)->FirstBlock;
	FilePtr = (char*)GetAddr(BlockNo);//获取文数据块的首部地址
	cout << "请输入文件内容：" << endl;
	getchar();
	ParentBlock = BlockNo;
	while ((input = getchar()) != '$')
	{
		if (j > BlockSize - 2)//本磁盘块完了,注意：i=511时，本块就用完了
		{//申请一块磁盘块
			j = 0;
			BlockNo = GetBlock();
			*(DiskBase + ParentBlock) = BlockNo;
			if (BlockNo == -1)
			{
				cout << "磁盘空间已用完!" << endl;
				cout << "你可以删除一些文件或文件夹后再试！" << endl;
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
		cout << "你要删除的文件" << FileName << "不存在！" << endl;
		return -1;
	}
	else
	{
		//先删除并初始化文件项所指的磁盘块
		BlockNo = (FileBase + FileItem)->FirstBlock;
		do
		{
			ParentBlock = BlockNo;
			pFileItem = GetAddr(BlockNo);
			for (i = 0;i < MaxFileItems;i++)
			{//初始化要删掉的文件中文件控制块
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
			BlockNo = *(DiskBase + ParentBlock);//指向相连的另一磁盘块
			*(DiskBase + ParentBlock) = 0;
		} while (BlockNo != -1);
		//接着删除文件目录项
		(FileBase + FileItem)->FileName[0] = '\0';
		(FileBase + FileItem)->FileOrNot = '0';
		(FileBase + FileItem)->FileSize = 0;
		(FileBase + FileItem)->FirstBlock = 0;
		(FileBase + FileItem)->FreeOrNot = 0;
		cout << "\n文件 " << FileName << "  删除成功！" << endl;
		return 0;
	}
}

int exit()
{
	char QuitOrNot = 'n';
	cout << "你是否要保存文件系统到硬盘并退出? y or n" << endl;
	cin >> QuitOrNot;
	if (QuitOrNot == 'y' || QuitOrNot == 'Y')
	{
		fp = fopen(FileSavePath, "w+");
		if (fp == NULL)
		{
			cout << "打开文件" << FileSavePath << "失败！" << endl;
			cout << "Sorry! -_- 文件系统无法正常退出和保存到硬盘上！" << endl;
			return -1;
		}
		fwrite(DiskBase, sizeof(int), (BlockSize / sizeof(int)) * BlockSum, fp);
		cout << "文件系统保存成功！" << endl;
		fclose(fp);
		free(DiskBase);
		exit(0);
	}
	cout << "你没有保存文件系统到硬盘!" << endl;
	return -1;
}
int SaveFileSystem()
{
	char SaveOrNot = 'n';
	cout << "你是否要保存文件系统到硬盘? y or n" << endl;
	cin >> SaveOrNot;
	if (SaveOrNot == 'y' || SaveOrNot == 'Y')
	{
		fp = fopen(FileSavePath, "w+");
		if (fp == NULL)
		{
			cout << "打开文件" << FileSavePath << "失败！" << endl;
			cout << "Sorry! -_- 文件系统无法正常退出和保存到硬盘上！" << endl;
			return -1;
		}
		fwrite(DiskBase, sizeof(int), (BlockSize / sizeof(int)) * BlockSum, fp);
		cout << "文件系统保存成功！" << endl;
		fclose(fp);
		//free(DiskBase);
		return 0;
	}
	return -1;
}
int main()
{
	CurrentDir = TableBlocks;
	//申请FAT文件系统磁盘并且初始化
	DiskBase = (int*)malloc(BlockSize * BlockSum);
	fp = fopen(FileSavePath, "r");
	if (fp == NULL)
	{
		InitFileSys();
	}
	else
	{
		cout << "加载系统.";
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
			cout << "FileSystem文件读取失败!" << endl;
			fclose(fp);
			exit(0);
		}
		fclose(fp);
	}
	printf("         欢迎使用文件系统!            \n");
	char SaveOrNot = 'n';
	cout << "是否要格式化系统? y or n" << endl;
	cin >> SaveOrNot;
	if (SaveOrNot == 'y' || SaveOrNot == 'Y')
	{
		format();
	}
	cout << endl << endl;
	cout << "请选择用户登陆!: ";
	getchar();
	char in_char;
	std::cin.clear();
	in_char = getchar();
	user = in_char;
	if (in_char == '1' || in_char == '2')
	{
		cout << endl << "加载用户" << in_char << "资料中!" << endl;
	}
	ShowCommandHelp();
	while (1)
	{//开始使用功能
		cout << CURRENT << ">";
		std::cin.clear();//清空缓冲区
		cin >> cmd;
		for (int i = 0;i < cmd.length();i++)
			cmd[i] = tolower(cmd[i]);//忽略大小写
		if (cmd == "format") {
			format();
		}
		else if (cmd == "cursyspath") {
			CurSysPath();
		}
		else if (cmd == "delete")
		{
			cin >> command;
			if (CheckFileName(command) == -1) //文件名不合法
			{
				cout << "输入的文件名太长! 请重试!" << endl;
				continue;
			}
			DelFileSys(command);
		}
		else if (cmd == "mkdir") {
			cin >> command;
			if (CheckFileName(command) == -1) //文件名不合法
			{
				cout << "输入的文件名太长! 请重试!" << endl;
				continue;
			}
			mkdir(command);
		}
		else if (cmd == "rmdir") {
			cin >> command;
			if (CheckFileName(command) == -1) //文件名不合法
			{
				cout << "输入的文件名太长! 请重试!" << endl;
				continue;
			}
			rmdir(command);
		}
		else if (cmd == "ls") {
			ls();
		}
		else if (cmd == "cd") {
			cin >> command;
			if (CheckFileName(command) == -1) //文件名不合法
			{
				cout << "输入的文件名太长! 请重试!" << endl;
				continue;
			}
			cd(command);
		}
		else if (cmd == "create") {
			cin >> command;
			if (CheckFileName(command) == -1) //文件名不合法
			{
				cout << "输入的文件名太长! 请重试!" << endl;
				continue;
			}
			create(command);
		}
		else if (cmd == "saveas") {
			cin >> command;
			if (CheckFileName(command) == -1) //文件名不合法
			{
				cout << "输入的文件名太长! 请重试!" << endl;
				continue;
			}
			//SaveAs(command);
		}
		else if (cmd == "close") {
			cin >> command;
			if (CheckFileName(command) == -1) //文件名不合法
			{
				cout << "输入的文件名太长! 请重试!" << endl;
				continue;
			}
			close(command);
		}
		else if (cmd == "open") {
			cin >> command;
			if (CheckFileName(command) == -1) //文件名不合法
			{
				cout << "输入的文件名太长! 请重试!" << endl;
				continue;
			}
			open(command);
		}
		else if (cmd == "write") {
			cin >> command;
			if (CheckFileName(command) == -1) //文件名不合法
			{
				cout << "输入的文件名太长! 请重试!" << endl;
				continue;
			}
			write(command);
		}
		else if (cmd == "read") {
			cin >> command;
			if (CheckFileName(command) == -1) //文件名不合法
			{
				cout << "输入的文件名太长! 请重试!" << endl;
				continue;
			}
			read(command);
		}
		else if (cmd == "rm") {
			cin >> command;
			if (CheckFileName(command) == -1) //文件名不合法
			{
				cout << "输入的文件名太长! 请重试!" << endl;
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
			std::cin.clear();//清空缓冲区
			cout << "无效指令,请重新输入!" << endl;
			std::cin.clear();//清空缓冲区
		}
	}
	cout << "Thank you for using DaiBin's file system! ^o^\n\n\n" << endl;
	return 0;
}
