#include<stdio.h>
#include "windows.h"

// ��ֵΪ0�������˻���ȫ�ֱ�����
int nAccount1 = 0;
int nAccount2 = 0;

//�����ź�������
HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);

// �������ݵĲ����߳�
// ����param �� �߳�idx
DWORD WINAPI ThreadExecutive(LPVOID param)
{
	int nLoop = 0;//ѭ������
	int nTemp1, nTemp2, nRandom, sum;
	int* pID = (int*)param;

	// ��¼��ʼʱ��
	DWORD time = GetTickCount();

	do
	{
		// �����ź�������
		WaitForSingleObject(hMutex, INFINITE);

		nTemp1 = nAccount1;
		nTemp2 = nAccount2;
		nRandom = rand();
		nAccount1 = nTemp1 + nRandom;
		nAccount2 = nTemp2 - nRandom;
		nLoop++;

		//�����ź����ͷ�
		ReleaseMutex(hMutex);
	} while (nAccount1 + nAccount2 == 0 && nLoop < 1000000);

	printf("�߳�:%d\tѭ������:%d\t����ʱ��:%dms\n", *pID, nLoop, GetTickCount() - time);

	return 0;
}


int main()
{
	printf("Windowsͬ���̲߳�������\n");

	int pID1 = 1, pID2 = 2;

	HANDLE hThread[2];


	if ((hThread[0] = CreateThread(NULL, 0, ThreadExecutive, &pID1, 0, NULL)) == NULL)
	{
		printf("�߳�%d����ʧ�ܣ�\n", pID1);
		exit(0);
	}
	if ((hThread[1] = CreateThread(NULL, 0, ThreadExecutive, &pID2, 0, NULL)) == NULL)
	{
		printf("�߳�%d����ʧ�ܣ�\n", pID2);
		exit(0);
	}

	//���̰߳󶨵�ָ��CPU����
	SetThreadAffinityMask(hThread[0], 0x00000001);
	SetThreadAffinityMask(hThread[1], 0x00000001);

	// �ȴ��߳�
	WaitForMultipleObjects(2, hThread, true, INFINITE);

	//�߳̾���ر�
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

	printf("\n19281300 ������\n");

	system("PAUSE");


	return 0;
}

