#pragma once
#include "Robot.h"
#include "Workbench.h"
#include <vector>
#include <iostream>
#include <string>
using std::vector;
class Controller
{
	enum{MAP_WIDTH = 100, MAP_HEIGHT = 100};
	vector<Robot> robots;
	vector<Workbench> workbenchs;
	vector<vector<int>> workbenchIds;

	int curFrame;
	std::string debugLogFile;

public:
	Controller();
	void readMap();
	bool readFrame();
	void initTask();
	
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

	void assignIdle();
};

