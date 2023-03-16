#pragma once
#include <iostream>
#include <fstream>
#include "Workbench.h"

class Robot
{
public:
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

	Workbench *target = nullptr;
	Task *task = nullptr;

	int start_time;
	double start_x;
	double start_y;
	double start_dir;
	double start_xv;
	double start_yv;

	static int total_frame;
	static double total_distance;

	const double ang_bump = 0.15;

public:
	Robot();
	Robot(double x, double y);
	void setRobot(char *line);
	void scanRobot();
	void printRobot();

	void writeDebug(std::fstream &f, int frameID, int id);

	void goTo_greed(double nx, double ny, int &nv, double &nav);
	void goToTarget(int &nv, double &nav);

	bool arrive();
	bool readyForBuy();
	bool readyForSell();

	int assessTask(Task *t);

	void getTask(Task *t, int curFrame); // 只起记录作用
	void finishTask(int curFrame, std::fstream &fs); // fstream 完全不起作用

	static double distance(double x1, double y1, double x2, double y2);
};

