#include<stdio.h>
#include "windows.h"

// 初值为0的两个账户（全局变量）
int nAccount1 = 0;
int nAccount2 = 0;

// 共享数据的并发线程
// 参数param ： 线程idx
DWORD WINAPI ThreadExecutive(LPVOID param)
{
	int nLoop = 0;//循环次数

	int nTemp1, nTemp2, nRandom;
	int* pID = (int*)param;

	do
	{	// 两个账户同时增减
		nTemp1 = nAccount1;
		nTemp2 = nAccount2;
		nRandom = rand();
		nAccount1 = nTemp1 + nRandom;
		nAccount2 = nTemp2 - nRandom;
		nLoop++;
	} while (nAccount1 + nAccount2 == 0 && nLoop < 1000000);

	printf("线程:%d\t循环次数:%d\n", *pID, nLoop);

	return 0;
}


int main()
{
	printf("无同步控制的线程并发运行\n");

	// 两个线程的idx
	int pID1 = 1, pID2 = 2;

	HANDLE hThread[2];

	// 创建线程
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

	// 将线程绑定到指定的CPU核心
	SetThreadAffinityMask(hThread[0], 0x00000001);
	SetThreadAffinityMask(hThread[1], 0x00000001);

	// 等待线程执行
	WaitForMultipleObjects(2, hThread, true, INFINITE);

	// 线程句柄关闭
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

	printf("\n19281300 赵淑婷\n");

	system("PAUSE");

	return 0;
}
