#include<stdio.h>
#include "windows.h"

// 初值为0的两个账户（全局变量）
int nAccount1 = 0;
int nAccount2 = 0;

//互斥信号量创建
HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);

// 共享数据的并发线程
// 参数param ： 线程idx
DWORD WINAPI ThreadExecutive(LPVOID param)
{
	int nLoop = 0;//循环次数
	int nTemp1, nTemp2, nRandom, sum;
	int* pID = (int*)param;

	// 记录开始时间
	DWORD time = GetTickCount();

	do
	{
		// 互斥信号量申请
		WaitForSingleObject(hMutex, INFINITE);

		nTemp1 = nAccount1;
		nTemp2 = nAccount2;
		nRandom = rand();
		nAccount1 = nTemp1 + nRandom;
		nAccount2 = nTemp2 - nRandom;
		nLoop++;

		//互斥信号量释放
		ReleaseMutex(hMutex);
	} while (nAccount1 + nAccount2 == 0 && nLoop < 1000000);

	printf("线程:%d\t循环次数:%d\t持续时间:%dms\n", *pID, nLoop, GetTickCount() - time);

	return 0;
}


int main()
{
	printf("Windows同步线程并发运行\n");

	int pID1 = 1, pID2 = 2;

	HANDLE hThread[2];


	if ((hThread[0] = CreateThread(NULL, 0, ThreadExecutive, &pID1, 0, NULL)) == NULL)
	{
		printf("线程%d创建失败！\n", pID1);
		exit(0);
	}
	if ((hThread[1] = CreateThread(NULL, 0, ThreadExecutive, &pID2, 0, NULL)) == NULL)
	{
		printf("线程%d创建失败！\n", pID2);
		exit(0);
	}

	//将线程绑定到指定CPU核心
	SetThreadAffinityMask(hThread[0], 0x00000001);
	SetThreadAffinityMask(hThread[1], 0x00000001);

	// 等待线程
	WaitForMultipleObjects(2, hThread, true, INFINITE);

	//线程句柄关闭
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

	printf("\n19281300 赵淑婷\n");

	system("PAUSE");


	return 0;
}

