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
		time+= buyWb->type < 4 ? 50 : 20000;
	}
	if (sellWb->type < 8) {
		if (sellWb->sellDelegated[item] || !sellWb->readyForSell[item]) {
			time+= 10000;
		}
	}
	if (buyWb->pdt_status!=1) {
		//4567还是别等了
		//time += (buyWb->remain_t == -1 ? 20000 : buyWb->remain_t);
		time += (buyWb->type < 4 && buyWb->remain_t != -1 ? buyWb->remain_t : 10000);
	}
	return time;
}

void Task::printTask()
{
	std::cerr << "task " << buyWb->id << " -> " << sellWb->id << " distance = " << distance;
}
