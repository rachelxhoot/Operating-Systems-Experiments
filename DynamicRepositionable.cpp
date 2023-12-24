#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#pragma warning(disable: 4996)

int NUM; // ���Ҵ���

//�ɷ����ڴ��еķ����ṹ
typedef struct memory {
	int StartAddress;//������ַ
	int size;//������С
	char state[10];//����״̬(0Ϊ����1Ϊæµ)
	int number;//���̺�
	struct memory* next;
}MEM;
MEM* USER = NULL, * p;

//��ӡ��������Ϣ
void showMEM(MEM* p) {
	printf("%d\t%d\t%s\t%d\t\n", p->StartAddress,p->size,p->state,p->number);
}

//����������ʼλ�ý�������
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

//����
void CompactMEM() {
	printf("*------------------------�����㷨------------------------*\n");
	MEM* front = NULL, * temp = NULL;
	int tmpAdd = 131072;
	//��ʱ��ַ���ڴ�Ž��պ�Ľ��̷�����ַ����ʼΪ�ڴ���ַ ,��������Ϊ131072
	int sumAdd = 0; //��ſ��з��������ܺ�
	while (p != NULL) {
		if (strcmp(p->state, "1") == 0) { //��ǰ����״̬Ϊæµ
			p->StartAddress = tmpAdd;//��ǰ������ַ��Ϊ��ʱ��ַ����
			tmpAdd += p->size; //��ʱ��ַ����+=��ǰ��������
			front = p;
			p = p->next;//����
		}
		else {//��ǰ����Ϊ���з���
			sumAdd += p->size;//������з��������ܺ�
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
	temp = (MEM*)malloc(sizeof(MEM));//��ʼ�� temp ָ���ſ��з������
	temp->StartAddress = tmpAdd; //�¿��з�����ַ=��ʱ��ַ����
	temp->size = sumAdd; //�¿��з�������=���з��������ܺ�
	strcpy(temp->state, "0");//�¿��з���״̬��Ϊ����
	temp->number = -1; //�¿��з������̺���Ϊ-1
	front->next = temp;
	temp->next = NULL; //�¿��з����ڷ����ײ�
	p = USER;
}

//����
MEM* RecycleMEM() {
	printf("*------------------------�ڴ����------------------------*\n");
	int PCBnum;
	printf("�����տռ�Ľ��̺�:");
	scanf("%d", &PCBnum);
	printf("\n");
	MEM* pretail, * tail;
	p = USER;
	pretail = tail = p;
	while (tail != NULL) {
		if (tail->number == PCBnum) {//�ҵ�������
			if (tail->next != NULL) {//�������������������
				if (strcmp(pretail->state, "1") == 0 && strcmp(tail->next->state, "1") == 0) { //���¾����ڽ�
					strcpy(tail->state, "0");//�����ջ�������������״̬��Ϊ����
					tail->number = -1;//�Ľ��̺�Ϊ-1
					break;
				}
				if (strcmp(pretail->state, "0") == 0 &&
					strcmp(tail->next->state, "1") == 0) {//���ڽ�
					pretail->next = tail->next;//������ǰһ������������ϲ�
					pretail->size = tail->size + pretail->size;//��������=ǰһ�������� + ����������
					free(tail);//ɾ��������ָ��
					break;
				}
				if (strcmp(pretail->state, "1") == 0 &&
					strcmp(tail->next->state, "0") == 0) {//���ڽ�
					if (pretail != tail) {
						pretail->next = tail->next;//�����ĺ�һ������������ϲ�
						tail->next->size = tail->next->size + tail->size;//��������= ��һ�������� + ����������
						tail->next->StartAddress = tail->StartAddress;
						free(tail);//ɾ��������ָ��
						break;
					}
					else {
						p = tail->next;
						tail->next->StartAddress = 0;
						tail->next->size += tail->size;
						break;
					}
				}
				if (strcmp(pretail->state, "0") == 0 && strcmp(tail->next->state, "0") == 0) {//���¾��ڽ�
					pretail->next = tail->next->next;//������ǰ������������������ϲ�
					pretail->size = pretail->size + tail->size + tail->next->size;// �������� = ���������ܺ�
					free(tail->next);//ɾ�������ĺ�һ����ָ��
					free(tail);//ɾ��������ָ��
					break;
				}
			}
			else {//��������������
				if (strcmp(pretail->state, "1") == 0) {//������ǰһ����״̬Ϊæµ
					strcpy(tail->state, "0");//������״̬��Ϊ����
					break;
				}
				else {
					pretail->next = NULL;//�ϲ���������
					pretail->size = pretail->size + tail->size;//��������=ǰһ�������� + ����������
					free(tail);//ɾ��������ָ��
					break;
				}
			}
		}
		pretail = tail;
		tail = tail->next;//����
	}
	return p;
}


//�����ڴ�ռ䲼�ֳ�ʼ��
void InitMemory() {
	int n;
	printf("*------------------------�����ڴ�ռ䲼�ֳ�ʼ��------------------------*\n");
	printf("���������������");
	scanf("%d", &n);
	printf("\n����ʵ��Ҫ�󣬸�������ַ��λΪKB���Ҿ�λ��131072KB~524277KB֮�䣡\n");
	for (int i = 1;i <= n;i++) {
		int address, size1, number1;
		p = (MEM*)malloc(sizeof(MEM));
		printf("��%d ��������ʼ��ַ��", i);
		scanf("%d", &address);
		p->StartAddress = address;
		printf("��%d ��������С��", i);
		scanf("%d", &size1);
		p->size = size1;
		printf("��%d ��������״̬(0Ϊ����,1Ϊæµ):", i);
		scanf("%s", p->state);
		if (strcmp(p->state, "1") == 0) {//�жϷ���״̬�Ƿ���
			printf("��%i �������Ľ��̺ţ�", i);
			scanf("%d", &number1);
			p->number = number1;
		}
		else p->number = -1;//�������򶨽��̺�Ϊ-1
		printf("\n");
		p->next = NULL;
		SortMEM(); //��������ʼ��ַ��������
	}
	p = USER;
	printf("\n ʼַ\t ��С\t ״̬\t ���̺� \n");
	while (p != NULL) {
		showMEM(p);
		p = p->next;//��������
	}
}

//�״���Ӧ�㷨
MEM* SCSF()
{
	int PCBnum, PCBsize;
	printf("*------------------------�״���Ӧ�㷨------------------------*\n");
	printf("��Ҫ���еĽ��̺�����̴�С:");
	scanf("%d %d", &PCBnum, &PCBsize);
	printf("\n");
	MEM* pretail, * tail, * q;
	p = USER;
	pretail = tail = p;
	while (tail != NULL) {
		NUM++;
		if (strcmp(tail->state, "0") == 0 && tail->size > PCBsize) {//���������ҳ��ȴ��ڽ��̳��ȣ�����������Ϊ������ڴ�
				if (tail->size > PCBsize) {
					q = (MEM*)malloc(sizeof(MEM));
					q->StartAddress = tail->StartAddress;//���̷�����ַΪ���з�����ַ
						q->size = PCBsize;// ���̷�������Ϊ���̳�
					strcpy(q->state, "1");//�޸Ľ��̷���״̬Ϊæµ
					q->number = PCBnum;//���̷�����Ϊ���̺�
					q->next = tail;//���̷���ָ����з���
					pretail->next = q;//�� pretail ָ��ǰ���̷���
					tail->StartAddress += PCBsize;//���з�����ַ+=���̳���
					tail->size = tail->size - PCBsize;//���з�������-=���̳���
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
			tail = tail->next;//����
		}
	}
	//���������������
	if (tail == NULL) {
		CompactMEM();//����
		SCSF();//�ٴ��״���Ӧ
	}
	return p;
}



int main() {
	InitMemory();//��ʼ�������ڴ���
	
	while (1) {
		int x;
		printf("*------------------------19281300 ������------------------------*\n");
		printf("1:����ռ䣨���״���Ӧ���ٽ��գ�\n2:����ռ䣨�Ƚ��գ����״���Ӧ��\n3:���տռ�\n4:��ǰ����\n5:�˳�\n��ѡ��: ");
		scanf("%d", &x);
		int n = 0, m = 0;
		if (x == 3) break;//ѡ��3����
		switch (x) {
		case 1: //ѡ��1
			p = SCSF();//�״���Ӧ�㷨
			printf("\n\n ʼַ\t ��С\t ״̬\t ���̺� \n");
			while (p != NULL) {
				showMEM(p);//���������Ϣ
				p = p->next;//����
			}
			break;
		case 2: //ѡ��2
			CompactMEM();//����
			p=SCSF();//�ٴ��״���Ӧ
			printf("\n\n ʼַ\t ��С\t ״̬\t ���̺� \n");
			while (p != NULL) {
				showMEM(p);//���������Ϣ
				p = p->next;//����
			}
			printf("���Ҵ���:%d\n", NUM);
			NUM = 0;
			break;
		case 3://ѡ�� 2
			p = RecycleMEM();//�����ڴ���� �㷨
			printf("\n\n ʼַ\t ��С\t ״̬\t ���̺� \n");
			while (p != NULL) {
				showMEM(p);//���������Ϣ
				p = p->next;//����
			}
			printf("���Ҵ���:%d\n", NUM);
			NUM = 0;
			break;
		case 4:
			p = USER;
			while (p != NULL) {
				n += ( p->state[0] - '0') * p->size;
				m += p->size;
				p = p->next;
			}
			printf("�ռ�������:%.2f\n", n / m);
			break;
		default:
			printf("����������������롣\n\n");
			break;
		}
	}
	return 0;
}
