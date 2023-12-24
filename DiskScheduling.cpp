#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

const int MaxNumber = 199;//�ŵ������ֵ
int TrackNum=100;//��ʵ���еĴŵ���ĿΪ100
int StartTrack;//��ʼ�ŵ���
int TrackOrder[MaxNumber];//�ŵ������¼�����
int VisitOrder[MaxNumber];//�ŵ�����˳��
bool isVisited[MaxNumber];//�Ƿ񱻷��ʹ�
int MoveDistance[MaxNumber];//�ŵ�����ʼ�ŵ�����
int totalDistance;//�ܾ���
double AverageDistance;
bool direction;

//չʾ
void display()
{
	cout << setw(16) << "��" << StartTrack << "�Ŵŵ���ʼ" << setw(4) << endl;
	cout << setw(2) << "�����ʵĴŵ���˳��   " << setw(6) << "   �Ƶ����루�ŵ�����" << setw(4) << endl;
	for (int i = 0;i < TrackNum;i++)
	{
		cout << setw(10) << VisitOrder[i] << setw(24) << MoveDistance[i] << setw(8) << endl;
	}
	cout << "ƽ��Ѱ������" << AverageDistance << endl;
}

//������ɴŵ���������
void RandomTrackSeq()
{
	cout << "******* ������ɴŵ������¼����� *******" << endl;
	srand(time(NULL)); //���ݵ�ǰʱ�����á���������ӡ�
	for (int i = 0;i < TrackNum;i++)
	{
		TrackOrder[i]= (rand() % (TrackNum + 1)) ;   //������ɴŵ���������
	}
	StartTrack= (rand() % (TrackNum + 1));  //������ɿ�ʼ�ŵ���
	cout << "�ŵ�������" << endl;
	cout << TrackNum << endl;
	cout << "�ŵ��������У�" << endl;
	for (int i = 0;i < TrackNum;i++)
	{
		cout << TrackOrder[i] << " ";
	}
	cout << endl;
	cout << "��ʼ�ŵ��ţ�"<< endl;
	cout << StartTrack << endl;
	cout << "******* ******* ******* ******* *******" << endl;
}

//���������ŵ�֮��ľ���
int getPositive(int num1, int num2)
{
	if (num1 - num2 > 0)
	{
		return (num1 - num2);
	}
	else
		return (num2 - num1);
}

//��ʼ��
void initial()
{
	for (int i = 0;i < TrackNum;i++)
	{
		MoveDistance[i] = 0;
		VisitOrder[i] = TrackOrder[i];
		isVisited[i] = false;
	}
	totalDistance = 0;
	AverageDistance = 0;
}

//�����ȷ�������㷨
void FCFS()
{
	cout << "******* �����ȷ�������㷨 *******" << endl;
	initial();//��ʼ��

	MoveDistance[0] = getPositive(TrackOrder[0], StartTrack);
	totalDistance = MoveDistance[0];
	VisitOrder[0] = TrackOrder[0];

	for (int i = 1;i < TrackNum;i++)
	{
		//��TrackOrder��˳�����
		MoveDistance[i] = getPositive(TrackOrder[i], TrackOrder[i - 1]);
		totalDistance += MoveDistance[i];
		VisitOrder[i] = TrackOrder[i];
	}
	AverageDistance = totalDistance * 1.0 / TrackNum;
	display();
	cout << "******* ******* ******* ******* *******" << endl;
}

