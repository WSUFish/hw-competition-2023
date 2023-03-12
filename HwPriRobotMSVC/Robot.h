#pragma once
class Robot
{
	int workbench = -1;
	int item = 0;

	double time_co = 0;
	double coll_co = 0;

	double x;
	double y;
	double x_vel = 0;
	double y_vel = 0;
	double dir = 0;//初始方向为0?
	double ang_vel = 0;

public:
	Robot();
	Robot(double x, double y);
	void setRobot(char *line);
	void scanRobot();
	void printRobot();
};

