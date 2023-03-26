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
	int raw_status = 0; // �Ѵ�ż���ԭ���� λ��ʾ
	int	pdt_status = 0;

	int id;

	bool buyDelegated = false;
	bool readyForSell[8];
	bool sellDelegated[8];
	bool allSet = false;

	std::vector<Task*> buyTasks;
	std::vector<Task*> sellTasks;

	//������б�Ҫ��
	//item - �����˵�ǰ���� - ��ʱ�������<���и�item�ɽ����� , ʱ��>
	//vector<unordered_map<Task*, vector<pair<Task*, double>>>> typeTask2Task;

	//item - �����˵�ǰ���� - ���Ŷ�Ӧ����,ʱ��
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

	//��frame֡���ܷ���item���˹���վ
	bool ready(int item, int frame);

	bool readyForBuy(int frame);

	int demand(int item);
};