//���Ѱ��ʱ�����ȵ����㷨
void SSTF()
{
	cout << "******* ���Ѱ��ʱ������㷨 *******" << endl;
	initial();//��ʼ��

	int CurrentTrack = StartTrack;//��ʼ����ǰ�ŵ���
	int pointMin;//ָ��ŵ�������С�Ĵŵ�λ��
	int disTemp[MaxNumber];//��ǰ�ŵ������ŵ�֮��ľ���

	for (int i = 0;i < TrackNum;i++)
	{
		for (int j = 0;j < TrackNum;j++)
		{
			//ÿ�η���ǰ���µ�ǰ�ŵ������ŵ�֮��ľ���
			if (!isVisited[j])//���ôŵ���δ������
			{
				disTemp[j] = getPositive(TrackOrder[j], CurrentTrack);
			}
			else
				disTemp[j] = 10000;  //��ʾ����Զ�����ʹ��Ĵŵ��Ͳ��ٷ��ʣ�
		}

		pointMin = 0;
		for (int j = 0;j < TrackNum;j++)
		{
			if (disTemp[pointMin] > disTemp[j])
			{
				pointMin = j;   //ָ����С��λ��
			}
		}
		VisitOrder[i] = TrackOrder[pointMin];  //���������и�ֵ
		MoveDistance[i] = getPositive(TrackOrder[pointMin], CurrentTrack);  //����ÿ�ε��ƶ�����
		totalDistance += MoveDistance[i];   //�ۼ��ƶ�����
		CurrentTrack = TrackOrder[pointMin];   //�ı䵱ǰ�Ĵŵ���
		isVisited[pointMin] = true;  //����ǰ�Ĵŵ�������Ϊ�ѷ���

	}

	AverageDistance = totalDistance * 1.0 / (TrackNum);
	display();
	cout << "******* ******* ******* ******* *******" << endl;
}

//���ݵ����㷨
void SCAN()
{
	cout << "******* ���ݣ�ɨ�裩�����㷨 *******" << endl;
	cout << "��ѡ��ͷ�ƶ�����" << endl;
	cout << "0.��ŵ����ӵķ������" << endl;
	cout << "1.��ŵ���С�ķ������" << endl;
	cout << "ѡ��";
	cin >> direction;

	initial();//��ʼ��

	int TrackOrderIndex[MaxNumber];//�����Ĵŵ�������ֵ
	int SortTrackOrder[MaxNumber];//�����Ĵŵ���
	int i, j;
	int temp, tempIndex;//�����м�ֵ

	//��ʼ��
	for (i = 0;i < TrackNum;i++)
	{
		TrackOrderIndex[i] = i;
		SortTrackOrder[i] = TrackOrder[i];
	}

	//�����Ĵŵ����Լ���Ӧ��ԭ��������ֵ
	for (i = TrackNum - 1;i > 0;i--)
	{
		for (j = 0;j < i;j++)
		{
			if (SortTrackOrder[j] >= SortTrackOrder[j + 1])
			{
				//���������Ĵŵ���
				temp = SortTrackOrder[j];
				SortTrackOrder[j] = SortTrackOrder[j + 1];
				SortTrackOrder[j + 1] = temp;

				//���������Ĵŵ�������ֵ
				tempIndex = TrackOrderIndex[j];
				TrackOrderIndex[j] = TrackOrderIndex[j + 1];
				TrackOrderIndex[j + 1] = tempIndex;

			}//if
		}//for
	}//for 

	//�ҵ����ڵ�ǰ�ŵ�֮�⣬���Ǿ�������Ĵŵ�������ֵpoint
	int point = 0;
	while (StartTrack >= SortTrackOrder[point])
	{
		point++;
	}  

	int count = 0;
	int currentTrack = StartTrack;

	//��ŵ����ӵķ������
	if (direction == 0)
	{
		cout << "            ��ŵ����ӵķ������" << endl;
		//���ӷ���
		for (i = point;i < TrackNum;i++)
		{
			VisitOrder[count] = SortTrackOrder[i];
			MoveDistance[count] = getPositive(VisitOrder[count], currentTrack);
			currentTrack = VisitOrder[count];
			count++;
		}
		//���ٷ���
		for (i = point - 1;i >= 0;i--)
		{
			VisitOrder[count] = SortTrackOrder[i];
			MoveDistance[count] = getPositive(VisitOrder[count], currentTrack);
			currentTrack = VisitOrder[count];
			count++;
		}
	}//if

	//��ŵ���С�ķ������
	else if (direction == 1)
	{
		cout << "            ��ŵ���С�ķ������" << endl;
		//��С����
		for (i = point - 1;i >= 0;i--)
		{
			VisitOrder[count] = SortTrackOrder[i];
			MoveDistance[count] = getPositive(VisitOrder[count], currentTrack);
			currentTrack = VisitOrder[count];
			count++;
		}
		//���ӷ���
		for (i = point;i < TrackNum;i++)
		{
			VisitOrder[count] = SortTrackOrder[i];
			MoveDistance[count] = getPositive(VisitOrder[count], currentTrack);
			currentTrack = VisitOrder[count];
			count++;
		}
	}//else

	for (i = 0;i < TrackNum;i++)
	{
		totalDistance += MoveDistance[i];
	}

	AverageDistance = (totalDistance * 1.0) / TrackNum;

	display();
	cout << "******* ******* ******* ******* *******" << endl;
}

