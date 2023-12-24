#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

const int MaxNumber = 199;//磁道号最大值
int TrackNum=100;//该实验中的磁道数目为100
int StartTrack;//开始磁道号
int TrackOrder[MaxNumber];//磁道访问事件序列
int VisitOrder[MaxNumber];//磁道访问顺序
bool isVisited[MaxNumber];//是否被访问过
int MoveDistance[MaxNumber];//磁道到开始磁道距离
int totalDistance;//总距离
double AverageDistance;
bool direction;

//展示
void display()
{
	cout << setw(16) << "从" << StartTrack << "号磁道开始" << setw(4) << endl;
	cout << setw(2) << "被访问的磁道号顺序   " << setw(6) << "   移到距离（磁道数）" << setw(4) << endl;
	for (int i = 0;i < TrackNum;i++)
	{
		cout << setw(10) << VisitOrder[i] << setw(24) << MoveDistance[i] << setw(8) << endl;
	}
	cout << "平均寻道数：" << AverageDistance << endl;
}

//随机生成磁道访问序列
void RandomTrackSeq()
{
	cout << "******* 随机生成磁道访问事件序列 *******" << endl;
	srand(time(NULL)); //根据当前时间设置“随机数种子”
	for (int i = 0;i < TrackNum;i++)
	{
		TrackOrder[i]= (rand() % (TrackNum + 1)) ;   //随机生成磁道访问序列
	}
	StartTrack= (rand() % (TrackNum + 1));  //随机生成开始磁道号
	cout << "磁道个数：" << endl;
	cout << TrackNum << endl;
	cout << "磁道访问序列：" << endl;
	for (int i = 0;i < TrackNum;i++)
	{
		cout << TrackOrder[i] << " ";
	}
	cout << endl;
	cout << "开始磁道号："<< endl;
	cout << StartTrack << endl;
	cout << "******* ******* ******* ******* *******" << endl;
}

//计算两个磁道之间的距离
int getPositive(int num1, int num2)
{
	if (num1 - num2 > 0)
	{
		return (num1 - num2);
	}
	else
		return (num2 - num1);
}

//初始化
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

//先来先服务调度算法
void FCFS()
{
	cout << "******* 先来先服务调度算法 *******" << endl;
	initial();//初始化

	MoveDistance[0] = getPositive(TrackOrder[0], StartTrack);
	totalDistance = MoveDistance[0];
	VisitOrder[0] = TrackOrder[0];

	for (int i = 1;i < TrackNum;i++)
	{
		//按TrackOrder的顺序访问
		MoveDistance[i] = getPositive(TrackOrder[i], TrackOrder[i - 1]);
		totalDistance += MoveDistance[i];
		VisitOrder[i] = TrackOrder[i];
	}
	AverageDistance = totalDistance * 1.0 / TrackNum;
	display();
	cout << "******* ******* ******* ******* *******" << endl;
}

//最短寻道时间优先调度算法
void SSTF()
{
	cout << "******* 最短寻道时间调度算法 *******" << endl;
	initial();//初始化

	int CurrentTrack = StartTrack;//初始化当前磁道号
	int pointMin;//指向磁道距离最小的磁道位置
	int disTemp[MaxNumber];//当前磁道到各磁道之间的距离

	for (int i = 0;i < TrackNum;i++)
	{
		for (int j = 0;j < TrackNum;j++)
		{
			//每次访问前更新当前磁道到各磁道之间的距离
			if (!isVisited[j])//若该磁道还未被访问
			{
				disTemp[j] = getPositive(TrackOrder[j], CurrentTrack);
			}
			else
				disTemp[j] = 10000;  //表示无穷远（访问过的磁道就不再访问）
		}

		pointMin = 0;
		for (int j = 0;j < TrackNum;j++)
		{
			if (disTemp[pointMin] > disTemp[j])
			{
				pointMin = j;   //指向最小的位置
			}
		}
		VisitOrder[i] = TrackOrder[pointMin];  //给访问序列赋值
		MoveDistance[i] = getPositive(TrackOrder[pointMin], CurrentTrack);  //计算每次的移动距离
		totalDistance += MoveDistance[i];   //累计移动距离
		CurrentTrack = TrackOrder[pointMin];   //改变当前的磁道号
		isVisited[pointMin] = true;  //将当前的磁道号设置为已访问

	}

	AverageDistance = totalDistance * 1.0 / (TrackNum);
	display();
	cout << "******* ******* ******* ******* *******" << endl;
}

