#include "Workbench.h"
#include <iostream>

const std::vector<int> Workbench::periods = {-1, 50, 50, 50, 500, 500, 500, 1000, 1, 1};
std::vector<int> Workbench::items_need(8, 0);

Workbench::Workbench(int id, int type, double x, double y):
	id(id), type(type), x(x), y(y), typeTask2Task(8), typeSellTasks(8)
{
	if (type <= 3) {
		remain_t = 50;
	}
	else {
		remain_t = -1;
	}

	for (int i = 0; i < 8; i++) {
		readyForSell[i] = true;
		sellDelegated[i] = false;
	}
}

void Workbench::scanWorkbench()
{
	int old_raw = raw_status;
	scanf("%d %lf %lf %d %d %d", &type, &x, &y, &remain_t, &raw_status, &pdt_status);
	if (old_raw != raw_status) {
		switch (type)
		{
		case 4:
			readyForSell[1] = !(raw_status & (1 << 1));
			readyForSell[2] = !(raw_status & (1 << 2));
			allSet = !readyForSell[1] && !readyForSell[2];
			break;
		case 5:
			readyForSell[1] = !(raw_status & (1 << 1));
			readyForSell[3] = !(raw_status & (1 << 3));
			allSet = !readyForSell[1] && !readyForSell[3];
			break;
		case 6:
			readyForSell[2] = !(raw_status & (1 << 2));
			readyForSell[3] = !(raw_status & (1 << 3));
			allSet = !readyForSell[2] && !readyForSell[3];
			break;
		case 7:
			readyForSell[4] = !(raw_status & (1 << 4));
			readyForSell[5] = !(raw_status & (1 << 5));
			readyForSell[6] = !(raw_status & (1 << 6));
			allSet = !readyForSell[4] && !readyForSell[5] && !readyForSell[6];
			break;
		default:
#ifdef _DEBUG
			std::cerr << " wrong tyep change raw_status ??? " << type << std::endl;
#endif // _DEBUG

			break;
		}
	}
}

void Workbench::printWorkbench()
{
	std::cerr <<"id = " << id << ", type = " << type << ", x = " << x << 
		", y = " << y << ", remain time = " << remain_t << 
		", raw status = " << raw_status << 
		", product status = " << pdt_status << std::endl;
	
}

void Workbench::sellItem(int item)
{
	if (type >= 8) {
		return;
	}
	readyForSell[item] = false;
	raw_status |= (1 << item);
	switch (type)
	{
	case 4:
		allSet = !readyForSell[1] && !readyForSell[2];
		break;
	case 5:
		allSet = !readyForSell[1] && !readyForSell[3];
		break;
	case 6:
		allSet = !readyForSell[2] && !readyForSell[3];
		break;
	case 7:
		allSet = !readyForSell[4] && !readyForSell[5] && !readyForSell[6];
		break;
	}
}

bool Workbench::ready(int item, int frame)
{
	if (type >= 8) {
		return true;
	}
	return !sellDelegated[item] && (readyForSell[item] || (allSet && remain_t!=-1 && pdt_status == 0 && frame > remain_t));
}
