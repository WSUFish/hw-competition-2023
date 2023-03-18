#include "Robot.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <time.h>

#define MY_PI 3.14159
using namespace std;

int Robot::total_frame = 0;
double Robot::total_distance = 0;

Robot::Robot()
{
	x = 0;
	y = 0;
}

Robot::Robot(double x, double y):x(x), y(y)
{
}

void Robot::setRobot(char * line)
{
	char *temp = line;
	workbench = strtol(temp, &temp, 10);
	temp++;
	item = strtol(temp, &temp, 10);
	temp++;
	time_co = strtod(temp, &temp);
	temp++;
	coll_co = strtod(temp, &temp);
	temp++;
	ang_vel = strtod(temp, &temp);
	temp++;
	x_vel = strtod(temp, &temp);
	temp++;
	y_vel = strtod(temp, &temp);
	temp++;
	dir = strtod(temp, &temp);
	temp++;
	x = strtod(temp, &temp);
	temp++;
	y = strtod(temp, &temp);
}

void Robot::scanRobot()
{
	double oxv = x_vel, oyv = y_vel, odir = dir, oav = ang_vel, ox = x, oy = y;
	scanf("%d %d %lf %lf %lf %lf %lf %lf %lf %lf", &workbench, &item, &time_co,
		&coll_co, &ang_vel, &x_vel, &y_vel, &dir, &x, &y); 
	/*cerr << "delta xvel = " << x_vel - oxv << ", yvel = " << y_vel - oyv <<
		", dir = " << dir - odir << ", ang_vel = " << ang_vel - oav <<
		", x = " << x - ox << ", y = " << y - oy << endl;*/
}

void Robot::printRobot()
{
	std::cerr << "at workbench " << workbench << ", item = " << item <<
		", time_co = " << time_co << ", coll_co = " << coll_co <<
		", ang_vel = " << ang_vel << ", x_vel = " << x_vel <<
		", y_vel = " << y_vel << ", dir = " << dir << ", x = " << x <<
		", y = " << y << std::endl;
}

void Robot::writeDebug(std::fstream &f, int frameID, int id)
{
	vector<string> ss = {
		to_string(frameID), to_string(id), to_string(workbench),
		to_string(item), to_string(time_co), to_string(coll_co),
		to_string(ang_vel), to_string(x_vel), to_string(y_vel),
		to_string(dir), to_string(x) , to_string(y)
	};
	string s;
	for (int i = 0; i < ss.size(); i++) {
		if (i != 0) {
			s += ", ";
		}
		s += ss[i];
	}
	f << s << endl;
}

void Robot::goTo_greed(double nx, double ny, int & nv, double & nav)
{
	double deltax = nx - x;
	double deltay = ny - y;
	
	double expect_ang = atan2(deltay, deltax) - dir;
	if (expect_ang < -MY_PI) {
		expect_ang += (MY_PI * 2);
	}
	else if (expect_ang > MY_PI) {
		expect_ang -= (MY_PI * 2);
	}
	//cerr << dir << " -> " << expect_ang << " : " << x << ", " << y << " -> " << nx << ", " << ny << endl;
	if (item == 0) {
		if (expect_ang > 0) {
			if (expect_ang > ang_bump) {
				nav = MY_PI;
			}
			else {
				nav = 0.1;
			}
			if (expect_ang < 1) {
				nv = 6;
			}
			else {
				nv = -2;
			}
		}
		else {
			if (expect_ang < -ang_bump) {
				nav = -MY_PI;
			}
			else {
				nav = -0.1;
			}
			if (expect_ang > -1) {
				nv = 6;
			}
			else {
				nv = -2;
			}
		}
	}
	else {
		if (expect_ang > 0) {
			if (expect_ang > 0.32) {
				nav = MY_PI;
			}
			else {
				nav = 0.1;
			}
			if (expect_ang < 0.8) {
				nv = 6;
			}
			else {
				nv = -2;
			}
		}
		else {
			if (expect_ang < -0.32) {
				nav = -MY_PI;
			}
			else {
				nav = -0.1;
			}
			if (expect_ang > -0.8) {
				nv = 6;
			}
			else {
				nv = -2;
			}
		}
	}
	
	//nv = 6;
}

void Robot::goToTarget(int & nv, double & nav)
{
	if (target != nullptr) {
		goTo_greed(target->x, target->y, nv, nav);
	}
	avoidEdge(nv, nav);
}

void Robot::avoidEdge(int & nv, double & nav)
{
	double stop_frames = item == 0 ? 0.32 : 0.44;
	double stop_distance = item == 0 ? 0.45 : 0.53;
	double exp_x = x + x_vel * stop_frames;
	double exp_y = y + y_vel * stop_frames;
	if (exp_x < stop_distance || exp_x > 50 - stop_distance || exp_y < stop_distance || exp_y > 50 - stop_distance) {
		nv = 0;
	}
}

bool Robot::arrive()
{
	return target != nullptr && target->id == workbench;
}

bool Robot::readyForBuy()
{
	if (workbench != -1) {
		//cerr << "can buy ? ";
		//printRobot();
		//std::cerr << "robot target " << task->buyWb->id << " on " << workbench << endl;
		//std::cerr << "target pos = " << target->x << ", " << target->y << ", on pos = " << x << ", " << y << endl;
		//task->buyWb->printWorkbench();
		//task->sellWb->printWorkbench();
	}
	return item==0 && task->buyWb->id == workbench && task->buyWb->pdt_status == 1;
}

bool Robot::readyForSell()
{
	if (workbench != -1) {
		//cerr << "can sell ? ";
		//printRobot();
		//std::cerr << task->sellWb->readyForSell[item] << endl;
		//std::cerr << "robot target " << task->sellWb->id << " on " << workbench << endl;
		//std::cerr << "target pos = " << target->x << ", " << target->y << ", on pos = " << x << ", " << y << endl;
		//task->buyWb->printWorkbench();
		//task->sellWb->printWorkbench();
	}
	return item != 0 && task->sellWb->id == workbench && task->sellWb->readyForSell[item];
}

int Robot::assessTask(Task * t)
{
	double dis = task == nullptr ? distance(t->buyWb) : distance(task->sellWb->x, task->sellWb->y, t->sellWb->x, t->sellWb->y);
	return max((int)dis * 10, t->remainTime()) + (int)t->distance * 10;
}

void Robot::getTask(Task * t, int curFrame)
{
	//task = t;
	start_x = x;
	start_y = y;
	start_dir = dir;
	start_xv = x_vel;
	start_yv = y_vel;
	start_time = curFrame;
}

void Robot::finishTask(int curFrame, std::fstream &fs)
{
	double s_distance = distance(start_x, start_y, task->buyWb->x, task->buyWb->y);
	total_distance += (s_distance + task->distance);
	total_frame += (curFrame - start_time);
	fs << " Task Frame:" << curFrame - start_time << " frame ";
	fs << " distance " << s_distance << " + " << task->distance << " = " << s_distance + task->distance;
	fs << " dir change " << start_dir << " -> "  << atan2(start_y - task->buyWb->y, start_x - task->buyWb->x)<< " -> "<< task->dir;
	fs << " average v = " << (s_distance + task->distance) / (curFrame - start_time) << endl;
}

double Robot::distance(double x1, double y1, double x2, double y2)
{
	double dx = x1 - x2;
	double dy = y1 - y2;
	return sqrt(dx * dx + dy * dy);
}

double Robot::distance(double x1, double y1)
{
	double dx = x1 - x;
	double dy = y1 - y;
	return sqrt(dx * dx + dy * dy);
}