//ѭ��ʽ������ݵ����㷨
void CSCAN()
{
	cout << "******* ѭ��ɨ������㷨 *******" << endl;
	cout << "��ѡ��ͷ�ƶ�����" << endl;
	cout << "0.��ŵ����ӵķ������" << endl;
	cout << "1.��ŵ���С�ķ������" << endl;
	cout << "ѡ��";
	cin >> direction;

	initial();
	int TrackOrderIndex[MaxNumber];//�����Ĵŵ�������ֵ
	int SortTrackOrder[MaxNumber];//�����Ĵŵ���
	int i, j;
	int temp, tempIndex;
	for (i = 0;i < TrackNum;i++)
	{
		TrackOrderIndex[i] = i;
		SortTrackOrder[i] = TrackOrder[i];
	}

	//��������Ĵŵ����Լ���Ӧ��ԭ��������ֵ
	for (i = TrackNum - 1;i > 0;i--)
	{
		for (j = 0;j < i;j++)
		{
			if (SortTrackOrder[j] >= SortTrackOrder[j + 1])
			{
				//���������Ĵŵ���
				temp = SortTrackOrder[j];
				SortTrackOrder[j] = SortTrackOrder[j + 1];
				SortTrackOrder[j + 1] = temp;

				//���������Ĵŵ�������ֵ
				tempIndex = TrackOrderIndex[j];
				TrackOrderIndex[j] = TrackOrderIndex[j + 1];
				TrackOrderIndex[j + 1] = tempIndex;

			}//if
		}//for
	}//for

	//�ҵ����ڵ�ǰ�ŵ�֮�⣬���Ǿ�������Ĵŵ�������ֵpoint
	int point = 0;
	while (StartTrack >= SortTrackOrder[point])
	{
		point++;
	}

	int count = 0;
	int currentTrack = StartTrack;
	if (direction == 0)  //��ŵ����ӵķ������
	{
		cout << "            ��ŵ����ӵķ������" << endl;
		for (i = point;i < TrackNum;i++)
		{
			VisitOrder[count] = SortTrackOrder[i];
			MoveDistance[count] = getPositive(VisitOrder[count], currentTrack);
			currentTrack = VisitOrder[count];
			count++;
		}

		for (i = 0;i < point;i++)
		{
			VisitOrder[count] = SortTrackOrder[i];
			MoveDistance[count] = getPositive(VisitOrder[count], currentTrack);
			currentTrack = VisitOrder[count];
			count++;
		}
	}//if
	else if (direction == 1)
	{
		cout << "            ��ŵ���С�ķ������" << endl;

		for (i = 0;i < point;i++)
		{
			VisitOrder[count] = SortTrackOrder[i];
			MoveDistance[count] = getPositive(VisitOrder[count], currentTrack);
			currentTrack = VisitOrder[count];
			count++;
		}

		for (i = point;i < TrackNum;i++)
		{
			VisitOrder[count] = SortTrackOrder[i];
			MoveDistance[count] = getPositive(VisitOrder[count], currentTrack);
			currentTrack = VisitOrder[count];
			count++;
		}
	}//else

	for (i = 0;i < TrackNum;i++)
	{
		totalDistance += MoveDistance[i];
	}

	AverageDistance = (totalDistance * 1.0) / TrackNum;

	display();
	cout << "******* ******* ******* ******* *******" << endl;
}

