#define MAX 20 //ҳ����������20

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h> 
#include <string>

using namespace std;

//����������г�ʼ��ȫ�ֱ���
int* Page;//ҳ������ָ��
float N_Page;//�߼���ַ�ռ��е�ҳ������

int read_write_type[MAX];//ҳ���д��ʽ

int const num_C = 2;//���̷��䵽���������
int num = 0;
int lost = 0;

int state_flag[num_C][2];//[][0]����־λ [S][1]���޸�λ

//�����̭�㷨
void OPT(int* Page, int Num, int* C)
{
	int j, c_1, o;//�������������
	int i, p_1;//ҳ����������
	int flag, flag2;//ȱҳ��־λ
	int count = 0;//ȱҳ������¼
	int num = 0;//�ж�������Ƿ�����
	int index, rmax;

	cout << "*-------------------------------------------------*" << endl;
	for (int i = 0; i < num_C; i++)
		printf("�����%d ���ʷ�ʽ    ", i);
	for (i = 0; i <= Num; i++)
	{
		flag = 0;//��ʼ����־λ
		if ((rand() / (RAND_MAX + 1.0)) > 0.7 && read_write_type[i] == 1)//���ö�д״̬
			read_write_type[i] = 1;
		else
			read_write_type[i] = 0;
		for (j = 0; j < num; j++)
		{
			for (int k = 0; k < Num; k++)
			{
				if (C[j] == Page[k])
					o = read_write_type[k];
			}
			if (C[j] == Page[i])
				flag = 1;
			printf("      %d   %d      ", C[j], o);
		}
		printf("\n");
		if (flag == 1)//���Ѿ����ڴ���
			printf("                   %d�Ѿ����ڴ���\n", Page[i]);
		if (flag == 0 && num < num_C)//������ȱҳ
		{
			C[num] = Page[i];
			num++;
			continue;
		}
		if (flag == 0 && num == num_C)
		{
			rmax = 0;
			index = 0;
			for (c_1 = 0; c_1 < num_C; c_1++)
			{
				flag2 = 0;
				for (p_1 = i; p_1 < Num; p_1++)
				{
					if (Page[p_1] == C[c_1])
						break;
				}
				if (p_1 > rmax)
				{
					rmax = p_1;
					index = c_1;
				}
			}
			if (i < N_Page)
			{
				printf("                   %dȱҳ�ж� %d����                   \n", Page[i], C[index]);
				count++;
			}
			C[index] = Page[i];
		}
	}
	printf("OPTȱҳ��:%.2f%%\n", count / N_Page * 100);
	cout << "*-------------------------------------------------*" << endl;
}

//�Ƚ��ȳ���̭�㷨
void FIFO(int* Page, int Num, int* C)
{
	int ccount = 0;	//ȱҳ�жϴ���
	int j;//���������
	int i;//ҳ������
	int flag;//ȱҳ������־λ
	int index = 0,num = 0,o;

	cout << "*-------------------------------------------------*" << endl;
	for (int i = 0; i < num_C; i++)
		printf("�����%d ���ʷ�ʽ    ", i);
	for (i = 0; i <= Num; i++)
	{
		flag = 0;
		if ((rand() / (RAND_MAX + 1.0)) > 0.7 && read_write_type[i] == 1)
			read_write_type[i] = 1;
		else
			read_write_type[i] = 0;
		for (j = 0; j < num; j++)
		{
			for (int k = 0; k < N_Page; k++)
			{
				if (C[j] == Page[k])
					o = read_write_type[k];
			}
			printf("      %d   %d      ", C[j], o);
			if (C[j] == Page[i])
				flag = 1;
		}
		printf("\n");
		if (flag == 1)//���Ѿ����ڴ���
			printf("                   %d�Ѿ����ڴ���\n", Page[i]);
		if (flag == 0 && num == num_C)//������ȱҳ
		{
			ccount++;
			if (i < N_Page)
				printf("                   %dȱҳ�ж� %d����                   \n", Page[i], C[index]);
			C[index] = Page[i];
			index++;
			index = index % num_C;
		}
		if (flag == 0 && num < num_C)
		{
			ccount++;
			C[num] = Page[i];
			num++;
		}
	}
	printf("FIFOȱҳ��:%.2f%%\n", (ccount - num_C - 1) / N_Page * 100);
	cout << "*-------------------------------------------------*" << endl;
}

