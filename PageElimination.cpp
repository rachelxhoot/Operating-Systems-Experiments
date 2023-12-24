#define MAX 20 //页面数不超过20

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h> 
#include <string>

using namespace std;

//随机访问序列初始化全局变量
int* Page;//页面数组指针
float N_Page;//逻辑地址空间中的页面总数

int read_write_type[MAX];//页面读写方式

int const num_C = 2;//进程分配到的物理块数
int num = 0;
int lost = 0;

int state_flag[num_C][2];//[][0]：标志位 [S][1]：修改位

//最佳淘汰算法
void OPT(int* Page, int Num, int* C)
{
	int j, c_1, o;//物理块索引变量
	int i, p_1;//页面索引变量
	int flag, flag2;//缺页标志位
	int count = 0;//缺页次数记录
	int num = 0;//判断物理块是否已满
	int index, rmax;

	cout << "*-------------------------------------------------*" << endl;
	for (int i = 0; i < num_C; i++)
		printf("物理块%d 访问方式    ", i);
	for (i = 0; i <= Num; i++)
	{
		flag = 0;//初始化标志位
		if ((rand() / (RAND_MAX + 1.0)) > 0.7 && read_write_type[i] == 1)//设置读写状态
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
		if (flag == 1)//若已经在内存中
			printf("                   %d已经在内存中\n", Page[i]);
		if (flag == 0 && num < num_C)//若存在缺页
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
				printf("                   %d缺页中断 %d换出                   \n", Page[i], C[index]);
				count++;
			}
			C[index] = Page[i];
		}
	}
	printf("OPT缺页率:%.2f%%\n", count / N_Page * 100);
	cout << "*-------------------------------------------------*" << endl;
}

//先进先出淘汰算法
void FIFO(int* Page, int Num, int* C)
{
	int ccount = 0;	//缺页中断次数
	int j;//物理块索引
	int i;//页面索引
	int flag;//缺页否放入标志位
	int index = 0,num = 0,o;

	cout << "*-------------------------------------------------*" << endl;
	for (int i = 0; i < num_C; i++)
		printf("物理块%d 访问方式    ", i);
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
		if (flag == 1)//若已经在内存中
			printf("                   %d已经在内存中\n", Page[i]);
		if (flag == 0 && num == num_C)//若存在缺页
		{
			ccount++;
			if (i < N_Page)
				printf("                   %d缺页中断 %d换出                   \n", Page[i], C[index]);
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
	printf("FIFO缺页率:%.2f%%\n", (ccount - num_C - 1) / N_Page * 100);
	cout << "*-------------------------------------------------*" << endl;
}

//最近最久未使用
void LRU(int* Page, int Num, int* C)
{
	int c;//物理块索引
	int p;//页面索引
	int i, flag;//缺页否标志位
	int ccount = 0;	//缺页中断次数

	int num = 0, * temp, o;
	temp = (int*)malloc(sizeof(int) * num_C);
	cout << "*-------------------------------------------------*" << endl;
	for (int i = 0; i < num_C; i++)
		printf("物理块%d 访问方式    ", i);
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
			printf("                   %d已在内存\n", Page[p]);
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
				printf("                   %d缺页中断 %d换出                   \n", Page[p], C[max_i]);
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
	printf("LRU缺页率:%.2f%%\n", (ccount - num_C - 1) / N_Page * 100);
	cout << "*-------------------------------------------------*" << endl;
}

//简单Clock淘汰算法
void CLOCK(int* Page, int Num, int* C)

{
	int flag[MAX] = { 0 };//访问标志位
	int i,j=0;//页面、物理块索引
	int count = 0;//缺页数
	int o, k;

	cout << "*-------------------------------------------------*" << endl;
	for (int i = 0; i < num_C; i++)
		printf("物理块%d  访问位  访问方式     ", i);
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
					printf("                                      %d缺页中断，%d换出                   \n", Page[i], C[j]);
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
				printf("                                      %d已在内存\n", Page[i]);
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
				printf("                                      %d已在内存\n", Page[i]);
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
	printf("CLOCK缺页率:%.2f%%\n", (float)(count - num_C) / N_Page * 100);
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

//改进型CLOCK
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
		printf("                                      %d已在内存\n", Page[num]);
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
				printf("                                      %d缺页中断，%d换出                   \n", Page[num], M[j]);
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
		printf("物理块%d  访问位  修改位  访问方式     ", aa);
	printf("\n");
	for (int i = 0; i < N_Page; i++)
		Modified_Clock(Page, i, M);
	printf("改进型Clock缺页率:%.2f%%\n", (float)(N_Page - lost - num_C) / N_Page * 100);
	cout << "*-------------------------------------------------*" << endl;
}

int main()
{
	int s, w, v;//工作集起始页号s、页数w、工作集移动率v
	float t = 0.8;
	cout << "*--------------------------19281300 赵淑婷-----------------------*" << endl;
	printf("*------------------页面访问序列的随机发生机制-------------------*\n");
	printf("逻辑地址空间页面总数N与各逻辑页面的读写访问方式（读写1，读0）:\n");
	scanf_s("%f", &N_Page);
	for (int i = 0; i < N_Page; i++)
		scanf_s("%d", &read_write_type[i]);

	printf("请输入工作集起始页号、包含的页数、移动速率：\n");
	scanf_s("%d", &s);

	scanf_s("%d", &w);

	scanf_s("%d", &v);

	Page = (int*)malloc(sizeof(int) * N_Page);
	int page_size = 0;
	while (page_size < N_Page)//[s, min(s + w, N - 1)] v 个随机数
	{
		for (int i = 0; i < v && page_size < N_Page; i++, page_size++) {
			int temp1 = N_Page - 1, temp2 = s + w;
			if (temp1 > temp2) temp1 = temp2;
			Page[page_size] = rand() % (temp1 - s + 1) + s;
		}

		if (page_size >= N_Page)
			break;
		double r = rand() / (RAND_MAX + 1.0);//随机产生一个[0,1]之间的r
		if (r < t)
			s = rand() % ((int)N_Page + 1);
		else
			s = (s + 1) % (int)N_Page;
	}

	int* M;//物理块数组
	M = (int*)malloc(sizeof(int) * num_C);
	int x;

	printf("*------------------访问序列-------------------*\n");
	for (int i = 0; i < N_Page; i++)
		printf("%d ", Page[i]);
	printf("\n");

	printf("*------------------操作菜单-------------------*\n");
	printf("1.OPT\n2.FIFO\n3.LRU\n4.Clock\n5.改进型Clock\n");

	while (1)
	{
		printf("操作编号（输入其它字符程序终止）:");
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