#pragma once
class Workbench
{
	int type;
	double x;
	double y;
	int remain_t = -1;
	int raw_status = 0; // �Ѵ�ż���ԭ���� λ��ʾ
	int	pdt_status = 0;

public:
	Workbench(int type, double x, double y);
	void scanWorkbench();
	void printWorkbench();
};

