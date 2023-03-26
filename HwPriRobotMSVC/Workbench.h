#pragma once
#include <vector>
#include "Task.h"
#include <unordered_map>
using std::unordered_map;
using std::pair;
using std::vector;
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
	bool allSet = false;

	std::vector<Task*> buyTasks;
	std::vector<Task*> sellTasks;

	//这真的有必要吗
	//item - 机器人当前任务 - 按时间升序的<所有该item可接任务 , 时间>
	//vector<unordered_map<Task*, vector<pair<Task*, double>>>> typeTask2Task;

	//item - 机器人当前任务 - 最优对应任务,时间
	vector < unordered_map<Task*, pair<Task*, double>>> typeTask2Task;

	// item - selltask
	vector<vector<Task*>> typeSellTasks;

	const static std::vector<int> periods;

	static std::vector<vector<int>> items_need;

	bool nearEdge;
	double edge_x;
	double edge_y;

public:
	Workbench(int id, int type, double x, double y);
	void scanWorkbench();
	void printWorkbench();

	void sellItem(int item);

	bool isJamed() { return remain_t == 0; };

	//在frame帧后，能否卖item给此工作站
	bool ready(int item, int frame);

	bool readyForBuy(int frame);

	int demand(int item);
};