//������δʹ��
void LRU(int* Page, int Num, int* C)
{
	int c;//���������
	int p;//ҳ������
	int i, flag;//ȱҳ���־λ
	int ccount = 0;	//ȱҳ�жϴ���

	int num = 0, * temp, o;
	temp = (int*)malloc(sizeof(int) * num_C);
	cout << "*-------------------------------------------------*" << endl;
	for (int i = 0; i < num_C; i++)
		printf("�����%d ���ʷ�ʽ    ", i);
	for (p = 0; p <= Num; p++)
	{
		if ((rand() / (RAND_MAX + 1.0)) > 0.7 && read_write_type[p] == 1)
			read_write_type[p] = 1;
		else
			read_write_type[p] = 0;
		flag = 0;
		for (c = 0; c < num; c++)
		{
			for (int j = 0; j < N_Page; j++)
			{
				if (C[c] == Page[j])
				{
					o = read_write_type[j];
				}
			}
			printf("      %d   %d      ", C[c], o);
			if (Page[p] == C[c])
			{
				flag = 1;
				break;
			}
		}
		printf("\n");
		if (flag == 1)
		{
			for (i = 0; i < num; i++)
				temp[i] ++;
			temp[c] = 0;
			printf("                   %d�����ڴ�\n", Page[p]);
		}
		if (num == num_C && flag == 0)
		{
			ccount++;
			int max = -1, max_i;
			for (i = 0; i < num_C; i++)
			{
				if (temp[i] > max)
				{
					max_i = i;
					max = temp[i];
				}
				temp[i] ++;
			}
			if (p < N_Page)
				printf("                   %dȱҳ�ж� %d����                   \n", Page[p], C[max_i]);
			C[max_i] = Page[p];
			temp[max_i] = 0;
		}
		if (num < num_C && flag == 0)
		{
			ccount++;
			for (i = 0; i < num; i++)
			{
				temp[i] ++;
			}
			C[num] = Page[p];
			temp[num] = 0;
			num++;
		}
	}
	printf("LRUȱҳ��:%.2f%%\n", (ccount - num_C - 1) / N_Page * 100);
	cout << "*-------------------------------------------------*" << endl;
}

//��Clock��̭�㷨
void CLOCK(int* Page, int Num, int* C)

