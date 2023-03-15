#include "Robot.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#define MY_PI 3.14159
using namespace std;

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
	if (expect_ang > 0) {
		nav = MY_PI;
	}
	else {
		nav = -MY_PI;
	}
	nv = 6;
}

void Robot::goToTarget(int & nv, double & nav)
{
	if (target != nullptr) {
		goTo_greed(target->x, target->y, nv, nav);
	}
}

bool Robot::arrive()
{
	return target != nullptr && target->id == workbench;
}

bool Robot::readyForBuy()
{
	if (workbench != -1) {
		cerr << "can buy ? ";
		//printRobot();
		std::cerr << "robot target " << task->buyWb->id << " on " << workbench << endl;
		//std::cerr << "target pos = " << target->x << ", " << target->y << ", on pos = " << x << ", " << y << endl;
		task->buyWb->printWorkbench();
		//task->sellWb->printWorkbench();
	}
	return task->buyWb->id == workbench && task->buyWb->pdt_status == 1;
}

bool Robot::readyForSell()
{
	if (workbench != -1) {
		cerr << "can sell ? ";
		//printRobot();
		std::cerr << task->sellWb->readyForSell[item] << endl;
		std::cerr << "robot target " << task->sellWb->id << " on " << workbench << endl;
		//std::cerr << "target pos = " << target->x << ", " << target->y << ", on pos = " << x << ", " << y << endl;
		//task->buyWb->printWorkbench();
		task->sellWb->printWorkbench();
	}
	return item != 0 && task->sellWb->id == workbench && task->sellWb->readyForSell[item];
}

int Robot::assessTask(Task * t)
{
	double dx = x - t->buyWb->x;
	double dy = y - t->sellWb->y;
	return (int)(sqrt(dx * dx + dy * dy) + t->distance);
}

