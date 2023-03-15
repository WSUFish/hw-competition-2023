#include "Task.h"
#include <cmath>
#include <iostream>

Task::Task(Workbench * buy, Workbench * sell):buyWb(buy),sellWb(sell)
{
	item = buy->type;
	double dx = buyWb->x - sellWb->x;
	double dy = buyWb->y - sellWb->y;
	distance = sqrt(dx * dx + dy * dy);
	dir = atan2(dy, dx);
}

void Task::buy() {
	buyWb->buyDelegated = false;
	buyWb->pdt_status = 0;
	buyWb->remain_t = Workbench::periods[item];
}
void Task::sell() { 
	sellWb->sellDelegated[item] = false; 
	sellWb->sellItem(item);
}

bool Task::valid()
{
	return (!buyWb->buyDelegated) && buyWb->readyForSell && (!sellWb->sellDelegated[item]) && sellWb->readyForSell[item];
}

int Task::remainTime()
{
	//std::cerr << item << ", " << buyWb->buyDelegated << sellWb->sellDelegated[item] << sellWb->readyForSell[item] << buyWb->remain_t << std::endl;
	int time = 0;
	if (buyWb->buyDelegated) {
		return 10000;
	}
	if (sellWb->type < 8) {
		if (sellWb->sellDelegated[item] || !sellWb->readyForSell[item]) {
			return 10000;
		}
	}
	if (buyWb->pdt_status!=1) {
		time += (buyWb->remain_t == -1 ? 10000 : buyWb->remain_t);
	}
	return time;
}
