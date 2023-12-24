#pragma warning(disable:4996)
#define MAX 20

#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<stdlib.h>

//存储进程的各项信息
typedef struct Zst_PCB {
    char PCBName[20];
    int RunningTime;
    int EnterTime;
    int priority;
    int StartTime;

    struct Zst_PCB* next;
}Zst_PCB;

typedef struct Zst_PCBQueue {
    Zst_PCB* firstPCB;
    Zst_PCB* LastPCB;
    int size;
}Zst_PCBQueue;

//初始化进程队列
void Queueinit(Zst_PCBQueue* queue) {
    if (queue == NULL) {
        return;
    }
    queue->size = 0;
    queue->LastPCB = (Zst_PCB*)malloc(sizeof(Zst_PCB));
    queue->firstPCB = queue->LastPCB;
}

//加入进程队列
void EnterQueue(Zst_PCBQueue* queue, Zst_PCB* pro) {   
    queue->LastPCB->next = (Zst_PCB*)malloc(sizeof(Zst_PCB));
    queue->LastPCB = queue->LastPCB->next;
    queue->LastPCB->EnterTime = pro->EnterTime;
    memcpy(queue->LastPCB->PCBName, pro->PCBName, sizeof(pro->PCBName));
    queue->LastPCB->priority = pro->priority;
    queue->LastPCB->RunningTime = pro->RunningTime;
    queue->LastPCB->StartTime = pro->StartTime;
    queue->size++;
}

//进程出队列
Zst_PCB* poll(Zst_PCBQueue* queue) {
    Zst_PCB* thisPCB = queue->firstPCB->next;
    if (thisPCB == queue->LastPCB) {
        queue->LastPCB = queue->firstPCB;
        queue->size--;
        return thisPCB;
    }
    queue->firstPCB->next = queue->firstPCB->next->next;
    queue->size--;
    return thisPCB;
}

void Zst_PCBinput(Zst_PCB pro[], int num) {
    printf("*----------------------------------19281300----------------------------------*\n");
    for(int i = 0;i < num;i++){
        Zst_PCB thisPCB;
        printf("第%d个进程的信息（名字 到达时间 服务时间 优先级）：", i + 1);
        scanf("%s", thisPCB.PCBName);
        scanf("%d", &thisPCB.EnterTime);
        scanf("%d", &thisPCB.RunningTime);
        scanf("%d", &thisPCB.priority);
        printf("\n");
        pro[i] = thisPCB;
    }
    printf("*----------------------------------19281300----------------------------------*\n");
}

//按照开始时间排序
void sortWithEnterTime(Zst_PCB pro[], int num) {
    for (int i = 1;i < num;i++) {
        for (int j = 0;j < num - i;j++) {
            if (pro[j].EnterTime > pro[j + 1].EnterTime) {
                Zst_PCB temp = pro[j];
                pro[j] = pro[j + 1];
                pro[j + 1] = temp;
            }
        }
    }
}

//按照运行时间排序
void sortWithLongth(Zst_PCB pro[], int start, int end) {
    int len = end - start;
    if (len == 1)
        return;
    for (int i = 1;i < len;i++) {
        for (int j = start;j < end - i;j++) {
            if (pro[j].RunningTime > pro[j + 1].RunningTime) {
                Zst_PCB temp = pro[j];
                pro[j] = pro[j + 1];
                pro[j + 1] = temp;
            }
        }
    }
}

//按优先级排序
void sortWithPriority(Zst_PCB pro[], int start, int end) {
    int len = end - start;
    if (len == 1) return;
    for (int i = 1;i < len;i++) {
        for (int j = start;j < end - i;j++) {
            if (pro[j].priority > pro[j + 1].priority) {
                Zst_PCB temp = pro[j];
                pro[j] = pro[j + 1];
                pro[j + 1] = temp;
            }
        }
    }
}

//先来先服务算法
void FCFS(Zst_PCB pro[], int num) {
    printf("*----------------------------------------------------------------------------*\n");
    printf("进程名字 到达时间  服务时间 开始时间 完成时间 周转时间 带权周转时间\n");
    sortWithEnterTime(pro, num);
    Zst_PCBQueue* queue = (Zst_PCBQueue*)malloc(sizeof(Zst_PCBQueue));
    Queueinit(queue);
    EnterQueue(queue, &pro[0]);
    int curtime = pro[0].EnterTime;
    int curnum = 1;
    float sum_T_time = 0;
    float sum_QT_time = 0;
    while (queue->size > 0) {
        Zst_PCB* curPCB = poll(queue);
        if (curtime < curPCB->EnterTime)
            curtime = curPCB->EnterTime;
        int done_time = curtime + curPCB->RunningTime;
        int T_time = done_time - curPCB->EnterTime;
        sum_T_time += T_time;
        float QT_time = T_time / (curPCB->RunningTime + 0.0);
        sum_QT_time += QT_time;

        for (int tt = curtime;tt <= done_time && curnum < num;tt++) {
            if (tt >= pro[curnum].EnterTime) {
                EnterQueue(queue, &pro[curnum]);
                curnum++;
            }
        }
        printf("%s\t %d\t   %d\t    %d\t     %d\t      %d\t       %.2f\n", curPCB->PCBName, curPCB->EnterTime, curPCB->RunningTime, curtime, done_time
            , T_time, QT_time);
        curtime += curPCB->RunningTime;
        if (queue->size == 0 && curnum < num) {
            EnterQueue(queue, &pro[curnum]);
            curnum++;
        }
    }
    printf("平均周转时间:%.2f\t平均带权周转时间：%.2f\n", sum_T_time / (num + 0.0), sum_QT_time / (num + 0.0));
    printf("*----------------------------------------------------------------------------*\n");
}

