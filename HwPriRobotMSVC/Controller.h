#pragma once
#include "Robot.h"
#include "Workbench.h"
#include <vector>
#include <iostream>
#include <string>
#include "MathUtils.h"
using std::vector;
class Controller
{
	enum{MAP_WIDTH = 100, MAP_HEIGHT = 100};
	enum{ChokeMode, PipeLineMode, MultiMode, GreedMode};
	vector<Robot> robots;
	vector<Workbench> workbenchs;
	vector<vector<int>> workbenchIds;

	int curFrame;
	int mode;
	bool greed_flag = false;

	vector<int> itemsNum;
	vector<int> itemsInPrepare;
	vector<int> itemsNeeded;
	int sum456;

	vector<Task*> allTasks;

	vector < std::pair<int, vector<int>>> topos = {
		{1, {4, 5, 9}},
		{2, {4, 6, 9}},
		{3, {5, 6, 9}},
		{4, {7, 9}},
		{5, {7, 9}},
		{6, {7, 9}},
		{7, {8, 9}}
	};

	vector < std::pair<int, vector<int>>> selltopos = {
		{4, {1, 2}},
		{5, {1, 3}},
		{6, {2, 3}},
		{7, {4, 5, 6}},
		{8, {7}},
		{9, {1, 2, 3, 4, 5, 6, 7}}
	};

	//两个机器人距离够近就开始考虑指派问题
	const double match_range_sq = 50;
	MinimumPowerMatch mpm;

	Workbench *chocked = nullptr;
	vector<double> item_prioirty;

public:
	Controller();
	void readMap();
	bool readFrame();
	void initTask();

	void initRobotTask();
	
	void writeFrame();
	void perform();

	inline double static itoy(int i) { return (MAP_HEIGHT - i) * 0.5 - 0.25; };
	inline double static jtox(int j) { return j * 0.5 + 0.25; }

	bool readUntilOK();
	inline void OK() { puts("OK"); fflush(stdout); };

	void allocate();
	bool allocateSell(Robot &r, int items);
	bool allocateBuy(Robot &r, int items);
	void debug();

	void allocateTask(Robot &r, Task *task);
	void assignTask(Robot &r);
	//考虑分工合作
	Task* assignTask(int ri);
	// 找能直接执行的任务
	Task* findTask(Robot &r, vector<Task*> &optionalT);

	//指派问题 wbItems大小应为4
	void allocateNet(const vector<pair<int, int>> &wbItems);
	
	void allocateMatch_distance(int ri);

	Task* allocateMatch(int ri);

	int matchPriority(Robot &r, int wi, int item);

	Task *chokeAllocate(int ri);

	int jamValue(Task *t);

	//是否允许卖给9，需要根据场上情况判断
	int assessSenoirTask(Robot &r, Task *tarT);

	void avoidCollision(int ri, int &nv, double &nav);

	void assignIdle();

	void summary();

	bool readyForBuySienor(int ri);

	Task* allocatePipeLine(int ri);

	int pipeLinePriority(Robot &r, int wi, int item);

	Task* allocateGreed(int ri);
};

