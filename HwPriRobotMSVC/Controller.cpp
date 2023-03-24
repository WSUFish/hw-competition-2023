#include "Controller.h"
//#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_set>

#ifdef _DEBUG
#include <time.h>
#include <fstream>
#endif

using std::cerr;
using std::endl;
using std::pair;

template<class T>
struct compareTaskPair {
	bool operator()(const T &p1, const T &p2) const{
		return p1.second < p2.second;
	}
};



Controller::Controller():workbenchIds(10),itemsNum(8)
{
	curFrame = 0;
}

void Controller::readMap()
{
	char line[1024];
	int wbid = 0;
	int rid = 0;
	for (int i = 0; i < MAP_HEIGHT; i++) {
		fgets(line, sizeof line, stdin);
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (line[j] == 'A') {
				robots.emplace_back(rid, jtox(j), itoy(i));
				rid++;
			}
			else if (line[j] <= '9' && line[j] >= '1') {
				workbenchIds[line[j] - '0'].push_back(wbid);
				workbenchs.emplace_back(wbid, line[j] - '0', jtox(j), itoy(i));
				wbid++;
			}
		}
	}
#ifdef _DEBUG
	cerr << "robots num = " << robots.size() << ", workbenchs num = " << workbenchs.size() << endl;
#endif // _DEBUG

	readUntilOK();
}

void Controller::initTask()
{
	
	//TODO 三级for就是搞砸了，现在直接4级for
	int task_num = 0;
	for (const auto & p : topos) {
		for (int si : workbenchIds[p.first]) {
			for (int etype : p.second) {
				for (int ei : workbenchIds[etype]) {
					Task* tempt = new Task(&workbenchs[si], &workbenchs[ei]);
					allTasks.push_back(tempt);
					workbenchs[si].buyTasks.push_back(tempt);
					workbenchs[ei].sellTasks.push_back(tempt);
					workbenchs[ei].typeSellTasks[p.first].push_back(tempt);
					task_num++;
				}
			}
		}
	}
	
	//TODO 改完更是直接5级
	for (Task *t : allTasks) {
		//所有收购点类型
		for (const auto &p : selltopos) {
			//对应的收购工作台
			for (int wi : workbenchIds[p.first]) {
				//收购的物品类型
				for (int item : p.second) {
					//最优任务;
					Task *optTask = nullptr;
					double minTime = 1000000;
					//所有出售该类型的任务
					for (Task *et : workbenchs[wi].typeSellTasks[item]) {
						//计算任务->出售任务所花时间
						double tempTime = t->toDoTime(et);
						if (tempTime < minTime) {
							minTime = tempTime;
							optTask = et;
						}
					}
					if (optTask != nullptr) {
						workbenchs[wi].typeTask2Task[item][t] = {optTask, minTime};
					}
				}
			}
		}
	}
#ifdef _DEBUG
	cerr << "task num = " <<task_num << endl;
#endif // _DEBUG

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
	curFrame = frameID;
	for (int i = 0; i < 8; i++) {
		itemsNum[i] = 0;
	}
	for (int i = 0; i < wbnums; i++) {
		workbenchs[i].scanWorkbench();
		//if (workbenchs[i].pdt_status == 1 || workbenchs[i].remain_t!=-1) {
		//	itemsNum[workbenchs[i].type]++;
		//}
		for (int ii = 1; ii < 8; ii++) {
			if (workbenchs[i].readyForSell[ii] == false || workbenchs[i].sellDelegated[ii] == true) {
				itemsNum[ii]++;
			}
		}
		//workbenchs[i].printWorkbench();
	}
	sum456 = itemsNum[4] + itemsNum[5] + itemsNum[6];
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
		avoidCollision(robotId, lineSpeed, angleSpeed);
		printf("forward %d %d\n", robotId, lineSpeed);
		printf("rotate %d %f\n", robotId, angleSpeed);
	}//TODO 看来应该先存速度，最后一块print
	//OK();
}

