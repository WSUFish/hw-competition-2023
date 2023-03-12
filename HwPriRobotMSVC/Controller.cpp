#include "Controller.h"
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>

using std::cerr;
using std::endl;

Controller::Controller()
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
				workbenchs.emplace_back(wbid++, line[j] - '0', jtox(j), itoy(i));
			}
		}
	}
	cerr << "robots num = " << robots.size() << ", workbenchs num = " << workbenchs.size() << endl;
	readUntilOK();
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
	cerr << "frameID = " << frameID << ", time = " << frameID * 0.02 << endl;
	curFrame = frameID;
	cerr << "workbench nums = " << wbnums << endl;
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
	printf("%d\n", curFrame);
	int lineSpeed = 3;
	double angleSpeed = 1.5;
	//vector<std::pair<double, double>> destinate = {
	//	{workbenchs[0].x, workbenchs[0].y}
	//	};
	for (int robotId = 0; robotId < 4; robotId++) {
		robots[robotId].target = &workbenchs[(curFrame / 1000 + robotId * 10) % workbenchs.size()];
		robots[robotId].goToTarget(lineSpeed, angleSpeed);
		printf("forward %d %d\n", robotId, lineSpeed);
		printf("rotate %d %f\n", robotId, angleSpeed);
	}
	//OK();
}

void Controller::perform()
{
	readMap();
	OK();
	while (readFrame()) {
		writeFrame();
		OK();
	}
}

bool Controller::readUntilOK()
{
	char line[1024];
	while (fgets(line, sizeof line, stdin)) {
		if (line[0] == 'O' && line[1] == 'K') {
			return true;
		}
		cerr << "error! unhandle input " << line << endl;
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
