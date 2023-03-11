#include "Robot.h"
#include <cstdlib>
#include <iostream>
using namespace std;

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

void Robot::readRobot()
{
	scanf("%d %d %lf %lf %lf %lf %lf %lf %lf %lf", &workbench, &item, &time_co,
		&coll_co, &ang_vel, &x_vel, &y_vel, &dir, &x, &y);
}