{
	int flag[MAX] = { 0 };//���ʱ�־λ
	int i,j=0;//ҳ�桢���������
	int count = 0;//ȱҳ��
	int o, k;

	cout << "*-------------------------------------------------*" << endl;
	for (int i = 0; i < num_C; i++)
		printf("�����%d  ����λ  ���ʷ�ʽ     ", i);
	printf("\n");
	for (i = 0; i < Num; i++)
	{
		if ((rand() / (RAND_MAX + 1.0)) > 0.7 && read_write_type[i] == 1)
			read_write_type[i] = 1;
		else
			read_write_type[i] = 0;
		while (1)
		{
			if ((flag[j] == 0) && (C[j] != Page[i]))
			{
				if (i >= num_C)
					printf("                                      %dȱҳ�жϣ�%d����                   \n", Page[i], C[j]);
				flag[j] = 1;
				C[j] = Page[i];
				count++;
				for (int k = 0; k < num_C; k++)
				{
					if (i == 0)
					{
						C[1] = 0, C[2] = 0;
						for (int h = 0; h < N_Page; h++)
						{
							if (C[k] == Page[h])
							{
								o = read_write_type[h];
							}
						}
						printf("      %d    %d    %d            ", C[k], flag[k], o);
					}
					else if (i == 1)
					{
						C[2] = 0;
						for (int h = 0; h < N_Page; h++)
						{
							if (C[k] == Page[h])
							{
								o = read_write_type[h];
							}
						}
						printf("      %d    %d    %d            ", C[k], flag[k], o);
					}
					else
					{
						for (int h = 0; h < N_Page; h++)
						{
							if (C[k] == Page[h])
							{
								o = read_write_type[h];
							}
						}
						printf("      %d    %d    %d            ", C[k], flag[k], o);
					}
				}
				printf("\n");
				j++;
				if (j - num_C == 0)
					j = 0;
				break;
			}
			if (flag[j] == 1 && C[j] != Page[i])
			{
				flag[j] = 0;
				j++;
				if (j - num_C == 0)
					j = 0;
				continue;
			}
			if (flag[j] == 1 && C[j] == Page[i])
			{
				flag[j] = 0;
				printf("                                      %d�����ڴ�\n", Page[i]);
				for (int k = 0; k < num_C; k++)
					printf("      %d    %d    %d            ", C[k], flag[k], o);
				printf("\n");
				j++;
				if (j - num_C == 0)
					j = 0;
				break;
			}
			if (flag[j] == 0 && C[j] == Page[i])
			{
				flag[j] = 1;
				printf("                                      %d�����ڴ�\n", Page[i]);
				for (int k = 0; k < num_C; k++)
				{
					for (int h = 0; h < N_Page; h++)
					{
						if (C[k] == Page[h])
						{
							o = read_write_type[h];
						}
					}
					printf("      %d    %d    %d            ", C[k], flag[k], o);
				}

				printf("\n");
				j++;
				if (j - num_C == 0)
					j = 0;
				break;
			}
		}
	}
	printf("CLOCKȱҳ��:%.2f%%\n", (float)(count - num_C) / N_Page * 100);
	cout << "*-------------------------------------------------*" << endl;
}

int SelectPage()
{
	int j;
	for (j = 0; j < num_C; j++)
	{
		if (state_flag[j][0] == 0 && state_flag[j][1] == 0)
			return j;
	}
	for (j = 0; j < num_C; j++)
	{
		if (state_flag[j][0] == 0 && state_flag[j][1] == 1)
			return j;
		state_flag[j][0] = 0;
	}
	for (j = 0; j < num_C; j++)
		state_flag[j][0] = 0;
	return SelectPage();
}

//�Ľ���CLOCK
void Modified_Clock(int* Page, int num, int* M)
{
	int j, o, flag = 0;
	for (int i = 0; i < num_C; i++)
	{
		if (M[i] == Page[num])
		{
			if ((rand() % 2 + 1) == 1)
			{
				state_flag[i][0] = 1;
				state_flag[i][1] = 1;
			}
			else
				state_flag[i][0] = 1;
			flag = 1;
			break;
		}
	}
	if (flag == 1)
	{
		printf("                                      %d�����ڴ�\n", Page[num]);
		lost++;
		for (int i = 0; i < num_C; i++)
		{
			for (int h = 0; h < N_Page; h++)
			{
				if (M[i] == Page[h])
					o = read_write_type[h];
			}
			printf("    %d      %d      %d      %d          ", M[i], state_flag[i][0], state_flag[i][1], o);
		}
		printf("\n");
	}
	else
		if (num == num_C)
		{
			j = SelectPage();
			if (num >= num_C)
				printf("                                      %dȱҳ�жϣ�%d����                   \n", Page[num], M[j]);
			M[j] = Page[num];
			state_flag[j][0] = 1;
			for (int i = 0; i < num_C; i++)
			{
				for (int h = 0; h < N_Page; h++)
				{
					if (M[i] == Page[h])
						o = read_write_type[h];
				}
				printf("    %d      %d      %d      %d          ", M[i], state_flag[i][0], state_flag[i][1], o);
			}
			printf("\n");
		}
		else
		{
			M[num] = Page[num];
			num++;
			for (int i = 0; i < num_C; i++)
			{
				for (int h = 0; h < N_Page; h++)
				{
					if (M[i] == Page[h])
						o = read_write_type[h];
				}
				printf("    %d      %d      %d      %d          ", M[i], state_flag[i][0], state_flag[i][1], o);
			}
			printf("\n");
		}
}