//˫���е���ɨ������㷨
void FSCAN()
{
	cout << "******* ˫���е���ɨ������㷨 *******" << endl;
	cout << "��ѡ��ͷ�ƶ�����" << endl;
	cout << "0.��ŵ����ӵķ������" << endl;
	cout << "1.��ŵ���С�ķ������" << endl;
	cout << "ѡ��";
	cin >> direction;

	int TrackOrder_1[MaxNumber/2];//����1
	int TrackOrder_2[MaxNumber/2];//����2
	for (int i = 0;i < TrackNum / 2;i++) {
		TrackOrder_1[i] = TrackOrder[i];
	}
	for (int i = 0;i < TrackNum / 2;i++) {
		TrackOrder_2[i] = TrackOrder[i+ TrackNum / 2];
	}
	int VisitOrder_1[MaxNumber/2], VisitOrder_2[MaxNumber / 2];//�ŵ�����˳��
	bool isVisited_1[MaxNumber/2], isVisited_2[MaxNumber / 2];//�Ƿ񱻷��ʹ�
	int MoveDistance_1[MaxNumber/2], MoveDistance_2[MaxNumber / 2];//�ŵ�����ʼ�ŵ�����
	for (int i = 0;i < TrackNum/2;i++)
	{
		MoveDistance_1[i] = 0;
		VisitOrder_1[i] = TrackOrder_1[i];
		isVisited_1[i] = false;
	}
	for (int i = 0;i < TrackNum / 2;i++)
	{
		MoveDistance_2[i] = 0;
		VisitOrder_2[i] = TrackOrder_2[i];
		isVisited_2[i] = false;
	}
	totalDistance = 0;
	AverageDistance = 0;
	//��ԭ���з�Ϊ������

	int TrackOrderIndex_1[MaxNumber], TrackOrderIndex_2[MaxNumber];//�����Ĵŵ�������ֵ
	int SortTrackOrder_1[MaxNumber], SortTrackOrder_2[MaxNumber];//�����Ĵŵ���
	int i, j;
	int temp, tempIndex;
	for (i = 0;i < TrackNum/2;i++)
	{
		TrackOrderIndex_1[i] = i;
		SortTrackOrder_1[i] = TrackOrder_1[i];
	}
	for (i = 0;i < TrackNum / 2;i++)
	{
		TrackOrderIndex_2[i] = i;
		SortTrackOrder_2[i] = TrackOrder_2[i];
	}

	//��������Ĵŵ����Լ���Ӧ��ԭ��������ֵ
	for (i = TrackNum/2 - 1;i > 0;i--)
	{
		for (j = 0;j < i;j++)
		{
			if (SortTrackOrder_1[j] >= SortTrackOrder_1[j + 1])
			{
				//���������Ĵŵ���
				temp = SortTrackOrder_1[j];
				SortTrackOrder_1[j] = SortTrackOrder_1[j + 1];
				SortTrackOrder_1[j + 1] = temp;

				//���������Ĵŵ�������ֵ
				tempIndex = TrackOrderIndex_1[j];
				TrackOrderIndex_1[j] = TrackOrderIndex_1[j + 1];
				TrackOrderIndex_1[j + 1] = tempIndex;

			}//if
		}//for
	}//for

	for (i = TrackNum / 2 - 1;i > 0;i--)
	{
		for (j = 0;j < i;j++)
		{
			if (SortTrackOrder_2[j] >= SortTrackOrder_2[j + 1])
			{
				//���������Ĵŵ���
				temp = SortTrackOrder_2[j];
				SortTrackOrder_2[j] = SortTrackOrder_2[j + 1];
				SortTrackOrder_2[j + 1] = temp;

				//���������Ĵŵ�������ֵ
				tempIndex = TrackOrderIndex_2[j];
				TrackOrderIndex_2[j] = TrackOrderIndex_2[j + 1];
				TrackOrderIndex_2[j + 1] = tempIndex;

			}//if
		}//for
	}//for
	///////////////////////
	//�ҵ����ڵ�ǰ�ŵ�֮�⣬���Ǿ�������Ĵŵ�������ֵpoint
	int point = 0;
	while (StartTrack >= SortTrackOrder_1[point])
	{
		point++;
	}

	int count = 0;
	int currentTrack = StartTrack;
	if (direction == 0)  //��ŵ����ӵķ������
	{
		cout << "            ��ŵ����ӵķ������" << endl;
		for (i = point;i < TrackNum/2;i++)
		{
			VisitOrder_1[count] = SortTrackOrder_1[i];
			MoveDistance_1[count] = getPositive(VisitOrder_1[count], currentTrack);
			currentTrack = VisitOrder_1[count];
			count++;
		}

		for (i = 0;i < point;i++)
		{
			VisitOrder_1[count] = SortTrackOrder_1[i];
			MoveDistance_1[count] = getPositive(VisitOrder_1[count], currentTrack);
			currentTrack = VisitOrder_1[count];
			count++;
		}
	}//if
	else if (direction == 1)
	{
		cout << "            ��ŵ���С�ķ������" << endl;

		for (i = 0;i < point;i++)
		{
			VisitOrder_1[count] = SortTrackOrder_1[i];
			MoveDistance_1[count] = getPositive(VisitOrder_1[count], currentTrack);
			currentTrack = VisitOrder_1[count];
			count++;
		}

		for (i = point;i < TrackNum/2;i++)
		{
			VisitOrder_1[count] = SortTrackOrder_1[i];
			MoveDistance_1[count] = getPositive(VisitOrder_1[count], currentTrack);
			currentTrack = VisitOrder_1[count];
			count++;
		}
	}//else
		///////////////////////
	//�ҵ����ڵ�ǰ�ŵ�֮�⣬���Ǿ�������Ĵŵ�������ֵpoint
	point = 0;
	while (StartTrack >= SortTrackOrder_2[point])
	{
		point++;
	}

	count = 0;
    currentTrack = StartTrack;
	if (direction == 0)  //��ŵ����ӵķ������
	{
		cout << "            ��ŵ����ӵķ������" << endl;
		for (i = point;i < TrackNum / 2;i++)
		{
			VisitOrder_2[count] = SortTrackOrder_2[i];
			MoveDistance_2[count] = getPositive(VisitOrder_2[count], currentTrack);
			currentTrack = VisitOrder_2[count];
			count++;
		}

		for (i = 0;i < point;i++)
		{
			VisitOrder_2[count] = SortTrackOrder_2[i];
			MoveDistance_2[count] = getPositive(VisitOrder_2[count], currentTrack);
			currentTrack = VisitOrder_2[count];
			count++;
		}
	}//if
	else if (direction == 1)
	{
		cout << "            ��ŵ���С�ķ������" << endl;

		for (i = 0;i < point;i++)
		{
			VisitOrder_2[count] = SortTrackOrder_2[i];
			MoveDistance_2[count] = getPositive(VisitOrder_2[count], currentTrack);
			currentTrack = VisitOrder_2[count];
			count++;
		}

		for (i = point;i < TrackNum / 2;i++)
		{
			VisitOrder_2[count] = SortTrackOrder_2[i];
			MoveDistance_2[count] = getPositive(VisitOrder_2[count], currentTrack);
			currentTrack = VisitOrder_2[count];
			count++;
		}
	}//else
	//�ָ�
	for (i = 0;i < TrackNum / 2;i++)
	{
		MoveDistance[i] = MoveDistance_1[i];
		VisitOrder[i] = VisitOrder_1[i];
	}
	for (i = 0;i < TrackNum / 2;i++)
	{
		MoveDistance[i+TrackNum/2] = MoveDistance_2[i];
		VisitOrder[i + TrackNum / 2] = VisitOrder_2[i];
	}

	for (i = 0;i < TrackNum;i++)
	{
		totalDistance += MoveDistance[i];
	}

	AverageDistance = (totalDistance * 1.0) / TrackNum;

	display();
	cout << "******* ******* ******* ******* *******" << endl;
}



