#pragma once
#include "Workbench.h"
class Workbench;
class Task
{
public:
	Workbench* buyWb;
	Workbench* sellWb;
	int item;
	double distance;
	double dir;

	int priority = 0;

	Task(Workbench *buy, Workbench *sell);
	
	//Ϊ��ʼ��׼����
	Task(Workbench *start);
	void buy() ;
	void sell() ;

	bool valid();
	int remainTime();

	void printTask();

	//���ƴ�ʱȥ�������at�����ʱ��(ת��+ֱ��)
	double toDoTime(Task *at);

	double distanceSquare(Task *at);

	static double differ_dir(double d1, double d2);
};