void new_CLOCK(int* Page, int R, int* M) {
	lost = 0, num = 0;
	for (int m = 0; m < num_C; m++)
	{
		state_flag[m][0] = 0;
		state_flag[m][1] = rand() / (RAND_MAX + 1.0);
	}
	for (int j = 0; j < num_C; j++)
		M[j] = 0;
	cout << "*-------------------------------------------------*" << endl;
	for (int aa = 0; aa < num_C; aa++)
		printf("�����%d  ����λ  �޸�λ  ���ʷ�ʽ     ", aa);
	printf("\n");
	for (int i = 0; i < N_Page; i++)
		Modified_Clock(Page, i, M);
	printf("�Ľ���Clockȱҳ��:%.2f%%\n", (float)(N_Page - lost - num_C) / N_Page * 100);
	cout << "*-------------------------------------------------*" << endl;
}

int main()
{
	int s, w, v;//��������ʼҳ��s��ҳ��w���������ƶ���v
	float t = 0.8;
	cout << "*--------------------------19281300 ������-----------------------*" << endl;
	printf("*------------------ҳ��������е������������-------------------*\n");
	printf("�߼���ַ�ռ�ҳ������N����߼�ҳ��Ķ�д���ʷ�ʽ����д1����0��:\n");
	scanf_s("%f", &N_Page);
	for (int i = 0; i < N_Page; i++)
		scanf_s("%d", &read_write_type[i]);

	printf("�����빤������ʼҳ�š�������ҳ�����ƶ����ʣ�\n");
	scanf_s("%d", &s);

	scanf_s("%d", &w);

	scanf_s("%d", &v);

	Page = (int*)malloc(sizeof(int) * N_Page);
	int page_size = 0;
	while (page_size < N_Page)//[s, min(s + w, N - 1)] v �������
	{
		for (int i = 0; i < v && page_size < N_Page; i++, page_size++) {
			int temp1 = N_Page - 1, temp2 = s + w;
			if (temp1 > temp2) temp1 = temp2;
			Page[page_size] = rand() % (temp1 - s + 1) + s;
		}

		if (page_size >= N_Page)
			break;
		double r = rand() / (RAND_MAX + 1.0);//�������һ��[0,1]֮���r
		if (r < t)
			s = rand() % ((int)N_Page + 1);
		else
			s = (s + 1) % (int)N_Page;
	}

	int* M;//���������
	M = (int*)malloc(sizeof(int) * num_C);
	int x;

	printf("*------------------��������-------------------*\n");
	for (int i = 0; i < N_Page; i++)
		printf("%d ", Page[i]);
	printf("\n");

	printf("*------------------�����˵�-------------------*\n");
	printf("1.OPT\n2.FIFO\n3.LRU\n4.Clock\n5.�Ľ���Clock\n");

	while (1)
	{
		printf("������ţ����������ַ�������ֹ��:");
		scanf_s("%d", &x);
		switch (x)
		{
		case 1:
			OPT(Page, N_Page, M);
			break;
		case 2:
			FIFO(Page, N_Page, M);
			break;
		case 3:
			LRU(Page, N_Page, M);
			break;
		case 4:
			CLOCK(Page, N_Page, M);
			break;
		case 5:
			new_CLOCK(Page, N_Page, M);
			break;
		default:
			exit(0);
		}
	}
	return 0;
}