void Controller::allocate()
{	
#ifdef _DEBUG
	/*std::fstream fs;
	fs.open("D:\\past\\F\\C++_file\\HwPriRobotMSVC\\x64\\record.txt", std::ios::app | std::ios::out);*/
#endif // _DEBUG
	for (int ri = 0; ri < 4; ri++) {
		if (robots[ri].readyForUpdateTask()) {
			Task * t = allocateMatch(ri);
			allocateTask(robots[ri], t);
		}
		if (robots[ri].readyForSell()) {
			printf("sell %d\n", ri);
			robots[ri].task->sell();

#ifdef _DEBUG
			//robots[ri].finishTask(curFrame, fs);
#endif // _DEBUG
			//assignTask(robots[ri]);
			//Task * t = assignTask(ri);
			Task * t = allocateMatch(ri);
			allocateTask(robots[ri], t);
		}
		if (robots[ri].readyForBuy()) {
			printf("buy %d\n", ri);
			robots[ri].task->buy();
			itemsNum[robots[ri].task->item]++;
			robots[ri].target = robots[ri].task->sellWb;
		}
	}
#ifdef _DEBUG
	//fs.close();
#endif // _DEBUG

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
#ifdef _DEBUG
	summary();
#endif // _DEBUG

}

bool Controller::readUntilOK()
{
	char line[1024];
	while (fgets(line, sizeof line, stdin)) {
		if (line[0] == 'O' && line[1] == 'K') {
			return true;
		}
	}
	return false;
}

void Controller::debug()
{
}

void Controller::allocateTask(Robot & r, Task * task)
{
	//task->buyWb->printWorkbench();
	//task->sellWb->printWorkbench();
	if (r.valid_task == false) {
		r.task = task;
		r.target = task->buyWb;
#ifdef _DEBUG
		cerr << "robot " << r.id << " fake task " << task->buyWb->id << " -> " << task->sellWb->id << endl;
#endif // _DEBUG
		return;
	}
#ifdef _DEBUG
	cerr << "robot " << r.id << " allocated task " << task->buyWb->id << " -> " << task->sellWb->id << endl;
#endif // _DEBUG
	r.getTask(task, curFrame);
	r.task = task;
	task->buyWb->buyDelegated = true;
	task->sellWb->sellDelegated[task->buyWb->type] = true;
	r.target = task->buyWb;
}

