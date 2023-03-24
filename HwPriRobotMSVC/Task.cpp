#include "Task.h"
#include <cmath>
#include <iostream>

Task::Task(Workbench * buy, Workbench * sell):buyWb(buy),sellWb(sell)
{
	item = buy->type;
	double dx = sellWb->x - buyWb->x;
	double dy = sellWb->y - buyWb->y;
	distance = sqrt(dx * dx + dy * dy);
	dir = atan2(dy, dx);
}

Task::Task(Workbench * start):sellWb(start)
{
	buyWb = nullptr;
	item = 0;
	distance = 0;
	dir = 0;
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
	if (buyWb != nullptr) {
		std::cerr << "task " << buyWb->id <<"("<<buyWb->type<<")"<< " -> " << sellWb->id << "(" << sellWb->type << ")"<< " distance = " << distance << std::endl;
	}
	else {
		std::cerr << "task robot start point -> " << sellWb->id << "(" << sellWb->type << ")" << " distance = " << distance << std::endl;
	}
}

double Task::toDoTime(Task * at)
{
	double dx = at->buyWb->x - sellWb->x;
	double dy = at->buyWb->y - sellWb->y;
	double inter_d = atan2(dy, dx);
	
	double delta_d = differ_dir(dir, inter_d);
	double inter_delta_d = differ_dir(inter_d, at->dir);
	
	double delta_distance = sqrt(dx * dx + dy * dy);

	return delta_d + (at->distance + delta_distance) / 6;
}

double Task::distanceSquare(Task * at)
{
	double dx = sellWb->x - at->sellWb->x;
	double dy = sellWb->y - at->sellWb->y;
	return dx*dx + dy*dy;
}

double Task::differ_dir(double d1, double d2)
{
	double dd = abs(d1 - d2);
	if (dd > 3.14) {
		dd = 6.28 - dd;
	}
	return dd;
}
