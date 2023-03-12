#include "Workbench.h"
#include <iostream>

Workbench::Workbench(int id, int type, double x, double y):
	id(id), type(type), x(x), y(y)
{
}

void Workbench::scanWorkbench()
{
	scanf("%d %lf %lf %d %d %d", &type, &x, &y, &remain_t, &raw_status, &pdt_status);
}

void Workbench::printWorkbench()
{
	std::cerr << type << ", x = " << x << 
		", y = " << y << ", remain time = " << remain_t << 
		", raw status = " << raw_status << 
		", product status = " << pdt_status << std::endl;
}
