#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#pragma warning(disable: 4996)

int NUM; // 查找次数

//可分配内存中的分区结构
typedef struct memory {
	int StartAddress;//分区首址
	int size;//分区大小
	char state[10];//分区状态(0为挂起、1为忙碌)
	int number;//进程号
	struct memory* next;
}MEM;
MEM* USER = NULL, * p;

//打印各分区信息
void showMEM(MEM* p) {
	printf("%d\t%d\t%s\t%d\t\n", p->StartAddress,p->size,p->state,p->number);
}

//按分区的起始位置进行排序
void SortMEM()
{
	MEM* first, * second;
	int insert = 0;
	if ((USER == NULL) || ((p->StartAddress) < (USER->StartAddress))) {
		p->next = USER;
		USER = p;
	}
	else {
		first = USER;
		second = first->next;
		while (second != NULL) {
			if (p->StartAddress < second->StartAddress) {
				p->next = second;
				first->next = p;
				insert = 1;
				break;
			}
			else {
				first = first->next;
				second = second->next;
			}
		}
		if (insert == 0) {
			first->next = p;
			p->next = NULL;
		}
	}
}

//紧凑
void CompactMEM() {
	printf("*------------------------紧凑算法------------------------*\n");
	MEM* front = NULL, * temp = NULL;
	int tmpAdd = 131072;
	//临时地址用于存放紧凑后的进程分区首址，初始为内存首址 ,本程序中为131072
	int sumAdd = 0; //存放空闲分区长度总和
	while (p != NULL) {
		if (strcmp(p->state, "1") == 0) { //当前分区状态为忙碌
			p->StartAddress = tmpAdd;//当前分区首址置为临时地址变量
			tmpAdd += p->size; //临时地址变量+=当前分区长度
			front = p;
			p = p->next;//后移
		}
		else {//当前分区为空闲分区
			sumAdd += p->size;//计算空闲分区长度总和
			if (front != NULL) {
				front->next = p->next;
			}
			else {
				USER = p->next;
			}
			temp = p;
			p = p->next;
			free(temp);
		}
	}
	temp = (MEM*)malloc(sizeof(MEM));//初始化 temp 指针存放空闲分区情况
	temp->StartAddress = tmpAdd; //新空闲分区首址=临时地址变量
	temp->size = sumAdd; //新空闲分区长度=空闲分区长度总和
	strcpy(temp->state, "0");//新空闲分区状态置为空闲
	temp->number = -1; //新空闲分区进程号置为-1
	front->next = temp;
	temp->next = NULL; //新空闲分区在分区底部
	p = USER;
}

//回收
MEM* RecycleMEM() {
	printf("*------------------------内存回收------------------------*\n");
	int PCBnum;
	printf("被回收空间的进程号:");
	scanf("%d", &PCBnum);
	printf("\n");
	MEM* pretail, * tail;
	p = USER;
	pretail = tail = p;
	while (tail != NULL) {
		if (tail->number == PCBnum) {//找到分区号
			if (tail->next != NULL) {//如果回收区不是最后分区
				if (strcmp(pretail->state, "1") == 0 && strcmp(tail->next->state, "1") == 0) { //上下均不邻接
					strcpy(tail->state, "0");//仅回收回收区，回收区状态置为空闲
					tail->number = -1;//改进程号为-1
					break;
				}
				if (strcmp(pretail->state, "0") == 0 &&
					strcmp(tail->next->state, "1") == 0) {//上邻接
					pretail->next = tail->next;//插入点的前一分区与回收区合并
					pretail->size = tail->size + pretail->size;//分区长度=前一分区长度 + 回收区长度
					free(tail);//删除回收区指针
					break;
				}
				if (strcmp(pretail->state, "1") == 0 &&
					strcmp(tail->next->state, "0") == 0) {//下邻接
					if (pretail != tail) {
						pretail->next = tail->next;//插入点的后一分区与回收区合并
						tail->next->size = tail->next->size + tail->size;//分区长度= 后一分区长度 + 回收区长度
						tail->next->StartAddress = tail->StartAddress;
						free(tail);//删除回收区指针
						break;
					}
					else {
						p = tail->next;
						tail->next->StartAddress = 0;
						tail->next->size += tail->size;
						break;
					}
				}
				if (strcmp(pretail->state, "0") == 0 && strcmp(tail->next->state, "0") == 0) {//上下均邻接
					pretail->next = tail->next->next;//插入点的前后两个分区与回收区合并
					pretail->size = pretail->size + tail->size + tail->next->size;// 分区长度 = 三区长度总和
					free(tail->next);//删除插入点的后一分区指针
					free(tail);//删除回收区指针
					break;
				}
			}
			else {//回收区是最后分区
				if (strcmp(pretail->state, "1") == 0) {//插入点的前一分区状态为忙碌
					strcpy(tail->state, "0");//回收区状态置为空闲
					break;
				}
				else {
					pretail->next = NULL;//合并两个分区
					pretail->size = pretail->size + tail->size;//分区长度=前一分区长度 + 回收区长度
					free(tail);//删除回收区指针
					break;
				}
			}
		}
		pretail = tail;
		tail = tail->next;//后移
	}
	return p;
}


