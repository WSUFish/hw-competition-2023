#pragma once
#include "Robot.h"
#include "Workbench.h"
#include <vector>
#include <iostream>
using std::vector;
class Controller
{
	enum{MAP_WIDTH = 100, MAP_HEIGHT = 100};
	vector<Robot> robots;
	vector<Workbench> workbenchs;

	int curFrame;

public:
	Controller();
	void readMap();
	bool readFrame();
	
	void writeFrame();
	void perform();

	inline double static itoy(int i) { return (MAP_HEIGHT - i) * 0.5 - 0.25; };
	inline double static jtox(int j) { return j * 0.5 + 0.25; }

	bool readUntilOK();
	inline void OK() { puts("OK"); fflush(stdout); };
};

