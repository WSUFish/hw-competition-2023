#include "Controller.h"
using std::cerr;
using std::endl;

Controller::Controller()
{
	curFrame = 0;
}

void Controller::readMap()
{
	char line[1024];
	for (int i = 0; i < MAP_HEIGHT; i++) {
		fgets(line, sizeof line, stdin);
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (line[j] == 'A') {
				robots.emplace_back(jtox(j), itoy(i));
			}
			else if (line[j] <= '9' && line[j] >= '1') {
				workbenchs.emplace_back(line[j] - '0', jtox(j), itoy(i));
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
		workbenchs[i].printWorkbench();
	}
	for (Robot &r : robots) {
		r.scanRobot();
		r.printRobot();
	}
	readUntilOK();
	return true;
}

void Controller::writeFrame()
{
	printf("%d\n", curFrame);
	int lineSpeed = 3;
	double angleSpeed = 1.5;
	for (int robotId = 0; robotId < 4; robotId++) {
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