//短进程优先算法
void SJF(Zst_PCB pro[], int num) {
    printf("*----------------------------------------------------------------------------*\n");
    printf("进程名字 到达时间  服务时间 开始时间 完成时间 周转时间 带权周转时间\n");
    sortWithEnterTime(pro, num); 
    Zst_PCBQueue* queue = (Zst_PCBQueue*)malloc(sizeof(Zst_PCBQueue));;
    Queueinit(queue);
    EnterQueue(queue, &pro[0]);
    int curtime = pro[0].EnterTime;
    int curnum = 1;
    float sum_T_time = 0, sum_QT_time = 0;
    while (queue->size > 0) {
        Zst_PCB* curpro = poll(queue); 
        if (curtime < curpro->EnterTime)
            curtime = curpro->EnterTime;
        int done_time = curtime + curpro->RunningTime;
        int T_time = done_time - curpro->EnterTime;
        float QT_time = T_time / (curpro->RunningTime + 0.0);
        sum_T_time += T_time;
        sum_QT_time += QT_time;
        int prenum = curnum;
        for (int tt = curtime;tt <= done_time && curnum < num;tt++) {
            if (tt >= pro[curnum].EnterTime) {
                curnum++;
            }
        }
        sortWithLongth(pro, prenum, curnum);
        for (int i = prenum;i < curnum;i++) { 
            EnterQueue(queue, &pro[i]);
        }
        prenum = curnum;
        printf("%s\t %d\t   %d\t    %d\t     %d\t      %d\t       %.2f\n", curpro->PCBName, curpro->EnterTime, curpro->RunningTime, curtime, done_time
            , T_time, QT_time);
        curtime += curpro->RunningTime;
        if (queue->size == 0 && curnum < num) {
            EnterQueue(queue, &pro[curnum]);
            curnum++;
        }
    }
    printf("平均周转时间:%.2f\t平均带权周转时间：%.2f\n", sum_T_time / (num + 0.0), sum_QT_time / (num + 0.0));
    printf("*----------------------------------------------------------------------------*\n");
}

//优先级调度算法
void HPF(Zst_PCB pro[], int num) {
    printf("*----------------------------------------------------------------------------*\n");
    printf("进程名字 到达时间  服务时间 开始时间 完成时间 周转时间 带权周转时间\n");
    sortWithEnterTime(pro, num);
    Zst_PCBQueue* queue = (Zst_PCBQueue*)malloc(sizeof(Zst_PCBQueue));;
    Queueinit(queue);
    EnterQueue(queue, &pro[0]);
    int time = pro[0].EnterTime;
    int pronum = 1;
    float sum_T_time = 0, sum_QT_time = 0;
    while (queue->size > 0) {
        Zst_PCB* curPCB = poll(queue);
        if (time < curPCB->EnterTime)
            time = curPCB->EnterTime;
        int done_time = time + curPCB->RunningTime;
        int T_time = done_time - curPCB->EnterTime;
        float QT_time = T_time / (curPCB->RunningTime + 0.0);
        sum_T_time += T_time;
        sum_QT_time += QT_time;
        int pre = pronum;
        for (int tt = time;tt <= done_time && pronum < num;tt++) {
            if (tt >= pro[pronum].EnterTime) {
                pronum++;
            }
        }
        sortWithPriority(pro, pre, pronum);
        for (int i = pre;i < pronum;i++) {
            EnterQueue(queue, &pro[i]);
        }
        pre = pronum;
        printf("%s\t%d\t%d\t%d\t%d\t%d\t%.2f\n", curPCB->PCBName, curPCB->EnterTime, curPCB->RunningTime, time, done_time
            , T_time, QT_time);
        time += curPCB->RunningTime;
        if (queue->size == 0 && pronum < num) {
            EnterQueue(queue, &pro[pronum]);
            pronum++;
        }
    }
    printf("平均周转时间:%.2f\t平均带权周转时间：%.2f\n", sum_T_time / (num + 0.0), sum_QT_time / (num + 0.0));
    printf("*----------------------------------------------------------------------------*\n");
}

void PrintMenu() {
    printf("请选择进程调度算法：\n\n");
    printf("1.先来先服务算法\n2.短进程优先算法\n3.高优先级优先\n0.退出\n");
}

int main() {
    int pcbNum;
    printf("请输入进程的个数：");
    scanf("%d", &pcbNum);

    Zst_PCB pro[MAX];
    Zst_PCBinput(pro, pcbNum);
    PrintMenu();

    int choice;
    while (1) {
        scanf("%d", &choice);
        switch (choice) {
        case 1:FCFS(pro, pcbNum);PrintMenu();break;
        case 2:SJF(pro, pcbNum);PrintMenu();break;
        case 3:HPF(pro, pcbNum);PrintMenu();break;
        case 5:return 0;
        }
    }
    return 0;
}
