#include "Controller.h"
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>
#define MYDEBUG
#ifdef MYDEBUG
#include <time.h>
#include <Windows.h>
#include <fstream>
#endif

using std::cerr;
using std::endl;

Controller::Controller():workbenchIds(10)
{
	curFrame = 0;
	auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::stringstream ss;
	ss << std::put_time(std::localtime(&t), "%F-%T");
	debugLogFile = "D:/past/F/C++_file/HwPriRobotMSVC/x64/" + ss.str() + ".txt";
}

void Controller::readMap()
{
	char line[1024];
	int wbid = 0;
	for (int i = 0; i < MAP_HEIGHT; i++) {
		fgets(line, sizeof line, stdin);
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (line[j] == 'A') {
				robots.emplace_back(jtox(j), itoy(i));
			}
			else if (line[j] <= '9' && line[j] >= '1') {
				workbenchIds[line[j] - '0'].push_back(wbid);
				workbenchs.emplace_back(wbid, line[j] - '0', jtox(j), itoy(i));
				wbid++;
			}
		}
	}

	cerr << "robots num = " << robots.size() << ", workbenchs num = " << workbenchs.size() << endl;
	readUntilOK();
}

void Controller::initTask()
{
	vector < std::pair<int, vector<int>>> topos = {
		{1, {4, 5, 9}},
		{2, {4, 6, 9}},
		{3, {5, 6, 9}},
		{4, {7, 9}},
		{5, {7, 9}},
		{6, {7, 9}},
		{7, {8, 9}}
	};
	//TODO 三级for就是搞砸了，现在直接4级for
	int task_num = 0;
	for (const auto & p : topos) {
		for (int si : workbenchIds[p.first]) {
			for (int etype : p.second) {
				for (int ei : workbenchIds[etype]) {
					Task* tempt = new Task(&workbenchs[si], &workbenchs[ei]);
					workbenchs[si].buyTasks.push_back(tempt);
					workbenchs[ei].sellTasks.push_back(tempt);
					task_num++;
				}
			}
		}
	}
	
	cerr << "task num = " <<task_num << endl;
}

bool Controller::readFrame()
{
	int frameID;
	int money;
	int wbnums;
	if (scanf("%d %d", &frameID, &money) == EOF) {
		return false;
	}
	scanf("%d", &wbnums);
	//cerr << "frameID = " << frameID << ", time = " << frameID * 0.02 << endl;
	curFrame = frameID;
	//cerr << "workbench nums = " << wbnums << endl;
	for (int i = 0; i < wbnums; i++) {
		workbenchs[i].scanWorkbench();
		//workbenchs[i].printWorkbench();
	}
	for (Robot &r : robots) {
		r.scanRobot();
		//r.printRobot();
	}
	readUntilOK();
	return true;
}

void Controller::writeFrame()
{
	int lineSpeed = 3;
	double angleSpeed = 1.5;
	//vector<std::pair<double, double>> destinate = {
	//	{workbenchs[0].x, workbenchs[0].y}
	//	};
	for (int robotId = 0; robotId < 4; robotId++) {
		//robots[robotId].target = &workbenchs[(curFrame / 1000 + robotId * 10) % workbenchs.size()];
		robots[robotId].goToTarget(lineSpeed, angleSpeed);
		printf("forward %d %d\n", robotId, lineSpeed);
		printf("rotate %d %f\n", robotId, angleSpeed);
	}
	//OK();
}

void Controller::allocate()
{	
	std::fstream fs;
	fs.open("D:\\past\\F\\C++_file\\HwPriRobotMSVC\\x64\\record.txt", std::ios::app | std::ios::out);
	for (int ri = 0; ri < 4; ri++) {
		if (robots[ri].readyForSell()) {
			printf("sell %d\n", ri);
			robots[ri].task->sell();
			robots[ri].finishTask(curFrame, fs);
			//assignTask(robots[ri]);
			Task * t = assignTask(ri);
			allocateTask(robots[ri], t);
		}
		if (robots[ri].readyForBuy()) {
			printf("buy %d\n", ri);
			robots[ri].task->buy();
			robots[ri].target = robots[ri].task->sellWb;
		}
	}
	fs.close();
}

bool Controller::allocateSell(Robot & r, int items)
{
	return false;
}

bool Controller::allocateBuy(Robot & r, int items)
{
	return false;
}

void Controller::perform()
{
	readMap();
	initTask();
	assignIdle();
	OK();
	while (readFrame()) {
		printf("%d\n", curFrame);
		allocate();
		writeFrame();
		OK();
	}
	summary();
}

bool Controller::readUntilOK()
{
	char line[1024];
	while (fgets(line, sizeof line, stdin)) {
		if (line[0] == 'O' && line[1] == 'K') {
			return true;
		}
		//cerr << "error! unhandle input " << line << endl;
	}
	return false;
}