int main()
{
	cout << "******* 19281300-������ �ƶ�ͷ���̵����㷨ģ��ʵ�� *******" << endl;
	RandomTrackSeq();//������ɴŵ���������
	int chooseAlgorithm;
	int IsContinue = 1;
	while (IsContinue)
	{
		cout << "******* �ƶ�ͷ���̵����㷨 *******" << endl;
		cout << "1.�����ȷ�������㷨-FCFS" << endl;
		cout << "2.���Ѱ��ʱ�����ȵ����㷨-SSTF" << endl;
		cout << "3.���ݣ�ɨ�裩�����㷨-SCAN" << endl;
		cout << "4.ѭ��ʽ�������ɨ������㷨-CSCAN *******" << endl;
		cout << "5.˫���е���ɨ������㷨-FSCAN *******" << endl;
		cout << "0.�˳�" << endl;
		cout << "ѡ����ţ�";
		cin >> chooseAlgorithm;

		switch (chooseAlgorithm)
		{
		case 0:
			IsContinue = 0;
			break;
		case 1:
			FCFS();break;
		case 2:
			SSTF();break;
		case 3:
			SCAN();break;
		case 4:
			CSCAN();break;
		case 5:
			FSCAN();break;
		default:
			cout << "ѡ�����" << endl;break;
		}
	}
	return 0;
}