//物理内存空间布局初始化
void InitMemory() {
	int n;
	printf("*------------------------物理内存空间布局初始化------------------------*\n");
	printf("请输入分区个数：");
	scanf("%d", &n);
	printf("\n根据实验要求，各分区地址单位为KB，且均位于131072KB~524277KB之间！\n");
	for (int i = 1;i <= n;i++) {
		int address, size1, number1;
		p = (MEM*)malloc(sizeof(MEM));
		printf("第%d 个分区起始地址：", i);
		scanf("%d", &address);
		p->StartAddress = address;
		printf("第%d 个分区大小：", i);
		scanf("%d", &size1);
		p->size = size1;
		printf("第%d 个分区的状态(0为空闲,1为忙碌):", i);
		scanf("%s", p->state);
		if (strcmp(p->state, "1") == 0) {//判断分区状态是否闲
			printf("第%i 个分区的进程号：", i);
			scanf("%d", &number1);
			p->number = number1;
		}
		else p->number = -1;//若空闲则定进程号为-1
		printf("\n");
		p->next = NULL;
		SortMEM(); //按进程起始地址递增排序
	}
	p = USER;
	printf("\n 始址\t 大小\t 状态\t 进程号 \n");
	while (p != NULL) {
		showMEM(p);
		p = p->next;//继续遍历
	}
}

//首次适应算法
MEM* SCSF()
{
	int PCBnum, PCBsize;
	printf("*------------------------首次适应算法------------------------*\n");
	printf("需要运行的进程号与进程大小:");
	scanf("%d %d", &PCBnum, &PCBsize);
	printf("\n");
	MEM* pretail, * tail, * q;
	p = USER;
	pretail = tail = p;
	while (tail != NULL) {
		NUM++;
		if (strcmp(tail->state, "0") == 0 && tail->size > PCBsize) {//分区空闲且长度大于进程长度，符合条件则为其分配内存
				if (tail->size > PCBsize) {
					q = (MEM*)malloc(sizeof(MEM));
					q->StartAddress = tail->StartAddress;//进程分区首址为空闲分区首址
						q->size = PCBsize;// 进程分区长度为进程长
					strcpy(q->state, "1");//修改进程分区状态为忙碌
					q->number = PCBnum;//进程分区号为进程号
					q->next = tail;//进程分区指向空闲分区
					pretail->next = q;//令 pretail 指向当前进程分区
					tail->StartAddress += PCBsize;//空闲分区首址+=进程长度
					tail->size = tail->size - PCBsize;//空闲分区长度-=进程长度
					break;
				}
			if (tail->size == PCBsize) {
				tail->number = PCBnum;
				strcpy(tail->state, "1");
				break;
			}
		}
		else {
			pretail = tail;
			tail = tail->next;//后移
		}
	}
	//不符合条件则紧凑
	if (tail == NULL) {
		CompactMEM();//紧凑
		SCSF();//再次首次适应
	}
	return p;
}



int main() {
	InitMemory();//初始化物理内存区
	
	while (1) {
		int x;
		printf("*------------------------19281300 赵淑婷------------------------*\n");
		printf("1:分配空间（先首次适应，再紧凑）\n2:分配空间（先紧凑，再首次适应）\n3:回收空间\n4:当前性能\n5:退出\n请选择: ");
		scanf("%d", &x);
		int n = 0, m = 0;
		if (x == 3) break;//选择3结束
		switch (x) {
		case 1: //选择1
			p = SCSF();//首次适应算法
			printf("\n\n 始址\t 大小\t 状态\t 进程号 \n");
			while (p != NULL) {
				showMEM(p);//输出分区信息
				p = p->next;//后移
			}
			break;
		case 2: //选择2
			CompactMEM();//紧凑
			p=SCSF();//再次首次适应
			printf("\n\n 始址\t 大小\t 状态\t 进程号 \n");
			while (p != NULL) {
				showMEM(p);//输出分区信息
				p = p->next;//后移
			}
			printf("查找次数:%d\n", NUM);
			NUM = 0;
			break;
		case 3://选择 2
			p = RecycleMEM();//调用内存回收 算法
			printf("\n\n 始址\t 大小\t 状态\t 进程号 \n");
			while (p != NULL) {
				showMEM(p);//输出分区信息
				p = p->next;//后移
			}
			printf("查找次数:%d\n", NUM);
			NUM = 0;
			break;
		case 4:
			p = USER;
			while (p != NULL) {
				n += ( p->state[0] - '0') * p->size;
				m += p->size;
				p = p->next;
			}
			printf("空间利用率:%.2f\n", n / m);
			break;
		default:
			printf("输入错误！请重新输入。\n\n");
			break;
		}
	}
	return 0;
}
