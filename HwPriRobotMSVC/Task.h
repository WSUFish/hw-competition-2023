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
	void buy() ;
	void sell() ;

	bool valid();
	int remainTime();

	void printTask();

	//估计此时去完成任务at所需的时间(转向+直行)
	double toDoTime(Task *at);

	double distanceSquare(Task *at);
};