void Controller::debug()
{
	std::fstream of;
	of.open(debugLogFile, std::ios::out | std::ios::app);
	for (int i = 0; i < 4; i++) {
		robots[i].writeDebug(of, curFrame, i);
	}
	of.close();
}

void Controller::allocateTask(Robot & r, Task * task)
{
	cerr << "allocated task " << task->buyWb->id << " -> " << task->sellWb->id << endl;
	task->buyWb->printWorkbench();
	task->sellWb->printWorkbench();
#ifdef MYDEBUG
	//Sleep(10000);
#endif // MYDEBUG
	r.getTask(task, curFrame);
	r.task = task;
	task->buyWb->buyDelegated = true;
	task->sellWb->sellDelegated[task->buyWb->type] = true;
	r.target = task->buyWb;
}

void Controller::assignTask(Robot & r)
{
	//std::vector<std::pair<Task*, int>> priority;
	int start_time = clock();
	Task *t = nullptr;
	int priority = INT_MAX;
	for (int type = 7; type > 0; type--) {
		for (int wi : workbenchIds[type]) {
			for (Task* sellt : workbenchs[wi].buyTasks) {
				int tempp = max(r.distance(sellt->buyWb) * 10, sellt->remainTime()) + sellt->distance * 10;
				if (tempp < priority) {
					priority = tempp;
					t = sellt;
				}
			}
		}
		// 前缀搜索
		if (priority < 10000) {
			break;
		}
	}
	if (t != nullptr) {
		// 前缀搜索
		while (true) {
			Task *temp = findTask(r, t->buyWb->sellTasks);
			if (temp != nullptr) {
				t = temp;
			}
			else {
				break;
			}
		}
		allocateTask(r, t);
		cerr << clock() - start_time << " time to allocate, priority = " << priority <<endl;
		/*cerr << max(r.distance(t->buyWb) * 10, t->remainTime()) <<", " <<t->distance * 10 << ", ";
		cerr << r.distance(t->buyWb) * 10 << ", " << t->remainTime() << endl;*/
		return;
	}
	cerr << clock() - start_time << " time to allocate time " << endl;
}

//简单分配：所有能干的，干其中出价第一的最好的，没有则干最好的
//优先干低级，低级完成能送就顺便送中级
Task* Controller::assignTask(int ri)
{
	int start_time = clock();
	if (robots[ri].task != nullptr) {
		Task *sinorT = findTask(robots[ri], robots[ri].task->sellWb->buyTasks);
		if (sinorT != nullptr) {
			cerr << ri << " find afterward task ";
			sinorT->printTask();
			cerr << endl;
			return sinorT;
		}
	}
	Task *t = nullptr;
	int priority = 10000;
	Task *single_t = nullptr;
	int single_priority = INT_MAX;
	int robotspri[4] = { 0 };
	for (int type = 3; type > 0; type--) {
		for (int wi : workbenchIds[type]) {
			for (Task* sellt : workbenchs[wi].buyTasks) {
				for (int tri = 0; tri < 4; tri++) {
					robotspri[tri] = robots[tri].assessTask(sellt);
				}
				int temp_p = robotspri[ri];
				int max_e = *std::min_element(robotspri, robotspri + 4);
				//同时记录4个机器人中最优的选择 和只考虑一个机器人最优选
				if (max_e == temp_p && temp_p < priority) {
					t = sellt;
					priority = temp_p;
				}
				if (temp_p < single_priority) {
					single_priority = temp_p;
					single_t = sellt;
				}
			}
		}
	}
	if (t != nullptr) {
		cerr << ri << " find all best task ";
		t->printTask();
		cerr << endl;
		cerr << clock() - start_time << " time to allocate, priority = " << priority << endl;
		return t;
	}
	else {
		cerr << ri << " find local best task ";
		single_t->printTask();
		cerr << endl;
		cerr << clock() - start_time << " time to allocate, priority = " << single_priority << endl;
		return single_t;
	}
}

Task * Controller::findTask(Robot & r, vector<Task*>& optionalT)
{
	Task *t = nullptr;
	int priority = 10000;
	for (Task* sellt : optionalT) {
		int tempp = r.assessTask(sellt);
		if (tempp < priority) {
			priority = tempp;
			t = sellt;
		}
	}
	return t;
}

void Controller::assignIdle()
{
	for (Robot &r : robots) {
		if (r.task == nullptr) {
			assignTask(r);
		}
	}
}

void Controller::summary()
{
	cerr << "robot cost total " << Robot::total_frame << " ms go " << Robot::total_distance << " m, ";
	cerr << "average velcoity = " << Robot::total_distance / Robot::total_frame << endl;
}
