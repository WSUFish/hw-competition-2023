#pragma once
#include <vector>
#include "Task.h"
class Task;
class Workbench
{
public:
	int type;
	double x;
	double y;
	int remain_t;
	int raw_status = 0; // 已存放几个原材料 位表示
	int	pdt_status = 0;

	int id;

	bool buyDelegated = false;
	bool readyForSell[8];
	bool sellDelegated[8];

	std::vector<Task*> buyTasks;
	std::vector<Task*> sellTasks;

	const static std::vector<int> periods;

public:
	Workbench(int id, int type, double x, double y);
	void scanWorkbench();
	void printWorkbench();

	void sellItem(int item);
};

