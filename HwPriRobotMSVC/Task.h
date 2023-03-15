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

	Task(Workbench *buy, Workbench *sell);
	void buy() ;
	void sell() ;

	bool valid();
	int remainTime();
};