//电梯调度算法
void SCAN()
{
	cout << "******* 电梯（扫描）调度算法 *******" << endl;
	cout << "请选择头移动方向：" << endl;
	cout << "0.向磁道增加的方向访问" << endl;
	cout << "1.向磁道减小的方向访问" << endl;
	cout << "选择：";
	cin >> direction;

	initial();//初始化

	int TrackOrderIndex[MaxNumber];//排序后的磁道号索引值
	int SortTrackOrder[MaxNumber];//排序后的磁道号
	int i, j;
	int temp, tempIndex;//排序中间值

	//初始化
	for (i = 0;i < TrackNum;i++)
	{
		TrackOrderIndex[i] = i;
		SortTrackOrder[i] = TrackOrder[i];
	}

	//排序后的磁道号以及对应的原来的索引值
	for (i = TrackNum - 1;i > 0;i--)
	{
		for (j = 0;j < i;j++)
		{
			if (SortTrackOrder[j] >= SortTrackOrder[j + 1])
			{
				//更新排序后的磁道号
				temp = SortTrackOrder[j];
				SortTrackOrder[j] = SortTrackOrder[j + 1];
				SortTrackOrder[j + 1] = temp;

				//更新排序后的磁道号索引值
				tempIndex = TrackOrderIndex[j];
				TrackOrderIndex[j] = TrackOrderIndex[j + 1];
				TrackOrderIndex[j + 1] = tempIndex;

			}//if
		}//for
	}//for 

	//找到既在当前磁道之外，又是距离最近的磁道号索引值point
	int point = 0;
	while (StartTrack >= SortTrackOrder[point])
	{
		point++;
	}  

	int count = 0;
	int currentTrack = StartTrack;

	//向磁道增加的方向访问
	if (direction == 0)
	{
		cout << "            向磁道增加的方向访问" << endl;
		//增加方向
		for (i = point;i < TrackNum;i++)
		{
			VisitOrder[count] = SortTrackOrder[i];
			MoveDistance[count] = getPositive(VisitOrder[count], currentTrack);
			currentTrack = VisitOrder[count];
			count++;
		}
		//减少方向
		for (i = point - 1;i >= 0;i--)
		{
			VisitOrder[count] = SortTrackOrder[i];
			MoveDistance[count] = getPositive(VisitOrder[count], currentTrack);
			currentTrack = VisitOrder[count];
			count++;
		}
	}//if

	//向磁道减小的方向访问
	else if (direction == 1)
	{
		cout << "            向磁道减小的方向访问" << endl;
		//减小方向
		for (i = point - 1;i >= 0;i--)
		{
			VisitOrder[count] = SortTrackOrder[i];
			MoveDistance[count] = getPositive(VisitOrder[count], currentTrack);
			currentTrack = VisitOrder[count];
			count++;
		}
		//增加方向
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

//循环式单向电梯调度算法
void CSCAN()
{
	cout << "******* 循环扫描调度算法 *******" << endl;
	cout << "请选择头移动方向：" << endl;
	cout << "0.向磁道增加的方向访问" << endl;
	cout << "1.向磁道减小的方向访问" << endl;
	cout << "选择：";
	cin >> direction;

	initial();
	int TrackOrderIndex[MaxNumber];//排序后的磁道号索引值
	int SortTrackOrder[MaxNumber];//排序后的磁道号
	int i, j;
	int temp, tempIndex;
	for (i = 0;i < TrackNum;i++)
	{
		TrackOrderIndex[i] = i;
		SortTrackOrder[i] = TrackOrder[i];
	}

	//获得排序后的磁道号以及对应的原来的索引值
	for (i = TrackNum - 1;i > 0;i--)
	{
		for (j = 0;j < i;j++)
		{
			if (SortTrackOrder[j] >= SortTrackOrder[j + 1])
			{
				//更新排序后的磁道号
				temp = SortTrackOrder[j];
				SortTrackOrder[j] = SortTrackOrder[j + 1];
				SortTrackOrder[j + 1] = temp;

				//更新排序后的磁道号索引值
				tempIndex = TrackOrderIndex[j];
				TrackOrderIndex[j] = TrackOrderIndex[j + 1];
				TrackOrderIndex[j + 1] = tempIndex;

			}//if
		}//for
	}//for

	//找到既在当前磁道之外，又是距离最近的磁道号索引值point
	int point = 0;
	while (StartTrack >= SortTrackOrder[point])
	{
		point++;
	}

	int count = 0;
	int currentTrack = StartTrack;
	if (direction == 0)  //向磁道增加的方向访问
	{
		cout << "            向磁道增加的方向访问" << endl;
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
		cout << "            向磁道减小的方向访问" << endl;

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

//双队列电梯扫描调度算法
void FSCAN()
{
	cout << "******* 双队列电梯扫描调度算法 *******" << endl;
	cout << "请选择头移动方向：" << endl;
	cout << "0.向磁道增加的方向访问" << endl;
	cout << "1.向磁道减小的方向访问" << endl;
	cout << "选择：";
	cin >> direction;

	int TrackOrder_1[MaxNumber/2];//队列1
	int TrackOrder_2[MaxNumber/2];//队列2
	for (int i = 0;i < TrackNum / 2;i++) {
		TrackOrder_1[i] = TrackOrder[i];
	}
	for (int i = 0;i < TrackNum / 2;i++) {
		TrackOrder_2[i] = TrackOrder[i+ TrackNum / 2];
	}
	int VisitOrder_1[MaxNumber/2], VisitOrder_2[MaxNumber / 2];//磁道访问顺序
	bool isVisited_1[MaxNumber/2], isVisited_2[MaxNumber / 2];//是否被访问过
	int MoveDistance_1[MaxNumber/2], MoveDistance_2[MaxNumber / 2];//磁道到开始磁道距离
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
	//将原队列分为两部分

	int TrackOrderIndex_1[MaxNumber], TrackOrderIndex_2[MaxNumber];//排序后的磁道号索引值
	int SortTrackOrder_1[MaxNumber], SortTrackOrder_2[MaxNumber];//排序后的磁道号
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

	//获得排序后的磁道号以及对应的原来的索引值
	for (i = TrackNum/2 - 1;i > 0;i--)
	{
		for (j = 0;j < i;j++)
		{
			if (SortTrackOrder_1[j] >= SortTrackOrder_1[j + 1])
			{
				//更新排序后的磁道号
				temp = SortTrackOrder_1[j];
				SortTrackOrder_1[j] = SortTrackOrder_1[j + 1];
				SortTrackOrder_1[j + 1] = temp;

				//更新排序后的磁道号索引值
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
				//更新排序后的磁道号
				temp = SortTrackOrder_2[j];
				SortTrackOrder_2[j] = SortTrackOrder_2[j + 1];
				SortTrackOrder_2[j + 1] = temp;

				//更新排序后的磁道号索引值
				tempIndex = TrackOrderIndex_2[j];
				TrackOrderIndex_2[j] = TrackOrderIndex_2[j + 1];
				TrackOrderIndex_2[j + 1] = tempIndex;

			}//if
		}//for
	}//for
	///////////////////////
	//找到既在当前磁道之外，又是距离最近的磁道号索引值point
	int point = 0;
	while (StartTrack >= SortTrackOrder_1[point])
	{
		point++;
	}

	int count = 0;
	int currentTrack = StartTrack;
	if (direction == 0)  //向磁道增加的方向访问
	{
		cout << "            向磁道增加的方向访问" << endl;
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
		cout << "            向磁道减小的方向访问" << endl;

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
	//找到既在当前磁道之外，又是距离最近的磁道号索引值point
	point = 0;
	while (StartTrack >= SortTrackOrder_2[point])
	{
		point++;
	}

	count = 0;
    currentTrack = StartTrack;
	if (direction == 0)  //向磁道增加的方向访问
	{
		cout << "            向磁道增加的方向访问" << endl;
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
		cout << "            向磁道减小的方向访问" << endl;

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
	//恢复
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
	cout << "******* 19281300-赵淑婷 移动头磁盘调度算法模拟实验 *******" << endl;
	RandomTrackSeq();//随机生成磁道访问序列
	int chooseAlgorithm;
	int IsContinue = 1;
	while (IsContinue)
	{
		cout << "******* 移动头磁盘调度算法 *******" << endl;
		cout << "1.先来先服务调度算法-FCFS" << endl;
		cout << "2.最短寻道时间优先调度算法-SSTF" << endl;
		cout << "3.电梯（扫描）调度算法-SCAN" << endl;
		cout << "4.循环式单向电梯扫描调度算法-CSCAN *******" << endl;
		cout << "5.双队列电梯扫描调度算法-FSCAN *******" << endl;
		cout << "0.退出" << endl;
		cout << "选择序号：";
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
			cout << "选择错误！" << endl;break;
		}
	}
	return 0;
}