void Controller::assignTask(Robot & r)
{
	//std::vector<std::pair<Task*, int>> priority;
#ifdef _DEBUG
	int start_time = clock();
#endif // _DEBUG

	Task *t = nullptr;
	int priority = 100000;
	for (int type = 7; type > 0; type--) {
		for (int wi : workbenchIds[type]) {
			for (Task* sellt : workbenchs[wi].buyTasks) {
				int tempp = std::max((int)r.distance(sellt->buyWb) * 10, sellt->remainTime()) + (int)sellt->distance * 10;
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
#ifdef _DEBUG
		cerr << clock() - start_time << " time to allocate, priority = " << priority <<endl;
#endif // _DEBUG

		/*cerr << max(r.distance(t->buyWb) * 10, t->remainTime()) <<", " <<t->distance * 10 << ", ";
		cerr << r.distance(t->buyWb) * 10 << ", " << t->remainTime() << endl;*/
		return;
	}
#ifdef _DEBUG
	cerr << clock() - start_time << " time to allocate time " << endl;
#endif // _DEBUG

}

//简单分配：所有能干的，干其中出价第一的最好的，没有则干最好的
//优先干低级，低级完成能送就顺便送中级
Task* Controller::assignTask(int ri)
{
	//allocateMatch_distance(ri);
	//allocateMatch(ri);
	robots[ri].valid_task = true;

#ifdef _DEBUG
	int start_time = clock();
#endif // _DEBUG

	if (robots[ri].task != nullptr) {
		Task *sinorT = findTask(robots[ri], robots[ri].task->sellWb->buyTasks);
		if (sinorT != nullptr) {
#ifdef _DEBUG
			cerr << ri << " find afterward task ";
			sinorT->printTask();
			cerr << endl;
#endif // _DEBUG
			return sinorT;
		}
	}
	Task *t = nullptr;
	int priority = 10000;
	Task *single_t = nullptr;
	int single_priority = 100000;
	int robotspri[4] = { 0 };
	vector<int> jam_pri(10, 0);
	int min_buy_item = std::min(itemsNum[4], std::min(itemsNum[5], itemsNum[6]));
#ifdef _DEBUG
	cerr << curFrame << " frame allocate " << ri << endl;
#endif // _DEBUG
	for (int i = 4; i <= 6; i++) {
		if (itemsNum[i] > min_buy_item) {
			jam_pri[i] = 1000;
		}
		//cerr << i << " items num = " << itemsNum[i]<< endl;
	}
	jam_pri[9] = 4000;

	for (int type = 3; type > 0; type--) {
		for (int wi : workbenchIds[type]) {
			for (Task* sellt : workbenchs[wi].buyTasks) {
				for (int tri = 0; tri < 4; tri++) {
					robotspri[tri] = robots[tri].assessTask(sellt) + jam_pri[sellt->sellWb->type];
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
#ifdef _DEBUG
		cerr << ri << " find all best task ";
		t->printTask();
		cerr << endl;
		cerr << clock() - start_time << " time to allocate, priority = " << priority << endl;
#endif // _DEBUG
		return t;
	}

	for (int type = 4; type < 8; type++) {
		for (int wi : workbenchIds[type]) {
			for (Task* sellt : workbenchs[wi].buyTasks) {
				// 闲的没事干的去搞疏通吧
				int temp_p = robots[ri].assessTask(sellt) + (workbenchs[wi].isJamed() ? 0 : 1000);
				if (temp_p < single_priority) {
					single_priority = temp_p;
					single_t = sellt;
				}
			}
		}
	}
#ifdef _DEBUG
	cerr << ri << " find local best task ";
	single_t->printTask();
	cerr << endl;
	cerr << clock() - start_time << " time to allocate, priority = " << single_priority << endl;
#endif // _DEBUG
	if (single_priority > 10000) {
		robots[ri].valid_task = false;
	}
	return single_t;
	
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

void Controller::allocateNet(const vector<pair<int, int>> &wbItems)
{

}

void Controller::allocateMatch_distance(int ri)
{
	//寻找与当前机器人接近的机器人
	vector<int> near_ris;
	for (int iri = 0; iri < 4; iri++) {
		if (ri == iri) {
			continue;
		}
		if (robots[ri].task->distanceSquare(robots[iri].task) < match_range_sq) {
			near_ris.push_back(iri);
		}
	}
	int near_nums = (int)near_ris.size() + 1;
	//根据数量决定分配几个任务
	std::priority_queue<pair<Task*, double>, vector<pair<Task*, double>>, compareTaskPair<pair<Task*, double>>> pq;
	Task *curT = robots[ri].task;
	for (int sti = 0; sti < 3; sti++) {
		for (int wi : workbenchIds[selltopos[sti].first]) {
			for (int item : selltopos[sti].second) {
				//TODO 要更精确吗
				if (!workbenchs[wi].ready(item, 100)) {
					continue;
				}
				double assess_time = workbenchs[wi].typeTask2Task[item][curT].second;
				if (pq.empty() || assess_time < pq.top().second) {
					pq.push(workbenchs[wi].typeTask2Task[item][curT]);
				}
				if (pq.size() > near_nums) {
					pq.pop();
				}
			}
		}
	}
	vector<Task*> alterTasks;
	while (!pq.empty()) {
		alterTasks.push_back(pq.top().first);
		pq.pop();
	}
	cerr << "robot " << ri << " allocate task, there are " << near_nums-1 << " robots around ";
	cerr << "and alternative tasks for robot" << ri << " are: \n";
	for (Task * at : alterTasks) {
		at->printTask();
	}
}

Task* Controller::allocateMatch(int ri)
{
#ifdef _DEBUG
	int start_time = clock();
#endif // _DEBUG

	robots[ri].valid_task = true;

	//来都来了，顺手送了

	//感觉只有图三要考虑是否先干附近能干的活再送走
	//if (robots[ri].task->sellWb->pdt_status == 1 && robots[ri].task->sellWb->allSet)
	if (robots[ri].task->sellWb->pdt_status == 1) {
		int item = robots[ri].task->sellWb->type;
		Task *seniorT = nullptr;
		int minFrame = 1000000;
		for (Task *buyTask : robots[ri].task->sellWb->buyTasks) {
			int frame = (int)robots[ri].task->toDoTime(buyTask) * 50;
			//能7何9？
			int senoirPri = frame + (buyTask->sellWb->type == 9 ? 3000 : 0);
			//优先需求 TODO加入分工考虑？
			senoirPri += 1000 * (3 - buyTask->sellWb->demand(item));
			if (buyTask->sellWb->ready(item, frame) && minFrame > senoirPri) {
				seniorT = buyTask;
				minFrame = senoirPri;
			}
		}
		if (seniorT != nullptr) {
			return seniorT;
		}
	}

	std::priority_queue<pair<Task*, double>, vector<pair<Task*, double>>, compareTaskPair<pair<Task*, double>>> pq;
	std::unordered_set<int> taskType;
	//引入根据场上456数量调整对456的输入
	vector<double> item_prioirty(8);
	for (int i = 4; i <= 6; i++) {
		item_prioirty[i] = itemsNum[i] > (sum456 / 3) ? 200 : 0;
	}
	for (int iri = 0; iri < 4; iri++) {
		Task* curT = robots[iri].task;
		// 456
		for (int sti = 0; sti < 3; sti++) {
			for (int wi : workbenchIds[selltopos[sti].first]) {
				for (int item : selltopos[sti].second) {
					//TODO 要更精确吗
					if (!workbenchs[wi].ready(item, 100)) {
						continue;
					}
					double assess_time = workbenchs[wi].typeTask2Task[item][curT].second + item_prioirty[selltopos[sti].first];
					assess_time += 50 * (3 - workbenchs[wi].demand(item));
					//cerr << wi << "(" << workbenchs[wi].type << ") demand item " << item << " : "<<workbenchs[wi].demand(item) << endl;
					if (pq.size() < 4) {
						pq.emplace(workbenchs[wi].typeTask2Task[item][curT].first, assess_time);
					}
					else if (assess_time < pq.top().second) {
						pq.pop();
						pq.emplace(workbenchs[wi].typeTask2Task[item][curT].first, assess_time);
					}
				}
			}

		}
#ifdef _DEBUG
		cerr << "robot " << iri << " current task : ";
		curT->printTask();
#endif // _DEBUG

		while (!pq.empty()) {

#ifdef _DEBUG
			cerr << "robot " << iri << " best task : " << pq.top().second << " ";
			pq.top().first->printTask();
#endif // _DEBUG

			int tempType = pq.top().first->sellWb->id * 100 + pq.top().first->item;
			taskType.insert(tempType);
			pq.pop();
		}
	}
#ifdef _DEBUG
	cerr << "can allocate " << taskType.size() << " different type task" << endl;
#endif // _DEBUG

	
	vector<vector<int>> timeMatrix(4, vector<int>(taskType.size()));
	vector<Task*> riTasks(taskType.size());
	int tti = 0;
	for (int tempTaskType : taskType) {
		int wi = tempTaskType / 100;
		int item = tempTaskType % 10;
		riTasks[tti] = workbenchs[wi].typeTask2Task[item][robots[ri].task].first;
		for (int iri = 0; iri < 4; iri++) {
			timeMatrix[iri][tti] = (int)(100 * workbenchs[wi].typeTask2Task[item][robots[iri].task].second);
		}
		tti++;
	}
	int taskTypeId = mpm.minimumTaskId(timeMatrix, ri);

#ifdef _DEBUG
	for (int iri = 0; iri < 4; iri++) {
		if (mpm.robotMatchTaskId[iri] >= riTasks.size()) {
			cerr << "robot " << iri << " should spare! ";
			continue;
		}
		cerr << "robot " << iri << " should get task ";
		riTasks[mpm.robotMatchTaskId[iri]]->printTask();
	}
	cerr << "allocate_match cost " << clock() - start_time << " ms " << endl;
#endif // _DEBUG

	if (taskTypeId < riTasks.size()) {
		return riTasks[taskTypeId];
	}

	//空闲处理
	int single_priority = 100000;
	Task *single_t = nullptr;
	for (int type = 4; type < 8; type++) {
		for (int wi : workbenchIds[type]) {
			for (Task* sellt : workbenchs[wi].buyTasks) {
				// 闲的没事干的去搞疏通吧
				int way_frame = 50 * (int)(robots[ri].task->toDoTime(sellt));
				int temp_p = (sellt->buyWb->readyForBuy(50) ? 0 : 10000)
					+ (sellt->sellWb->ready(type, way_frame) ? way_frame : 10000)
					+ (workbenchs[wi].isJamed()? 0 : 1000) + (sellt->sellWb->type==9 ? 900:0);
				if (temp_p < single_priority) {
					single_priority = temp_p;
					single_t = sellt;
				}
			}
		}
	}
	if (single_priority >= 10000) {
		//连个闲的任务都没有啊
		if (riTasks.empty()) {
			//就做假任务
			robots[ri].valid_task = false;
		}
		else {
			return riTasks.front();
		}
	}
#ifdef _DEBUG
	cerr << "spare task assign to robot " << ri << " : priority = " << single_priority << " ";
	single_t->printTask();
#endif // _DEBUG

	return single_t;
}

int Controller::jamValue(Task * t)
{
	if (t->item < 4) {
		int dletanum = itemsNum[t->sellWb->type] - itemsNum[7];
	}
	return 0;
}

int Controller::assessSenoirTask(Robot &r, Task *tarT)
{
	Workbench *wb = tarT->sellWb;
	
	return 0;
}

void Controller::avoidCollision(int ri, int &nv, double &nav)
{
	int avoid_p = 0;
	int last_ap = robots[ri].item;
	int last_ri = 0;
	double dxv = 0;
	double dyv = 0;
	double coll_dir = 0;
	for (int ari = 0; ari < 4; ari++) {
		// 优先级更低 
		if (last_ap > robots[ari].item || (last_ap == robots[ari].item && ari >= ri)) {
			continue;
		}
		if (robots[ri].mayCollision(robots[ari])) {
#ifdef _DEBUG
			//cerr << ri << " may collision with " << ari << endl;
#endif // _DEBUG
			avoid_p++;
			last_ap = robots[ari].item;
			coll_dir = robots[ari].dir;
			dxv = robots[ari].x_vel - robots[ri].x_vel;
			dyv = robots[ari].y_vel - robots[ri].y_vel;
			last_ri = ari;
		}
	}
	if (avoid_p > 0) {
		double rel_dir = atan2(dyv, dxv);
		double bt_dir = atan2(robots[ri].y - robots[last_ri].y, robots[ri].x - robots[last_ri].x);
		double diff_btdir = Robot::dir_minus(rel_dir, bt_dir);
		double temp_dir;
		if (diff_btdir > 0) {
			temp_dir = Robot::dir_minus(rel_dir, 1.57);
			robots[ri].goToDir(temp_dir, nv, nav);
		}
		else {
			temp_dir = Robot::dir_minus(rel_dir, -1.57);
			robots[ri].goToDir(temp_dir, nv, nav);
		}
		double diff_adir = std::abs(Robot::dir_minus(coll_dir, temp_dir));
		if (diff_adir < 0.785) {
			nv = (int)(nv * diff_adir / 1.57);
		}
	}
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
