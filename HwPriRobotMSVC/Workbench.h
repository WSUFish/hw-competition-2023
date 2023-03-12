#pragma once
class Workbench
{
	int type;
	double x;
	double y;
	int remain_t = -1;
	int raw_status = 0; // 已存放几个原材料 位表示
	int	pdt_status = 0;

public:
	Workbench(int type, double x, double y);
	void scanWorkbench();
	void printWorkbench();
};

